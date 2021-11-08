
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
void OnMoreSetMenuEnterFunction(void);
void ShowMainMenu(const MenuShow_t *ptShowInfo);
void ShowSetMenu(const MenuShow_t *ptShowInfo);


/**************************** �����˵� *****************************************/

/* �������� */
MenuRegister_t sg_MoreSetMenuTable[] = 
{
    {"����", "Upgrade", 0, NULL, NULL, NULL, NULL, OnUpgradeFunction, NULL},
    {"����", "Language", 0, NULL, NULL, NULL, NULL, OnLanguageFunction, NULL},
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
    {"����", "Bluetooth",        0,                                  NULL,                   NULL,       NULL, NULL, OnBluetoothFunction, NULL},
    {"���", "Battery",         0,                                  NULL,                   NULL,       NULL, NULL, OnBatteryFunction, NULL},
    {"����", "Store",         0,                                  NULL,                   NULL,       NULL, NULL, OnStorageFunction, NULL},
    {"����", "More",         GET_MENU_NUM(sg_MoreSetMenuTable),  sg_MoreSetMenuTable,    ShowSetMenu,   OnMoreSetMenuEnterFunction, NULL, NULL, NULL},
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
        ShowSetMenu,    NULL,                   NULL,                   NULL, (MenuImage_t *)&sgc_CameraImage},
    
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
    printf("--------------------------\n");
    printf("     ���Թ��ܲ��Խ���\n");
    printf("--------------------------\n");
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

void OnMoreSetMenuEnterFunction(void)
{
    Menu_SetMenuShowLimit(2);
}

/* ���˵���ʾЧ�� */
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

/* ���ò˵���ʾЧ�� */
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
            printf("ѡ�����(0-�������˵�; 1-�˳�): ");
            scanf(" %d", &cmd); // �ո������Ǻ����ϴεĻس�

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
                    printf("ѡ�����(0-�˳����˵�; 2-����; 3-��һ��; 4-��һ��): ");
                }
                else
                {
                    printf("ѡ�����(0-����; 1-�������˵�; 2-����; 3-��һ��; 4-��һ��): ");
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
                printf("ѡ�����(0-����; 1-�������˵�): ");
                scanf(" %d", &cmd); // �ո������Ǻ����ϴεĻس�

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