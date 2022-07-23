#include "hmi_set.h"
#include "hmi_more_set.h"
#include "menu.h"
#include "key.h"
#include <string.h>

static bool sg_isInit = false;


static void OnLanguageFunctionLoad(void);
static void OnLanguageFunction(void);
static void OnBluetoothFunctionLoad(void);
static void OnBluetoothFunction(void);
static void OnBatteryFunctionLoad(void);
static void OnBatteryFunction(void);
static void OnStorageFunctionLoad(void);
static void OnStorageFunction(void);

/* 设置菜单 */
MenuList_t sg_SetMenuTable[] = 
{
    {"语言", "Language",   OnLanguageFunctionLoad, NULL, OnLanguageFunction, NULL},
    {"蓝牙", "Bluetooth",  OnBluetoothFunctionLoad, NULL, OnBluetoothFunction, NULL},
    {"电池", "Battery",    OnBatteryFunctionLoad, NULL, OnBatteryFunction, NULL},
    {"储存", "Store",      OnStorageFunctionLoad, NULL, OnStorageFunction, NULL},
    {"更多", "More",       Hmi_MoreSetLoad, Hmi_MoreSetExit, Hmi_MoreSetTask, NULL},
};

/* 设置菜单显示效果 */
static void ShowSetMenu(MenuShow_t *ptShowInfo)
{
    uint8_t showNum = 3;
    menusize_t  tmpselect;
    char szDesc[17]; 
    oledsize_t idx = (128 - 8 * strlen(ptShowInfo->pszDesc)) / 2;

    Menu_LimitShowListNum(ptShowInfo, &showNum);
    
    OLED_SetText(idx, 0, ptShowInfo->pszDesc, 0, FONT_16X16);

    for (int i = 0; i < showNum; i++)
    {
        tmpselect = i + ptShowInfo->showBaseItem;

        if (tmpselect == ptShowInfo->selectItem)
        {
            OLED_SetColor(OLED_WHITE, OLED_BLACK);
        }
        else
        {
            OLED_SetColor(OLED_BLACK, OLED_WHITE);
        }
        
        memset(szDesc, ' ', 16);
        szDesc[16] = '0';
        memcpy(&szDesc[1], ptShowInfo->pszItemsDesc[tmpselect], strlen(ptShowInfo->pszItemsDesc[tmpselect]));
        OLED_SetText(0, 16 + i * 16, szDesc, 0, FONT_16X16);
    }
    
    OLED_SetColor(OLED_BLACK, OLED_WHITE);
}

static void OnKeyFunCB(IoKeyType_e key, KeyAction_e action)
{
    if (action == KEY_ACTION_LOSSEN)
    {
        if (key == IO_KEY_0)
        {
            Menu_Exit(true);
        }
        else if (key == IO_KEY_1)
        {
            Menu_Enter();
        }
        else if (key == IO_KEY_UP)
        {
            Menu_SelectNext(true);
        }
    }
}


void Hmi_SetLoad(void)
{
    sg_isInit = true;
    KEY_Register(IO_KEY_0, OnKeyFunCB);
    KEY_Register(IO_KEY_1, OnKeyFunCB);
    KEY_Register(IO_KEY_UP, OnKeyFunCB);
    
    Menu_BingMenu(sg_SetMenuTable, GET_MENU_NUM(sg_SetMenuTable), ShowSetMenu);
}

void Hmi_SetExit(void)
{

}

void Hmi_SetTask(void)
{
    if (sg_isInit)
    {
        sg_isInit = false;
        OLED_Clear(OLED_BLACK);
    }
}

/****************************** 
              语言 
*******************************/

static void OnChangeChinese(void);
static void OnChangeEnglish(void);

MenuList_t sg_LanguageMenuTable[] = 
{
    {"中文", "中文",   NULL, NULL, OnChangeChinese, NULL},
    {"English", "English",  NULL, NULL, OnChangeEnglish, NULL},
};

static void OnKeyFunLanguageCB(IoKeyType_e key, KeyAction_e action)
{
    if (action == KEY_ACTION_LOSSEN)
    {
        if (key == IO_KEY_0)
        {
            Menu_Exit(true);
        }
        else if (key == IO_KEY_1)
        {
            Menu_Enter();
        }
        else if (key == IO_KEY_UP)
        {
            Menu_SelectNext(true);
        }
    }
}

static void OnLanguageFunctionLoad(void)
{
    sg_isInit = true;
    KEY_Register(IO_KEY_0, OnKeyFunLanguageCB);
    KEY_Register(IO_KEY_1, OnKeyFunLanguageCB);
    KEY_Register(IO_KEY_UP, OnKeyFunLanguageCB);
    Menu_BingMenu(sg_LanguageMenuTable, GET_MENU_NUM(sg_LanguageMenuTable), NULL);
}

static void OnLanguageFunction(void)
{
    if (sg_isInit)
    {
        sg_isInit = false;
        OLED_Clear(OLED_BLACK);
    }
}

static void OnChangeChinese(void)
{
    Menu_SetEnglish(false);
    
    // 切换完成自动退出
    Menu_Exit(false); // 退出到语言设置界面
    Menu_Exit(false); // 退出到设置界面
}

static void OnChangeEnglish(void)
{
    Menu_SetEnglish(true);
    
    // 切换完成自动退出
    Menu_Exit(false); // 退出到语言设置界面
    Menu_Exit(false); // 退出到设置界面
}

/****************************** 
              蓝牙 
*******************************/
static void OnBluetoothFunctionLoad(void)
{
    sg_isInit = true;
    KEY_UnRegister(IO_KEY_1);
    KEY_UnRegister(IO_KEY_UP);
}

static void OnBluetoothFunction(void)
{
    if (sg_isInit)
    {
        sg_isInit = false;
        OLED_Clear(OLED_BLACK);
        OLED_SetText(0, 0, "----蓝牙测试----", 0, FONT_16X16);
    }
}

/****************************** 
              电池 
*******************************/
static void OnBatteryFunctionLoad(void)
{
    sg_isInit = true;
    KEY_UnRegister(IO_KEY_1);
    KEY_UnRegister(IO_KEY_UP);
}

static void OnBatteryFunction(void)
{
    if (sg_isInit)
    {
        sg_isInit = false;
        OLED_Clear(OLED_BLACK);
        OLED_SetText(0, 0, "----电池测试----", 0, FONT_16X16);
    }
}

/****************************** 
              储存 
*******************************/
static void OnStorageFunctionLoad(void)
{
    sg_isInit = true;
    KEY_UnRegister(IO_KEY_1);
    KEY_UnRegister(IO_KEY_UP);
}

static void OnStorageFunction(void)
{
    if (sg_isInit)
    {
        sg_isInit = false;
        OLED_Clear(OLED_BLACK);
        OLED_SetText(0, 0, "----储存界面----", 0, FONT_16X16);
    }
}
