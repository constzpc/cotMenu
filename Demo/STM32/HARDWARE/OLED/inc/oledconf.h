/**
  **********************************************************************************************************************
  * @file    oledconf.h
  * @brief   ���ļ��ṩ OLED �������ѡ��
  * @author  ������    any question please send mail to const_zpc@163.com
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
  * @brief  OLED �Ŀ�Ⱥ͸߶�
  */
#define OLED_WIDTH              128
#define OLED_HEIGHT             64

/**
  * @brief  BMP ͼ�ε���Ŀ.
  *         û�� BMP ͼ��������Ϊ 0
  */
#define OLED_GRAPHIC_NUM        4

/**
  * @brief  OLED ��ʽ�����������.
  *         ������Ҫ�ù���, ���ö���
  */
#define _USE_OLED_PRINTF

#ifdef _USE_OLED_PRINTF
/************************************************* Printf ������ڶ��� ************************************************/
#define OLED_PRINT_X            64
#define OLED_PRINT_Y            0

#define OLED_PRINT_WIDTH        (128 - OLED_PRINT_X)
#define OLED_PRINT_HIGH         (64 - OLED_PRINT_Y)

#define OLED_PRINT_FONT         FONT_12X12
/************************************************* Printf ������ڶ��� ************************************************/
#endif // _USE_OLED_PRINTF

/* Exported types ----------------------------------------------------------------------------------------------------*/
/**
  * @brief �����Сö�ٶ���
  * @note  ö��ֵ�� sg_tFontCfgTable ����ֵ��Ӧ
  *        ö��ֵ�� 0 ��ʼ
  */
typedef enum
{
    FONT_12X12 = 0,                         /*!< �ֳ����: 12 x 12, Ӣ�ĳ���� 6 x 12 */
    FONT_16X16,                             /*!< �ֳ����: 16 x 16, Ӣ�ĳ���� 8 x 16 */
    FONT_24X24,                             /*!< �ֳ����: 24 x 24, Ӣ�ĳ���� 12 x 24 */
    FONT_32X32,                             /*!< �ֳ����: 32 x 32, Ӣ�ĳ���� 16 x 32 */

    FONT_MAX_NUM
} efontSize;

/**
  * @brief ����ȡģ��ʽö�ٶ���
  */
typedef enum
{
    FONT_MODE_COLUMN_BY_COLUMN = 0,         /*!< (0)����ʽ */
    FONT_MODE_PROGRESSIVE,                  /*!< (1)����ʽ */
    FONT_MODE_COLUMN_BY_ROW,                /*!< (2)����ʽ */
    FONT_MODE_DETERMINANT,                  /*!< (3)����ʽ */
} eMoldMode;

/**
  * @brief �������ýṹ�嶨��
  */
typedef struct
{
    eMoldMode way;                              /*!< ȡģ��ʽ, �ο�ö�� @enum eMoldMode */

    uint8_t charWidth;                          /*!< ASCII�ַ���� */

    uint8_t fontWidth;                          /*!< ���ֿ�� */

    uint8_t height;                             /*!< ASCII�ַ��߶�, �ͺ��ָ߶�һ�� */
    
    char charSIndex;                            /*!< ASCII�ַ����ݵ��ַ�����ʼƫ���ַ� */
    
    char charEIndex;                            /*!< ASCII�ַ����ݵ��ַ�������ƫ���ַ� */
    
    const uint8_t *pkCharBufData;               /*!< ASCII�ַ�����(ȡģ���ã����� + ����ʽ + ���� + 16���� + C51��ʽ) */

    const uint8_t *pkFontIdx;                   /*!< ������������, ��֧����Ϊ0 */

    const uint8_t *pkFontBufData;               /*!< ������������, ��֧����Ϊ0 */
} fontConfig_t;

/**
  * @brief ͼ�����ýṹ�嶨��
  */
typedef struct
{
    eMoldMode way;                              /*!< ȡģ��ʽ, �ο�ö�� @enum eMoldMode */
    
    uint8_t width;                              /*!< �ֿ�, ��ӦӢ�ĳ���Ϊ�ֿ��һ�� */

    uint8_t high;                               /*!< �ָ�, ��ӦӢ�ĸ߶Ⱥ��ָ�һ�� */

    const char *pkszName;					    /*!< ͼ�ε����� */

    const uint8_t *pkBufData;				    /*!< ͼ�ε����� */
} graphicConfig_t;

/* Exported constants ------------------------------------------------------------------------------------------------*/
/* Exported macro ----------------------------------------------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------------------------------------------------*/


extern int OLED_GetFontConfig(efontSize eSize, fontConfig_t *pInfo);
extern int OLED_GetGraphicConfig(const char *pkszName, graphicConfig_t *pInfo);

#endif // __OLEDCONF_H
