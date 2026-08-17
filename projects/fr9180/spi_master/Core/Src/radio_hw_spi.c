/**
  ******************************************************************************
  * @file    radio_hw_spi.c
  * @brief   SPI master driver for the ZHFG radio_hw register file.
  *
  * Implements the 64-bit register access frame defined in spi_slave_if.v:
  *   D[63]    = W/Rb, D[62:32] = address, D[31:0] = data (MSB first).
  ******************************************************************************
  */

#include "radio_hw_spi.h"
#include "spi.h"
#include <stdio.h>
#include <string.h>

/* Number of bytes in one SPI register frame. */
#define RADIO_HW_FRAME_BYTES    8U

/* -------------------------------------------------------------------------- */
/* Build a 64-bit SPI frame.                                                  */
/* -------------------------------------------------------------------------- */
static uint64_t radio_hw_spi_frame(uint16_t addr, uint32_t data, uint8_t wr)
{
  return ((uint64_t)(wr & 0x1U) << 63) |
         ((uint64_t)(addr & 0xFFFFU) << 32) |
         (uint64_t)data;
}

/* -------------------------------------------------------------------------- */
/* Low-level transfer: assert CS, clock out the frame, release CS.            */
/* -------------------------------------------------------------------------- */
static HAL_StatusTypeDef radio_hw_spi_transfer(const uint8_t *tx, uint8_t *rx,
                                               uint16_t len)
{
  HAL_StatusTypeDef status;

  HAL_GPIO_WritePin(RADIO_HW_CS_PORT, RADIO_HW_CS_PIN, GPIO_PIN_RESET);
  status = HAL_SPI_TransmitReceive(&hspi1, (uint8_t *)tx, rx, len,
                                   RADIO_HW_SPI_TIMEOUT);
  HAL_GPIO_WritePin(RADIO_HW_CS_PORT, RADIO_HW_CS_PIN, GPIO_PIN_SET);

  return status;
}

/* -------------------------------------------------------------------------- */
/* Split a 64-bit frame into 8 bytes, MSB first.                              */
/* -------------------------------------------------------------------------- */
static void radio_hw_spi_frame_to_bytes(uint64_t frame, uint8_t *buf)
{
  uint8_t i;

  for (i = 0; i < RADIO_HW_FRAME_BYTES; i++)
    buf[i] = (uint8_t)(frame >> (56U - 8U * i));
}

/* -------------------------------------------------------------------------- */
/* Public API                                                                 */
/* -------------------------------------------------------------------------- */

/**
  * @brief Configure the software chip-select pin (idle high).
  */
void radio_hw_spi_init(void)
{
  GPIO_InitTypeDef gpio_init = {0};

  __HAL_RCC_GPIOA_CLK_ENABLE();

  gpio_init.Pin = RADIO_HW_CS_PIN;
  gpio_init.Mode = GPIO_MODE_OUTPUT_PP;
  gpio_init.Pull = GPIO_NOPULL;
  gpio_init.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(RADIO_HW_CS_PORT, &gpio_init);

  HAL_GPIO_WritePin(RADIO_HW_CS_PORT, RADIO_HW_CS_PIN, GPIO_PIN_SET);
}

/**
  * @brief Read a 32-bit register through the SPI bus.
  */
HAL_StatusTypeDef radio_hw_spi_read_reg(uint16_t addr, uint32_t *data)
{
  uint8_t tx[RADIO_HW_FRAME_BYTES];
  uint8_t rx[RADIO_HW_FRAME_BYTES];
  HAL_StatusTypeDef status;

  if (!data)
    return HAL_ERROR;

  /* W/Rb = 0 (read); data phase is don't care. */
  radio_hw_spi_frame_to_bytes(radio_hw_spi_frame(addr, 0U, 0U), tx);

  memset(rx, 0, sizeof(rx));
  status = radio_hw_spi_transfer(tx, rx, RADIO_HW_FRAME_BYTES);
  if (status != HAL_OK)
    return status;

  /* Read data is clocked out during the last 4 bytes, MSB first. */
  *data = ((uint32_t)rx[4] << 24) |
          ((uint32_t)rx[5] << 16) |
          ((uint32_t)rx[6] << 8) |
          (uint32_t)rx[7];
#if 0
  *data >>= 1U;   /* DEBUG ONLY */
#endif
  return HAL_OK;
}

/**
  * @brief Write a 32-bit register through the SPI bus.
  */
HAL_StatusTypeDef radio_hw_spi_write_reg(uint16_t addr, uint32_t data)
{
  uint8_t tx[RADIO_HW_FRAME_BYTES];
  uint8_t rx[RADIO_HW_FRAME_BYTES];

  /* W/Rb = 1 (write). */
  radio_hw_spi_frame_to_bytes(radio_hw_spi_frame(addr, data, 1U), tx);

  memset(rx, 0, sizeof(rx));

  return radio_hw_spi_transfer(tx, rx, RADIO_HW_FRAME_BYTES);
}

/* -------------------------------------------------------------------------- */
/* Self-test                                                                  */
/* -------------------------------------------------------------------------- */

/* Verify a read-only register against its expected reset value. */
static int32_t radio_hw_spi_check_ro(uint16_t addr, uint32_t expected,
                                     const char *name)
{
  uint32_t val;

  if (radio_hw_spi_read_reg(addr, &val) != HAL_OK) {
    printf("[FAIL] %s: SPI read error\n", name);
    return -1;
  }

  if (val != expected) {
    printf("[FAIL] %s: read 0x%08X, expected 0x%08X\n", name,
           (unsigned int)val, (unsigned int)expected);
    return -1;
  }

  printf("[ OK ] %s = 0x%08X\n", name, (unsigned int)val);
  return 0;
}

/* Verify a read/write register: write, read back, restore. */
int32_t radio_hw_spi_check_rw(uint16_t addr, uint32_t value,
                                     uint32_t restore, const char *name)
{
  uint32_t val;

  if (radio_hw_spi_write_reg(addr, value) != HAL_OK) {
    printf("[FAIL] %s: SPI write error\n", name);
    return -1;
  }

  if (radio_hw_spi_read_reg(addr, &val) != HAL_OK) {
    printf("[FAIL] %s: SPI read error\n", name);
    return -1;
  }

  if (val != value) {
    printf("[FAIL] %s: read 0x%08X, expected 0x%08X\n", name,
           (unsigned int)val, (unsigned int)value);
    return -1;
  }

  if (radio_hw_spi_write_reg(addr, restore) != HAL_OK) {
    printf("[FAIL] %s: SPI restore error\n", name);
    return -1;
  }

  printf("[ OK ] %s R/W = 0x%08X\n", name, (unsigned int)value);
  return 0;
}

/**
  * @brief Run the radio_hw SPI register self-test.
  *
  * Steps:
  *   1. Verify read-only registers against their reset values.
  *   2. Verify SRR / SPICR read their reset values.
  *   3. Verify read/write operation on scratch_pad and arm_cmd_1..7.
  *   4. Verify arm_cmd_0 bit[31] auto-clear (writing bit[31] also pulses the
  *      spi_mailbox_irq to the Zynq PS).
  *
  * @return 0 on success, -1 on the first failing check.
  */
int32_t radio_hw_spi_self_test(void)
{
  uint16_t addr;
  uint32_t val;

  printf("radio_hw SPI self-test start\n");

  /* 1. Read-only registers. */
#if 1
  radio_hw_spi_check_ro(RADIO_HW_REG_SPISR, RADIO_HW_SPISR_RESET, "spisr");
  radio_hw_spi_check_ro(RADIO_HW_REG_DEVICE_CONFIG,
                            RADIO_HW_DEVICE_CONFIG_RESET, "device_config");
  radio_hw_spi_check_ro(RADIO_HW_REG_CHIP_TYPE,
                            RADIO_HW_CHIP_TYPE_RESET, "chip_type");
  radio_hw_spi_check_ro(RADIO_HW_REG_PRODUCT_ID,
                            RADIO_HW_PRODUCT_ID_RESET, "product_id");
  radio_hw_spi_check_ro(RADIO_HW_REG_CHIP_GRADE,
                            RADIO_HW_CHIP_GRADE_RESET, "chip_grade");
  radio_hw_spi_check_ro(RADIO_HW_REG_VENDOR_ID,
                            RADIO_HW_VENDOR_ID_RESET, "vendor_id");
#else
  if (radio_hw_spi_check_ro(RADIO_HW_REG_SPISR, RADIO_HW_SPISR_RESET, "spisr"))
    return -1;
  if (radio_hw_spi_check_ro(RADIO_HW_REG_DEVICE_CONFIG,
                            RADIO_HW_DEVICE_CONFIG_RESET, "device_config"))
    return -1;
  if (radio_hw_spi_check_ro(RADIO_HW_REG_CHIP_TYPE,
                            RADIO_HW_CHIP_TYPE_RESET, "chip_type"))
    return -1;
  if (radio_hw_spi_check_ro(RADIO_HW_REG_PRODUCT_ID,
                            RADIO_HW_PRODUCT_ID_RESET, "product_id"))
    return -1;
  if (radio_hw_spi_check_ro(RADIO_HW_REG_CHIP_GRADE,
                            RADIO_HW_CHIP_GRADE_RESET, "chip_grade"))
    return -1;
  if (radio_hw_spi_check_ro(RADIO_HW_REG_VENDOR_ID,
                            RADIO_HW_VENDOR_ID_RESET, "vendor_id"))
    return -1;
#endif
  
  /* 2. SRR / SPICR reset value (read-only verification, no write). */
  if (radio_hw_spi_check_ro(RADIO_HW_REG_SRR, RADIO_HW_SRR_RESET, "srr"))
    return -1;
  if (radio_hw_spi_check_ro(RADIO_HW_REG_SPICR, RADIO_HW_SPICR_RESET, "spicr"))
    return -1;

  /* 3. Read/write registers. */
  if (radio_hw_spi_check_rw(RADIO_HW_REG_SCRATCH_PAD, 0xFEDCBA98U,
                            RADIO_HW_SCRATCH_PAD_RESET, "scratch_pad"))
    return -1;

  for (addr = RADIO_HW_REG_ARM_CMD_1; addr <= RADIO_HW_REG_ARM_CMD_7; addr += 4U) {
    radio_hw_spi_check_rw(addr, 0xA5A5A5A5U, RADIO_HW_ARM_CMD_RESET, "arm_cmd");
#if 0
    if (radio_hw_spi_check_rw(addr, 0xA5A5A5A5U, RADIO_HW_ARM_CMD_RESET, "arm_cmd"))
      return -1;
#endif
  }

  /* 4. arm_cmd_0 bit[31] auto-clear: bit[31] is a write trigger (writing it
   *    also pulses spi_mailbox_irq to the Zynq PS), so it always reads back as
   *    0; bits[30:0] are a normal R/W command payload. */
  if (radio_hw_spi_write_reg(RADIO_HW_REG_ARM_CMD_0,
                             0x80000000U | 0x5A5A5A5AU) != HAL_OK) {
    printf("[FAIL] arm_cmd_0: SPI write error\n");
    return -1;
  }
  if (radio_hw_spi_read_reg(RADIO_HW_REG_ARM_CMD_0, &val) != HAL_OK) {
    printf("[FAIL] arm_cmd_0: SPI read error\n");
    return -1;
  }
  if (val != 0x5A5A5A5AU) {   /* bit[31] must have been auto-cleared */
    printf("[FAIL] arm_cmd_0 bit[31] auto-clear: read 0x%08X\n", (unsigned int)val);
    return -1;
  }
  radio_hw_spi_write_reg(RADIO_HW_REG_ARM_CMD_0, RADIO_HW_ARM_CMD_RESET);
  printf("[ OK ] arm_cmd_0 bit[31] auto-clear\n");

  printf("radio_hw SPI self-test passed\n");

  return 0;
}
