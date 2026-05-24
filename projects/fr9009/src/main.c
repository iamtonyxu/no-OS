//TODO: remove NOUPDATES later
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

/* UART command protocol:
 * C: configure tx source/registers
 * D: download waveform payload to DDR
 * G: trigger capture and print IQ samples
 * W0/W1: load built-in waveform table
 * E: exit loop
 */
#define CMD_CAPTURE			'G'
#define CMD_EXIT			'E'
#define CMD_TX_WAVEFORM		'W'
#define CMD_TX_DOWNLOAD		'D'
#define CMD_TX_CONFIG			'C'
#define CMD_TX_WAVEFORM_491M	'0'
#define CMD_TX_WAVEFORM_245M	'1'

/* global variables */
uint32_t cap_buf[CAP_LENGTH_MAX];

#if FR9009_DEVICE
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
#else
config8_reg_t config8 = {
	.dds_ctrl = 3u, //dds_ctrl[0]=1,enable I; dds_ctrl[1]=1,enable Q Data
	.src_sel = 1u, //0:fpga dds; 1:ddr; 2:const_data
#if JESD_MODE == 0
	.mapper_sel = 0u, //0:JESD L=4,M=4,S=2,DAC=491.52M
#else
	.mapper_sel = 1u, //1:JESD L=2,M=4,S=1,DAC=245.76M
#endif
	.play_ctrl = 1u, //Enable play Tx of module data2fpga
	.capture_en = 1u, //enable capture of module rx_data_capture
	/* 	dds_pinc		frequency
	*	0x14D5555u		5M
	*	0x29AAAAAu		10M (default)
	*	0x3333333u		12.288M
	*	0x5355555u		20M
	*	0x8000000		30.72M
	*/
	.dds_pinc_0 = 0x14D5555u, //default: 10M
	.dds_poff_0 = 0u,
	.dds_pinc_1 = 0u,
	.dds_poff_1 = 0u,
	.const_data_0 = 0x11223344u,
	.const_data_1 = 0x55667788u,
	.play_len_cfg = 0u // [31]=1,enable;[22:0]:data length bytes
};
#endif

/* extern variables */
extern fr9009Device_t brDev[DEVICE_NUMS];
void main_step(void);
static void load_tx_waveform(uint8_t waveform_sel);
static uint32_t uart_recv_u32_le(void);
static int download_tx_waveform_from_uart(void);
static int config_tx_source_from_uart(void);

#endif

void main_step(void)
{
	print("Hello FR9009\n\r");
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

static void load_tx_waveform(uint8_t waveform_sel)
{
	switch (waveform_sel)
	{
	case CMD_TX_WAVEFORM_491M:
		copy_waveform_to_ddr(tone_lut_iq_491M, sizeof(tone_lut_iq_491M));
		printf("tx_waveform=491M\n");
		break;
	case CMD_TX_WAVEFORM_245M:
		copy_waveform_to_ddr(tone_lut_iq_245M, sizeof(tone_lut_iq_245M));
		printf("tx_waveform=245M\n");
		break;
	default:
		printf("Invalid tx waveform command: %c\n", waveform_sel);
		break;
	}
}

static uint32_t uart_recv_u32_le(void)
{
	uint32_t val = 0u;

	for (uint32_t i = 0u; i < 4u; i++)
		val |= ((uint32_t)XUartPs_RecvByte(XPAR_XUARTPS_0_BASEADDR)) << (8u * i);

	return val;
}

static int download_tx_waveform_from_uart(void)
{
	uint32_t word_count = uart_recv_u32_le();
	uint32_t *dest = (uint32_t *)TX_BUF_ADDR;

	if (word_count == 0u || word_count > TX_BUF_LEN)
	{
		printf("waveform_download_err_len=%lu\n", (unsigned long)word_count);
		return -1;
	}

	for (uint32_t i = 0u; i < word_count; i++)
		dest[i] = uart_recv_u32_le();

	printf("waveform_download_ok words=%lu\n", (unsigned long)word_count);
	return 0;
}

static int config_tx_source_from_uart(void)
{
	uint8_t src_sel = (uint8_t)XUartPs_RecvByte(XPAR_XUARTPS_0_BASEADDR);

#if FR9009_DEVICE
	printf("tx_config_err_unsupported\n");
	return -1;
#else
	config8.src_sel = src_sel;

	switch (src_sel)
	{
	case 0u: /* fpga dds */
		config8.dds_pinc_0 = uart_recv_u32_le();
		config8.dds_poff_0 = uart_recv_u32_le();
		config8.dds_pinc_1 = uart_recv_u32_le();
		config8.dds_poff_1 = uart_recv_u32_le();
		break;
	case 1u: /* ddr */
		/* no extra payload */
		break;
	case 2u: /* dc const data */
		config8.const_data_0 = uart_recv_u32_le();
		config8.const_data_1 = uart_recv_u32_le();
		break;
	default:
		printf("tx_config_err_src=%u\n", (unsigned)src_sel);
		return -1;
	}

	if (CONFIG_8_REG_init(&config8) != 0)
	{
		printf("tx_config_err_apply src=%u\n", (unsigned)src_sel);
		return -1;
	}

	printf("tx_config_ok src=%u\n", (unsigned)src_sel);
	return 0;
#endif
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
	uint8_t status = XST_FAILURE;
#if 1
	/* disable DDR DCache */
	Xil_DCacheDisable();

#if JESD_MODE == 0
	/* copy tx waveform into ddr */
	load_tx_waveform(CMD_TX_WAVEFORM_491M);
#else
	load_tx_waveform(CMD_TX_WAVEFORM_245M);
#endif

#if FR9009_DEVICE
	status = axi_fr9009_selfTest();
	if (status == XST_SUCCESS)
	{
		// CONFIG_8_REG for Tx test
		axi_fr9009_config_init(&fr9009_config);
	}
#else
	status = CONFIG_8_REG_Reg_SelfTest((void *)CONFIG_8_REG_BASEADDR);
	if (status == XST_SUCCESS)
	{
		// CONFIG_8_REG for Tx test
		CONFIG_8_REG_init(&config8);
	}
#endif

	/* gpio and fr9009 init */
	gpio_init();
	main_step();
#endif

	/* read capture buffer */
	usleep(100);
	printf("Enter 'C' to configure tx source, 'G' to read capture buffer, 'D' to download tx waveform, 'W0' for 491M tx waveform, 'W1' for 245M tx waveform, 'E' to exit...\n");
	while (1)
	{
#if 0
		char c = getchar();
#else
		char c = (char)XUartPs_RecvByte(XPAR_XUARTPS_0_BASEADDR);
#endif
		if (c == CMD_TX_WAVEFORM || c == 'w')
		{
			uint8_t waveform_sel = (uint8_t)XUartPs_RecvByte(XPAR_XUARTPS_0_BASEADDR);
			load_tx_waveform(waveform_sel);
		}
		else if (c == CMD_TX_CONFIG || c == 'c')
		{
			config_tx_source_from_uart();
		}
		else if (c == CMD_TX_DOWNLOAD || c == 'd')
		{
			download_tx_waveform_from_uart();
		}
		else if (c == CMD_CAPTURE)
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
		else if (c == CMD_EXIT)
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

