/**
  **********************************************************************************************************************
  * @file    oledio.c
  * @brief   该文件提供 OLED 驱动硬件接口相关功能
  * @author  周鹏程    any question please send mail to const_zpc@163.com
  * @version V1.0.0
  * @date    2021-3-12
  *
  * @details  功能详细说明：
  *           + 接口驱动通信接口定义
  *           + 驱动初始化函数
  *           + 驱动通信函数
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
  * @brief      硬件底层初始化.
  * @retval     None.
  */
void OledDrv_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    /* 使能端口复用时钟 GPIOA GPIOB 时钟*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    //GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); //失能JTAG

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		//推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    OLED_DC_Clr();
    OLED_RST_Set();
}

#if _DRIVE_INTERFACE_TYPE == OLED_IIC_INTERFACE

/**
  * @brief      IIC 通信开始.
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
  * @brief      IIC 通信停止.
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
  * @brief      IIC 写以一个字节.
  * @param[in]  data  字节数据.
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
  * @brief      SPI 写以一个字节.
  * @param[in]  data  字节数据.
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

