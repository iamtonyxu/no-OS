/**
 * \file adrv9009/src/app/headless.c
 *
 * \brief Contains example code for user integration with their application
 *
 * Copyright 2015-2017 Analog Devices Inc.
 * Released under the AD9378-AD9379 API license, for more information see the "LICENSE.txt" file in this zip file.
 *
 */

/****< Insert User Includes Here >***/

#include <stdio.h>
#include "adi_hal.h"
#include "no_os_spi.h"
#include "no_os_error.h"
#include "no_os_delay.h"
#include "no_os_uart.h"
#include "parameters.h"
#include "no_os_util.h"
#include "axi_dac_core.h"
#include "axi_adc_core.h"
#include "axi_dmac.h"
#ifndef ALTERA_PLATFORM
#include "xil_cache.h"
#include "xilinx_gpio.h"
#include "xilinx_spi.h"
#include "xilinx_uart.h"
#else
#include "altera_spi.h"
#include "altera_gpio.h"
#endif
#include "talise.h"
#include "talise_config.h"
#include "app_config.h"
#include "app_clocking.h"
#include "app_jesd.h"
#include "app_transceiver.h"
#include "app_talise.h"
#include "ad9528.h"
#include "app_dpd.h"
#include "sdcard_access.h"
char file_name[32] = "TEST0.BIN";

extern uint32_t lutEntries[DPD_LUT_DEPTH*DPD_LUT_MAX];
uint32_t rdLut[DPD_LUT_DEPTH*DPD_LUT_MAX] = {0};

#define ADRV9009_DEVICE 0
#define DAC_BUFFER_SAMPLES MAX_FILE_SIZE/4
#define ADC_BUFFER_SAMPLES MAX_FILE_SIZE/4

#ifdef DMA_EXAMPLE

struct no_os_gpio_desc *gpio_plddrbypass;
struct no_os_gpio_init_param gpio_init_plddrbypass;
uint8_t tx_is_transfering = 0u;

#endif

#ifdef IIO_SUPPORT

#include "iio.h"
#include "iio_app.h"
#include "iio_axi_adc.h"
#include "iio_axi_dac.h"
#ifndef ALTERA_PLATFORM
#include "xilinx_uart.h"
#else
#include "altera_uart.h"
#endif

int32_t start_iiod(struct axi_dmac *rx_dmac, struct axi_dmac *tx_dmac,
		   struct axi_adc *rx_adc, struct axi_dac *tx_dac)
{
	struct iio_axi_adc_init_param	iio_axi_adc_init_par;
	struct iio_axi_dac_init_param	iio_axi_dac_init_par;
	struct iio_app_init_param app_init_param = { 0 };
	struct iio_axi_adc_desc		*iio_axi_adc_desc;
	struct iio_axi_dac_desc		*iio_axi_dac_desc;
	struct iio_device		*adc_dev_desc;
	struct iio_device		*dac_dev_desc;
	struct iio_app_desc *app;
	int32_t				status;
	struct xil_uart_init_param platform_uart_init_par = {
#ifdef XPAR_XUARTLITE_NUM_INSTANCES
		.type = UART_PL,
#else
		.type = UART_PS,
		.irq_id = UART_IRQ_ID
#endif
	};

	struct no_os_uart_init_param iio_uart_ip = {
		.device_id = UART_DEVICE_ID,
		.irq_id = UART_IRQ_ID,
		.baud_rate = UART_BAUDRATE,
		.size = NO_OS_UART_CS_8,
		.parity = NO_OS_UART_PAR_NO,
		.stop = NO_OS_UART_STOP_1_BIT,
		.extra = &platform_uart_init_par,
		.platform_ops = &xil_uart_ops
	};

#ifndef ADRV9008_2
	iio_axi_adc_init_par = (struct iio_axi_adc_init_param) {
		.rx_adc = rx_adc,
		.rx_dmac = rx_dmac,
#ifndef PLATFORM_MB
		.dcache_invalidate_range = (void (*)(uint32_t,
						     uint32_t))Xil_DCacheInvalidateRange
#endif
	};
#endif
#ifndef ADRV9008_1
	iio_axi_dac_init_par = (struct iio_axi_dac_init_param) {
		.tx_dac = tx_dac,
		.tx_dmac = tx_dmac,
#ifndef PLATFORM_MB
		.dcache_flush_range = (void (*)(uint32_t, uint32_t))Xil_DCacheFlushRange,
#endif
	};
#endif

#ifndef ADRV9008_2
	status = iio_axi_adc_init(&iio_axi_adc_desc, &iio_axi_adc_init_par);
	if (status < 0)
		return status;
#endif

#ifndef ADRV9008_1
	status = iio_axi_dac_init(&iio_axi_dac_desc, &iio_axi_dac_init_par);
	if(status < 0)
		return status;
#endif

#ifndef ADRV9008_2
	iio_axi_adc_get_dev_descriptor(iio_axi_adc_desc, &adc_dev_desc);
#endif
#ifndef ADRV9008_1
	iio_axi_dac_get_dev_descriptor(iio_axi_dac_desc, &dac_dev_desc);
#endif

#ifndef ADRV9008_2
	struct iio_data_buffer read_buff = {
		.buff = (void *)ADC_DDR_BASEADDR,
		.size = 0xFFFFFFFF,
	};
#endif
#ifndef ADRV9008_1
	struct iio_data_buffer write_buff = {
		.buff = (void *)DAC_DDR_BASEADDR,
		.size = 0xFFFFFFFF,
	};
#endif

	struct iio_app_device devices[] = {
#ifndef ADRV9008_2
		IIO_APP_DEVICE("axi_adc", iio_axi_adc_desc, adc_dev_desc,
			       &read_buff, NULL, NULL),
#endif
#ifndef ADRV9008_1
		IIO_APP_DEVICE("axi_dac", iio_axi_dac_desc, dac_dev_desc,
			       NULL, &write_buff, NULL)
#endif
	};

	app_init_param.devices = devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(devices);
	app_init_param.uart_init_params = iio_uart_ip;

	status = iio_app_init(&app, app_init_param);
	if (status)
		return status;

	return iio_app_run(app);
}

#endif // IIO_SUPPORT

void parse_spi_command(void *devHalInfo);

	struct axi_adc_init rx_adc_init = {
		"rx_adc",
		RX_CORE_BASEADDR,
		TALISE_NUM_CHANNELS
	};
	struct axi_adc *rx_adc;

	struct axi_adc_init rx_os_adc_init = {
		"rx_os_adc",
		RX_OS_CORE_BASEADDR,
		TALISE_NUM_CHANNELS / 2
	};
	struct axi_adc *rx_os_adc;

	struct axi_dac_init tx_dac_init = {
		"tx_dac",
		TX_CORE_BASEADDR,
		TALISE_NUM_CHANNELS,
		NULL,
		3
	};
	struct axi_dac *tx_dac;

	struct axi_dmac_init rx_dmac_init = {
		"rx_dmac",
		RX_DMA_BASEADDR,
		IRQ_DISABLED
	};
	struct axi_dmac *rx_dmac;

	struct axi_dmac_init rx_os_dmac_init = {
		"rx_os_dmac",
		RX_OS_DMA_BASEADDR,
		IRQ_DISABLED
	};
	struct axi_dmac *rx_os_dmac;

	struct axi_dmac_init tx_dmac_init = {
		"tx_dmac",
		TX_DMA_BASEADDR,
		IRQ_DISABLED
	};
	struct axi_dmac *tx_dmac;

#ifndef ALTERA_PLATFORM
	struct xil_spi_init_param hal_spi_param = {
#ifdef PLATFORM_MB
		.type = SPI_PL,
#else
		.type = SPI_PS,
#endif
		.flags = SPI_CS_DECODE
	};
	struct xil_gpio_init_param hal_gpio_param = {
#ifdef PLATFORM_MB
		.type = GPIO_PL,
#else
		.type = GPIO_PS,
#endif
		.device_id = GPIO_DEVICE_ID
	};
#else
	struct altera_spi_init_param hal_spi_param = {
		.type = NIOS_II_SPI,
		.base_address = SPI_BASEADDR
	};
	struct altera_gpio_init_param hal_gpio_param = {
		.type = NIOS_II_GPIO,
		.device_id = 0,
		.base_address = GPIO_BASEADDR
	};

	hal.extra_gpio = &hal_gpio_param;
#endif

	/* Transfer 16384 samples from MEM to ADC */
	struct axi_dma_transfer transfer_tx = {
		// Number of bytes to write/read
		.size = sizeof(sine_lut_iq),
		// Transfer done flag
		.transfer_done = 0,
		// Signal transfer mode
		.cyclic = CYCLIC,

		// Address of data source
		.src_addr = (uintptr_t)DAC_DDR_BASEADDR,
		// Address of data destination
		.dest_addr = 0
	};

	/* Transfer 16384 samples from ADC to MEM */
	struct axi_dma_transfer transfer_rx = {
		// Number of bytes to write/read, need to be updated later
#ifndef ADRV9008_2
		.size = ADC_BUFFER_SAMPLES * TALISE_NUM_CHANNELS,
#else
		.size = ADC_BUFFER_SAMPLES * TALISE_NUM_CHANNELS / 2,
#endif
		// Transfer done flag
		.transfer_done = 0,
		// Signal transfer mode
		.cyclic = NO,
		// Address of data source
		.src_addr = 0,
		// Address of data destination
		.dest_addr = (uintptr_t)(ADC_DDR_BASEADDR)
	};

/**********************************************************/
/**********************************************************/
/********** Talise Data Structure Initializations ********/
/**********************************************************/
/**********************************************************/

int main(void)
{
	adiHalErr_t err;
	int status;

	// compute the lane rate from profile settings
	// lane_rate = input_rate * M * 20 / L
	// 		where L and M are explained in taliseJesd204bFramerConfig_t comments
	uint32_t rx_lane_rate_khz = talInit.rx.rxProfile.rxOutputRate_kHz *
				    talInit.jesd204Settings.framerA.M * (20 /
						    no_os_hweight8(talInit.jesd204Settings.framerA.serializerLanesEnabled));
	uint32_t rx_div40_rate_hz = rx_lane_rate_khz * (1000 / 40);
	uint32_t tx_lane_rate_khz = talInit.tx.txProfile.txInputRate_kHz *
				    talInit.jesd204Settings.deframerA.M * (20 /
						    no_os_hweight8(talInit.jesd204Settings.deframerA.deserializerLanesEnabled));
	uint32_t tx_div40_rate_hz = tx_lane_rate_khz * (1000 / 40);
	uint32_t rx_os_lane_rate_khz = talInit.obsRx.orxProfile.orxOutputRate_kHz *
				       talInit.jesd204Settings.framerB.M * (20 /
						       no_os_hweight8(talInit.jesd204Settings.framerB.serializerLanesEnabled));
	uint32_t rx_os_div40_rate_hz = rx_os_lane_rate_khz * (1000 / 40);

	// compute the local multiframe clock
	// serializer:   lmfc_rate = (lane_rate * 100) / (K * F)
	// deserializer: lmfc_rate = (lane_rate * 100) / (K * 2 * M / L)
	// 		where K, F, L, M are explained in taliseJesd204bFramerConfig_t comments
	uint32_t rx_lmfc_rate = (rx_lane_rate_khz * 100) /
				(talInit.jesd204Settings.framerA.K * talInit.jesd204Settings.framerA.F);
	uint32_t tx_lmfc_rate = (tx_lane_rate_khz * 100) /
				(talInit.jesd204Settings.deframerA.K * 2 * talInit.jesd204Settings.deframerA.M /
				 no_os_hweight8(talInit.jesd204Settings.deframerA.deserializerLanesEnabled));
	uint32_t rx_os_lmfc_rate = (rx_os_lane_rate_khz * 100) /
				   (talInit.jesd204Settings.framerB.K * talInit.jesd204Settings.framerB.F);

	uint32_t lmfc_rate = no_os_min(rx_lmfc_rate, rx_os_lmfc_rate);
	lmfc_rate = no_os_min(tx_lmfc_rate, lmfc_rate);

	int t;
	struct adi_hal hal[TALISE_DEVICE_ID_MAX];
	taliseDevice_t tal[TALISE_DEVICE_ID_MAX];
	for (t = TALISE_A; t < TALISE_DEVICE_ID_MAX; t++) {
		hal[t].extra_gpio= &hal_gpio_param;
		hal[t].extra_spi = &hal_spi_param;
		tal[t].devHalInfo = (void *) &hal[t];
	}
	hal[TALISE_A].gpio_adrv_resetb_num = TRX_A_RESETB_GPIO;
	hal[TALISE_A].spi_adrv_csn = ADRV_CS;
#if defined(ZU11EG) || defined(FMCOMMS8_ZCU102)
	hal[TALISE_B].gpio_adrv_resetb_num = TRX_B_RESETB_GPIO;
	hal[TALISE_B].spi_adrv_csn = ADRV_B_CS;
#endif

#ifndef ALTERA_PLATFORM
	/* Enable the instruction cache. */
	Xil_ICacheEnable();
	/* Enable the data cache. */
	Xil_DCacheEnable();
#endif

	printf("Hello\n");

	/**********************************************************/
	/**********************************************************/
	/************ Talise Initialization Sequence *************/
	/**********************************************************/
	/**********************************************************/

	err = clocking_init(rx_div40_rate_hz,
			    tx_div40_rate_hz,
			    rx_os_div40_rate_hz,
			    talInit.clocks.deviceClock_kHz,
			    lmfc_rate);
	if (err != ADIHAL_OK)
		goto error_0;

	err = jesd_init(rx_div40_rate_hz,
			tx_div40_rate_hz,
			rx_os_div40_rate_hz);
	if (err != ADIHAL_OK)
		goto error_1;

	err = fpga_xcvr_init(rx_lane_rate_khz,
			     tx_lane_rate_khz,
			     rx_os_lane_rate_khz,
			     talInit.clocks.deviceClock_kHz);
	if (err != ADIHAL_OK)
		goto error_2;

	for (t = TALISE_A; t < TALISE_DEVICE_ID_MAX; t++) {
		err = talise_setup(&tal[t], &talInit);
		if (err != ADIHAL_OK)
			goto error_3;
	}
#if defined(ZU11EG) || defined(FMCOMMS8_ZCU102)
	printf("Performing multi-chip synchronization...\n");
	for(int i=0; i < 12; i++) {
		for (t = TALISE_A; t < TALISE_DEVICE_ID_MAX; t++) {
			err = talise_multi_chip_sync(&tal[t], i);
			if (err != ADIHAL_OK)
				goto error_3;
		}
	}
#endif
	ADIHAL_sysrefReq(tal[TALISE_A].devHalInfo, SYSREF_CONT_ON);

	jesd_rx_watchdog();

	/* Print JESD status */
	jesd_status();

	/* Initialize the DAC core */
#ifndef ADRV9008_1
	status = axi_dac_init(&tx_dac, &tx_dac_init);
	if (status) {
		printf("axi_dac_init() failed with status %d\n", status);
		goto error_3;
	}
#endif

	/* Initialize the ADC core */
#ifndef ADRV9008_2
	status = axi_adc_init(&rx_adc, &rx_adc_init);
	if (status) {
		printf("axi_adc_init() failed with status %d\n", status);
		goto error_3;
	}
#else


	status = axi_adc_init(&rx_os_adc, &rx_os_adc_init);
	if (status) {
		printf("OBS axi_adc_init() failed with status %d\n", status);
		goto error_3;
	}
#endif

#if 0
////////////////////////////////
	/* check data sel is adc */
	for(int ch = 0; ch < rx_os_adc_init.num_channels; ch++)
	{
		uint8_t data_sel = axi_adc_get_datasel(rx_os_adc, ch);
		printf("data_sel before adc_init for ch-%d = %d\n", ch, data_sel);
	}
////////////////////////////////
#endif

#ifndef ADRV9008_1
	status = axi_dmac_init(&tx_dmac, &tx_dmac_init);
	if (status) {
		printf("axi_dmac_init() tx init error: %d\n", status);
		goto error_3;
	}

#endif
#ifndef ADRV9008_2
	status = axi_dmac_init(&rx_dmac, &rx_dmac_init);
	if (status) {
		printf("axi_dmac_init() rx init error: %d\n", status);
		goto error_3;
	}
#else

	status = axi_dmac_init(&rx_os_dmac, &rx_os_dmac_init);
	if (status) {
		printf("OBS axi_dmac_init() rx init error: %d\n", status);
		goto error_3;
	}
#endif

#ifdef DMA_EXAMPLE
	gpio_init_plddrbypass.extra = &hal_gpio_param;
#ifndef ALTERA_PLATFORM
	gpio_init_plddrbypass.platform_ops = &xil_gpio_ops;
#else
	gpio_init_plddrbypass.platform_ops = &altera_gpio_ops;
#endif
	gpio_init_plddrbypass.number = DAC_FIFO_BYPASS_GPIO;
	status = no_os_gpio_get(&gpio_plddrbypass, &gpio_init_plddrbypass);
	if (status) {
		printf("no_os_gpio_get() failed with status %d", status);
		goto error_3;
	}
	no_os_gpio_direction_output(gpio_plddrbypass, 1);

#ifndef ADRV9008_1
	axi_dac_set_datasel(tx_dac, -1, AXI_DAC_DATA_SEL_DMA);

	axi_dac_load_custom_data(tx_dac, sine_lut_iq,
				 NO_OS_ARRAY_SIZE(sine_lut_iq),
				 (uintptr_t)DAC_DDR_BASEADDR);
#ifndef ALTERA_PLATFORM
	Xil_DCacheFlush();
#endif
#if 0
	axi_dmac_transfer_start(tx_dmac, &transfer_tx);
	Xil_DCacheInvalidateRange((uintptr_t)DAC_DDR_BASEADDR, sizeof(sine_lut_iq));
#endif
	tx_is_transfering = 1u;
	no_os_mdelay(1000);
#endif

#ifndef ADRV9008_2
	transfer_rx.size = ADC_BUFFER_SAMPLES * TALISE_NUM_CHANNELS *
						NO_OS_DIV_ROUND_UP(talInit.jesd204Settings.framerA.Np, 8);

	status = axi_dmac_transfer_start(rx_dmac, &transfer_rx);
	if(status)
		return status;
	printf("Rx ");
	status = axi_dmac_transfer_wait_completion(rx_dmac, 500);
	uint8_t num_chans = rx_adc_init.num_channels;
#else
	transfer_rx.size = ADC_BUFFER_SAMPLES * TALISE_NUM_CHANNELS / 2 *
						NO_OS_DIV_ROUND_UP(talInit.jesd204Settings.framerA.Np, 8);

	status = axi_dmac_transfer_start(rx_os_dmac, &transfer_rx);
	if(status)
		return status;
	printf("Rx obs ");
	status = axi_dmac_transfer_wait_completion(rx_os_dmac, 500);
	uint8_t num_chans = rx_os_adc_init.num_channels;
#endif

	if(status)
		return status;

#ifndef ADRV9008_2
	Xil_DCacheInvalidateRange(ADC_DDR_BASEADDR,
				  ADC_BUFFER_SAMPLES * TALISE_NUM_CHANNELS *
				  NO_OS_DIV_ROUND_UP(talInit.jesd204Settings.framerA.Np, 8));
#else
	Xil_DCacheInvalidateRange(ADC_DDR_BASEADDR,
				  ADC_BUFFER_SAMPLES * TALISE_NUM_CHANNELS / 2 *
				  NO_OS_DIV_ROUND_UP(talInit.jesd204Settings.framerA.Np, 8));
#endif

	printf("DMA_EXAMPLE: address=%#lx samples=%lu channels=%u bits=%u\n",
	       transfer_rx.dest_addr, transfer_rx.size / NO_OS_DIV_ROUND_UP(
		       talInit.jesd204Settings.framerA.Np, 8),
	       num_chans,
	       8 * NO_OS_DIV_ROUND_UP(talInit.jesd204Settings.framerA.Np, 8));
#endif

#if 1
	// dpd test
	status = dpd_luts_access_test();
	uint32_t dpd_ipVersion = dpd_read_ipVersion();
	uint64_t dpd_idMask = dpd_read_idMask();
	uint32_t dpd_scrach_val = dpd_write_scratch_reg(0x12345678);
	uint8_t dpd_out_sel = dpd_write_act_out_sel(DPD_BYPASS);
#endif

#if 1
	for(uint8_t lutId = 0; lutId < 64; lutId++)
	{
		if((dpd_idMask & (1u << lutId)) != 0u)
		{
			int lutsOffset = lutId * DPD_LUT_DEPTH;
			dpd_luts_write(lutId, &lutEntries[lutsOffset]);
			dpd_luts_read(lutId, &rdLut[lutsOffset]);
		}
	}
	if(memcmp(rdLut, lutEntries, sizeof(rdLut)) != 0)
	{
		printf("Warning: rdLut is NOT equal with wrLut!");
	}
	dpd_out_sel = dpd_write_act_out_sel(DPD_ENABLE); //DPD_ENABLE
#endif

	while(1)
	{
		/* parse cmd received via uart */
		parse_spi_command(tal[TALISE_A].devHalInfo);
	}

#ifdef IIO_SUPPORT
	status = start_iiod(rx_dmac, tx_dmac, rx_adc, tx_dac);
	if (status)
		printf("iiod error: %d\n", status);
#endif // IIO_SUPPORT

	for (t = TALISE_A; t < TALISE_DEVICE_ID_MAX; t++) {
		talise_shutdown(&tal[t]);
	}
error_3:
	fpga_xcvr_deinit();
error_2:
	jesd_deinit();
error_1:
	clocking_deinit();
error_0:
	printf("Bye\n");

#ifndef ALTERA_PLATFORM
	/* Disable the instruction cache. */
	Xil_ICacheDisable();
	/* Disable the data cache. */
	Xil_DCacheDisable();
#endif

	return 0;
}

void parse_spi_command(void *devHalInfo)
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
#if ADRV9009_DEVICE
					talSpiWriteByte(devHalInfo, (uint16_t)spi_addr, (uint8_t)spi_data);
#endif
					break;
				case 0x5B:
#if ADRV9009_DEVICE
					// spi read
					spi_data = talSpiReadByte(devHalInfo, (uint16_t)spi_addr);
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
							axi_dac_set_datasel(tx_dac, -1, AXI_DAC_DATA_SEL_DMA);

							/* Reload the waveform */
							axi_dac_load_custom_data(tx_dac,
													zero_lut_iq,
													NO_OS_ARRAY_SIZE(zero_lut_iq),
													(uintptr_t)DAC_DDR_BASEADDR);
							Xil_DCacheFlush();

							/* Transfer the data. */
							transfer_tx.size = bytes_number*2; //NOTE: play half waveform if size = bytes_number
							axi_dmac_transfer_start(tx_dmac, &transfer_tx);

							/* Flush cache data. */
							Xil_DCacheInvalidateRange((uintptr_t)DAC_DDR_BASEADDR, sizeof(zero_lut_iq));
						}
					}
					no_os_mdelay(10);
					break;
				case 0x5D:
					bytes_number = (wr_data[1] << 2*8) | (wr_data[2] << 1*8) | (wr_data[3] << 0*8);
					uint16_t samples_number = bytes_number / 4;
#ifndef ADRV9008_2
					transfer_rx.size = samples_number * TALISE_NUM_CHANNELS *
										NO_OS_DIV_ROUND_UP(talInit.jesd204Settings.framerA.Np, 8);
#else
					transfer_rx.size = samples_number * TALISE_NUM_CHANNELS / 2 *
										NO_OS_DIV_ROUND_UP(talInit.jesd204Settings.framerA.Np, 8);
#endif
					/* Read the data from the ADC DMA. */
					axi_dmac_transfer_start(rx_os_dmac, &transfer_rx);

					/* Wait until transfer finishes */
					int32_t status = axi_dmac_transfer_wait_completion(rx_os_dmac, 500);

					/* Flush cache data. */
#ifndef ADRV9008_2
					Xil_DCacheInvalidateRange((uintptr_t)ADC_DDR_BASEADDR,
								samples_number * TALISE_NUM_CHANNELS *
								NO_OS_DIV_ROUND_UP(talInit.jesd204Settings.framerA.Np, 8));
#else
					Xil_DCacheInvalidateRange((uintptr_t)ADC_DDR_BASEADDR,
								samples_number * TALISE_NUM_CHANNELS / 2 *
								NO_OS_DIV_ROUND_UP(talInit.jesd204Settings.framerA.Np, 8));
#endif
					if(status < 0)
					{
						memset(wr_data, 0, bytes_number);
					}
					else
					{
						memcpy(wr_data, (uint8_t*)ADC_DDR_BASEADDR, bytes_number);
					}

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
				case 0x5E:
					/* Read waveform from SD card and transmit */
					char fileID = wr_data[1] + '0';
					int sd_status = 0;
					file_name[4] = fileID;
					uint32_t file_size = (wr_data[2] << 3*8) |
										(wr_data[3] << 2*8) |
										(wr_data[4] << 1*8) |
										(wr_data[5] << 0*8);

					if(file_size > MAX_FILE_SIZE)
						return;
					sd_status = read_wavefrom_from_sdcard(file_name,
													file_size,
													load_lut_iq);

					// transmit data if sd card read successfully
					if(sd_status == XST_SUCCESS)
					{
						/* Stop tranfering the data. */
						axi_dmac_transfer_stop(tx_dmac);

						/* Reload the waveform */
						axi_dac_load_custom_data(tx_dac,
												load_lut_iq,
												file_size/4,
												(uintptr_t)DAC_DDR_BASEADDR);
						Xil_DCacheFlush();

						/* Transfer the data. */
						transfer_tx.size = file_size;
						axi_dmac_transfer_start(tx_dmac, &transfer_tx);

						/* Flush cache data. */
						Xil_DCacheInvalidateRange((uintptr_t)DAC_DDR_BASEADDR, file_size);
					}
					break;
                case 0x6A:
                    /* dpd register write */
					dpd_register_write((uint8_t)spi_addr, spi_data);
                    break;
                case 0x6B:
                    /* dpd register read */
					spi_data = dpd_register_read((uint8_t)spi_addr);

					// send data
					wr_data[6] = (spi_data >> 3*8) & 0xff;
					wr_data[7] = (spi_data >> 2*8) & 0xff;
					wr_data[8] = (spi_data >> 1*8) & 0xff;
					wr_data[9] = (spi_data >> 0*8) & 0xff;
					no_os_uart_write(uart_desc, wr_data, bytes_number);  
                    break;
                case 0x6C:
                    /* dpd luts write */
					bytes_number = (wr_data[1] << 2*8) | (wr_data[2] << 1*8) | (wr_data[3] << 0*8);
					dpd_lutId = wr_data[4];

                    bytes_recv = no_os_uart_read(uart_desc, wr_data, bytes_number);

                    if((dpd_lutId < DPD_LUT_MAX) && (bytes_number/4 == DPD_LUT_DEPTH))
                    {
                        for(int sample = 0; sample < bytes_number/4; sample++)
                        {
							uint32_t iq = (wr_data[sample*4 + 1] << 0) |
										(wr_data[sample*4 + 0] << 8) |
										(wr_data[sample*4 + 3] << 16) |
										(wr_data[sample*4 + 2] << 24);
							lutEntries[dpd_lutId*DPD_LUT_DEPTH + sample] = iq;
                        }
                        dpd_luts_write(dpd_lutId, &lutEntries[dpd_lutId*DPD_LUT_DEPTH]);
                        no_os_mdelay(10);
                    }
                    else
                    {
                        /* report error */
                    }
                    break;
                case 0x6D:
                    /* dpd luts read */
					bytes_number = (wr_data[1] << 2*8) | (wr_data[2] << 1*8) | (wr_data[3] << 0*8);
                    dpd_lutId = wr_data[4];
                    if((dpd_lutId < DPD_LUT_MAX) && (bytes_number/4 == DPD_LUT_DEPTH))
                    {
                        uint32_t* pLut = (uint32_t*) wr_data;
                        dpd_luts_read(dpd_lutId, pLut);

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
                    }
                    else
                    {
                        /* report error */                        
                    }
                    break;
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
