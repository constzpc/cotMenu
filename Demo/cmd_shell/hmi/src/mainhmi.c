#include "mainhmi.h"
#include "hmi_camera.h"
#include "hmi_music.h"
#include "hmi_set.h"
#include "hmi_video.h"
#include "hmi_common.h"
#include "menu.h"
#include <stdio.h>
#include <string.h>



/* 主菜单 */
MenuList_t sg_MainMenuTable[] = 
{
    {"  音乐  ", "  Music ",  Hmi_MusicLoad, Hmi_MusicExit, Hmi_MusicTask, (MenuImage_t *)&sgc_MusicImage},
    
    {"  视频  ", "  Video ",  Hmi_VideoLoad, Hmi_VideoExit, Hmi_VideoTask, (MenuImage_t *)&sgc_VideoImage},
    
    {" 摄像机 ", " Camera ",  Hmi_CameraLoad, Hmi_CameraExit, Hmi_CameraTask, (MenuImage_t *)&sgc_CameraImage},
    
    {"  设置  ", " Setting",  Hmi_SetLoad,   Hmi_SetExit,   Hmi_SetTask, (MenuImage_t *)&sgc_SettingImage},
};



/* 主菜单显示效果 */
static void ShowMainMenu(MenuShow_t *ptShowInfo)
{
    uint8_t showNum = 3;
    MenuImage_t *pMenuImage;
    menusize_t  tmpselect;

    Menu_LimitShowListNum(ptShowInfo, &showNum);

    printf("\e[0;30;47m ------------- %s ------------- \e[0m\n", ptShowInfo->pszDesc);

    for (int i = 0; i < showNum; i++)
    {
        tmpselect = i + ptShowInfo->showBaseItem;
        pMenuImage = (MenuImage_t *)ptShowInfo->pItemsExData[tmpselect];

        if (tmpselect == ptShowInfo->selectItem)
        {
            printf("\e[0;30;47m %-10s \e[0m", pMenuImage->pImageFrame);
        }
        else
        {
            printf("\e[7;30;47m %-10s \e[0m", pMenuImage->pImageFrame);
        }
    }

    printf("\n");

    for (int i = 0; i < showNum; i++)
    {
        tmpselect = i + ptShowInfo->showBaseItem;
        pMenuImage = (MenuImage_t *)ptShowInfo->pItemsExData[tmpselect];

        if (tmpselect == ptShowInfo->selectItem)
        {
            printf("\e[0;30;47m %-s%-8s%-s \e[0m", pMenuImage->pImage, ptShowInfo->pszItemsDesc[tmpselect], pMenuImage->pImage);
        }
        else
        {
            printf("\e[7;30;47m %-s%-8s%-s \e[0m", pMenuImage->pImage, ptShowInfo->pszItemsDesc[tmpselect], pMenuImage->pImage);
        }
    }

    printf("\n");

    for (int i = 0; i < showNum; i++)
    {
        tmpselect = i + ptShowInfo->showBaseItem;
        pMenuImage = (MenuImage_t *)ptShowInfo->pItemsExData[tmpselect];

        if (tmpselect == ptShowInfo->selectItem)
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

void Hmi_LoadMainHmi(void)
{
    Menu_BingMenu(sg_MainMenuTable, GET_MENU_NUM(sg_MainMenuTable), ShowMainMenu);
}

void Hmi_MainTask(void)
{
    int cmd;

    printf("选择操作(0-退出主菜单; 2-进入; 3-下一个; 4-上一个; 5-快捷音乐; 6-快捷语言; 7-快捷更多设置): ");
    scanf(" %d", &cmd); // 空格作用是忽略上次的回车
 
    switch (cmd)
    {
    case 0:
        Menu_DeInit();
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

    // case 5:
    //     Menu_EnterShortcutMenu(musicMenuId);
    //     break;

    // case 6:
    //     Menu_EnterShortcutMenu(languageMenuId);
    //     break;

    // case 7:
    //     Menu_EnterShortcutMenu(moreSetMenuId);
    //     break;
        
    default:
        break;    
    }
            
}