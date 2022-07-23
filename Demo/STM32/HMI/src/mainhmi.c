#include "mainhmi.h"
#include "hmi_camera.h"
#include "hmi_music.h"
#include "hmi_set.h"
#include "hmi_video.h"
#include "hmi_common.h"
#include "menu.h"
#include "key.h"
#include <string.h>

static bool sg_isInit = false;

/* 主菜单 */
MenuList_t sg_MainMenuTable[] = 
{
    {"音乐", "Music",  Hmi_MusicLoad, Hmi_MusicExit, Hmi_MusicTask, "music"},
    
    {"视频", "Video",  Hmi_VideoLoad, Hmi_VideoExit, Hmi_VideoTask, "video"},
    
    {"摄像机", "Camera",  Hmi_CameraLoad, Hmi_CameraExit, Hmi_CameraTask, "camera"},
    
    {"设置", "Setting",  Hmi_SetLoad,   Hmi_SetExit,   Hmi_SetTask, "setting"},
};



/* 主菜单显示效果 */
static void ShowMainMenu(MenuShow_t *ptShowInfo)
{
    char *pszSelectDesc = ptShowInfo->pszItemsDesc[ptShowInfo->selectItem];
    oledsize_t idx = (128 - 6 * strlen(pszSelectDesc)) / 2;

    OLED_DrawGraphic(40, 0, (const char *)ptShowInfo->pItemsExData[ptShowInfo->selectItem], 1);
    
    OLED_SetText(0, 50, "                ", 0, FONT_12X12);
    OLED_SetText(idx, 50, pszSelectDesc, 0, FONT_12X12);
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

void Hmi_LoadMainHmi(void)
{
    KEY_Register(IO_KEY_0, OnKeyFunCB);
    KEY_Register(IO_KEY_1, OnKeyFunCB);
    KEY_Register(IO_KEY_UP, OnKeyFunCB);
    
    Menu_BingMenu(sg_MainMenuTable, GET_MENU_NUM(sg_MainMenuTable), ShowMainMenu);
    sg_isInit = true;
}

void Hmi_MainTask(void)
{
    if (sg_isInit)
    {
        sg_isInit = false;
        OLED_Clear(OLED_BLACK);
    }
    
}
