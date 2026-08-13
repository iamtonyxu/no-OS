/******************************************************************************
*
* Copyright (C) 2026 ZHFG, Inc.  All rights reserved.
*
* Description:
* This file contains the declarations of the axi_radio_hw interrupt driver.
*
* The axi_radio_hw core exposes a single interrupt source, spi_mailbox_irq,
* which pulses when the SPI host writes arm_cmd_0 with bit[31] set. In the
* ZC706 block design this signal is routed through sys_concat_intc In0 into
* the PS IRQ_F2P[0] input, which maps to Zynq-7000 GIC SPI interrupt ID 61.
*
******************************************************************************/

#ifndef _AXI_RADIO_HW_IRQ_H_
#define _AXI_RADIO_HW_IRQ_H_

#include <stdint.h>
#include "no_os_irq.h"
#include "axi_radio_hw_driver.h"

/******************************************************************************/
/* Interrupt definition                                                       */
/******************************************************************************/

/*
 * Zynq-7000 GIC mapping for the PL fabric interrupt:
 *   IRQ_F2P[0] -> GIC SPI ID 61
 * The block design connects sys_concat_intc In0 (spi_mailbox_irq) to dout[0],
 * which drives IRQ_F2P[0].
 */
#define AXI_RADIO_HW_IRQ_ID	61U

/******************************************************************************/
/* Device structures                                                          */
/******************************************************************************/

/**
 * @struct axi_radio_hw_irq
 * @brief axi_radio_hw interrupt instance.
 */
struct axi_radio_hw_irq {
	/** no-OS IRQ controller descriptor (GIC). */
	struct no_os_irq_ctrl_desc	*irq_ctrl;
	/** Callback descriptor registered against the GIC. */
	struct no_os_callback_desc	callback_desc;
	/** Pointer to the axi_radio_hw register device, used by the ISR. */
	struct axi_radio_hw		*radio_hw;
	/** GIC SPI interrupt id (default AXI_RADIO_HW_IRQ_ID). */
	uint32_t			irq_id;
	/** Optional user callback invoked from the ISR. */
	void				(*mailbox_cb)(void *ctx);
	/** User context passed to mailbox_cb. */
	void				*mailbox_ctx;
	/** Last arm_cmd_0 payload latched by the ISR (bit[31] already cleared). */
	volatile uint32_t		arm_cmd_0;
	/** Number of mailbox interrupts handled since init. */
	volatile uint32_t		irq_count;
};

/**
 * @struct axi_radio_hw_irq_init_param
 * @brief axi_radio_hw interrupt initialization parameters.
 */
struct axi_radio_hw_irq_init_param {
	/** axi_radio_hw register device. */
	struct axi_radio_hw	*radio_hw;
	/** GIC SPI interrupt id; 0 selects the default (IRQ_F2P[0] = 61). */
	uint32_t		irq_id;
	/** Optional user callback invoked from the ISR. */
	void			(*mailbox_cb)(void *ctx);
	/** User context passed to mailbox_cb. */
	void			*mailbox_ctx;
};

/******************************************************************************/
/* Driver API                                                                 */
/******************************************************************************/

/* Initialize the axi_radio_hw interrupt instance and enable the interrupt. */
int32_t axi_radio_hw_irq_init(struct axi_radio_hw_irq **device,
			      const struct axi_radio_hw_irq_init_param *init);
/* Disable the interrupt and free the instance. */
int32_t axi_radio_hw_irq_remove(struct axi_radio_hw_irq *device);

/* Enable the mailbox interrupt. */
int32_t axi_radio_hw_irq_enable(struct axi_radio_hw_irq *device);
/* Disable the mailbox interrupt. */
int32_t axi_radio_hw_irq_disable(struct axi_radio_hw_irq *device);

#endif /* _AXI_RADIO_HW_IRQ_H_ */
