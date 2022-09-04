/**
  **********************************************************************************************************************
  * @file    cTask.h
  * @brief   该文件提供任务时间片论功能函数原型
  * @author  const_zpc    any question please send mail to const_zpc@163.com
  * @date    2022-04-16
  **********************************************************************************************************************
  *
  **********************************************************************************************************************
  */

/* Define to prevent recursive inclusion -----------------------------------------------------------------------------*/
#ifndef __C_TASH_H
#define __C_TASH_H

/* Includes ----------------------------------------------------------------------------------------------------------*/
#include <stdint.h>

#ifndef NULL
#define NULL   (void *)0
#endif

/* Exported types ----------------------------------------------------------------------------------------------------*/

typedef void (*TaskRunCB)(void);

/* Exported constants ------------------------------------------------------------------------------------------------*/
/* Exported macro ----------------------------------------------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------------------------------------------------*/

extern void cTASK_Init(void);
extern void cTASK_Remarks(void);
extern void cTASK_Start(void);

extern void cTASK_Creat(TaskRunCB pfnTask, uint16_t time, uint8_t priority);
extern void cTASK_Destory(TaskRunCB pfnTask);

extern void cTASK_IsrCreat(TaskRunCB pfnTask, uint16_t time);
extern void cTASK_IsrDestory(TaskRunCB pfnTask);

extern uint8_t cTASK_GetLoadRate(void);
extern uint32_t cTASK_GetSysTimer(void);

#endif // !__C_TASH_H
