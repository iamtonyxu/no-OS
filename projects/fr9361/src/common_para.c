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