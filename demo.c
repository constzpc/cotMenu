
#include "menu.h"
#include <stdio.h>
#include <string.h>

// 清除屏幕
#define CLEAR() printf("\033[2J")
// 定位光标
#define MOVETO(x,y) printf("\033[%d;%dH", (x), (y))

void OnMusicEnterFunction(void);
void OnMusicExitFunction(void);
void OnMusicFunction(void);
void OnVideoFunction(void);
void OnPhotoFunction(void);
void OnCameraFunction(void);
void OnUpgradeFunction(void);
void OnLanguageFunction(void);
void OnAboutMenuFunction(void);
void OnBluetoothFunction(void);
void OnBatteryFunction(void);
void OnStorageFunction(void);
void OnMoreSetMenuEnterFunction(void);
void ShowMainMenu(const MenuShow_t *ptShowInfo);
void ShowSetMenu(const MenuShow_t *ptShowInfo);


/**************************** 三级菜单 *****************************************/

/* 更多设置 */
MenuRegister_t sg_MoreSetMenuTable[] = 
{
    {"升级", "Upgrade", 0, NULL, NULL, NULL, NULL, OnUpgradeFunction, NULL},
    {"语言", "Language", 0, NULL, NULL, NULL, NULL, OnLanguageFunction, NULL},
    {"关于", "About", 0, NULL, NULL, NULL, NULL, OnAboutMenuFunction, NULL},
};

/**************************** 二级菜单 *****************************************/

/* 摄像机菜单 */
MenuRegister_t sg_CameraMenuTable[] = 
{
    {"拍照", "Photo", 0, NULL, NULL, NULL, NULL, OnPhotoFunction, NULL},
    {"摄影", "Camera", 0, NULL, NULL, NULL, NULL, OnCameraFunction, NULL},
};

/* 设置菜单 */
MenuRegister_t sg_SetMenuTable[] = 
{
    {"蓝牙", "Bluetooth",        0,                                  NULL,                   NULL,       NULL, NULL, OnBluetoothFunction, NULL},
    {"电池", "Battery",         0,                                  NULL,                   NULL,       NULL, NULL, OnBatteryFunction, NULL},
    {"储存", "Store",         0,                                  NULL,                   NULL,       NULL, NULL, OnStorageFunction, NULL},
    {"更多", "More",         GET_MENU_NUM(sg_MoreSetMenuTable),  sg_MoreSetMenuTable,    ShowSetMenu,   OnMoreSetMenuEnterFunction, NULL, NULL, NULL},
};

/**************************** 一级菜单 *****************************************/


/* 自定义图标数据 */
typedef struct demo
{
    const char *pImageFrame;
    const char *pImage;
} MenuImage_t;

const MenuImage_t sgc_MusicImage = {
"mmmmmmmmmm",
"@"
};

const MenuImage_t sgc_VideoImage = {
"vvvvvvvvvv",
"#"
};

const MenuImage_t sgc_CameraImage = {
"**********",
"&"
};

const MenuImage_t sgc_SettingImage = {
"$$$$$$$$$$",
"%"
};

/* 主菜单 */
MenuRegister_t sg_MainMenuTable[] = 
{
    {"  音乐  ", "  Music ",  0,                                  NULL,                   
        NULL,           OnMusicEnterFunction,   OnMusicExitFunction,    OnMusicFunction, (MenuImage_t *)&sgc_MusicImage},
    
    {"  视频  ", "  Video ",  0,                                  NULL,                   
        NULL,           NULL,                   NULL,                   OnVideoFunction, (MenuImage_t *)&sgc_VideoImage},
    
    {" 摄像机 ", " Camera ",  GET_MENU_NUM(sg_CameraMenuTable),   sg_CameraMenuTable,     
        ShowSetMenu,    NULL,                   NULL,                   NULL, (MenuImage_t *)&sgc_CameraImage},
    
    {"  设置  ", " Setting",  GET_MENU_NUM(sg_SetMenuTable),              sg_SetMenuTable,        
        ShowSetMenu,    NULL,                   NULL,                   NULL, (MenuImage_t *)&sgc_SettingImage},
};

void OnMusicEnterFunction(void)
{
    printf("--------------------------\n");
}

void OnMusicExitFunction(void)
{

}

void OnMusicFunction(void)
{
    //printf("--------------------------\n");
    printf("    音乐功能测试界面\n");
    printf("--------------------------\n");
}

void OnVideoFunction(void)
{
    printf("--------------------------\n");
    printf("     视频功能测试界面\n");
    printf("--------------------------\n");
}

void OnPhotoFunction(void)
{
    printf("--------------------------\n");
    printf("     拍照功能测试界面\n");
    printf("--------------------------\n");
}

void OnCameraFunction(void)
{
    printf("--------------------------\n");
    printf("     摄像功能测试界面\n");
    printf("--------------------------\n");
}

void OnUpgradeFunction(void)
{
    printf("--------------------------\n");
    printf("     升级功能测试界面\n");
    printf("--------------------------\n");
}

void OnLanguageFunction(void)
{
    printf("--------------------------\n");
    printf("     语言功能测试界面\n");
    printf("--------------------------\n");
}

void OnAboutMenuFunction(void)
{
    printf("--------------------------\n");
    printf("     关于菜单框架界面\n");
    printf("--------------------------\n");
}

void OnBluetoothFunction(void)
{
    printf("--------------------------\n");
    printf("     蓝牙功能测试界面\n");
    printf("--------------------------\n");
}

void OnBatteryFunction(void)
{
    printf("--------------------------\n");
    printf("     电池功能测试界面\n");
    printf("--------------------------\n");
}

void OnStorageFunction(void)
{
    printf("--------------------------\n");
    printf("     储存功能测试界面\n");
    printf("--------------------------\n");
}

void OnMoreSetMenuEnterFunction(void)
{
    Menu_SetMenuShowLimit(2);
}

/* 主菜单显示效果 */
void ShowMainMenu(const MenuShow_t *ptShowInfo)
{
    MenuImage_t *pMenuImage;
    menusize_t  tmpselect;

    for (int i = 0; i < ptShowInfo->show.num; i++)
    {
        tmpselect = i + ptShowInfo->show.base;
        pMenuImage = (MenuImage_t *)ptShowInfo->pExtendData[tmpselect];

        if (tmpselect == ptShowInfo->select)
        {
            printf("\e[0;30;47m %-10s \e[0m", pMenuImage->pImageFrame);
        }
        else
        {
            printf("\e[7;30;47m %-10s \e[0m", pMenuImage->pImageFrame);
        }
    }

    printf("\n");

    for (int i = 0; i < ptShowInfo->show.num; i++)
    {
        tmpselect = i + ptShowInfo->show.base;
        pMenuImage = (MenuImage_t *)ptShowInfo->pExtendData[tmpselect];

        if (tmpselect == ptShowInfo->select)
        {
            printf("\e[0;30;47m %-s%-8s%-s \e[0m", pMenuImage->pImage, ptShowInfo->pszDesc[tmpselect], pMenuImage->pImage);
        }
        else
        {
            printf("\e[7;30;47m %-s%-8s%-s \e[0m", pMenuImage->pImage, ptShowInfo->pszDesc[tmpselect], pMenuImage->pImage);
        }
    }

    printf("\n");

    for (int i = 0; i < ptShowInfo->show.num; i++)
    {
        tmpselect = i + ptShowInfo->show.base;
        pMenuImage = (MenuImage_t *)ptShowInfo->pExtendData[tmpselect];

        if (tmpselect == ptShowInfo->select)
        {
            printf("\e[0;30;47m %-10s \e[0m", pMenuImage->pImageFrame);
        }
        else
        {
            printf("\e[7;30;47m %-10s \e[0m", pMenuImage->pImageFrame);
        }
    }

    printf("\n");
}

/* 设置菜单显示效果 */
void ShowSetMenu(const MenuShow_t *ptShowInfo)
{
    menusize_t  tmpselect;

    for (int i = 0; i < ptShowInfo->show.num; i++)
    {
        tmpselect = i + ptShowInfo->show.base;
        
        if (tmpselect == ptShowInfo->select)
        {
            printf("> %-10s\n", ptShowInfo->pszDesc[tmpselect]);
        }
        else
        {
            printf("  %-10s\n", ptShowInfo->pszDesc[tmpselect]);
        }
    }
}


int main(int argc, char **argv)
{
    int ret, cmd = 0;

    while (1)
    {
        CLEAR();
        MOVETO(0, 0);
        Menu_Task();

        if (!Menu_IsRun())
        {
            printf("选择操作(0-进入主菜单; 1-退出): ");
            scanf(" %d", &cmd); // 空格作用是忽略上次的回车

            if (cmd == 0)
            {
                Menu_Init(sg_MainMenuTable, GET_MENU_NUM(sg_MainMenuTable), ShowMainMenu); 
                Menu_SetMenuShowLimit(3);
            }
            else if (cmd == 1)
            {
                return 0;
            }
        }
        else
        {
            if (Menu_IsAtMenu())
            {
                if (Menu_IsMainMenu())
                {
                    printf("选择操作(0-退出主菜单; 2-进入; 3-下一个; 4-上一个): ");
                }
                else
                {
                    printf("选择操作(0-返回; 1-返回主菜单; 2-进入; 3-下一个; 4-上一个): ");
                }
                
                scanf(" %d", &cmd); // 空格作用是忽略上次的回车

                switch (cmd)
                {
                case 0:
                    if (Menu_IsMainMenu())
                    {
                        Menu_DeInit();
                    }
                    else
                    {
                        Menu_Exit(1);
                    }
                    break;
                case 1:
                    if (!Menu_IsMainMenu())
                    {
                        Menu_ResetMainMenu();
                    }
                    break;
                case 2:
                    Menu_Enter();
                    break;
                case 3:
                    Menu_SelectNext(1);
                    break;
                case 4:
                    Menu_SelectPrevious(1);
                    break;
                default:
                    break;    
                }
            }
            else
            {
                printf("选择操作(0-返回; 1-返回主菜单): ");
                scanf(" %d", &cmd); // 空格作用是忽略上次的回车

                if (cmd == 0)
                {
                    Menu_Exit(0); 
                }
                else if (cmd == 1)
                {
                    Menu_ResetMainMenu();;
                }
            }
        }
    }

    return 0;
}