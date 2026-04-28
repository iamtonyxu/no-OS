#ifndef HEADLESS_H_
#define HEADLESS_H_

#include <stdint.h>
#include "fr9009.h"
#include "fr9009_config.h"

#ifdef __cplusplus
extern "C" {
#endif


#define DEVICE_NUMS	1

extern struct fr_hal frhal[DEVICE_NUMS];
extern fr9009Device_t brDev[DEVICE_NUMS];
MYDLL_API int32_t rf_initialize(void);
MYDLL_API int32_t fr9009_rf_setup(fr9009Device_t  *device, fr9009Init_t *pInitPara, uint16_t step);
MYDLL_API int32_t fr9009_rf_init(uint16_t chip_index, uint16_t *pInitStep, fr9009Init_t *pInitPara_new, fr9009Device_t  *tdevice);
MYDLL_API int32_t fr9009_JesdRelink(uint16_t chip_index);
#ifdef __cplusplus
}
#endif

#endif
