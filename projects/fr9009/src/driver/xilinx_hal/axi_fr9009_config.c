#include "parameters.h"
#include "axi_fr9009_config.h"

#if FR9009_DEVICE
int axi_fr9009_selfTest(void)
{
	uint8_t status = 0;
	void *baseaddr = (void *)FR9009_CONFIG_REG_BASEADDR;
	uint32_t const_data_0 = 0x12345678;
	uint32_t const_data_1 = 0x9abcdef0;

	FPGA_WriteReg(baseaddr, OFFSET_CONST_DATA0, const_data_0);
	FPGA_WriteReg(baseaddr, OFFSET_CONST_DATA1, const_data_1);

	uint32_t rd_const_data_0 = FPGA_ReadReg(baseaddr, OFFSET_CONST_DATA0);
	uint32_t rd_const_data_1 = FPGA_ReadReg(baseaddr, OFFSET_CONST_DATA1);

	if ((rd_const_data_0 != const_data_0) ||
	    (rd_const_data_0 != const_data_1))
	{
		status = 1; // err
	}

	return status;
}

int axi_fr9009_config_init(fr9009_config_t *pConfig)
{
	uint8_t status = 0;
	void *baseaddr = (void *)FR9009_CONFIG_REG_BASEADDR;
	uint32_t src_sel = (pConfig->src_sel & 0x03);
	uint32_t mapper_sel = (pConfig->mapper_sel & 0x01);
	uint32_t ddr_play_ctrl = (pConfig->ddr_play_ctrl & 0x01);
#if 0
	uint32_t ddr_play_len = pConfig->ddr_play_len;
#else
	uint32_t ddr_play_len = (0x80000000 | pConfig->ddr_play_len);
#endif
	uint32_t const_data_0 = pConfig->const_data_0;
	uint32_t const_data_1 = pConfig->const_data_1;
	uint32_t dds_sync = (pConfig->dds_sync & 0x01);
	uint32_t tone_1_scale = (pConfig->tone_1_scale & 0xffffu);
	uint32_t tone_1_freq_word = (pConfig->tone_1_freq_word & 0xffffu);
	uint32_t tone_2_scale = (pConfig->tone_2_scale & 0xffffu);
	uint32_t tone_2_freq_word = (pConfig->tone_2_freq_word & 0xffffu);
	uint32_t rx_cap_config = pConfig->rx_cap_config;

	FPGA_WriteReg(baseaddr, OFFSET_SRC_SEL, src_sel);
	FPGA_WriteReg(baseaddr, OFFSET_MAPPER_SEL, mapper_sel);
	if (ddr_play_ctrl == 1u) {
		FPGA_WriteReg(baseaddr, OFFSET_DDR_PLAY_CTRL, 0u);
		(void)FPGA_ReadReg(baseaddr, OFFSET_DDR_PLAY_CTRL);
		FPGA_WriteReg(baseaddr, OFFSET_DDR_PLAY_CTRL, 1u);
	} else {
		FPGA_WriteReg(baseaddr, OFFSET_DDR_PLAY_CTRL, ddr_play_ctrl);
	}
	FPGA_WriteReg(baseaddr, OFFSET_DDR_PLAY_LEN, ddr_play_len);
	FPGA_WriteReg(baseaddr, OFFSET_CONST_DATA0, const_data_0);
	FPGA_WriteReg(baseaddr, OFFSET_CONST_DATA1, const_data_1);
	FPGA_WriteReg(baseaddr, OFFSET_DDS_SYNC, dds_sync);
	FPGA_WriteReg(baseaddr, OFFSET_TONE_1_SCALE, tone_1_scale);
	FPGA_WriteReg(baseaddr, OFFSET_TONE_1_FREQWORD, tone_1_freq_word);
	FPGA_WriteReg(baseaddr, OFFSET_TONE_2_SCALE, tone_2_scale);
	FPGA_WriteReg(baseaddr, OFFSET_TONE_2_FREQWORD, tone_2_freq_word);
	FPGA_WriteReg(baseaddr, OFFSET_RX_CAP_CONFIG, rx_cap_config);

	// cfg0=0x67 when dds_ctrl=3,src_sel=1,mapper_sel=0,play_ctrl=1,capture_en=1
	uint32_t rd_src_sel = FPGA_ReadReg(baseaddr, OFFSET_SRC_SEL);
	uint32_t rd_const_data_0 = FPGA_ReadReg(baseaddr, OFFSET_CONST_DATA0);
	uint32_t rd_const_data_1 = FPGA_ReadReg(baseaddr, OFFSET_CONST_DATA1);

	if ((rd_src_sel != src_sel) ||
	    (rd_const_data_0 != const_data_0) ||
	    (rd_const_data_0 != const_data_1))
	{
		status = 1; // err
	}

	return status;
}

int trigger_capture(void)
{
	uint8_t status = 0;
	void *baseaddr = (void *)FR9009_CONFIG_REG_BASEADDR; // XPAR_AXI_FR9009_CONFIG_0_BASEADDR
	uint32_t cfg;
	uint32_t cleared_cfg;
	uint32_t set_cfg;

	/*
	 * rx_cap_config[0] is edge-triggered (0->1), not self-clearing:
	 * each trigger must generate a low-to-high transition explicitly.
	 */
	cfg = FPGA_ReadReg(baseaddr, OFFSET_RX_CAP_CONFIG);

	/* Force low first. */
	cleared_cfg = (cfg & 0xFEu);
	FPGA_WriteReg(baseaddr, OFFSET_RX_CAP_CONFIG, cleared_cfg);
	(void)FPGA_ReadReg(baseaddr, OFFSET_RX_CAP_CONFIG);

	/* Then drive high to create 0->1 edge. */
	set_cfg = (cleared_cfg | 0x01u);
	FPGA_WriteReg(baseaddr, OFFSET_RX_CAP_CONFIG, set_cfg);
	cfg = FPGA_ReadReg(baseaddr, OFFSET_RX_CAP_CONFIG);

	if ((cfg & 0x01u) == 0u)
		status = 1u;

	return status;
}

#else

#define READ_WRITE_MUL_FACTOR 0x10

XStatus CONFIG_8_REG_Reg_SelfTest(void * baseaddr_p)
{
	u32 baseaddr = (u32) baseaddr_p;
	int write_loop_index;
	int read_loop_index;
	int Index;

	/* Write to user logic slave module register(s) and read back */
	for (write_loop_index = 0 ; write_loop_index < 4; write_loop_index++)
	  CONFIG_8_REG_mWriteReg (baseaddr, write_loop_index*4, (write_loop_index+1)*READ_WRITE_MUL_FACTOR);
	for (read_loop_index = 0 ; read_loop_index < 4; read_loop_index++)
	  if ( CONFIG_8_REG_mReadReg (baseaddr, read_loop_index*4) != (read_loop_index+1)*READ_WRITE_MUL_FACTOR)
	  {
		  return XST_FAILURE;
	  }

	return XST_SUCCESS;
}

int trigger_capture(void)
{
	uint8_t status = 0;
	void * baseaddr = (void *)CONFIG_8_REG_BASEADDR; //0x43C30000
	uint32_t oldcfg0, newcfg0 = 0u;

	// read cfg0, clear capture_en of cfg0(bit6) then reset it to trigger capture
	oldcfg0 = CONFIG_8_REG_mReadReg(baseaddr, 0);
	newcfg0 = oldcfg0 & 0xbfu;
	CONFIG_8_REG_mWriteReg(baseaddr, 0, newcfg0);
	newcfg0 = oldcfg0 | 0x40u;
	CONFIG_8_REG_mWriteReg(baseaddr, 0, newcfg0);

	newcfg0 = CONFIG_8_REG_mReadReg(baseaddr, 0);
	status = (newcfg0 & 0x40u) ? 0u : 1u;

	return status;
}

int CONFIG_8_REG_init(config8_reg_t *pConfig8)
{
	uint8_t status = 0;
	void * baseaddr = (void *)CONFIG_8_REG_BASEADDR;
	uint32_t cfg0 = (pConfig8->dds_ctrl & 0x03) |
					((pConfig8->src_sel << 2) & 0x0C) |
					((pConfig8->mapper_sel << 4) & 0x10) |
					((pConfig8->play_ctrl << 5) & 0x20) |
					((pConfig8->capture_en << 6) & 0x40);
	uint32_t cfg1 = pConfig8->dds_pinc_0;
	uint32_t cfg2 = pConfig8->dds_poff_0;
	uint32_t cfg3 = pConfig8->dds_pinc_1;
	uint32_t cfg4 = pConfig8->dds_poff_1;
	uint32_t cfg5 = pConfig8->const_data_0;
	uint32_t cfg6 = pConfig8->const_data_1;
#if 1
	pConfig8->play_len_cfg = 0x80000000 | (TX_BUF_LEN);
#endif
	uint32_t cfg7 = pConfig8->play_len_cfg;

	CONFIG_8_REG_mWriteReg(baseaddr, 0, cfg0);
	CONFIG_8_REG_mWriteReg(baseaddr, 4, cfg1);
	CONFIG_8_REG_mWriteReg(baseaddr, 8, cfg2);
	CONFIG_8_REG_mWriteReg(baseaddr, 12, cfg3);
	CONFIG_8_REG_mWriteReg(baseaddr, 16, cfg4);
	CONFIG_8_REG_mWriteReg(baseaddr, 20, cfg5);
	CONFIG_8_REG_mWriteReg(baseaddr, 24, cfg6);
	CONFIG_8_REG_mWriteReg(baseaddr, 28, cfg7);

	//cfg0=0x67 when dds_ctrl=3,src_sel=1,mapper_sel=0,play_ctrl=1,capture_en=1
	cfg0 = CONFIG_8_REG_mReadReg(baseaddr, 0);
	cfg5 = CONFIG_8_REG_mReadReg(baseaddr, 20);
	cfg6 = CONFIG_8_REG_mReadReg(baseaddr, 24);

	if(((cfg0 & 0x03) != pConfig8->dds_ctrl) ||
			(cfg5 != pConfig8->const_data_0) ||
			(cfg6 != pConfig8->const_data_1))
	{
		status = 1;
	}

	return status;
}

#endif

