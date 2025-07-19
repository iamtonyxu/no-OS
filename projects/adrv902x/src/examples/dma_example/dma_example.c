/***************************************************************************//**
 *   @file   dma_example.c
 *   @brief  DMA example for the adrv902x project
 *   @author GMois (george.mois@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#include "dma_example.h"
#include "common_data.h"
#include "parameters.h"
#include "app_config.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "no_os_print_log.h"
#include "no_os_error.h"
#include "no_os_util.h"
#include "no_os_spi.h"
#include "no_os_uart.h"

#include "axi_jesd204_rx.h"
#include "axi_jesd204_tx.h"
#include "axi_adc_core.h"
#include "axi_dac_core.h"
#include "axi_adxcvr.h"
#include "axi_dmac.h"

#include "xilinx_gpio.h"
#include "xilinx_spi.h"
#include "xil_cache.h"
#include "xilinx_uart.h"

#include "adrv9025.h"
#include "ad9528.h"

/*******************************************************************************
 * global and static variables
*******************************************************************************/
#define DAC_DDR_ENABLE 1
#define BASIC_EXAMPLE 0
#define ORX_CAPTURE 1

uint32_t dac_buffer_dma[DAC_BUFFER_SAMPLES] __attribute__((aligned(16)));
//uint16_t adc_buffer_dma[ADC_BUFFER_SAMPLES * ADC_CHANNELS] __attribute__((aligned(16)));
uint8_t tx_is_transfering;

uint32_t zero_lut_iq[DAC_BUFFER_SAMPLES]; // buffer to receive dac waveform

struct adrv9025_init_param adrv9025_init_par = { 0 };
struct adi_adrv9025_Device adrv9025_device = { 0 };
adi_adrv9025_AgcCfg_t agcConfig_init_param = { 0 };
struct ad9528_platform_data ad9528_pdata = { 0 };
struct ad9528_channel_spec ad9528_channels[14];
struct ad9528_init_param ad9528_param;
struct ad9528_dev* ad9528_device;
struct adrv9025_rf_phy *phy;
int status;

struct axi_dmac_init rx_dmac_init = {
	"rx_dmac",
	RX_DMA_BASEADDR,
	IRQ_DISABLED
};
struct axi_dmac *rx_dmac;

struct axi_dmac_init rx_os_dmac_init = {
	"rx_os_dmac",
	RX_DMA_BASEADDR,
	IRQ_DISABLED
};
struct axi_dmac *rx_os_dmac;

struct axi_dmac_init tx_dmac_init = {
	"tx_dmac",
	TX_DMA_BASEADDR,
	IRQ_DISABLED
};
struct axi_dmac *tx_dmac;

struct axi_adc_init rx_adc_init = {
	.name = "rx_adc",
	.base = RX_CORE_BASEADDR,
	.num_channels = 8
};
struct axi_dac_init tx_dac_init = {
	.name = "tx_dac",
	.base = TX_CORE_BASEADDR,
	.channels = NULL,
	.rate = 3,
	.num_channels = 8
};

struct jesd204_tx_init tx_jesd_init = {
	.name = "tx_jesd",
	.base = TX_JESD_BASEADDR,
	.octets_per_frame = ADRV9025_TX_JESD_OCTETS_PER_FRAME,
	.frames_per_multiframe = ADRV9025_TX_JESD_FRAMES_PER_MULTIFRAME,
	.converters_per_device = ADRV9025_TX_JESD_CONVS_PER_DEVICE,
	.converter_resolution = ADRV9025_TX_JESD_CONV_RESOLUTION,
	.bits_per_sample = ADRV9025_TX_JESD_BITS_PER_SAMPLE,
	.high_density = ADRV9025_TX_JESD_HIGH_DENSITY,
	.control_bits_per_sample = ADRV9025_TX_JESD_CTRL_BITS_PER_SAMPLE,// optional
	.subclass = ADRV9025_TX_JESD_SUBCLASS,
	.device_clk_khz = ADRV9025_DEVICE_CLK_KHZ,
	.lane_clk_khz = ADRV9025_LANE_RATE_KHZ
};

struct jesd204_rx_init rx_jesd_init = {
	.name = "rx_jesd",
	.base = RX_JESD_BASEADDR,
	.octets_per_frame = ADRV9025_RX_JESD_OCTETS_PER_FRAME,
	.frames_per_multiframe = ADRV9025_RX_JESD_FRAMES_PER_MULTIFRAME,
	.subclass = ADRV9025_RX_JESD_SUBCLASS,
	.device_clk_khz = ADRV9025_DEVICE_CLK_KHZ,
	.lane_clk_khz = ADRV9025_LANE_RATE_KHZ
};

struct adxcvr_init tx_adxcvr_init = {
	.name = "tx_adxcvr",
	.base = TX_XCVR_BASEADDR,
	.sys_clk_sel = ADXCVR_SYS_CLK_QPLL0,
	.out_clk_sel = ADXCVR_REFCLK,
	.lpm_enable = 0,
	.lane_rate_khz = ADRV9025_LANE_RATE_KHZ,
	.ref_rate_khz = ADRV9025_DEVICE_CLK_KHZ,
	.export_no_os_clk = true
};
struct adxcvr *tx_adxcvr;

struct adxcvr_init rx_adxcvr_init = {
	.name = "rx_adxcvr",
	.base = RX_XCVR_BASEADDR,
	.sys_clk_sel = ADXCVR_SYS_CLK_CPLL,
	.out_clk_sel = ADXCVR_REFCLK,
	.lpm_enable = 1,
	.lane_rate_khz = ADRV9025_LANE_RATE_KHZ,
	.ref_rate_khz = ADRV9025_DEVICE_CLK_KHZ,
	.export_no_os_clk = true
};
struct adxcvr *rx_adxcvr;

/*******************************************************************************
 * external variables
*******************************************************************************/
extern const uint32_t sine_lut_iq[1024];

/*******************************************************************************
 * static functions
*******************************************************************************/
static void parse_spi_command(void *devHalInfo);

/***************************************************************************//**
 * @brief DMA example main execution.
 *
 * @return ret - Result of the example execution.
*******************************************************************************/
int dma_example_main(void)
{
	Xil_ICacheEnable();
	Xil_DCacheEnable();

	ad9528_param.spi_init = ad9528_spi_param;

	// Export no_os_clk_desc for each channel
	ad9528_param.export_no_os_clk = true;

	// ad9528 defaults
	ad9528_param.gpio_resetb = &clkchip_gpio_init_param;
	ad9528_param.pdata = &ad9528_pdata;
	ad9528_param.pdata->num_channels = 14;
	ad9528_param.pdata->channels = &ad9528_channels[0];
	ad9528_init(&ad9528_param);

	// channel 0
	ad9528_channels[0].channel_num = 0;
	ad9528_channels[0].driver_mode = DRIVER_MODE_LVDS;
	ad9528_channels[0].divider_phase = 0;
	ad9528_channels[0].channel_divider = 5;
	ad9528_channels[0].signal_source = SOURCE_SYSREF_VCO;
	ad9528_channels[0].output_dis = 0;

	// channel 1
	ad9528_channels[1].channel_num = 1;
	ad9528_channels[1].driver_mode = DRIVER_MODE_LVDS;
	ad9528_channels[1].divider_phase = 0;
	ad9528_channels[1].channel_divider = 5;
	ad9528_channels[1].signal_source = SOURCE_VCO;
	ad9528_channels[1].output_dis = 0;

	// channel 3
	ad9528_channels[3].channel_num = 3;
	ad9528_channels[3].driver_mode = DRIVER_MODE_LVDS;
	ad9528_channels[3].divider_phase = 0;
	ad9528_channels[3].channel_divider = 5;
	ad9528_channels[3].signal_source = SOURCE_VCO;
	ad9528_channels[3].output_dis = 0;

	// channel 12
	ad9528_channels[12].channel_num = 12;
	ad9528_channels[12].driver_mode = DRIVER_MODE_LVDS;
	ad9528_channels[12].divider_phase = 0;
	ad9528_channels[12].channel_divider = 5;
	ad9528_channels[12].signal_source = SOURCE_SYSREF_VCO;
	ad9528_channels[12].output_dis = 0;

	// channel 13
	ad9528_channels[13].channel_num = 13;
	ad9528_channels[13].driver_mode = DRIVER_MODE_LVDS;
	ad9528_channels[13].divider_phase = 0;
	ad9528_channels[13].channel_divider = 5;
	ad9528_channels[13].signal_source = SOURCE_VCO;
	ad9528_channels[13].output_dis = 0;

	// pllx settings
	ad9528_param.pdata->spi3wire = 0;
	ad9528_param.pdata->vcxo_freq = 122880000;
	ad9528_param.pdata->osc_in_diff_en = 0;
	ad9528_param.pdata->refa_en = 1;
	ad9528_param.pdata->refa_diff_rcv_en = 1;
	ad9528_param.pdata->refb_diff_rcv_en = 0;
	ad9528_param.pdata->osc_in_diff_en = 0;
	/* JESD */
	ad9528_param.pdata->jdev_desired_sysref_freq = 7680000 / 2;
	/* PLL1 config */
	ad9528_param.pdata->pll1_feedback_div = 4;
	ad9528_param.pdata->pll1_charge_pump_current_nA = 5000;
	ad9528_param.pdata->ref_mode = REF_MODE_EXT_REF;
	/* PLL2 config */
	ad9528_param.pdata->pll2_charge_pump_current_nA = 805000;
	ad9528_param.pdata->pll2_vco_div_m1 = 3;
	ad9528_param.pdata->pll2_r1_div = 1;
	ad9528_param.pdata->pll2_ndiv_a_cnt = 3;
	ad9528_param.pdata->pll2_ndiv_b_cnt = 27;
	ad9528_param.pdata->pll2_n2_div = 10;
	/* SYSREF config */
	ad9528_param.pdata->sysref_src = SYSREF_SRC_INTERNAL;
	ad9528_param.pdata->sysref_k_div = 512;
	ad9528_param.pdata->sysref_pattern_mode = SYSREF_PATTERN_NSHOT;
	ad9528_param.pdata->sysref_nshot_mode = SYSREF_NSHOT_8_PULSES;
	ad9528_param.pdata->sysref_req_trigger_mode = SYSREF_LEVEL_HIGH;
	ad9528_param.pdata->sysref_req_en = false;
	ad9528_param.pdata->rpole2 = RPOLE2_900_OHM;
	ad9528_param.pdata->rzero = RZERO_1850_OHM;
	ad9528_param.pdata->cpole1 = CPOLE1_16_PF;
	ad9528_param.pdata->pll1_bypass_en = false;
	ad9528_param.pdata->pll2_bypass_en = false;
	ad9528_param.pdata->stat0_pin_func_sel = 1; /* PLL1 & PLL2 Locked */
	ad9528_param.pdata->stat1_pin_func_sel = 7; /* REFA Correct */

	status = ad9528_setup(&ad9528_device, ad9528_param);
	if (status != 0) {
		printf("error: ad9528_setup() failed\n");
		goto error;
	}

	status = adxcvr_init(&tx_adxcvr, &tx_adxcvr_init);
	if (status)
		goto error_1;

	status = adxcvr_init(&rx_adxcvr, &rx_adxcvr_init);
	if (status)
		goto error_2;

	struct axi_jesd204_rx *rx_jesd;
	struct axi_jesd204_tx *tx_jesd;

	rx_jesd_init.lane_clk = rx_adxcvr->clk_out;
	tx_jesd_init.lane_clk = tx_adxcvr->clk_out;

	status = axi_jesd204_tx_init(&tx_jesd, &tx_jesd_init);
	if (status)
		goto error_3;

	status = axi_jesd204_rx_init(&rx_jesd, &rx_jesd_init);
	if (status)
		goto error_4;

	adrv9025_init_par.adrv9025_device = &adrv9025_device;
	adrv9025_init_par.dev_clk = ad9528_device->clk_desc[1];
	adrv9025_init_par.streamImageFile = ADRV9025_STREAM_IMAGE_FILE;
	adrv9025_init_par.agcConfig_init_param = &agcConfig_init_param;

	status = adrv9025_init(&phy, &adrv9025_init_par);
	if (status) {
		pr_err("error: adrv9025_init() failed\n");
		goto error_5;
	}

	//status = axi_dac_init(&phy->tx_dac, &tx_dac_init);
	status = axi_dac_init_begin(&phy->tx_dac, &tx_dac_init);
	if (status)
		goto error_6;

	//status = axi_adc_init(&phy->rx_os_adc, &rx_os_adc_init);
	status = axi_adc_init_begin(&phy->rx_adc, &rx_adc_init);
	if (status)
		goto error_7;

 	// todo: why reset Tx DAC and Rx ADC here???
#if 1
	// Reset Tx DAC
	axi_adc_write(phy->rx_adc, 0x4040, 0);
	axi_adc_write(phy->rx_adc, 0x4040,
		      NO_OS_BIT(1) | NO_OS_BIT(0));

	// Reset Rx ADC
	axi_adc_write(phy->rx_adc, AXI_ADC_REG_RSTN, 0);
	axi_adc_write(phy->rx_adc, AXI_ADC_REG_RSTN,
		      AXI_ADC_MMCM_RSTN | AXI_ADC_RSTN);
#endif

	status = adrv9025_post_setup(phy);
	if (status) {
		pr_err("error: adrv9025_post_setup() failed\n");
		goto error_8;
	}

#if BASIC_EXAMPLE
#if 1
	// Set DDS data
	axi_dac_data_setup(phy->tx_dac);
#else
	// Set PN
	axi_dac_set_datasel(phy->tx_dac, -1, AXI_DAC_DATA_SEL_ZERO);
#endif
#endif //BASIC_EXAMPLE

	status = axi_dmac_init(&tx_dmac, &tx_dmac_init);
	if (status) {
		printf("axi_dmac_init tx init error: %d\n", status);
		goto error_8;
	}
	status = axi_dmac_init(&rx_dmac, &rx_dmac_init);
	if (status) {
		printf("axi_dmac_init rx init error: %d\n", status);
		goto error_9;
	}
	status = axi_dmac_init(&rx_os_dmac, &rx_os_dmac_init);
	if (status) {
		printf("OBS axi_dmac_init() rx init error: %d\n", status);
		goto error_9;
	}

struct axi_dma_transfer transfer = {
	// Number of bytes to write/read
	.size = sizeof(sine_lut_iq),
	// Transfer done flag
	.transfer_done = 0,
	// Signal transfer mode
	.cyclic = CYCLIC,
	// Address of data source
#if DAC_DDR_ENABLE
	.src_addr = (uintptr_t)DAC_DDR_BASEADDR,
#else
	.src_addr = (uintptr_t)dac_buffer_dma,
#endif
	// Address of data destination
	.dest_addr = 0
};

struct axi_dma_transfer read_transfer = {
	// Number of bytes to write/read
	.size = ADC_BUFFER_SAMPLES * 2,
	// Transfer done flag
	.transfer_done = 0,
	// Signal transfer mode
	.cyclic = NO,
	// Address of data source
	.src_addr = 0,
	// Address of data destination
	.dest_addr = (uintptr_t)ADC_DDR_BASEADDR
};

#if BASIC_EXAMPLE == 0
	axi_dac_set_datasel(phy->tx_dac, -1, AXI_DAC_DATA_SEL_DMA);
#if DAC_DDR_ENABLE
	axi_dac_load_custom_data(phy->tx_dac, sine_lut_iq,
				 NO_OS_ARRAY_SIZE(sine_lut_iq),
				 (uintptr_t)DAC_DDR_BASEADDR);
#else
	axi_dac_load_custom_data(phy->tx_dac, sine_lut_iq,
							NO_OS_ARRAY_SIZE(sine_lut_iq),
							(uintptr_t)dac_buffer_dma);
#endif
#endif

	Xil_DCacheFlush();

	struct jesd204_topology *topology;
	struct jesd204_topology_dev devs[] = {
		{
			.jdev = ad9528_device->jdev,
			.link_ids = {DEFRAMER0_LINK_TX, FRAMER0_LINK_RX},
			.links_number = 2,
			.is_sysref_provider = true,
		},
		{
			.jdev = rx_jesd->jdev,
			.link_ids = {FRAMER0_LINK_RX},
			.links_number = 1,
		},
		{
			.jdev = tx_jesd->jdev,
			.link_ids = {DEFRAMER0_LINK_TX},
			.links_number = 1,
		},
		{
			.jdev = phy->jdev,
			.link_ids = {DEFRAMER0_LINK_TX, FRAMER0_LINK_RX},
			.links_number = 2,
			.is_top_device = true,
		},
	};

	status = adi_adrv9025_HwOpen(phy->madDevice, &phy->spiSettings);
	if (status) {
		pr_err("error: adi_adrv9025_HwOpen() failed\n");
		goto error_8;
	}

	jesd204_topology_init(&topology, devs,
			      sizeof(devs) / sizeof(*devs));

	jesd204_fsm_start(topology, JESD204_LINKS_ALL);

	axi_jesd204_tx_status_read(tx_jesd);
	axi_jesd204_rx_status_read(rx_jesd);

#if 1
	// Note: run adrv9025 init cals
	status = adrv9025_jesd204_link_setup(phy->jdev, JESD204_STATE_OP_REASON_INIT);
	if(status != JESD204_STATE_CHANGE_DONE){
		goto error_10;
	}
	status = adi_adrv9025_HwOpen(phy->madDevice, &phy->spiSettings);
	if (status) {
		pr_err("error: adi_adrv9025_HwOpen() failed\n");
		goto error_8;
	}
#endif

#if 0
	pr_info("DMA_EXAMPLE Tx: address=%#lx samples=%lu channels=%u bits=%lu\n",
		(uintptr_t)dac_buffer_dma, NO_OS_ARRAY_SIZE(dac_buffer_dma),
		tx_dac_init.num_channels,
		8 * sizeof(dac_buffer_dma[0]));
#endif

#if BASIC_EXAMPLE == 0
	/* Transfer the data. */
	axi_dmac_transfer_start(tx_dmac, &transfer);
	axi_dmac_transfer_wait_completion(tx_dmac, 500);

	/* Flush cache data. */
#if DAC_DDR_ENABLE
	Xil_DCacheInvalidateRange((uintptr_t)DAC_DDR_BASEADDR, sizeof(sine_lut_iq));
#else
	Xil_DCacheInvalidateRange((uintptr_t)dac_buffer_dma, sizeof(sine_lut_iq));
#endif
#endif

	tx_is_transfering = 1u; // 1 indicates tx is transfering the data...
	no_os_mdelay(1000);

#if 1
	/* Read the data from the ADC DMA. */
#if ORX_CAPTURE == 0
	axi_dmac_transfer_start(rx_dmac, &read_transfer);
#else
	axi_dmac_transfer_start(rx_os_dmac, &read_transfer);
#endif

	/* Wait until transfer finishes */
#if ORX_CAPTURE == 0
	status = axi_dmac_transfer_wait_completion(rx_dmac, 1000);
#else
	status = axi_dmac_transfer_wait_completion(rx_os_dmac, 1000);
#endif

	if (status)
		goto error_10;

	Xil_DCacheInvalidateRange((uintptr_t)ADC_DDR_BASEADDR, ADC_BUFFER_SAMPLES*2);
#endif

#if 0
	pr_info("DMA_EXAMPLE Rx: address=%#lx samples=%lu channels=%u bits=%lu\n",
		(uintptr_t)adc_buffer_dma, NO_OS_ARRAY_SIZE(adc_buffer_dma),
		rx_adc_init.num_channels,
		8 * sizeof(adc_buffer_dma[0]));
#endif

	/* no-ending loop to handle commands from PC */
	parse_spi_command((void *)phy->madDevice);

error_10:
	axi_dmac_remove(rx_dmac);
error_9:
	axi_dmac_remove(tx_dmac);
error_8:
	axi_adc_remove(phy->rx_adc);
error_7:
	axi_dac_remove(phy->tx_dac);
error_6:
	adrv9025_remove(phy);
error_5:
	axi_jesd204_rx_remove(rx_jesd);
error_4:
	axi_jesd204_tx_remove(tx_jesd);
error_3:
	adxcvr_remove(rx_adxcvr);
error_2:
	adxcvr_remove(tx_adxcvr);
error_1:
	ad9528_remove(ad9528_device);

	/* If we are here and status non-zero, diplay status */
	if (status)
		goto error;

	printf("Bye \n");

	/* Disable the instruction cache. */
	Xil_ICacheDisable();
	/* Disable the data cache. */
	Xil_DCacheDisable();

	return 0;

error:
	printf("Error %d.\n", status);
	return status;
}


#if 1
static void parse_spi_command(void *devHalInfo)
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

#if 1
	struct axi_dma_transfer transfer = {
		// Number of bytes to write/read
		.size = sizeof(zero_lut_iq),
		// Transfer done flag
		.transfer_done = 0,
		// Signal transfer mode
		.cyclic = CYCLIC,
		// Address of data source
	#if DAC_DDR_ENABLE
		.src_addr = (uintptr_t)DAC_DDR_BASEADDR,
	#else
		.src_addr = (uintptr_t)dac_buffer_dma,
	#endif
		// Address of data destination
		.dest_addr = 0
	};

	struct axi_dma_transfer read_transfer = {
		// Number of bytes to write/read
		.size = ADC_BUFFER_SAMPLES * 2,
		// Transfer done flag
		.transfer_done = 0,
		// Signal transfer mode
		.cyclic = NO,
		// Address of data source
		.src_addr = 0,
		// Address of data destination
		.dest_addr = (uintptr_t)ADC_DDR_BASEADDR
	};
#endif

#define MAX_SIZE (DAC_BUFFER_SAMPLES*4*2)
	int32_t error = 0;
	uint32_t bytes_number = 10;
	uint8_t wr_data[MAX_SIZE] = {0};
	uint32_t bytes_recv = 0;
    uint32_t bytes_send = 0u;
    const uint32_t bytes_chunk = 4096u;
	uint8_t spi_mode = 0u;
	uint32_t spi_addr = 0;
	uint32_t spi_data = 0;
    uint8_t dpd_lutId = 0;

    //for txqec test
	uint32_t chan = 0u;
	uint32_t enableMask = 0u;
	uint32_t txqec_param_mask = 0x0Fu;

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
				switch(wr_data[0])
				{
				case 0x5A:
					adi_adrv9025_SpiByteWrite(devHalInfo, (uint16_t)spi_addr, (uint8_t)spi_data);
					break;
				case 0x5B:
#if 0
					// spi read
					adi_adrv9025_SpiByteRead(devHalInfo, (uint16_t)spi_addr, &spi_rd_data);
#else
					spi_data = 0xa1b2c3e4;
#endif
					// send data
					wr_data[6] = (spi_data >> 3*8) & 0xff;
					wr_data[7] = (spi_data >> 2*8) & 0xff;
					wr_data[8] = (spi_data >> 1*8) & 0xff;
					wr_data[9] = (spi_data >> 0*8) & 0xff;
					no_os_uart_write(uart_desc, wr_data, bytes_number);
					break;

				case 0x5C:
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
						if(tx_is_transfering == 1u)
						{
							/* Stop tranfering the data. */
							axi_dmac_transfer_stop(tx_dmac);

							//Xil_DCacheFlush();

							axi_dac_set_datasel(phy->tx_dac, -1, AXI_DAC_DATA_SEL_DMA);
							/* Reload the waveform */
#if DAC_DDR_ENABLE
							axi_dac_load_custom_data(phy->tx_dac,
													zero_lut_iq,
													NO_OS_ARRAY_SIZE(zero_lut_iq),
													(uintptr_t)DAC_DDR_BASEADDR);
#else
							axi_dac_load_custom_data(phy->tx_dac,
													zero_lut_iq,
													NO_OS_ARRAY_SIZE(zero_lut_iq),
													(uintptr_t)dac_buffer_dma);
#endif
							Xil_DCacheFlush();

							/* Transfer the data. */
							transfer.size = bytes_number*2; //NOTE: play half waveform if size = bytes_number
							axi_dmac_transfer_start(tx_dmac, &transfer);
							axi_dmac_transfer_wait_completion(tx_dmac, 500);

							/* Flush cache data. */
#if DAC_DDR_ENABLE
							Xil_DCacheInvalidateRange((uintptr_t)DAC_DDR_BASEADDR, sizeof(zero_lut_iq));
#else
							Xil_DCacheInvalidateRange((uintptr_t)dac_buffer_dma, sizeof(zero_lut_iq));
#endif
							no_os_mdelay(100);
						}
					}
					no_os_mdelay(10);
					break;

				case 0x5D:
					bytes_number = (wr_data[1] << 2*8) | (wr_data[2] << 1*8) | (wr_data[3] << 0*8);
					if(bytes_number > ADC_BUFFER_SAMPLES * 2)
					{
						// todo: sanity check later
						bytes_number = ADC_BUFFER_SAMPLES * 2;
					}

#if ORX_CAPTURE == 0
					/* Read the data from the ADC DMA. */
					axi_dmac_transfer_start(rx_dmac, &read_transfer);

					/* Wait until transfer finishes */
					status = axi_dmac_transfer_wait_completion(rx_dmac, 1000);
#else
					/* Read the data from the ADC DMA. */
					axi_dmac_transfer_start(rx_os_dmac, &read_transfer);

					/* Wait until transfer finishes */
					status = axi_dmac_transfer_wait_completion(rx_os_dmac, 1000);

#endif

					Xil_DCacheInvalidateRange((uintptr_t)ADC_DDR_BASEADDR, ADC_BUFFER_SAMPLES*2);

					// send ORx via uart
					memcpy(wr_data, (uint8_t*)ADC_DDR_BASEADDR, bytes_number);
					bytes_send = 0u;
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
					break;
#if 0
				case 0x70: // get EnabledTrackingCals
					TALISE_getEnabledTrackingCals(&tal[TALISE_A], &enableMask);
					for(int i = 1; i < 5; i++) {
						wr_data[i + 1] = (enableMask >> (8 * (4 - i))) & 0xff;
					}
					no_os_uart_write(uart_desc, wr_data, bytes_number);
					break;
				case 0x71: // set EnabledTrackingCals
					enableMask = 0u;
					for(int i = 1; i < 5; i++) {
						enableMask |= (wr_data[i + 1] << (8 * (4 - i)));
					}
					TALISE_enableTrackingCals(&tal[TALISE_A], enableMask);
					break;
				case 0x72: // read qec correction gain and phase adj
					txqec_param_mask = 0x0Fu;
					txqec_get_phase_gain_gd(devHalInfo, chan, &txqecOut, txqec_param_mask);
					wr_data[2] = (txqecOut.gain >> 8) & 0xff;
					wr_data[3] = (txqecOut.gain >> 0) & 0xff;

					wr_data[4] = (txqecOut.phase >> 8) & 0xff;
					wr_data[5] = (txqecOut.phase >> 0) & 0xff;

					wr_data[6] = (txqecOut.gd[0] >> 8) & 0xff;
					wr_data[7] = (txqecOut.gd[0] >> 0) & 0xff;

					wr_data[8] = (txqecOut.gd[1] >> 8) & 0xff;
					wr_data[9] = (txqecOut.gd[1] >> 0) & 0xff;
					no_os_uart_write(uart_desc, wr_data, bytes_number);
					break;
				case 0x73: // write qec correction gain and phase adj
					int16_t wr_gain = (wr_data[2] << 8) +  wr_data[3];
					int16_t wr_phase = (wr_data[4] << 8) +  wr_data[5];

					txqec_set_phase_gain_gd(devHalInfo, chan, PARAM_GAIN, wr_gain);
					txqec_set_phase_gain_gd(devHalInfo, chan, PARAM_PHASE, wr_phase);
					break;
#endif
				default:
					/* do nothing */
					printf("Invalid command.\n");
					break;
				}

			}
		}
	}
	no_os_uart_remove(&uart_desc);
}

#endif
