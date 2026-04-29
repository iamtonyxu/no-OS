/**
 * \file fr9009_gpio_types.h
 * \brief Contains functions to allow control of the General Purpose IO functions on the FR9009 device
 *
 * Copyright 2022 BriRadio.
 * Released under the FR9009 API license, for more information see the "LICENSE.txt" file in this zip file.
 */

#ifndef FR9009_GPIO_TYPES_H_
#define FR9009_GPIO_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "fr9009_jesd204_types.h"

/**
 *  \brief Enum to set the low voltage GPIO mode
 */
typedef enum{
	FR_GPIO_RX_MANUAL_GAIN_CONTROL_MODE_1 = 1,			//Rx Manual Gain control  //input only
	FR_GPIO_TX_ATT_CONTROL_MODE_2	= 2,				//Tx Attenuation control & OrxEnable , input only
	FR_GPIO_BITBANG_IO_MODE_3		= 3,				//FR_GPIO_BITBANG_MODE = 3,      GPIO的输出由 usr_set_gpio_out和usr_set_gpio_oe(高电平有效)的相应比特决定。
	FR_GPIO_UART_MODE_5			= 5,				//UART mode （此种模式下只有各组中最低位[0,4,8,12]为输出，驱动源为UART_TX，而第bit[2,6,10,14]为UART_RX输入，对于GPIO[19:16]不支持此模式）
	FR_GPIO_PROG_EVENT_IO_MODE_6	= 6,				//prog_event_in_out ,输出由TDD模块中的 prg_event_out, prg_event_gpio_oe决定
	FR_GPIO_AGC_MONITOR_MODE_7		= 7,				//monitor输出模式，monitor的数据来自AGC模块，对于[3:0]/[11:8]，输出的是monitor[3:0]，对于[7:4]/[15:12]，输出的是monitor[7:4]，对于[18:16]使出的是monitor_data[6:4]
	FR_GPIO_ARM_OUT_MODE_9			= 9,				//FR_GPIO_ARM_OUT_MODE = 9,   上位机透过spi_sysctrl模块配置arm_gpio_out, arm_gpio_oe来控制相应比特输出，并回读相应的管脚电平
	FR_GPIO_SLICER_OUT_MODE_10		=10,				//FR_GPIO_SLICER_OUT_MODE，[3:0]/[7:4]/[18:16]不支持，[10:8]输出ch1_slicer_out[2:0]，[14:12]输出ch2_slicer_out[2:0]
	FR_GPIO_TEST_OUT_MODE_13		=13,				//test_out，GPIO[15:0]分别对应test_out[15:0]
	FR_GPIO_SPI2_MODE_14			=14,				//SPI2 仅对GPIO[3:0]这个组有效，GPIO_0: SPI2_DIO/MOSI (支持双向模式)GPIO_1: SPI2_DOUT GPIO_2: SPI2_CLK GPIO_3: SPI2_CS_N 
	FR_GPIO_PWM_MODE_15			=15,				//pwm输出模式, bit[0]/bit[4]/bit[8]/bit[12]为pwm1的输出，bit[9]/bit[13]为pwm2的输出
	FR_GPIO_MODE_INVALID		
}fr9009GpioMode_t;
	
/**
 *  \brief Enum to set the GPIO3v3 mode
 */	
typedef enum{
	FR_GPIO3V3_LEVELTRANSLATE_MODE	=1, 		//Level translate mode. Signal level on low voltage GPIO pins are level shifted to 3.3 V. gpio_out, gpio_oe_pre
	FR_GPIO3V3_INVLEVELTRANSLATE_MODE=2, 		//Inverted level translate mode. Inverse of signal levels on low voltage GPIO pins are outputs. ~gpio_out, gpio_oe_pre
	FR_GPIO3V3_BITBANG_MODE=3, 				//Manual control mode. When enabled, use FR9009_setGpio3v3PinLevel() to control the logic output level of a pin. arm_gpio_3v3_out, arm_gpio_3v3_oe
	FR_GPIO3V3_EXTATTEN_LUT_MODE=4, 		//This mode configures specific 3.3 V GPIO pins to output the 4-bit, external TX attenuator control word for the selected gain index. gpio_3v3[3:0] = {ch2_ext_tx_dsa[3:2],ch1_ext_tx_dsa[3:2]},[7:4] = ch1_ext_tx_dsa,[11:8] = ch2_ext_tx_dsa
	FR_GPIO3V3_ANALOG_MODE=7, 				//gpio_3v3_ie_pre[3:0]	= ~arm_gpio_3v3_ana_en[3:0];
	FR_GPIO3V3_EXTGAIN_LUT_MODE=8, 		//This mode configures specific 3.3 V GPIO pins to output the 4-bit, external RX attenuator control word for the selected gain index. gpio_3v3[3:0] = {ch2_rx_rf_dsa[3:2],ch1_rx_rf_dsa[3:2]},[7:4] = ch1_rx_rf_dsa,[11:8] = ch2_rx_rf_dsa
}fr9009Gpio3v3Mode_t;


/**
 *  \brief Enum of unique error codes for the FR9009 GPIO API functions.
 * Each error condition in the library should get its own enum value to allow
 * easy debug of errors.
 */
typedef enum {
	FR9009_ERR_GPIO_OK = 0,
	FR9009_ERR_MONITOR_OUT_INDEX_RANGE,
	FR9009_ERR_GETGPIOMON_INDEX_NULL_PARM,
	FR9009_ERR_GETGPIOMON_MONITORMASK_NULL_PARM,
	FR9009_ERR_GETGPIO_OE_NULL_PARM,
	FR9009_ERR_GPIO_OE_INV_PARM,
	FR9009_ERR_GPIO_SRC_INV_PARM,
	FR9009_ERR_GETGPIO_SRC_NULL_PARM,
	FR9009_ERR_GPIO_LEVEL_INV_PARM,
	FR9009_ERR_GETGPIO_LEVEL_NULL_PARM,
	FR9009_ERR_GETGPIO_SETLEVEL_NULL_PARM,
	FR9009_ERR_SETUPAUXDAC_NULL_PARM,
	FR9009_ERR_SETUPAUXDAC_INV_10BIT_AUXDACCODE,
	FR9009_ERR_SETUPAUXDAC_INV_12BIT_AUXDACCODE,
	FR9009_ERR_WRITEAUXDAC_INV_10BIT_AUXDACCODE,
	FR9009_ERR_WRITEAUXDAC_INV_12BIT_AUXDACCODE,
	FR9009_ERR_WRITEAUXDAC_INV_AUXDACINDEX,
	FR9009_ERR_SETUPAUXDAC_INV_RESOLUTION,
	FR9009_ERR_GPIO3V3_OE_INV_PARM,
	FR9009_ERR_GETGPIO3V3_OE_NULL_PARM,
	FR9009_ERR_GPIO3V3_SRC_INV_PARM,
	FR9009_ERR_GETGPIO3V3_SRC_NULL_PARM,
	FR9009_ERR_GPIO3V3_LEVEL_INV_PARM,
	FR9009_ERR_GETGPIO3V3_LEVEL_NULL_PARM,
	FR9009_ERR_GETGPIO3V3_SETLEVEL_NULL_PARM,
	FR9009_ERR_GPINT_OK,
	FR9009_ERR_GPINT_STATUS_NULL_PARM,
	FR9009_ERR_GPINT_GPINTDIAG_NULL_PARM,
	FR9009_ERR_GPINT_NO_SOURCE_FOUND,
	FR9009_ERR_GPINT_SOURCE_NOT_IMPLEMENTED,
	FR9009_ERR_GPINT_CLKPLL_UNLOCKED,
	FR9009_ERR_GPINT_RFPLL_UNLOCKED,
	FR9009_ERR_GPINT_AUXPLL_UNLOCKED,
	FR9009_ERR_GPINT_ARM_WATCHDOG_TIMEOUT,
	FR9009_ERR_GPINT_ARM_FORCE_GPINT,
	FR9009_ERR_GPINT_ARM_SYSTEM_ERROR,
	FR9009_ERR_GPINT_ARM_DATA_PARITY_ERROR,
	FR9009_ERR_GPINT_ARM_PROG_PARITY_ERROR,
	FR9009_ERR_GPINT_ARM_CALIBRATION_ERROR,
	FR9009_ERR_GPINT_FRAMERA,
	FR9009_ERR_GPINT_DEFRAMERA,
	FR9009_ERR_GPINT_FRAMERB,
	FR9009_ERR_GPINT_DEFRAMERB,
	FR9009_ERR_GPINT_PA_PROTECT_CH1,
	FR9009_ERR_GPINT_PA_PROTECT_CH2,
	FR9009_ERR_GPINT_STREAM_ERROR,
	FR9009_ERR_SETAUXADCPINMODEGPIO_INV_GPIO,
	FR9009_ERR_SETAUXADCPINMODEGPIO_GPIO_IN_USE,
	FR9009_ERR_STARTAUXADC_INV_CHANNEL,
	FR9009_ERR_STARTAUXADC_INV_MODE,
	FR9009_ERR_STARTAUXADC_INV_NUM_SAMPLES,
	FR9009_ERR_STARTAUXADC_INV_SAMPLING_PERIOD,
	FR9009_ERR_STARTAUXADC_NULL_PARAM,
	FR9009_ERR_READAUXADC_NULL_PARAM,
	FR9009_ERR_SETORXTXPINSEL_NULL_PARAM,
	FR9009_ERR_SETORXTXPINSEL_INV_PIN,
	FR9009_ERR_GETORXTXPINSEL_NULL_PARAM,
	FR9009_ERR_SETRXMGCSEL_NULL_PARAM,
	FR9009_ERR_SETRXMGCSEL_INV_PIN,
	FR9009_ERR_GETRXMGCSEL_NULL_PARAM,
	FR9009_ERR_SETORXMGCSEL_NULL_PARAM,
	FR9009_ERR_SETORXMGCSEL_INV_PIN,
	FR9009_ERR_GETORXMGCSEL_NULL_PARAM,
	FR9009_ERR_SETTXATTNSEL_NULL_PARAM,
	FR9009_ERR_SETTXATTNSEL_INV_PIN,
	FR9009_ERR_GETTXATTNSEL_NULL_PARAM,
	FR9009_ERR_SETTXATTNSTATE_NULL_PARAM,
	FR9009_ERR_SETTXATTNSTATE_INV_PIN,
	FR9009_ERR_GETTXATTNSTATE_NULL_PARAM,
	FR9009_ERR_GETPWMPERIOD_NULL_PARAM,
	FR9009_ERR_SETPWMENABLE_INV_VAL,
	FR9009_ERR_GETPWMENABLE_NULL_PARAM,
	FR9009_ERR_GETRXCFG_NULL_PARAM,
	FR9009_ERR_SETORXENABLEGPIOSEL_NULL_PARAM,
	FR9009_ERR_SETORXENABLEGPIOSEL_INV_VAL,
	FR9009_ERR_GETORXENABLEGPIOSEL_NULL_PARAM,
	FR_ERR_GPIO_NUMBER_OF_ERRORS /* Keep this ENUM last as a reference to the total number of error enum values */
} fr9009GpioErr_t;

/**
 * \brief Enumeration for 10bit AuxDAC voltage for center DAC code (code 512).
 */
typedef enum {
	FR_AUXDACVREF_1V = 0, /*!< AuxDAC reference at 1V */
	FR_AUXDACVREF_1P5V = 1, /*!< AuxDAC reference at 1.5V */
	FR_AUXDACVREF_2V = 2, /*!< AuxDAC reference at 2V */
	FR_AUXDACVREF_2P5V = 3 /*!< AuxDAC reference at 2.5V */
} fr9009AuxDacVref_t;

/**
 * \brief Enumeration for AuxDAC resolution modes.
 */
typedef enum {
	FR_AUXDACRES_12BIT = 0, /*!< 12bit DAC resolution for a subset of the output voltage range centered around VREF */
	FR_AUXDACRES_11BIT = 1, /*!< 11bit DAC resolution for a subset of the output voltage range centered around VREF */
	FR_AUXDACRES_10BIT = 2  /*!< 10bit DAC resolution for 100mv to 3v range */
} fr9009AuxDacResolution_t;
/**
 * \brief Enumeration for SPI Sel .
 */
typedef enum {
	SPI1_SEL = 0, /*!< select spi 1 */
	SPI2_SEL = 1, /*!< select spi 2 */
} SPISel_t;
/**
 * \brief Data structure to hold the auxiliary DAC settings
 */
typedef struct {
	uint16_t auxDacEnables;         /*!< Aux DAC enable bit for each DAC, where the first ten bits correspond to the 10-bit DACs, and the next consecutive two bits enable the 12-bit DACs */
	uint16_t auxDacValues[10];      /*!< Aux DAC values for each 10-bit DAC correspond to the first 10 array elements */
} fr9009AuxDac_t;

/**
 * \brief General Purpose interrupt mask types
 */
typedef enum {
	FR_GP_MASK_STREAM_ERROR            = 0x1000,   /*!< Stream processor error GP Interrupt mask bit */
	FR_GP_MASK_ARM_CALIBRATION_ERROR   = 0x0800,   /*!< ARM calibration error GP Interrupt mask bit */
	FR_GP_MASK_ARM_SYSTEM_ERROR        = 0x0400,   /*!< ARM System error GP Interrupt mask bit */
	FR_GP_MASK_ARM_FORCE_INTERRUPT     = 0x0200,   /*!< ARM force GP Interrupt mask bit */
	FR_GP_MASK_WATCHDOG_TIMEOUT        = 0x0100,   /*!< Watchdog GP Interrupt mask bit */
	FR_GP_MASK_PA_PROTECTION_TX2_ERROR = 0x0080,   /*!< Tx2 PA protection error GP Interrupt mask bit */
	FR_GP_MASK_PA_PROTECTION_TX1_ERROR = 0x0040,   /*!< Tx1 PA protection error GP Interrupt mask bit */
	FR_GP_MASK_JESD_DEFRMER_IRQ        = 0x0020,   /*!< JESD204B Deframer IRQ error GP Interrupt mask bit */
	FR_GP_MASK_JESD_FRAMER_IRQ         = 0x0010,   /*!< JESD204B Framer IRQ error GP Interrupt mask bit */
	FR_GP_MASK_CLK_SYNTH_UNLOCK        = 0x0008,   /*!< Device clock PLL non-lock error GP Interrupt mask bit */
	FR_GP_MASK_AUX_SYNTH_UNLOCK        = 0x0004,   /*!< Auxiliary PLL non-lock error GP Interrupt mask bit */
	FR_GP_MASK_RF_SYNTH_UNLOCK         = 0x0002    /*!< RF PLL non-lock error GP Interrupt mask bit */
} fr9009GpIntMask_t;

#define FR_GPMASK_MSB (uint16_t)(FR_GP_MASK_STREAM_ERROR | \
                                  FR_GP_MASK_ARM_CALIBRATION_ERROR | \
                                  FR_GP_MASK_ARM_SYSTEM_ERROR | \
                                  FR_GP_MASK_ARM_FORCE_INTERRUPT | \
                                  FR_GP_MASK_WATCHDOG_TIMEOUT)

#define FR_GPMASK_LSB (uint16_t)(FR_GP_MASK_PA_PROTECTION_TX1_ERROR | \
                                  FR_GP_MASK_PA_PROTECTION_TX2_ERROR | \
                                  FR_GP_MASK_JESD_DEFRMER_IRQ | \
                                  FR_GP_MASK_JESD_FRAMER_IRQ | \
                                  FR_GP_MASK_CLK_SYNTH_UNLOCK | \
                                  FR_GP_MASK_AUX_SYNTH_UNLOCK | \
                                  FR_GP_MASK_RF_SYNTH_UNLOCK)
/**
 * \brief GPIO settings for SPI2 TxAtten select
 */
typedef enum {
	FR_SPI2_TXATTEN_GPIO4   = 0x00,    /*!< Select GPIO4 for SPI2 Tx Attenuation select */
	FR_SPI2_TXATTEN_GPIO8   = 0x01,    /*!< Select GPIO8 for SPI2 Tx Attenuation select */
	FR_SPI2_TXATTEN_GPIO14  = 0x02,    /*!< Select GPIO14 for SPI2 Tx Attenuation select */
	FR_SPI2_TXATTEN_DISABLE = 0x03     /*!< Disable GPIO for SPI2 Tx Attenuation select */
} fr9009Spi2TxAttenGpioSel_t;


/**
 * \brief gpIntHandler diagnostic structure
 */
typedef struct {
	uint8_t data[9];                /*!< All GP_INT sources */
	fr9009FramerSel_t
	framer;       /*!< Interrupting framer, valid only for framer sources */
	fr9009DeframerSel_t
	deframer;   /*!< Interrupting deframer, valid only for deframer sources */
	int32_t deframerInputsMask;     /*!< Interrupting deframer input mask (bit per deframer input), valid only for deframer sources (valid 0x0-0xF)
                                         deframerInputsMask is the deframer lane after the deframer lane crossbar swapping (lane input of the deframer) */
} fr9009GpIntInformation_t;

/**
 * \brief FR9009 Aux ADC Channel types
 */
typedef enum {
	FR_AUXADC_CH0 = 0,  /*!< Select Aux ADC Channel 0 for sampling and conversion*/
	FR_AUXADC_CH1 = 1,  /*!< Select Aux ADC Channel 1 for sampling and conversion*/
	FR_AUXADC_CH2 = 2,  /*!< Select Aux ADC Channel 2 for sampling and conversion*/
	FR_AUXADC_CH3 = 3   /*!< Select Aux ADC Channel 3 for sampling and conversion*/
} fr9009AuxAdcChannels_t;

/**
 * \brief FR9009 Aux ADC modes for sampling
 */
typedef enum {
	FR_AUXADC_NONPIN_MODE = 0,  /*!< Select Aux ADC sampling and conversion in Non-Pin mode (ARM Internal timer is used for sampling and conversion)*/
	FR_AUXADC_PIN_MODE    = 1   /*!< Select Aux ADC sampling and conversion in Pin mode (Pulses on ARM GPIO Input pins are used to schedule sampling and conversion)*/
} fr9009AuxAdcModes_t;

/**
 * \brief FR9009 Aux ADC configuration structure
 */
typedef struct {
	fr9009AuxAdcChannels_t
	auxAdcChannelSel;  /*!< Selects the channel which is supposed to sample AuxADC input for A/D conversion */
	fr9009AuxAdcModes_t
	auxAdcMode;        /*!< Selects mode to latch and store conversion results */
	uint16_t
	numSamples;        /*!< No. of A/D conversions to be performed in range 1 - 1000 */
	uint16_t
	samplingPeriod_us; /*!< Sampling interval time in microseconds (Minimum 15us) NOTE: Valid only for non pin mode. Ignored for pin mode. */
} fr9009AuxAdcConfig_t;

/**
 * \brief FR9009 Aux ADC conversion result structure
 */
typedef struct {
	uint16_t auxAdcCodeAvg;     /*!< 12-bit Average of AuxADC A/D conversion samples */
	uint16_t numSamples;        /*!< No. of samples averaged in AuxAdcCodeAvg */
	uint8_t  completeIndicator; /*!< Flag to indicate if a scheduled AuxADC conversion completed. 1 - AuxADC Conversion Complete, 0 - AuxADC Conversion Incomplete */
} fr9009AuxAdcResult_t;


#ifdef __cplusplus
}
#endif

#endif /* FR9009_GPIO_TYPES_H_ */
