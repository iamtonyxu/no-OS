#ifndef AXI_FR9009_CONFIG_H
#define AXI_FR9009_CONFIG_H

/****************** Include Files ********************/
#include "xil_types.h"
#include "xstatus.h"

#if FR9009_DEVICE
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

#else

#define CONFIG_8_REG_S00_AXI_SLV_REG0_OFFSET 0
#define CONFIG_8_REG_S00_AXI_SLV_REG1_OFFSET 4
#define CONFIG_8_REG_S00_AXI_SLV_REG2_OFFSET 8
#define CONFIG_8_REG_S00_AXI_SLV_REG3_OFFSET 12
#define CONFIG_8_REG_S00_AXI_SLV_REG4_OFFSET 16
#define CONFIG_8_REG_S00_AXI_SLV_REG5_OFFSET 20
#define CONFIG_8_REG_S00_AXI_SLV_REG6_OFFSET 24
#define CONFIG_8_REG_S00_AXI_SLV_REG7_OFFSET 28


/* structure definition */
typedef struct
{
	uint8_t dds_ctrl;
	uint8_t src_sel;
	uint8_t mapper_sel;
	uint8_t play_ctrl;
	uint8_t capture_en;
	uint32_t dds_pinc_0;
	uint32_t dds_poff_0;
	uint32_t dds_pinc_1;
	uint32_t dds_poff_1;
	uint32_t const_data_0;
	uint32_t const_data_1;
	uint32_t play_len_cfg;
} config8_reg_t;

/**************************** Type Definitions *****************************/
#define CONFIG_8_REG_mWriteReg(BaseAddress, RegOffset, Data) \
  	Xil_Out32((BaseAddress) + (RegOffset), (u32)(Data))

#define CONFIG_8_REG_mReadReg(BaseAddress, RegOffset) \
    Xil_In32((BaseAddress) + (RegOffset))

XStatus CONFIG_8_REG_Reg_SelfTest(void * baseaddr_p);

int trigger_capture(void);

int CONFIG_8_REG_init(config8_reg_t *pConfig8);

#endif

#endif
