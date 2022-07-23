/**
  ******************************************************************************
  * @file    timer.c
  * @author  const-zpc
  * @date    2020-12-1
  * @brief   该文件提供功能模块层管理功能，以管理 TIMER 驱动的以下功能：
  *           + timer底层驱动，引脚配置
  *           + timer的中断服务函数
  *
  *
  ******************************************************************************
  * @attention
  *
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "timer.h"
#include "sys.h"
#include <string.h>
#include "cTask.h"

/* Private typedef -----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private function ----------------------------------------------------------*/

/**
  * @brief      PIT CH1 的初始化启动
  * @note       这里时钟选择为APB1的2倍，而APB1为36M
  * @param[in]  arr 自动重装值
  * @param[in]  psc 时钟预分频数
  * @retval     None
  */
void TIME_Init(uint16_t arr, uint16_t psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    //定时器TIM3初始化
    TIM_TimeBaseStructure.TIM_Period = arr;
    TIM_TimeBaseStructure.TIM_Prescaler = psc;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

    //中断优先级NVIC设置
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);


    TIM_Cmd(TIM3, ENABLE);
}

/**
  * @brief  PIT CH1 的初始化启动.
  * @retval None
  */
void FML_TIME_Init(void)
{
    //100Khz的计数频率，计数到100为1ms
    TIME_Init(99, 719);
}

/**
  * @brief  定时器3中断服务程序
  * @retval None
  */
void TIM3_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);

        cTASK_Remarks();
    }
}

