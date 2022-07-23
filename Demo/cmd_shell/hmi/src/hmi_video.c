#include "hmi_video.h"
#include "menu.h"
#include <stdio.h>
#include <string.h>


const MenuImage_t sgc_VideoImage = {
"vvvvvvvvvv",
"#"
};




void Hmi_VideoLoad(void)
{
    
}

void Hmi_VideoExit(void)
{

}

void Hmi_VideoTask(void)
{
    int cmd;

    printf("--------------------------\n");
    printf("     视频功能测试界面\n");
    printf("--------------------------\n");

    printf("选择操作(0-退出): ");
    scanf(" %d", &cmd); // 空格作用是忽略上次的回车

    if (cmd == 0)
    {
        Menu_Exit(false);
    }
}
