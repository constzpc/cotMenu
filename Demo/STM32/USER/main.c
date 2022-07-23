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


void TestTask(void);

void KEY_ScanTask(void)
{
    KEY_Scan(10);
}

void TestTask(void)
{
    Menu_Task();
}

void OnKeyFunCB(IoKeyType_e key, KeyAction_e action)
{
    static uint16_t s_cnt = 0;

    if (action == KEY_ACTION_LOSSEN)
    {
        OLED_Printf("s_cnt:%d\r\n", s_cnt++);
    }
}

int main(void)
{
    SystemTickInit();
    FML_USART_Init();

    OLED_Init();
    OLED_Clear(OLED_BLACK);
    //OLED_SetText(0, 10, "test", 0, FONT_24X24);
    OLED_ShowTask();
    cTASK_Init();
    FML_TIME_Init();
    KEY_Init();
    KEY_Register(IO_KEY_0, OnKeyFunCB);

    cTASK_Creat(TestTask, 10, 7);
    cTASK_Creat(KEY_ScanTask, 10, 6);
    cTASK_Creat(OLED_ShowTask, 100, 10);
//    //cTASK_Creat(Menu_Task, 10, 10);

    MainMenuCfg_t tMainMenu;
    
    tMainMenu.pszDesc = "主菜单";
    tMainMenu.pszEnDesc = "Main Menu";
    tMainMenu.pfnLoadCallFun = Hmi_LoadMainHmi;
    tMainMenu.pfnRunCallFun = Hmi_MainTask;
    
    Menu_Init(&tMainMenu);
    
    Menu_ShortcutEnter(2, 3, 0); // 开机通过快捷菜单索引进入语言选择界面
    
    cTASK_Start();
}






