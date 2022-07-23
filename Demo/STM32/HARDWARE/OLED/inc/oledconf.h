/**
  **********************************************************************************************************************
  * @file    oledconf.h
  * @brief   该文件提供 OLED 相关配置选项
  * @author  周鹏程    any question please send mail to const_zpc@163.com
  * @version V1.1.0
  * @date    2021-3-30
  **********************************************************************************************************************
  *
  **********************************************************************************************************************
  */

/* Define to prevent recursive inclusion -----------------------------------------------------------------------------*/
#ifndef __OLEDCONF_H
#define __OLEDCONF_H

/* Includes ----------------------------------------------------------------------------------------------------------*/
#include <stdint.h>

/**
  * @brief  OLED 的宽度和高度
  */
#define OLED_WIDTH              128
#define OLED_HEIGHT             64

/**
  * @brief  BMP 图形的数目.
  *         没有 BMP 图形数据则为 0
  */
#define OLED_GRAPHIC_NUM        4

/**
  * @brief  OLED 的式样化输出功能.
  *         若不需要该功能, 则不用定义
  */
#define _USE_OLED_PRINTF

#ifdef _USE_OLED_PRINTF
/************************************************* Printf 输出窗口定义 ************************************************/
#define OLED_PRINT_X            64
#define OLED_PRINT_Y            0

#define OLED_PRINT_WIDTH        (128 - OLED_PRINT_X)
#define OLED_PRINT_HIGH         (64 - OLED_PRINT_Y)

#define OLED_PRINT_FONT         FONT_12X12
/************************************************* Printf 输出窗口定义 ************************************************/
#endif // _USE_OLED_PRINTF

/* Exported types ----------------------------------------------------------------------------------------------------*/
/**
  * @brief 字体大小枚举定义
  * @note  枚举值和 sg_tFontCfgTable 索引值对应
  *        枚举值从 0 开始
  */
typedef enum
{
    FONT_12X12 = 0,                         /*!< 字长宽比: 12 x 12, 英文长宽比 6 x 12 */
    FONT_16X16,                             /*!< 字长宽比: 16 x 16, 英文长宽比 8 x 16 */
    FONT_24X24,                             /*!< 字长宽比: 24 x 24, 英文长宽比 12 x 24 */
    FONT_32X32,                             /*!< 字长宽比: 32 x 32, 英文长宽比 16 x 32 */

    FONT_MAX_NUM
} efontSize;

/**
  * @brief 字体取模方式枚举定义
  */
typedef enum
{
    FONT_MODE_COLUMN_BY_COLUMN = 0,         /*!< (0)逐列式 */
    FONT_MODE_PROGRESSIVE,                  /*!< (1)逐行式 */
    FONT_MODE_COLUMN_BY_ROW,                /*!< (2)列行式 */
    FONT_MODE_DETERMINANT,                  /*!< (3)行列式 */
} eMoldMode;

/**
  * @brief 字体配置结构体定义
  */
typedef struct
{
    eMoldMode way;                              /*!< 取模方式, 参考枚举 @enum eMoldMode */

    uint8_t charWidth;                          /*!< ASCII字符宽度 */

    uint8_t fontWidth;                          /*!< 汉字宽度 */

    uint8_t height;                             /*!< ASCII字符高度, 和汉字高度一致 */
    
    char charSIndex;                            /*!< ASCII字符数据的字符集开始偏移字符 */
    
    char charEIndex;                            /*!< ASCII字符数据的字符集结束偏移字符 */
    
    const uint8_t *pkCharBufData;               /*!< ASCII字符数据(取模设置：阴码 + 逐行式 + 逆向 + 16进制 + C51格式) */

    const uint8_t *pkFontIdx;                   /*!< 汉字字体索引, 不支持则为0 */

    const uint8_t *pkFontBufData;               /*!< 汉字字体数据, 不支持则为0 */
} fontConfig_t;

/**
  * @brief 图形配置结构体定义
  */
typedef struct
{
    eMoldMode way;                              /*!< 取模方式, 参考枚举 @enum eMoldMode */
    
    uint8_t width;                              /*!< 字宽, 对应英文长度为字宽的一半 */

    uint8_t high;                               /*!< 字高, 对应英文高度和字高一样 */

    const char *pkszName;					    /*!< 图形的名字 */

    const uint8_t *pkBufData;				    /*!< 图形的数据 */
} graphicConfig_t;

/* Exported constants ------------------------------------------------------------------------------------------------*/
/* Exported macro ----------------------------------------------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------------------------------------------------*/


extern int OLED_GetFontConfig(efontSize eSize, fontConfig_t *pInfo);
extern int OLED_GetGraphicConfig(const char *pkszName, graphicConfig_t *pInfo);

#endif // __OLEDCONF_H
