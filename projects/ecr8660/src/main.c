/***************************************************************************//**
 *   @file   ad9361/src/main.c
 *   @brief  Implementation of Main Function.
 *   @author DBogdan (dragos.bogdan@analog.com)
 *   @author TonyXu
********************************************************************************
 * Copyright 2013(c) Analog Devices, Inc.
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
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <inttypes.h>
#include "app_config.h"
#include "ad9361_api.h"
#include "parameters.h"
#include "no_os_spi.h"
#include "no_os_gpio.h"
#include "no_os_delay.h"
#ifdef XILINX_PLATFORM
#include <xparameters.h>
#include <xil_cache.h>
#include "xilinx_spi.h"
#include "xilinx_gpio.h"
#include "no_os_irq.h"
#include "axi_sysid.h"
#endif
#ifdef LINUX_PLATFORM
#include "linux_spi.h"
#include "linux_gpio.h"
#else
#include "xilinx_irq.h"
#endif //LINUX

#include "axi_adc_core.h"
#include "axi_dac_core.h"
#include "axi_dmac.h"
#include "no_os_error.h"

//ECR8660 header files
#include "spi.h"
#include "ECR8660_SPI.h"
#include "ECR8660.h"

#ifdef IIO_SUPPORT

#include "iio_axi_adc.h"
#include "iio_axi_dac.h"
#include "iio_ad9361.h"
#include "no_os_uart.h"
#include "iio_app.h"

#ifdef XILINX_PLATFORM
#include "xilinx_uart.h"
#include "xil_cache.h"
#endif //XILINX

#if defined LINUX_PLATFORM || defined GENERIC_PLATFORM
static uint8_t in_buff[MAX_SIZE_BASE_ADDR];
static uint8_t out_buff[MAX_SIZE_BASE_ADDR];
#endif

#endif // IIO_SUPPORT

#include "no_os_uart.h"
#include "xilinx_uart.h"
#include "xil_cache.h"

#if defined(DMA_EXAMPLE) || defined(SYSID_BASEADDR)
#include <string.h>
#endif

#define ECR8660_DEVICE 1

typedef enum
{
	ECR8660_NONE	= 0,
	ECR8660_SPI_TEST = 1,
	ECR8660_FUNC_TEST = 2,
}ECR8660_TestMask;

ECR8660_TestMask ECR8660_TestItem = ECR8660_SPI_TEST | ECR8660_FUNC_TEST;

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/

#if defined(DMA_EXAMPLE) || defined(IIO_SUPPORT)
uint32_t dac_buffer[DAC_BUFFER_SAMPLES] __attribute__ ((aligned));
#endif
uint16_t adc_buffer[ADC_BUFFER_SAMPLES * ADC_CHANNELS] __attribute__ ((
			aligned));

#define AD9361_ADC_DAC_BYTES_PER_SAMPLE 2

#ifdef XILINX_PLATFORM
struct xil_spi_init_param xil_spi_param = {
#ifdef PLATFORM_MB
	.type = SPI_PL,
#else
	.type = SPI_PS,
#endif
	.flags = 0
};

struct xil_gpio_init_param xil_gpio_param = {
#ifdef PLATFORM_MB
	.type = GPIO_PL,
#else
	.type = GPIO_PS,
#endif
	.device_id = GPIO_DEVICE_ID
};
#if 0
/**
 * uart configurations.
 */
struct xil_uart_init_param platform_uart_init_par = {
#ifdef XPAR_XUARTLITE_NUM_INSTANCES
	.type = UART_PL,
#else
	.type = UART_PS,
	.irq_id = UART_IRQ_ID
#endif
};

struct no_os_uart_init_param iio_uart_ip = {
	.device_id = UART_DEVICE_ID,
	.irq_id = UART_IRQ_ID,
	.baud_rate = UART_BAUDRATE,
	.size = NO_OS_UART_CS_8,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.extra = &platform_uart_init_par,
	.platform_ops = &xil_uart_ops
};
#endif
#define GPIO_OPS	&xil_gpio_ops
#define SPI_OPS		&xil_spi_ops
#define GPIO_PARAM	&xil_gpio_param
#define SPI_PARAM	&xil_spi_param
#endif

#ifdef GENERIC_PLATFORM
#define GPIO_OPS	&generic_gpio_ops
#define SPI_OPS		&generic_spi_ops
#define GPIO_PARAM	NULL
#define SPI_PARAM	NULL
#endif
#ifdef XILINX_PLATFORM
#endif
#ifdef LINUX_PLATFORM
#define GPIO_OPS	&linux_gpio_ops
#define SPI_OPS		&linux_spi_ops
#define GPIO_PARAM	NULL
#define SPI_PARAM	NULL
#endif

struct axi_adc_init rx_adc_init = {
	.name = "cf-ad9361-lpc",
	.base = RX_CORE_BASEADDR,
#ifdef FMCOMMS5
	.slave_base = AD9361_RX_1_BASEADDR,
	.num_channels = 8,
#else
	.num_channels = 4,
#endif
	.num_slave_channels =  4
};
struct axi_dac_init tx_dac_init = {
	"cf-ad9361-dds-core-lpc",
	TX_CORE_BASEADDR,
	4,
	NULL,
	1
};
struct axi_dmac_init rx_dmac_init = {
	"rx_dmac",
	CF_AD9361_RX_DMA_BASEADDR,
#ifdef DMA_IRQ_ENABLE
	IRQ_ENABLED
#else
	IRQ_DISABLED
#endif
};
struct axi_dmac *rx_dmac;
struct axi_dmac_init tx_dmac_init = {
	"tx_dmac",
	CF_AD9361_TX_DMA_BASEADDR,
#ifdef DMA_IRQ_ENABLE
	IRQ_ENABLED
#else
	IRQ_DISABLED
#endif
};
struct axi_dmac *tx_dmac;
struct axi_dma_transfer transfer = {
	// Number of bytes to write/read
	.size = sizeof(sine_lut_iq),
	// Transfer done flag
	.transfer_done = 0,
	// Signal transfer mode
	.cyclic = CYCLIC,
	// Address of data source
	.src_addr = (uintptr_t)dac_buffer,
	// Address of data destination
	.dest_addr = 0
};

struct axi_dma_transfer read_transfer = {
	// Number of bytes to write/read
	.size = sizeof(adc_buffer),
	// Transfer done flag
	.transfer_done = 0,
	// Signal transfer mode
	.cyclic = NO,
	// Address of data source
	.src_addr = 0,
	// Address of data destination
	.dest_addr = (uintptr_t)adc_buffer
};

uint8_t tx_is_transfering = 0u;

AD9361_InitParam default_init_param = {
	/* Device selection */
	ID_AD9361,	// dev_sel
	/* Reference Clock */
	40000000UL,	//reference_clk_rate
	/* Base Configuration */
	1,		//two_rx_two_tx_mode_enable *** adi,2rx-2tx-mode-enable
	1,		//one_rx_one_tx_mode_use_rx_num *** adi,1rx-1tx-mode-use-rx-num
	1,		//one_rx_one_tx_mode_use_tx_num *** adi,1rx-1tx-mode-use-tx-num
	1,		//frequency_division_duplex_mode_enable *** adi,frequency-division-duplex-mode-enable
	0,		//frequency_division_duplex_independent_mode_enable *** adi,frequency-division-duplex-independent-mode-enable
	0,		//tdd_use_dual_synth_mode_enable *** adi,tdd-use-dual-synth-mode-enable
	0,		//tdd_skip_vco_cal_enable *** adi,tdd-skip-vco-cal-enable
	0,		//tx_fastlock_delay_ns *** adi,tx-fastlock-delay-ns
	0,		//rx_fastlock_delay_ns *** adi,rx-fastlock-delay-ns
	0,		//rx_fastlock_pincontrol_enable *** adi,rx-fastlock-pincontrol-enable
	0,		//tx_fastlock_pincontrol_enable *** adi,tx-fastlock-pincontrol-enable
	0,		//external_rx_lo_enable *** adi,external-rx-lo-enable
	0,		//external_tx_lo_enable *** adi,external-tx-lo-enable
	5,		//dc_offset_tracking_update_event_mask *** adi,dc-offset-tracking-update-event-mask
	6,		//dc_offset_attenuation_high_range *** adi,dc-offset-attenuation-high-range
	5,		//dc_offset_attenuation_low_range *** adi,dc-offset-attenuation-low-range
	0x28,	//dc_offset_count_high_range *** adi,dc-offset-count-high-range
	0x32,	//dc_offset_count_low_range *** adi,dc-offset-count-low-range
	0,		//split_gain_table_mode_enable *** adi,split-gain-table-mode-enable
	MAX_SYNTH_FREF,	//trx_synthesizer_target_fref_overwrite_hz *** adi,trx-synthesizer-target-fref-overwrite-hz
	0,		// qec_tracking_slow_mode_enable *** adi,qec-tracking-slow-mode-enable
	/* ENSM Control */
	0,		//ensm_enable_pin_pulse_mode_enable *** adi,ensm-enable-pin-pulse-mode-enable
	0,		//ensm_enable_txnrx_control_enable *** adi,ensm-enable-txnrx-control-enable
	/* LO Control */
	2400000000UL,	//rx_synthesizer_frequency_hz *** adi,rx-synthesizer-frequency-hz
	2400000000UL,	//tx_synthesizer_frequency_hz *** adi,tx-synthesizer-frequency-hz
	1,				//tx_lo_powerdown_managed_enable *** adi,tx-lo-powerdown-managed-enable
	/* Rate & BW Control */
	{983040000, 245760000, 122880000, 61440000, 30720000, 30720000},// rx_path_clock_frequencies[6] *** adi,rx-path-clock-frequencies
	{983040000, 122880000, 122880000, 61440000, 30720000, 30720000},// tx_path_clock_frequencies[6] *** adi,tx-path-clock-frequencies
	18000000,//rf_rx_bandwidth_hz *** adi,rf-rx-bandwidth-hz
	18000000,//rf_tx_bandwidth_hz *** adi,rf-tx-bandwidth-hz
	/* RF Port Control */
	0,		//rx_rf_port_input_select *** adi,rx-rf-port-input-select
	0,		//tx_rf_port_input_select *** adi,tx-rf-port-input-select
	/* TX Attenuation Control */
	10000,	//tx_attenuation_mdB *** adi,tx-attenuation-mdB
	0,		//update_tx_gain_in_alert_enable *** adi,update-tx-gain-in-alert-enable
	/* Reference Clock Control */
	0,		//xo_disable_use_ext_refclk_enable *** adi,xo-disable-use-ext-refclk-enable
	{8, 5920},	//dcxo_coarse_and_fine_tune[2] *** adi,dcxo-coarse-and-fine-tune
	CLKOUT_DISABLE,	//clk_output_mode_select *** adi,clk-output-mode-select
	/* Gain Control */
	2,		//gc_rx1_mode *** adi,gc-rx1-mode
	2,		//gc_rx2_mode *** adi,gc-rx2-mode
	58,		//gc_adc_large_overload_thresh *** adi,gc-adc-large-overload-thresh
	4,		//gc_adc_ovr_sample_size *** adi,gc-adc-ovr-sample-size
	47,		//gc_adc_small_overload_thresh *** adi,gc-adc-small-overload-thresh
	8192,	//gc_dec_pow_measurement_duration *** adi,gc-dec-pow-measurement-duration
	0,		//gc_dig_gain_enable *** adi,gc-dig-gain-enable
	800,	//gc_lmt_overload_high_thresh *** adi,gc-lmt-overload-high-thresh
	704,	//gc_lmt_overload_low_thresh *** adi,gc-lmt-overload-low-thresh
	24,		//gc_low_power_thresh *** adi,gc-low-power-thresh
	15,		//gc_max_dig_gain *** adi,gc-max-dig-gain
	0,		//gc_use_rx_fir_out_for_dec_pwr_meas_enable *** adi,gc-use-rx-fir-out-for-dec-pwr-meas-enable
	/* Gain MGC Control */
	2,		//mgc_dec_gain_step *** adi,mgc-dec-gain-step
	2,		//mgc_inc_gain_step *** adi,mgc-inc-gain-step
	0,		//mgc_rx1_ctrl_inp_enable *** adi,mgc-rx1-ctrl-inp-enable
	0,		//mgc_rx2_ctrl_inp_enable *** adi,mgc-rx2-ctrl-inp-enable
	0,		//mgc_split_table_ctrl_inp_gain_mode *** adi,mgc-split-table-ctrl-inp-gain-mode
	/* Gain AGC Control */
	10,		//agc_adc_large_overload_exceed_counter *** adi,agc-adc-large-overload-exceed-counter
	2,		//agc_adc_large_overload_inc_steps *** adi,agc-adc-large-overload-inc-steps
	0,		//agc_adc_lmt_small_overload_prevent_gain_inc_enable *** adi,agc-adc-lmt-small-overload-prevent-gain-inc-enable
	10,		//agc_adc_small_overload_exceed_counter *** adi,agc-adc-small-overload-exceed-counter
	4,		//agc_dig_gain_step_size *** adi,agc-dig-gain-step-size
	3,		//agc_dig_saturation_exceed_counter *** adi,agc-dig-saturation-exceed-counter
	1000,	// agc_gain_update_interval_us *** adi,agc-gain-update-interval-us
	0,		//agc_immed_gain_change_if_large_adc_overload_enable *** adi,agc-immed-gain-change-if-large-adc-overload-enable
	0,		//agc_immed_gain_change_if_large_lmt_overload_enable *** adi,agc-immed-gain-change-if-large-lmt-overload-enable
	10,		//agc_inner_thresh_high *** adi,agc-inner-thresh-high
	1,		//agc_inner_thresh_high_dec_steps *** adi,agc-inner-thresh-high-dec-steps
	12,		//agc_inner_thresh_low *** adi,agc-inner-thresh-low
	1,		//agc_inner_thresh_low_inc_steps *** adi,agc-inner-thresh-low-inc-steps
	10,		//agc_lmt_overload_large_exceed_counter *** adi,agc-lmt-overload-large-exceed-counter
	2,		//agc_lmt_overload_large_inc_steps *** adi,agc-lmt-overload-large-inc-steps
	10,		//agc_lmt_overload_small_exceed_counter *** adi,agc-lmt-overload-small-exceed-counter
	5,		//agc_outer_thresh_high *** adi,agc-outer-thresh-high
	2,		//agc_outer_thresh_high_dec_steps *** adi,agc-outer-thresh-high-dec-steps
	18,		//agc_outer_thresh_low *** adi,agc-outer-thresh-low
	2,		//agc_outer_thresh_low_inc_steps *** adi,agc-outer-thresh-low-inc-steps
	1,		//agc_attack_delay_extra_margin_us; *** adi,agc-attack-delay-extra-margin-us
	0,		//agc_sync_for_gain_counter_enable *** adi,agc-sync-for-gain-counter-enable
	/* Fast AGC */
	64,		//fagc_dec_pow_measuremnt_duration ***  adi,fagc-dec-pow-measurement-duration
	260,	//fagc_state_wait_time_ns ***  adi,fagc-state-wait-time-ns
	/* Fast AGC - Low Power */
	0,		//fagc_allow_agc_gain_increase ***  adi,fagc-allow-agc-gain-increase-enable
	5,		//fagc_lp_thresh_increment_time ***  adi,fagc-lp-thresh-increment-time
	1,		//fagc_lp_thresh_increment_steps ***  adi,fagc-lp-thresh-increment-steps
	/* Fast AGC - Lock Level (Lock Level is set via slow AGC inner high threshold) */
	1,		//fagc_lock_level_lmt_gain_increase_en ***  adi,fagc-lock-level-lmt-gain-increase-enable
	5,		//fagc_lock_level_gain_increase_upper_limit ***  adi,fagc-lock-level-gain-increase-upper-limit
	/* Fast AGC - Peak Detectors and Final Settling */
	1,		//fagc_lpf_final_settling_steps ***  adi,fagc-lpf-final-settling-steps
	1,		//fagc_lmt_final_settling_steps ***  adi,fagc-lmt-final-settling-steps
	3,		//fagc_final_overrange_count ***  adi,fagc-final-overrange-count
	/* Fast AGC - Final Power Test */
	0,		//fagc_gain_increase_after_gain_lock_en ***  adi,fagc-gain-increase-after-gain-lock-enable
	/* Fast AGC - Unlocking the Gain */
	0,		//fagc_gain_index_type_after_exit_rx_mode ***  adi,fagc-gain-index-type-after-exit-rx-mode
	1,		//fagc_use_last_lock_level_for_set_gain_en ***  adi,fagc-use-last-lock-level-for-set-gain-enable
	1,		//fagc_rst_gla_stronger_sig_thresh_exceeded_en ***  adi,fagc-rst-gla-stronger-sig-thresh-exceeded-enable
	5,		//fagc_optimized_gain_offset ***  adi,fagc-optimized-gain-offset
	10,		//fagc_rst_gla_stronger_sig_thresh_above_ll ***  adi,fagc-rst-gla-stronger-sig-thresh-above-ll
	1,		//fagc_rst_gla_engergy_lost_sig_thresh_exceeded_en ***  adi,fagc-rst-gla-engergy-lost-sig-thresh-exceeded-enable
	1,		//fagc_rst_gla_engergy_lost_goto_optim_gain_en ***  adi,fagc-rst-gla-engergy-lost-goto-optim-gain-enable
	10,		//fagc_rst_gla_engergy_lost_sig_thresh_below_ll ***  adi,fagc-rst-gla-engergy-lost-sig-thresh-below-ll
	8,		//fagc_energy_lost_stronger_sig_gain_lock_exit_cnt ***  adi,fagc-energy-lost-stronger-sig-gain-lock-exit-cnt
	1,		//fagc_rst_gla_large_adc_overload_en ***  adi,fagc-rst-gla-large-adc-overload-enable
	1,		//fagc_rst_gla_large_lmt_overload_en ***  adi,fagc-rst-gla-large-lmt-overload-enable
	0,		//fagc_rst_gla_en_agc_pulled_high_en ***  adi,fagc-rst-gla-en-agc-pulled-high-enable
	0,		//fagc_rst_gla_if_en_agc_pulled_high_mode ***  adi,fagc-rst-gla-if-en-agc-pulled-high-mode
	64,		//fagc_power_measurement_duration_in_state5 ***  adi,fagc-power-measurement-duration-in-state5
	2,		//fagc_large_overload_inc_steps *** adi,fagc-adc-large-overload-inc-steps
	/* RSSI Control */
	1,		//rssi_delay *** adi,rssi-delay
	1000,	//rssi_duration *** adi,rssi-duration
	3,		//rssi_restart_mode *** adi,rssi-restart-mode
	0,		//rssi_unit_is_rx_samples_enable *** adi,rssi-unit-is-rx-samples-enable
	1,		//rssi_wait *** adi,rssi-wait
	/* Aux ADC Control */
	256,	//aux_adc_decimation *** adi,aux-adc-decimation
	40000000UL,	//aux_adc_rate *** adi,aux-adc-rate
	/* AuxDAC Control */
	1,		//aux_dac_manual_mode_enable ***  adi,aux-dac-manual-mode-enable
	0,		//aux_dac1_default_value_mV ***  adi,aux-dac1-default-value-mV
	0,		//aux_dac1_active_in_rx_enable ***  adi,aux-dac1-active-in-rx-enable
	0,		//aux_dac1_active_in_tx_enable ***  adi,aux-dac1-active-in-tx-enable
	0,		//aux_dac1_active_in_alert_enable ***  adi,aux-dac1-active-in-alert-enable
	0,		//aux_dac1_rx_delay_us ***  adi,aux-dac1-rx-delay-us
	0,		//aux_dac1_tx_delay_us ***  adi,aux-dac1-tx-delay-us
	0,		//aux_dac2_default_value_mV ***  adi,aux-dac2-default-value-mV
	0,		//aux_dac2_active_in_rx_enable ***  adi,aux-dac2-active-in-rx-enable
	0,		//aux_dac2_active_in_tx_enable ***  adi,aux-dac2-active-in-tx-enable
	0,		//aux_dac2_active_in_alert_enable ***  adi,aux-dac2-active-in-alert-enable
	0,		//aux_dac2_rx_delay_us ***  adi,aux-dac2-rx-delay-us
	0,		//aux_dac2_tx_delay_us ***  adi,aux-dac2-tx-delay-us
	/* Temperature Sensor Control */
	256,	//temp_sense_decimation *** adi,temp-sense-decimation
	1000,	//temp_sense_measurement_interval_ms *** adi,temp-sense-measurement-interval-ms
	0xCE,	//temp_sense_offset_signed *** adi,temp-sense-offset-signed
	1,		//temp_sense_periodic_measurement_enable *** adi,temp-sense-periodic-measurement-enable
	/* Control Out Setup */
	0xFF,	//ctrl_outs_enable_mask *** adi,ctrl-outs-enable-mask
	0,		//ctrl_outs_index *** adi,ctrl-outs-index
	/* External LNA Control */
	0,		//elna_settling_delay_ns *** adi,elna-settling-delay-ns
	0,		//elna_gain_mdB *** adi,elna-gain-mdB
	0,		//elna_bypass_loss_mdB *** adi,elna-bypass-loss-mdB
	0,		//elna_rx1_gpo0_control_enable *** adi,elna-rx1-gpo0-control-enable
	0,		//elna_rx2_gpo1_control_enable *** adi,elna-rx2-gpo1-control-enable
	0,		//elna_gaintable_all_index_enable *** adi,elna-gaintable-all-index-enable
	/* Digital Interface Control */
	0,		//digital_interface_tune_skip_mode *** adi,digital-interface-tune-skip-mode
	0,		//digital_interface_tune_fir_disable *** adi,digital-interface-tune-fir-disable
	1,		//pp_tx_swap_enable *** adi,pp-tx-swap-enable
	1,		//pp_rx_swap_enable *** adi,pp-rx-swap-enable
	0,		//tx_channel_swap_enable *** adi,tx-channel-swap-enable
	0,		//rx_channel_swap_enable *** adi,rx-channel-swap-enable
	1,		//rx_frame_pulse_mode_enable *** adi,rx-frame-pulse-mode-enable
	0,		//two_t_two_r_timing_enable *** adi,2t2r-timing-enable
	0,		//invert_data_bus_enable *** adi,invert-data-bus-enable
	0,		//invert_data_clk_enable *** adi,invert-data-clk-enable
	0,		//fdd_alt_word_order_enable *** adi,fdd-alt-word-order-enable
	0,		//invert_rx_frame_enable *** adi,invert-rx-frame-enable
	0,		//fdd_rx_rate_2tx_enable *** adi,fdd-rx-rate-2tx-enable
	0,		//swap_ports_enable *** adi,swap-ports-enable
	0,		//single_data_rate_enable *** adi,single-data-rate-enable
	1,		//lvds_mode_enable *** adi,lvds-mode-enable
	0,		//half_duplex_mode_enable *** adi,half-duplex-mode-enable
	0,		//single_port_mode_enable *** adi,single-port-mode-enable
	0,		//full_port_enable *** adi,full-port-enable
	0,		//full_duplex_swap_bits_enable *** adi,full-duplex-swap-bits-enable
	0,		//delay_rx_data *** adi,delay-rx-data
	0,		//rx_data_clock_delay *** adi,rx-data-clock-delay
	4,		//rx_data_delay *** adi,rx-data-delay
	7,		//tx_fb_clock_delay *** adi,tx-fb-clock-delay
	0,		//tx_data_delay *** adi,tx-data-delay
#ifdef ALTERA_PLATFORM
	300,	//lvds_bias_mV *** adi,lvds-bias-mV
#else
	150,	//lvds_bias_mV *** adi,lvds-bias-mV
#endif
	1,		//lvds_rx_onchip_termination_enable *** adi,lvds-rx-onchip-termination-enable
	0,		//rx1rx2_phase_inversion_en *** adi,rx1-rx2-phase-inversion-enable
	0xFF,	//lvds_invert1_control *** adi,lvds-invert1-control
	0x0F,	//lvds_invert2_control *** adi,lvds-invert2-control
	/* GPO Control */
	0,		//gpo_manual_mode_enable *** adi,gpo-manual-mode-enable
	0,		//gpo_manual_mode_enable_mask *** adi,gpo-manual-mode-enable-mask
	0,		//gpo0_inactive_state_high_enable *** adi,gpo0-inactive-state-high-enable
	0,		//gpo1_inactive_state_high_enable *** adi,gpo1-inactive-state-high-enable
	0,		//gpo2_inactive_state_high_enable *** adi,gpo2-inactive-state-high-enable
	0,		//gpo3_inactive_state_high_enable *** adi,gpo3-inactive-state-high-enable
	0,		//gpo0_slave_rx_enable *** adi,gpo0-slave-rx-enable
	0,		//gpo0_slave_tx_enable *** adi,gpo0-slave-tx-enable
	0,		//gpo1_slave_rx_enable *** adi,gpo1-slave-rx-enable
	0,		//gpo1_slave_tx_enable *** adi,gpo1-slave-tx-enable
	0,		//gpo2_slave_rx_enable *** adi,gpo2-slave-rx-enable
	0,		//gpo2_slave_tx_enable *** adi,gpo2-slave-tx-enable
	0,		//gpo3_slave_rx_enable *** adi,gpo3-slave-rx-enable
	0,		//gpo3_slave_tx_enable *** adi,gpo3-slave-tx-enable
	0,		//gpo0_rx_delay_us *** adi,gpo0-rx-delay-us
	0,		//gpo0_tx_delay_us *** adi,gpo0-tx-delay-us
	0,		//gpo1_rx_delay_us *** adi,gpo1-rx-delay-us
	0,		//gpo1_tx_delay_us *** adi,gpo1-tx-delay-us
	0,		//gpo2_rx_delay_us *** adi,gpo2-rx-delay-us
	0,		//gpo2_tx_delay_us *** adi,gpo2-tx-delay-us
	0,		//gpo3_rx_delay_us *** adi,gpo3-rx-delay-us
	0,		//gpo3_tx_delay_us *** adi,gpo3-tx-delay-us
	/* Tx Monitor Control */
	37000,	//low_high_gain_threshold_mdB *** adi,txmon-low-high-thresh
	0,		//low_gain_dB *** adi,txmon-low-gain
	24,		//high_gain_dB *** adi,txmon-high-gain
	0,		//tx_mon_track_en *** adi,txmon-dc-tracking-enable
	0,		//one_shot_mode_en *** adi,txmon-one-shot-mode-enable
	511,	//tx_mon_delay *** adi,txmon-delay
	8192,	//tx_mon_duration *** adi,txmon-duration
	2,		//tx1_mon_front_end_gain *** adi,txmon-1-front-end-gain
	2,		//tx2_mon_front_end_gain *** adi,txmon-2-front-end-gain
	48,		//tx1_mon_lo_cm *** adi,txmon-1-lo-cm
	48,		//tx2_mon_lo_cm *** adi,txmon-2-lo-cm
	/* GPIO definitions */
	{
		.number = -1,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_PARAM
	},		//gpio_resetb *** reset-gpios
	/* MCS Sync */
	{
		.number = -1,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_PARAM
	},		//gpio_sync *** sync-gpios

	{
		.number = -1,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_PARAM
	},		//gpio_cal_sw1 *** cal-sw1-gpios

	{
		.number = -1,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_PARAM
	},		//gpio_cal_sw2 *** cal-sw2-gpios

	{
		.device_id = SPI_DEVICE_ID,
		.mode = NO_OS_SPI_MODE_1,
		.chip_select = SPI_CS,
		.platform_ops = SPI_OPS,
		.extra = SPI_PARAM,
		//.max_speed_hz = 640000,
	},

	/* External LO clocks */
	NULL,	//(*ad9361_rfpll_ext_recalc_rate)()
	NULL,	//(*ad9361_rfpll_ext_round_rate)()
	NULL,	//(*ad9361_rfpll_ext_set_rate)()
#ifndef AXI_ADC_NOT_PRESENT
	&rx_adc_init,	// *rx_adc_init
	&tx_dac_init,   // *tx_dac_init
#endif
};

AD9361_RXFIRConfig rx_fir_config = {	// BPF PASSBAND 3/20 fs to 1/4 fs
	3, // rx
	0, // rx_gain
	1, // rx_dec
	{
		-4, -6, -37, 35, 186, 86, -284, -315,
			107, 219, -4, 271, 558, -307, -1182, -356,
			658, 157, 207, 1648, 790, -2525, -2553, 748,
			865, -476, 3737, 6560, -3583, -14731, -5278, 14819,
			14819, -5278, -14731, -3583, 6560, 3737, -476, 865,
			748, -2553, -2525, 790, 1648, 207, 157, 658,
			-356, -1182, -307, 558, 271, -4, 219, 107,
			-315, -284, 86, 186, 35, -37, -6, -4,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0
		}, // rx_coef[128]
	64, // rx_coef_size
	{0, 0, 0, 0, 0, 0}, //rx_path_clks[6]
	0 // rx_bandwidth
};

AD9361_TXFIRConfig tx_fir_config = {	// BPF PASSBAND 3/20 fs to 1/4 fs
	3, // tx
	-6, // tx_gain
	1, // tx_int
	{
		-4, -6, -37, 35, 186, 86, -284, -315,
			107, 219, -4, 271, 558, -307, -1182, -356,
			658, 157, 207, 1648, 790, -2525, -2553, 748,
			865, -476, 3737, 6560, -3583, -14731, -5278, 14819,
			14819, -5278, -14731, -3583, 6560, 3737, -476, 865,
			748, -2553, -2525, 790, 1648, 207, 157, 658,
			-356, -1182, -307, 558, 271, -4, 219, 107,
			-315, -284, 86, 186, 35, -37, -6, -4,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0
		}, // tx_coef[128]
	64, // tx_coef_size
	{0, 0, 0, 0, 0, 0}, // tx_path_clks[6]
	0 // tx_bandwidth
};
struct ad9361_rf_phy *ad9361_phy;

void parse_spi_command(struct no_os_spi_desc *spi);

/***************************************************************************//**
 * @brief main
*******************************************************************************/
int main(void)
{
	uint32_t retWord = 0;
	ErrorCode error_code = NO_ERROR;
	int32_t status = 0;

#ifdef XILINX_PLATFORM
	Xil_ICacheEnable();
	Xil_DCacheEnable();
	default_init_param.spi_param.extra = &xil_spi_param;
	default_init_param.spi_param.platform_ops = &xil_spi_ops;
#endif

	// NOTE: The user has to choose the GPIO numbers according to desired
	// carrier board.
	default_init_param.gpio_resetb.number = GPIO_RESET_PIN;

	default_init_param.gpio_sync.number = -1;
	default_init_param.gpio_cal_sw1.number = -1;
	default_init_param.gpio_cal_sw2.number = -1;

	//DEBUG: add 3s delay
	no_os_mdelay(3000);
	printf("\nHello from ZYNQ...\n");

	if (AD9364_DEVICE) {
		default_init_param.dev_sel = ID_AD9364;
		tx_dac_init.num_channels = 2;
		tx_dac_init.rate = 1;
		rx_adc_init.num_channels = 2;
		rx_adc_init.num_slave_channels = 0;
	}
	if (AD9363A_DEVICE)
		default_init_param.dev_sel = ID_AD9363A;

#if ECR8660_DEVICE
	if((ECR8660_TestItem & ECR8660_SPI_TEST) == ECR8660_SPI_TEST)
	{
		/* init ad9361 phy */
		ad9361_phy = (struct ad9361_rf_phy *)no_os_calloc(1, sizeof(*ad9361_phy));
		if (!ad9361_phy)
		{
			error_code = PHY_INIT_FAIL;
			printf("\nno_os_calloc(1, sizeof(ad9361_phy)) failed.\n");
		}
		else
		{
			printf("\nad9361_phy has been initialized.\n");
		}
		ad9361_phy->clk_refin = (struct no_os_clk *)no_os_calloc(1, sizeof(*ad9361_phy->clk_refin));
		if (!ad9361_phy->clk_refin) {
			no_os_free(ad9361_phy);
			return -ENOMEM;
		}

		ad9361_phy->pdata = (struct ad9361_phy_platform_data *)no_os_calloc(1,
				sizeof(*ad9361_phy->pdata));
		if (!ad9361_phy->pdata) {
			no_os_free(ad9361_phy->clk_refin);
			no_os_free(ad9361_phy);
			return -ENOMEM;
		}
#ifndef AXI_ADC_NOT_PRESENT
		ad9361_phy->adc_conv = (struct axiadc_converter *)no_os_calloc(1,
				sizeof(*ad9361_phy->adc_conv));
		if (!ad9361_phy->adc_conv) {
			no_os_free(ad9361_phy->pdata);
			no_os_free(ad9361_phy->clk_refin);
			no_os_free(ad9361_phy);
			return -ENOMEM;
		}

		ad9361_phy->adc_state = (struct axiadc_state *)no_os_calloc(1,
				 sizeof(*ad9361_phy->adc_state));
		if (!ad9361_phy->adc_state) {
			no_os_free(ad9361_phy->adc_conv);
			no_os_free(ad9361_phy->pdata);
			no_os_free(ad9361_phy->clk_refin);
			no_os_free(ad9361_phy);
			return -ENOMEM;
		}
		ad9361_phy->adc_state->phy = ad9361_phy;
#endif

		if(error_code == NO_ERROR)
		{
#if 0
			ad9361_init(&ad9361_phy, &default_init_param);
#else
			if(no_os_spi_init(&ad9361_phy->spi, &default_init_param.spi_param) != 0)
			{
				error_code = SPI_INIT_FAIL;
				printf("\nad9361 spi init failed.\n");
			}
#endif
		}

#if 0
		/* eswin_9631 control */
		spi_hook(ad9361_phy->spi);
		uint32_t temp_data = 0;
		while(1)
		{

			temp_data = ECR8660_Read(SPI_RW_EXTERNAL, 0x002002B0, 1);
			no_os_mdelay(1);
			ECR8660_write(SPI_RW_EXTERNAL, 0x002002B0, 0x0);
			no_os_mdelay(1);
			temp_data = ECR8660_Read(SPI_RW_EXTERNAL, 0x002002B0, 1);
			no_os_mdelay(1);
			ECR8660_write(SPI_RW_EXTERNAL, 0x00200494, 0x0);
			no_os_mdelay(1);
			temp_data = ECR8660_Read(SPI_RW_EXTERNAL, 0x00200494, 1);
			no_os_mdelay(1);
			ECR8660_write(SPI_RW_EXTERNAL, 0x002004D4, 0X0);
			no_os_mdelay(1);
			temp_data = ECR8660_Read(SPI_RW_EXTERNAL, 0x002004D4, 1);
			no_os_mdelay(1);
			ECR8660_write(SPI_RW_EXTERNAL, 0x002002B0, 0x12345678);
			no_os_mdelay(1);
		}

		while(0)
		{
			// ECR8660 Write/Read
			uint32_t ECR8660_RAddr = 0xF40;
			uint32_t ECR8660_Len = 2;
			uint32_t ECR8660_Rdata = 0;
			uint32_t ECR8660_WAddr = 0xF40;
			uint32_t ECR8660_Wdata = 1;

			// Write/Read 1
			ECR8660_Wdata = 1;
			ECR8660_write(SPI_RW_INTERIOR, ECR8660_WAddr, ECR8660_Wdata);
			ECR8660_Rdata = ECR8660_Read(SPI_RW_INTERIOR, ECR8660_RAddr, ECR8660_Len);

			// Write/Read 3
			ECR8660_Wdata = 3;
			ECR8660_write(SPI_RW_INTERIOR, ECR8660_WAddr, ECR8660_Wdata);
			ECR8660_Rdata = ECR8660_Read(SPI_RW_INTERIOR, ECR8660_RAddr, ECR8660_Len);

			// Write/Read 2
			ECR8660_Wdata = 2;
			ECR8660_write(SPI_RW_INTERIOR, ECR8660_WAddr, ECR8660_Wdata);
			ECR8660_Rdata = ECR8660_Read(SPI_RW_INTERIOR, ECR8660_RAddr, ECR8660_Len);

			/* Check SPI access to register SPI_RW_EXTERNAL */
			ECR8660_write(SPI_RW_EXTERNAL, 0x00, 0x12345678);
			no_os_mdelay(100);
			ECR8660_Rdata = ECR8660_Read(SPI_RW_EXTERNAL, 0x00, 4);
			printf("retWord from SPI_RW_EXTERNAL = 0x%X\n", retWord);
			if(retWord != 0x12345678)
			{
				error_code = SPI_RW_EXTERNAL_FAIL;
				printf("\nECR8660_Read(SPI_RW_EXTERNAL, 0x00, 4) failed.\n");
			}

		}
#endif

		/* eswin_9631 control */
		spi_hook(ad9361_phy->spi);

		/* read page register 0xF40, if return 0x02, spi works correctly! */
		retWord = ECR8660_Read(SPI_RW_INTERIOR, 0xF40, 1);
		if((retWord & 0x03) == 0)
		{
			error_code = PAGE_TEST_REG_FAIL;
			printf("\nECR8660 SPI_RW_INTERIOR failed.\n");
		}
		else
		{
			printf("\nECR8660 SPI_RW_INTERIOR done.\n");
		}
		no_os_mdelay(100);

		/* ECR8660 init */
		if(error_code == NO_ERROR)
		{
			uint8_t initStatus = ECR8660_INIT();
			if(initStatus != 0)
			{
				error_code = ECR8660_INIT_FAIL;
				printf("\nECR8660_INIT() failed.\n");
			}
			else
			{
				printf("\nECR8660_INIT() done.\n");
			}
		}

		if(error_code == NO_ERROR)
		{
			/* Check SPI access to register SPI_RW_EXTERNAL */
			ECR8660_write(SPI_RW_EXTERNAL, 0x00, 0x12345678);
			no_os_mdelay(100);
			retWord = ECR8660_Read(SPI_RW_EXTERNAL, 0x00, 1);
			printf("retWord from SPI_RW_EXTERNAL = 0x%X\n", retWord);
			if(retWord != 0x12345678)
			{
				error_code = SPI_RW_EXTERNAL_FAIL;
				printf("\nECR8660_Read(SPI_RW_EXTERNAL, 0x00, 4) failed.\n");
			}
			else
			{
				printf("\nECR8660_Read(SPI_RW_EXTERNAL, 0x00, 4) done.\n");
			}
			no_os_mdelay(10);
		}

		if(error_code == NO_ERROR)
		{
			/* Download bin file and reset */
			retWord = ECR8660_CodeDownload();
			if(retWord != 0)
			{
				error_code = DOWNLOAD_BIN_FAIL;
				printf("\nECR8660_CodeDownload() failed.\n");
			}
			else
				printf("\nECR8660_CodeDownload() done.\n");
		}
/*
		write 0 0x20004000 0x00020505       //闂佹澘绉堕悿锟� 1T1R_FDD 闁活煉鎷� CH0 婵☆垪锟藉磭纭�闁挎冻鎷� API 闂佹澘绉堕悿鍡涘礃閸涙潙鍔ラ悽顖ょ畱椤旀棃宕仦鐓庘枏闁烩偓鍔戦。鍫曟偝閸ヮ煂浣割嚕閿燂拷
		write 0 0x20004004 0x00050005      //闁革负鍔戦。鍫曟偝閸ヮ煂浣割嚕韫囥儳绀� IForFREQUE=1闁挎稑顦粭鍛村礂椤掑倻绉笲W:20MHz
		write 0 0x20004008 1950000           //閻炴稏鍔庨妵姝硏濡増鍨归崑锟�1950M闁挎稑鑻々褔寮稿锟界粭澶愬绩椤栨稑鐦�10閺夆晜绋戦崺妤冩啺娴ｈ鏆柟杈炬嫹16閺夆晜绋戦崺锟�
		write 0 0x20004018 2350000           //閻炴稏鍔庨妵姝祒濡増鍨归崑锟�2350M闁挎稑鑻々褔寮稿锟界粭澶愬绩椤栨稑鐦�10閺夆晜绋戦崺妤冩啺娴ｈ鏆柟杈炬嫹16閺夆晜绋戦崺锟�
		write 0 0x20004028 0x00008038     //閻炴稏鍔庨妵姝硏濠⒀呭仧濞夛拷8001~8038
		write 0 0x20004030 0x00008023     //閻炴稏鍔庨妵姝祒濠⒀呭仧濞夛拷8001~8023
		write 0 0x00201180 0x11110001     //閻炴稏鍔庨妵锟�1婵☆垪锟藉磭纭�濞戞搩鍘介弻鍥晬瀹�鍐惧晭缂傚喚鍣ｉ敓鑺ュ哺娴滐拷0 A缂佹棏鍨拌ぐ娑欐綇閹惧啿姣�
		delay 100ms
		write 0 0x00201080 0x30000000    //RF闁绘粠鍨板ú鏍晬瀹�鍐惧晭缂傚啰鏁歩t[29]濞戞搫鎷�1
*/
		ECR8660_write(SPI_RW_EXTERNAL, 0x20004000, 0x00020505);
		ECR8660_write(SPI_RW_EXTERNAL, 0x20004004, 0x00050005);
		ECR8660_write(SPI_RW_EXTERNAL, 0x20004008, 1950000);
		ECR8660_write(SPI_RW_EXTERNAL, 0x20004018, 2350000);
		ECR8660_write(SPI_RW_EXTERNAL, 0x20004028, 0x00008038);
		ECR8660_write(SPI_RW_EXTERNAL, 0x20004030, 0x00008023);
		ECR8660_write(SPI_RW_EXTERNAL, 0x00201180, 0x11110001);
		no_os_mdelay(100);
		ECR8660_write(SPI_RW_EXTERNAL, 0x00201080, 0x30000000);
	}

	if((ECR8660_TestItem & ECR8660_FUNC_TEST) == ECR8660_FUNC_TEST)
	{
		status = axi_dmac_init(&tx_dmac, &tx_dmac_init);
		if (status < 0) {
			printf("axi_dmac_init tx init error: %"PRIi32"\n", status);
			return status;
		}
		status = axi_dmac_init(&rx_dmac, &rx_dmac_init);
		if (status < 0) {
			printf("axi_dmac_init rx init error: %"PRIi32"\n", status);
			return status;
		}

#ifdef DMA_EXAMPLE

#if ECR8660_DAC_TEST
	  /* dac init */
		axi_dac_init(&ad9361_phy->tx_dac, &tx_dac_init);
		extern const uint32_t sine_lut_iq[1024];
		axi_dac_set_datasel(ad9361_phy->tx_dac, -1, AXI_DAC_DATA_SEL_DMA);
#if 0
  	axi_dac_load_custom_data(ad9361_phy->tx_dac, sine_lut_iq,
  				 NO_OS_ARRAY_SIZE(sine_lut_iq),
  				 (uintptr_t)dac_buffer);
#else
  	axi_dac_load_custom_data_v2(ad9361_phy->tx_dac, sine_lut_iq, sine_lut_iq,
  				 NO_OS_ARRAY_SIZE(sine_lut_iq),
  				 (uintptr_t)dac_buffer);
#endif
		Xil_DCacheFlush();

		uint32_t samples = 16384;

		/* Transfer the data. */
		axi_dmac_transfer_start(tx_dmac, &transfer);

		/* Flush cache data. */
		Xil_DCacheInvalidateRange((uintptr_t)dac_buffer, sizeof(sine_lut_iq));
		tx_is_transfering = 1u;

		no_os_mdelay(1000);
#endif // end of #if ECR8660_DAC_TEST

#if ECR8660_ADC_TEST
	  	/* adc init */
		axi_adc_init(&ad9361_phy->rx_adc, &rx_adc_init);
   
		/* check data sel is adc */
		for(int ch = 0; ch < rx_adc_init.num_channels; ch++)
		{
			uint8_t data_sel = axi_adc_get_datasel(ad9361_phy->rx_adc, ch);
			printf("data_sel after adc_init for ch-%d = %d\n", ch, data_sel);
			if(data_sel != 0)
			{
				axi_adc_set_datasel(ad9361_phy->rx_adc, ch, 0u);
			}
		}

		/* Read the data from the ADC DMA. */
		axi_dmac_transfer_start(rx_dmac, &read_transfer);

		/* Wait until transfer finishes */
		status = axi_dmac_transfer_wait_completion(rx_dmac, 500);
		if(status < 0)
			return status;

		Xil_DCacheInvalidateRange((uintptr_t)adc_buffer, sizeof(adc_buffer));
		printf("DMA_EXAMPLE: address=%#lx samples=%lu channels=%u bits=%lu\n",
			   (uintptr_t)adc_buffer, NO_OS_ARRAY_SIZE(adc_buffer), rx_adc_init.num_channels,
			   8 * sizeof(adc_buffer[0]));
#endif // end of #if ECR8660_ADC_TEST

#endif //end of #ifdef DMA_EXAMPLE
	}

	printf("\nerror_code = %d\n", error_code);

	while(1)
	{
		parse_spi_command(ad9361_phy->spi);
	}

#endif

	ad9361_remove(ad9361_phy);

#ifdef XILINX_PLATFORM
	Xil_DCacheDisable();
	Xil_ICacheDisable();
#endif

	return 0;
}


void parse_spi_command(struct no_os_spi_desc *spi)
{
	struct xil_uart_init_param platform_uart_init_par = {
		.type = UART_PS,
		.irq_id = UART_IRQ_ID
	};

	struct no_os_uart_init_param uart_param = {
		.device_id = UART_DEVICE_ID,
		.irq_id = UART_IRQ_ID,
		.baud_rate = UART_BAUDRATE,
		.size = NO_OS_UART_CS_8,
		.parity = NO_OS_UART_PAR_NO,
		.stop = NO_OS_UART_STOP_1_BIT,
		.extra = &platform_uart_init_par,
		.platform_ops = &xil_uart_ops
	};

	struct no_os_uart_desc *uart_desc;
#define MAX_SIZE (8192*4)
	uint32_t bytes_number = 10;
	uint8_t wr_data[MAX_SIZE] = {0};
	uint32_t bytes_recv = 0;
	int32_t error = 0;

	uint8_t spi_mode = 0u;
	uint32_t spi_addr = 0;
	uint32_t spi_data = 0;

	error = no_os_uart_init(&uart_desc, &uart_param);

	if(error == 0)
	{
		while(1)
		{
			bytes_number = 10; // length of spi_write and spi_read
			// receive data
			bytes_recv = no_os_uart_read(uart_desc, wr_data, bytes_number);

			if(bytes_recv == bytes_number)
			{
				// spi write
				spi_mode = wr_data[1];
				spi_addr = (wr_data[2] << 3*8) | (wr_data[3] << 2*8) | (wr_data[4] << 1*8) | wr_data[5];
				spi_data = (wr_data[6] << 3*8) | (wr_data[7] << 2*8) | (wr_data[8] << 1*8) | wr_data[9];
				if(wr_data[0] == 0x5A)
				{
#if ECR8660_DEVICE
					ECR8660_write(spi_mode, spi_addr, spi_data);
#endif
				}
				else if(wr_data[0] == 0x5B)
				{
#if ECR8660_DEVICE
					// spi read
					spi_data = ECR8660_Read(spi_mode, spi_addr, 1);
#else
					spi_data = 0xa1b2c3e4;
#endif
					// send data
					wr_data[6] = (spi_data >> 3*8) & 0xff;
					wr_data[7] = (spi_data >> 2*8) & 0xff;
					wr_data[8] = (spi_data >> 1*8) & 0xff;
					wr_data[9] = (spi_data >> 0*8) & 0xff;
					no_os_uart_write(uart_desc, wr_data, bytes_number);
				}
				else if(wr_data[0] == 0x5C)
				{
					bytes_number = (wr_data[1] << 1*8) | (wr_data[2] << 0*8);
					bytes_recv = no_os_uart_read(uart_desc, wr_data, bytes_number);
					if(bytes_number/4 <= 1024)
					{
						for(int sample = 0; sample < 1024; sample++)
						{
							uint32_t iq = (wr_data[sample*4 + 1] << 0) |
										(wr_data[sample*4 + 0] << 8) |
										(wr_data[sample*4 + 3] << 16) |
										(wr_data[sample*4 + 2] << 24);
							zero_lut_iq[sample] = iq;
						}
						/* Reload transfer data memory and transfer the data */
						if(tx_is_transfering == 1u)
						{
							/* Stop tranfering the data. */
							axi_dmac_transfer_stop(tx_dmac);

							/* Reload the waveform */
							axi_dac_load_custom_data_v2(ad9361_phy->tx_dac, zero_lut_iq, zero_lut_iq,
										 NO_OS_ARRAY_SIZE(sine_lut_iq),
										 (uintptr_t)dac_buffer);
							Xil_DCacheFlush();

							/* Transfer the data. */
							transfer.size = bytes_number;
							axi_dmac_transfer_start(tx_dmac, &transfer);

							/* Flush cache data. */
							Xil_DCacheInvalidateRange((uintptr_t)dac_buffer, sizeof(sine_lut_iq));
						}
					}
					no_os_mdelay(10);
				}
				else if(wr_data[0] == 0x5D)
				{
					int32_t status = -1;
#if ECR8660_ADC_TEST
					/* Read the data from the ADC DMA. */
					axi_dmac_transfer_start(rx_dmac, &read_transfer);

					/* Wait until transfer finishes */
					status = axi_dmac_transfer_wait_completion(rx_dmac, 500);

					/* Flush cache data. */
					Xil_DCacheInvalidateRange((uintptr_t)adc_buffer, sizeof(adc_buffer));
#endif
					bytes_number = (wr_data[1] << 1*8) | (wr_data[2] << 0*8);
					if(status < 0)
					{
						memset(wr_data, 0, bytes_number);
					}
					else
					{
						memcpy(wr_data, adc_buffer, bytes_number);
					}
					no_os_uart_write(uart_desc, wr_data, bytes_number);
					no_os_mdelay(10);
				}
			}
		}
	}
	no_os_uart_remove(&uart_desc);
}
