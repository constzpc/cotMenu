/**
  **********************************************************************************************************************
  * @file    keyio.c
  * @brief   ���ļ��ṩ����Ӳ���ӿ�ʵ�ֹ���
  * @author  const_zpc  any question please send mail to const_zpc@163.com
  * @version V3.0.0
  * @date    2022-03-27
  *
  *
  **********************************************************************************************************************
  * 
  *  ��ֲʱ�ں��� InitKeyIo ��ʵ�� IO �ĳ�ʼ������, ͬʱ����ö�� @reg IoKeyType_e, Ϊÿ��ʵ�尴�������ⰴ������Щ����
  *  ��һ����һ�� IO ��Ӧ����, ����ΰ���������ң�ذ����ȣ�����һ��ö��ֵ���� 0 ��ʼ����Ȼ���ں��� IoKey_Scan �еõ�ÿ
  *  ��ʵ�尴�������ⰴ������ʱ��״̬������ IO_KEY_ON ��
  *  
  **********************************************************************************************************************
  */
  
/* Includes ----------------------------------------------------------------------------------------------------------*/
#include "keyio.h"
#include "stm32f10x.h"

/* Private typedef ---------------------------------------------------------------------------------------------------*/
/* Private define ----------------------------------------------------------------------------------------------------*/
/* Private macro -----------------------------------------------------------------------------------------------------*/
/* Private variables -------------------------------------------------------------------------------------------------*/

static IoKeyState_e sg_arrKeyState[IO_KEY_MAX_NUM];

/* Private function prototypes ---------------------------------------------------------------------------------------*/
static void InitKeyIo(void);
/* Private function --------------------------------------------------------------------------------------------------*/

static void InitKeyIo(void)
{ 
 	GPIO_InitTypeDef GPIO_InitStructure;
 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOE,ENABLE);//ʹ��PORTA,PORTEʱ��

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_3|GPIO_Pin_4;//KEY0-KEY1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //���ó���������
 	GPIO_Init(GPIOE, &GPIO_InitStructure);//��ʼ��GPIOE2,3,4

	//��ʼ�� WK_UP-->GPIOA.0	  ��������
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //PA0���ó����룬Ĭ������	  
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.0
}

/**
  * @brief      ����������ʼ��
  *
  */
void IoKey_Init(void)
{
    uint8_t i;
    
    for (i = 0; i < IO_KEY_MAX_NUM; i++)
    {
        sg_arrKeyState[i] = IO_KEY_OFF;
    }
    
    InitKeyIo();
}

/**
  * @brief      ��ȡ���� IO ״̬
  *
  * @param[in]  eKey ָ������, ȡֵΪ @reg IoKeyType_e
  * @retval     ���� IO ״̬, ȡֵΪ @reg IoKeyState_e.
  */
IoKeyState_e IoKey_Read(IoKeyType_e eKey)
{
    if (eKey < IO_KEY_MAX_NUM)
    {
        return sg_arrKeyState[eKey];
    }
    
    return IO_KEY_OFF;
}

/**
  * @brief      ���� IO ɨ������
  *
  */
void IoKey_Scan(void)
{
	sg_arrKeyState[IO_KEY_0] = (IoKeyState_e)!GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4);//��ȡ����0
    sg_arrKeyState[IO_KEY_1] = (IoKeyState_e)!GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3);//��ȡ����0
    sg_arrKeyState[IO_KEY_UP] = (IoKeyState_e)GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0);//��ȡ����0
}
