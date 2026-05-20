#ifndef AXI_FR9009_CONFIG_H
#define AXI_FR9009_CONFIG_H

/****************** Include Files ********************/
#include "xil_types.h"
#include "xstatus.h"

#define OFFSET_SRC_SEL          0
#define OFFSET_MAPPER_SEL       4
#define OFFSET_DDR_PLAY_CTRL	8
#define OFFSET_DDR_PLAY_LEN     12
#define OFFSET_CONST_DATA0		16
#define OFFSET_CONST_DATA1		20
#define OFFSET_DDS_SYNC			24
#define OFFSET_TONE_1_SCALE		28
#define OFFSET_TONE_1_FREQWORD	32
#define OFFSET_TONE_2_SCALE		36
#define OFFSET_TONE_2_FREQWORD	40
#define OFFSET_RX_CAP_CONFIG	64


#define FPGA_WriteReg(BaseAddress, RegOffset, Data) \
	Xil_Out32((BaseAddress) + (RegOffset), (u32)(Data))

#define FPGA_ReadReg(BaseAddress, RegOffset) \
    Xil_In32((BaseAddress) + (RegOffset))

#define FR9009_SRC_SEL_DDS      0u
#define FR9009_SRC_SEL_DDR      1u
#define FR9009_SRC_SEL_CONST    2u

/* structure definition */
typedef struct
{
	uint8_t src_sel;
	uint8_t mapper_sel;
	uint8_t ddr_play_ctrl;
	uint32_t ddr_play_len;
	uint32_t const_data_0;
	uint32_t const_data_1;
	uint8_t dds_sync;
	uint32_t tone_1_scale;
	uint32_t tone_1_freq_word;
	uint32_t tone_2_scale;
	uint32_t tone_2_freq_word;
	uint8_t rx_cap_config;
} fr9009_config_t;

int axi_fr9009_selfTest(void);
int axi_fr9009_config_init(fr9009_config_t *pConfig);
int axi_fr9009_set_src_sel(uint8_t src_sel);
int axi_fr9009_get_src_sel(uint8_t *src_sel);

#endif
