#include "spi.h"

uint8_t spi_write(void *devHalInfo, uint16_t addr, uint8_t data)
{
	uint8_t ret = 0u;
    // dummy implementation for writing a byte via SPI
    if (devHalInfo == NULL) {
        ret = 1u;
    }
    ret = talSpiWriteByte(devHalInfo, addr, data);
    return ret; // success
}

uint8_t spi_read(void *devHalInfo, uint16_t addr, uint8_t *readdata)
{
	uint8_t ret = 0u;
    // dummy implementation for reading a byte via SPI
    if (devHalInfo == NULL || readdata == NULL) {
        ret = 1u;
    }

    ret = talSpiReadByte(devHalInfo, addr, readdata);
    return ret; // success
}

void spi_read_mod_write(void *devHalInfo, uint16_t addr, uint8_t mask, uint8_t val)
{
    uint8_t tmp;
    spi_read(devHalInfo, addr, &tmp);
    tmp = (tmp & ~mask) | (val & mask);
    spi_write(devHalInfo, addr, tmp);
}
