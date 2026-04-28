#include "ad9528_app.h"
#include "stdint.h"
#include "ad9528.h"

extern ad9528Device_t ad9528_spi[2];
#define ad9528_buffer_size sizeof(ad9528_chip1_reg_cfg)/sizeof(ad9528_chip1_reg_cfg[0])
uint16_t ad9528_chip1_reg_cfg[][2]={
		//# 主板子9528配置
#if 1	//AD9528_NUM > 1
		//# old 122.88M
		        {0x0000, 0x3C},
		        {0x0001, 0x80},
		        {0x0100, 0x01},
		        {0x0102, 0x01},
		        {0x0104, 0x04},
		        {0x0106, 0x0A},
		        {0x0107, 0x03},
		        {0x0108, 0x2A},
		        {0x010A, 0x02},
		        {0x0200, 0xE6},
		        {0x0201, 0x87}, //#ref>100M
		        //# {0x0201, 0x0A},  //# ref=100M
		        {0x0203, 0x01},
		        {0x0204, 0x03},//#ref>100M
		        //# {0x0204, 0x04},  //# ref>100M
		        {0x0205, 0x2A},
		        {0x0208, 0x09},
		        {0x0302, 0x09},
		        {0x0303, 0x00},
		        {0x0305, 0x04},//#默认配置
		        //# {0x0305, 0x00},  //# out1除1
		        {0x0308, 0x09},
		        {0x309, 0xE0},  //# {3'b111,,,1'b0,,4'b0},//#OUT3,as,,FPGA,SYS_REF
		        {0x030B, 0x09},
		        {0x030E, 0x09},
		        {0x030F, 0x00},
		        {0x0311, 0x09},
		        {0x0314, 0x09},
		        {0x0315, 0x00},
		        {0x0317, 0x09},
		        {0x0318, 0x40},
		        {0x0319, 0x80},
		        {0x031A, 0x09},
		        {0x031B, 0x00},
		        {0x031D, 0x09},
		        {0x0320, 0x09},
		        {0x0321, 0x00},
		        {0x032D, 0x00},//#默认
		        //# {0x032D, 0x01},  //# 外灌REF不经过9528PLL2
		        {0x0323, 0x09},
		        {0x324, 0xE0},  //# {3'b111,,,1'b0,,4'b0},//#out12,setting
		        {0x325, 0x80},
		        {0x0326, 0x09},
		        {0x0327, 0x00},
		        {0x0329, 0x09},//#默认配置
		        //# {0x0329, 0x00},  //# out13除1
		        {0x400, 0x00},  //#//#K,divider,[7:0] //#1024
		        {0x401, 0x02},  //#//#K,divider,[15:8]//#1024
		        //# {0x400, 0x40},  //#//#K,divider,[7:0]//#128
		        //# {0x401, 0x00},  //#//#K,divider,[15:8]//#128
		        {0x402, 0x1C},  //#//#{1'b0,,,2'b00,,,1'b1,,,1'b1,,2'b10,,1'b0},,,,,,,
		        {0x403, 0x9A},  //#//#{2'b10,,2'b01,,,,3'b101,,1'b0}
		        {0x403, 0x9B},  //#//#{2'b10,,2'b01,,,,3'b101,,1'b1},,//#start,the,sysref,generation,,,,
		        {0x404, 0x04},  //# ,,,
		        {0x0501, 0xF5},
		        {0x0502, 0x02},
		        {0x0505, 0x07},
		        {0x0506, 0x01},
		        {0x0507, 0x0C},
		        {0x0F, 0x01},

#else // 50M in 125 out 78out
				0x0000,0x3C,
				0x0001,0x80,
				0x0100,0x01,
				0x0102,0x01,
				0x0104,0x04,
				0x0106,0x8A,
				0x0107,0x03,
				0x0108,0x23,
				0x010A,0x0A,
				0x0200,0xFE,
				0x0201,0x14,
				0x0203,0x00,
				0x0204,0x04,
				0x0205,0xC1,
				0x0208,0x13,
				0x0303,0x00,
				0x0305,0x03,
				0x0308,0x09,
				0x030B,0x09,
				0x030E,0x09,
				0x030F,0x00,
				0x0311,0x09,
				0x0314,0x09,
				0x0315,0x00,
				//0x0316,0x80, //ldvs 0701
				0x0317,0x07, //0x031709
				0x0318,0x40,
				0x0319,0x80,
				0x031A,0x09,
				0x031B,0x00,
				0x031D,0x09,
				0x0320,0x09,
				0x0321,0x00,
				0x0323,0x09,
				0x0324,0x40,
				0x0325,0x00,
				0x0326,0x09,
				0x0327,0x00,
				0x0329,0x07,
				0x032E,0x00,
				0x0401,0x01,
				0x0402,0x04,//0x04021C,
				0x0403,0x9A,
				0x0501,0x75,//0x0501F5
				0x0502,0x0E,
				0x0203,0x01,
				0x000F,0x01,
				0x0403,0x9B,
				0x000F,0x01,
#endif
};

uint16_t ad9528_chip2_reg_cfg[][2]={

        //# {0x0000,0x1},
        {0x0000, 0x3C},
        {0x0001, 0x80},
        {0x0100, 0x01},
        {0x0102, 0x01},
        {0x0104, 0x04},
        {0x0106, 0x0A},
        {0x0107, 0x03},
        {0x0108, 0x2A},
        {0x010A, 0x02},
        {0x0200, 0xE6},
        {0x0201, 0x87}, //#ref>100M
        {0x0203, 0x01},
        {0x0204, 0x03},//#ref>100M
        {0x0205, 0x2A},
        {0x0208, 0x09},
        {0x0302, 0x09},
        {0x0303, 0x00},//#榛樿閰嶇疆
        {0x0303, 0x20},
        {0x0305, 0x00},  //# out1闄�1
        {0x0308, 0x09},
        {0x309, 0xE0},
        {0x030B, 0x09},
        {0x030E, 0x09},
        {0x030F, 0x00},
        {0x0311, 0x09},
        {0x0314, 0x09},
        {0x0315, 0x00},
        {0x0317, 0x09},
        {0x031A, 0x09},
        {0x031B, 0x00},
        {0x031D, 0x09},
        {0x0320, 0x09},
        {0x0321, 0x00},
        {0x032D, 0x01},  //# 澶栫亴REF涓嶇粡杩�9528PLL2
        {0x0323, 0x09},
        {0x324, 0xE0},
        {0x325, 0x80},
        {0x0326, 0x09},
        {0x0327, 0x20},
        {0x0329, 0x00},  //# out13闄�1
        //# {0x0401,0x0},
        //# {0x0403,0x8},
        {0x400, 0x00},  //# K,divider,[7:0]
        {0x401, 0x02},  //# K,divider,[15:8]
        {0x402, 0x1C},  //# {1'b0,,,2'b00,,,1'b1,,,1'b1,,2'b10,,1'b0},,,,,,,
        {0x403, 0x9A},  //# {2'b10,,2'b01,,,,3'b101,,1'b0}
        {0x403, 0x1B},  //# {2'b10,,2'b01,,,,3'b101,,1'b1},,//#start,the,sysref,generation,,,,
        //# {0x404,0x04},
        {0x404, 0x01},
        {0x0501, 0xF5},
        {0x0502, 0x0F},
        {0x0505, 0x07},
        {0x0506, 0x01},
        {0x0507, 0x0C},
        {0xF, 0x1},
};


uint16_t ad9528_sys_reg_on[][2] = {
        {0x403,0x9b},
        {0x404,0x01},
        {0xF,0x1},
};
uint16_t ad9528_sys_reg_off[][2] = {
        {0x403,0x1b},
        {0x404,0x00},
        {0xF,0x1},
};




u32 ad9528_sys_on(ad9528Device_t *pSPI)
{
	u32 cfg_data;
	u8 cfg_data_reg[3],i;
	for(i=0;i<3;i++){
        AD9528_spiWriteByte(pSPI, ad9528_sys_reg_on[i][0], ad9528_sys_reg_on[i][1]);

	}
	return 0;
}

u32 ad9528_sys_off(ad9528Device_t *pSPI)
{
	u32 cfg_data;
	u8 cfg_data_reg[3],i;
	for(i=0;i<3;i++){
        AD9528_spiWriteByte(pSPI, ad9528_sys_reg_off[i][0], ad9528_sys_reg_off[i][1]);
	}
	return 0;
}
void ad9528_sys_onoff(uint8_t on)
{
	ad9528Device_t *pAD9528_SPI = &ad9528_spi[0];
	if(on){
		ad9528_sys_on(pAD9528_SPI);
	}else{
		ad9528_sys_off(pAD9528_SPI);
	}
}


int ad9528_chip1_cfg(void)
{
    int i = 0;
	ad9528Device_t *pAD9528_SPI = &ad9528_spi[0];
	u8 cfg_read_data[ad9528_buffer_size];

    AD9528_resetDevice(pAD9528_SPI);
	for(i=0;i<sizeof(ad9528_chip1_reg_cfg)/sizeof(ad9528_chip1_reg_cfg[0]);i++){
        AD9528_spiWriteByte(pAD9528_SPI, ad9528_chip1_reg_cfg[i][0], ad9528_chip1_reg_cfg[i][1]);

	}
	delay_ms(100);
	for(i=0;i<sizeof(ad9528_chip1_reg_cfg)/sizeof(ad9528_chip1_reg_cfg[0]);i++){
        AD9528_spiReadByte(pAD9528_SPI, ad9528_chip1_reg_cfg[i][0], &cfg_read_data[i]);
		// if(cfg_read_data[i] != ad9528_chip1_reg_cfg[i][1]){
		// 	printf("ad9528 CFG err set:0x%x read: 0x%x\r\n",ad9528_chip1_reg_cfg[i][1], cfg_read_data[i]);
		// }
	}
	for(i=0;i<2;i++){
        AD9528_spiReadByte(pAD9528_SPI, 0x508+i, &cfg_read_data[i]);
        printf("ad9528 status:0x%x read: 0x%x\r\n",0x508+i, cfg_read_data[i]);
	}
	//sys on
//	ad9528_sys_on(pAD9528_SPI);
	delay_ms(100);
	printf("ad9528 1 CFG Success\r\n");

	return 0;

}

int ad9528_chip2_cfg(void)
{

	ad9528Device_t *pAD9528_SPI = &ad9528_spi[1];
    int i = 0;
    u8 cfg_read_data[ad9528_buffer_size];

//	gpio_direction_output(gpio_ad9528_resetn, 1);
//	delay_10ms();
//	gpio_direction_output(gpio_ad9528_resetn, 0);
//	delay_10ms();
//	gpio_direction_output(gpio_ad9528_resetn, 1);
//	delay_10ms();

	// cfg_data = 0x000081;
	// cfg_data_reg[2] = cfg_data & 0x0000ff;
	// cfg_data_reg[1] = cfg_data >> 8  & 0x0000ff;
	// cfg_data_reg[0] = cfg_data >> 16 & 0x0000ff;
	// spi_transfer(pAD9528_SPI, cfg_data_reg, NULL, 0x3);
	// delay_ms(100);
	// cfg_data = 0x000000;
	// cfg_data_reg[2] = cfg_data & 0x0000ff;
	// cfg_data_reg[1] = cfg_data >> 8  & 0x0000ff;
	// cfg_data_reg[0] = cfg_data >> 16 & 0x0000ff;
	// spi_transfer(pAD9528_SPI, cfg_data_reg, NULL, 0x3);
	// delay_ms(100);


    AD9528_resetDevice(pAD9528_SPI);
	for(i=0;i<sizeof(ad9528_chip2_reg_cfg)/sizeof(ad9528_chip2_reg_cfg[0]);i++){
        AD9528_spiWriteByte(pAD9528_SPI, ad9528_chip2_reg_cfg[i][0], ad9528_chip2_reg_cfg[i][1]);

	}
	delay_ms(100);
	for(i=0;i<sizeof(ad9528_chip2_reg_cfg)/sizeof(ad9528_chip2_reg_cfg[0]);i++){
        AD9528_spiReadByte(pAD9528_SPI, ad9528_chip2_reg_cfg[i][0], &cfg_read_data[i]);
		if(cfg_read_data[i] != ad9528_chip2_reg_cfg[i][1]){
			printf("ad9528 CFG err set:0x%x read: 0x%x\r\n",ad9528_chip1_reg_cfg[i][1], cfg_read_data[i]);
		}
	}
	delay_ms(100);
	printf("ad9528 2 CFG Success\r\n");

	return 0;

}


int ad9528_cfg(void)
{

	// gpio_direction_output(gpio_ad9528_resetn, 1);
	// delay_10ms();
	// gpio_direction_output(gpio_ad9528_resetn, 0);
	// delay_10ms();
	// gpio_direction_output(gpio_ad9528_resetn, 1);
	// delay_10ms();
	ad9528_chip1_cfg();
	ad9528_chip1_cfg();
	if(AD9528_NUM > 1){
		ad9528_chip2_cfg();
		ad9528_chip2_cfg();
	}
	printf("ad9528 CFG Success\r\n");
	return 0;

}
#include "parameters.h"
int ad9528_device_init(void)
{
	struct spi_init_param spi_param;
	int32_t status;
	int i = 0;
	ad9528Device_t *device = NULL;
	uint8_t reset_pin[2]={CLK_HPC_RESETB};
    for(i = 0; i < AD9528_NUM; i++){
    	device =&ad9528_spi[i];
		status = gpio_get(&device->gpio_resetb, reset_pin[i]);
//		status |= gpio_get(&device->gpio_sysref_req, ADRV_SYSREF_REQ);
		spi_param.flags |= SPI_CS_DECODE;
		spi_param.id = i;
		spi_param.mode = SPI_MODE_0;
		spi_param.chip_select = CLK_CS;
		status |= spi_init(&device->spi_desc, &spi_param);

		if (status != SUCCESS)
			return FRHAL_ERR;
        
        AD9528_resetDevice(device);
    }
	return FRHAL_OK;
}


