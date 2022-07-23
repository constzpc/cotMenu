#ifndef HMI_MUSIC_H
#define HMI_MUSIC_H

#include "hmi_common.h"


extern const MenuImage_t sgc_MusicImage;

void Hmi_MusicLoad(void);
void Hmi_MusicExit(void);
void Hmi_MusicTask(void);

#endif