/**
  **********************************************************************************************************************
  * @file    menu.c
  * @brief   该文件提供菜单框架功能
  * @author  const_zpc  any question please send mail to const_zpc@163.com
  * @version V1.0.0
  * @date    2021-11-05
  *
  * @details  功能详细说明：
  *           + 菜单初始化函数
  *           + 返回主菜单函数
  *           + 菜单控制函数
  *           + 菜单轮询任务函数
  *
  **********************************************************************************************************************
  * 源码路径：https://gitee.com/const-zpc/menu.git 具体问题及建议可在该网址填写 Issue
  *
  * 使用方式:
  *    1、使用前初始化函数 Menu_Init, 设置主菜单内容
  *    2、周期调用函数 Menu_Task, 用来处理菜单显示和执行相关回调函数
  *    3、使用其他函数对菜单界面控制
  *
  **********************************************************************************************************************
  */

/* Includes ----------------------------------------------------------------------------------------------------------*/
#include "menu.h"

#if MENU_MAX_DEPTH == 0
#include <malloc.h>
#endif

/* Private typedef ---------------------------------------------------------------------------------------------------*/
typedef struct MenuCtrl
{
    struct MenuCtrl    *pLastMenuCtrl;      /*!< 父菜单控制处理 */
    ShowMenuCallFun_f   pfnShowMenuFun;     /*!< 菜单显示效果函数 */
    MenuRegister_t     *pMenuInfo;          /*!< 菜单选项内容 */
    menusize_t          menuNum;            /*!< 菜单选项数目 */
    menusize_t          currPos;            /*!< 当前选择 */
    uint8_t             isRunCallback;      /*!< 是否执行回调功能函数 */
}MenuCtrl_t;

typedef struct
{
    MenuCtrl_t         *pCurrMenuCtrl;      /*!< 当前菜单控制处理 */
}MenuManage_t;

/* Private define ----------------------------------------------------------------------------------------------------*/
/* Private macro -----------------------------------------------------------------------------------------------------*/
/* Private variables -------------------------------------------------------------------------------------------------*/
static MenuManage_t sg_tMenuManage;

#if MENU_MAX_DEPTH != 0
static MenuCtrl_t sg_arrMenuCtrl[MENU_MAX_DEPTH];
static uint8_t    sg_currMenuDepth = 0;
#endif

/* Private function prototypes ---------------------------------------------------------------------------------------*/
static MenuCtrl_t *NewMenu();
static void DeleteMenu(MenuCtrl_t *pMenu);

/* Private function --------------------------------------------------------------------------------------------------*/
/**
  * @brief      新建菜单层级
  * 
  * @return     MenuCtrl_t* 
  */
static MenuCtrl_t *NewMenu()
{
    MenuCtrl_t *pMenuCtrl = NULL;
#if MENU_MAX_DEPTH == 0
    pMenuCtrl = (MenuCtrl_t *)malloc(sizeof(MenuCtrl_t));
#else
    if (sg_currMenuDepth < MENU_MAX_DEPTH)
    {
        pMenuCtrl = &sg_arrMenuCtrl[sg_currMenuDepth++];
    }
#endif

    return pMenuCtrl;
}

/**
  * @brief      销毁菜单层级
  * 
  * @param      pMenu 
  */
static void DeleteMenu(MenuCtrl_t *pMenu)
{
#if MENU_MAX_DEPTH == 0
    free(pMenu);
#else
    if (sg_currMenuDepth > 0)
    {
        sg_currMenuDepth--;
    }
#endif   
}

/**
  * @brief      菜单初始化
  * 
  * @param[in]  pMainMenu  主菜单注册信息
  * @param[in]  num        主菜单数目
  * @param[in]  fpnShowMenu  主菜单显示效果函数
  * @return     0,成功; -1,失败 
  */
int Menu_Init(MenuRegister_t *pMainMenu, uint8_t num, ShowMenuCallFun_f fpnShowMenu)
{
    MenuCtrl_t *pMenuCtrl = NULL;

#if MENU_MAX_DEPTH != 0
    sg_currMenuDepth = 0;
#endif

    if ((pMenuCtrl = NewMenu()) != NULL)
    {
        pMenuCtrl->pLastMenuCtrl = NULL;
        pMenuCtrl->pfnShowMenuFun = fpnShowMenu;
        pMenuCtrl->pMenuInfo = pMainMenu;
        pMenuCtrl->menuNum = num;
        pMenuCtrl->currPos = 0;
        pMenuCtrl->isRunCallback = 0;

        sg_tMenuManage.pCurrMenuCtrl = pMenuCtrl;

        return 0;
    }

    return -1;
}

/**
  * @brief      返回主菜单界面
  * 
  * @return     0,成功; -1,失败 
  */
int Menu_ResetMainMenu(void)
{
    while (Menu_Exit(1) == 0);
    
    return 0;
}

/**
  * @brief      进入当前菜单选项
  * 
  * @return     0,成功; -1,失败  
  */
int Menu_Enter(void)
{
    MenuCtrl_t *pMenuCtrl = NULL;

    if (sg_tMenuManage.pCurrMenuCtrl->pMenuInfo[sg_tMenuManage.pCurrMenuCtrl->currPos].subMenuNum != 0)
    {
        sg_tMenuManage.pCurrMenuCtrl->isRunCallback = 0;

        if ((pMenuCtrl = NewMenu()) != NULL)
        {
            pMenuCtrl->pLastMenuCtrl = sg_tMenuManage.pCurrMenuCtrl;
            pMenuCtrl->pMenuInfo = sg_tMenuManage.pCurrMenuCtrl->pMenuInfo[sg_tMenuManage.pCurrMenuCtrl->currPos].pSubMenu;
            pMenuCtrl->menuNum = sg_tMenuManage.pCurrMenuCtrl->pMenuInfo[sg_tMenuManage.pCurrMenuCtrl->currPos].subMenuNum;

            /* 若子菜单没有设置显示风格，则延续上个菜单界面的 */
            if (sg_tMenuManage.pCurrMenuCtrl->pMenuInfo[sg_tMenuManage.pCurrMenuCtrl->currPos].pfnShowMenuFun != NULL)
            {
                pMenuCtrl->pfnShowMenuFun = sg_tMenuManage.pCurrMenuCtrl->pMenuInfo[sg_tMenuManage.pCurrMenuCtrl->currPos].pfnShowMenuFun;
            }
            else
            {
                pMenuCtrl->pfnShowMenuFun = sg_tMenuManage.pCurrMenuCtrl->pfnShowMenuFun;
            }
            
            pMenuCtrl->currPos = 0;
            pMenuCtrl->isRunCallback = 0;

            sg_tMenuManage.pCurrMenuCtrl = pMenuCtrl;
            return 0;
        }
    }
    else
    {
        sg_tMenuManage.pCurrMenuCtrl->isRunCallback = 1;
        return 0;
    }

    return -1;
}

/**
  * @brief      退出当前选项并返回上一层菜单
  * 
  * @param[in]  isReset 菜单选项是否从头选择
  * @return     0,成功; -1,失败, 即主菜单 
  */
int Menu_Exit(uint8_t isReset)
{
    if (sg_tMenuManage.pCurrMenuCtrl->isRunCallback == 1)
    {
        sg_tMenuManage.pCurrMenuCtrl->isRunCallback = 0;
    }
    else
    {
        if (sg_tMenuManage.pCurrMenuCtrl->pLastMenuCtrl != NULL)
        {
            MenuCtrl_t *pMenuCtrl = sg_tMenuManage.pCurrMenuCtrl;

            sg_tMenuManage.pCurrMenuCtrl = sg_tMenuManage.pCurrMenuCtrl->pLastMenuCtrl;
            
            if (isReset)
            {
                sg_tMenuManage.pCurrMenuCtrl->currPos = 0;
            }

            DeleteMenu(pMenuCtrl);
        }
        else
        {
            return -1;
        }
    }

    return 0;
}

/**
  * @brief      选择上一个菜单选项
  * 
  * @param[in]  isAllowRoll 第一个选项时是否从跳转到最后一个选项
  * @return     0,成功; -1,失败
  */
int Menu_SelectPrevious(uint8_t isAllowRoll)
{
    if (sg_tMenuManage.pCurrMenuCtrl->currPos > 0)
    {
        sg_tMenuManage.pCurrMenuCtrl->currPos--;
    }
    else
    {
        if (isAllowRoll)
        {
            sg_tMenuManage.pCurrMenuCtrl->currPos = sg_tMenuManage.pCurrMenuCtrl->menuNum - 1;
        }
        else
        {
            sg_tMenuManage.pCurrMenuCtrl->currPos = 0;
            return -1;
        }        
    }

    return 0;
}

/**
  * @brief      选择下一个菜单选项
  * 
  * @param[in]  isAllowRoll 最后一个选项时是否跳转到第一个选项
  * @return     0,成功; -1,失败 
  */
int Menu_SelectNext(uint8_t isAllowRoll)
{
    if (sg_tMenuManage.pCurrMenuCtrl->currPos < (sg_tMenuManage.pCurrMenuCtrl->menuNum - 1))
    {
        sg_tMenuManage.pCurrMenuCtrl->currPos++;
    }
    else
    {
        if (isAllowRoll)
        {
            sg_tMenuManage.pCurrMenuCtrl->currPos = 0;
        }
        else
        {
            sg_tMenuManage.pCurrMenuCtrl->currPos = sg_tMenuManage.pCurrMenuCtrl->menuNum - 1;
            return -1;
        }
    }

    return 0;
}

/**
  * @brief  菜单任务
  * 
  */
int Menu_Task(void)
{
    MenuRegister_t *pMenu = sg_tMenuManage.pCurrMenuCtrl->pMenuInfo;
    char *parrszDesc[MENU_MAX_NUM];

    if (sg_tMenuManage.pCurrMenuCtrl->isRunCallback == 0)
    {
        for (int i = 0; i < sg_tMenuManage.pCurrMenuCtrl->menuNum && i < MENU_MAX_NUM; i++)
        {
            parrszDesc[i] = (char *)pMenu[i].pszDesc;
        }

        if (sg_tMenuManage.pCurrMenuCtrl->pfnShowMenuFun != NULL)
        {
            sg_tMenuManage.pCurrMenuCtrl->pfnShowMenuFun(sg_tMenuManage.pCurrMenuCtrl->menuNum, 
                        sg_tMenuManage.pCurrMenuCtrl->currPos, (const char **)parrszDesc);
        }
    }
    else
    {
        if (pMenu[sg_tMenuManage.pCurrMenuCtrl->currPos].pfnMenuCallFun != NULL)
        {
            pMenu[sg_tMenuManage.pCurrMenuCtrl->currPos].pfnMenuCallFun();
        }
    }

    return 0;
}