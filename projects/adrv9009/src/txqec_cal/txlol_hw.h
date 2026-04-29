#ifndef __TXLOL_HW_H__
#define __TXLOL_HW_H__

#include <stdint.h>
#include <stdbool.h>
#include "spi_addr_def.h"
#include "spi.h"

#define BITM_SPI_TXQEC_FILTER_CFG_TXQEC_FILTER_CH1_DC_OFFSET_UPDATE (0x01U)    /*  Update DC offset (self clear) */

typedef struct iq_int16_t {
  int16_t i;                            /* I component of IQ pair */
  int16_t q;                            /* Q component of IQ pair */
} iq_int16_t;

void txlol_hw_set_correction(void *devHalInfo, iq_int16_t *dc_offset, uint32_t ch_num);
void txlol_hw_get_correction(void *devHalInfo, iq_int16_t *dc_offset, uint32_t ch_num);

#endif
