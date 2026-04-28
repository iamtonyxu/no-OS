#ifndef _AD9528_APP_H_
#define _AD9528_APP_H_
#include "ad9528.h"

#define AD9528_NUM 1
int ad9528_cfg(void);
int ad9528_device_init(void);
void ad9528_sys_onoff(uint8_t on);

#endif
