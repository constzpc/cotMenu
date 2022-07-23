/**
  **********************************************************************************************************************
  * @file      usart.h
  * @brief     串口
  * @author    
  * @date      2020-12-2
  *
  **********************************************************************************************************************
  *
  **********************************************************************************************************************
  */

/* Define to prevent recursive inclusion -----------------------------------------------------------------------------*/
#ifndef USART_H_
#define USART_H_

/* Includes ----------------------------------------------------------------------------------------------------------*/
#include <stdint.h>

#ifndef NULL
#define NULL  0
#endif

/* Exported types ----------------------------------------------------------------------------------------------------*/

typedef void (*UsartFun)(const void *, uint16_t);

/* Exported constants ------------------------------------------------------------------------------------------------*/

/* 串口驱动定义 */
#define USART_DEBUG                         0  // PA9_TX  PA10_RX
#define USART_ESP8266                       1  // PB10_TX  PB11_RX

/* Exported macro ----------------------------------------------------------------------------------------------------*/

#define _USER_WIFI_UASRT_DEBUG

#ifdef _USER_WIFI_UASRT_DEBUG
#define DEBUG_PRINTF(format, ...) USART_Printf(USART_DEBUG, format, ##__VA_ARGS__)
#else
#define DEBUG_PRINTF(format, ...)
#endif

/* Exported variables ------------------------------------------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------------------------------------------------*/

/* 初始化函数 *********************************************************************************************************/
extern void     FML_USART_Init(void);

/* 毫秒计时函数 *******************************************************************************************************/
extern void     FML_USART_MillisecondTimer(void);

/* 相关函数 ***********************************************************************************************************/
extern int      FML_USART_Transmit(uint8_t drv, const void *data, uint32_t len);
extern int      FML_USART_Register(uint8_t drv, const UsartFun pUsartFun);
extern void     FML_USART_UnRegister(uint8_t drv, const UsartFun pUsartFun);
extern void     FML_USART_RecvTask(void);

extern void     USART_Printf(uint8_t drv, const char *fmt, ...);

#endif /* USART_H_ */
