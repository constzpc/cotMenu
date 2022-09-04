/**
  **********************************************************************************************************************
  * @file    cTask.c
  * @brief   ���ļ��ṩ����ʱ��Ƭ�۹���
  * @author  const_zpc  any question please send mail to const_zpc@163.com
  * @version V0.1.0
  * @date    2022-04-16
  *
  * @details  ������ϸ˵����
  *           + ������ȳ�ʼ��
  *           + ������ȹ�������
  *         
  **********************************************************************************************************************
  * Դ��·����https://gitee.com/const-zpc/cTask.git �������⼰������ڸ���ַ��д Issue
  *
  * ʹ�÷�ʽ:
  *    1��ʹ��ǰ��ʼ������ cTASK_Init
  *    2������������ cTASK_Start �����������, ���������˳�
  *    3��ʹ�� 1ms ��ʱ���жϵ��� cTASK_Remarks
  *    4������ sg_tTaskManage �������
  *
  * ע�����
  *    1��ÿ�����������ܱ�������������֤����ִ�е�ʱ��С�� 1ms���ҵ�������һ��Ҫ���ڵ��Ⱥ��������ʱ
  *
  **********************************************************************************************************************
  */

/* Includes ----------------------------------------------------------------------------------------------------------*/
#include "cTask.h"
#include "cTask_cfg.h"
#include <stdbool.h>

/* Private typedef ---------------------------------------------------------------------------------------------------*/

/**
  * @brief ��ͨ����������Ϣ�ṹ�嶨��
  */
typedef struct
{
    bool isUsed;                            /*!< �Ƿ�ʹ�� */
    
    bool isRunnig;                          /*!< �����������У�0-�����У�1���� */
    
    uint8_t priority;                       /*!< �������ȼ�, ֵԽС, ���ȼ�Խ�� */

    uint16_t timer;                         /*!< ��ʱ�� */

    uint16_t itvTime;                       /*!< �������м��ʱ�� */

    TaskRunCB pfnTaskHook;                  /*!< Ҫ���е������� */
} TaskInfo_t;

/**
  * @brief �ж�������Ϣ�ṹ�嶨��
  */
typedef struct
{
    uint16_t timer;                         /*!< ��ʱ�� */

    uint16_t itvTime;                       /*!< �������м��ʱ�� */

    TaskRunCB pfnTaskHook;                  /*!< Ҫ���е������� */
} IsrTaskInfo_t;

/**
  * @brief ������Ϣ�ṹ�嶨��
  */
typedef struct
{
#ifdef CTASK_USE_LOAD_RATE
    uint8_t  mcuLoadRate;                   /*!< mcuLoadRate ������ 0-100% */
    
    uint16_t idleTaskExpectedCnt;           /*!< ���������������д��� */
    
    uint16_t idleTaskActualCnt;             /*!< ��������ʵ�����д��� */
#endif
    bool isStart;                           /*!< ����ϵͳ�Ƿ����� */
    
    uint32_t systimer;                      /*!< ϵͳ��ʱ�� */
    
    TaskInfo_t task[CTASK_SUPPORT_NUM];     /*!< ��ͨ������ѭ������ */
    
    IsrTaskInfo_t isrTask[CTASK_ISR_SUPPORT_NUM];/*!< �ж������жϵ��� */
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
  * @brief      ��������
  *
  */
static void __IdleTask(void)
{
    sg_tTaskManage.idleTaskActualCnt++;
}

/**
  * @brief      �����ж�����
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
  * @brief      ������ȳ�ʼ��
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
  * @brief      �����µ��������
  *
  * @note       ���������ܱ�������������֤����ִ�е�ʱ��С�� 1ms���ҵ�������һ��Ҫ���ڵ��Ⱥ��������ʱ
  * @param[in]  pfnTask �ص�������
  * @param[in]  time    ��������, ��λms.
  * @param[in]  priority �������ȼ� 5-250, ����Ԥ��, ֵԽС, ���ȼ�Խ��
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
  * @brief      ��������
  *
  * @param[in]  pfnTask �ص�������
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
  * @brief      �����µ��ж��������
  *
  * @note       ���ڲ��õ����жϷ�ʽ, �ж����������ܱ����������뱣֤�����ʱ����; ������Ҫ�󼫸��Һ�ʱ���̵�����
  * @param[in]  pfnTask �ص�������
  * @param[in]  time    ��������, ��λms.
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
  * @brief      �����ж�����
  *
  * @param[in]  pfnTask �ص�������
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
  * @brief      ִ��������Ⱥ���
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
  * @brief ��������.
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
  * @brief �������״̬����.
  *
  * @note  ��Ҫ 1ms ��ʱ���жϵ���
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
  * @brief      ��ȡ MCU ������
  * 
  * @note       ͨ���ú����õ� MCU ������, ������Խ��, ʵʱ�Ժ���Ӧ��Խ��
  * @retval     MCU ������.0-100%
  */
uint8_t cTASK_GetLoadRate(void)
{
    return sg_tTaskManage.mcuLoadRate;
}
#endif

/**
  * @brief      ��ȡϵͳ��ʱ
  * 
  * @note       ���������ſ�ʼ��ʱ
  * @retval     ϵͳ��ʱ, ��λms
  */
uint32_t cTASK_GetSysTimer(void)
{
    return sg_tTaskManage.systimer;
}

