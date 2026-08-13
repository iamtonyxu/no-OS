/******************************************************************************
*
* Copyright (C) 2026 ZHFG, Inc.  All rights reserved.
*
* Description:
* This file contains the declarations of the axi_radio_hw driver.
* The axi_radio_hw register map is defined in \axi_radio_hw\pcore_registers.v.
*
******************************************************************************/

#ifndef _AXI_RADIO_HW_DRIVER_H_
#define _AXI_RADIO_HW_DRIVER_H_

#include <stdint.h>
#include "xparameters.h"

/******************************************************************************/
/* Register map (byte offsets) -- defined in pcore_registers.v                */
/******************************************************************************/

/* Read / Write registers */
#define AXI_RADIO_HW_REG_SRR           0x0000U  /* Software reset register   */
#define AXI_RADIO_HW_REG_SPICR         0x0004U  /* SPI control register      */
#define AXI_RADIO_HW_REG_SCRATCH_PAD   0x001CU  /* Scratch pad register      */
#define AXI_RADIO_HW_REG_ARM_CMD_0     0x0030U  /* ARM command 0             */
#define AXI_RADIO_HW_REG_ARM_CMD_1     0x0034U  /* ARM command 1             */
#define AXI_RADIO_HW_REG_ARM_CMD_2     0x0038U  /* ARM command 2             */
#define AXI_RADIO_HW_REG_ARM_CMD_3     0x003CU  /* ARM command 3             */
#define AXI_RADIO_HW_REG_ARM_CMD_4     0x0040U  /* ARM command 4             */
#define AXI_RADIO_HW_REG_ARM_CMD_5     0x0044U  /* ARM command 5             */
#define AXI_RADIO_HW_REG_ARM_CMD_6     0x0048U  /* ARM command 6             */
#define AXI_RADIO_HW_REG_ARM_CMD_7     0x004CU  /* ARM command 7             */

/* Read-only registers */
#define AXI_RADIO_HW_REG_SPISR         0x0008U  /* SPI status register       */
#define AXI_RADIO_HW_REG_DEVICE_CONFIG 0x000CU  /* Device configuration      */
#define AXI_RADIO_HW_REG_CHIP_TYPE     0x0010U  /* Chip type                 */
#define AXI_RADIO_HW_REG_PRODUCT_ID    0x0014U  /* Product id                */
#define AXI_RADIO_HW_REG_CHIP_GRADE    0x0018U  /* Chip grade                */
#define AXI_RADIO_HW_REG_VENDOR_ID     0x0020U  /* Vendor id                 */

/*
 * ARM status registers.
 * Read-only for the SPI host (up_if_1), read/write for the PS (up_if_2).
 */
#define AXI_RADIO_HW_REG_ARM_STATUS_0  0x0050U  /* ARM status 0              */
#define AXI_RADIO_HW_REG_ARM_STATUS_1  0x0054U  /* ARM status 1              */
#define AXI_RADIO_HW_REG_ARM_STATUS_2  0x0058U  /* ARM status 2              */
#define AXI_RADIO_HW_REG_ARM_STATUS_3  0x005CU  /* ARM status 3              */
#define AXI_RADIO_HW_REG_ARM_STATUS_4  0x0060U  /* ARM status 4              */
#define AXI_RADIO_HW_REG_ARM_STATUS_5  0x0064U  /* ARM status 5              */
#define AXI_RADIO_HW_REG_ARM_STATUS_6  0x0068U  /* ARM status 6              */
#define AXI_RADIO_HW_REG_ARM_STATUS_7  0x006CU  /* ARM status 7              */

/******************************************************************************/
/* Register reset values (defined in pcore_registers.v)                       */
/******************************************************************************/

#define AXI_RADIO_HW_SRR_RESET            0x00000000U
#define AXI_RADIO_HW_SPICR_RESET          0x00000180U
#define AXI_RADIO_HW_SPISR_RESET          0x000000A5U
#define AXI_RADIO_HW_DEVICE_CONFIG_RESET  0x00001234U
#define AXI_RADIO_HW_CHIP_TYPE_RESET      0x00000001U
#define AXI_RADIO_HW_PRODUCT_ID_RESET     0x00000001U
#define AXI_RADIO_HW_CHIP_GRADE_RESET     0x0000AAAAU
#define AXI_RADIO_HW_SCRATCH_PAD_RESET    0x0000FFFFU
#define AXI_RADIO_HW_VENDOR_ID_RESET      0x0000ABCDU
#define AXI_RADIO_HW_ARM_CMD_RESET        0x00000000U
#define AXI_RADIO_HW_ARM_STATUS_RESET     0x00000000U

/******************************************************************************/
/* Device structures                                                          */
/******************************************************************************/

/**
 * @struct axi_radio_hw
 * @brief axi_radio_hw device instance.
 */
struct axi_radio_hw {
	uint32_t base;
};

/**
 * @struct axi_radio_hw_init_param
 * @brief axi_radio_hw initialization parameters.
 */
struct axi_radio_hw_init_param {
	uint32_t base;
};

/******************************************************************************/
/* Driver API                                                                 */
/******************************************************************************/

/* Initialize the axi_radio_hw instance. */
int32_t axi_radio_hw_init(struct axi_radio_hw **device,
			  const struct axi_radio_hw_init_param *init);
/* Free the axi_radio_hw instance. */
int32_t axi_radio_hw_remove(struct axi_radio_hw *device);

/* Read a register at the given byte offset. */
int32_t axi_radio_hw_read_reg(struct axi_radio_hw *device, uint32_t offset,
			      uint32_t *data);
/* Write a register at the given byte offset. */
int32_t axi_radio_hw_write_reg(struct axi_radio_hw *device, uint32_t offset,
			       uint32_t data);

/* Verify read-only register values and read/write register operation. */
int32_t axi_radio_hw_self_test(struct axi_radio_hw *device);

#endif /* _AXI_RADIO_HW_DRIVER_H_ */
