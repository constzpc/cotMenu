/**
  **********************************************************************************************************************
  * @file    oled.c
  * @brief   该文件提供OLED驱动相关功能
  * @author  周鹏程    any question please send mail to const_zpc@163.com
  * @version V1.2.0
  * @date    2021-4-24
  *
  * @details  功能详细说明：
  *           + OLED 初始化函数
  *           + OLED 控制函数
  *           + 画点/读点函数
  *           + OLED 清屏函数
  *           + 画面操作函数
  *           + 画图形函数
  *           + 设置文本/数字函数
  *           + 格式化输出函数
  *
  **********************************************************************************************************************
  * 源码路径：https://gitee.com/const-zpc/STM32_OLED.git 具体问题及建议可在该网址填写 Issue
  *
  * 使用方式:
  *    1、使用前初始化函数 OLED_Init, 同时可以使用 OLED_Clear 立即清屏
  *    2、周期调用函数 OLED_ShowTask, 用来同步画面内容(即缓冲同步到 OLED 上显示)
  *    3、可定义宏定义 "_USE_OLED_PRINTF" 使能OLED 的式样化输出功能, 即类似 printf 的功能
  *
  **********************************************************************************************************************
  */

/* Includes ----------------------------------------------------------------------------------------------------------*/
#include "oled.h"
#include "oledio.h"
#include <string.h>

#ifdef _USE_OLED_PRINTF
#include <stdio.h>
#include <stdarg.h>
#endif // _USE_OLED_PRINTF

/* Private typedef ---------------------------------------------------------------------------------------------------*/

/**
  * @brief OLED 重要参数集结构体定义
  */
typedef struct  tag_OLedDev
{
    oledsize_t width;			    /*!< OLED 宽度 */

    oledsize_t height;	            /*!< OLED 高度 */

    eOledcolor backColor;           /*!< 背景色 */

    eOledcolor pointColor;          /*!< 画笔色 */
} OLedDev_t;

#ifdef _USE_OLED_PRINTF

/**
  * @brief OLED Printf 输出相关定义
  */
typedef struct  tag_OLedPrint
{
    oledsize_t printX;              /*!< 屏幕输出纵坐标起点 */

    oledsize_t printY;              /*!< 屏幕输出横坐标起点 */

    oledsize_t printWidth;          /*!< 屏幕输出宽度 */

    oledsize_t printHeight;         /*!< 屏幕输出高度 */

    efontSize  printFontSize;       /*!< 屏幕输出的字体大小 */

    uint8_t lastTextLenth;          /*!< 最后一行文本内容长度 */

    char szlastText[100];           /*!< 最后一行文本内容 */

} OLedPrint_t;

#endif // _USE_OLED_PRINTF

/* Private define ----------------------------------------------------------------------------------------------------*/
/* Private macro -----------------------------------------------------------------------------------------------------*/

/* 取绝对值 */
#define LCD_ABS(x)               ((x) > 0 ? (x) : -(x))

/* Private variables -------------------------------------------------------------------------------------------------*/

//OLED的显存
static uint8_t sg_arrFrameBuffer[OLED_WIDTH][OLED_HEIGHT / 8] = {0};  // OLED画布

/** OLED 重要参数集配置表 */
static OLedDev_t sg_tOLedDevInfo =
{
    OLED_WIDTH,
    OLED_HEIGHT,
    OLED_BLACK,
    OLED_WHITE,
};

#ifdef _USE_OLED_PRINTF
/** OLED Printf 输出配置表 */
static OLedPrint_t sg_tOLedPrintInfo =
{
    OLED_PRINT_X,
    OLED_PRINT_Y,
    OLED_PRINT_WIDTH,
    OLED_PRINT_HIGH,
    OLED_PRINT_FONT,
    0,
    {0}
};
#endif // _USE_OLED_PRINTF

/* Private function prototypes ---------------------------------------------------------------------------------------*/
static void WriteCmd(uint8_t cmd);
static void WriteData(uint8_t data);
static uint16_t GetCharDataSize(eMoldMode way, oledsize_t width, oledsize_t high);
static void DrawDotMatrix(oledsize_t x, oledsize_t y, eMoldMode way, oledsize_t width, oledsize_t high, const uint8_t *pkBufData);
static int IsCharExit(char chr, char schr, char echr);
static void DrawOneChar(oledsize_t x, oledsize_t y, uint8_t chr, const fontConfig_t *pkfontInfo);
static int IsFontExit(uint8_t pszFont[2], const uint8_t *pkInx);
static void DrawOneFont(oledsize_t x, oledsize_t y, uint8_t pszFont[2], const fontConfig_t *pkfontInfo);
static int GetLineString(char *pszDest, const char *pszSrc, oledsize_t x, oledsize_t setWidth, oledsize_t charWidth, oledsize_t fontWidth, uint8_t init);
static void DrawLine(oledsize_t sx, oledsize_t sy, oledsize_t ex, oledsize_t ey);
static uint32_t myPow(uint8_t m, uint8_t n);
static void UintToStr(uint32_t num, char *str, uint8_t intLen, uint8_t zero);
static void IntToStr(int32_t num, char *str, uint8_t intLen, uint8_t zero);
static void FloatToStr(double num, char *str, uint8_t intLen, uint8_t decLen, uint8_t zero);

/* Private function --------------------------------------------------------------------------------------------------*/

/**
  * @brief      写指令.
  * @param[in]  cmd  指令.
  * @retval     None.
  */
static void WriteCmd(uint8_t cmd)
{
#if _DRIVE_INTERFACE_TYPE == OLED_IIC_INTERFACE
    OledDrv_IICStart();
    OledDrv_IICWriteByte(0x78);            //Slave address,SA0=0
    OledDrv_IICWriteByte(0x00);			//write command
    OledDrv_IICWriteByte(cmd);
    OledDrv_IICStop();
#else
    OLED_DC_Clr();
    OledDrv_SPIWriteByte(cmd);
#endif
}

/**
  * @brief      写数据.
  * @param[in]  data  数据.
  * @retval     None.
  */
static void WriteData(uint8_t data)
{
#if _DRIVE_INTERFACE_TYPE == OLED_IIC_INTERFACE
    OledDrv_IICStart();
    OledDrv_IICWriteByte(0x78);			//D/C#=0; R/W#=0
    OledDrv_IICWriteByte(0x40);			//write data
    OledDrv_IICWriteByte(data);
    OledDrv_IICStop();
#else
    OLED_DC_Set();
    OledDrv_SPIWriteByte(data);
#endif
}

/**
  * @brief      OLED 初始化.
  * @retval     None.
  */
void OLED_Init(void)
{
    OledDrv_Init();

    WriteCmd(0xae);//--turn off oled panel
    WriteCmd(0x00);//---set low column address
    WriteCmd(0x10);//---set high column address
    WriteCmd(0x40);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
    WriteCmd(0x81);//--set contrast control register
    WriteCmd(0xcf);// Set SEG Output Current Brightness
    WriteCmd(0xa1);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
    WriteCmd(0xc8);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
    WriteCmd(0xa6);//--set normal display
    WriteCmd(0xa8);//--set multiplex ratio(1 to 64)
    WriteCmd(0x3f);//--1/64 duty
    WriteCmd(0xd3);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
    WriteCmd(0x00);//-not offset
    WriteCmd(0xd5);//--set display clock divide ratio/oscillator frequency
    WriteCmd(0x80);//--set divide ratio, Set Clock as 100 Frames/Sec
    WriteCmd(0xd9);//--set pre-charge period
    WriteCmd(0xf1);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
    WriteCmd(0xda);//--set com pins hardware configuration
    WriteCmd(0x12);
    WriteCmd(0xdb);//--set vcomh
    WriteCmd(0x40);//Set VCOM Deselect Level
    WriteCmd(0x20);//-Set Page Addressing Mode (0x00/0x01/0x02)
    WriteCmd(0x02);//
    WriteCmd(0x8d);//--set Charge Pump enable/disable
    WriteCmd(0x14);//--set(0x10) disable
    WriteCmd(0xa4);// Disable Entire Display On (0xa4/0xa5)
    WriteCmd(0xa6);// Disable Inverse Display On (0xa6/a7)
    WriteCmd(0xaf);//--turn on oled panel
}

/**
  * @brief      设置光标.
  * @param[in]  data  字节数据.
  * @retval     None.
  */
void SetCursor(oledsize_t x, oledsize_t y)
{
    WriteCmd(0xb0 + (y >> 3));
    WriteCmd(((x & 0xf0) >> 4) | 0x10);
    WriteCmd((x & 0x0f));
    //WriteCmd((x & 0x0f) | 0x01);
}

/**
  * @brief      开启OLED显示.
  * @retval     None.
  */
void OLED_DisplayOn(void)
{
    WriteCmd(0X8D); //SET DCDC命令
    WriteCmd(0X14); //DCDC ON
    WriteCmd(0XAF); //DISPLAY ON
}

/**
  * @brief      关闭OLED显示.
  * @retval     None.
  */
void OLED_DisplayOff(void)
{
    WriteCmd(0X8D); //SET DCDC命令
    WriteCmd(0X10); //DCDC OFF
    WriteCmd(0XAE); //DISPLAY OFF
}

/**
  * @brief      画点函数.
  * @param[in]  x  纵坐标
  * @param[in]  y  横坐标
  * @param[in]  color  画点颜色
  * @retval     None.
  */
void OLED_DrawPoint(oledsize_t x, oledsize_t y, eOledcolor color)
{
    oledsize_t tmpY;

    x = x > sg_tOLedDevInfo.width ? sg_tOLedDevInfo.width : x;
    y = y > sg_tOLedDevInfo.height ? sg_tOLedDevInfo.height : y;

    tmpY = 0 + (y >> 3);

    if (color == OLED_WHITE)
    {
        sg_arrFrameBuffer[x][tmpY] = sg_arrFrameBuffer[x][tmpY] | (0x01 << (y % 8));
    }
    else
    {
        sg_arrFrameBuffer[x][tmpY] = sg_arrFrameBuffer[x][tmpY] & (~(0x01 << (y % 8)));
    }
}

/**
  * @brief      画粗点函数.
  * @param[in]  x  纵坐标
  * @param[in]  y  横坐标
  * @param[in]  color  画点颜色
  * @param[in]  size  点大小
  * @retval     None.
  */
void OLED_DrawRoughPoint(oledsize_t x, oledsize_t y, eOledcolor color, uint8_t size)
{
    uint8_t i, j;

    for (i = 0; i < size; i++)
    {
        for (j = 0; j < size; j++)
        {
            OLED_DrawPoint(x + i, y + j, color);
        }
    }
}

/**
  * @brief      读点函数.
  * @param[in]  x  纵坐标
  * @param[in]  y  横坐标
  * @retval     None.
  */
eOledcolor OLED_ReadPoint(oledsize_t x, oledsize_t y)
{
    oledsize_t tmpY = 0 + (y >> 3);

    if (sg_arrFrameBuffer[x][tmpY] & (0x01 << (y % 8)))
    {
        return OLED_WHITE;
    }
    else
    {
        return OLED_BLACK;
    }
}

/**
  * @brief      全部清除.
  * @note       屏幕会立即刷新
  * @param[in]  color  画笔颜色.
  * @retval     None.
  */
void OLED_Clear(uint8_t color)
{
    oledsize_t x, y;

    for (y = 0; y < sg_tOLedDevInfo.height / 8; y++)
    {
        for (x = 0; x < sg_tOLedDevInfo.width; x++)
        {
            sg_arrFrameBuffer[x][y] = color;
        }
    }
    
    OLED_SyncScreen(0, 0, sg_tOLedDevInfo.width, sg_tOLedDevInfo.height);
}

/**
  * @brief      局部清除.
  * @param[in]  sx  纵坐标起点
  * @param[in]  sy  横坐标起点
  * @param[in]  width 宽度
  * @param[in]  high  高度
  * @param[in]  color  画笔颜色.
  * @retval     None.
  */
void OLED_SetFill(oledsize_t sx, oledsize_t sy, oledsize_t width, oledsize_t high, eOledcolor color)
{
    oledsize_t x, y;

    for (y = sy; y < high; y++)
    {
        for (x = sx; x < width; x++)
        {
            OLED_DrawPoint(x, y, color);
        }
    }
}

/**
  * @brief      同步画面函数.
  * @param[in]  sx  纵坐标起点
  * @param[in]  sy  横坐标起点
  * @param[in]  width 宽度
  * @param[in]  high  高度
  * @retval     None.
  */
void OLED_SyncScreen(oledsize_t sx, oledsize_t sy, oledsize_t width, oledsize_t high)
{
    uint8_t y, x;
    oledsize_t ex = sx + width;
    oledsize_t ey = sy + high;

    sx = sx > sg_tOLedDevInfo.width ? sg_tOLedDevInfo.width : sx;
    ex = ex > sg_tOLedDevInfo.width ? sg_tOLedDevInfo.width : ex;

    sy = sy > sg_tOLedDevInfo.height ? sg_tOLedDevInfo.height : sy;
    ey = ey > sg_tOLedDevInfo.height ? sg_tOLedDevInfo.height : ey;
    
    sy = sy / 8;
    ey % 8 == 0 ? (ey = ey / 8) : (ey = ey / 8 + 1);

    for (y = sy; y < ey; y++)
    {
        WriteCmd(0xb0 + y);                 //设置页地址（0~7）
        WriteCmd((sx & 0x0f) + 0x02);       //设置显示位置—列低地址
        WriteCmd(((sx & 0xf0) >> 4) | 0x10);//设置显示位置—列高地址

        for (x = sx; x < ex; x++)
        {
            WriteData(sg_arrFrameBuffer[x][y]);
        }
    }
}

/**
  * @brief      屏幕刷新显示函数.
  *             界面画面同步任务
  * @retval     None.
  */
void OLED_ShowTask(void)
{
    uint8_t y, x;

    for (y = 0; y < sg_tOLedDevInfo.height / 8; y++)
    {
        WriteCmd(0xb0 + y);  //设置页地址（0~7）
        WriteCmd(0x02);      //设置显示位置—列低地址
        WriteCmd(0x10);      //设置显示位置—列高地址
        
        for (x = 0; x < sg_tOLedDevInfo.width; x++)
        {
            WriteData(sg_arrFrameBuffer[x][y]);
        }
    }
}

/**
  * @brief      指定窗口已有画面往指定方向偏移, 超出屏幕部分则自动填充背景色.
  * @param[in]  x       纵坐标起点位置.
  * @param[in]  y       行坐标起点位置
  * @param[in]  width   窗口宽度
  * @param[in]  high    窗口高度
  * @param[in]  dir     偏移方向
  *                      @arg 0 往上偏移
  *                      @arg 1 往下偏移
  *                      @arg 2 往左偏移
  *                      @arg 3 往右偏移
  * @param[in]  pixels  偏移的像素点
  * @retval     None.
  */
void OLED_SetScreenOffset(oledsize_t x, oledsize_t y, oledsize_t width, oledsize_t high, uint8_t dir, uint8_t pixels)
{
    uint8_t i, j;

    /* 屏幕部分窗口往上偏移消失 */
    if (dir == 0)
    {
        for (i = x; i < x + width; i++)
        {
            for (j = y; j < y + high; j++)
            {
                if (j + pixels < (y + high))
                {
                    OLED_DrawPoint(i, j, OLED_ReadPoint(i, j + pixels));
                }
                else
                {
                    OLED_DrawPoint(i, j, sg_tOLedDevInfo.backColor);
                }
            }
        }
    }
    else if (dir == 1) /* 屏幕部分窗口往下偏移消失 */
    {
        for (i = x; i < x + width; i++)
        {
            for (j = y + high; j > y; j--)
            {
                if (j - 1 - pixels > y)
                {
                    OLED_DrawPoint(i, j - 1, OLED_ReadPoint(i, j - 1 - pixels));
                }
                else
                {
                    OLED_DrawPoint(i, j - 1, sg_tOLedDevInfo.backColor);
                }
            }
        }
    }
    else if (dir == 2) /* 屏幕部分窗口往左偏移消失 */
    {
        for (j = y; j < y + high; j++)
        {
            for (i = x; i < x + width; i++)
            {
                if (i + pixels < (x + width))
                {
                    OLED_DrawPoint(i, j, OLED_ReadPoint(i + pixels, j));
                }
                else
                {
                    OLED_DrawPoint(i, j, sg_tOLedDevInfo.backColor);
                }
            }
        }
    }
    else if (dir == 3) /* 屏幕部分窗口往右偏移消失 */
    {
        for (j = y; j < y + high; j++)
        {
            for (i = x + width; i > x; i--)
            {
                if (i - 1 - pixels > x)
                {
                    OLED_DrawPoint(i - 1, j, OLED_ReadPoint(i - 1 - pixels, j));
                }
                else
                {
                    OLED_DrawPoint(i - 1, j, sg_tOLedDevInfo.backColor);
                }
            }
        }
    }
}

/**
  * @brief      画面反显函数.
  * @param[in]  sx  纵坐标起点
  * @param[in]  sy  横坐标起点
  * @param[in]  width 宽度
  * @param[in]  high  高度
  * @retval     None.
  */
void OLED_ReverseScreen(oledsize_t sx, oledsize_t sy, oledsize_t width, oledsize_t high)
{
    oledsize_t i, j;

    for (i = sx; i < sx + width; i++)
    {
        for (j = sy; j < sy + high; j++)
        {
            if (OLED_ReadPoint(i, j) == OLED_BLACK)
            {
                OLED_DrawPoint(i, j, OLED_WHITE);
            }
            else
            {
                OLED_DrawPoint(i, j, OLED_BLACK);
            }
        }
    }
}

/**
  * @brief      根据取模方式得到字符点阵数据大小.
  * @param      way     取模方式, 参考枚举 @enum eMoldMode
  * @param      width   显示的宽度
  * @param      high    显示的高度
  * @retval     字符点阵数据大小.
  */
static uint16_t GetCharDataSize(eMoldMode way, oledsize_t width, oledsize_t high)
{
    uint16_t dataSize = 0;
    
    switch (way)
    {
        case FONT_MODE_COLUMN_BY_COLUMN:     // 逐列式
        case FONT_MODE_COLUMN_BY_ROW:     // 列行式
            if (high & 0x07)
            {
                dataSize = ((high >> 3) + 1) * width;
            }
            else
            {
                dataSize = (high >> 3) * width;
            }
        break;
        
        case FONT_MODE_PROGRESSIVE:     // 逐行式
        case FONT_MODE_DETERMINANT:     // 行列式
            if (width & 0x07)
            {
                dataSize = ((width >> 3) + 1) * high;
            }
            else
            {
                dataSize = (width >> 3) * high;
            }
        break;
        default:
            break;
    }
    
    return dataSize;
}

/**
  * @brief      在指定位置画点阵内容.
  * @param      x       纵坐标起点位置.
  * @param      y       行坐标起点位置
  * @param      way     取模方式, 参考枚举 @enum eMoldMode
  * @param      width   显示的宽度
  * @param      high    显示的高度
  * @param      pkBufData 字符点阵数据
  * @retval     None.
  */
static void DrawDotMatrix(oledsize_t x, oledsize_t y, eMoldMode way, oledsize_t width, oledsize_t high, const uint8_t *pkBufData)
{
    oledsize_t temp;
    oledsize_t tmpx, tmpy;
    uint8_t dataNum;
    eOledcolor colortemp[2];

    colortemp[1] = sg_tOLedDevInfo.pointColor;
    colortemp[0] = sg_tOLedDevInfo.backColor;
    
    switch (way)
    {
        case FONT_MODE_COLUMN_BY_COLUMN:     // 逐列式
            dataNum = (high & 7) ? ((high >> 3) + 1) : (high >> 3);
        
            for (tmpx = 0; tmpx < width; tmpx++)
            {
                for (tmpy = 0; tmpy < high; tmpy++)
                {
                    temp = pkBufData[tmpx * dataNum + (tmpy >> 3)] >> (tmpy & 7);
                    OLED_DrawPoint(tmpx + x, tmpy + y, colortemp[temp & 0x01]);
                }
            }
            break;
        
        case FONT_MODE_PROGRESSIVE:     // 逐行式
            dataNum = (width & 7) ? (width >> 3) + 1 : (width >> 3);

            for (tmpy = 0; tmpy < high; tmpy++)
            {
                for (tmpx = 0; tmpx < width; tmpx++)
                {
                    temp = pkBufData[tmpy * dataNum + (tmpx >> 3)] >> (tmpx & 7);
                    OLED_DrawPoint(tmpx + x, tmpy + y, colortemp[temp & 0x01]);
                }
            }
            break;
        
        case FONT_MODE_COLUMN_BY_ROW:     // 列行式
            for (tmpx = 0; tmpx < width; tmpx++)
            {
                for (tmpy = 0; tmpy < high; tmpy++)
                {
                    temp = pkBufData[tmpx + (tmpy >> 3)  * width] >> (tmpy & 7);
                    OLED_DrawPoint(tmpx + x, tmpy + y, colortemp[temp & 0x01]);
                }
            }
            break;
            
        case FONT_MODE_DETERMINANT:     // 行列式
            for (tmpy = 0; tmpy < high; tmpy++)
            {
                for (tmpx = 0; tmpx < width; tmpx++)
                {
                    temp = pkBufData[tmpy + (tmpx >> 3)  * high] >> (tmpx & 7);
                    OLED_DrawPoint(tmpx + x, tmpy + y, colortemp[temp & 0x01]);
                }
            }
            break;
        default:
            break;
    }
}

/**
  * @brief      ASCII 字符集是否存在该字符.
  * @param[in]  chr    需要查找的字符
  * @param[in]  schr   ASCII 字符集开始偏移字符
  * @param[in]  echr   ASCII 字符集结束偏移字符
  * @retval     -1, 不存在; 其他,该字符的偏移位置.
  *
  */
static int IsCharExit(char chr, char schr, char echr)
{
    if (chr >= schr && chr <= echr)
    {
        return chr - schr;
    }

    return -1;
}

/**
  * @brief      在指定位置画一个非叠加的字符.
  * @param      x       纵坐标起点位置.
  * @param      y       行坐标起点位置
  * @param      chr     字符(" "--->"~")
  * @param      pkfontInfo 字体配置信息
  * @retval     None.
  */
static void DrawOneChar(oledsize_t x, oledsize_t y, uint8_t chr, const fontConfig_t *pkfontInfo)
{
    int idx;
    oledsize_t tmpx, tmpy;

    if (x > sg_tOLedDevInfo.width - pkfontInfo->charWidth || y > sg_tOLedDevInfo.height - pkfontInfo->height)
    {
        return;
    }
    
    if ((idx = IsCharExit(chr, pkfontInfo->charSIndex, pkfontInfo->charEIndex)) >= 0)
    {
        idx *= GetCharDataSize(pkfontInfo->way, pkfontInfo->charWidth, pkfontInfo->height);
        DrawDotMatrix(x, y, pkfontInfo->way, pkfontInfo->charWidth, pkfontInfo->height, &pkfontInfo->pkCharBufData[idx]);
    }
    else
    {
        for (tmpx = 0; tmpx < pkfontInfo->charWidth; tmpx++)
        {
            for (tmpy = 0; tmpy < pkfontInfo->height; tmpy++)
            {
                OLED_DrawPoint(tmpx + x, tmpy + y, sg_tOLedDevInfo.backColor);
            }
        }
    }
}

/**
  * @brief      汉字字符库是否存在该汉字.
  * @param[in]  pszFont 汉字(GB2312)
  * @param[in]  pkInx  汉字字符库
  * @retval     -1, 不存在; 其他,该汉字所在的字符库位置.
  *
  */
static int IsFontExit(uint8_t pszFont[2], const uint8_t *pkInx)
{
    uint16_t index = 0;

    /* 寻找字体索引 */
    while (pkInx[index] > 127)
    {
        if (pkInx[index] == pszFont[0] && pkInx[index + 1] == pszFont[1])
        {
            return index >> 1;
        }

        index += 2;
    }

    return -1;
}

/**
  * @brief      在指定位置画一个非叠加的汉字.
  * @param      x       纵坐标起点位置.
  * @param      y       行坐标起点位置
  * @param      pszFont  GB2312 格式的汉字
  * @param      pkfontInfo 字体配置信息
  * @retval     None.
  */
static void DrawOneFont(oledsize_t x, oledsize_t y, uint8_t pszFont[2], const fontConfig_t *pkfontInfo)
{
    oledsize_t tmpx, tmpy;
    int32_t idx;

    if (x > sg_tOLedDevInfo.width - pkfontInfo->fontWidth || y > sg_tOLedDevInfo.height - pkfontInfo->height)
    {
        return;
    }

    if ((idx = IsFontExit(pszFont, pkfontInfo->pkFontIdx)) >= 0)
    {
        idx *= GetCharDataSize(pkfontInfo->way, pkfontInfo->fontWidth, pkfontInfo->height);
        DrawDotMatrix(x, y, pkfontInfo->way, pkfontInfo->fontWidth, pkfontInfo->height, &pkfontInfo->pkFontBufData[idx]);
    }
    else
    {
        for (tmpx = 0; tmpx < pkfontInfo->fontWidth; tmpx++)
        {
            for (tmpy = 0; tmpy < pkfontInfo->height; tmpy++)
            {
                OLED_DrawPoint(tmpx + x, tmpy + y, sg_tOLedDevInfo.backColor);
            }
        }
    }
}

/**
  * @brief      设置背景色和画笔色.
  * @param[in]  backColor  画笔背景颜色.
  * @param[in]  pointColor  画笔颜色.
  * @retval     None.
  */
void OLED_SetColor(eOledcolor backColor, eOledcolor pointColor)
{
    sg_tOLedDevInfo.backColor = backColor;
    sg_tOLedDevInfo.pointColor = pointColor;
}

/**
  * @brief      根据换行符和设定屏幕宽度依次得到每行的文本内容(需要多次调用得到).
  * @param[out] pszDest 一行的文本内容
  * @param[in]  pszSrc  需要处理的字符串
  * @param[in]  x       纵坐标起点位置.
  * @param[in]  setWidth 设定屏幕宽度
  * @param[in]  fontWidth 字宽
  * @param[in]  init    重新获取每行文本内容
  * @retval     -1, 没有下行文本
  *              0, 回车符
  *              1, 换行符.
  *              2, 超出屏幕范围
  */
static int GetLineString(char *pszDest, const char *pszSrc, oledsize_t x, oledsize_t setWidth, 
                            oledsize_t charWidth, oledsize_t fontWidth, uint8_t init)
{
    oledsize_t tmpWidth = 0;
    static const char *s_pszSrc = 0;

    if (init)
    {
        s_pszSrc = pszSrc;
    }

    while (s_pszSrc != 0 && *s_pszSrc != 0)
    {
        if (*s_pszSrc == '\n')
        {
            s_pszSrc++;
            return 1;
        }
        else if (*s_pszSrc == '\r')
        {
            s_pszSrc++;

            if (*s_pszSrc == '\n')
            {
                s_pszSrc++;
                return 1;
            }
            else
            {
                return 0;
            }
        }

        if (*s_pszSrc > 127)
        {
            tmpWidth += fontWidth;

            if (tmpWidth > setWidth)
            {
                return 2;
            }

            *pszDest = *s_pszSrc;
            s_pszSrc++;
            pszDest++;
        }
        else
        {
            tmpWidth += charWidth;

            if (tmpWidth > setWidth)
            {
                return 2;
            }
        }

        *pszDest = *s_pszSrc;
        s_pszSrc++;
        pszDest++;
    }

    return -1;
}

/**
  * @brief      在指定位置设置文本内容.
  * @param[in]  x       纵坐标起点位置.
  * @param[in]  y       行坐标起点位置
  * @param[in]  pszStr  文本内容
  * @param[in]  isMultiLine 0-不支持多行显示 1-支持多行显示
  * @param[in]  size    文本大小
  * @retval     None.
  */
void OLED_SetText(oledsize_t x, oledsize_t y, const char *pszStr, uint8_t isMultiLine, efontSize size)
{
    fontConfig_t tFontConfig;
    uint8_t isAscii = 1;
    oledsize_t tmpX = x;
    oledsize_t tmpY = y;
    uint8_t szFontData[2] = {0};
    uint16_t i = 0;
    
    if (OLED_GetFontConfig(size, &tFontConfig))
    {
        return;
    }

    while (pszStr[i] != '\0')
    {
        if (pszStr[i] > 127)
        {
            isAscii = 0;

            if (tmpX > sg_tOLedDevInfo.width - tFontConfig.fontWidth)
            {
                if (isMultiLine && tmpY < (sg_tOLedDevInfo.height - tFontConfig.height))
                {
                    tmpX = x;
                    tmpY += tFontConfig.height;
                }
                else
                {
                    break;
                }
            }
        }
        else
        {
            isAscii = 1;

            if (tmpX > sg_tOLedDevInfo.width - tFontConfig.charWidth)
            {
                if (isMultiLine && tmpY < (sg_tOLedDevInfo.height - tFontConfig.height))
                {
                    tmpX = x;
                    tmpY += tFontConfig.height;
                }
                else
                {
                    break;
                }
            }
        }

        if (isAscii == 0)
        {
            if (tFontConfig.pkFontIdx != 0 && tFontConfig.pkFontBufData != 0)
            {
                szFontData[0] = pszStr[i];
                szFontData[1] = pszStr[i + 1];
                DrawOneFont(tmpX, tmpY, szFontData, &tFontConfig);
                tmpX += tFontConfig.fontWidth;
            }

            i += 2;
        }
        else
        {
            if (tFontConfig.pkCharBufData != 0)
            {
                DrawOneChar(tmpX, tmpY, pszStr[i], &tFontConfig);
                tmpX += tFontConfig.charWidth;
            }

            i++;
        }
    }
}

/**
  * @brief      原型函数 pow.
  * @param      m     基数
  * @param      n     指数
  * @return     m 的 n 次幂的结果
  */
static uint32_t myPow(uint8_t m, uint8_t n)
{
    uint32_t result = 1;

    while (n--)
    {
        result *= m;
    }

    return result;
}

/**
  * @brief      无符号整数转字符串.
  * @param      num     数值
  * @param      str     转换后的字符串
  * @param      intLen  整数长度
  * @param      zero    0,高位为 0 时不显示; 1,高位为 0 时显示
  * @retval     None.
  */
static void UintToStr(uint32_t num, char *str, uint8_t intLen, uint8_t zero)
{
    uint8_t t;
    uint8_t flag = 0;

    if (intLen > 10)
    {
        intLen = 10;
    }

    for (t = 0; t < intLen; t++)
    {
        str[t] = ((num / myPow(10, intLen - t - 1)) % 10) + '0';

        /* 高位不显示 && 高位为 0 未搜索完成 && 当前位为 0 && 若检索到最低位还是 0 则保留, 防止 0 没有显示 */
        if (zero == 0 && flag == 0 && str[t] == '0' && (t < intLen - 1))
        {
            str[t] = ' ';
        }
        else
        {
            flag = 1; /* 标记高位为 0 的个数已完结 */
        }
    }
}

/**
  * @brief      有符号整数转字符串.
  * @param      num     数值
  * @param      str     转换后的字符串
  * @param      intLen  整数长度
  * @param      zero    0,高位为 0 时不显示; 1,高位为 0 时显示
  * @retval     None.
  */
static void IntToStr(int32_t num, char *str, uint8_t intLen, uint8_t zero)
{
    uint8_t t;
    uint8_t flag = 0;
    uint8_t zeroNum = 0;

    if (intLen > 10)
    {
        intLen = 10;
    }

    /* 增加符号位 */
    str[0] = ' ';

    if (num > 0)
    {
        for (t = 0; t < intLen; t++)
        {
            str[t + 1] = ((num / myPow(10, intLen - t - 1)) % 10) + '0';

            /* 高位不显示 && 高位为 0 未搜索完成 && 当前位为 0 && 若检索到最低位还是 0 则保留, 防止 0 没有显示 */
            if (zero == 0 && flag == 0 && str[t + 1] == '0' && (t < intLen - 1))
            {
                str[t + 1] = ' ';
            }
            else
            {
                flag = 1; /* 标记高位为 0 的个数已完结 */
            }
        }
    }
    else
    {
        num = LCD_ABS(num);

        for (t = 0; t < intLen; t++)
        {
            str[t + 1] = ((num / myPow(10, intLen - t - 1)) % 10) + '0';

            /* 高位不显示 && 高位为 0 未搜索完成 && 当前位为 0 && 若检索到最低位还是 0 则保留, 防止 0 没有显示 */
            if (zero == 0 && flag == 0 && str[t + 1] == '0' && (t < intLen - 1))
            {
                str[t + 1] = ' ';
                zeroNum++;
            }
            else
            {
                flag = 1;   /* 标记高位为 0 的个数已完结 */
            }
        }

        if (num != 0)       /* 该值为 0 不显示负号 */
        {
            str[zeroNum] = '-';
        }
    }
}

/**
  * @brief      浮点数转字符串.
  * @param      num     数值
  * @param      str     转换后的字符串
  * @param      intLen  整数长度
  * @param      decLen  小数长度
  * @param      zero    0,高位为 0 时不显示; 1,高位为 0 时显示
  * @retval     None.
  */
static void FloatToStr(double num, char *str, uint8_t intLen, uint8_t decLen, uint8_t zero)
{
    uint8_t t;
    uint8_t flag = 0;
    uint8_t zeroNum = 0;
    int16_t powVal = myPow(10, decLen);
    int16_t integer_num = (int16_t)num;
    int16_t decimal_num = (int16_t)((num * powVal - integer_num * powVal));

    if (intLen > 10)
    {
        intLen = 10;
    }

    if (decLen > 10)
    {
        decLen = 10;
    }

    integer_num = LCD_ABS(integer_num);
    decimal_num = LCD_ABS(decimal_num);

    /* 增加符号位 */
    str[0] = ' ';

    /* 增加小数位 */
    if (decLen != 0)
    {
        str[intLen + 1] = '.';
    }

    for (t = 0; t < intLen; t++)
    {
        str[t + 1] = ((integer_num / myPow(10, intLen - t - 1)) % 10) + '0';

        /* 高位不显示 && 高位为 0 未搜索完成 && 当前位为 0 && 若检索到最低位还是 0 则保留, 防止 0 没有显示 */
        if (zero == 0 && flag == 0 && str[t + 1] == '0' && (t < intLen - 1))
        {
            str[t + 1] = ' ';

            if (num < 0)
            {
                zeroNum++;
            }
        }
        else
        {
            flag = 1; /* 标记高位为 0 的个数已完结 */
        }
    }

    for (t = 0; t < decLen; t++)
    {
        str[t + intLen + 2] = ((decimal_num / myPow(10, decLen - t - 1)) % 10) + '0';
    }

    if (num >= 0)
    {
        for (t = 0; t < (decLen + intLen + 2); t++)
        {
            str[t] = str[t + 1];
        }
    }
    else
    {
        str[zeroNum] = '-';
    }


}

/**
  * @brief      在指定位置设置整数型数字内容.
  * @param[in]  x       纵坐标起点位置.
  * @param[in]  y       行坐标起点位置
  * @param[in]  num     整数型数字
  * @param[in]  len     可显示的数字最大长度
  * @param[in]  zero    最高位为0是否显示, 0-不显示 1-显示
  * @param[in]  size    数字文本大小
  * @retval     None.
  */
void OLED_SetIntegerNum(oledsize_t x, oledsize_t y, int32_t num, uint8_t len, uint8_t zero, efontSize size)
{
    char tmpStr[25] = {0};
    uint32_t unum = num;

    if (num < 0)
    {
        IntToStr(num, tmpStr, len, zero);
    }
    else
    {
        UintToStr(unum, tmpStr, len, zero);
    }

    OLED_SetText(x, y, tmpStr, 0, size);
}

/**
  * @brief      在指定位置设置浮点型数字内容.
  * @param[in]  x       纵坐标起点位置.
  * @param[in]  y       行坐标起点位置
  * @param[in]  num     浮点型数字
  * @param[in]  intLen  可显示的整数部分数字最大长度
  * @param[in]  decLen  可显示的小数部分数字最大长度
  * @param[in]  zero    最高位为0是否显示, 0-不显示 1-显示
  * @param[in]  size    数字文本大小
  * @retval     None.
  */
void OLED_SetFloatNum(oledsize_t x, oledsize_t y, float num, uint8_t intLen, uint8_t decLen, uint8_t zero, efontSize size)
{
    char tmpStr[25] = {0};

    FloatToStr(num, tmpStr, intLen, decLen, zero);

    OLED_SetText(x, y, tmpStr, 0, size);
}

#ifdef _USE_OLED_PRINTF
/**
  * @brief      发送格式化输出到 OLED 输出(简易).
  * @param[in]  x       纵坐标起点位置.
  * @param[in]  y       行坐标起点位置
  * @param[in]  isMultiLine 0-不支持多行显示 1-支持多行显示
  * @param[in]  size    文本大小
  * @param[in]  format 格式化字符串
  * @param[in]  ... 格式化字符串中的参数表
  * @retval     None
  */
void OLED_XYPrintf(oledsize_t x, oledsize_t y, uint8_t isMultiLine, efontSize size, const char *format, ...)
{
    char szText[100] = {0};
    va_list aptr;

    va_start(aptr, format);
    vsprintf(szText, format, aptr);
    va_end(aptr);

    OLED_SetText(x, y, szText, isMultiLine, size);
}

/**
  * @brief      发送格式化输出到 OLED 输出.
  *             具备自动换行, 同时支持识别'\r'和'\n'
  * @param[in]  format 格式化字符串
  * @param[in]  ... 格式化字符串中的参数表
  * @retval     None
  */
void OLED_Printf(const char *format, ...)
{
    fontConfig_t tFontConfig;
    OLedPrint_t *pOLedPrint = &sg_tOLedPrintInfo;
    oledsize_t printWindowHeight = pOLedPrint->printHeight;
    uint8_t init = 1;
    int ret = 0;
    char szText[100] = {0};
    va_list aptr;

    if (OLED_GetFontConfig(pOLedPrint->printFontSize, &tFontConfig))
    {
        return;
    }
    
    if (pOLedPrint->lastTextLenth)
    {
        memcpy(szText, pOLedPrint->szlastText, pOLedPrint->lastTextLenth);
    }

    va_start(aptr, format);
    vsprintf(&szText[pOLedPrint->lastTextLenth], format, aptr);
    va_end(aptr);

    do
    {
        ret = GetLineString(pOLedPrint->szlastText, szText, OLED_PRINT_X, OLED_PRINT_WIDTH, 
                                tFontConfig.charWidth, tFontConfig.fontWidth, init);
        init = 0;

        if (pOLedPrint->printY > (printWindowHeight - tFontConfig.height) && ret >= 0)
        {
            OLED_SetScreenOffset(OLED_PRINT_X, OLED_PRINT_Y, OLED_PRINT_WIDTH, OLED_PRINT_HIGH,
                                 0, tFontConfig.height - (printWindowHeight - pOLedPrint->printY));
            pOLedPrint->printY = printWindowHeight - tFontConfig.height;
        }

        OLED_SetText(pOLedPrint->printX, pOLedPrint->printY, pOLedPrint->szlastText, 0, pOLedPrint->printFontSize);

        if (ret >= 0)
        {
            pOLedPrint->lastTextLenth = 0;
            memset(pOLedPrint->szlastText, 0, sizeof(pOLedPrint->szlastText));

            if (ret > 0)
            {
                pOLedPrint->printY += tFontConfig.height;
            }
        }
        else
        {
            pOLedPrint->lastTextLenth = strlen(pOLedPrint->szlastText);
        }
    } while (ret >= 0);
}
#endif // _USE_OLED_PRINTF

/**
  * @brief      画线.
  * @param[in]  sx  纵坐标起点
  * @param[in]  sy  横坐标起点
  * @param[in]  ex  纵坐标终点
  * @param[in]  ey  横坐标终点
  * @param[in]  color  画笔颜色.
  * @retval     None.
  */
static void DrawLine(oledsize_t sx, oledsize_t sy, oledsize_t ex, oledsize_t ey)
{
    uint16_t t;
    int32_t xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int32_t incx, incy, uRow, uCol;

    delta_x = (int32_t)ex - sx; //计算坐标增量
    delta_y = (int32_t)ey - sy;
    uRow = sx;
    uCol = sy;

    if (delta_x > 0)
    {
        incx = 1;    //设置单步方向
    }
    else if (delta_x == 0)
    {
        incx = 0;    //垂直线
    }
    else
    {
        incx = -1;
        delta_x = -delta_x;
    }

    if (delta_y > 0)
    {
        incy = 1;
    }
    else if (delta_y == 0)
    {
        incy = 0;    //水平线
    }
    else
    {
        incy = -1;
        delta_y = -delta_y;
    }

    if (delta_x > delta_y)
    {
        distance = delta_x;    //选取基本增量坐标轴
    }
    else
    {
        distance = delta_y;
    }

    for (t = 0; t <= distance + 1; t++) //画线输出
    {
        OLED_DrawPoint(uRow, uCol, sg_tOLedDevInfo.pointColor);

        xerr += delta_x ;
        yerr += delta_y ;

        if (xerr > distance)
        {
            xerr -= distance;
            uRow += incx;
        }

        if (yerr > distance)
        {
            yerr -= distance;
            uCol += incy;
        }
    }
}

/**
  * @brief      画线.
  *             线体根据大小往下缩放
  * @param[in]  sx  纵坐标起点
  * @param[in]  sy  横坐标起点
  * @param[in]  ex  纵坐标终点
  * @param[in]  ey  横坐标终点
  * @param[in]  size 线体大小.
  * @retval     None.
  */
void OLED_DrawLine(oledsize_t sx, oledsize_t sy, oledsize_t ex, oledsize_t ey, uint8_t size)
{
    uint8_t i;

    for (i = 0; i < size; i++)
    {
        DrawLine(sx + i, sy + i, ex + i, ey + i);
    }
}

/**
  * @brief      画矩形.
  *             线体根据大小往矩形内缩放
  * @param[in]  sx  纵坐标起点
  * @param[in]  sy  横坐标起点
  * @param[in]  width 宽度
  * @param[in]  high  高度
  * @param[in]  size 线体大小.
  * @retval     None.
  */
void OLED_DrawRectangle(oledsize_t sx, oledsize_t sy, oledsize_t width, oledsize_t high, uint8_t size)
{
    uint8_t i;
    oledsize_t ex = sx + width;
    oledsize_t ey = sy + high;

    for (i = 0; i < size; i++)
    {
        DrawLine(sx + i, sy + i, ex - i,   sy + i);
        DrawLine(sx + i, sy + i, sx + i, ey - i);
        DrawLine(sx + i, ey - i,   ex - i,   ey - i);
        DrawLine(ex - i, sy + i, ex - i,   ey - i);
    }
}

/**
  * @brief      画矩形.
  *             线体根据大小往矩形内缩放
  * @param[in]  sx  纵坐标起点
  * @param[in]  sy  横坐标起点
  * @param[in]  ex  纵坐标对点
  * @param[in]  ey  横坐标对点
  * @param[in]  size 线体大小.
  * @retval     None.
  */
void OLED_DrawCircle(oledsize_t x, oledsize_t y, oledsize_t radius, uint8_t size)
{
    uint8_t i;
    oledsize_t tmpRadius = radius;
    int32_t a = 0;
    int32_t di = 3 - (radius << 1);       /* 判断下个点位置的标志 */

    radius = radius - size / 2;

    tmpRadius = radius;
    di = 3 - (radius << 1);

    for (i = 0; i < size; i++)
    {
        while (a <= tmpRadius)
        {
            OLED_DrawPoint(x - tmpRadius, y - a,       sg_tOLedDevInfo.pointColor);        //3
            OLED_DrawPoint(x + tmpRadius, y - a,       sg_tOLedDevInfo.pointColor);        //0

            OLED_DrawPoint(x - a,      y + tmpRadius,  sg_tOLedDevInfo.pointColor);        //1
            OLED_DrawPoint(x - tmpRadius, y - a,       sg_tOLedDevInfo.pointColor);        //7

            OLED_DrawPoint(x - a,      y - tmpRadius,  sg_tOLedDevInfo.pointColor);        //2
            OLED_DrawPoint(x + tmpRadius, y + a,       sg_tOLedDevInfo.pointColor);        //4

            OLED_DrawPoint(x + a,      y - tmpRadius,  sg_tOLedDevInfo.pointColor);        //5
            OLED_DrawPoint(x + a,      y + tmpRadius,  sg_tOLedDevInfo.pointColor);        //6
            OLED_DrawPoint(x - tmpRadius, y + a,       sg_tOLedDevInfo.pointColor);

            a++;

            /* 使用Bresenham算法画圆 */
            if (di < 0)
            {
                di += 8 * a + 5 - i;
            }
            else
            {
                di += 10 + 8 * (a - tmpRadius) - i;
                tmpRadius--;
            }

            OLED_DrawPoint(x + a, y + tmpRadius, sg_tOLedDevInfo.pointColor);
        }

        a = 0;
        tmpRadius = radius - i;
        di = 3 - (radius << 1);
    }
}

#if OLED_GRAPHIC_NUM
/**
  * @brief      画图形.
  * @param[in]  sx  纵坐标起点
  * @param[in]  sy  横坐标起点
  * @param[in]  ex  纵坐标对点
  * @param[in]  ey  横坐标对点
  * @param[in]  pkszName  图形名称
  * @param[in]  size 图形线体大小.
  * @retval     None.
  */
void OLED_DrawGraphic(oledsize_t x, oledsize_t y, const char *pkszName, uint8_t size)
{
    graphicConfig_t graphicInfo;

    if (OLED_GetGraphicConfig(pkszName, &graphicInfo) == 0)
    {
        DrawDotMatrix(x, y, graphicInfo.way, graphicInfo.width, graphicInfo.high, graphicInfo.pkBufData);
    }
}
#endif



