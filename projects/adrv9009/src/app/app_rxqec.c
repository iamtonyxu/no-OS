#include "app_rxqec.h"

static void rxqec_write(uint16_t offset, uint32_t wrdata)
{
	no_os_axi_io_write((uint32_t)RXQEC_BASEADDR, (uint32_t)offset, wrdata);
}

static uint32_t rxqec_read(uint16_t offset)
{
	uint32_t rddata = 0;
	no_os_axi_io_read((uint32_t)RXQEC_BASEADDR, (uint32_t)offset, &rddata);
	return rddata;
}

uint32_t rxqec_read_ipVersion(void)
{
	return rxqec_read(OFFSET_IP_VERSION);
}

void rxqec_write_scratch(uint32_t scratch)
{
	rxqec_write(OFFSET_SCRATCH, scratch);
}

uint32_t rxqec_read_scratch(void)
{
	return rxqec_read(OFFSET_SCRATCH);
}

void rxqec_write_enable(uint8_t enable)
{
	rxqec_write(OFFSET_ENABLE, (uint32_t)enable);
}

uint32_t rxqec_read_enable(void)
{
	return rxqec_read(OFFSET_ENABLE);
}

void rxqec_write_hi(uint16_t hi[IFIR_TAPS_HALF])
{
	for(int ii = 0; ii < IFIR_TAPS_HALF; ii++)
	{
		rxqec_write(OFFSET_HI0 + ii*4, (uint32_t)hi[ii]);
	}
}

uint32_t rxqec_read_hi(uint16_t hi[IFIR_TAPS_HALF])
{
	for(int ii = 0; ii < IFIR_TAPS_HALF; ii++)
	{
		hi[ii] = rxqec_read(OFFSET_HI0 + ii*4);
	}
	return 0u;
}

void rxqec_write_hq(uint16_t hq[QFIR_TAPS_HALF])
{
	for(int ii = 0; ii < QFIR_TAPS_HALF; ii++)
	{
		rxqec_write(OFFSET_HQ0 + ii*4, (uint32_t)hq[ii]);
	}
}

uint32_t rxqec_read_hq(uint16_t hq[QFIR_TAPS_HALF])
{
	for(int ii = 0; ii < QFIR_TAPS_HALF; ii++)
	{
		hq[ii] = rxqec_read(OFFSET_HQ0 + ii*4);
	}
	return 0u;
}
