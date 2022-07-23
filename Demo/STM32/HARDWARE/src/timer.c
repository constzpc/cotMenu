/**
  ******************************************************************************
  * @file    timer.c
  * @author  const-zpc
  * @date    2020-12-1
  * @brief   ���ļ��ṩ����ģ�������ܣ��Թ��� TIMER ���������¹��ܣ�
  *           + timer�ײ���������������
  *           + timer���жϷ�����
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
  * @brief      PIT CH1 �ĳ�ʼ������
  * @note       ����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
  * @param[in]  arr �Զ���װֵ
  * @param[in]  psc ʱ��Ԥ��Ƶ��
  * @retval     None
  */
void TIME_Init(uint16_t arr, uint16_t psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    //��ʱ��TIM3��ʼ��
    TIM_TimeBaseStructure.TIM_Period = arr;
    TIM_TimeBaseStructure.TIM_Prescaler = psc;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

    //�ж����ȼ�NVIC����
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);


    TIM_Cmd(TIM3, ENABLE);
}

/**
  * @brief  PIT CH1 �ĳ�ʼ������.
  * @retval None
  */
void FML_TIME_Init(void)
{
    //100Khz�ļ���Ƶ�ʣ�������100Ϊ1ms
    TIME_Init(99, 719);
}

/**
  * @brief  ��ʱ��3�жϷ������
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

