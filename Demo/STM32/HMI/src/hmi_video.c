#include "hmi_video.h"
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

void Hmi_VideoLoad(void)
{
    KEY_Register(IO_KEY_0, OnKeyFunCB);
    KEY_Register(IO_KEY_1, OnKeyFunCB);
    KEY_Register(IO_KEY_UP, OnKeyFunCB);
    sg_isInit = true;
}

void Hmi_VideoExit(void)
{

}

void Hmi_VideoTask(void)
{
    if (sg_isInit)
    {
        sg_isInit = false;
        OLED_Clear(OLED_BLACK);
        OLED_SetText(0, 0, "----��Ƶ����----", 0, FONT_16X16);
    }
    
    if (sg_isPlaying)
    {
        OLED_SetText(24, 24, "���ڲ���", 0, FONT_16X16);
    }
    else
    {
        OLED_SetText(24, 24, "������ͣ", 0, FONT_16X16);
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
