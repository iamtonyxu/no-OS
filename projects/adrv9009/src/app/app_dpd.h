#ifndef __APP_DPD_H
#define __APP_DPD_H

#include <stdint.h>
#include "adi_hal.h"
#include "parameters.h"

// DPD Base Address
//#define XPAR_AXI_DPD_ACTUATOR_0_BASEADDR 0xffff0000
#define DPD_CTRL_BASEADDR       XPAR_AXI_DPD_ACTUATOR_0_BASEADDR
#define DPD_MEM_BASEADDR        XPAR_AXI_DPD_ACTUATOR_0_BASEADDR + 0x8000
#define DPD_LUT_DEPTH           1024
#define DPD_LUT_MAX             64

// DPD Ctrl Registers
#define ADDR_IP_VERSION         0x0000
#define ADDR_ID_MASK_LOW        0x0004
#define ADDR_ID_MASK_HIGH       0x0008
#define ADDR_SCRATCH            0x000C
#define ADDR_ACT_OUT_SEL        0x0010
#define ADDR_LUTID_L            0x0014
#define ADDR_LUTID_H            0x0018

// DPD ACT Out Select
typedef enum{
    DPD_ENABLE      = 0x01u,
    DPD_BYPASS      = 0x02u,
    DPD_SHUTDOWN    = 0x04u,
    DPD_FREEZE      = 0x08u,
} Dpd_ActOut_Sel;

uint8_t dpd_luts_access_test(void);
uint8_t dpd_write_luts(uint8_t lutId, uint32_t *pLut);
uint8_t dpd_read_luts(uint8_t lutId, uint32_t *pLut);

uint32_t dpd_read_ipVersion(void);
uint64_t dpd_read_idMask(void);

uint32_t dpd_write_scratch_reg(uint32_t scratch);
uint32_t dpd_read_scratch_reg(void);
uint8_t dpd_write_act_out_sel(Dpd_ActOut_Sel sel);
uint8_t dpd_read_act_out_sel(void);
uint64_t dpd_write_lutid(uint64_t lutid);
uint64_t dpd_read_lutid(void);

#endif
