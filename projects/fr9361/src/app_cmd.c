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

void cmd_api_adc_ram_dump(TRX_CHN_ENUM chn, char *name)
{
	#if HAVE_FS
	if(g_phy_obj[g_phy_select].config->chip_ver< 2)
	{
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
		char convert_result[17], fullN[4][200];
		FILE *fp_i_dec, *fp_q_dec, *fp_i_bin, *fp_q_bin;

		LOG_MAIN("\n\nreading adc ram data...\n");
		memset(fullN, 0, sizeof(fullN));
		sprintf(fullN[0], "%s_I_DEC.txt", name);
		sprintf(fullN[1], "%s_Q_DEC.txt", name);
		sprintf(fullN[2], "%s_I_BIN.txt", name);
		sprintf(fullN[3], "%s_Q_BIN.txt", name);
		remove(fullN[0]);
		remove(fullN[1]);
		remove(fullN[2]);
		remove(fullN[3]);

		if((fp_i_dec = fopen(fullN[0],"w")) == NULL)
		{
		LOG_ERROR("Failed to Open File For I_DEC\n");
		return;
		}

		if((fp_q_dec = fopen(fullN[1],"w")) == NULL)
		{
		LOG_ERROR("Failed to Open File For Q_DEC\n");
		return;
		}

		if((fp_i_bin = fopen(fullN[2],"w")) == NULL)
		{
		LOG_ERROR("Failed to Open File For I_BIN\n");
		return;
		}

		if((fp_q_bin = fopen(fullN[3],"w")) == NULL)
		{
		LOG_ERROR("Failed to Open File For Q_BIN\n");
		return;
		}

		//step1, adc init
		for (i=0; i<4; i++)
		{
			hal_spi_write_reg(&g_phy_obj[g_phy_select], chn? (adc_init[i][0] +1):(adc_init[i][0]), adc_init[i][1]);
			CHIP_UDELAY(50);
		}
		//step2, adc reset
		for (i=0; i<4; i++)
		{
			hal_spi_write_reg(&g_phy_obj[g_phy_select], chn? (adc_reset[i][0]+1):(adc_reset[i][0]), adc_reset[i][1]);
			CHIP_UDELAY(50);
		}

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
		fprintf(fp_i_dec, "%d\n", cat_val_sign);

		cat_val = cat_val & 0xffff;
		M16bithex2bin(cat_val, convert_result);
		convert_result[16] = 0;
		fprintf(fp_i_bin, "%s\n", convert_result);
		}

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
		fprintf(fp_q_dec, "%d\n", cat_val_sign);

		cat_val = cat_val & 0xffff;
		M16bithex2bin(cat_val, convert_result);
		convert_result[16] = 0;
		fprintf(fp_q_bin, "%s\n", convert_result);
		}

		LOG_MAIN("finished!\n");

		fclose(fp_i_dec);
		fclose(fp_q_dec);
		fclose(fp_i_bin);
		fclose(fp_q_bin);

	}
	else if(g_phy_obj[g_phy_select].config->chip_ver == 2)
	{
		char convert_result[17], fullN[4][200];
		FILE *fp_i_dec, *fp_q_dec, *fp_i_bin, *fp_q_bin;
		int addr=0;

		LOG_MAIN("\n\nreading adc ram data...\n");
		memset(fullN, 0, sizeof(fullN));
		sprintf(fullN[0], "%s_I_DEC.txt", name);
		sprintf(fullN[1], "%s_Q_DEC.txt", name);
		sprintf(fullN[2], "%s_I_BIN.txt", name);
		sprintf(fullN[3], "%s_Q_BIN.txt", name);
		remove(fullN[0]);
		remove(fullN[1]);
		remove(fullN[2]);
		remove(fullN[3]);

		if((fp_i_dec = fopen(fullN[0],"w")) == NULL)
		{
		LOG_ERROR("Failed to Open File For I_DEC\n");
		return;
		}

		if((fp_q_dec = fopen(fullN[1],"w")) == NULL)
		{
		LOG_ERROR("Failed to Open File For Q_DEC\n");
		return;
		}

		if((fp_i_bin = fopen(fullN[2],"w")) == NULL)
		{
		LOG_ERROR("Failed to Open File For I_BIN\n");
		return;
		}

		if((fp_q_bin = fopen(fullN[3],"w")) == NULL)
		{
		LOG_ERROR("Failed to Open File For Q_BIN\n");
		return;
		}

		LOG_MAIN("\n\nreading adc ram data...\n");

		hal_spi_write_reg(&g_phy_obj[g_phy_select], 0x14E, 0x00);

		hal_spi_write_reg(&g_phy_obj[g_phy_select], 0x0f8, 0x00);
		hal_spi_write_reg(&g_phy_obj[g_phy_select], 0x0f8, 0x10);

		hal_spi_write_reg(&g_phy_obj[g_phy_select], 0x0f8, 0x10);
		hal_spi_write_reg(&g_phy_obj[g_phy_select], 0x0f8, 0x14);
		hal_spi_write_reg(&g_phy_obj[g_phy_select], 0x0f8, 0x10);

		hal_spi_write_reg(&g_phy_obj[g_phy_select], 0x0f8, 0x10);
		hal_spi_write_reg(&g_phy_obj[g_phy_select], 0x0f8, 0x18);
		CHIP_DELAY(10);
		hal_spi_write_reg(&g_phy_obj[g_phy_select], 0x0f8, 0x10);
		hal_spi_write_reg(&g_phy_obj[g_phy_select], 0x0f8, 0x00);
		for( addr =128;addr<1023;addr++)
		{
			unsigned short ram_rdata_i=0,ram_rdata_q=0;
			hal_spi_write_reg(&g_phy_obj[g_phy_select], 0x0f8, 0x04);

			hal_spi_write_reg(&g_phy_obj[g_phy_select], 0x00b6, (addr>>8)&0x07);
			hal_spi_write_reg(&g_phy_obj[g_phy_select], 0x00b7, addr&0xff);

			hal_spi_write_reg(&g_phy_obj[g_phy_select], 0x0f8, 0x0c);

			//read I data
			ram_rdata_i = hal_spi_read_reg(&g_phy_obj[g_phy_select], 0x00b7);
			ram_rdata_i = ram_rdata_i<<8;
			ram_rdata_i |= hal_spi_read_reg(&g_phy_obj[g_phy_select], 0x00b6);

			fprintf(fp_i_dec, "%d\n", ram_rdata_i);
            ram_rdata_i = ram_rdata_i & 0xffff;
		    M16bithex2bin(ram_rdata_i, convert_result);
		    convert_result[16] = 0;
			fprintf(fp_i_bin, "%s\n", convert_result);


			hal_spi_write_reg(&g_phy_obj[g_phy_select], 0x0f8, 0x04);

			hal_spi_write_reg(&g_phy_obj[g_phy_select], 0x00b6, (addr>>8)&0x03);
			hal_spi_write_reg(&g_phy_obj[g_phy_select], 0x00b7, addr&0xff);

			hal_spi_write_reg(&g_phy_obj[g_phy_select], 0x0f8, 0x0c);

			//read Q data
			ram_rdata_q = hal_spi_read_reg(&g_phy_obj[g_phy_select], 0x00bf);
			ram_rdata_q = ram_rdata_q<<8;
			ram_rdata_q |= hal_spi_read_reg(&g_phy_obj[g_phy_select], 0x00be);

			fprintf(fp_q_dec, "%d\n", ram_rdata_q);
            ram_rdata_q = ram_rdata_q & 0xffff;
		    M16bithex2bin(ram_rdata_q, convert_result);
		    convert_result[16] = 0;
			fprintf(fp_q_bin, "%s\n", convert_result);
			hal_spi_write_reg(&g_phy_obj[g_phy_select], 0x0f8, 0x04);

		}
		fclose(fp_i_dec);
		fclose(fp_q_dec);
		fclose(fp_i_bin);
		fclose(fp_q_bin);

		LOG_MAIN("finished!\n");
	}
	#else
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
	#endif

}

