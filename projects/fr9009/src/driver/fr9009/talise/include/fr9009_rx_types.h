/**
 * \file fr9009_rx_types.h
 * \brief Contains FR9009 API Rx datapath data types
 *
 * Copyright 2022 BriRadio.
 * Released under the FR9009 API license, for more information see the "LICENSE.txt" file in this zip file.
 */

#ifndef FR9009_RX_TYPES_H_
#define FR9009_RX_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "fr9009_radioctrl_types.h"

/**
 *  \brief Enum of possible Rx channel enables
 */
typedef enum {
	FR_RXOFF = 0,      /*!< No Rx channels are enabled */
	FR_RX1,            /*!< Rx1 channel enabled */
	FR_RX2,            /*!< Rx2 channel enabled */
	FR_RX1RX2          /*!< Rx1 + Rx2 channels enabled */
} fr9009RxChannels_t;

/**
 *  \brief Enum of possible Observation Rx channels to enable and run calibrations for during init.
 *  Choose ENUM value that enables all channels that will be used in the system.  During system use,
 *  only one channel can be used at a time.  This is also used to alert the ARM processor
 *  which observation channels are valid for the current desired system setup.
 */
typedef enum {
	FR_ORXOFF      = 0x00,
	FR_ORX1        = 0x04,
	FR_ORX2        = 0x08,
	FR_ORX1ORX2    = 0x0C
} fr9009ObsRxChannels_t;

/**
 *  \brief Enum to set the Rx Gain control mode
 */
typedef enum {
	FR_MGC = 0,        /*!< Manual Gain Control USE API*/
	FR_AGCFAST = 1,    /*!< 01 Fast Attack AGC Mode TDD mode */
	FR_AGCSLOW = 2,    /*!< Slow Loop AGC FDD, TDD modes  */
	FR_HYBRID = 3,      /*!< Hybrid AGC Gain Control */
	FR_MGC_PIN = 4,        /*!< Manual Gain Control USE PIN */
} fr9009GainMode_t;

typedef struct {
	uint8_t rxFeGain;
	uint8_t extControl;
	uint8_t adcTiaGain;
	int16_t digGain;
	uint16_t phaseOffset;
} fr9009RxGainTable_t;

typedef struct {
	uint8_t rxFeGain;
	uint8_t extControl;
	uint8_t adcTiaGain;
	int16_t digGain;
} fr9009OrxGainTable_t;

/**
 *  \brief Enum to hold FR9009 Floating Point Formatter number of exponent bits
 */
typedef enum {
	FR_2_EXPONENTBITS = 0,     /*!< Floating point values have 2 exponent bits, 13 significand bits, 1 sign bit */
	FR_3_EXPONENTBITS = 1,     /*!< Floating point values have 3 exponent bits, 12 significand bits, 1 sign bit */
	FR_4_EXPONENTBITS = 2,     /*!< Floating point values have 4 exponent bits, 11 significand bits, 1 sign bit */
	FR_5_EXPONENTBITS = 3      /*!< Floating point values have 5 exponent bits, 10 significand bits, 1 sign bit  */
} fr9009FpExponentModes_t;

/**
 *  \brief Enum to hold FR9009 Floating Point Formatter rounding modes for the Rx data path
 */
typedef enum {
	FR_ROUND_TO_EVEN = 0,           /*!< Round floating point ties to an even value          */
	FR_ROUNDTOWARDS_POSITIVE = 1,   /*!< Round floating point toward the positive direction  */
	FR_ROUNDTOWARDS_NEGATIVE = 2,   /*!< Round floating point toward the negative direction  */
	FR_ROUNDTOWARDS_ZERO = 3,       /*!< Round floating point toward the zero direction      */
	FR_ROUND_FROM_EVEN = 4          /*!< Round floating point ties away from even value      */
} fr9009FpRoundModes_t;

/**
 *  \brief Enum to hold FR9009 Rx attenuation values that are used when the
 *         floating point format is enabled
 */
typedef enum {
	FR_FPATTEN_24DB = 4,      /*!< Set Rx attenuation to 24 dB when Rx Data format set to floating point mode  */
	FR_FPATTEN_18DB = 5,      /*!< Set Rx attenuation to 18 dB when Rx Data format set to floating point mode  */
	FR_FPATTEN_12DB = 6,      /*!< Set Rx attenuation to 12 dB when Rx Data format set to floating point mode */
	FR_FPATTEN_6DB = 7,       /*!< Set Rx attenuation to 6 dB when Rx Data format set to floating point mode */
	FR_FPATTEN_0DB = 0,            /*!< Set Rx attenuation to 0 dB when Rx Data format set to floating point mode */
	FR_FPATTEN_MINUS6DB = 1,            /*!< Set Rx attenuation to -6 dB when Rx Data format set to floating point mode */
	FR_FPATTEN_MINUS12DB = 2,           /*!< Set Rx attenuation to -12 dB when Rx Data format set to floating point mode */
	FR_FPATTEN_MINUS18DB = 3            /*!< Set Rx attenuation to -18 dB when Rx Data format set to floating point mode */
} fr9009FpAttenSteps_t;

/**
 *  \brief Enum to select the four Data Formatting options
 */
typedef enum {
	FR_GAIN_COMPENSATION_DISABLED = 0,  /*!< Gain Compensation and Data Formatting are disabled */
	FR_GAIN_WITH_FLOATING_POINT,        /*!< Gain Compensation enabled with floating point data formatting enabled and internal slicer enabled */
	FR_GAIN_WITH_INTSLICER_NOGPIO,      /*!< Gain Compensation enabled with integer data formatting and internal slicer enabled with no GPIO Slicer Position output */
	FR_GAIN_WITH_INTSLICER,             /*!< Gain Compensation enabled with integer data formatting and internal slicer enabled with GPIO Slicer Position output */
	FR_GAIN_WITH_EXTERNAL_SLICER        /*!< Gain Compensation enabled with integer data formatting and external slicer enabled */
} fr9009DataFormattingModes_t;

/**
 *  \brief Enum to hold FR9009 Gain Slicer external pin gain step size.  Slicer gain in this mode is determined by multiplying the step size by the step size selection from the base band processor.
 */
typedef enum {
	FR_EXTSLICER_STEPSIZE_1DB = 0,   /*!< Set Gain Slicer External gain step size to 1dB  */
	FR_EXTSLICER_STEPSIZE_2DB,       /*!< Set Gain Slicer External gain step size to 2dB  */
	FR_EXTSLICER_STEPSIZE_3DB,       /*!< Set Gain Slicer External gain step size to 3dB  */
	FR_EXTSLICER_STEPSIZE_4DB        /*!< Set Gain Slicer External gain step size to 4dB  */
} fr9009GainStepSize_t;

/**
 *  \brief Enum to hold FR9009 Rx1 Gain Slicer external pin GPIO selection options
 */
typedef enum {
	FR_EXTSLICER_RX1_GPIO0_1_2 = 0,      /*!< Select Rx1 Gain Slicer External GPIO0, GPIO1, GPIO2 */
	FR_EXTSLICER_RX1_GPIO5_6_7,          /*!< Select Rx1 Gain Slicer External GPIO5, GPIO6, GPIO7 */
	FR_EXTSLICER_RX1_GPIO8_9_10,         /*!< Select Rx1 Gain Slicer External GPIO8, GPIO9, GPIO10 */
	FR_EXTSLICER_RX1_GPIO_DISABLE        /*!< Select Rx1 Disable Gain Slicer External GPIO */
} fr9009Rx1ExtSlicerGpioSelect_t;

/**
*  \brief Enum to hold FR9009 Rx2 Gain Slicer external pin GPIO selection options
*/
typedef enum {
	FR_EXTSLICER_RX2_GPIO11_12_13 = 0,   /*!< Select Rx2 Gain Slicer External GPIO11, GPIO12, GPIO13 */
	FR_EXTSLICER_RX2_GPIO5_6_7 = 1,      /*!< Select Rx2 Gain Slicer External GPIO5, GPIO6, GPIO7 */
	FR_EXTSLICER_RX2_GPIO_DISABLE = 3    /*!< Select Rx2 Disable Gain Slicer External GPIO */
} fr9009Rx2ExtSlicerGpioSelect_t;

/**
 *  \brief Enum to hold FR9009 integer mode number of embedded slicer bits and positions
 */
typedef enum {
	FR_NO_EMBEDDED_SLICER_BITS = 0,       /*!< Disabled all embedded slicer bits  */
	FR_EMBED_1_SLICERBIT_AT_MSB,          /*!< Embeds 1 slicer bits on I and 1 slicer bits on Q and the MSB position in the data frame */
	FR_EMBED_1_SLICERBIT_AT_LSB,          /*!< Embeds 1 slicer bits on I and 1 slicer bits on Q and the LSB position in the data frame */
	FR_EMBED_2_SLICERBITS_AT_MSB,         /*!< Embeds 2 slicer bits on I and 2 slicer bits on Q and the MSB position in the data frame */
	FR_EMBED_2_SLICERBITS_AT_LSB          /*!< Embeds 2 slicer bits on I and 2 slicer bits on Q and the LSB position in the data frame */
} fr9009EmbeddedBits_t;

/**
 *  \brief Enum to hold FR9009 integer sample resolution
 */
typedef enum {
	FR_INTEGER_12BIT_2SCOMP = 0,    /*!< Selects integer sample 12 bit resolution with 2s compliment    */
	FR_INTEGER_12BIT_SIGNED,        /*!< Selects integer sample 12 bit resolution with signed magnitude */
	FR_INTEGER_16BIT_2SCOMP,        /*!< Selects integer sample 16 bit resolution with 2s compliment    */
	FR_INTEGER_16BIT_SIGNED,        /*!< Selects integer sample 16 bit resolution with signed magnitude */
	FR_INTEGER_24BIT_2SCOMP,        /*!< Selects integer sample 24 bit resolution with 2s compliment    */
	FR_INTEGER_24BIT_SIGNED         /*!< Selects integer sample 24 bit resolution with signed magnitude */
} fr9009IntSampleResolution_t;

/**
*  \brief Data structure to hold FR9009 Floating Point Formatter Configuration
*/
typedef struct {
	fr9009DataFormattingModes_t formatSelect; /*!< Rx Channel format mode selects */

	/* Float Config Settings */
	fr9009FpRoundModes_t	fpRoundMode;         /*!< Rounding mode for floating point format (See enum values) */
	uint8_t fpDataFormat;                     /*!< If floating point format is enabled in formatSelect member, this sets the 16 bit output from MSB to LSB, 1 = {Sign, Significand, Exponent}, 0 = {Sign, Exponent, Significand} */
	uint8_t fpEncodeNan;                      /*!< 1 =  encodes the highest value of Exponent to mean NaN (Not a Number) to be compatible to IEEE754 specification (Valid: 0 or 1) */
	fr9009FpExponentModes_t	fpNumExpBits;     /*!< Indicates the number of exponent and significand bits in the floating point number */
	uint8_t fpHideLeadingOne;                 /*!< 1 =  Hides the leading one in significand to be compatible to the IEEE754 specification. 0 = a leading one exists at the MSB of the significand.  (Valid: 0, 1) */
	fr9009FpAttenSteps_t	fpRx1Atten;          /*!< Rx1 - Attenuate integer data when floating point mode enabled, see enum for values from 0dB to 42dB in 6dB steps */
	fr9009FpAttenSteps_t	fpRx2Atten;          /*!< Rx2 - Attenuate integer data when floating point mode enabled, see enum for values from 0dB to 42dB in 6dB steps */

	/* Integer Config Settings */
	fr9009EmbeddedBits_t	intEmbeddedBits;             /*!< Integer number of embedded bits and position */
	fr9009IntSampleResolution_t	intSampleResolution;  /*!< Integer sample resolution selecting either 12, 16, 14 bit modes with signed or 2s Complement */

	/* Slicer Config Settings */
	fr9009GainStepSize_t	extPinStepSize;          /*!< Enum selects the external pin gain step size */
	fr9009Rx1ExtSlicerGpioSelect_t	rx1GpioSelect; /*!< Enum selects the Rx1 GPIO Configuration */
	fr9009Rx2ExtSlicerGpioSelect_t	rx2GpioSelect; /*!< Enum selects the Rx2 GPIO Configuration */
	uint8_t externalLnaGain;                      /*!< Selects Slicer to compensate for external dualband LNA {0 - disabled, 1 - enabled */
	uint8_t tempCompensationEnable;               /*!< Selects Slicer to compensate for temperature variations {0 - disabled, 1 - enabled */
} fr9009RxDataFormat_t;

/**
 *  \brief Data structure to hold FR9009 Rx Gain Control Pin Configuration
 */
typedef struct {
	uint8_t incStep;				/*!< Increment in gain index applied when the increment gain pin is pulsed. A value of 0 to 7 applies a step size of 1 to 8 */
	uint8_t decStep;				/*!< Decrement in gain index applied when the increment gain pin is pulsed. A value of 0 to 7 applies a step size of 1 to 8 */
	fr9009GpioPinSel_t	rxGainIncPin;/*!< GPIO used for the Increment gain input: Rx1 : FR_GPIO_00 or FR_GPIO_10, Rx2 : FR_GPIO_03 or FR_GPIO_13*/
	fr9009GpioPinSel_t	rxGainDecPin;/*!< GPIO used for the Decrement gain input: Rx1 : FR_GPIO_01 or FR_GPIO_11, Rx2 : FR_GPIO_04 or FR_GPIO_14*/
	uint8_t enable;                 /*!< Enable (1) or disable (0) the gain pin control*/
} fr9009RxGainCtrlPin_t;

/**
 *  \brief Data structure to hold FR9009 Rx dualband LNA gain table entries
 */
typedef struct {
	uint8_t dualbandControl;    /*!< The external control value to be output on the 3.3V GPIO?s to control the LNA (values 0-3). */
	uint8_t dualbandGain;       /*!< The gain compensation value for the corresponding external control, used for RSSI and gain compensation.
                                     Range of 0 to 63 (0 to +31.5db in 0.5db steps). */
} fr9009DualBandLnaGainTable_t;

typedef enum {
	FR_RX1_NCO1A = 0,
	FR_RX1_NCO1B,
	FR_RX1_NCO2A,
	FR_RX1_NCO2B,

	FR_RX2_NCO1A,
	FR_RX2_NCO1B,
	FR_RX2_NCO2A,
	FR_RX2_NCO2B
} fr9009RxNcoChannel_t;

#ifdef __cplusplus
}
#endif

#endif /* FR9009_RX_TYPES_H_ */
