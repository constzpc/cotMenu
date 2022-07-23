/***********************************************************************************************************************

        程序说明：
        
             环境：采用 精英STM32F103 开发板 + OLED
        OLED 引脚：DC--PC2    RES--PC1   MOSI--PC0     CLK--PC3
             按键：KEY0--PE4  KEY1--PE3  KEY_UP--PA0
        
        功能：
            1、开机则进入语言选择界面，选择后自动返回上级菜单
            2、主菜单采用图像+描述选择
            3、菜单选择下：KEY_UP-切换选项  KEY1-确定  KEY0-返回


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
    
    tMainMenu.pszDesc = "主菜单";
    tMainMenu.pszEnDesc = "Main Menu";
    tMainMenu.pfnLoadCallFun = Hmi_LoadMainHmi;
    tMainMenu.pfnRunCallFun = Hmi_MainTask;
    
    Menu_Init(&tMainMenu);
    
    Menu_ShortcutEnter(true, 2, 3, 0); // 开机通过快捷菜单索引进入语言选择界面
    
    cTASK_Start();
}






