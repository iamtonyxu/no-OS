/**
  ******************************************************************************
  * @file    radio_hw_spi.h
  * @brief   SPI master driver for the ZHFG radio_hw register file.
  *
  * The radio_hw core exposes its internal registers to an external SPI master
  * through a simple 4-wire SPI slave (spi_slave_if.v):
  *   - SPI mode 0 (CPOL = 0, CPHA = 0), MSB first.
  *   - Each transaction is a 64-bit frame:
  *         D[63]    = W/Rb  (1 = write, 0 = read)
  *         D[62:32] = register byte address (bits [15:2] are used)
  *         D[31:0]  = write data / read data
  *   - cs_n must be asserted low for the whole 64-bit frame.
  *
  * From the SPI master, arm_status_0..7 are read-only (they are writable only
  * through the AXI interface of the internal processor).
  ******************************************************************************
  */
#ifndef __RADIO_HW_SPI_H__
#define __RADIO_HW_SPI_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include <stdint.h>

/* ---- Chip select pin (software controlled) ---- */
#define RADIO_HW_CS_PORT        GPIOA
#define RADIO_HW_CS_PIN         GPIO_PIN_4

/* SPI transaction timeout (ms). */
#define RADIO_HW_SPI_TIMEOUT    100U

/* ---- Register map (byte offsets), see pcore_registers.v ---- */

/* Read / Write registers */
#define RADIO_HW_REG_SRR           0x0000U  /* Software reset register   */
#define RADIO_HW_REG_SPICR         0x0004U  /* SPI control register      */
#define RADIO_HW_REG_SCRATCH_PAD   0x001CU  /* Scratch pad register      */
#define RADIO_HW_REG_ARM_CMD_0     0x0030U  /* ARM command 0             */
#define RADIO_HW_REG_ARM_CMD_1     0x0034U  /* ARM command 1             */
#define RADIO_HW_REG_ARM_CMD_2     0x0038U  /* ARM command 2             */
#define RADIO_HW_REG_ARM_CMD_3     0x003CU  /* ARM command 3             */
#define RADIO_HW_REG_ARM_CMD_4     0x0040U  /* ARM command 4             */
#define RADIO_HW_REG_ARM_CMD_5     0x0044U  /* ARM command 5             */
#define RADIO_HW_REG_ARM_CMD_6     0x0048U  /* ARM command 6             */
#define RADIO_HW_REG_ARM_CMD_7     0x004CU  /* ARM command 7             */

/* Read-only registers */
#define RADIO_HW_REG_SPISR         0x0008U  /* SPI status register       */
#define RADIO_HW_REG_DEVICE_CONFIG 0x000CU  /* Device configuration      */
#define RADIO_HW_REG_CHIP_TYPE     0x0010U  /* Chip type                 */
#define RADIO_HW_REG_PRODUCT_ID    0x0014U  /* Product id                */
#define RADIO_HW_REG_CHIP_GRADE    0x0018U  /* Chip grade                */
#define RADIO_HW_REG_VENDOR_ID     0x0020U  /* Vendor id                 */

/* ARM status registers: read-only from the SPI master. */
#define RADIO_HW_REG_ARM_STATUS_0  0x0050U  /* ARM status 0              */
#define RADIO_HW_REG_ARM_STATUS_1  0x0054U  /* ARM status 1              */
#define RADIO_HW_REG_ARM_STATUS_2  0x0058U  /* ARM status 2              */
#define RADIO_HW_REG_ARM_STATUS_3  0x005CU  /* ARM status 3              */
#define RADIO_HW_REG_ARM_STATUS_4  0x0060U  /* ARM status 4              */
#define RADIO_HW_REG_ARM_STATUS_5  0x0064U  /* ARM status 5              */
#define RADIO_HW_REG_ARM_STATUS_6  0x0068U  /* ARM status 6              */
#define RADIO_HW_REG_ARM_STATUS_7  0x006CU  /* ARM status 7              */

/* ---- Register reset values, see pcore_registers.v ---- */
#define RADIO_HW_SRR_RESET            0x00000000U
#define RADIO_HW_SPICR_RESET          0x00000180U
#define RADIO_HW_SPISR_RESET          0x000000A5U
#define RADIO_HW_DEVICE_CONFIG_RESET  0x00001234U
#define RADIO_HW_CHIP_TYPE_RESET      0x00000001U
#define RADIO_HW_PRODUCT_ID_RESET     0x00000001U
#define RADIO_HW_CHIP_GRADE_RESET     0x0000AAAAU
#define RADIO_HW_SCRATCH_PAD_RESET    0x0000FFFFU
#define RADIO_HW_VENDOR_ID_RESET      0x0000ABCDU
#define RADIO_HW_ARM_CMD_RESET        0x00000000U
#define RADIO_HW_ARM_STATUS_RESET     0x00000000U

/* ---- Driver API ---- */

/* Configure the software chip-select pin (idle high). */
void radio_hw_spi_init(void);

/* Read a 32-bit register through the SPI bus. */
HAL_StatusTypeDef radio_hw_spi_read_reg(uint16_t addr, uint32_t *data);

/* Write a 32-bit register through the SPI bus. */
HAL_StatusTypeDef radio_hw_spi_write_reg(uint16_t addr, uint32_t data);

/* Verify a read/write register: write, read back, restore. */
int32_t radio_hw_spi_check_rw(uint16_t addr, uint32_t value,
                                     uint32_t restore, const char *name);

/* Verify read-only register values and read/write register operation.
 * Returns 0 on success, -1 on the first failing check. */
int32_t radio_hw_spi_self_test(void);

#ifdef __cplusplus
}
#endif

#endif /* __RADIO_HW_SPI_H__ */
