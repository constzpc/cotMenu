#include "hmi_camera.h"
#include "menu.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

const MenuImage_t sgc_CameraImage = {
"**********",
"&"
};


static void OnPhotoFunctionTask(void);
static void OnCameraFunctionTask(void);

/* 摄像机菜单 */
MenuList_t sg_CameraMenuTable[] = 
{
    {"拍照", "Photo", NULL, NULL, OnPhotoFunctionTask, NULL},
    {"摄影", "Camera", NULL, NULL, OnCameraFunctionTask, NULL},
};

static bool sg_isInit = false;

/* 摄像机菜单显示效果 */
static void ShowCameraMenu(MenuShow_t *ptShowInfo)
{
    uint8_t showNum = 3;
    menusize_t  tmpselect;

    Menu_UpdateShowBase(ptShowInfo, &showNum);

    printf("\e[0;30;46m ------------- %s ------------- \e[0m\n", ptShowInfo->pszDesc);

    for (int i = 0; i < showNum; i++)
    {
        tmpselect = i + ptShowInfo->showBaseItem;

        if (tmpselect == ptShowInfo->selectItem)
        {
            printf("\e[0;30;47m %d. %-34s\e[0m\n", tmpselect + 1, ptShowInfo->pszItemsDesc[tmpselect]);
        }
        else
        {
            printf("\e[7;30;47m %d. %-34s\e[0m\n", tmpselect + 1, ptShowInfo->pszItemsDesc[tmpselect]);
        }
    }
}

void Hmi_CameraLoad(void)
{
        CLEAR();
        MOVETO(0, 0);
    printf("---加载-----\n");
    sg_isInit = true;
    Menu_BingMenu(sg_CameraMenuTable, GET_MENU_NUM(sg_CameraMenuTable), ShowCameraMenu);
}

void Hmi_CameraExit(void)
{
    printf("--------------------------\n");
    printf("     视频正在停止播放...\n");
    printf("--------------------------\n");
    sleep(1);
}

void Hmi_CameraTask(void)
{
    int cmd;

    printf("选择操作(0-返回; 1-返回主菜单; 2-进入; 3-下一个; 4-上一个): ");
    scanf(" %d", &cmd); // 空格作用是忽略上次的回车
 
    switch (cmd)
    {
    case 0:
        Menu_Exit(true);
        break;
    case 1:
        Menu_Reset();
        break;
    case 2:
        Menu_Enter();
        break;
    case 3:
        Menu_SelectNext(true);
        break;
    case 4:
        Menu_SelectPrevious(true);
        break;

    default:
        break;    
    }
}



static void OnPhotoFunctionTask(void)
{
    int cmd = 0;

    printf("--------------------------\n");
    printf("     拍照功能测试界面\n");
    printf("--------------------------\n");

    printf("选择操作(0-退出): ");
    scanf(" %d", &cmd); // 空格作用是忽略上次的回车

    if (cmd == 0)
    {
        Menu_Exit(false);
    }
}

static void OnCameraFunctionTask(void)
{
    int cmd = 0;

    printf("--------------------------\n");
    printf("     摄像功能测试界面\n");
    printf("--------------------------\n");

    printf("选择操作(0-退出): ");
    scanf(" %d", &cmd); // 空格作用是忽略上次的回车

    if (cmd == 0)
    {
        Menu_Exit(false);
    }
}