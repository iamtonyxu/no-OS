#ifndef _COMMON_PARA_H
#define _COMMON_PARA_H

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

#define LWIP_DEBUG				0

#define SPI_DEVICE_ID			XPAR_PS7_SPI_0_DEVICE_ID
#define SPI_CS                  0
#define SPI_OPS					&xil_spi_ops


#define UART_DEVICE_ID			XPAR_XUARTPS_0_DEVICE_ID
#define UART_IRQ_ID				XPAR_XUARTPS_1_INTR
#define UART_BAUDRATE 			115200

#define GPIO_DEVICE_ID			XPAR_PS7_GPIO_0_DEVICE_ID
#define GPIO_OPS				&xil_gpio_ops
#define GPIO_RESET_PIN			100
#define GPIO_ENABLE_PIN			101
#define GPIO_TXNRX_PIN			102

#define DAC_BUFFER_SAMPLES		4096
#define ADC_BUFFER_SAMPLES 		4096
#define ADC_CHANNELS 			4
#define RX_CORE_BASEADDR		XPAR_AXI_AD9361_0_BASEADDR
#define TX_CORE_BASEADDR		(XPAR_AXI_AD9361_0_BASEADDR + 0x4000)
#define CF_AD9361_RX_DMA_BASEADDR	XPAR_AXI_DMAC_0_BASEADDR
#define CF_AD9361_TX_DMA_BASEADDR	XPAR_AXI_DMAC_1_BASEADDR


/******************************************************************************/
/************************ global variable declaration**************************/
/******************************************************************************/

extern struct xil_spi_init_param xil_spi_param;
extern struct xil_gpio_init_param xil_gpio_param;
extern struct no_os_spi_init_param	spi_param;
extern struct no_os_gpio_init_param gpio_resetb;
extern struct no_os_spi_desc **spi_desc;
extern struct no_os_gpio_desc 	*gpio_desc_resetb;
extern uint32_t dac_buffer[DAC_BUFFER_SAMPLES] __attribute__ ((aligned));
extern uint16_t adc_buffer[ADC_BUFFER_SAMPLES * ADC_CHANNELS] __attribute__ ((aligned));
extern const uint32_t sine_lut_iq[1024];
extern struct axi_adc **rx_adc;
extern struct axi_adc_init rx_adc_init;
extern struct axi_dac **tx_dac;
extern struct axi_dac_init tx_dac_init;
extern struct axi_dmac_init rx_dmac_init;
extern struct axi_dmac *rx_dmac;
extern struct axi_dmac_init tx_dmac_init;
extern struct axi_dmac *tx_dmac;
extern struct axi_dma_transfer transfer;
extern struct axi_dma_transfer read_transfer;

/******************************************************************************/
/************************ global function declaration**************************/
/******************************************************************************/

int fr9361_hw_init(void);

#endif
