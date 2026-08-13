/******************************************************************************
*
* Copyright (C) 2026 ZHFG, Inc.  All rights reserved.
*
******************************************************************************/

/*
 * axi_radio_hw example for the ZC706 board.
 *
 * - Initializes the axi_radio_hw register device.
 * - Initializes the SPI mailbox interrupt (spi_mailbox_irq -> IRQ_F2P[0]).
 * - Runs the register self-test.
 * - Spins forever, reporting each mailbox command from the ISR callback.
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include "platform.h"
#include "xil_printf.h"
#include "axi_radio_hw_driver.h"
#include "axi_radio_hw_irq.h"

/* axi_radio_hw register device instance. */
static struct axi_radio_hw *radio_hw_dev;

/* axi_radio_hw mailbox interrupt instance. */
static struct axi_radio_hw_irq *radio_hw_irq;

/*
 * SPI mailbox interrupt callback.
 *
 * Called from IRQ context, so it only prints the latched command payload.
 * xil_printf is used instead of the (non-reentrant) standard printf.
 */
static void on_mailbox(void *ctx)
{
	(void)ctx;

	xil_printf("mailbox cmd = 0x%08X\n\r", radio_hw_irq->arm_cmd_0);
}

int main(void)
{
	struct axi_radio_hw_init_param radio_hw_init = {
		.base = XPAR_AXI_RADIO_HW_0_BASEADDR,
	};
	struct axi_radio_hw_irq_init_param radio_hw_irq_init = {
		.irq_id = 0,		/* 0 selects the default (IRQ_F2P[0] = 61) */
		.mailbox_cb = on_mailbox,
	};
	int32_t status;

	init_platform();

	xil_printf("zc706 axi_radio_hw example\n\r");

	/* Initialize the axi_radio_hw register device. */
	status = axi_radio_hw_init(&radio_hw_dev, &radio_hw_init);
	if (status) {
		xil_printf("axi_radio_hw_init failed (%d)\n\r", status);
		return -1;
	}

	/*
	 * Initialize the mailbox interrupt. radio_hw must be assigned only after
	 * axi_radio_hw_init() has returned the device pointer.
	 */
	radio_hw_irq_init.radio_hw = radio_hw_dev;
	status = axi_radio_hw_irq_init(&radio_hw_irq, &radio_hw_irq_init);
	if (status) {
		xil_printf("axi_radio_hw_irq_init failed (%d)\n\r", status);
		return -1;
	}

	/* Verify read-only register values and read/write register operation. */
	xil_printf("axi_radio_hw self-test\n\r");
	status = axi_radio_hw_self_test(radio_hw_dev);
	if (status) {
		xil_printf("axi_radio_hw_self_test failed (%d)\n\r", status);
		return -1;
	}

	/* Spin forever; each mailbox command is reported from the ISR. */
	while (1) {
		/* Wait for the SPI mailbox interrupt. */
	}

	cleanup_platform();

	return 0;
}
