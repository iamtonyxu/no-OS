#ifndef __SPI_H__
#define __SPI_H__

#include "talise_hal.h"

uint8_t spi_write(void *devHalInfo, uint16_t addr, uint8_t data);
uint8_t spi_read(void *devHalInfo, uint16_t addr, uint8_t *readdata);
void spi_read_mod_write(void *devHalInfo, uint16_t addr, uint8_t mask, uint8_t val);


#endif // __SPI_H__
