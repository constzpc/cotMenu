/**
  **********************************************************************************************************************
  * @file    cTask.c
  * @brief   该文件提供任务时间片论功能
  * @author  const_zpc  any question please send mail to const_zpc@163.com
  * @version V0.1.0
  * @date    2022-04-16
  *
  * @details  功能详细说明：
  *           + 任务调度初始化
  *           + 任务调度功能启动
  *         
  **********************************************************************************************************************
  * 源码路径：https://gitee.com/const-zpc/cTask.git 具体问题及建议可在该网址填写 Issue
  *
  * 使用方式:
  *    1、使用前初始化函数 cTASK_Init
  *    2、主函数调用 cTASK_Start 启动任务调度, 函数不会退出
  *    3、使用 1ms 定时器中断调用 cTASK_Remarks
  *    4、定义 sg_tTaskManage 添加任务
  *
  * 注意事项：
  *    1、每个任务函数不能被阻塞，尽量保证任务执行的时间小于 1ms，且调度周期一定要大于调度函数任务耗时
  *
  **********************************************************************************************************************
  */

/* Includes ----------------------------------------------------------------------------------------------------------*/
#include "cTask.h"
#include "cTask_cfg.h"
#include <stdbool.h>

/* Private typedef ---------------------------------------------------------------------------------------------------*/

/**
  * @brief 普通调度任务信息结构体定义
  */
typedef struct
{
    bool isUsed;                            /*!< 是否被使用 */
    
    bool isRunnig;                          /*!< 程序正在运行：0-不运行，1运行 */
    
    uint8_t priority;                       /*!< 任务优先级, 值越小, 优先级越高 */

    uint16_t timer;                         /*!< 计时器 */

    uint16_t itvTime;                       /*!< 任务运行间隔时间 */

    TaskRunCB pfnTaskHook;                  /*!< 要运行的任务函数 */
} TaskInfo_t;

/**
  * @brief 中断任务信息结构体定义
  */
typedef struct
{
    uint16_t timer;                         /*!< 计时器 */

    uint16_t itvTime;                       /*!< 任务运行间隔时间 */

    TaskRunCB pfnTaskHook;                  /*!< 要运行的任务函数 */
} IsrTaskInfo_t;

/**
  * @brief 任务信息结构体定义
  */
typedef struct
{
#ifdef CTASK_USE_LOAD_RATE
    uint8_t  mcuLoadRate;                   /*!< mcuLoadRate 负载率 0-100% */
    
    uint16_t idleTaskExpectedCnt;           /*!< 空闲任务理想运行次数 */
    
    uint16_t idleTaskActualCnt;             /*!< 空闲任务实际运行次数 */
#endif
    bool isStart;                           /*!< 调度系统是否启动 */
    
    uint32_t systimer;                      /*!< 系统计时器 */
    
    TaskInfo_t task[CTASK_SUPPORT_NUM];     /*!< 普通任务：主循环调用 */
    
    IsrTaskInfo_t isrTask[CTASK_ISR_SUPPORT_NUM];/*!< 中断任务：中断调用 */
} TaskManage_t;

/* Private define ----------------------------------------------------------------------------------------------------*/
/* Private macro -----------------------------------------------------------------------------------------------------*/
/* Private variables -------------------------------------------------------------------------------------------------*/

/*static*/ TaskManage_t sg_tTaskManage;

/* Private function prototypes ---------------------------------------------------------------------------------------*/
static void __IdleTask(void);
static void __IdleIsrTask(void);
/* Private function --------------------------------------------------------------------------------------------------*/

#ifdef CTASK_USE_LOAD_RATE
/**
  * @brief      空闲任务
  *
  */
static void __IdleTask(void)
{
    sg_tTaskManage.idleTaskActualCnt++;
}

/**
  * @brief      空闲中断任务
  *
  */
static void __IdleIsrTask(void)
{
    sg_tTaskManage.idleTaskExpectedCnt++;
    
    if (sg_tTaskManage.idleTaskExpectedCnt >= 1000)
    {
        sg_tTaskManage.mcuLoadRate = 100 - (sg_tTaskManage.idleTaskActualCnt * 100 / sg_tTaskManage.idleTaskExpectedCnt);
        sg_tTaskManage.idleTaskExpectedCnt = 0;
        sg_tTaskManage.idleTaskActualCnt = 0;
    }
}
#endif

/**
  * @brief      任务调度初始化
  *
  */
void cTASK_Init(void)
{
    int i;
    
    sg_tTaskManage.isStart = false;
    sg_tTaskManage.mcuLoadRate = 0;
    sg_tTaskManage.idleTaskActualCnt = 0;
    sg_tTaskManage.idleTaskExpectedCnt = 0;
    sg_tTaskManage.systimer = 0;
    
    for (i = 0; i < CTASK_SUPPORT_NUM; i++)
    {
        sg_tTaskManage.task[i].isUsed = false;
        sg_tTaskManage.task[i].isRunnig = false;
        sg_tTaskManage.task[i].itvTime = 0;
        sg_tTaskManage.task[i].pfnTaskHook = NULL;
        sg_tTaskManage.task[i].timer = 0;
        sg_tTaskManage.task[i].priority = 255;
    }

    for (i = 0; i < CTASK_ISR_SUPPORT_NUM; i++)
    {
        sg_tTaskManage.isrTask[i].itvTime = 0;
        sg_tTaskManage.isrTask[i].pfnTaskHook = NULL;
        sg_tTaskManage.isrTask[i].timer = 0;
    }
    
#ifdef CTASK_USE_LOAD_RATE
    cTASK_Creat(__IdleTask, 1, 255);
    cTASK_IsrCreat(__IdleIsrTask, 1);
#endif
}

/**
  * @brief      创建新的任务调度
  *
  * @note       任务函数不能被阻塞，尽量保证任务执行的时间小于 1ms，且调度周期一定要大于调度函数任务耗时
  * @param[in]  pfnTask 回调任务函数
  * @param[in]  time    调度周期, 单位ms.
  * @param[in]  priority 调度优先级 5-250, 其他预留, 值越小, 优先级越高
  */
void cTASK_Creat(TaskRunCB pfnTask, uint16_t time, uint8_t priority)
{
    int i, j;
    
    if (priority < 5)
    {
        priority = 5;
    }
    
    if (priority > 250)
    {
        priority = 250;
    }
    
    for (i = 0; i < CTASK_SUPPORT_NUM; i++)
    {
        if (sg_tTaskManage.task[i].priority < priority)
        {
            continue;
        }
        
        if (sg_tTaskManage.task[i].isUsed)
        {
            for (j = CTASK_SUPPORT_NUM - 1; j > i; j--)
            {
                sg_tTaskManage.task[j] = sg_tTaskManage.task[j - 1];
                sg_tTaskManage.task[j - 1].isUsed = false;
            }
        }
        
        sg_tTaskManage.task[i].isUsed = true;
        sg_tTaskManage.task[i].isRunnig = false;
        sg_tTaskManage.task[i].itvTime = time;
        sg_tTaskManage.task[i].pfnTaskHook = pfnTask;
        sg_tTaskManage.task[i].timer = time;
        sg_tTaskManage.task[i].priority = priority;
        break;
    }
}

/**
  * @brief      销毁任务
  *
  * @param[in]  pfnTask 回调任务函数
  */
void cTASK_Destory(TaskRunCB pfnTask)
{
    int i;
    
    for (i = 0; i < CTASK_SUPPORT_NUM; i++)
    {
        if (sg_tTaskManage.task[i].pfnTaskHook == pfnTask)
        {
            sg_tTaskManage.task[i].isUsed = false;
            break;
        }
    }
}

/**
  * @brief      创建新的中断任务调度
  *
  * @note       由于采用的是中断方式, 中断任务函数不能被阻塞，必须保证任务耗时极短; 适用于要求极高且耗时极短的任务
  * @param[in]  pfnTask 回调任务函数
  * @param[in]  time    调度周期, 单位ms.
  */
void cTASK_IsrCreat(TaskRunCB pfnTask, uint16_t time)
{
    int i;
    
    for (i = 0; i < CTASK_ISR_SUPPORT_NUM; i++)
    {
        if (sg_tTaskManage.isrTask[i].pfnTaskHook == NULL)
        {
            sg_tTaskManage.isrTask[i].itvTime = time;
            sg_tTaskManage.isrTask[i].pfnTaskHook = pfnTask;
            sg_tTaskManage.isrTask[i].timer = time;
            break;
        }
    }
}

/**
  * @brief      销毁中断任务
  *
  * @param[in]  pfnTask 回调任务函数
  */
void cTASK_IsrDestory(TaskRunCB pfnTask)
{
    int i, j;
    
    for (i = 0; i < CTASK_ISR_SUPPORT_NUM; i++)
    {
        if (sg_tTaskManage.isrTask[i].pfnTaskHook == pfnTask)
        {
            sg_tTaskManage.isrTask[i].pfnTaskHook = NULL;
            
            for (j = i; j < CTASK_ISR_SUPPORT_NUM - 1; j++)
            {
                sg_tTaskManage.isrTask[j] = sg_tTaskManage.isrTask[j + 1];
            }
            
            break;
        }
    }
}

/**
  * @brief      执行任务调度函数
  */
static void __TaskScheduling(void)
{
    uint8_t idx = 0;

    while (idx < CTASK_SUPPORT_NUM && sg_tTaskManage.task[idx].pfnTaskHook != NULL)
    {
        if (sg_tTaskManage.task[idx].isUsed && sg_tTaskManage.task[idx].isRunnig)
        {
            sg_tTaskManage.task[idx].pfnTaskHook();
            sg_tTaskManage.task[idx].isRunnig = false;
            break;
        }

        idx++;
    }
}

/**
  * @brief 任务启动.
  *
  */
void cTASK_Start(void)
{
    sg_tTaskManage.isStart = true;
    
    while (1)
    {
        __TaskScheduling();
    }
}

/**
  * @brief 任务调度状态管理.
  *
  * @note  需要 1ms 定时器中断调用
  */
void cTASK_Remarks(void)
{
    uint8_t idx = 0;
    
    if (!sg_tTaskManage.isStart)
    {
        return;
    }
    
    sg_tTaskManage.systimer++;

    while (idx < CTASK_SUPPORT_NUM && sg_tTaskManage.task[idx].pfnTaskHook != NULL)
    {
        if (sg_tTaskManage.task[idx].isUsed)
        {
            if (sg_tTaskManage.task[idx].timer)
            {
                sg_tTaskManage.task[idx].timer--;

                if (sg_tTaskManage.task[idx].timer == 0)
                {
                    sg_tTaskManage.task[idx].timer = sg_tTaskManage.task[idx].itvTime;
                    sg_tTaskManage.task[idx].isRunnig = true;
                }
            }      
        }

        idx++;
    }

    idx = 0;
    
    while (idx < CTASK_ISR_SUPPORT_NUM && sg_tTaskManage.isrTask[idx].pfnTaskHook != NULL)
    {
        if (sg_tTaskManage.isrTask[idx].timer)
        {
            sg_tTaskManage.isrTask[idx].timer--;

            if (sg_tTaskManage.isrTask[idx].timer == 0)
            {
                sg_tTaskManage.isrTask[idx].timer = sg_tTaskManage.isrTask[idx].itvTime;
                sg_tTaskManage.isrTask[idx].pfnTaskHook();
            }
        } 

        idx++;
    }
}

#ifdef CTASK_USE_LOAD_RATE
/**
  * @brief      获取 MCU 负载率
  * 
  * @note       通过该函数得到 MCU 负载率, 负载率越大, 实时性和响应性越差
  * @retval     MCU 负载率.0-100%
  */
uint8_t cTASK_GetLoadRate(void)
{
    return sg_tTaskManage.mcuLoadRate;
}
#endif

/**
  * @brief      获取系统计时
  * 
  * @note       需等启动后才开始计时
  * @retval     系统计时, 单位ms
  */
uint32_t cTASK_GetSysTimer(void)
{
    return sg_tTaskManage.systimer;
}

