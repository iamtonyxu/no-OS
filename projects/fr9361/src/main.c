#include <stdio.h>
#include <inttypes.h>
//xilinx headers
#include <xparameters.h>
#include <xil_cache.h>
#include "xilinx_irq.h"
#include "xilinx_spi.h"
#include "xilinx_gpio.h"
#include "xilinx_uart.h"
//no-os headers
#include "no_os_uart.h"
#include "no_os_spi.h"
#include "no_os_gpio.h"
#include "no_os_irq.h"
#include "no_os_error.h"
//axi-components headers
#include "axi_sysid.h"
#include "axi_adc_core.h"
#include "axi_dac_core.h"
#include "axi_dmac.h"
//fr9361 custom header files
#include "custom_cfg.h"
#include "main_init.h"
#include "spi_rw.h"
#include "driver.h"

/******************************************************************************/
/************************ Micro-Definitions ***********************************/
/******************************************************************************/
#define SPI_DEVICE_ID			XPAR_PS7_SPI_0_DEVICE_ID
#define SPI_CS                  0
#define SPI_OPS					&xil_spi_ops


#define UART_DEVICE_ID			XPAR_XUARTPS_0_DEVICE_ID
#define UART_IRQ_ID				XPAR_XUARTPS_1_INTR
#define UART_BAUDRATE 			115200

#define GPIO_DEVICE_ID			XPAR_PS7_GPIO_0_DEVICE_ID
#define GPIO_OPS				&xil_gpio_ops
#define GPIO_RESET_PIN			100
#define GPIO_SYNC_PIN			99
#define GPIO_ENABLE_PIN			101
#define GPIO_TXNRX_PIN			102

#define DAC_BUFFER_SAMPLES		16384
#define ADC_BUFFER_SAMPLES 		16384
#define ADC_CHANNELS 			4
#define RX_CORE_BASEADDR		XPAR_AXI_AD9361_0_BASEADDR
#define TX_CORE_BASEADDR		(XPAR_AXI_AD9361_0_BASEADDR + 0x4000)
#define CF_AD9361_RX_DMA_BASEADDR	XPAR_AXI_DMAC_0_BASEADDR
#define CF_AD9361_TX_DMA_BASEADDR	XPAR_AXI_DMAC_1_BASEADDR


/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
struct xil_spi_init_param xil_spi_param = {
	.type = SPI_PS,
	.flags = 0
};

struct xil_gpio_init_param xil_gpio_param = {
	.type = GPIO_PS,
	.device_id = GPIO_DEVICE_ID
};

struct no_os_spi_init_param	spi_param =
{
	.device_id = SPI_DEVICE_ID,
	.mode = NO_OS_SPI_MODE_1,
	.chip_select = SPI_CS,
	.platform_ops = SPI_OPS,
	.extra = &xil_spi_param,
	.max_speed_hz = 0, //0:prescaler_default(64) is used, spi_clk = 167/64 = 2.6MHz
};

//gpio_resetb
struct no_os_gpio_init_param gpio_resetb =
{
	.number = GPIO_RESET_PIN,
	.platform_ops = GPIO_OPS,
	.extra = &xil_gpio_param,
};

//gpio_sync
struct no_os_gpio_init_param gpio_sync =
{
	.number = GPIO_SYNC_PIN,
	.platform_ops = GPIO_OPS,
	.extra = &xil_gpio_param,
};

struct no_os_spi_desc **spi_desc;
struct no_os_gpio_desc 	*gpio_desc_resetb;
struct no_os_gpio_desc 	*gpio_desc_sync;

uint32_t dac_buffer[DAC_BUFFER_SAMPLES] __attribute__ ((aligned));
uint16_t adc_buffer[ADC_BUFFER_SAMPLES * ADC_CHANNELS] __attribute__ ((aligned));
extern const uint32_t sine_lut_iq[1024];

struct axi_adc *rx_adc;
struct axi_adc_init rx_adc_init = {
	.name = "cf-fr9361-rx-adc-lpc",
	.base = RX_CORE_BASEADDR,
	.num_channels = 4,
	.num_slave_channels =  4
};

struct axi_dac *tx_dac;
struct axi_dac_init tx_dac_init = {
	"cf-fr9361-tx-dac-lpc",
	TX_CORE_BASEADDR,
	4,
	NULL,
	1
};

struct axi_dmac_init rx_dmac_init = {
	"rx_dmac",
	CF_AD9361_RX_DMA_BASEADDR,
	IRQ_DISABLED
};

struct axi_dmac *rx_dmac;
struct axi_dmac_init tx_dmac_init = {
	"tx_dmac",
	CF_AD9361_TX_DMA_BASEADDR,
	IRQ_DISABLED
};

struct axi_dmac *tx_dmac;
struct axi_dma_transfer transfer = {
	// Number of bytes to write/read
	.size = sizeof(sine_lut_iq),
	// Transfer done flag
	.transfer_done = 0,
	// Signal transfer mode
	.cyclic = CYCLIC,
	// Address of data source
	.src_addr = (uintptr_t)dac_buffer,
	// Address of data destination
	.dest_addr = 0
};

struct axi_dma_transfer read_transfer = {
	// Number of bytes to write/read
	.size = sizeof(adc_buffer),
	// Transfer done flag
	.transfer_done = 0,
	// Signal transfer mode
	.cyclic = NO,
	// Address of data source
	.src_addr = 0,
	// Address of data destination
	.dest_addr = (uintptr_t)adc_buffer
};

/******************************************************************************/
/************************ Function Definitions ********************************/
/******************************************************************************/
void parse_spi_command(struct no_os_spi_desc *spi);


/******************************************************************************/
/************************ Main function ***************************************/
/******************************************************************************/
int main(void)
{
	int32_t status = -1;
	uint8_t spi_wrdata = 0, spi_rddata = 0;

	Xil_ICacheEnable();
	Xil_DCacheEnable();

	printf("\nHello from ZYNQ...\n");

	no_os_gpio_get(&gpio_desc_resetb, &gpio_resetb);

	//fr9361_reset
	if(gpio_desc_resetb)
	{
		no_os_gpio_direction_output(gpio_desc_resetb, NO_OS_GPIO_OUT);
		no_os_gpio_set_value(gpio_desc_resetb, 0); // DMM to check resetb = 0
		no_os_mdelay(1);
		no_os_gpio_set_value(gpio_desc_resetb, 1);// DMM to check resetb = 1
		no_os_mdelay(1);
	}

	// spi init
	if(no_os_spi_init(spi_desc, &spi_param) != 0)
	{
		status = -2;
		printf("\nfr9361 spi init failed, status = %d\n", status);
	}

	// spi hook
	spi_hook(*spi_desc);
#if 0
	// Debugging on AD9361_WR/RD
	AD9361_WR(0x900, 0x07);
	no_os_mdelay(1);
	AD9361_WR(0x904, 0xA4);
	no_os_mdelay(1);

	spi_rddata = AD9361_RD(0x615);
	spi_wrdata = 0x5a;
	AD9361_WR(0x615, spi_wrdata);
	spi_rddata = AD9361_RD(0x615);
	if(spi_rddata != spi_wrdata)
	{
		printf("spi access error, addr=0x615, wrdata=0x%x, rddata=0x%x\n", spi_wrdata, spi_rddata);
		return -1;
	}
#endif

	// fr9361 init
    status = fr936x_init(&g_phy_obj[0], &g_phy_config[0]);
    if (status < 0)
    {
        printf("chip init failed!\n");
        return status;
    }

    //cmos if
    if(CMOS_IF == g_phy_obj[0].config->dig_if)
    {
        AD9361_WR(0x67, 0xFF);
        no_os_mdelay(1);
        AD9361_WR(0x6B, 0xA0); // adjust timing between fclk and data
        no_os_mdelay(1);
    }

    // tx_dmac init
	status = axi_dmac_init(&tx_dmac, &tx_dmac_init);
	if (status < 0) {
		printf("axi_dmac_init tx init error: %"PRIi32"\n", status);
		return status;
	}

	// rx dmac init
	status = axi_dmac_init(&rx_dmac, &rx_dmac_init);
	if (status < 0) {
		printf("axi_dmac_init rx init error: %"PRIi32"\n", status);
		return status;
	}

  	/* dac init */
	// mode = 2t2r
	tx_dac_init.num_channels = 4;
	tx_dac_init.rate = 1;
	axi_dac_init(&tx_dac, &tx_dac_init); // fpga dds is working!

	/* adc init */
	rx_adc_init.num_channels = 4;
	rx_adc_init.num_slave_channels = 0;
	axi_adc_init(&rx_adc, &rx_adc_init);

	/* set data selection */
	axi_dac_set_datasel(tx_dac, -1, AXI_DAC_DATA_SEL_DMA);

	/* load custom data */
  	axi_dac_load_custom_data_v2(tx_dac, sine_lut_iq, sine_lut_iq,
  				 	 	 	 NO_OS_ARRAY_SIZE(sine_lut_iq),
							 (uintptr_t)dac_buffer);

  	Xil_DCacheFlush();

	/* Transfer the data. */
	axi_dmac_transfer_start(tx_dmac, &transfer);

	/* Flush cache data. */
	Xil_DCacheInvalidateRange((uintptr_t)dac_buffer, sizeof(sine_lut_iq));

	no_os_mdelay(1000);

	/* Read the data from the ADC DMA. */
	axi_dmac_transfer_start(rx_dmac, &read_transfer);

	/* Wait until transfer finishes */
	status = axi_dmac_transfer_wait_completion(rx_dmac, 500);
	if(status < 0) {
		printf("failed to read rx data, timeout: %d\n", status);
		return status;
	}
	Xil_DCacheInvalidateRange((uintptr_t)adc_buffer, sizeof(adc_buffer));
	printf("DMA_EXAMPLE: address=%#lx samples=%lu channels=%u bits=%lu\n",
		   (uintptr_t)adc_buffer, NO_OS_ARRAY_SIZE(adc_buffer), rx_adc_init.num_channels,
		   8 * sizeof(adc_buffer[0]));


	// parse api command
	while(1)
	{
		parse_spi_command(*spi_desc);
	}

	Xil_DCacheDisable();
	Xil_ICacheDisable();
	return 0;
}

/******************************************************************************/
/************************ parse_spi_command Functions *************************/
/******************************************************************************/
void parse_spi_command(struct no_os_spi_desc *spi)
{
	struct xil_uart_init_param platform_uart_init_par = {
		.type = UART_PS,
		.irq_id = UART_IRQ_ID
	};

	struct no_os_uart_init_param uart_param = {
		.device_id = UART_DEVICE_ID,
		.irq_id = UART_IRQ_ID,
		.baud_rate = UART_BAUDRATE,
		.size = NO_OS_UART_CS_8,
		.parity = NO_OS_UART_PAR_NO,
		.stop = NO_OS_UART_STOP_1_BIT,
		.extra = &platform_uart_init_par,
		.platform_ops = &xil_uart_ops
	};

	struct no_os_uart_desc *uart_desc;
#define MAX_SIZE (16384*4*2)
	uint32_t bytes_number = 10;
	uint8_t wr_data[MAX_SIZE] = {0};
	uint32_t bytes_recv = 0;
	int32_t error = 0;

	uint8_t spi_mode = 0u;
	uint32_t spi_addr = 0;
	uint32_t spi_data = 0;

	uint8_t chan = 0;
	error = no_os_uart_init(&uart_desc, &uart_param);

	if(error == 0)
	{
		while(1)
		{
			bytes_number = 10; // length of spi_write and spi_read
			// receive data
			bytes_recv = no_os_uart_read(uart_desc, wr_data, bytes_number);

			if(bytes_recv == bytes_number)
			{
				// spi write
				spi_mode = wr_data[1];
				spi_addr = (wr_data[2] << 3*8) | (wr_data[3] << 2*8) | (wr_data[4] << 1*8) | wr_data[5];
				spi_data = (wr_data[6] << 3*8) | (wr_data[7] << 2*8) | (wr_data[8] << 1*8) | wr_data[9];
				if(wr_data[0] == 0x5A)
				{
					AD9361_WR(spi_addr, spi_data);
				}
				else if(wr_data[0] == 0x5B)
				{
#if 1
					// spi read
					spi_data = AD9361_RD(spi_addr);
#else
					spi_data = 0xa1b2c3e4;
#endif
					// send data
					wr_data[6] = (spi_data >> 3*8) & 0xff;
					wr_data[7] = (spi_data >> 2*8) & 0xff;
					wr_data[8] = (spi_data >> 1*8) & 0xff;
					wr_data[9] = (spi_data >> 0*8) & 0xff;
					no_os_uart_write(uart_desc, wr_data, bytes_number);
				}
				else if(wr_data[0] == 0x5C)
				{
					bytes_number = (wr_data[1] << 2*8) | (wr_data[2] << 1*8) | (wr_data[3] << 0*8);
					bytes_recv = no_os_uart_read(uart_desc, wr_data, bytes_number);
					if(bytes_number/4 <= DAC_BUFFER_SAMPLES)
					{
						for(int sample = 0; sample < bytes_number/4; sample++)
						{
							uint32_t iq = (wr_data[sample*4 + 1] << 0) |
										(wr_data[sample*4 + 0] << 8) |
										(wr_data[sample*4 + 3] << 16) |
										(wr_data[sample*4 + 2] << 24);
							zero_lut_iq[sample] = iq;
						}
						/* Reload transfer data memory and transfer the data */
						/* Stop tranfering the data. */
						axi_dmac_transfer_stop(tx_dmac);

						/* Reload the waveform */
						axi_dac_load_custom_data_v2(tx_dac, zero_lut_iq, zero_lut_iq,
									 NO_OS_ARRAY_SIZE(zero_lut_iq),
									 (uintptr_t)dac_buffer);
						Xil_DCacheFlush();

						/* Transfer the data. */
						transfer.size = bytes_number;
						axi_dmac_transfer_start(tx_dmac, &transfer);

						/* Flush cache data. */
						Xil_DCacheInvalidateRange((uintptr_t)dac_buffer, sizeof(zero_lut_iq));
					}
					no_os_mdelay(10);
				}
				else if(wr_data[0] == 0x5D)
				{
					int32_t status = -1;

					/* Read the data from the ADC DMA. */
					axi_dmac_transfer_start(rx_dmac, &read_transfer);

					/* Wait until transfer finishes */
					status = axi_dmac_transfer_wait_completion(rx_dmac, 500);

					/* Flush cache data. */
					Xil_DCacheInvalidateRange((uintptr_t)adc_buffer, sizeof(adc_buffer));

					bytes_number = (wr_data[1] << 2*8) | (wr_data[2] << 1*8) | (wr_data[3] << 0*8);
					if(status < 0)
					{
						memset(wr_data, 0, bytes_number);
					}
					else
					{
						memcpy(wr_data, adc_buffer, bytes_number);
					}
					uint32_t bytes_send = 0u;
					const uint32_t bytes_chunk = 4096u;

					while(bytes_send + bytes_chunk < bytes_number)
					{
						no_os_uart_write(uart_desc, &wr_data[bytes_send], bytes_chunk);
						bytes_send += bytes_chunk;
					}
					if(bytes_send < bytes_number)
					{
						no_os_uart_write(uart_desc, &wr_data[bytes_send], (bytes_number-bytes_send));
					}

					no_os_mdelay(10);
				}
				else if(wr_data[0] == 0x60)
				{
					// read rx power: rx_rssi_get
					chan = wr_data[1];
					int rssi = rx_rssi_get(&g_phy_obj[0], chan);
					// send data
					wr_data[2] = (rssi >> 3*8) & 0xff;
					wr_data[3] = (rssi >> 2*8) & 0xff;
					wr_data[4] = (rssi >> 1*8) & 0xff;
					wr_data[5] = (rssi >> 0*8) & 0xff;
					no_os_uart_write(uart_desc, wr_data, bytes_number);
				}
				else if(wr_data[0] == 0x61)
				{
					//set tx tone: cmd_api_tx_tone
					chan = wr_data[1];
					short on = wr_data[2];
					long freq = (wr_data[3] << 3*8) | (wr_data[4] << 2*8) | (wr_data[5] << 1*8) | wr_data[6];
					cmd_api_tx_tone((TRX_CHN_ENUM)chan, on, freq);
				}
				else if(wr_data[0] == 0x62)
				{
					//set rx gain: chip_rx_mgc_gain
					chan = wr_data[1];
					RX_MGC_GAIN_ENUM tb = (RX_MGC_GAIN_ENUM)wr_data[2];
					unsigned char val = wr_data[3];
					cmd_api_rx_mgc_split_table_gain((TRX_CHN_ENUM)chan, tb, val);
				}
				else if(wr_data[0] == 0x63)
				{
					//set debug option: module_debug_onoff
					unsigned long en = (wr_data[1] << 3*8) | (wr_data[2] << 2*8) | (wr_data[3] << 1*8) | wr_data[4];
					module_debug_onoff(&g_phy_obj[0], en);
				}
				else if(wr_data[0] == 0x64)
				{
					//read sxtrx_lock_status
					short dir = wr_data[1];
					int voltage = 0;
					short lock = sxtrx_lock_status(&g_phy_obj[0], dir, &voltage);
					// send data
					wr_data[2] = (lock >> 1*8) & 0xff;
					wr_data[3] = (lock >> 0*8) & 0xff;
					wr_data[4] = (voltage >> 3*8) & 0xff;
					wr_data[5] = (voltage >> 2*8) & 0xff;
					wr_data[6] = (voltage >> 1*8) & 0xff;
					wr_data[7] = (voltage >> 0*8) & 0xff;
					no_os_uart_write(uart_desc, wr_data, bytes_number);
				}
				else if(wr_data[0] == 0x65)
				{
					//set_tx_atten_chn
					chan = wr_data[1];
					unsigned long long flo = ( (unsigned long long)wr_data[2] << 3*8) |
											( (unsigned long long)wr_data[3] << 2*8) |
											( (unsigned long long)wr_data[4] << 1*8) |
											( (unsigned long long)wr_data[5] << 0*8);
					unsigned char gain = wr_data[6];
					short immed = wr_data[7];
					set_tx_atten_chn(&g_phy_obj[0], chan, flo, gain, immed);
				}
				else if(wr_data[0] == 0x66)
				{
					//set_tx_dig_atten
					chan = wr_data[1];
					unsigned short index = (wr_data[2] << 1*8) | wr_data[3];
					set_tx_dig_atten(&g_phy_obj[0], (TRX_CHN_ENUM)chan, index);
				}
				else if(wr_data[0] == 0x67)
				{
					//todo
					//read capture data from adc_ram: cmd_api_adc_ram_dump_v2
					chan = wr_data[1];
					uint16_t len = (wr_data[2] << 1*8) | wr_data[3];
					uint16_t I_data[1024] = {0};
					uint16_t Q_data[1024] = {0};
					cmd_api_adc_ram_dump_v2((TRX_CHN_ENUM)chan, I_data, Q_data, len);
					// send data
					bytes_number = len * 4 * 2;
					for(int i = 0; i < len; i++)
					{
						wr_data[i*4 + 0] = (I_data[i] >> 8) & 0xff;
						wr_data[i*4 + 1] = (I_data[i] >> 0) & 0xff;
						wr_data[i*4 + 2] = (Q_data[i] >> 8) & 0xff;
						wr_data[i*4 + 3] = (Q_data[i] >> 0) & 0xff;
					}

					uint32_t bytes_send = 0u;
					const uint32_t bytes_chunk = 4096u;

					while(bytes_send + bytes_chunk < bytes_number)
					{
						no_os_uart_write(uart_desc, &wr_data[bytes_send], bytes_chunk);
						bytes_send += bytes_chunk;
					}
					if(bytes_send < bytes_number)
					{
						no_os_uart_write(uart_desc, &wr_data[bytes_send], (bytes_number-bytes_send));
					}
					no_os_mdelay(10);
				}
			}
		}
	}
	no_os_uart_remove(&uart_desc);
}

