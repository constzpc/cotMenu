/**
  **********************************************************************************************************************
  * @file    oledconf.c
  * @brief   ���ļ��ṩ OLED ������ù���
  * @author  ������    any question please send mail to const_zpc@163.com
  * @version V1.1.0
  * @date    2021-3-30
  *
  * @details  ������ϸ˵����
  *           + ��������
  *           + ʽ�����������
  *           + BMP ͼ������
  *
  **********************************************************************************************************************
  * 
  *
  **********************************************************************************************************************
  */

/* Includes ----------------------------------------------------------------------------------------------------------*/
#include "oledconf.h"
#include "bmp.h"
#include "font12x12.h"
#include "font16x16.h"
#include "font24x24.h"
#include "font32x32.h"
#include <string.h>

/* Private typedef ---------------------------------------------------------------------------------------------------*/
/* Private define ----------------------------------------------------------------------------------------------------*/
/* Private macro -----------------------------------------------------------------------------------------------------*/
/* Private variables -------------------------------------------------------------------------------------------------*/

/** �����С���ñ� */
static fontConfig_t sg_tFontCfgTable[FONT_MAX_NUM] =
{
    {FONT_MODE_COLUMN_BY_ROW, 6, 12, 12, ' ', '~', ASCII_6X12_DATA, FONT_12X12_IDX, FONT_12X12_DATA},
    {FONT_MODE_COLUMN_BY_ROW, 8, 16, 16, ' ', '~', ASCII_8X16_DATA, FONT_16X16_IDX, FONT_16X16_DATA},
    {FONT_MODE_COLUMN_BY_ROW, 12, 24, 24, ' ', '~', ASCII_12X24_DATA, FONT_24X24_IDX, FONT_24X24_DATA},
    {FONT_MODE_COLUMN_BY_ROW, 16, 32, 32, ' ', '~', ASCII_16X32_DATA, FONT_32X32_IDX, FONT_32X32_DATA},
};

#if OLED_GRAPHIC_NUM
/** ͼ�����ñ� */
static graphicConfig_t sg_tGraphicTable[OLED_GRAPHIC_NUM] =
{
    {FONT_MODE_COLUMN_BY_ROW, 48, 48, "music", MUSIC_BMP},
    {FONT_MODE_COLUMN_BY_ROW, 48, 48, "video", VIDEO_BMP},
    {FONT_MODE_COLUMN_BY_ROW, 48, 48, "camera", CAMERA_BMP},
    {FONT_MODE_COLUMN_BY_ROW, 48, 48, "setting", SETTING_BMP},
};
#endif

/* Private function prototypes ---------------------------------------------------------------------------------------*/
/* Private function --------------------------------------------------------------------------------------------------*/


/**
  * @brief      ��ȡ OLED ����������Ϣ����.
  * @param[in]  eSize  �����С
  * @param[out] pInfo  ��������Ϣ
  * @retval     0,��ȡ�ɹ�; -1,��ȡʧ��.
  */
int OLED_GetFontConfig(efontSize eSize, fontConfig_t *pInfo)
{
    if (eSize < FONT_MAX_NUM)
    {
        *pInfo = sg_tFontCfgTable[eSize];
        return 0;
    }
    
    return -1;
}

/**
  * @brief      ��ȡ OLED ͼ��������Ϣ����.
  * @param[in]  eSize  �����С
  * @param[out] pInfo  ��������Ϣ
  * @retval     0,��ȡ�ɹ�; -1,��ȡʧ��.
  */
int OLED_GetGraphicConfig(const char *pkszName, graphicConfig_t *pInfo)
{
    uint8_t i;

    for (i = 0; i < OLED_GRAPHIC_NUM; i++)
    {
        if (strcmp(pkszName, sg_tGraphicTable[i].pkszName) == 0)
        {
            *pInfo = sg_tGraphicTable[i];
            return 0;
        }
    }
    
    return -1;
}
