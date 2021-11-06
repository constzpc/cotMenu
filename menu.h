/**
  **********************************************************************************************************************
  * @file    menu.h
  * @brief   该文件提供菜单框架所有函数原型
  * @author  const_zpc    any question please send mail to const_zpc@163.com
  * @version V1.0.0
  * @date    2021-11-05
  **********************************************************************************************************************
  *
  **********************************************************************************************************************
  */

/* Define to prevent recursive inclusion -----------------------------------------------------------------------------*/
#ifndef MENU_H
#define MENU_H


/* Includes ----------------------------------------------------------------------------------------------------------*/
#include <stdint.h>

#ifndef NULL
#define NULL 0
#endif



/******************************************* 配置项 ********************************************************************/

/* 多级菜单深度, 为0则表示采用 malloc/free 的方式实现多级菜单, 可无限延申, 
   否则通过数组的形式实现多级菜单,最多为 MENU_MAX_DEPTH */
#define MENU_MAX_DEPTH          0

/* 菜单支持的最大选项数目 */
#define MENU_MAX_NUM            20

/******************************************* 配置项 ********************************************************************/


/* Exported types ----------------------------------------------------------------------------------------------------*/

#if MENU_MAX_NUM < 255
typedef uint8_t menusize_t;
#else
typedef uint16_t menusize_t;
#endif


typedef enum{MENU_FALSE = 0, MENU_TRUE} menubool;
typedef void (*MenuCallFun_f)(void);

// total: 菜单项总数目; select: 所选项; pszDesc: 菜单项的字符串描述; pExtendData: 扩展数据
typedef void (*ShowMenuCallFun_f)(menusize_t total, menusize_t select, const char *pszDesc[], void *pExtendData[]); 

/**
  * @brief 菜单信息注册结构体
  * 
  */
typedef struct MenuRegister
{
    const char     *pszDesc;            /*!< 当前选项的中文字符串描述 */
    
    const char     *pszEnDesc;          /*!< 当前选项的英文字符串描述 */

    menusize_t      subMenuNum;         /*!< 当前选项的子菜单数目, 子菜单数目为0则表示下一级非菜单界面, 会执行非菜单功能函数 */

    struct MenuRegister *pSubMenu;      /*!< 当前选项的子菜单内容 */

    ShowMenuCallFun_f pfnShowMenuFun;   /*!< 当前选项的子菜单显示效果函数, 为NULL则延续上级菜单显示效果 */
    
    MenuCallFun_f     pfnEnterCallFun;  /*!< 当前选项进入所执行的函数, 只首次进入执行一次, 为NULL不执行 */
    
    MenuCallFun_f     pfnExitCallFun;   /*!< 当前选项退出所执行的函数, 只退出时执行一次, 为NULL不执行 */
    
    MenuCallFun_f     pfnMenuCallFun;   /*!< 当前选项的非菜单功能函数, 只有当菜单数目为0有效, 为NULL不执行 */

    void             *pExtendData;      /*!< 当前选项的菜单显示效果函数扩展数据入参, 可自行设置该内容 */
}MenuRegister_t;

/* Exported constants ------------------------------------------------------------------------------------------------*/
/* Exported macro ----------------------------------------------------------------------------------------------------*/

#define GET_MENU_NUM(X)    (sizeof(X) / sizeof(MenuRegister_t))

/* Exported functions ------------------------------------------------------------------------------------------------*/

extern int Menu_Init(MenuRegister_t *pMainMenu, uint8_t num, ShowMenuCallFun_f fpnShowMenu);

extern void Menu_SetEnglishLanguage(menubool isEnable);

extern int Menu_ResetMainMenu(void);

extern menubool Menu_IsMenu(void);

extern int Menu_Enter(void);
extern int Menu_Exit(uint8_t isReset);

extern int Menu_SelectPrevious(uint8_t isAllowRoll);
extern int Menu_SelectNext(uint8_t isAllowRoll);

extern int Menu_Task(void);

#endif // MENU_H
