/******************************************************************************
*
* Copyright (C) 2026 ZHFG, Inc.  All rights reserved.
*
* Description:
* This file contains the implementation of the axi_radio_hw_app driver.
* module axi_radio_hw register map is defined in \axi_radio_hw\pcore_registers.v.
*
*
******************************************************************************/

#include "no_os_error.h"
#include "no_os_alloc.h"
#include "no_os_axi_io.h"
#include "no_os_print_log.h"
#include "axi_radio_hw_driver.h"

/******************************************************************************/
/* Generic register access                                                    */
/******************************************************************************/

/**
 * @brief Read a 32-bit register at the given byte offset.
 * @param device - axi_radio_hw instance.
 * @param offset - Byte offset of the register (see AXI_RADIO_HW_REG_*).
 * @param data   - Pointer where the read value is stored.
 * @return 0 on success, negative error code otherwise.
 */
int32_t axi_radio_hw_read_reg(struct axi_radio_hw *device, uint32_t offset,
			      uint32_t *data)
{
	return no_os_axi_io_read(device->base, offset, data);
}

/**
 * @brief Write a 32-bit register at the given byte offset.
 * @param device - axi_radio_hw instance.
 * @param offset - Byte offset of the register (see AXI_RADIO_HW_REG_*).
 * @param data   - Value to write.
 * @return 0 on success, negative error code otherwise.
 */
int32_t axi_radio_hw_write_reg(struct axi_radio_hw *device, uint32_t offset,
			       uint32_t data)
{
	return no_os_axi_io_write(device->base, offset, data);
}

/******************************************************************************/
/* Init / Remove                                                              */
/******************************************************************************/

/**
 * @brief Initialize the axi_radio_hw instance.
 * @param device - Pointer where the allocated instance is returned.
 * @param init   - Initialization parameters (base address).
 * @return 0 on success, negative error code otherwise.
 */
int32_t axi_radio_hw_init(struct axi_radio_hw **device,
			  const struct axi_radio_hw_init_param *init)
{
	struct axi_radio_hw *dev;

	if (!device || !init)
		return -EINVAL;

	dev = (struct axi_radio_hw *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	dev->base = init->base;

	*device = dev;

	return 0;
}

/**
 * @brief Free the axi_radio_hw instance.
 * @param device - axi_radio_hw instance.
 * @return 0 on success, negative error code otherwise.
 */
int32_t axi_radio_hw_remove(struct axi_radio_hw *device)
{
	if (!device)
		return -EINVAL;

	no_os_free(device);

	return 0;
}

/******************************************************************************/
/* Self-test helpers                                                          */
/******************************************************************************/

/**
 * @brief Verify a read-only register against its expected reset value.
 * @return 0 on success, negative error code otherwise.
 */
static int32_t axi_radio_hw_check_ro(struct axi_radio_hw *device, uint32_t offset,
				     uint32_t expected, const char *name)
{
	uint32_t val;
	int32_t ret;

	ret = axi_radio_hw_read_reg(device, offset, &val);
	if (ret)
		return ret;

	if (val != expected) {
		pr_err("axi_radio_hw: %s mismatch (read 0x%08X, expected 0x%08X)\n",
		       name, val, expected);
		return -EIO;
	}

	pr_info("axi_radio_hw: %s = 0x%08X OK\n", name, val);

	return 0;
}

/**
 * @brief Verify a read/write register: write a pattern, read it back, then
 *        restore the original value.
 * @return 0 on success, negative error code otherwise.
 */
static int32_t axi_radio_hw_check_rw(struct axi_radio_hw *device, uint32_t offset,
				     uint32_t value, uint32_t restore, const char *name)
{
	uint32_t val;
	int32_t ret;

	ret = axi_radio_hw_write_reg(device, offset, value);
	if (ret)
		return ret;

	ret = axi_radio_hw_read_reg(device, offset, &val);
	if (ret)
		return ret;

	if (val != value) {
		pr_err("axi_radio_hw: %s write/read mismatch (read 0x%08X, expected 0x%08X)\n",
		       name, val, value);
		return -EIO;
	}

	/* Restore the original value so the self-test is transparent. */
	ret = axi_radio_hw_write_reg(device, offset, restore);
	if (ret)
		return ret;

	pr_info("axi_radio_hw: %s R/W = 0x%08X OK\n", name, value);

	return 0;
}

/******************************************************************************/
/* Self-test                                                                  */
/******************************************************************************/

/**
 * @brief Run the axi_radio_hw self-test.
 *
 * Steps:
 *   1. Verify the read-only registers against their reset values.
 *   2. Verify SRR / SPICR read their reset values.
 *   3. Verify read/write operation on scratch_pad, arm_cmd_1..7 and
 *      arm_status_0..7.
 *   4. Verify arm_cmd_0 bit[31] is auto-cleared by hardware (the same bit
 *      that triggers the SPI mailbox IRQ).
 *
 * @param device - axi_radio_hw instance.
 * @return 0 on success, negative error code otherwise.
 */
int32_t axi_radio_hw_self_test(struct axi_radio_hw *device)
{
	uint32_t offset;
	uint32_t val;
	int32_t ret;

	if (!device)
		return -EINVAL;

	pr_info("axi_radio_hw: self-test start\n");

	/* 1. Read-only registers. */
	ret = axi_radio_hw_check_ro(device, AXI_RADIO_HW_REG_SPISR,
				    AXI_RADIO_HW_SPISR_RESET, "spisr");
	if (ret)
		return ret;
	ret = axi_radio_hw_check_ro(device, AXI_RADIO_HW_REG_DEVICE_CONFIG,
				    AXI_RADIO_HW_DEVICE_CONFIG_RESET, "device_config");
	if (ret)
		return ret;
	ret = axi_radio_hw_check_ro(device, AXI_RADIO_HW_REG_CHIP_TYPE,
				    AXI_RADIO_HW_CHIP_TYPE_RESET, "chip_type");
	if (ret)
		return ret;
	ret = axi_radio_hw_check_ro(device, AXI_RADIO_HW_REG_PRODUCT_ID,
				    AXI_RADIO_HW_PRODUCT_ID_RESET, "product_id");
	if (ret)
		return ret;
	ret = axi_radio_hw_check_ro(device, AXI_RADIO_HW_REG_CHIP_GRADE,
				    AXI_RADIO_HW_CHIP_GRADE_RESET, "chip_grade");
	if (ret)
		return ret;
	ret = axi_radio_hw_check_ro(device, AXI_RADIO_HW_REG_VENDOR_ID,
				    AXI_RADIO_HW_VENDOR_ID_RESET, "vendor_id");
	if (ret)
		return ret;

	/* 2. SRR / SPICR reset value (read-only verification, no write). */
	ret = axi_radio_hw_check_ro(device, AXI_RADIO_HW_REG_SRR,
				    AXI_RADIO_HW_SRR_RESET, "srr");
	if (ret)
		return ret;
	ret = axi_radio_hw_check_ro(device, AXI_RADIO_HW_REG_SPICR,
				    AXI_RADIO_HW_SPICR_RESET, "spicr");
	if (ret)
		return ret;

	/* 3. Read/write registers. */
	ret = axi_radio_hw_check_rw(device, AXI_RADIO_HW_REG_SCRATCH_PAD,
				    0x5A5A5A5AU, AXI_RADIO_HW_SCRATCH_PAD_RESET,
				    "scratch_pad");
	if (ret)
		return ret;

	for (offset = AXI_RADIO_HW_REG_ARM_CMD_1; offset <= AXI_RADIO_HW_REG_ARM_CMD_7;
	     offset += 4) {
		ret = axi_radio_hw_check_rw(device, offset, 0xA5A5A5A5U,
					    AXI_RADIO_HW_ARM_CMD_RESET, "arm_cmd");
		if (ret)
			return ret;
	}

	for (offset = AXI_RADIO_HW_REG_ARM_STATUS_0; offset <= AXI_RADIO_HW_REG_ARM_STATUS_7;
	     offset += 4) {
		ret = axi_radio_hw_check_rw(device, offset, 0x5A5A5A5AU,
					    AXI_RADIO_HW_ARM_STATUS_RESET, "arm_status");
		if (ret)
			return ret;
	}

	/*
	 * 4. arm_cmd_0 bit[31] is auto-cleared in hardware (it is the SPI
	 *    mailbox IRQ trigger). Writing it also pulses spi_mailbox_irq,
	 *    which is expected here.
	 */
	ret = axi_radio_hw_write_reg(device, AXI_RADIO_HW_REG_ARM_CMD_0, 0xFFFFFFFFU);
	if (ret)
		return ret;
	ret = axi_radio_hw_read_reg(device, AXI_RADIO_HW_REG_ARM_CMD_0, &val);
	if (ret)
		return ret;
	if (val != 0x7FFFFFFFU) {
		pr_err("axi_radio_hw: arm_cmd_0 bit[31] auto-clear failed (read 0x%08X)\n",
		       val);
		return -EIO;
	}
	ret = axi_radio_hw_write_reg(device, AXI_RADIO_HW_REG_ARM_CMD_0,
				     AXI_RADIO_HW_ARM_CMD_RESET);
	if (ret)
		return ret;
	pr_info("axi_radio_hw: arm_cmd_0 bit[31] auto-clear OK\n");

	pr_info("axi_radio_hw: self-test passed\n");

	return 0;
}
