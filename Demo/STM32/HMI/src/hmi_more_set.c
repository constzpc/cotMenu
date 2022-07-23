#include "hmi_more_set.h"
#include "menu.h"
#include "key.h"
#include <string.h>

static void OnUpgradeFunctionLoad(void);
static void OnAboutMenuFunctionLoad(void);
static void OnUpgradeFunction(void);
static void OnAboutMenuFunction(void);

static bool sg_isInit = false;

/* �������� */
MenuList_t sg_MoreSetMenuTable[] = 
{
    {"����", "Upgrade", OnUpgradeFunctionLoad, NULL, OnUpgradeFunction, NULL},
    {"����", "About",   OnAboutMenuFunctionLoad, NULL, OnAboutMenuFunction, NULL},
};

/* �������ò˵���ʾЧ��: �Ҳ൯���˵�Ч�� */
static void ShowMoreSetMenu(MenuShow_t *ptShowInfo)
{
    uint8_t showNum = 3;
    menusize_t  tmpselect;
    char szDesc[17]; 
    oledsize_t idx = (128 - 8 * strlen(ptShowInfo->pszDesc)) / 2;

    MenuShow_t tParentMenuShowInfo;

    if (Menu_GetParentMenuShow(&tParentMenuShowInfo, 1) != 0)
    {
        return;
    }
    
    Menu_UpdateShowBase(&tParentMenuShowInfo, &showNum);
    
    OLED_SetText(idx, 0, tParentMenuShowInfo.pszDesc, 0, FONT_16X16);

    for (int i = 0; i < showNum; i++)
    {
        tmpselect = i + tParentMenuShowInfo.showBaseItem;

        if (tmpselect == tParentMenuShowInfo.selectItem)
        {
            OLED_SetColor(OLED_WHITE, OLED_BLACK);
        }
        else
        {
            OLED_SetColor(OLED_BLACK, OLED_WHITE);
        }
        
        memset(szDesc, ' ', 16);
        memcpy(&szDesc[1], tParentMenuShowInfo.pszItemsDesc[tmpselect], strlen(tParentMenuShowInfo.pszItemsDesc[tmpselect]));
        //szDesc[6] = '|';
        szDesc[6] = '\0';
        OLED_SetText(0, 16 + i * 16, szDesc, 0, FONT_16X16);
        
        OLED_SetColor(OLED_BLACK, OLED_WHITE);
        OLED_SetText(50, 16 + i * 16, "|", 0, FONT_16X16);
    }
    
    OLED_SetColor(OLED_BLACK, OLED_WHITE);
    
    showNum = 3;
    Menu_UpdateShowBase(ptShowInfo, &showNum);
    
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
        memcpy(&szDesc[1], ptShowInfo->pszItemsDesc[tmpselect], strlen(ptShowInfo->pszItemsDesc[tmpselect]));
        szDesc[7] = '\0';
        OLED_SetText(60, 16 + i * 16, szDesc, 0, FONT_16X16);
    }
    
    OLED_SetColor(OLED_BLACK, OLED_WHITE);
}

static void OnKeyFunCB(IoKeyType_e key, KeyAction_e action)
{
    if (action == KEY_ACTION_LOSSEN)
    {
        if (key == IO_KEY_0)
        {
            Menu_Exit(false);
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

void Hmi_MoreSetLoad(void)
{
    sg_isInit = true;
    KEY_Register(IO_KEY_0, OnKeyFunCB);
    KEY_Register(IO_KEY_1, OnKeyFunCB);
    KEY_Register(IO_KEY_UP, OnKeyFunCB);
    
    Menu_BingMenu(sg_MoreSetMenuTable, GET_MENU_NUM(sg_MoreSetMenuTable), ShowMoreSetMenu);
}

void Hmi_MoreSetExit(void)
{

}

void Hmi_MoreSetTask(void)
{
    if (sg_isInit)
    {
        sg_isInit = false;
        OLED_Clear(OLED_BLACK);
    }
}


static void OnUpgradeFunctionLoad(void)
{
    sg_isInit = true;
    KEY_UnRegister(IO_KEY_1);
    KEY_UnRegister(IO_KEY_UP);
}

static void OnUpgradeFunction(void)
{
    if (sg_isInit)
    {
        sg_isInit = false;
        OLED_Clear(OLED_BLACK);
        OLED_SetText(0, 0, "----��������----", 0, FONT_16X16);
    }
}

static void OnAboutMenuFunctionLoad(void)
{
    sg_isInit = true;
    KEY_UnRegister(IO_KEY_1);
    KEY_UnRegister(IO_KEY_UP);
}

static void OnAboutMenuFunction(void)
{
    if (sg_isInit)
    {
        sg_isInit = false;
        OLED_Clear(OLED_BLACK);
        OLED_SetText(0, 0, "----���ڲ˵�----", 0, FONT_16X16);
    }
}
