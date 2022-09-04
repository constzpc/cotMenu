#include "hmi_camera.h"
#include "menu.h"
#include "key.h"
#include <string.h>

static void OnPhotoFunctionLoad(void);
static void OnPhotoFunctionTask(void);
static void OnCameraFunctionLoad(void);
static void OnCameraFunctionTask(void);

/* 摄像机菜单 */
MenuList_t sg_CameraMenuTable[] = 
{
    {"拍照", "Photo", OnPhotoFunctionLoad, NULL, OnPhotoFunctionTask, NULL},
    {"摄影", "Camera", OnCameraFunctionLoad, NULL, OnCameraFunctionTask, NULL},
};

static bool sg_isInit = false;

/* 摄像机菜单显示效果 */
static void ShowCameraMenu(MenuShow_t *ptShowInfo)
{
    char *pszSelectDesc = ptShowInfo->pszItemsDesc[ptShowInfo->selectItem];
    oledsize_t idx = (128 - 8 * strlen(pszSelectDesc)) / 2;

    OLED_DrawCircle(64, 32, 26, 4);
    OLED_SetText(0, 24, "                ", 0, FONT_16X16);
    OLED_SetText(idx, 24, pszSelectDesc, 0, FONT_16X16);
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

void Hmi_CameraLoad(void)
{
    sg_isInit = true;
    KEY_Register(IO_KEY_0, OnKeyFunCB);
    KEY_Register(IO_KEY_1, OnKeyFunCB);
    KEY_Register(IO_KEY_UP, OnKeyFunCB);
    
    Menu_Bind(sg_CameraMenuTable, GET_MENU_NUM(sg_CameraMenuTable), ShowCameraMenu);
}

void Hmi_CameraExit(void)
{

}

void Hmi_CameraTask(void)
{
    if (sg_isInit)
    {
        sg_isInit = false;
        OLED_Clear(OLED_BLACK);
    }
}

static void OnPhotoFunctionLoad(void)
{
    sg_isInit = true;
    KEY_UnRegister(IO_KEY_1);
    KEY_UnRegister(IO_KEY_UP);
}

static void OnPhotoFunctionTask(void)
{
    if (sg_isInit)
    {
        sg_isInit = false;
        OLED_Clear(OLED_BLACK);
        OLED_SetText(0, 0, "----拍照界面----", 0, FONT_16X16);
    }
    
}

static void OnCameraFunctionLoad(void)
{
    sg_isInit = true;
    KEY_UnRegister(IO_KEY_1);
    KEY_UnRegister(IO_KEY_UP);
}

static void OnCameraFunctionTask(void)
{
    if (sg_isInit)
    {
        sg_isInit = false;
        OLED_Clear(OLED_BLACK);
        OLED_SetText(0, 0, "----摄像界面----", 0, FONT_16X16);
    }
}
