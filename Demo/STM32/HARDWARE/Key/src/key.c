/**
  **********************************************************************************************************************
  * @file    key.c
  * @brief   该文件提供按键动作识别功能
  * @author  const_zpc  any question please send mail to const_zpc@163.com
  * @version V3.0.0
  * @date    2022-03-27
  *
  * @details  功能详细说明：
  *           + 按键初始化
  *           + 按键动作监听
  *           + 按键动作信息获取
  *           + 按键轮询任务
  *
  **********************************************************************************************************************
  * 源码路径：https://gitee.com/const-zpc/key.git 具体问题及建议可在该网址填写 Issue
  *
  * 使用方式:
  *    1、使用前初始化函数 KEY_Init
  *    2、周期调用函数 KEY_Scan, 用来处理按键动作识别和处理
  *    3、使用其他函数对获取按键动作附加信息
  *
  **********************************************************************************************************************
  */

/* Includes ----------------------------------------------------------------------------------------------------------*/
#include "key.h"
#include "keyio.h"

/* Private typedef ---------------------------------------------------------------------------------------------------*/

/**
  * @brief 按键动作信息结构体定义
  */
typedef struct
{
    KeyAction_e eKeyAction;                     /*!< 指定所选按键的动作状态 */

    KeyAction_e eKeyActionBak;                  /*!< 指定所选按键的动作状态备份 */

    uint8_t clickCnt;                           /*!< 指定所选按键的单击次数 */

    uint16_t uiPressTic;                        /*!< 指定所选按键的按下累计时间, 该参数单位为1ms,该值初值为0 */

    uint16_t uiLossenTic;                       /*!< 指定所选按键的松开累计时间, 该参数单位为1ms,该值初值为0 */

    uint16_t uiAntiShakeTime;                   /*!< 指定所选按键的防抖时间, 该参数单位为1ms */
    
    uint16_t refreshNotifyTic;					/*!< 指定所选按键的触发回调通知函数的定时计时, 单位为1ms */

    KeyFunCB pfnKeyNotifyFun;                   /*!< 指定所选按键的动作改变回调通知函数 */
} KeyManage_t;

/* Private define ----------------------------------------------------------------------------------------------------*/

/** 按键防抖时间 */
#define KEY_ANTI_SHAKE_TIME      20

/* Private macro -----------------------------------------------------------------------------------------------------*/
/* Private variables -------------------------------------------------------------------------------------------------*/

static KeyManage_t sg_tKeyManage[IO_KEY_MAX_NUM];

/* Private function prototypes ---------------------------------------------------------------------------------------*/
/* Private function --------------------------------------------------------------------------------------------------*/

/**
  * @brief      按键初始化
  *
  */
void KEY_Init(void)
{
    uint8_t key;

    for (key = 0; key < IO_KEY_MAX_NUM; key++)
    {
        sg_tKeyManage[key].eKeyAction = KEY_ACTION_NO_PRESS;
        sg_tKeyManage[key].eKeyActionBak = KEY_ACTION_NO_PRESS;
        sg_tKeyManage[key].clickCnt = 0;
        sg_tKeyManage[key].uiPressTic = 0;
        sg_tKeyManage[key].uiLossenTic = 0;
        sg_tKeyManage[key].uiAntiShakeTime = KEY_ANTI_SHAKE_TIME;
        sg_tKeyManage[key].refreshNotifyTic = 0;
        sg_tKeyManage[key].pfnKeyNotifyFun = NULL;
    }

    IoKey_Init();
}

/**
  * @brief      注册按键动作改变时的通知回调函数
  *
  * @param[in]  eKey 指定按键, 取值为 @reg IoKeyType_e
  * @param[in]  pKeyFun 通知回调函数
  */
void KEY_Register(IoKeyType_e eKey, KeyFunCB pKeyFun)
{
    if (eKey < IO_KEY_MAX_NUM)
    {
        sg_tKeyManage[eKey].pfnKeyNotifyFun = pKeyFun;
    }
}

/**
  * @brief      注销按键动作改变时的通知回调函数
  *
  * @param[in]  eKey 指定按键, 取值为 @reg IoKeyType_e
  */
void KEY_UnRegister(IoKeyType_e eKey)
{
    if (eKey < IO_KEY_MAX_NUM)
    {
        sg_tKeyManage[eKey].pfnKeyNotifyFun = NULL;
    }
}

/**
  * @brief      设置按键触发时间
  * 
  * @param[in]  eKey 指定按键, 取值为 @reg IoKeyType_e
  * @param[in]  time 下次触发按键事件回调函数的时间
  */
void KEY_SetNotifyTime(IoKeyType_e eKey, uint16_t time)
{
    if (eKey < IO_KEY_MAX_NUM)
    {
        sg_tKeyManage[eKey].refreshNotifyTic = time;
    }
}

/**
  * @brief      获取按键单击次数
  * 
  * @note       调用该函数时不能有关于按键动作的限制条件
  * @param[in]  eKey 指定按键, 取值为 @reg IoKeyType_e
  * @param[in]  time 每次单击的间隔时间
  * @retval     单击次数.
  */
uint8_t KEY_GetClickCnt(IoKeyType_e eKey, uint16_t time)
{
    if (eKey >= IO_KEY_MAX_NUM)
    {
        return 0;
    }

    /* 连续单击后等间隔时间再进行回调通知, 目的在松开按键后再次触发得到单击次数 */
    if (sg_tKeyManage[eKey].eKeyAction == KEY_ACTION_LOSSEN)
    {
        sg_tKeyManage[eKey].refreshNotifyTic = time;
    }
    
    if (sg_tKeyManage[eKey].eKeyActionBak != KEY_ACTION_PRESS && sg_tKeyManage[eKey].eKeyAction == KEY_ACTION_PRESS)
    {
        sg_tKeyManage[eKey].refreshNotifyTic = 0;

        if (sg_tKeyManage[eKey].clickCnt < 0xFF)
        {
            sg_tKeyManage[eKey].clickCnt++;
        }
    }

    if (sg_tKeyManage[eKey].eKeyAction == KEY_ACTION_NO_PRESS && sg_tKeyManage[eKey].uiLossenTic >= time)
    {
        uint8_t cnt = sg_tKeyManage[eKey].clickCnt;
        
        sg_tKeyManage[eKey].clickCnt = 0;
        return cnt;
    }
    
    return 0;
}

/**
  * @brief      获取按键按下时间
  *
  * @param[in]  eKey 指定按键, 取值为 @reg IoKeyType_e
  * @retval     按键按下时间, 单位ms.
  */
uint16_t KEY_GetPressTime(IoKeyType_e eKey)
{
    if (eKey >= IO_KEY_MAX_NUM)
    {
        return 0;
    }

    return sg_tKeyManage[eKey].uiPressTic;
}

/**
  * @brief      获取按键松开时间
  *
  * @param[in]  eKey 指定按键, 取值为 @reg IoKeyType_e
  * @retval     按键松开时间, 单位ms.
  */
uint16_t KEY_GetLossenTime(IoKeyType_e eKey)
{
    if (eKey >= IO_KEY_MAX_NUM)
    {
        return 0;
    }

    return sg_tKeyManage[eKey].uiLossenTic;
}

/**
  * @brief      按键扫描周期任务
  *
  * @param[in]  cycleTime 周期时间, 单位ms.
  */
void KEY_Scan(uint16_t cycleTime)
{
    uint8_t key;
    IoKeyState_e eKeyState;

    IoKey_Scan();

    for (key = 0; key < IO_KEY_MAX_NUM; key++)
    {
        eKeyState = IoKey_Read((IoKeyType_e)key);

        if (eKeyState == IO_KEY_OFF)
        {
            if (sg_tKeyManage[key].eKeyAction == KEY_ACTION_PRESS)
            {
                if (sg_tKeyManage[key].uiLossenTic < (0xFFFF - cycleTime))
                {
                    sg_tKeyManage[key].uiLossenTic += cycleTime;
                }

                if (sg_tKeyManage[key].uiLossenTic >= sg_tKeyManage[key].uiAntiShakeTime)
                {
                    sg_tKeyManage[key].eKeyAction = KEY_ACTION_LOSSEN;
                }
            }
            else
            {
                sg_tKeyManage[key].eKeyAction = KEY_ACTION_NO_PRESS;
                sg_tKeyManage[key].uiPressTic = 0;

                if (sg_tKeyManage[key].uiLossenTic < (0xFFFF - cycleTime))
                {
                    sg_tKeyManage[key].uiLossenTic += cycleTime;
                }
            }
        }
        else
        {
            if (sg_tKeyManage[key].eKeyAction != KEY_ACTION_LOSSEN)
            {
                if (sg_tKeyManage[key].eKeyAction == KEY_ACTION_PRESS)
                {
                    sg_tKeyManage[key].uiLossenTic = 0;
                }

                if (sg_tKeyManage[key].uiPressTic < (0xFFFF - cycleTime))
                {
                    sg_tKeyManage[key].uiPressTic += cycleTime;
                }

                if (sg_tKeyManage[key].uiPressTic >= sg_tKeyManage[key].uiAntiShakeTime)
                {
                    sg_tKeyManage[key].eKeyAction = KEY_ACTION_PRESS;
                }
            }
            else
            {
                sg_tKeyManage[key].eKeyAction = KEY_ACTION_NO_PRESS;
                sg_tKeyManage[key].uiPressTic = 0;
                sg_tKeyManage[key].uiLossenTic = 1;
            }
        }
        
        if (sg_tKeyManage[key].pfnKeyNotifyFun != NULL)
        {
            if (sg_tKeyManage[key].refreshNotifyTic > 0)
            {
                if (sg_tKeyManage[key].refreshNotifyTic >= cycleTime)
                {
                    sg_tKeyManage[key].refreshNotifyTic -= cycleTime;
                }
                else
                {
                    sg_tKeyManage[key].refreshNotifyTic = 0;
                }
                
                if (sg_tKeyManage[key].refreshNotifyTic == 0)
                {
                    sg_tKeyManage[key].pfnKeyNotifyFun((IoKeyType_e)key, sg_tKeyManage[key].eKeyAction);
                }
            }
            
            if (sg_tKeyManage[key].eKeyAction != sg_tKeyManage[key].eKeyActionBak)
            {
                sg_tKeyManage[key].pfnKeyNotifyFun((IoKeyType_e)key, sg_tKeyManage[key].eKeyAction);
            }
        }

        sg_tKeyManage[key].eKeyActionBak = sg_tKeyManage[key].eKeyAction;
    }
}
