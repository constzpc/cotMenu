
/* �˵���ʾЧ��ͼ�ɿ�: 

https://blog.csdn.net/qq_24130227/article/details/121167276 

*/

#include "menu.h"
#include <stdio.h>
#include <string.h>

// �����Ļ
#define CLEAR() printf("\033[2J")
// ��λ���
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
void ShowMainMenu(MenuShow_t *ptShowInfo);
void ShowCameraMenu(MenuShow_t *ptShowInfo);
void ShowSetMenu(MenuShow_t *ptShowInfo);
void ShowMoreSetMenu(MenuShow_t *ptShowInfo);


/**************************** �����˵� *****************************************/

/* �������� */
MenuRegister_t sg_MoreSetMenuTable[] = 
{
    {"����", "Upgrade", 0, NULL, NULL, NULL, NULL, OnUpgradeFunction, NULL},
    {"����", "About", 0, NULL, NULL, NULL, NULL, OnAboutMenuFunction, NULL},
};

/**************************** �����˵� *****************************************/

/* ������˵� */
MenuRegister_t sg_CameraMenuTable[] = 
{
    {"����", "Photo", 0, NULL, NULL, NULL, NULL, OnPhotoFunction, NULL},
    {"��Ӱ", "Camera", 0, NULL, NULL, NULL, NULL, OnCameraFunction, NULL},
};

/* ���ò˵� */
MenuRegister_t sg_SetMenuTable[] = 
{
    {"����", "Language", 0, NULL, NULL, NULL, NULL, OnLanguageFunction, NULL},
    {"����", "Bluetooth",        0,                                  NULL,                   NULL,       NULL, NULL, OnBluetoothFunction, NULL},
    {"���", "Battery",         0,                                  NULL,                   NULL,       NULL, NULL, OnBatteryFunction, NULL},
    {"����", "Store",         0,                                  NULL,                   NULL,       NULL, NULL, OnStorageFunction, NULL},
    {"����", "More",         GET_MENU_NUM(sg_MoreSetMenuTable),  sg_MoreSetMenuTable,    ShowMoreSetMenu,   NULL, NULL, NULL, NULL},
};

/**************************** һ���˵� *****************************************/


/* �Զ���ͼ������ */
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

/* ���˵� */
MenuRegister_t sg_MainMenuTable[] = 
{
    {"  ����  ", "  Music ",  0,                                  NULL,                   
        NULL,           OnMusicEnterFunction,   OnMusicExitFunction,    OnMusicFunction, (MenuImage_t *)&sgc_MusicImage},
    
    {"  ��Ƶ  ", "  Video ",  0,                                  NULL,                   
        NULL,           NULL,                   NULL,                   OnVideoFunction, (MenuImage_t *)&sgc_VideoImage},
    
    {" ����� ", " Camera ",  GET_MENU_NUM(sg_CameraMenuTable),   sg_CameraMenuTable,     
        ShowCameraMenu,    NULL,                   NULL,                   NULL, (MenuImage_t *)&sgc_CameraImage},
    
    {"  ����  ", " Setting",  GET_MENU_NUM(sg_SetMenuTable),              sg_SetMenuTable,        
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
    printf("    ���ֹ��ܲ��Խ���\n");
    printf("--------------------------\n");
}

void OnVideoFunction(void)
{
    printf("--------------------------\n");
    printf("     ��Ƶ���ܲ��Խ���\n");
    printf("--------------------------\n");
}

void OnPhotoFunction(void)
{
    printf("--------------------------\n");
    printf("     ���չ��ܲ��Խ���\n");
    printf("--------------------------\n");
}

void OnCameraFunction(void)
{
    printf("--------------------------\n");
    printf("     �����ܲ��Խ���\n");
    printf("--------------------------\n");
}

void OnUpgradeFunction(void)
{
    printf("--------------------------\n");
    printf("     �������ܲ��Խ���\n");
    printf("--------------------------\n");
}

void OnLanguageFunction(void)
{
    int cmd;

    printf("--------------------------\n");
    printf("     ���Թ��ܲ��Խ���\n");
    printf("--------------------------\n");
    
    printf("ѡ�����(0-����; 1-English): ");
    scanf(" %d", &cmd); // �ո������Ǻ����ϴεĻس�

    if (cmd == 0)
    {
        Menu_SetEnglish(MENU_FALSE);
    }
    else
    {
        Menu_SetEnglish(MENU_TRUE);
    }

    Menu_Exit(0); // �л����Զ��˳�
}

void OnAboutMenuFunction(void)
{
    printf("--------------------------\n");
    printf("     ���ڲ˵���ܽ���\n");
    printf("--------------------------\n");
}

void OnBluetoothFunction(void)
{
    printf("--------------------------\n");
    printf("     �������ܲ��Խ���\n");
    printf("--------------------------\n");
}

void OnBatteryFunction(void)
{
    printf("--------------------------\n");
    printf("     ��ع��ܲ��Խ���\n");
    printf("--------------------------\n");
}

void OnStorageFunction(void)
{
    printf("--------------------------\n");
    printf("     ���湦�ܲ��Խ���\n");
    printf("--------------------------\n");
}

/* ���˵���ʾЧ�� */
void ShowMainMenu(MenuShow_t *ptShowInfo)
{
    uint8_t showNum = 3;
    MenuImage_t *pMenuImage;
    menusize_t  tmpselect;

    Menu_UpdateShowBase(ptShowInfo, &showNum);

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

/* ������˵���ʾЧ�� */
void ShowCameraMenu(MenuShow_t *ptShowInfo)
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

/* ���ò˵���ʾЧ�� */
void ShowSetMenu(MenuShow_t *ptShowInfo)
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
            printf("\e[0;30;47m %d. %-16s\e[0m |\n", tmpselect + 1, ptShowInfo->pszItemsDesc[tmpselect]);
        }
        else
        {
            printf("\e[7;30;47m %d. %-16s\e[0m |\n", tmpselect + 1, ptShowInfo->pszItemsDesc[tmpselect]);
        }
    }
}

/* �������ò˵���ʾЧ��: �Ҳ൯���˵�Ч�� */
void ShowMoreSetMenu(MenuShow_t *ptShowInfo)
{
    uint8_t showNum = 3;
    uint8_t showsubNum = 3;
    menusize_t  tmpselect;

    MenuShow_t tParentMenuShowInfo;

    if (Menu_GetParentMenuShow(&tParentMenuShowInfo, 1) != 0)
    {
        return;
    }

    Menu_UpdateShowBase(&tParentMenuShowInfo, &showNum);
    Menu_UpdateShowBase(ptShowInfo, &showsubNum);

    printf("\e[0;30;46m ------------- %s ------------- \e[0m\n", tParentMenuShowInfo.pszDesc);

    for (int i = 0; i < showNum; i++)
    {
        tmpselect = i + tParentMenuShowInfo.showBaseItem;

        if (tmpselect == tParentMenuShowInfo.selectItem)
        {
            printf("\e[0;30;47m %d. %-16s\e[0m |", tmpselect + 1, tParentMenuShowInfo.pszItemsDesc[tmpselect]);
        }
        else
        {
            printf("\e[7;30;47m %d. %-16s\e[0m |", tmpselect + 1, tParentMenuShowInfo.pszItemsDesc[tmpselect]);
        }

        if (i < showsubNum)
        {
            tmpselect = i + ptShowInfo->showBaseItem;

            if (tmpselect == ptShowInfo->selectItem)
            {
                printf(" \e[0;30;47m %-14s\e[0m", ptShowInfo->pszItemsDesc[tmpselect]);
            }
            else
            {
                printf(" \e[7;30;47m %-14s\e[0m", ptShowInfo->pszItemsDesc[tmpselect]);
            }
        }

        printf("\n");
    }   
}


int main(int argc, char **argv)
{
    int ret, cmd = 0;
    int8_t musicMenuId, languageMenuId, moreSetMenuId;
    MainMenuCfg_t tMainMenu;
    
    tMainMenu.pszDesc = "���˵�";
    tMainMenu.pszEnDesc = "Main Menu";
    tMainMenu.pMenu = sg_MainMenuTable;
    tMainMenu.menuNum = GET_MENU_NUM(sg_MainMenuTable);
    tMainMenu.pfnEnterCallFun = NULL;
    tMainMenu.pfnShowMenuFun = ShowMainMenu;
    
    while (1)
    {
        CLEAR();
        MOVETO(0, 0);
        Menu_Task();

        /* ����ִ��һ��ԭ��: ����scanf��������, Menu_Task���ܶ�ʱִ��, �����ĳЩ��ִ�зǲ˵����ܺ���ʱ�������˲˵�����������
           ����ʵ�ַǲ˵����ܺ����Զ��˳����ܣ����ܼ�ʱ���²˵�״̬, ͨ������ִ��һ��ˢ����ʾ״̬��������� */
        CLEAR();
        MOVETO(0, 0);
        Menu_Task();

        if (!Menu_IsRun())
        {
            printf("ѡ�����(0-�������˵�; 1-�˳�): ");
            scanf(" %d", &cmd); // �ո������Ǻ����ϴεĻس�

            if (cmd == 0)
            {
                Menu_Init(&tMainMenu);

                musicMenuId = Menu_AddShortcutMenu(&sg_MainMenuTable[0]);
                languageMenuId = Menu_AddShortcutMenu(&sg_SetMenuTable[0]);
                moreSetMenuId = Menu_AddShortcutMenu(&sg_SetMenuTable[4]);
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
                    printf("ѡ�����(0-�˳����˵�; 2-����; 3-��һ��; 4-��һ��; 5-�������; 6-�������; 7-��ݸ�������): ");
                }
                else
                {
                    printf("ѡ�����(0-����; 1-�������˵�; 2-����; 3-��һ��; 4-��һ��; 5-�������; 6-�������; 7-��ݸ�������): ");
                }
                
                scanf(" %d", &cmd); // �ո������Ǻ����ϴεĻس�

                switch (cmd)
                {
                case 0:
                    if (Menu_IsMainMenu())
                    {
                        Menu_DeInit();
                    }
                    else
                    {
                        Menu_Exit(MENU_TRUE);
                    }
                    break;
                case 1:
                    if (!Menu_IsMainMenu())
                    {
                        Menu_Reset();
                    }
                    break;
                case 2:
                    Menu_Enter();
                    break;
                case 3:
                    Menu_SelectNext(MENU_TRUE);
                    break;
                case 4:
                    Menu_SelectPrevious(MENU_TRUE);
                    break;

                case 5:
                    Menu_EnterShortcutMenu(musicMenuId);
                    break;

                case 6:
                    Menu_EnterShortcutMenu(languageMenuId);
                    break;

                case 7:
                    Menu_EnterShortcutMenu(moreSetMenuId);
                    break;
                    
                default:
                    break;    
                }
            }
            else
            {
                printf("ѡ�����(0-����): ");
                scanf(" %d", &cmd); // �ո������Ǻ����ϴεĻس�

                if (cmd == 0)
                {
                    Menu_Exit(0); 
                }
            }
        }
    }

    return 0;
}