
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
void ShowMainMenu(menusize_t total, menusize_t select, const char *pszDesc[]);
void ShowSetMenu(menusize_t total, menusize_t select, const char *pszDesc[]);


/**************************** �����˵� *****************************************/

/* �������� */
MenuRegister_t sg_MoreSetMenuTable[] = 
{
    {"����", "Upgrade", 0, NULL, NULL, OnUpgradeFunction},
    {"����", "Language", 0, NULL, NULL, OnLanguageFunction},
    {"����", "About", 0, NULL, NULL, OnAboutMenuFunction},
};

/**************************** �����˵� *****************************************/

/* ������˵� */
MenuRegister_t sg_CameraMenuTable[] = 
{
    {"����", "Photo", 0, NULL, NULL, OnPhotoFunction},
    {"��Ӱ", "Camera", 0, NULL, NULL, OnCameraFunction},
};

/* ���ò˵� */
MenuRegister_t sg_SetMenuTable[] = 
{
    {"����", "Bluetooth",        0,                                  NULL,                   NULL,       OnBluetoothFunction},
    {"���", "Battery",         0,                                  NULL,                   NULL,       OnBatteryFunction},
    {"����", "Store",         0,                                  NULL,                   NULL,       OnStorageFunction},
    {"����", "More",         GET_MENU_NUM(sg_MoreSetMenuTable),  sg_MoreSetMenuTable,    ShowSetMenu,   NULL},
};

/**************************** һ���˵� *****************************************/
/* ���˵� */
MenuRegister_t sg_MainMenuTable[] = 
{
    {"  ����  ", "  Music ",  0,                                  NULL,                   
        NULL,           OnMusicEnterFunction,   OnMusicExitFunction,    OnMusicFunction},
    
    {"  ��Ƶ  ", "  Video ",  0,                                  NULL,                   
        NULL,           NULL,                   NULL,                   OnVideoFunction},
    
    {" ����� ", " Camera ",  GET_MENU_NUM(sg_CameraMenuTable),   sg_CameraMenuTable,     
        ShowSetMenu,    NULL,                   NULL,                   NULL},
    
    {"  ����  ", " Setting",  GET_MENU_NUM(sg_SetMenuTable),              sg_SetMenuTable,        
        ShowSetMenu,    NULL,                   NULL,                   NULL},
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

/* ���˵���ʾЧ�� */
void ShowMainMenu(menusize_t total, menusize_t select, const char *pszDesc[])
{
    for (int i = 0; i < total; i++)
    {
        if (i == select)
        {
            printf("\e[0;30;47m %-10s \e[0m", pszDesc[i]);
        }
        else
        {
            printf("\e[7;30;47m %-10s \e[0m", pszDesc[i]);
        }
    }

    printf("\n");
}

/* ���ò˵���ʾЧ�� */
void ShowSetMenu(menusize_t total, menusize_t select, const char *pszDesc[])
{
    for (int i = 0; i < total; i++)
    {
        if (i == select)
        {
            printf("> %-10s\n", pszDesc[i]);
        }
        else
        {
            printf("  %-10s\n", pszDesc[i]);
        }
    }
}



int main(int argc, char **argv)
{
    int ret, cmd = 0;

    Menu_Init(sg_MainMenuTable, GET_MENU_NUM(sg_MainMenuTable), ShowMainMenu);

    while (1)
    {
        CLEAR();
        MOVETO(0, 0);
        Menu_Task();

        printf("ѡ�����(0-����; 1-����; 2-��һ��; 3-��һ��; 4-���˵�): ");
        scanf(" %d", &cmd); // �ո������Ǻ����ϴεĻس�

        switch (cmd)
        {
        case 0:
            if (Menu_Exit(1) < 0)
            {
                return 0;
            }
            break;

        case 1:
            Menu_Enter();
            break;

        case 2:
            Menu_SelectNext(1);
            break;

        case 3:
            Menu_SelectPrevious(1);
            break;

        case 4:
            Menu_ResetMainMenu();
            break;
        default:
            break;    
        }
    }

    return 0;
}