#include "sys.h"
#include <string.h>

#define SYSTEM_TIME     (uint32_t)(1000000)

static uint8_t  sg_facUs = 0;
static timeval_t sg_timeval;

void WFI_SET(void)
{
	__ASM volatile("wfi");		  
}

/**
  * @brief  	关闭所有中断
  * @return 	None
  */
void INTX_DISABLE(void)
{		  
	__ASM volatile("cpsid i");
}

/**
  * @brief  	开启所有中断
  * @return 	None
  */
void INTX_ENABLE(void)
{
	__ASM volatile("cpsie i");		  
}

/**
  * @brief  	设置栈顶地址
  * @param[in] 	addr 栈顶地址
  * @return 	None
  */
__asm void MSR_MSP(u32 addr) 
{
    MSR MSP, r0 			//set Main Stack value
    BX r14
}

/**
  * @brief  	系统滴答时钟初始化
  * @param[in] 	nus 延时的us数
  * @return 	None
  */
void SystemTickInit(void)
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
	sg_facUs = SystemCoreClock / 8000000;
    
	SysTick->LOAD = SYSTEM_TIME * sg_facUs;	 
	SysTick->VAL = 0x00;
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk ;
    
    memset(&sg_timeval, 0, sizeof(timeval_t));
}

/**
  * @brief  	系统滴答时钟更新任务，用来同步数据
  * @note       2 秒内任务
  * @param[in] 	nus 延时的us数
  * @return 	None
  */
void SystemTickTask(void)
{		
	uint32_t temp;	
    
    temp = SysTick->LOAD - SysTick->VAL;
    SysTick->VAL = 0x00;
    
    sg_timeval.tv_usec += temp / sg_facUs;
    sg_timeval.tv_msec = (sg_timeval.tv_usec / 1000);
    sg_timeval.tv_sec = (sg_timeval.tv_msec / 1000);
}

/**
  * @brief  	读取nus
  * @param[out] pTime 系统时间
  * @return 	None
  */
void ReadSystemTime(timeval_t *pTime)
{		
	SystemTickTask();
    *pTime = sg_timeval;
}

/**
  * @brief  	微妙延时
  * @param[in] 	nus 延时的us数
  * @return 	None
  */
void DelayUs(uint32_t nus)
{		
    timeval_t lastTimeval = {0};
    timeval_t timeval = {0};

    ReadSystemTime(&lastTimeval);
    
    do
    {
        ReadSystemTime(&timeval);
    }while ((timeval.tv_usec - lastTimeval.tv_usec) < nus);
}

/**
  * @brief  	毫秒延时
  * @param[in] 	nms 延时的ms数
  * @return 	None
  */
void DelayMs(uint16_t nms)
{	 		  	  
    timeval_t lastTimeval = {0};
    timeval_t timeval = {0};

    ReadSystemTime(&lastTimeval);
    
    do
    {
        ReadSystemTime(&timeval);
    }while ((timeval.tv_msec - lastTimeval.tv_msec) < nms);
} 
