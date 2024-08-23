#ifndef __APP_DPD_H
#define __APP_DPD_H

#include <stdint.h>
#include "adi_hal.h"
#include "parameters.h"

#define DPD_CTRL_BASEADDR       XPAR_AXI_DPD_ACTUATOR_0_BASEADDR
#define DPD_MEM_BASEADDR        XPAR_AXI_DPD_ACTUATOR_0_BASEADDR + 0x8000
#define DPD_LUT_DEPTH           512
#define DPD_LUT_LIMIT           64

#define ADDR_IP_VERSION         0x0000
#define ADDR_ID_MASK_LOW        0x0004
#define ADDR_ID_MASK_HIGH       0x0008
#define ADDR_SCRATCH            0x000C
#define ADDR_BYPASS             0x0010

uint8_t dpd_luts_access_test(void);
uint8_t dpd_write_luts(uint8_t lutId, uint32_t *pLut);
uint8_t dpd_read_luts(uint8_t lutId, uint32_t *pLut);

uint32_t dpd_read_ipVersion(void);
uint32_t dpd_read_idMask_low(void);
uint32_t dpd_read_idMask_high(void);
uint32_t dpd_write_scratch_reg(uint32_t scratch);
uint32_t dpd_read_scratch_reg(void);
uint8_t dpd_write_bypass_reg(uint8_t enable);
uint8_t dpd_read_bypass_reg(void);

#endif
