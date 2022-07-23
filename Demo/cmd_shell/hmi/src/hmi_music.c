#include "hmi_music.h"
#include "menu.h"
#include <stdio.h>
#include <string.h>

static bool sg_isInit = false;

const MenuImage_t sgc_MusicImage = {
"mmmmmmmmmm",
"@"
};

void Hmi_MusicLoad(void)
{
    sg_isInit = true;
}

void Hmi_MusicExit(void)
{

}

void Hmi_MusicTask(void)
{
    int cmd = 0;

    if (sg_isInit)
    {
        sg_isInit = false;
        printf("--------------------------\n");
        printf("    音乐功能测试界面\n");
        printf("--------------------------\n"); 
        sleep(1);
    }

    printf("选择操作(0-退出): ");
    scanf(" %d", &cmd); // 空格作用是忽略上次的回车

    if (cmd == 0)
    {
        Menu_Exit(false);
    }
}