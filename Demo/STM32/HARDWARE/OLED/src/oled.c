/**
  **********************************************************************************************************************
  * @file    oled.c
  * @brief   ���ļ��ṩOLED������ع���
  * @author  ������    any question please send mail to const_zpc@163.com
  * @version V1.2.0
  * @date    2021-4-24
  *
  * @details  ������ϸ˵����
  *           + OLED ��ʼ������
  *           + OLED ���ƺ���
  *           + ����/���㺯��
  *           + OLED ��������
  *           + �����������
  *           + ��ͼ�κ���
  *           + �����ı�/���ֺ���
  *           + ��ʽ���������
  *
  **********************************************************************************************************************
  * Դ��·����https://gitee.com/const-zpc/STM32_OLED.git �������⼰������ڸ���ַ��д Issue
  *
  * ʹ�÷�ʽ:
  *    1��ʹ��ǰ��ʼ������ OLED_Init, ͬʱ����ʹ�� OLED_Clear ��������
  *    2�����ڵ��ú��� OLED_ShowTask, ����ͬ����������(������ͬ���� OLED ����ʾ)
  *    3���ɶ���궨�� "_USE_OLED_PRINTF" ʹ��OLED ��ʽ�����������, ������ printf �Ĺ���
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
  * @brief OLED ��Ҫ�������ṹ�嶨��
  */
typedef struct  tag_OLedDev
{
    oledsize_t width;			    /*!< OLED ��� */

    oledsize_t height;	            /*!< OLED �߶� */

    eOledcolor backColor;           /*!< ����ɫ */

    eOledcolor pointColor;          /*!< ����ɫ */
} OLedDev_t;

#ifdef _USE_OLED_PRINTF

/**
  * @brief OLED Printf �����ض���
  */
typedef struct  tag_OLedPrint
{
    oledsize_t printX;              /*!< ��Ļ������������ */

    oledsize_t printY;              /*!< ��Ļ������������ */

    oledsize_t printWidth;          /*!< ��Ļ������ */

    oledsize_t printHeight;         /*!< ��Ļ����߶� */

    efontSize  printFontSize;       /*!< ��Ļ����������С */

    uint8_t lastTextLenth;          /*!< ���һ���ı����ݳ��� */

    char szlastText[100];           /*!< ���һ���ı����� */

} OLedPrint_t;

#endif // _USE_OLED_PRINTF

/* Private define ----------------------------------------------------------------------------------------------------*/
/* Private macro -----------------------------------------------------------------------------------------------------*/

/* ȡ����ֵ */
#define LCD_ABS(x)               ((x) > 0 ? (x) : -(x))

/* Private variables -------------------------------------------------------------------------------------------------*/

//OLED���Դ�
static uint8_t sg_arrFrameBuffer[OLED_WIDTH][OLED_HEIGHT / 8] = {0};  // OLED����

/** OLED ��Ҫ���������ñ� */
static OLedDev_t sg_tOLedDevInfo =
{
    OLED_WIDTH,
    OLED_HEIGHT,
    OLED_BLACK,
    OLED_WHITE,
};

#ifdef _USE_OLED_PRINTF
/** OLED Printf ������ñ� */
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
  * @brief      дָ��.
  * @param[in]  cmd  ָ��.
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
  * @brief      д����.
  * @param[in]  data  ����.
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
  * @brief      OLED ��ʼ��.
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
    WriteCmd(0xa1);//--Set SEG/Column Mapping     0xa0���ҷ��� 0xa1����
    WriteCmd(0xc8);//Set COM/Row Scan Direction   0xc0���·��� 0xc8����
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
  * @brief      ���ù��.
  * @param[in]  data  �ֽ�����.
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
  * @brief      ����OLED��ʾ.
  * @retval     None.
  */
void OLED_DisplayOn(void)
{
    WriteCmd(0X8D); //SET DCDC����
    WriteCmd(0X14); //DCDC ON
    WriteCmd(0XAF); //DISPLAY ON
}

/**
  * @brief      �ر�OLED��ʾ.
  * @retval     None.
  */
void OLED_DisplayOff(void)
{
    WriteCmd(0X8D); //SET DCDC����
    WriteCmd(0X10); //DCDC OFF
    WriteCmd(0XAE); //DISPLAY OFF
}

/**
  * @brief      ���㺯��.
  * @param[in]  x  ������
  * @param[in]  y  ������
  * @param[in]  color  ������ɫ
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
  * @brief      ���ֵ㺯��.
  * @param[in]  x  ������
  * @param[in]  y  ������
  * @param[in]  color  ������ɫ
  * @param[in]  size  ���С
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
  * @brief      ���㺯��.
  * @param[in]  x  ������
  * @param[in]  y  ������
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
  * @brief      ȫ�����.
  * @note       ��Ļ������ˢ��
  * @param[in]  color  ������ɫ.
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
  * @brief      �ֲ����.
  * @param[in]  sx  ���������
  * @param[in]  sy  ���������
  * @param[in]  width ���
  * @param[in]  high  �߶�
  * @param[in]  color  ������ɫ.
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
  * @brief      ͬ�����溯��.
  * @param[in]  sx  ���������
  * @param[in]  sy  ���������
  * @param[in]  width ���
  * @param[in]  high  �߶�
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
        WriteCmd(0xb0 + y);                 //����ҳ��ַ��0~7��
        WriteCmd((sx & 0x0f) + 0x02);       //������ʾλ�á��е͵�ַ
        WriteCmd(((sx & 0xf0) >> 4) | 0x10);//������ʾλ�á��иߵ�ַ

        for (x = sx; x < ex; x++)
        {
            WriteData(sg_arrFrameBuffer[x][y]);
        }
    }
}

/**
  * @brief      ��Ļˢ����ʾ����.
  *             ���滭��ͬ������
  * @retval     None.
  */
void OLED_ShowTask(void)
{
    uint8_t y, x;

    for (y = 0; y < sg_tOLedDevInfo.height / 8; y++)
    {
        WriteCmd(0xb0 + y);  //����ҳ��ַ��0~7��
        WriteCmd(0x02);      //������ʾλ�á��е͵�ַ
        WriteCmd(0x10);      //������ʾλ�á��иߵ�ַ
        
        for (x = 0; x < sg_tOLedDevInfo.width; x++)
        {
            WriteData(sg_arrFrameBuffer[x][y]);
        }
    }
}

/**
  * @brief      ָ���������л�����ָ������ƫ��, ������Ļ�������Զ���䱳��ɫ.
  * @param[in]  x       ���������λ��.
  * @param[in]  y       ���������λ��
  * @param[in]  width   ���ڿ��
  * @param[in]  high    ���ڸ߶�
  * @param[in]  dir     ƫ�Ʒ���
  *                      @arg 0 ����ƫ��
  *                      @arg 1 ����ƫ��
  *                      @arg 2 ����ƫ��
  *                      @arg 3 ����ƫ��
  * @param[in]  pixels  ƫ�Ƶ����ص�
  * @retval     None.
  */
void OLED_SetScreenOffset(oledsize_t x, oledsize_t y, oledsize_t width, oledsize_t high, uint8_t dir, uint8_t pixels)
{
    uint8_t i, j;

    /* ��Ļ���ִ�������ƫ����ʧ */
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
    else if (dir == 1) /* ��Ļ���ִ�������ƫ����ʧ */
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
    else if (dir == 2) /* ��Ļ���ִ�������ƫ����ʧ */
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
    else if (dir == 3) /* ��Ļ���ִ�������ƫ����ʧ */
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
  * @brief      ���淴�Ժ���.
  * @param[in]  sx  ���������
  * @param[in]  sy  ���������
  * @param[in]  width ���
  * @param[in]  high  �߶�
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
  * @brief      ����ȡģ��ʽ�õ��ַ��������ݴ�С.
  * @param      way     ȡģ��ʽ, �ο�ö�� @enum eMoldMode
  * @param      width   ��ʾ�Ŀ��
  * @param      high    ��ʾ�ĸ߶�
  * @retval     �ַ��������ݴ�С.
  */
static uint16_t GetCharDataSize(eMoldMode way, oledsize_t width, oledsize_t high)
{
    uint16_t dataSize = 0;
    
    switch (way)
    {
        case FONT_MODE_COLUMN_BY_COLUMN:     // ����ʽ
        case FONT_MODE_COLUMN_BY_ROW:     // ����ʽ
            if (high & 0x07)
            {
                dataSize = ((high >> 3) + 1) * width;
            }
            else
            {
                dataSize = (high >> 3) * width;
            }
        break;
        
        case FONT_MODE_PROGRESSIVE:     // ����ʽ
        case FONT_MODE_DETERMINANT:     // ����ʽ
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
  * @brief      ��ָ��λ�û���������.
  * @param      x       ���������λ��.
  * @param      y       ���������λ��
  * @param      way     ȡģ��ʽ, �ο�ö�� @enum eMoldMode
  * @param      width   ��ʾ�Ŀ��
  * @param      high    ��ʾ�ĸ߶�
  * @param      pkBufData �ַ���������
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
        case FONT_MODE_COLUMN_BY_COLUMN:     // ����ʽ
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
        
        case FONT_MODE_PROGRESSIVE:     // ����ʽ
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
        
        case FONT_MODE_COLUMN_BY_ROW:     // ����ʽ
            for (tmpx = 0; tmpx < width; tmpx++)
            {
                for (tmpy = 0; tmpy < high; tmpy++)
                {
                    temp = pkBufData[tmpx + (tmpy >> 3)  * width] >> (tmpy & 7);
                    OLED_DrawPoint(tmpx + x, tmpy + y, colortemp[temp & 0x01]);
                }
            }
            break;
            
        case FONT_MODE_DETERMINANT:     // ����ʽ
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
  * @brief      ASCII �ַ����Ƿ���ڸ��ַ�.
  * @param[in]  chr    ��Ҫ���ҵ��ַ�
  * @param[in]  schr   ASCII �ַ�����ʼƫ���ַ�
  * @param[in]  echr   ASCII �ַ�������ƫ���ַ�
  * @retval     -1, ������; ����,���ַ���ƫ��λ��.
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
  * @brief      ��ָ��λ�û�һ���ǵ��ӵ��ַ�.
  * @param      x       ���������λ��.
  * @param      y       ���������λ��
  * @param      chr     �ַ�(" "--->"~")
  * @param      pkfontInfo ����������Ϣ
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
  * @brief      �����ַ����Ƿ���ڸú���.
  * @param[in]  pszFont ����(GB2312)
  * @param[in]  pkInx  �����ַ���
  * @retval     -1, ������; ����,�ú������ڵ��ַ���λ��.
  *
  */
static int IsFontExit(uint8_t pszFont[2], const uint8_t *pkInx)
{
    uint16_t index = 0;

    /* Ѱ���������� */
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
  * @brief      ��ָ��λ�û�һ���ǵ��ӵĺ���.
  * @param      x       ���������λ��.
  * @param      y       ���������λ��
  * @param      pszFont  GB2312 ��ʽ�ĺ���
  * @param      pkfontInfo ����������Ϣ
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
  * @brief      ���ñ���ɫ�ͻ���ɫ.
  * @param[in]  backColor  ���ʱ�����ɫ.
  * @param[in]  pointColor  ������ɫ.
  * @retval     None.
  */
void OLED_SetColor(eOledcolor backColor, eOledcolor pointColor)
{
    sg_tOLedDevInfo.backColor = backColor;
    sg_tOLedDevInfo.pointColor = pointColor;
}

/**
  * @brief      ���ݻ��з����趨��Ļ������εõ�ÿ�е��ı�����(��Ҫ��ε��õõ�).
  * @param[out] pszDest һ�е��ı�����
  * @param[in]  pszSrc  ��Ҫ������ַ���
  * @param[in]  x       ���������λ��.
  * @param[in]  setWidth �趨��Ļ���
  * @param[in]  fontWidth �ֿ�
  * @param[in]  init    ���»�ȡÿ���ı�����
  * @retval     -1, û�������ı�
  *              0, �س���
  *              1, ���з�.
  *              2, ������Ļ��Χ
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
  * @brief      ��ָ��λ�������ı�����.
  * @param[in]  x       ���������λ��.
  * @param[in]  y       ���������λ��
  * @param[in]  pszStr  �ı�����
  * @param[in]  isMultiLine 0-��֧�ֶ�����ʾ 1-֧�ֶ�����ʾ
  * @param[in]  size    �ı���С
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
  * @brief      ԭ�ͺ��� pow.
  * @param      m     ����
  * @param      n     ָ��
  * @return     m �� n ���ݵĽ��
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
  * @brief      �޷�������ת�ַ���.
  * @param      num     ��ֵ
  * @param      str     ת������ַ���
  * @param      intLen  ��������
  * @param      zero    0,��λΪ 0 ʱ����ʾ; 1,��λΪ 0 ʱ��ʾ
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

        /* ��λ����ʾ && ��λΪ 0 δ������� && ��ǰλΪ 0 && �����������λ���� 0 ����, ��ֹ 0 û����ʾ */
        if (zero == 0 && flag == 0 && str[t] == '0' && (t < intLen - 1))
        {
            str[t] = ' ';
        }
        else
        {
            flag = 1; /* ��Ǹ�λΪ 0 �ĸ�������� */
        }
    }
}

/**
  * @brief      �з�������ת�ַ���.
  * @param      num     ��ֵ
  * @param      str     ת������ַ���
  * @param      intLen  ��������
  * @param      zero    0,��λΪ 0 ʱ����ʾ; 1,��λΪ 0 ʱ��ʾ
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

    /* ���ӷ���λ */
    str[0] = ' ';

    if (num > 0)
    {
        for (t = 0; t < intLen; t++)
        {
            str[t + 1] = ((num / myPow(10, intLen - t - 1)) % 10) + '0';

            /* ��λ����ʾ && ��λΪ 0 δ������� && ��ǰλΪ 0 && �����������λ���� 0 ����, ��ֹ 0 û����ʾ */
            if (zero == 0 && flag == 0 && str[t + 1] == '0' && (t < intLen - 1))
            {
                str[t + 1] = ' ';
            }
            else
            {
                flag = 1; /* ��Ǹ�λΪ 0 �ĸ�������� */
            }
        }
    }
    else
    {
        num = LCD_ABS(num);

        for (t = 0; t < intLen; t++)
        {
            str[t + 1] = ((num / myPow(10, intLen - t - 1)) % 10) + '0';

            /* ��λ����ʾ && ��λΪ 0 δ������� && ��ǰλΪ 0 && �����������λ���� 0 ����, ��ֹ 0 û����ʾ */
            if (zero == 0 && flag == 0 && str[t + 1] == '0' && (t < intLen - 1))
            {
                str[t + 1] = ' ';
                zeroNum++;
            }
            else
            {
                flag = 1;   /* ��Ǹ�λΪ 0 �ĸ�������� */
            }
        }

        if (num != 0)       /* ��ֵΪ 0 ����ʾ���� */
        {
            str[zeroNum] = '-';
        }
    }
}

/**
  * @brief      ������ת�ַ���.
  * @param      num     ��ֵ
  * @param      str     ת������ַ���
  * @param      intLen  ��������
  * @param      decLen  С������
  * @param      zero    0,��λΪ 0 ʱ����ʾ; 1,��λΪ 0 ʱ��ʾ
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

    /* ���ӷ���λ */
    str[0] = ' ';

    /* ����С��λ */
    if (decLen != 0)
    {
        str[intLen + 1] = '.';
    }

    for (t = 0; t < intLen; t++)
    {
        str[t + 1] = ((integer_num / myPow(10, intLen - t - 1)) % 10) + '0';

        /* ��λ����ʾ && ��λΪ 0 δ������� && ��ǰλΪ 0 && �����������λ���� 0 ����, ��ֹ 0 û����ʾ */
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
            flag = 1; /* ��Ǹ�λΪ 0 �ĸ�������� */
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
  * @brief      ��ָ��λ��������������������.
  * @param[in]  x       ���������λ��.
  * @param[in]  y       ���������λ��
  * @param[in]  num     ����������
  * @param[in]  len     ����ʾ��������󳤶�
  * @param[in]  zero    ���λΪ0�Ƿ���ʾ, 0-����ʾ 1-��ʾ
  * @param[in]  size    �����ı���С
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
  * @brief      ��ָ��λ�����ø�������������.
  * @param[in]  x       ���������λ��.
  * @param[in]  y       ���������λ��
  * @param[in]  num     ����������
  * @param[in]  intLen  ����ʾ����������������󳤶�
  * @param[in]  decLen  ����ʾ��С������������󳤶�
  * @param[in]  zero    ���λΪ0�Ƿ���ʾ, 0-����ʾ 1-��ʾ
  * @param[in]  size    �����ı���С
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
  * @brief      ���͸�ʽ������� OLED ���(����).
  * @param[in]  x       ���������λ��.
  * @param[in]  y       ���������λ��
  * @param[in]  isMultiLine 0-��֧�ֶ�����ʾ 1-֧�ֶ�����ʾ
  * @param[in]  size    �ı���С
  * @param[in]  format ��ʽ���ַ���
  * @param[in]  ... ��ʽ���ַ����еĲ�����
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
  * @brief      ���͸�ʽ������� OLED ���.
  *             �߱��Զ�����, ͬʱ֧��ʶ��'\r'��'\n'
  * @param[in]  format ��ʽ���ַ���
  * @param[in]  ... ��ʽ���ַ����еĲ�����
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
  * @brief      ����.
  * @param[in]  sx  ���������
  * @param[in]  sy  ���������
  * @param[in]  ex  �������յ�
  * @param[in]  ey  �������յ�
  * @param[in]  color  ������ɫ.
  * @retval     None.
  */
static void DrawLine(oledsize_t sx, oledsize_t sy, oledsize_t ex, oledsize_t ey)
{
    uint16_t t;
    int32_t xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int32_t incx, incy, uRow, uCol;

    delta_x = (int32_t)ex - sx; //������������
    delta_y = (int32_t)ey - sy;
    uRow = sx;
    uCol = sy;

    if (delta_x > 0)
    {
        incx = 1;    //���õ�������
    }
    else if (delta_x == 0)
    {
        incx = 0;    //��ֱ��
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
        incy = 0;    //ˮƽ��
    }
    else
    {
        incy = -1;
        delta_y = -delta_y;
    }

    if (delta_x > delta_y)
    {
        distance = delta_x;    //ѡȡ��������������
    }
    else
    {
        distance = delta_y;
    }

    for (t = 0; t <= distance + 1; t++) //�������
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
  * @brief      ����.
  *             ������ݴ�С��������
  * @param[in]  sx  ���������
  * @param[in]  sy  ���������
  * @param[in]  ex  �������յ�
  * @param[in]  ey  �������յ�
  * @param[in]  size �����С.
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
  * @brief      ������.
  *             ������ݴ�С������������
  * @param[in]  sx  ���������
  * @param[in]  sy  ���������
  * @param[in]  width ���
  * @param[in]  high  �߶�
  * @param[in]  size �����С.
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
  * @brief      ������.
  *             ������ݴ�С������������
  * @param[in]  sx  ���������
  * @param[in]  sy  ���������
  * @param[in]  ex  ������Ե�
  * @param[in]  ey  ������Ե�
  * @param[in]  size �����С.
  * @retval     None.
  */
void OLED_DrawCircle(oledsize_t x, oledsize_t y, oledsize_t radius, uint8_t size)
{
    uint8_t i;
    oledsize_t tmpRadius = radius;
    int32_t a = 0;
    int32_t di = 3 - (radius << 1);       /* �ж��¸���λ�õı�־ */

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

            /* ʹ��Bresenham�㷨��Բ */
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
  * @brief      ��ͼ��.
  * @param[in]  sx  ���������
  * @param[in]  sy  ���������
  * @param[in]  ex  ������Ե�
  * @param[in]  ey  ������Ե�
  * @param[in]  pkszName  ͼ������
  * @param[in]  size ͼ�������С.
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



