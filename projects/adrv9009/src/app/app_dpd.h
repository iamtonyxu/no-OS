#ifndef __APP_DPD_H
#define __APP_DPD_H

#include <stdint.h>
#include "adi_hal.h"
#include "parameters.h"

// DPD Base Address
#define DPD_CTRL_BASEADDR       XPAR_AXI_DPD_ACTUATOR_0_BASEADDR
#define DPD_MEM_BASEADDR        XPAR_AXI_DPD_ACTUATOR_0_BASEADDR + 0x8000
#define DPD_CAP0_BASEADDR       XPAR_AXI_DPD_CAPTURE_0_BASEADDR
#define DPD_CAP1_BASEADDR       XPAR_AXI_DPD_CAPTURE_1_BASEADDR
#define DPD_LUT_DEPTH           1024
#define DPD_LUT_MAX             64
#define DPD_CAP_SIZE            4096

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
uint32_t dpd_register_write(uint8_t offset, uint32_t value);
uint32_t dpd_register_read(uint8_t offset);
uint8_t dpd_luts_write(uint8_t lutId, uint32_t *pLut);
uint8_t dpd_luts_read(uint8_t lutId, uint32_t *pLut);

uint32_t dpd_read_ipVersion(void);
uint64_t dpd_read_idMask(void);

uint32_t dpd_write_scratch_reg(uint32_t scratch);
uint32_t dpd_read_scratch_reg(void);
uint8_t dpd_write_act_out_sel(Dpd_ActOut_Sel sel);
uint8_t dpd_read_act_out_sel(void);
uint64_t dpd_write_lutid(uint64_t lutid);
uint64_t dpd_read_lutid(void);

uint8_t dpd_read_capture(uint8_t position, uint32_t *pBuf, uint32_t size);

#endif
