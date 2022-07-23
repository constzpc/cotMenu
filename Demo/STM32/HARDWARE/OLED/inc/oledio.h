/**
  **********************************************************************************************************************
  * @file    oledio.h
  * @brief   ���ļ��ṩ OLED ����Ӳ���ӿ����к���ԭ��
  * @author  ������    any question please send mail to const_zpc@163.com
  * @version V1.0.0
  * @date    2021-3-12
  **********************************************************************************************************************
  *
  **********************************************************************************************************************
  */

/* Define to prevent recursive inclusion -----------------------------------------------------------------------------*/
#ifndef __OLEDIO_H
#define __OLEDIO_H

/* Includes ----------------------------------------------------------------------------------------------------------*/
#include <stdint.h>
#include "sys.h"

/* Exported macro ----------------------------------------------------------------------------------------------------*/

/* OLED �����ӿڶ��� */
#define OLED_IIC_INTERFACE      0 // iic �ӿ�
#define OLED_SPI_INTERFACE      1 // spi �ӿ�
#define _DRIVE_INTERFACE_TYPE   OLED_SPI_INTERFACE

/* Exported constants ------------------------------------------------------------------------------------------------*/
#if _DRIVE_INTERFACE_TYPE == OLED_IIC_INTERFACE
/*************************************************** OLED IIC �˿ڶ��� ************************************************/
#define OLED_SCLK_Clr() GPIO_ResetBits(GPIOB,GPIO_Pin_3)//SCL IIC�ӿڵ�ʱ���ź�
#define OLED_SCLK_Set() GPIO_SetBits(GPIOB,GPIO_Pin_3)

#define OLED_SDIN_Clr() GPIO_ResetBits(GPIOB,GPIO_Pin_5)//SDA IIC�ӿڵ������ź�
#define OLED_SDIN_Set() GPIO_SetBits(GPIOB,GPIO_Pin_5)
/*************************************************** OLED IIC �˿ڶ��� ************************************************/
#else
/*************************************************** OLED SPI �˿ڶ��� ************************************************/
#define  OLED_DC_Clr()  GPIO_ResetBits(GPIOC, GPIO_Pin_2)
#define  OLED_DC_Set()  GPIO_SetBits(GPIOC, GPIO_Pin_2)

#define  OLED_RST_Clr()  GPIO_ResetBits(GPIOC, GPIO_Pin_1)
#define  OLED_RST_Set()  GPIO_SetBits(GPIOC, GPIO_Pin_1)

#define  OLED_DIN_Clr()  GPIO_ResetBits(GPIOC, GPIO_Pin_0)
#define  OLED_DIN_Set()  GPIO_SetBits(GPIOC, GPIO_Pin_0)

#define  OLED_CLK_Clr()  GPIO_ResetBits(GPIOC, GPIO_Pin_3)
#define  OLED_CLK_Set()  GPIO_SetBits(GPIOC, GPIO_Pin_3)
/*************************************************** OLED SPI �˿ڶ��� ************************************************/
#endif

/* Exported functions ------------------------------------------------------------------------------------------------*/

/* Ӳ����ʼ������ *****************************************************************************************************/
extern void OledDrv_Init(void);

/* IIC ͨ�Žӿں��� ***************************************************************************************************/
extern void OledDrv_IICStart(void);
extern void OledDrv_IICStop(void);
extern void OledDrv_IICWriteByte(uint8_t data);

/* SPI ͨ�Žӿں��� ***************************************************************************************************/
extern void OledDrv_SPIWriteByte(uint8_t data);
    
    
#endif // __OLEDIO_H
