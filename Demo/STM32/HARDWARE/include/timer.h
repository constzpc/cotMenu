/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TIMER_H
#define __TIMER_H

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Exported types ------------------------------------------------------------*/

#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE  0
#endif

#ifndef NULL
#define NULL  0
#endif

/*!
  * @brief 定时器时标信息定义
  *
  */
typedef struct
{
    uint8_t bTic1msSign ;          /*!< 1ms定时标志 */
    uint8_t bTic10msSign ;         /*!< 10ms定时标志 */
    uint8_t bTic20msSign ;         /*!< 20ms定时标志 */
    uint8_t bTic100msSign ;        /*!< 100ms定时标志 */
    uint8_t bTic500msSign ;        /*!< 500ms定时标志 */
    uint8_t bTic1secSign;          /*!< 1s定时标志 */
}Tim_TimeSignType;

typedef void (*TimeFun)(void);

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
extern Tim_TimeSignType g_tTimeSign;

/* Exported functions --------------------------------------------------------*/

/* 初始化函数 ******************************************************************/
extern void FML_TIME_Init(void);

/* 定时处理函数 ****************************************************************/
extern int FML_TIME_Register(TimeFun pTimeFun, uint32_t time);
extern void FML_TIME_UnRegister(TimeFun pTimeFun);

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */


#endif



