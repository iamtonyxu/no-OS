#ifndef _SPI_H_
#define _SPI_H_

#include "no_os_spi.h"
#include "no_os_delay.h"

/* ErrorStatus is defined for ECR8660 SDK */
typedef enum
{
  SUCCESS = 0,
  ERROR = !SUCCESS
} ErrorStatus;

typedef enum
{
	NO_ERROR = 0,
	PHY_INIT_FAIL = 1,
	SPI_INIT_FAIL = 2,
	PAGE_TEST_REG_FAIL = 3,
	ECR8660_INIT_FAIL = 4,
	SPI_RW_EXTERNAL_FAIL = 5,
	DOWNLOAD_BIN_FAIL = 6,

} ErrorCode;

uint8_t spi_hook(struct no_os_spi_desc *spi_desc);
uint64_t spi_readwrite(uint32_t write_data1, uint32_t write_data2, uint32_t len);
void Delayms(uint32_t msecs);


#endif
