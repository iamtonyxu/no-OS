#include "txlol_hw.h"

// ----------------------------------------------------------------------------------------------------
// apply_correction
//
// Function to apply correction values to the hardware LOL offset registers.  Before applying, the
// values are clipped to user defined limits.  If we got to a limit, a return value indicates that
// we reached a saturated value.
// ----------------------------------------------------------------------------------------------------
void txlol_hw_set_correction(void *devHalInfo, iq_int16_t *dc_offset, uint32_t ch_num)
{
	if (ch_num == 0) {
		// Apply I channel correction to hardware
		spi_write(devHalInfo, SPI_IDX_TX1_TXQEC_FILTER_OFFSET_I_MSB, (dc_offset->i >> 8) & 0xff);
		spi_write(devHalInfo, SPI_IDX_TX1_TXQEC_FILTER_OFFSET_I_LSB, (dc_offset->i >> 0) & 0xff);
		// Apply Q channel correction to hardware
		spi_write(devHalInfo, SPI_IDX_TX1_TXQEC_FILTER_OFFSET_Q_MSB, (dc_offset->q >> 8) & 0xff);
		spi_write(devHalInfo, SPI_IDX_TX1_TXQEC_FILTER_OFFSET_Q_LSB, (dc_offset->q >> 0) & 0xff);
	} else {
		// Apply I channel correction to hardware
		spi_write(devHalInfo, SPI_IDX_TX2_TXQEC_FILTER_OFFSET_I_MSB, (dc_offset->i >> 8) & 0xff);
		spi_write(devHalInfo, SPI_IDX_TX2_TXQEC_FILTER_OFFSET_I_LSB, (dc_offset->i >> 0) & 0xff);
		// Apply Q channel correction to hardware
		spi_write(devHalInfo, SPI_IDX_TX2_TXQEC_FILTER_OFFSET_Q_MSB, (dc_offset->q >> 8) & 0xff);
		spi_write(devHalInfo, SPI_IDX_TX2_TXQEC_FILTER_OFFSET_Q_LSB, (dc_offset->q >> 0) & 0xff);
	}
		/* Forces DC correction to be applied */
		spi_write(devHalInfo, SPI_IDX_TX_TXQEC_FILTER_CFG, (BITM_SPI_TXQEC_FILTER_CFG_TXQEC_FILTER_CH1_DC_OFFSET_UPDATE << ch_num));

	return;
}

void txlol_hw_get_correction(void *devHalInfo, iq_int16_t *dc_offset, uint32_t ch_num)
{
	uint8_t tempdata = 0;
	// Read the hardware state of the offset, and put it in a software register which the ISR can
	// use to add offsets when the attenuation changes.
	if (ch_num == 0) {
		spi_read(devHalInfo, SPI_IDX_TX1_TXQEC_FILTER_OFFSET_I_MSB, &tempdata);
		dc_offset->i = tempdata << 8;
		spi_read(devHalInfo, SPI_IDX_TX1_TXQEC_FILTER_OFFSET_I_LSB, &tempdata);
		dc_offset->i = dc_offset->i | tempdata;

		spi_read(devHalInfo, SPI_IDX_TX1_TXQEC_FILTER_OFFSET_Q_MSB, &tempdata);
		dc_offset->q = tempdata << 8;
		spi_read(devHalInfo, SPI_IDX_TX1_TXQEC_FILTER_OFFSET_Q_LSB, &tempdata);
		dc_offset->q = dc_offset->q | tempdata;
	} else {
		spi_read(devHalInfo, SPI_IDX_TX2_TXQEC_FILTER_OFFSET_I_MSB, &tempdata);
		dc_offset->i = tempdata << 8;
		spi_read(devHalInfo, SPI_IDX_TX2_TXQEC_FILTER_OFFSET_I_LSB, &tempdata);
		dc_offset->i = dc_offset->i | tempdata;

		spi_read(devHalInfo, SPI_IDX_TX2_TXQEC_FILTER_OFFSET_Q_MSB, &tempdata);
		dc_offset->q = tempdata << 8;
		spi_read(devHalInfo, SPI_IDX_TX2_TXQEC_FILTER_OFFSET_Q_LSB, &tempdata);
		dc_offset->q = dc_offset->q | tempdata;
	}
	return;
}
