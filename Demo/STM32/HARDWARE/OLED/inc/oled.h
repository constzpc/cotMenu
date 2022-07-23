/**
  **********************************************************************************************************************
  * @file    oled.h
  * @brief   该文件提供OLED驱动所有函数原型
  * @author  周鹏程    any question please send mail to const_zpc@163.com
  * @version V1.1.0
  * @date    2021-3-30
  **********************************************************************************************************************
  *
  **********************************************************************************************************************
  */

/* Define to prevent recursive inclusion -----------------------------------------------------------------------------*/
#ifndef __OLED_H
#define __OLED_H

/* Includes ----------------------------------------------------------------------------------------------------------*/
#include "oledconf.h"
#include <stdint.h>

/* Exported types ----------------------------------------------------------------------------------------------------*/

typedef unsigned char oledsize_t;

/**
  * @brief 画笔颜色
  */
typedef enum
{
    OLED_BLACK = 0,
    OLED_WHITE = 0xff,
} eOledcolor;

/* Exported constants ------------------------------------------------------------------------------------------------*/
/* Exported macro ----------------------------------------------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------------------------------------------------*/

/* OLED 初始化函数 ****************************************************************************************************/
extern void OLED_Init(void);

/* OLED 控制函数 ******************************************************************************************************/
extern void OLED_DisplayOn(void);
extern void OLED_DisplayOff(void);

/* 画点/读点函数 ******************************************************************************************************/
extern void OLED_DrawPoint(oledsize_t x, oledsize_t y, eOledcolor color);
extern void OLED_DrawRoughPoint(oledsize_t x, oledsize_t y, eOledcolor color, uint8_t size);
extern eOledcolor OLED_ReadPoint(oledsize_t x, oledsize_t y);

/* OLED 清屏函数 ******************************************************************************************************/
extern void OLED_Clear(uint8_t color);
extern void OLED_SetFill(oledsize_t sx, oledsize_t sy, oledsize_t width, oledsize_t high, eOledcolor color);

/* 画面操作函数 *******************************************************************************************************/
extern void OLED_SyncScreen(oledsize_t sx, oledsize_t sy, oledsize_t width, oledsize_t high);
extern void OLED_ShowTask(void);
extern void OLED_SetScreenOffset(oledsize_t x, oledsize_t y, oledsize_t width, oledsize_t high, uint8_t dir, uint8_t pixels);
extern void OLED_ReverseScreen(oledsize_t sx, oledsize_t sy, oledsize_t width, oledsize_t high);

/* 画图形函数 *********************************************************************************************************/
extern void OLED_DrawLine(oledsize_t sx, oledsize_t sy, oledsize_t ex, oledsize_t ey, uint8_t size);
extern void OLED_DrawRectangle(oledsize_t sx, oledsize_t sy, oledsize_t width, oledsize_t high, uint8_t size);
extern void OLED_DrawCircle(oledsize_t x, oledsize_t y, oledsize_t radius, uint8_t size);
extern void OLED_DrawGraphic(oledsize_t x, oledsize_t y, const char *pkszName, uint8_t size);

/* 设置文本/数字函数 **************************************************************************************************/
extern void OLED_SetColor(eOledcolor backColor, eOledcolor pointColor);
extern void OLED_SetText(oledsize_t x, oledsize_t y, const char *pszStr, uint8_t isMultiLine, efontSize size);
extern void OLED_SetIntegerNum(oledsize_t x, oledsize_t y, int32_t num, uint8_t len, uint8_t zero, efontSize size);
extern void OLED_SetFloatNum(oledsize_t x, oledsize_t y, float num, uint8_t intLen, uint8_t decLen, uint8_t zero, efontSize size);

/* 格式化输出函数 *****************************************************************************************************/
extern void OLED_XYPrintf(oledsize_t x, oledsize_t y, uint8_t isMultiLine, efontSize size, const char *format, ...);
extern void OLED_Printf(const char *format, ...);

#endif  // __OLED_H




