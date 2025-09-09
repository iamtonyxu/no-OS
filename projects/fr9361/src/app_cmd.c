#include "app_cmd.h"

extern rf_chip_phy_t g_phy_obj[RF_PHY_NUMBER];

/******************************************************************************/
/************************ api-command Functions *******************************/
/******************************************************************************/
void cmd_api_tx_tone(TRX_CHN_ENUM chn, short on, long freq)
{
	LOG_MAIN("tx tone, chn:%d, on:%d, freq=%d\n", chn, on, freq);
	//fn_tx_send_tone(chn, on, freq);
	send_cordic_signal(&g_phy_obj[g_phy_select], chn, g_phy_obj[g_phy_select].config->bandwidth, on, freq);
	return;
}

void cmd_api_rx_mgc_split_table_gain(TRX_CHN_ENUM chn, RX_MGC_GAIN_ENUM tb, unsigned char val)
{
	short index;
	LOG_MAIN("rx mgc gain, chn:%d, table:%d, val:%d\n", chn, tb, val);
	if (rx_mgc_split_table_gain(&g_phy_obj[g_phy_select], chn, tb, val) < 0)
		return;

	index = (chn==TRX_CHN1)?0:1;
	switch (tb) {
		case LMT_G: g_phy_obj[g_phy_select].rx_lmt_gain[index]=val; break;
		case LPF_G: g_phy_obj[g_phy_select].rx_lpf_gain[index]=val; break;
		case DIG_G: g_phy_obj[g_phy_select].rx_dig_gain[index]=val; break;
	}
}

void cmd_api_adc_ram_dump_v2(TRX_CHN_ENUM chn, uint16_t *I_data, uint16_t *Q_data, uint16_t len)
{
	const uint16_t MAX_LEN = 1024;
	unsigned int  reg_address, reg_w_value, reg_r_value1, reg_r_value2, cat_val;

	unsigned short adc_init[4][2] =
    {
        {0x0f8, 0x10},
        {0x0f8, 0x50},
        {0x0f8, 0x54},
        {0x0f8, 0x50},
    };
    unsigned short adc_reset[4][2] =
    {
        {0x0f8, 0x58},
        {0x0f8, 0x50},
        {0x0f8, 0x54},
        {0x0f8, 0x50},
    };
    int i, j, cat_val_sign, tmp_int;

	/* sanity check on sample length */
	if(len > MAX_LEN)
	{
		len = MAX_LEN;
	}

	    //step1, adc init
    for (i=0; i<4; i++)
    hal_spi_write_reg(&g_phy_obj[g_phy_select], chn? (adc_init[i][0] +1):(adc_init[i][0]), adc_init[i][1]);

    //step2, adc reset
    for (i=0; i<4; i++)
    hal_spi_write_reg(&g_phy_obj[g_phy_select], chn? (adc_reset[i][0]+1):(adc_reset[i][0]), adc_reset[i][1]);

    //step3, read 0 to 1023 for I data
    for (i=0; i<1024; i++)
    {
		//step3.1, adc set
		hal_spi_write_reg(&g_phy_obj[g_phy_select], chn?0x0f9:0x0f8, 0x54);

		//step3.2, addr set
		hal_spi_write_reg(&g_phy_obj[g_phy_select], 0x0b6, (i >> 8) & 0xff);
		hal_spi_write_reg(&g_phy_obj[g_phy_select], 0x0b7, i & 0xff);

		//step3.3, adc set
		hal_spi_write_reg(&g_phy_obj[g_phy_select], chn?0x0f9:0x0f8, 0x5C);

		//step3.4, read
		reg_r_value1 = hal_spi_read_reg(&g_phy_obj[g_phy_select], 0x0be);//h
		reg_r_value2 = hal_spi_read_reg(&g_phy_obj[g_phy_select], 0x0bf);//l

		//step3.5, adc set
		hal_spi_write_reg(&g_phy_obj[g_phy_select], chn?0x0f9:0x0f8, 0x54);

		//step3.6, data
		cat_val = ((reg_r_value1 & 0xff) << 8) | (reg_r_value2 & 0xff);
		#if 0
		cat_val_sign = cat_val;
		if (cat_val >= 32768)
			cat_val_sign = cat_val_sign - 65536;
		LOG_MAIN("%d\n",cat_val_sign);
		#endif 
		I_data[i]=cat_val;
    }
    //LOG_MAIN("______________________Q_PATH______________________\n");
    //step4, read 1024 to 2047 for Q data
    for (i=1024; i<2048; i++)
    {
		//step3.1, adc set
		hal_spi_write_reg(&g_phy_obj[g_phy_select], chn?0x0f9:0x0f8, 0x54);

		//step3.2, addr set
		hal_spi_write_reg(&g_phy_obj[g_phy_select], 0x0b6, (i >> 8) & 0xff);
		hal_spi_write_reg(&g_phy_obj[g_phy_select], 0x0b7, i & 0xff);

		//step3.3, adc set
		hal_spi_write_reg(&g_phy_obj[g_phy_select], chn?0x0f9:0x0f8, 0x5C);

		//step3.4, read
		reg_r_value1 = hal_spi_read_reg(&g_phy_obj[g_phy_select], 0x0be);//h
		reg_r_value2 = hal_spi_read_reg(&g_phy_obj[g_phy_select], 0x0bf);//l

		//step3.5, adc set
		hal_spi_write_reg(&g_phy_obj[g_phy_select], chn?0x0f9:0x0f8, 0x54);

		//step3.6, data
		cat_val = ((reg_r_value1 & 0xff) << 8) | (reg_r_value2 & 0xff);
		#if 0
		cat_val_sign = cat_val;
		if (cat_val >= 32768)
			cat_val_sign = cat_val_sign - 65536;
		LOG_MAIN("%d\n",cat_val_sign);
		#endif
		Q_data[i-1024]=cat_val;
    }

	return;
}

void cmd_api_adc_ram_dump(TRX_CHN_ENUM chn, char *name)
{
    unsigned int  reg_address, reg_w_value, reg_r_value1, reg_r_value2, cat_val;
	//int I_data[1024]={0},Q_data[1024]={0};
    unsigned short adc_init[4][2] =
    {
        {0x0f8, 0x10},
        {0x0f8, 0x50},
        {0x0f8, 0x54},
        {0x0f8, 0x50},
    };
    unsigned short adc_reset[4][2] =
    {
        {0x0f8, 0x58},
        {0x0f8, 0x50},
        {0x0f8, 0x54},
        {0x0f8, 0x50},
    };
    int i, j, cat_val_sign, tmp_int;
    char convert_result[17], fullN[4][200];

    LOG_MAIN("\n\nreading adc ram data...\n");

    LOG_MAIN("______________________I_PATH______________________\n");
    //step1, adc init
    for (i=0; i<4; i++)
    hal_spi_write_reg(&g_phy_obj[g_phy_select], chn? (adc_init[i][0] +1):(adc_init[i][0]), adc_init[i][1]);

    //step2, adc reset
    for (i=0; i<4; i++)
    hal_spi_write_reg(&g_phy_obj[g_phy_select], chn? (adc_reset[i][0]+1):(adc_reset[i][0]), adc_reset[i][1]);

    //step3, read 0 to 1023 for I data
    for (i=0; i<1024; i++)
    {
    //step3.1, adc set
    hal_spi_write_reg(&g_phy_obj[g_phy_select], chn?0x0f9:0x0f8, 0x54);

    //step3.2, addr set
    hal_spi_write_reg(&g_phy_obj[g_phy_select], 0x0b6, (i >> 8) & 0xff);
    hal_spi_write_reg(&g_phy_obj[g_phy_select], 0x0b7, i & 0xff);

    //step3.3, adc set
    hal_spi_write_reg(&g_phy_obj[g_phy_select], chn?0x0f9:0x0f8, 0x5C);

    //step3.4, read
    reg_r_value1 = hal_spi_read_reg(&g_phy_obj[g_phy_select], 0x0be);//h
    reg_r_value2 = hal_spi_read_reg(&g_phy_obj[g_phy_select], 0x0bf);//l

    //step3.5, adc set
    hal_spi_write_reg(&g_phy_obj[g_phy_select], chn?0x0f9:0x0f8, 0x54);

    //step3.6, data
    cat_val = ((reg_r_value1 & 0xff) << 8) | (reg_r_value2 & 0xff);
    cat_val_sign = cat_val;
    if (cat_val >= 32768)
        cat_val_sign = cat_val_sign - 65536;
    LOG_MAIN("%d\n",cat_val_sign);
	//I_data[i]=cat_val_sign;
    }
    LOG_MAIN("______________________Q_PATH______________________\n");
    //step4, read 1024 to 2047 for Q data
    for (i=1024; i<2048; i++)
    {
    //step3.1, adc set
    hal_spi_write_reg(&g_phy_obj[g_phy_select], chn?0x0f9:0x0f8, 0x54);

    //step3.2, addr set
    hal_spi_write_reg(&g_phy_obj[g_phy_select], 0x0b6, (i >> 8) & 0xff);
    hal_spi_write_reg(&g_phy_obj[g_phy_select], 0x0b7, i & 0xff);

    //step3.3, adc set
    hal_spi_write_reg(&g_phy_obj[g_phy_select], chn?0x0f9:0x0f8, 0x5C);

    //step3.4, read
    reg_r_value1 = hal_spi_read_reg(&g_phy_obj[g_phy_select], 0x0be);//h
    reg_r_value2 = hal_spi_read_reg(&g_phy_obj[g_phy_select], 0x0bf);//l

    //step3.5, adc set
    hal_spi_write_reg(&g_phy_obj[g_phy_select], chn?0x0f9:0x0f8, 0x54);

    //step3.6, data
    cat_val = ((reg_r_value1 & 0xff) << 8) | (reg_r_value2 & 0xff);
    cat_val_sign = cat_val;
    if (cat_val >= 32768)
        cat_val_sign = cat_val_sign - 65536;
    LOG_MAIN("%d\n",cat_val_sign);
	//Q_data[i-1024]=cat_val_sign;
    }
	//I_Q_DFT_Print(I_data,Q_data,adc_fs);

    LOG_MAIN("finished!\n");
}

