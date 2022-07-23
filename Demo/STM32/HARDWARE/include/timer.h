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
  * @brief ��ʱ��ʱ����Ϣ����
  *
  */
typedef struct
{
    uint8_t bTic1msSign ;          /*!< 1ms��ʱ��־ */
    uint8_t bTic10msSign ;         /*!< 10ms��ʱ��־ */
    uint8_t bTic20msSign ;         /*!< 20ms��ʱ��־ */
    uint8_t bTic100msSign ;        /*!< 100ms��ʱ��־ */
    uint8_t bTic500msSign ;        /*!< 500ms��ʱ��־ */
    uint8_t bTic1secSign;          /*!< 1s��ʱ��־ */
}Tim_TimeSignType;

typedef void (*TimeFun)(void);

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
extern Tim_TimeSignType g_tTimeSign;

/* Exported functions --------------------------------------------------------*/

/* ��ʼ������ ******************************************************************/
extern void FML_TIME_Init(void);

/* ��ʱ������ ****************************************************************/
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



