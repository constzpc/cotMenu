/**
  **********************************************************************************************************************
  * @file    menu.c
  * @brief   该文件提供菜单框架功能
  * @author  const_zpc  any question please send mail to const_zpc@163.com
  * @version V2.0.0
  * @date    2021-11-10
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

#ifdef _USE_MALLOC_
#include <malloc.h>
#endif

/* Private typedef ---------------------------------------------------------------------------------------------------*/
typedef struct MenuCtrl
{
    struct MenuCtrl    *pParentMenuCtrl;    /*!< 父菜单控制处理 */
    ShowMenuCallFun_f   pfnShowMenuFun;     /*!< 当前菜单显示效果函数 */
    MenuRegister_t     *pMenuInfo;          /*!< 当前菜单选项信息 */
    menusize_t          itemsNum;           /*!< 当前菜单选项总数目 */
    menusize_t          showBaseItem;       /*!< 当前菜单首个显示的选项 */
    menusize_t          selectItem;         /*!< 当前菜单选中的选项 */
    menubool            isRunCallback;      /*!< 是否执行回调功能函数 */
}MenuCtrl_t;

typedef struct
{
    MenuCtrl_t        *pMenuCtrl;           /*!< 当前菜单控制处理 */
    menubool           isEnglish;           /*!< 是否切换成英文 */
    MenuRegister_t    *pShortcutMenuAddr[MENU_MAX_SHORTCUT_NUM];/*!< 快捷选择项列表 */
    MenuCallFun_f      pfnEnterCallFun;     /*!< 当前选项进入所执行的函数 */
    MenuCallFun_f      pfnExitCallFun;      /*!< 当前选项退出所执行的函数 */
}MenuManage_t;

/* Private define ----------------------------------------------------------------------------------------------------*/
/* Private macro -----------------------------------------------------------------------------------------------------*/
/* Private variables -------------------------------------------------------------------------------------------------*/
static MenuManage_t sg_tMenuManage;

#ifndef _USE_MALLOC_
static MenuCtrl_t sg_arrMenuCtrl[MENU_MAX_DEPTH];
#endif

static uint8_t    sg_currMenuDepth = 0;

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

    if (sg_currMenuDepth < MENU_MAX_DEPTH)
    {
#ifdef _USE_MALLOC_
        pMenuCtrl = (MenuCtrl_t *)malloc(sizeof(MenuCtrl_t));
#else  
        pMenuCtrl = &sg_arrMenuCtrl[sg_currMenuDepth];
#endif
        sg_currMenuDepth++;
    }

    return pMenuCtrl;
}

/**
  * @brief      销毁菜单层级
  * 
  * @param      pMenu 
  */
static void DeleteMenu(MenuCtrl_t *pMenu)
{
#ifdef _USE_MALLOC_
    free(pMenu);
#endif
    if (sg_currMenuDepth > 0)
    {
        sg_currMenuDepth--;
    }
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
    int i;
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

    for (i = 0; i < MENU_MAX_SHORTCUT_NUM; i++)
    {
        sg_tMenuManage.pShortcutMenuAddr[i] = NULL;
    }

    if ((pMenuCtrl = NewMenu()) != NULL)
    {
        pMenuCtrl->pParentMenuCtrl = NULL;
        pMenuCtrl->pfnShowMenuFun = fpnShowMenu;
        pMenuCtrl->pMenuInfo = pMainMenu;
        pMenuCtrl->itemsNum = num;
        pMenuCtrl->selectItem = 0;
        pMenuCtrl->showBaseItem = 0;
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
  * @brief      是否为英文显示
  * 
  * @return     MENU_FALSE,中文; MENU_TRUE,英文 
  */
menubool Menu_IsEnglish(void)
{
    return sg_tMenuManage.isEnglish;
}

/**
  * @brief      设置英文显示
  * 
  * @param[in]  isEnable 使能英文显示
  * @return     0,成功; -1,失败  
  */
int Menu_SetEnglish(menubool isEnable)
{
    sg_tMenuManage.isEnglish = isEnable;
    return 0;
}

/**
 * @brief       查询快捷菜单选择项信息指针是否存在
 * 
 * @param[out]  pPathId     所在父菜单的索引号
 * @param[in]   pMenuItem   快捷菜单选择项信息指针
 * @param[in]   pMenuList   菜单列表
 * @param[in]   menuNum     菜单数目
 * @return      MENU_FALSE,不存在; MENU_TRUE,存在 
 */
static menubool FindMenu(int8_t *pPathId, const MenuRegister_t *pMenuItem, const MenuRegister_t *pMenuList, menusize_t menuNum)
{
    int i;

    if (pMenuItem == NULL || pMenuList == NULL || menuNum == 0)
    {
        return MENU_FALSE;
    }

    for (i = 0; i < menuNum; i++)
    {
        *pPathId = i;

        if (pMenuItem != (&pMenuList[i]))
        {
            if (FindMenu(pPathId + 1, pMenuItem, pMenuList[i].pSubMenu, pMenuList[i].subMenuNum))
            {
                return MENU_TRUE;
            }
        }
        else
        {
            *(pPathId + 1) = -1;
            return MENU_TRUE;
        }
    }

    return MENU_FALSE;
}

/**
  * @brief      添加快捷菜单选择项
  * 
  * @param[in]  pMenuAddr 选择项信息指针
  * @return     -1,失败; 其他,快捷菜单选择项ID
  */
int Menu_AddShortcutMenu(MenuRegister_t *pMenuAddr)
{
    int idx = 0;
    int8_t pathId[MENU_MAX_DEPTH + 1];

    while (idx < MENU_MAX_SHORTCUT_NUM && sg_tMenuManage.pShortcutMenuAddr[idx++] != NULL);
    
    if (idx == MENU_MAX_SHORTCUT_NUM)
    {
        return -1;
    }

    idx--;
    sg_tMenuManage.pShortcutMenuAddr[idx] = pMenuAddr;

    if (!FindMenu(pathId, pMenuAddr, sg_tMenuManage.pMenuCtrl->pMenuInfo, sg_tMenuManage.pMenuCtrl->itemsNum))
    {
        return -1;
    }

    return idx;
}

/**
  * @brief      删除快捷菜单选择项
  * 
  * @param[in]  pMenuAddr 选择项信息指针
  * @return     0,成功; -1,失败
  */
int Menu_DeleteShortcutMenu(MenuRegister_t *pMenuPath)
{
    int i, idx = 0;

    while (idx < MENU_MAX_SHORTCUT_NUM && sg_tMenuManage.pShortcutMenuAddr[idx] != NULL)
    {
        if (sg_tMenuManage.pShortcutMenuAddr[idx] == pMenuPath)
        {
            for (i = idx; i < MENU_MAX_SHORTCUT_NUM; i++)
            {
                if (i == MENU_MAX_SHORTCUT_NUM - 1)
                {
                    sg_tMenuManage.pShortcutMenuAddr[i] = NULL;
                }
                else
                {
                    sg_tMenuManage.pShortcutMenuAddr[i] = sg_tMenuManage.pShortcutMenuAddr[i + 1];
                }
            }
        }

        idx++;
    }
    
    return 0;
}

/**
  * @brief      复位菜单, 回到主菜单界面
  * 
  * @note       该复位回到主菜单不会执行退出所需要执行的回调函数
  * @return     0,成功; -1,失败 
  */
int Menu_Reset(void)
{
    if (sg_tMenuManage.pMenuCtrl == NULL)
    {
        return -1;
    }

    while (sg_tMenuManage.pMenuCtrl->pParentMenuCtrl != NULL)
    {
        MenuCtrl_t *pMenuCtrl = sg_tMenuManage.pMenuCtrl;

        sg_tMenuManage.pMenuCtrl = sg_tMenuManage.pMenuCtrl->pParentMenuCtrl;
        DeleteMenu(pMenuCtrl);
    }

    sg_tMenuManage.pMenuCtrl->selectItem = 0;
    sg_tMenuManage.pMenuCtrl->isRunCallback = MENU_FALSE;
    
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

    if (sg_tMenuManage.pMenuCtrl->pMenuInfo[sg_tMenuManage.pMenuCtrl->selectItem].subMenuNum != 0)
    {
        sg_tMenuManage.pMenuCtrl->isRunCallback = MENU_FALSE;

        if ((pMenuCtrl = NewMenu()) != NULL)
        {
            pMenuCtrl->pParentMenuCtrl = sg_tMenuManage.pMenuCtrl;
            pMenuCtrl->pMenuInfo = sg_tMenuManage.pMenuCtrl->pMenuInfo[sg_tMenuManage.pMenuCtrl->selectItem].pSubMenu;
            pMenuCtrl->itemsNum = sg_tMenuManage.pMenuCtrl->pMenuInfo[sg_tMenuManage.pMenuCtrl->selectItem].subMenuNum;

            /* 若子菜单没有设置显示风格，则延续上个菜单界面的 */
            if (sg_tMenuManage.pMenuCtrl->pMenuInfo[sg_tMenuManage.pMenuCtrl->selectItem].pfnShowMenuFun != NULL)
            {
                pMenuCtrl->pfnShowMenuFun = sg_tMenuManage.pMenuCtrl->pMenuInfo[sg_tMenuManage.pMenuCtrl->selectItem].pfnShowMenuFun;
            }
            else
            {
                pMenuCtrl->pfnShowMenuFun = sg_tMenuManage.pMenuCtrl->pfnShowMenuFun;
            }
            
            pMenuCtrl->selectItem = 0;
            pMenuCtrl->isRunCallback = MENU_FALSE;

            sg_tMenuManage.pfnEnterCallFun = sg_tMenuManage.pMenuCtrl->pMenuInfo[sg_tMenuManage.pMenuCtrl->selectItem].pfnEnterCallFun;
            sg_tMenuManage.pMenuCtrl = pMenuCtrl;
            return 0;
        }
    }
    else
    {
        sg_tMenuManage.pMenuCtrl->isRunCallback = MENU_TRUE;
        sg_tMenuManage.pfnEnterCallFun = sg_tMenuManage.pMenuCtrl->pMenuInfo[sg_tMenuManage.pMenuCtrl->selectItem].pfnEnterCallFun;
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
int Menu_Exit(menubool isReset)
{
    if (sg_tMenuManage.pMenuCtrl == NULL)
    {
        return -1;
    }

    if (sg_tMenuManage.pMenuCtrl->isRunCallback)
    {
        sg_tMenuManage.pMenuCtrl->isRunCallback = MENU_FALSE;
        sg_tMenuManage.pfnExitCallFun = sg_tMenuManage.pMenuCtrl->pMenuInfo[sg_tMenuManage.pMenuCtrl->selectItem].pfnExitCallFun;
    }
    else
    {
        if (sg_tMenuManage.pMenuCtrl->pParentMenuCtrl != NULL)
        {
            MenuCtrl_t *pMenuCtrl = sg_tMenuManage.pMenuCtrl;

            sg_tMenuManage.pMenuCtrl = sg_tMenuManage.pMenuCtrl->pParentMenuCtrl;
            sg_tMenuManage.pfnExitCallFun = sg_tMenuManage.pMenuCtrl->pMenuInfo[sg_tMenuManage.pMenuCtrl->selectItem].pfnExitCallFun;
            
            if (isReset)
            {
                sg_tMenuManage.pMenuCtrl->selectItem = 0;
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
int Menu_SelectPrevious(menubool isAllowRoll)
{
    if (sg_tMenuManage.pMenuCtrl == NULL || sg_tMenuManage.pMenuCtrl->isRunCallback)
    {
        return -1;
    }

    if (sg_tMenuManage.pMenuCtrl->selectItem > 0)
    {
        sg_tMenuManage.pMenuCtrl->selectItem--;
    }
    else
    {
        if (isAllowRoll)
        {
            sg_tMenuManage.pMenuCtrl->selectItem = sg_tMenuManage.pMenuCtrl->itemsNum - 1;
        }
        else
        {
            sg_tMenuManage.pMenuCtrl->selectItem = 0;
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
int Menu_SelectNext(menubool isAllowRoll)
{
    if (sg_tMenuManage.pMenuCtrl == NULL || sg_tMenuManage.pMenuCtrl->isRunCallback)
    {
        return -1;
    }

    if (sg_tMenuManage.pMenuCtrl->selectItem < (sg_tMenuManage.pMenuCtrl->itemsNum - 1))
    {
        sg_tMenuManage.pMenuCtrl->selectItem++;
    }
    else
    {
        if (isAllowRoll)
        {
            sg_tMenuManage.pMenuCtrl->selectItem = 0;
        }
        else
        {
            sg_tMenuManage.pMenuCtrl->selectItem = sg_tMenuManage.pMenuCtrl->itemsNum - 1;
            return -1;
        }
    }

    return 0;
}


/**
  * @brief      在菜单界面通过快捷菜单id进入
  * 
  * @param[in]  id 选择项id, 即通过函数 Menu_AddShortcutMenu 获取的id
  * @return     0,成功; -1,失败
  */
int Menu_EnterShortcutMenu(int8_t id)
{
    MenuCtrl_t *pMenuCtrl = NULL;
    uint8_t depth = 0;
    int8_t pathId[MENU_MAX_DEPTH + 1];

    if (sg_tMenuManage.pMenuCtrl == NULL || sg_tMenuManage.pMenuCtrl->isRunCallback)
    {
        return -1;
    }

    if (id < 0 || id >= MENU_MAX_SHORTCUT_NUM || sg_tMenuManage.pShortcutMenuAddr[id] == NULL)
    {
        return -1;
    }

    Menu_Reset();

    if (!FindMenu(pathId, sg_tMenuManage.pShortcutMenuAddr[id], sg_tMenuManage.pMenuCtrl->pMenuInfo, sg_tMenuManage.pMenuCtrl->itemsNum))
    {
        return -1;
    }

    while (pathId[depth] != -1)
    {
        sg_tMenuManage.pMenuCtrl->selectItem = pathId[depth];
        
        if (sg_tMenuManage.pMenuCtrl->pMenuInfo[pathId[depth]].subMenuNum != 0)
        {
            sg_tMenuManage.pMenuCtrl->isRunCallback = MENU_FALSE;

            if ((pMenuCtrl = NewMenu()) != NULL)
            {
                pMenuCtrl->pParentMenuCtrl = sg_tMenuManage.pMenuCtrl;
                pMenuCtrl->pMenuInfo = sg_tMenuManage.pMenuCtrl->pMenuInfo[pathId[depth]].pSubMenu;
                pMenuCtrl->itemsNum = sg_tMenuManage.pMenuCtrl->pMenuInfo[pathId[depth]].subMenuNum;

                /* 若子菜单没有设置显示风格，则延续上个菜单界面的 */
                if (sg_tMenuManage.pMenuCtrl->pMenuInfo[pathId[depth]].pfnShowMenuFun != NULL)
                {
                    pMenuCtrl->pfnShowMenuFun = sg_tMenuManage.pMenuCtrl->pMenuInfo[pathId[depth]].pfnShowMenuFun;
                }
                else
                {
                    pMenuCtrl->pfnShowMenuFun = sg_tMenuManage.pMenuCtrl->pfnShowMenuFun;
                }
                
                pMenuCtrl->selectItem = 0;
                pMenuCtrl->isRunCallback = MENU_FALSE;

                sg_tMenuManage.pfnEnterCallFun = sg_tMenuManage.pMenuCtrl->pMenuInfo[pathId[depth]].pfnEnterCallFun;
                sg_tMenuManage.pMenuCtrl = pMenuCtrl;
            }
        }
        else
        {
            sg_tMenuManage.pMenuCtrl->isRunCallback = MENU_TRUE;
            sg_tMenuManage.pfnEnterCallFun = sg_tMenuManage.pMenuCtrl->pMenuInfo[pathId[depth]].pfnEnterCallFun;
            return 0;
        }

        depth++;
    } 

    return 0;
}

/**
  * @brief      更新当前菜单首个显示的选项
  * 
  * @note       可在菜单显示效果回调函数中使用
  * @param[in,out]  tMenuShow   当前菜单显示信息
  * @param[in,out]  showNum     当前菜单中需要显示的选项数目, 根据当前菜单选项的总数得到最终的显示的选项数目
  * @return     0,成功; -1,失败 
  */
int Menu_UpdateShowBase(MenuShow_t *ptMenuShow, menusize_t *pShowNum)
{
    if (ptMenuShow == NULL || pShowNum == NULL)
    {
        return -1;
    }

    if (*pShowNum > ptMenuShow->itemsNum)
    {
        *pShowNum = ptMenuShow->itemsNum;
    }

    if (ptMenuShow->selectItem < ptMenuShow->showBaseItem)
    {
        ptMenuShow->showBaseItem = ptMenuShow->selectItem;
    }
    else if ((ptMenuShow->selectItem - ptMenuShow->showBaseItem) >= *pShowNum)
    {
        ptMenuShow->showBaseItem = ptMenuShow->selectItem - *pShowNum + 1;
    }
    else
    {
        // 保持
    }

    return 0;
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
        tMenuShow.itemsNum = sg_tMenuManage.pMenuCtrl->itemsNum;
        tMenuShow.selectItem = sg_tMenuManage.pMenuCtrl->selectItem;
        tMenuShow.showBaseItem = sg_tMenuManage.pMenuCtrl->showBaseItem;

        if (sg_tMenuManage.isEnglish)
        {
            for (i = 0; i < tMenuShow.itemsNum && i < MENU_MAX_NUM; i++)
            {
                tMenuShow.pszItemsDesc[i] = (char *)pMenu[i].pszEnDesc;
                tMenuShow.pItemsExData[i] = pMenu[i].pExtendData;
            }        
        }
        else
        {
            for (i = 0; i < tMenuShow.itemsNum && i < MENU_MAX_NUM; i++)
            {
                tMenuShow.pszItemsDesc[i] = (char *)pMenu[i].pszDesc;
                tMenuShow.pItemsExData[i] = pMenu[i].pExtendData;
            }        
        }

        if (sg_tMenuManage.pMenuCtrl->pfnShowMenuFun != NULL)
        {
            sg_tMenuManage.pMenuCtrl->pfnShowMenuFun(&tMenuShow);
        }

        sg_tMenuManage.pMenuCtrl->showBaseItem = tMenuShow.showBaseItem;
    }
    else
    {
        if (pMenu[sg_tMenuManage.pMenuCtrl->selectItem].pfnRunCallFun != NULL)
        {
            pMenu[sg_tMenuManage.pMenuCtrl->selectItem].pfnRunCallFun();
        }
    }

    return 0;
}
