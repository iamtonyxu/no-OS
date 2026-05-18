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
	.src_sel = 1u, // 0: fpga dds; 1: ddr; 2: const_data
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
int main()
{
	shell_init();
	while (1)
	{
		if (XUartPs_IsReceiveData(XPAR_XUARTPS_0_BASEADDR)) {
			char c = (char)XUartPs_RecvByte(XPAR_XUARTPS_0_BASEADDR);
			shell(c);
		}
	}

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
		// CONFIG_8_REG for Tx test
		axi_fr9009_config_init(&fr9009_config);
	}

	/* gpio and fr9009 init */
	gpio_init();
	main_step();
#endif

	/* read capture buffer */
	usleep(100);
	printf("Enter 'G' to read capture buffer...\n");
	while (1)
	{
		char c = getchar();
		if (c == 'G')
		{
			trigger_capture();
			printf("cap_buffer=\n");
			read_capture(cap_buf, CAP_LENGTH_MAX);

			for (int i = 0; i < CAP_LENGTH_READ; i++)
			{
				int j = i * 4;
				int16_t rx1_i = (int16_t)(((cap_buf[j] >> 8) & 0xff) |
							      ((cap_buf[j] & 0xff) << 8));
				int16_t rx1_q = (int16_t)(((cap_buf[i] >> 24) & 0xff) |
							      ((cap_buf[j] >> 8) & 0xff00));

				int16_t rx2_i = (int16_t)(((cap_buf[i + 1] >> 8) & 0xff) |
							      ((cap_buf[j + 1] & 0xff) << 8));
				int16_t rx2_q = (int16_t)(((cap_buf[i + 1] >> 24) & 0xff) |
							      ((cap_buf[j + 1] >> 8) & 0xff00));

				printf("%d %d\n", rx1_i, rx1_q);
				printf("%d %d\n", rx2_i, rx2_q);
			}
		}
		else if (c == 'E')
		{
			printf("Exit...\n");
			break;
		}
		else
		{
			// do nothing
			printf("Invalid command!\n");
		}
	}

	while (1)
		;

	return 0;
}
#endif

