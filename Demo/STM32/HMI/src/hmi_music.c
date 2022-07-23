#include "hmi_music.h"
#include "menu.h"
#include "key.h"
#include <string.h>

static bool sg_isInit = false;

static char sg_volume = 0;
static bool sg_isPlaying = false;

static void OnKeyFunCB(IoKeyType_e key, KeyAction_e action)
{
    if (action == KEY_ACTION_LOSSEN)
    {
        if (key == IO_KEY_0)
        {
            Menu_Exit(false);
        }
        else if (key == IO_KEY_1)
        {
            sg_isPlaying = !sg_isPlaying;
        }
        else if (key == IO_KEY_UP)
        {
            sg_volume++;
            
            if (sg_volume > 16)
            {
                sg_volume = 1;
            }
        }
    }
}

void Hmi_MusicLoad(void)
{
    sg_isInit = true;
    KEY_Register(IO_KEY_0, OnKeyFunCB);
    KEY_Register(IO_KEY_1, OnKeyFunCB);
    KEY_Register(IO_KEY_UP, OnKeyFunCB);
}

void Hmi_MusicExit(void)
{

}

void Hmi_MusicTask(void)
{
    if (sg_isInit)
    {
        sg_isInit = false;
        OLED_Clear(OLED_BLACK);
        OLED_SetText(0, 0, "----音乐界面----", 0, FONT_16X16);
    }
    
    if (sg_isPlaying)
    {
        OLED_SetText(24, 24, "正在播放", 0, FONT_16X16);
    }
    else
    {
        OLED_SetText(24, 24, "播放暂停", 0, FONT_16X16);
    }
    
    for (int i = 0; i < 16; i++)
    {
        if (i < sg_volume)
        {
            OLED_SetText(0 + i * 8, 48, "-", 0, FONT_16X16);
        }
        else
        {
            OLED_SetText(0 + i * 8, 48, " ", 0, FONT_16X16);
        }
    }
}
