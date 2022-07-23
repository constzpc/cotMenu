/**
  **********************************************************************************************************************
  * @file    oledio.c
  * @brief   ���ļ��ṩ OLED ����Ӳ���ӿ���ع���
  * @author  ������    any question please send mail to const_zpc@163.com
  * @version V1.0.0
  * @date    2021-3-12
  *
  * @details  ������ϸ˵����
  *           + �ӿ�����ͨ�Žӿڶ���
  *           + ������ʼ������
  *           + ����ͨ�ź���
  *
  **********************************************************************************************************************
  * 
  *
  **********************************************************************************************************************
  */

/* Includes ----------------------------------------------------------------------------------------------------------*/
#include "oledio.h"

/* Private typedef ---------------------------------------------------------------------------------------------------*/
/* Private define ----------------------------------------------------------------------------------------------------*/
/* Private macro -----------------------------------------------------------------------------------------------------*/
/* Private variables -------------------------------------------------------------------------------------------------*/
/* Private function prototypes ---------------------------------------------------------------------------------------*/
/* Private function --------------------------------------------------------------------------------------------------*/

/**
  * @brief      Ӳ���ײ��ʼ��.
  * @retval     None.
  */
void OledDrv_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    /* ʹ�ܶ˿ڸ���ʱ�� GPIOA GPIOB ʱ��*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    //GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); //ʧ��JTAG

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		//�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    OLED_DC_Clr();
    OLED_RST_Set();
}

#if _DRIVE_INTERFACE_TYPE == OLED_IIC_INTERFACE

/**
  * @brief      IIC ͨ�ſ�ʼ.
  * @retval     None.
  */
void OledDrv_IICStart(void)
{
    OLED_SCLK_Set() ;
    OLED_SDIN_Set();
    OLED_SDIN_Clr();
    OLED_SCLK_Clr();
}

/**
  * @brief      IIC ͨ��ֹͣ.
  * @retval     None.
  */
void OledDrv_IICStop(void)
{
    OLED_SCLK_Clr();
    OLED_SDIN_Clr();
    OLED_SDIN_Set();
    OLED_SCLK_Set() ;
}

/**
  * @brief      IIC д��һ���ֽ�.
  * @param[in]  data  �ֽ�����.
  * @retval     None.
  */
void OledDrv_IICWriteByte(uint8_t data)
{
    char i = 8;

    while (i--)
    {
        OLED_SCLK_Clr();

        if (data & 0x80)
        {
            OLED_SDIN_Set();
        }
        else
        {
            OLED_SDIN_Clr();
        }

        OLED_SCLK_Set();
        data <<= 1;
    }

    OLED_SCLK_Clr();
    OLED_SCLK_Set();
}

#else

/**
  * @brief      SPI д��һ���ֽ�.
  * @param[in]  data  �ֽ�����.
  * @retval     None.
  */
void OledDrv_SPIWriteByte(uint8_t data)
{
    char i = 8;

    OLED_CLK_Clr();

    while (i--)
    {
        if (data & 0x80)
        {
            OLED_DIN_Set();
        }
        else
        {
            OLED_DIN_Clr();
        }

        OLED_CLK_Set();
        OLED_CLK_Clr();
        data <<= 1;
    }
}

#endif

