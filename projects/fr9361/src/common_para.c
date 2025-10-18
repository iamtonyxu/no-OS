#include <stdio.h>
#include <inttypes.h>

#include "common_para.h"

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

struct no_os_spi_desc **spi_desc;
struct no_os_gpio_desc 	*gpio_desc_resetb;

uint32_t dac_buffer[DAC_BUFFER_SAMPLES] __attribute__ ((aligned));
uint16_t adc_buffer[ADC_BUFFER_SAMPLES * ADC_CHANNELS] __attribute__ ((aligned));
extern const uint32_t sine_lut_iq[1024];

struct axi_adc **rx_adc;
struct axi_adc_init rx_adc_init = {
	.name = "cf-fr9361-lpc",
	.base = RX_CORE_BASEADDR,
	.num_channels = 4,
	.num_slave_channels =  4
};

struct axi_dac **tx_dac;
struct axi_dac_init tx_dac_init = {
	"cf-fr9361-dds-core-lpc",
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

int fr9361_hw_init(void)
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
		no_os_gpio_direction_output(gpio_desc_resetb, 0);
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

	// fr9361 init
	module_debug_onoff(&g_phy_obj[0], 0xFFF); // enable debug info print
    status = fr936x_init(&g_phy_obj[0], &g_phy_config[0]);
    if (status < 0)
    {
        printf("chip init failed!\n");
        return status;
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
	axi_dac_init(tx_dac, &tx_dac_init);

	/* adc init */
	axi_adc_init(rx_adc, &rx_adc_init);

	/* set data selection */
	axi_dac_set_datasel(*tx_dac, -1, AXI_DAC_DATA_SEL_DMA);

	/* load custom data */
  	axi_dac_load_custom_data_v2(*tx_dac, sine_lut_iq, sine_lut_iq,
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

	return 0;
}
