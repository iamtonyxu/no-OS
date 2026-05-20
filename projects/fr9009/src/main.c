/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#define NOUPDATES 0

#if NOUPDATES
#include <stdio.h>
#include "ad9528_app.h"
#include "headless.h"
#include "xil_io.h"
#include "xil_printf.h"
#include <parameters.h>
#else
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "ad9528_app.h"
#include "axi_fr9009_config.h"
#include "fr9009_tx.h"
#include "headless.h"
#include "nr_micro_shell.h"
#include "rx_capture.h"
#include "sleep.h"
#include "tx_waveform.h"
#include "xil_printf.h"
#include "xil_io.h"
//#include "xilinx_uart.h"
//#include "no_os_uart.h"
#include "xuartps_hw.h"
#include <parameters.h>



/* JESD Mode
 * 0: JESD L=4, M=4, S=2, DAC=491.52M
 * 1: JESD L=2, M=4, S=1, DAC=245.76M
 * */
#define JESD_MODE 0

/* UART Config as default */
#if 0
#define UART_DEVICE_ID          XPAR_XUARTPS_0_DEVICE_ID
#define UART_BAUDRATE           115200
#define UART_IRQ_ID             XPAR_XUARTPS_1_INTR
#endif

/* global variables */
uint32_t cap_buf[CAP_LENGTH_MAX];

fr9009_config_t fr9009_config = {
	.src_sel = 0u, // 0: fpga dds; 1: ddr; 2: const_data
#if JESD_MODE == 0
	.mapper_sel = 0u, // 0: JESD L=4, M=4, S=2, DAC=491.52M
#else
	.mapper_sel = 1u, // 1: JESD L=2, M=4, S=1, DAC=245.76M
#endif
	.ddr_play_ctrl = 1u, // Enable play Tx of module data2fpga
	.ddr_play_len = 0u, // [31]=1 enable; [22:0] data length bytes，default 0 means 16k bytes (16k samples) as configured in data2fpga.v
	.const_data_0 = 0x11223344u,
	.const_data_1 = 0x55667788u,
	.dds_sync = 1u, // dds_sync[0]=0 as default; 1 triggers alignment, then self-clears
	.tone_1_scale = 0x4000u, // 1/2 full scale: 0x4000
	.tone_1_freq_word = 0x10bu, // round(tone_freq/clk_freq)*2^16, 1MHz: 0x10b
	.tone_2_scale = 0x2000u, // 1/4 full scale: 0x2000
	.tone_2_freq_word = 0xa6bu, // round(tone_freq/clk_freq)*2^16, 10MHz: 0xa6b
	.rx_cap_config = 1u // Enable capture of module rx_data_capture
};

/* extern variables */
extern fr9009Device_t brDev[DEVICE_NUMS];
void main_step(void);


#define UART_BASE_ADDR XPAR_XUARTPS_0_BASEADDR
#define WF_MAGIC_0 'F'
#define WF_MAGIC_1 '0'
#define WF_MAGIC_2 '9'
#define WF_RATE_491M 0u
#define WF_RATE_245M 1u

/*
 * Waveform download frame over UART (little-endian):
 *   Host sends: 'W' + 'F''0''9' + rateSel(1B) + sampleCount(2B)
 *               + payload(sampleCount * 4B) + checksum(4B)
 *   payload word format: [31:16] = Q(int16), [15:0] = I(int16)
 */

// Function declarations
static int uart_read_byte_timeout(uint8_t *out, uint32_t timeout_cycles);
static int uart_read_exact(uint8_t *buf, uint32_t len, uint32_t timeout_cycles);
static void dump_capture_buffer(void);
static void handle_src_sel_update(void);
static void handle_waveform_download(void);

#endif

void main_step(void)
{
	print("Hello World\n\r");
	ad9528_device_init();
	ad9528_cfg();
	Xil_Out32((RX_JESD_BASEADDR + 0x20), talInit.jesd204Settings.framerA.F - 1); // F: 4
	Xil_Out32((TX_JESD_BASEADDR + 0x20), (TX_INRATE_IS_REF_MULT == 4 ? 2 : 4) - 1); // F: 2
	Xil_Out32((RX_JESD_BASEADDR + 0x28), talInit.jesd204Settings.framerA.serializerLanesEnabled); // L: 2, lane in use
	Xil_Out32((TX_JESD_BASEADDR + 0x28), TX_INRATE_IS_REF_MULT == 4 ? 0xF : 0x9); // L: 4, lane in use
	gpio_direction_output(gpio_jesd_tx_reset, 1);
	gpio_direction_output(gpio_jesd_tx_reset, 0);
	gpio_direction_output(gpio_jesd_rx_reset, 1);
	gpio_direction_output(gpio_jesd_rx_reset, 0);
	rf_initialize();
}

#if NOUPDATES
int main()
{
	gpio_init();
	main_step();
	while (1)
		;

	return 0;
}
#else
#if NOUPDATES
int main()
{
	gpio_init();
	main_step();
	while (1)
		;

	return 0;
}
#else
int main()
{
	uint8_t status = XST_FAILURE;
#if 1
	/* disable DDR DCache */
	Xil_DCacheDisable();

#if JESD_MODE == 0
	/* copy tx waveform into ddr */
	copy_waveform_to_ddr(tone_lut_iq_491M, sizeof(tone_lut_iq_491M));
#else
	copy_waveform_to_ddr(tone_lut_iq_245M, sizeof(tone_lut_iq_245M));
#endif

	status = axi_fr9009_selfTest();
	if (status == XST_SUCCESS)
	{
		// fr9009_config initialization, including setting default values for the configuration structure etc.
		axi_fr9009_config_init(&fr9009_config);
	}

	/* gpio and fr9009 init */
	gpio_init();
	main_step();
#endif

	usleep(100);
	printf("Commands: 'G' capture, 'S'+srcSel configure TX source, frame 'WF09' download waveform.\n");

	while (1)
	{
		/* Heartbeat: tell host every 1s that board is alive and ready. */
		const char ready_msg[] = "READY\n";
		for (unsigned i = 0; i < sizeof(ready_msg) - 1; ++i)
			XUartPs_SendByte(UART_BASE_ADDR, ready_msg[i]);
		mdelay(1000);

		/* Command parser:
		 *   'G' -> capture and dump ADC samples
		 *   'S' -> receive one src_sel byte and update TX source select
		 *   'W' -> receive waveform frame and update TX LUT+DDR
		 *   'E' -> exit command loop
		 */
		if (XUartPs_IsReceiveData(UART_BASE_ADDR)) {
			char c = (char)XUartPs_RecvByte(UART_BASE_ADDR);
			if (c == 'G') {
				dump_capture_buffer();
			} else if (c == 'S') {
				handle_src_sel_update();
			} else if (c == 'W') {
				handle_waveform_download();
			} else if (c == 'E') {
				printf("Exit...\n");
				break;
			} else {
				printf("Invalid command!\n");
			}
		}
	}

	while (1)
		;

	return 0;
}
#endif

// ---- Subfunction definitions moved below main ----

static int uart_read_byte_timeout(uint8_t *out, uint32_t timeout_cycles)
{
	/* Poll RX FIFO until byte arrives or timeout. */
	uint32_t i;
	for (i = 0; i < timeout_cycles; i++) {
		if (XUartPs_IsReceiveData(UART_BASE_ADDR)) {
			*out = XUartPs_RecvByte(UART_BASE_ADDR);
			return 0;
		}
	}
	return -1;
}

static int uart_read_exact(uint8_t *buf, uint32_t len, uint32_t timeout_cycles)
{
	/* Read exactly len bytes (blocking with timeout per byte). */
	uint32_t i;
	for (i = 0; i < len; i++) {
		if (uart_read_byte_timeout(&buf[i], timeout_cycles) != 0)
			return -1;
	}
	return 0;
}

static void dump_capture_buffer(void)
{
	/* Trigger FPGA capture and print parsed I/Q pairs for MATLAB parser. */
	trigger_capture();
	printf("cap_buffer=\n");
	read_capture(cap_buf, CAP_LENGTH_MAX);
	for (int i = 0; i < CAP_LENGTH_READ; i++) {
		int j = i * 4;
		/* Word format from capture path: [31:16]=Q, [15:0]=I */
		int16_t rx1_i = (int16_t)(cap_buf[j] & 0xffffu);
		int16_t rx1_q = (int16_t)((cap_buf[j] >> 16) & 0xffffu);
		int16_t rx2_i = (int16_t)(cap_buf[j + 1] & 0xffffu);
		int16_t rx2_q = (int16_t)((cap_buf[j + 1] >> 16) & 0xffffu);
		printf("%d %d\n", rx1_i, rx1_q);
		printf("%d %d\n", rx2_i, rx2_q);
	}
}

static void handle_src_sel_update(void)
{
	uint8_t src_sel;
	int status;

	if (uart_read_exact(&src_sel, 1u, 5000000u) != 0) {
		printf("SRC:ERR:TIMEOUT\n");
		return;
	}

	if (src_sel > 2u) {
		printf("SRC:ERR:VALUE\n");
		return;
	}

	status = axi_fr9009_set_src_sel(src_sel);
	if (status != XST_SUCCESS) {
		printf("SRC:ERR:WRITE\n");
		return;
	}

	fr9009_config.src_sel = src_sel;
	printf("SRC:OK src_sel=%u\n", src_sel);
}

static void handle_waveform_download(void)
{
	/*
	 * Receive and validate one waveform frame, then:
	 * 1) update selected LUT buffer (491M or 245M)
	 * 2) zero-fill tail when sample_count < TX_BUF_LEN
	 * 3) copy whole LUT to DDR TX buffer
	 */
	uint8_t hdr[6];
	uint8_t raw_word[4];
	uint8_t raw_checksum[4];
	uint16_t sample_count;
	uint32_t rx_checksum = 0u;
	uint32_t calc_checksum = 0u;
	uint32_t *target_lut = NULL;
	if (uart_read_exact(hdr, sizeof(hdr), 5000000u) != 0) {
		printf("WF:ERR:TIMEOUT_HDR\n");
		return;
	}
	if ((hdr[0] != WF_MAGIC_0) || (hdr[1] != WF_MAGIC_1) || (hdr[2] != WF_MAGIC_2)) {
		printf("WF:ERR:MAGIC\n");
		return;
	}
	if (hdr[3] == WF_RATE_491M) {
		target_lut = tone_lut_iq_491M;
	} else if (hdr[3] == WF_RATE_245M) {
		target_lut = tone_lut_iq_245M;
	} else {
		printf("WF:ERR:RATE\n");
		return;
	}
	sample_count = (uint16_t)hdr[4] | ((uint16_t)hdr[5] << 8);
	/* sample_count is number of uint32 IQ words, not bytes. */
	if ((sample_count == 0u) || (sample_count > TX_BUF_LEN)) {
		printf("WF:ERR:LEN\n");
		return;
	}
	for (uint32_t i = 0; i < sample_count; i++) {
		uint32_t w;
		if (uart_read_exact(raw_word, sizeof(raw_word), 5000000u) != 0) {
			printf("WF:ERR:TIMEOUT_PAYLOAD\n");
			return;
		}
		w = ((uint32_t)raw_word[0]) |
		    ((uint32_t)raw_word[1] << 8) |
		    ((uint32_t)raw_word[2] << 16) |
		    ((uint32_t)raw_word[3] << 24);
		target_lut[i] = w;
		calc_checksum += w;
	}
	for (uint32_t i = sample_count; i < TX_BUF_LEN; i++)
		target_lut[i] = 0u;
	if (uart_read_exact(raw_checksum, sizeof(raw_checksum), 5000000u) != 0) {
		printf("WF:ERR:TIMEOUT_CKS\n");
		return;
	}
	rx_checksum = ((uint32_t)raw_checksum[0]) |
		      ((uint32_t)raw_checksum[1] << 8) |
		      ((uint32_t)raw_checksum[2] << 16) |
		      ((uint32_t)raw_checksum[3] << 24);
	if (rx_checksum != calc_checksum) {
		printf("WF:ERR:CKS\n");
		return;
	}
	/* Push full LUT to DDR so stale tail data is never transmitted. */
	copy_waveform_to_ddr(target_lut, TX_BUF_LEN * sizeof(uint32_t));
	printf("WF:OK rate=%u samples=%u checksum=0x%08lx\n",
	       hdr[3], sample_count, (unsigned long)calc_checksum);
}
#endif

