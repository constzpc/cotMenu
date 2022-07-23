#ifndef HMI_CAMERA_H
#define HMI_CAMERA_H

#include "hmi_common.h"

extern const MenuImage_t sgc_CameraImage;

void Hmi_CameraLoad(void);
void Hmi_CameraExit(void);
void Hmi_CameraTask(void);

#endif