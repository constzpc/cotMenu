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
    struct MenuCtrl    *pParentMenuCtrl;    /*!< 父菜单控制处理 */
    ShowMenuCallFun_f   pfnShowMenuFun;     /*!< 菜单显示效果函数 */
    MenuRegister_t     *pMenuInfo;          /*!< 菜单选项内容 */
    menusize_t          totalNum;           /*!< 当前菜单选项总数目 */
    menusize_t          showNum;            /*!< 当前菜单选项需要显示的数目 */
    menusize_t          select;             /*!< 当前菜单选中的选项 */
    menubool            isRunCallback;      /*!< 是否执行回调功能函数 */
}MenuCtrl_t;

typedef struct
{
    MenuCtrl_t        *pMenuCtrl;           /*!< 当前菜单控制处理 */
    menubool           isEnglish;           /*!< 是否切换成英文 */
    MenuCallFun_f      pfnEnterCallFun;     /*!< 当前选项进入所执行的函数 */
    MenuCallFun_f      pfnExitCallFun;      /*!< 当前选项退出所执行的函数 */
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
static MenuCtrl_t *NewMenu(void);
static void DeleteMenu(MenuCtrl_t *pMenu);

/* Private function --------------------------------------------------------------------------------------------------*/
/**
  * @brief      新建菜单层级
  * 
  * @return     MenuCtrl_t* 
  */
static MenuCtrl_t *NewMenu(void)
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
  * @param[in]  pMainMenu        主菜单注册信息
  * @param[in]  num              主菜单数目
  * @param[in]  fpnShowMenu      主菜单显示效果函数, 不能为 NULL
  * @return     0,成功; -1,失败 
  */
int Menu_Init(MenuRegister_t *pMainMenu, uint8_t num, ShowMenuCallFun_f fpnShowMenu)
{
    MenuCtrl_t *pMenuCtrl = NULL;

    if (sg_tMenuManage.pMenuCtrl != NULL)
    {
        return -1;
    }
#if MENU_MAX_DEPTH != 0
    sg_currMenuDepth = 0;
#endif
    
    sg_tMenuManage.isEnglish = MENU_FALSE;
    sg_tMenuManage.pfnEnterCallFun = NULL;
    sg_tMenuManage.pfnExitCallFun = NULL;

    if ((pMenuCtrl = NewMenu()) != NULL)
    {
        pMenuCtrl->pParentMenuCtrl = NULL;
        pMenuCtrl->pfnShowMenuFun = fpnShowMenu;
        pMenuCtrl->pMenuInfo = pMainMenu;
        pMenuCtrl->totalNum = num;
        pMenuCtrl->showNum = num;
        pMenuCtrl->select = 0;
        pMenuCtrl->isRunCallback = MENU_FALSE;

        sg_tMenuManage.pMenuCtrl = pMenuCtrl;

        return 0;
    }

    return -1;
}

/**
 * @brief  菜单反初始化
 * 
 * @return 0,成功; -1,失败  
 */
int Menu_DeInit(void)
{
    if (sg_tMenuManage.pMenuCtrl == NULL)
    {
        return -1;
    }

    while (Menu_Exit(1) == 0);

    DeleteMenu(sg_tMenuManage.pMenuCtrl);
    sg_tMenuManage.pMenuCtrl = NULL;
    sg_tMenuManage.isEnglish = MENU_FALSE;
    sg_tMenuManage.pfnEnterCallFun = NULL;
    sg_tMenuManage.pfnExitCallFun = NULL;

    return 0;
}

/**
  * @brief      设置英文显示
  * 
  * @param[in]  isEnable 使能英文显示
  * @return     0,成功; -1,失败  
  */
int Menu_SetEnglish(menubool isEnable)
{
    if (sg_tMenuManage.pMenuCtrl == NULL)
    {
        return -1;
    }

    sg_tMenuManage.isEnglish = isEnable;
    return 0;
}

/**
  * @brief      设置菜单显示限制
  * 
  * @note       设置成功后, 子菜单会延续父菜单的显示限制, 若需要单独设置子菜单显示限制，可通过菜单选项中的进入时执行函数设置
  * @param[in]  showNum 限制需要显示的数目
  * @return     0,成功; -1,失败
  */
int Menu_SetMenuShowLimit(menubool showNum)
{
    if (sg_tMenuManage.pMenuCtrl == NULL)
    {
        return -1;
    }

    if (showNum < sg_tMenuManage.pMenuCtrl->totalNum)
    {
        sg_tMenuManage.pMenuCtrl->showNum = showNum;
    }
    
    return 0;
}

/**
  * @brief      返回主菜单界面
  * 
  * @return     0,成功; -1,失败 
  */
int Menu_ResetMainMenu(void)
{
    if (sg_tMenuManage.pMenuCtrl == NULL)
    {
        return -1;
    }

    while (Menu_Exit(1) == 0);
    
    return 0;
}

/**
  * @brief      菜单功能是否正在运行
  * 
  * @return     MENU_FALSE,已停止运行; MENU_TRUE,正在运行
  */
menubool Menu_IsRun(void)
{
    if (sg_tMenuManage.pMenuCtrl == NULL)
    {
        return MENU_FALSE;
    }

    return MENU_TRUE;
}

/**
  * @brief      当前是否处于主菜单界面
  * 
  * @return     MENU_FALSE,否/已停止运行; MENU_TRUE,是
  */
menubool Menu_IsMainMenu(void)
{
    if (sg_tMenuManage.pMenuCtrl == NULL)
    {
        return MENU_FALSE;
    }
    
    if (sg_tMenuManage.pMenuCtrl->pParentMenuCtrl != NULL)
    {
        return MENU_FALSE;
    }

    return MENU_TRUE;
}

/**
  * @brief      当前是否处于菜单界面, 即没有在执行非菜单功能回调函数
  * 
  * @return     MENU_FALSE,正在执行非菜单功能回调函数/已停止运行; MENU_TRUE,处于菜单界面
  */
menubool Menu_IsAtMenu(void)
{
    if (sg_tMenuManage.pMenuCtrl == NULL)
    {
        return MENU_FALSE;
    }

    return !sg_tMenuManage.pMenuCtrl->isRunCallback;
}

/**
  * @brief      进入当前菜单选项
  * 
  * @return     0,成功; -1,失败  
  */
int Menu_Enter(void)
{
    MenuCtrl_t *pMenuCtrl = NULL;

    if (sg_tMenuManage.pMenuCtrl == NULL || sg_tMenuManage.pMenuCtrl->isRunCallback)
    {
        return -1;
    }

    if (sg_tMenuManage.pMenuCtrl->pMenuInfo[sg_tMenuManage.pMenuCtrl->select].subMenuNum != 0)
    {
        sg_tMenuManage.pMenuCtrl->isRunCallback = MENU_FALSE;

        if ((pMenuCtrl = NewMenu()) != NULL)
        {
            pMenuCtrl->pParentMenuCtrl = sg_tMenuManage.pMenuCtrl;
            pMenuCtrl->pMenuInfo = sg_tMenuManage.pMenuCtrl->pMenuInfo[sg_tMenuManage.pMenuCtrl->select].pSubMenu;
            pMenuCtrl->totalNum = sg_tMenuManage.pMenuCtrl->pMenuInfo[sg_tMenuManage.pMenuCtrl->select].subMenuNum;
            pMenuCtrl->showNum = sg_tMenuManage.pMenuCtrl->showNum;

            /* 若子菜单没有设置显示风格，则延续上个菜单界面的 */
            if (sg_tMenuManage.pMenuCtrl->pMenuInfo[sg_tMenuManage.pMenuCtrl->select].pfnShowMenuFun != NULL)
            {
                pMenuCtrl->pfnShowMenuFun = sg_tMenuManage.pMenuCtrl->pMenuInfo[sg_tMenuManage.pMenuCtrl->select].pfnShowMenuFun;
            }
            else
            {
                pMenuCtrl->pfnShowMenuFun = sg_tMenuManage.pMenuCtrl->pfnShowMenuFun;
            }
            
            pMenuCtrl->select = 0;
            pMenuCtrl->isRunCallback = MENU_FALSE;

            sg_tMenuManage.pfnEnterCallFun = sg_tMenuManage.pMenuCtrl->pMenuInfo[sg_tMenuManage.pMenuCtrl->select].pfnEnterCallFun;
            sg_tMenuManage.pMenuCtrl = pMenuCtrl;
            return 0;
        }
    }
    else
    {
        sg_tMenuManage.pMenuCtrl->isRunCallback = MENU_TRUE;
        sg_tMenuManage.pfnEnterCallFun = sg_tMenuManage.pMenuCtrl->pMenuInfo[sg_tMenuManage.pMenuCtrl->select].pfnEnterCallFun;
        return 0;
    }

    return -1;
}

/**
  * @brief      退出当前选项并返回上一层菜单
  * 
  * @param[in]  isReset 菜单选项是否从头选择
  * @return     0,成功; -1,失败, 即目前处于主菜单, 无法返回
  */
int Menu_Exit(uint8_t isReset)
{
    if (sg_tMenuManage.pMenuCtrl == NULL)
    {
        return -1;
    }

    if (sg_tMenuManage.pMenuCtrl->isRunCallback)
    {
        sg_tMenuManage.pMenuCtrl->isRunCallback = MENU_FALSE;
        sg_tMenuManage.pfnExitCallFun = sg_tMenuManage.pMenuCtrl->pMenuInfo[sg_tMenuManage.pMenuCtrl->select].pfnExitCallFun;
    }
    else
    {
        if (sg_tMenuManage.pMenuCtrl->pParentMenuCtrl != NULL)
        {
            MenuCtrl_t *pMenuCtrl = sg_tMenuManage.pMenuCtrl;

            sg_tMenuManage.pMenuCtrl = sg_tMenuManage.pMenuCtrl->pParentMenuCtrl;
            sg_tMenuManage.pfnExitCallFun = sg_tMenuManage.pMenuCtrl->pMenuInfo[sg_tMenuManage.pMenuCtrl->select].pfnExitCallFun;
            
            if (isReset)
            {
                sg_tMenuManage.pMenuCtrl->select = 0;
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
    if (sg_tMenuManage.pMenuCtrl == NULL || sg_tMenuManage.pMenuCtrl->isRunCallback)
    {
        return -1;
    }

    if (sg_tMenuManage.pMenuCtrl->select > 0)
    {
        sg_tMenuManage.pMenuCtrl->select--;
    }
    else
    {
        if (isAllowRoll)
        {
            sg_tMenuManage.pMenuCtrl->select = sg_tMenuManage.pMenuCtrl->totalNum - 1;
        }
        else
        {
            sg_tMenuManage.pMenuCtrl->select = 0;
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
    if (sg_tMenuManage.pMenuCtrl == NULL || sg_tMenuManage.pMenuCtrl->isRunCallback)
    {
        return -1;
    }

    if (sg_tMenuManage.pMenuCtrl->select < (sg_tMenuManage.pMenuCtrl->totalNum - 1))
    {
        sg_tMenuManage.pMenuCtrl->select++;
    }
    else
    {
        if (isAllowRoll)
        {
            sg_tMenuManage.pMenuCtrl->select = 0;
        }
        else
        {
            sg_tMenuManage.pMenuCtrl->select = sg_tMenuManage.pMenuCtrl->totalNum - 1;
            return -1;
        }
    }

    return 0;
}

/**
  * @brief      获取当前菜单中首个显示的选项
  * 
  * @param[in]  showNum     当前菜单中需要显示的选项数目
  * @param[in]  currBase    目前首个显示的选项
  * @param[in]  select      当前菜单中被选中的选项
  * @return     新首个显示的选项 
  */
static menusize_t GetShowBase(menusize_t showNum, menusize_t currBase, menusize_t select)
{
    menusize_t base = currBase;

    if (select < currBase)
    {
        base = select;
    }
    else if ((select - currBase) >= showNum)
    {
        base = select - showNum + 1;
    }
    else
    {
        base = currBase;
    }

    return base;
}

/**
  * @brief  菜单任务
  * 
  * @return 0,成功, 处于菜单模式下; -1,失败, 未处于菜单模式下 
  */
int Menu_Task(void)
{
    int i;
    MenuRegister_t *pMenu;
    MenuShow_t tMenuShow;

    if (sg_tMenuManage.pMenuCtrl == NULL)
    {
        return -1;
    }

    pMenu = sg_tMenuManage.pMenuCtrl->pMenuInfo;
    
    if (sg_tMenuManage.pfnEnterCallFun != NULL)
    {
        sg_tMenuManage.pfnEnterCallFun();
        sg_tMenuManage.pfnEnterCallFun = NULL;
    }

    if (sg_tMenuManage.pfnExitCallFun != NULL)
    {
        sg_tMenuManage.pfnExitCallFun();
        sg_tMenuManage.pfnExitCallFun = NULL;
    }
    
    if (!sg_tMenuManage.pMenuCtrl->isRunCallback)
    {
        tMenuShow.totalNum = sg_tMenuManage.pMenuCtrl->totalNum;
        tMenuShow.select = sg_tMenuManage.pMenuCtrl->select;
        tMenuShow.show.num = sg_tMenuManage.pMenuCtrl->showNum;
        tMenuShow.show.base = GetShowBase(tMenuShow.show.num, tMenuShow.show.base, tMenuShow.select);

        if (sg_tMenuManage.isEnglish)
        {
            for (i = 0; i < tMenuShow.totalNum && i < MENU_MAX_NUM; i++)
            {
                tMenuShow.pszDesc[i] = (char *)pMenu[i].pszEnDesc;
                tMenuShow.pExtendData[i] = pMenu[i].pExtendData;
            }        
        }
        else
        {
            for (i = 0; i < tMenuShow.totalNum && i < MENU_MAX_NUM; i++)
            {
                tMenuShow.pszDesc[i] = (char *)pMenu[i].pszDesc;
                tMenuShow.pExtendData[i] = pMenu[i].pExtendData;
            }        
        }

        if (sg_tMenuManage.pMenuCtrl->pfnShowMenuFun != NULL)
        {
            sg_tMenuManage.pMenuCtrl->pfnShowMenuFun(&tMenuShow);
        }
    }
    else
    {
        if (pMenu[sg_tMenuManage.pMenuCtrl->select].pfnMenuCallFun != NULL)
        {
            pMenu[sg_tMenuManage.pMenuCtrl->select].pfnMenuCallFun();
        }
    }

    return 0;
}
