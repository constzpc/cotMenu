/**
  **********************************************************************************************************************
  * @file    keyio.c
  * @brief   该文件提供按键硬件接口实现功能
  * @author  const_zpc  any question please send mail to const_zpc@163.com
  * @version V3.0.0
  * @date    2022-03-27
  *
  *
  **********************************************************************************************************************
  * 
  *  移植时在函数 InitKeyIo 中实现 IO 的初始化配置, 同时定义枚举 @reg IoKeyType_e, 为每个实体按键或虚拟按键（有些按键
  *  不一定是一个 IO 对应按键, 如矩形按键、红外遥控按键等）定义一个枚举值（从 0 开始），然后在函数 IoKey_Scan 中得到每
  *  个实体按键或虚拟按键按下时的状态，并和 IO_KEY_ON 绑定
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
 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOE,ENABLE);//使能PORTA,PORTE时钟

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_3|GPIO_Pin_4;//KEY0-KEY1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
 	GPIO_Init(GPIOE, &GPIO_InitStructure);//初始化GPIOE2,3,4

	//初始化 WK_UP-->GPIOA.0	  下拉输入
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //PA0设置成输入，默认下拉	  
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.0
}

/**
  * @brief      按键驱动初始化
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
  * @brief      读取按键 IO 状态
  *
  * @param[in]  eKey 指定按键, 取值为 @reg IoKeyType_e
  * @retval     按键 IO 状态, 取值为 @reg IoKeyState_e.
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
  * @brief      按键 IO 扫描任务
  *
  */
void IoKey_Scan(void)
{
	sg_arrKeyState[IO_KEY_0] = (IoKeyState_e)!GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4);//读取按键0
    sg_arrKeyState[IO_KEY_1] = (IoKeyState_e)!GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3);//读取按键0
    sg_arrKeyState[IO_KEY_UP] = (IoKeyState_e)GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0);//读取按键0
}
