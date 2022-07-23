
/* 菜单显示效果图可看: 

https://blog.csdn.net/qq_24130227/article/details/121167276 

*/

#include "menu.h"
#include "mainhmi.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
    int ret, cmd = 0;
    int8_t musicMenuId, languageMenuId, moreSetMenuId;
    MainMenuCfg_t tMainMenu;
    
    tMainMenu.pszDesc = "主菜单";
    tMainMenu.pszEnDesc = "Main Menu";
    tMainMenu.pfnLoadCallFun = Hmi_LoadMainHmi;
    tMainMenu.pfnRunCallFun = Hmi_MainTask;

    // Menu_Init(&tMainMenu);
    
    while (1)
    {
        CLEAR();
        MOVETO(0, 0);

        if (!Menu_IsRun())
        {
            printf("选择操作(0-进入主菜单; 1-退出): ");
            scanf(" %d", &cmd); // 空格作用是忽略上次的回车

            if (cmd == 0)
            {
                Menu_Init(&tMainMenu);

                // musicMenuId = Menu_AddShortcutMenu(&sg_MainMenuTable[0]);
                // languageMenuId = Menu_AddShortcutMenu(&sg_SetMenuTable[0]);
                // moreSetMenuId = Menu_AddShortcutMenu(&sg_SetMenuTable[4]);
                CLEAR();
                MOVETO(0, 0);
            }
            else if (cmd == 1)
            {
                return 0;
            }
        }

        Menu_Task();
    }

    return 0;
}