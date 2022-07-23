/***********************************************************************************************************************

        ����˵����
        
             ���������� ��ӢSTM32F103 ������ + OLED
        OLED ���ţ�DC--PC2    RES--PC1   MOSI--PC0     CLK--PC3
             ������KEY0--PE4  KEY1--PE3  KEY_UP--PA0
        
        ���ܣ�
            1���������������ѡ����棬ѡ����Զ������ϼ��˵�
            2�����˵�����ͼ��+����ѡ��
            3���˵�ѡ���£�KEY_UP-�л�ѡ��  KEY1-ȷ��  KEY0-����


***********************************************************************************************************************/
#include "sys.h"
#include "oled.h"
#include "usart.h"
#include "timer.h"
#include "cTask.h"
#include "key.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "menu.h"
#include "mainhmi.h"


void KEY_ScanTask(void)
{
    KEY_Scan(10);
    Menu_Task();
}

int main(void)
{
    SystemTickInit();

    OLED_Init();
    OLED_Clear(OLED_BLACK);
    cTASK_Init();
    FML_TIME_Init();
    KEY_Init();

    cTASK_Creat(KEY_ScanTask, 10, 6);
    cTASK_Creat(OLED_ShowTask, 100, 10);

    MainMenuCfg_t tMainMenu;
    
    tMainMenu.pszDesc = "���˵�";
    tMainMenu.pszEnDesc = "Main Menu";
    tMainMenu.pfnLoadCallFun = Hmi_LoadMainHmi;
    tMainMenu.pfnRunCallFun = Hmi_MainTask;
    
    Menu_Init(&tMainMenu);
    
    Menu_ShortcutEnter(true, 2, 3, 0); // ����ͨ����ݲ˵�������������ѡ�����
    
    cTASK_Start();
}






