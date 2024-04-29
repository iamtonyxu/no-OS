/***************************************************************************//**
 *   @file   adis.h
 *   @brief  Implementation of adis.h
 *   @author RBolboac (ramona.bolboaca@analog.com)
 *******************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/

#ifndef __ADIS_H__
#define __ADIS_H__

#ifdef TEST
#define STATIC
#else
#define STATIC static
#endif

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "no_os_spi.h"
#include "no_os_util.h"
#include <errno.h>
#include <stdlib.h>
#include <stdbool.h>

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define ADIS_SYNC_DEFAULT	0
#define ADIS_SYNC_DIRECT	1
#define ADIS_SYNC_SCALED	2
#define ADIS_SYNC_OUTPUT	3
#define ADIS_SYNC_PULSE		5

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @brief Supported device ids
 */
enum adis_device_id {
	ADIS16465_1,
	ADIS16465_2,
	ADIS16465_3,
	ADIS16467_1,
	ADIS16467_2,
	ADIS16467_3,
	ADIS16470,
	ADIS16475_1,
	ADIS16475_2,
	ADIS16475_3,
	ADIS16477_1,
	ADIS16477_2,
	ADIS16477_3,
	ADIS16500,
	ADIS16501,
	ADIS16505_1,
	ADIS16505_2,
	ADIS16505_3,
	ADIS16507_1,
	ADIS16507_2,
	ADIS16507_3,
	ADIS16575_2,
	ADIS16575_3,
	ADIS16576_2,
	ADIS16576_3,
	ADIS16577_2,
	ADIS16577_3,
};

/**
 * @brief Supported channels
 */
enum adis_chan_type {
	ADIS_ACCL_CHAN,
	ADIS_GYRO_CHAN,
	ADIS_TEMP_CHAN,
	ADIS_DELTAANGL_CHAN,
	ADIS_DELTAVEL_CHAN,
};

/** @struct adis_diag_flags
 *  @brief Bitfield struct which maps on the diagnosis register
 */
struct adis_diag_flags {
	/** Sensor initialization failure. */
	uint8_t snsr_init_failure	:1;
	/** Data path overrun bit. */
	uint8_t data_path_overrun	:1;
	/** Flash memory update failure. */
	uint8_t fls_mem_update_failure	:1;
	/** SPI communication error. */
	uint8_t spi_comm_err		:1;
	/** Standby mode. */
	uint8_t standby_mode		:1;
	/** Sensor failure. */
	uint8_t snsr_failure		:1;
	/** Memory failure. */
	uint8_t mem_failure		:1;
	/** Clock error. */
	uint8_t clk_err			:1;
	/** Gyroscope 1 failure. */
	uint8_t gyro1_failure		:1;
	/** Gyroscope 2 failure. */
	uint8_t gyro2_failure		:1;
	/** Accelerometer failure. */
	uint8_t accl_failure		:1;
	/** X-Axis gyroscope failure. */
	uint8_t x_axis_gyro_failure	:1;
	/** Y-Axis gyroscope failure. */
	uint8_t y_axis_gyro_failure	:1;
	/** Z-Axis gyroscope failure. */
	uint8_t z_axis_gyro_failure	:1;
	/** X-Axis accelerometer failure. */
	uint8_t x_axis_accl_failure	:1;
	/** Y-Axis accelerometer failure. */
	uint8_t y_axis_accl_failure	:1;
	/** Z-Axis accelerometer failure. */
	uint8_t z_axis_accl_failure	:1;
	/** ADuC microcontroller fault. */
	uint8_t aduc_mcu_fault		:1;
	/** Checksum error.  */
	uint8_t checksum_err		:1;
	/** Flash memory write count exceeded. */
	uint8_t fls_mem_wr_cnt_exceed	:1;
};

/** @struct adis_scale_fractional
 *  @brief ADIS fractional scale format structure; scale = dividend/divisor
 */
struct adis_scale_fractional {
	/** Scale dividend. */
	uint32_t dividend;
	/** Scale divisor. */
	uint32_t divisor;
};

/** @struct adis_scale_fractional_log2
 *  @brief ADIS fractional log2 scale format structure; scale = dividend/2^power
 */
struct adis_scale_fractional_log2 {
	/** Scale dividend. */
	uint32_t dividend;
	/** Scale 2's power. */
	uint32_t power;
};


/** @struct adis_burst_data
 *  @brief ADIS burst data structure
 */
struct adis_burst_data {
	uint16_t temp_lsb;
	uint16_t temp_msb;
	uint16_t data_cntr_lsb;
	uint16_t data_cntr_msb;
	uint16_t x_gyro_lsb;
	uint16_t x_gyro_msb;
	uint16_t y_gyro_lsb;
	uint16_t y_gyro_msb;
	uint16_t z_gyro_lsb;
	uint16_t z_gyro_msb;
	uint16_t x_accel_lsb;
	uint16_t x_accel_msb;
	uint16_t y_accel_lsb;
	uint16_t y_accel_msb;
	uint16_t z_accel_lsb;
	uint16_t z_accel_msb;
};

/** @struct adis_dev
 *  @brief ADIS device descriptor structure
 */
struct adis_dev {
	/** SPI descriptor used for SPI communication. */
	struct no_os_spi_desc		*spi_desc;
	/** GPIO descriptor used to handle the reset pin. */
	struct no_os_gpio_desc		*gpio_reset;
	/** Specific chip information. */
	const struct adis_chip_info  	*info;
	/** Current diagnosis flags values. */
	struct adis_diag_flags 		diag_flags;
	/** Current device id, specified by the user */
	enum adis_device_id		dev_id;
	/** Current page to be accessed in register map. */
	uint32_t			current_page;
	/** Transmit buffer used in SPI transactions. */
	uint8_t				tx[10];
	/** Receive buffer used in SPI transactions. */
	uint8_t				rx[4];
	/** Internal clock frequency in Hertz. */
	uint32_t 			int_clk;
	/** External clock frequency in Hertz. */
	uint32_t 			ext_clk;
	/** Set to true if device fifo is enabled. */
	bool				fifo_enabled;
	/** Set to true if device burst32 is enabled. */
	bool				burst32;
	/** Burst data selection: 0 for accel/gyro data; 1 for delta angle/ delta velocity data. */
	uint8_t				burst_sel;
};

/** @struct adis_init_param
 *  @brief ADIS device initialization parameters
 */
struct adis_init_param {
	/* Chip specific information. */
	const struct adis_chip_info *info;
	/** SPI initialization parameters. */
	struct no_os_spi_init_param 	*spi_init;
	/** GPIO initialization parameter for reset pin. */
	struct no_os_gpio_init_param	*gpio_reset;
	/** External clock frequency in Hertz to be configured at initialization
	 *  phase.
	 */
	uint32_t 			ext_clk;
	/** Desired synchronization mode to be configured at initialization
	 *  phase.
	 */
	uint32_t			sync_mode;
	/** Device id, specified by the user  */
	enum adis_device_id		dev_id;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/*! Initialize adis device. */
int adis_init(struct adis_dev **adis, const struct adis_init_param *ip);
/*! Remove adis device. */
void adis_remove(struct adis_dev *adis);
/*! Device initial setup. */
int adis_initial_startup(struct adis_dev *adis);

/*! Read N bytes from register. */
int adis_read_reg(struct adis_dev *adis,  uint32_t reg, uint32_t *val,
		  uint32_t size);
/*! Write N bytes to register. */
int adis_write_reg(struct adis_dev *adis, uint32_t reg, uint32_t value,
		   uint32_t size);
/*! Update the desired bits of reg in accordance with mask and val. */
int adis_update_bits_base(struct adis_dev *adis, uint32_t reg,
			  const uint32_t mask, const uint32_t val, uint8_t size);

/*! Read diag status register and update device diag flags. */
int adis_read_diag_stat(struct adis_dev *adis,
			struct adis_diag_flags *diag_flags);

/*! Diagnosis: read sensor initialization failure flag value. */
int adis_read_diag_snsr_init_failure(struct adis_dev *adis,
				     uint32_t *snsr_init_failure);
/*! Diagnosis: read data path overrun flag value. */
int adis_read_diag_data_path_overrun(struct adis_dev *adis,
				     uint32_t *data_path_overrun_err);
/*! Diagnosis: read flash memory update error flag value. */
int adis_read_diag_fls_mem_update_failure(struct adis_dev *adis,
		uint32_t *fls_mem_update_failure);
/*! Diagnosis: read spi communication error flag value. */
int adis_read_diag_spi_comm_err(struct adis_dev *adis, uint32_t *spi_comm_err);
/*! Diagnosis: read standby mode flag value. */
int adis_read_diag_standby_mode(struct adis_dev *adis,
				uint32_t *standby_mode);
/*! Diagnosis: read sensor self test error flag value. */
int adis_read_diag_snsr_failure(struct adis_dev *adis, uint32_t *snsr_failure);
/*! Diagnosis: read flash memory test error flag value. */
int adis_read_diag_mem_failure(struct adis_dev *adis, uint32_t *mem_failure);
/*! Diagnosis: read clock error flag value. */
int adis_read_diag_clk_err(struct adis_dev *adis, uint32_t *clk_err);
/*! Diagnosis: read gyroscope1 self test error flag value. */
int adis_read_diag_gyro1_failure(struct adis_dev *adis,
				 uint32_t *gyro1_failure);
/*! Diagnosis: read gyroscope2 self test error flag value. */
int adis_read_diag_gyro2_failure(struct adis_dev *adis,
				 uint32_t *gyro2_failure);
/*! Diagnosis: read accelerometer self test error flag value. */
int adis_read_diag_accl_failure(struct adis_dev *adis, uint32_t *accl_failure);
/*! Diagnosis: read X-Axis Gyroscope failure flag value. */
int adis_read_diag_x_axis_gyro_failure(struct adis_dev *adis,
				       uint32_t *x_axis_gyro_failure);
/*! Diagnosis: read Y-Axis Gyroscope failure flag value. */
int adis_read_diag_y_axis_gyro_failure(struct adis_dev *adis,
				       uint32_t *y_axis_gyro_failure);
/*! Diagnosis: read Z-Axis Gyroscope failure flag value. */
int adis_read_diag_z_axis_gyro_failure(struct adis_dev *adis,
				       uint32_t *z_axis_gyro_failure);
/*! Diagnosis: read X-Axis Accelerometer failure flag value. */
int adis_read_diag_x_axis_accl_failure(struct adis_dev *adis,
				       uint32_t *x_axis_accl_failure);
/*! Diagnosis: read Y-Axis Accelerometer failure flag value. */
int adis_read_diag_y_axis_accl_failure(struct adis_dev *adis,
				       uint32_t *y_axis_accl_failure);
/*! Diagnosis: read Z-Axis Accelerometer failure flag value. */
int adis_read_diag_z_axis_accl_failure(struct adis_dev *adis,
				       uint32_t *z_axis_accl_failure);
/*! Diagnosis: read ADuC microcontroller fault flag value. */
int adis_read_diag_aduc_mcu_fault(struct adis_dev *adis,
				  uint32_t *aduc_mcu_fault);
/*! Diagnosis: read checksum error flag value. */
void adis_read_diag_checksum_err(struct adis_dev *adis, uint32_t *checksum_err);
/*! Diagnosis: read flash memory write counts exceeded flag value. */
void adis_read_diag_fls_mem_wr_cnt_exceed(struct adis_dev *adis,
		uint32_t *fls_mem_wr_cnt_exceed);

/*! Gyroscope data: read raw gyroscope data on x axis. */
int adis_read_x_gyro(struct adis_dev *adis, int32_t *x_gyro);
/*! Gyroscope data: read raw gyroscope data on y axis. */
int adis_read_y_gyro(struct adis_dev *adis, int32_t *y_gyro);
/*! Gyroscope data: read raw gyroscope data on z axis. */
int adis_read_z_gyro(struct adis_dev *adis, int32_t *z_gyro);

/*! Acceleration data: read raw acceleration data on x axis. */
int adis_read_x_accl(struct adis_dev *adis, int32_t *x_accl);
/*! Acceleration data: read raw acceleration data on y axis. */
int adis_read_y_accl(struct adis_dev *adis, int32_t *y_accl);
/*! Acceleration data: read raw acceleration data on z axis. */
int adis_read_z_accl(struct adis_dev *adis, int32_t *z_accl);

/*! Temperature data: read raw temperature data. */
int adis_read_temp_out(struct adis_dev *adis, int32_t *temp_out);

/*! Synchronization data: read raw time stamp data. */
int adis_read_time_stamp(struct adis_dev *adis, uint32_t *time_stamp);
/*! Synchronization data: read raw data counter data. */
int adis_read_data_cntr(struct adis_dev *adis, uint32_t *data_cntr);

/*! Delta angle data: read raw delta angle data on x axis. */
int adis_read_x_deltang(struct adis_dev *adis, int32_t *x_deltang);
/*! Delta angle data: read raw delta angle data on y axis. */
int adis_read_y_deltang(struct adis_dev *adis, int32_t *y_deltang);
/*! Delta angle data: read raw delta angle data on z axis. */
int adis_read_z_deltang(struct adis_dev *adis, int32_t *z_deltang);

/*! Delta velocity data: read raw delta velocity data on x axis. */
int adis_read_x_deltvel(struct adis_dev *adis, int32_t *x_deltvel);
/*! Delta velocity data: read raw delta velocity data on y axis. */
int adis_read_y_deltvel(struct adis_dev *adis, int32_t *y_deltvel);
/*! Delta velocity data: read raw delta velocity data on z axis. */
int adis_read_z_deltvel(struct adis_dev *adis, int32_t *z_deltvel);

/*! Read output FIFO sample count. */
int adis_read_fifo_cnt(struct adis_dev *adis, uint32_t *fifo_cnt);
/*! Read current sample SPI transaction checksum. */
int adis_read_spi_chksum(struct adis_dev *adis, uint32_t *checksum);

/*! Gyroscope calibration offset: read raw gyroscope offset correction on
 *  x axis.
 */
int adis_read_xg_bias(struct adis_dev *adis, int32_t *xg_bias);
/*! Gyroscope calibration offset: write raw gyroscope offset correction on
 *  x axis.
 */
int adis_write_xg_bias(struct adis_dev *adis, int32_t xg_bias);
/*! Gyroscope calibration offset: read raw gyroscope offset correction on
 *  y axis.
 */
int adis_read_yg_bias(struct adis_dev *adis, int32_t *yg_bias);
/*! Gyroscope calibration offset: write raw gyroscope offset correction on
 *  y axis.
 */
int adis_write_yg_bias(struct adis_dev *adis, int32_t yg_bias);
/*! Gyroscope calibration offset: read raw gyroscope offset correction on
 *  z axis.
 */
int adis_read_zg_bias(struct adis_dev *adis, int32_t *zg_bias);
/*! Gyroscope calibration offset: write raw gyroscope offset correction on
 *  z axis.
 */
int adis_write_zg_bias(struct adis_dev *adis, int32_t zg_bias);

/*! Acceleration calibration offset: read raw acceleration offset correction on
 *  x axis.
 */
int adis_read_xa_bias(struct adis_dev *adis, int32_t *xa_bias);
/*! Acceleration calibration offset: write raw acceleration offset correction on
 *  x axis.
 */
int adis_write_xa_bias(struct adis_dev *adis, int32_t xa_bias);
/*! Acceleration calibration offset: read raw acceleration offset correction on
 *  y axis.
 */
int adis_read_ya_bias(struct adis_dev *adis, int32_t *ya_bias);
/*! Acceleration calibration offset: write raw acceleration offset correction on
 *  y axis.
 */
int adis_write_ya_bias(struct adis_dev *adis, int32_t ya_bias);
/*! Acceleration calibration offset: read raw acceleration offset correction on
 *  z axis.
 */
int adis_read_za_bias(struct adis_dev *adis, int32_t *za_bias);
/*! Acceleration calibration offset: write raw acceleration offset correction on
 *  z axis.
 */
int adis_write_za_bias(struct adis_dev *adis, int32_t za_bias);

/*! FIFO control: read FIFO enable bit value. */
int adis_read_fifo_en(struct adis_dev *adis, uint32_t *fifo_en);
/*! FIFO control: write FIFO enable bit value. */
int adis_write_fifo_en(struct adis_dev *adis, uint32_t fifo_en);
/*! FIFO overflow: read FIFO enable bit value. */
int adis_read_fifo_overflow(struct adis_dev *adis, uint32_t *fifo_overflow);
/*! FIFO control: write FIFO overflow bit value. */
int adis_write_fifo_overflow(struct adis_dev *adis, uint32_t fifo_overflow);
/*! FIFO control: read FIFO watermark interrupt enable bit value. */
int adis_read_fifo_wm_int_en(struct adis_dev *adis, uint32_t *fifo_wm_int_en);
/*! FIFO control: write FIFO watermark interrupt enable bit value. */
int adis_write_fifo_wm_int_en(struct adis_dev *adis, uint32_t fifo_wm_int_en);
/*! FIFO control: read FIFO watermark interrupt polarity bit value. */
int adis_read_fifo_wm_int_pol(struct adis_dev *adis, uint32_t *fifo_wm_int_pol);
/*! FIFO control: write FIFO watermark interrupt polarity bit value. */
int adis_write_fifo_wm_int_pol(struct adis_dev *adis, uint32_t fifo_wm_int_pol);
/*! FIFO control: read FIFO watermark threshold level value. */
int adis_read_fifo_wm_lvl(struct adis_dev *adis, uint32_t *fifo_wm_lvl);
/*! FIFO control: write FIFO watermark threshold level value. */
int adis_write_fifo_wm_lvl(struct adis_dev *adis, uint32_t fifo_wm_lvl);

/*! Filter control: read filter size variable B value. */
int adis_read_filt_size_var_b(struct adis_dev *adis, uint32_t *filt_size_var_b);
/*! Filter control: write filter size variable B value. */
int adis_write_filt_size_var_b(struct adis_dev *adis, uint32_t filt_size_var_b);

/*! Range identifier: read gyroscope measurement range value. */
int adis_read_gyro_meas_range(struct adis_dev *adis, uint32_t *gyro_meas_range);

/*! Miscellaneous control: read data ready polarity encoded value. */
int adis_read_dr_polarity(struct adis_dev *adis, uint32_t *dr_polarity);
/*! Miscellaneous control: write data ready polarity encoded value. */
int adis_write_dr_polarity(struct adis_dev *adis, uint32_t dr_polarity);
/*! Miscellaneous control: read sync polarity encoded value. */
int adis_read_sync_polarity(struct adis_dev *adis, uint32_t *sync_polarity);
/*! Miscellaneous control: write sync polarity encoded value. */
int adis_write_sync_polarity(struct adis_dev *adis, uint32_t sync_polarity);
/*! Miscellaneous control: read synchronization mode encoded value. */
int adis_read_sync_mode(struct adis_dev *adis, uint32_t *sync_mode);
/*! Miscellaneous control: write synchronization mode encoded value. */
int adis_write_sync_mode(struct adis_dev *adis, uint32_t sync_mode,
			 uint32_t ext_clk);
/*! Miscellaneous control: read internal sensor bandwidth encoded value. */
int adis_read_sens_bw(struct adis_dev *adis, uint32_t *sens_bw);
/*! Miscellaneous control: write internal sensor bandwidth encoded value. */
int adis_write_sens_bw(struct adis_dev *adis, uint32_t sens_bw);
/*! Miscellaneous control: read point of percussion alignment enable bit
 *  value.
 */
int adis_read_pt_of_perc_algnmt(struct adis_dev *adis,
				uint32_t *pt_of_perc_algnmt);
/*! Miscellaneous control: write point of percussion alignment enable bit
 *  value.
 */
int adis_write_pt_of_perc_algnmt(struct adis_dev *adis,
				 uint32_t pt_of_perc_algnmt);
/*! Miscellaneous control: read linear acceleration compensation enable bit
 *  value.
 */
int adis_read_linear_accl_comp(struct adis_dev *adis,
			       uint32_t *linear_accl_comp);
/*! Miscellaneous control: write linear acceleration compensation enable bit
 *  value.
 */
int adis_write_linear_accl_comp(struct adis_dev *adis,
				uint32_t linear_accl_comp);
/*! Miscellaneous control: read burst selection encoded value. */
int adis_read_burst_sel(struct adis_dev *adis, uint32_t *burst_sel);
/*! Miscellaneous control: write burst selection encoded value. */
int adis_write_burst_sel(struct adis_dev *adis, uint32_t burst_sel);
/*! Miscellaneous control: read burst32 enable bit value. */
int adis_read_burst32(struct adis_dev *adis, uint32_t *burst_size);
/*! Miscellaneous control: write burst32 enable bit value. */
int adis_write_burst32(struct adis_dev *adis, uint32_t burst_size);
/*! Miscellaneous control: read timestamp32 enable bit value. */
int adis_read_timestamp32(struct adis_dev *adis, uint32_t *timestamp32);
/*! Miscellaneous control: write timestamp32 enable bit value. */
int adis_write_timestamp32(struct adis_dev *adis, uint32_t timestamp32);
/*! Miscellaneous control: read 4khz internal sync enable bit value. */
int adis_read_sync_4khz(struct adis_dev *adis, uint32_t *sync_4khz);
/*! Miscellaneous control: write 4khz internal sync enable bit value. */
int adis_write_sync_4khz(struct adis_dev *adis, uint32_t sync_4khz);

/*! Sync input frequency multiplier: read external clock scale factor value. */
int adis_read_up_scale(struct adis_dev *adis, uint32_t *up_scale);
/*! Sync input frequency multiplier: write external clock scale factor value. */
int adis_write_up_scale(struct adis_dev *adis, uint32_t up_scale);

/*! Decimation filter: read decimation rate value.*/
int adis_read_dec_rate(struct adis_dev *adis, uint32_t *dec_rate);
/*! Decimation filter: write decimation rate value.*/
int adis_write_dec_rate(struct adis_dev *adis, uint32_t dec_rate);

/*! Continuous bias estimation: read time bias control. */
int adis_read_bias_corr_tbc(struct adis_dev *adis, uint32_t *bias_corr_tbc);
/*! Continuous bias estimation: write time bias control. */
int adis_write_bias_corr_tbc(struct adis_dev *adis, uint32_t bias_corr_tbc);
/*! Continuous bias estimation: read X-axis gyroscope bias correction enable
 *  bit.
 */
int adis_read_bias_corr_en_xg(struct adis_dev *adis, uint32_t *bias_corr_en_xg);
/*! Continuous bias estimation: write X-axis gyroscope bias correction enable
 *  bit.
 */
int adis_write_bias_corr_en_xg(struct adis_dev *adis, uint32_t bias_corr_en_xg);
/*! Continuous bias estimation: read Y-axis gyroscope bias correction enable
 *  bit.
 */
int adis_read_bias_corr_en_yg(struct adis_dev *adis, uint32_t *bias_corr_en_yg);
/*! Continuous bias estimation: write Y-axis gyroscope bias correction enable
 *  bit.
 */
int adis_write_bias_corr_en_yg(struct adis_dev *adis, uint32_t bias_corr_en_yg);
/*! Continuous bias estimation: read Z-axis gyroscope bias correction enable
 *  bit.
 */
int adis_read_bias_corr_en_zg(struct adis_dev *adis, uint32_t *bias_corr_en_zg);
/*! Continuous bias estimation: write Z-axis gyroscope bias correction enable
 *  bit.
 */
int adis_write_bias_corr_en_zg(struct adis_dev *adis, uint32_t bias_corr_en_zg);
/*! Continuous bias estimation: read X-axis accelerometer bias correction enable
 *  bit.
 */
int adis_read_bias_corr_en_xa(struct adis_dev *adis, uint32_t *bias_corr_en_xa);
/*! Continuous bias estimation: write X-axis accelerometer bias correction
 *  enable bit.
 */
int adis_write_bias_corr_en_xa(struct adis_dev *adis, uint32_t bias_corr_en_xa);
/*! Continuous bias estimation: read Y-axis accelerometer bias correction enable
 *  bit.
 */
int adis_read_bias_corr_en_ya(struct adis_dev *adis, uint32_t *bias_corr_en_ya);
/*! Continuous bias estimation: write Y-axis accelerometer bias correction
 *  enable bit.
 */
int adis_write_bias_corr_en_ya(struct adis_dev *adis, uint32_t bias_corr_en_ya);
/*! Continuous bias estimation: read Z-axis accelerometer bias correction enable
 *  bit.
 */
int adis_read_bias_corr_en_za(struct adis_dev *adis, uint32_t *bias_corr_en_za);
/*! Continuous bias estimation: write Z-axis accelerometer bias correction
 *  enable bit.
 */
int adis_write_bias_corr_en_za(struct adis_dev *adis, uint32_t bias_corr_en_za);

/*! Global commands: perform bias correction update command. */
int adis_cmd_bias_corr_update(struct adis_dev *adis);
/*! Global commands: perform factory calibration restore command. */
int adis_cmd_fact_calib_restore(struct adis_dev *adis);
/*! Global commands: perform sensor self test command. */
int adis_cmd_snsr_self_test(struct adis_dev *adis);
/*! Global commands: perform flash memory update command. */
int adis_cmd_fls_mem_update(struct adis_dev *adis);
/*! Global commands: perform flash memory test command. */
int adis_cmd_fls_mem_test(struct adis_dev *adis);
/*! Global commands: perform fifo flush command. */
int adis_cmd_fifo_flush(struct adis_dev *adis);
/*! Global commands: perform software reset command. */
int adis_cmd_sw_res(struct adis_dev *adis);

/*! Device identification data: read firmware revision. */
int adis_read_firm_rev(struct adis_dev *adis, uint32_t *firm_rev);
/*! Device identification data: read firmware revision day. */
int adis_read_firm_d(struct adis_dev *adis, uint32_t *firm_d);
/*! Device identification data: read firmware revision month. */
int adis_read_firm_m(struct adis_dev *adis, uint32_t *firm_m);
/*! Device identification data: read firmware revision year. */
int adis_read_firm_y(struct adis_dev *adis, uint32_t *firm_y);
/*! Device identification data: read product identification. */
int adis_read_prod_id(struct adis_dev *adis, uint32_t *prod_id);
/*! Device identification data: read serial number. */
int adis_read_serial_num(struct adis_dev *adis, uint32_t *serial_num);

/*! Scratch pad registers: read scratch register 1. */
int adis_read_usr_scr_1(struct adis_dev *adis, uint32_t *usr_scr_1);
/*! Scratch pad registers: write scratch register 1. */
int adis_write_usr_scr_1(struct adis_dev *adis, uint32_t usr_scr_1);
/*! Scratch pad registers: read scratch register 2. */
int adis_read_usr_scr_2(struct adis_dev *adis, uint32_t *usr_scr_2);
/*! Scratch pad registers: write scratch register 2. */
int adis_write_usr_scr_2(struct adis_dev *adis, uint32_t usr_scr_2);
/*! Scratch pad registers: read scratch register 3. */
int adis_read_usr_scr_3(struct adis_dev *adis, uint32_t *usr_scr_3);
/*! Scratch pad registers: write scratch register 3. */
int adis_write_usr_scr_3(struct adis_dev *adis, uint32_t usr_scr_3);

/*! Flash counter: read flash memory write cycle counter value. */
int adis_read_fls_mem_wr_cntr(struct adis_dev *adis, uint32_t *fls_mem_wr_cntr);

/*! Read burst data */
int adis_read_burst_data(struct adis_dev *adis,struct adis_burst_data *data,
			 bool burst32, uint8_t burst_sel, bool fifo_pop);

/*! Update external clock frequency. */
int adis_update_ext_clk_freq(struct adis_dev *adis, uint32_t clk_freq);

/*! Read adis synchronization clock frequency value in Hertz. */
int adis_get_sync_clk_freq(struct adis_dev *adis, uint32_t *clk_freq);

/*! Read adis device gyroscope scale in fractional form. */
int adis_get_anglvel_scale(struct adis_dev *adis,
			   struct adis_scale_fractional *anglvel_scale);

/*! Read adis device acceleration scale in fractional form. */
int adis_get_accl_scale(struct adis_dev *adis,
			struct adis_scale_fractional *accl_scale);

/*! Read adis device delta angle scale in fractional form. */
int adis_get_deltaangl_scale(struct adis_dev *adis,
			     struct adis_scale_fractional_log2 *deltaangl_scale);

/*! Read adis device delta velocity scale in fractional form. */
int adis_get_deltavelocity_scale(struct adis_dev *adis,
				 struct adis_scale_fractional_log2 *deltavelocity_scale);

/*! Read adis device temperature scale in fractional form. */
int adis_get_temp_scale(struct adis_dev *adis,
			struct adis_scale_fractional *temp_scale);
#endif
