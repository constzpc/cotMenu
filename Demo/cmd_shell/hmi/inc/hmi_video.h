#ifndef HMI_VIDEO_H
#define HMI_VIDEO_H

#include "hmi_common.h"

extern const MenuImage_t sgc_VideoImage;

void Hmi_VideoLoad(void);
void Hmi_VideoExit(void);
void Hmi_VideoTask(void);

#endif