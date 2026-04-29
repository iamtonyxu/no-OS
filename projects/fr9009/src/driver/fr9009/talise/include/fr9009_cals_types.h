/**
 * \file fr9009_cals_types.h
 * \brief Contains FR9009 API Calibration data types
 *
 * Copyright 2022 BriRadio.
 * Released under the FR9009 API license, for more information see the "LICENSE.txt" file in this zip file.
 */

#ifndef FR9009_CALS_TYPES_H_
#define FR9009_CALS_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "fr9009_arm_spi_cmd.h"
typedef enum
{
    // FR_TX_BB_FILTER            = 0x00000001,   /*!< Tx BB filter calibration */
    // FR_ADC_TUNER               = 0x00000002,   /*!< ADC tuner calibration */
    // FR_TIA_3DB_CORNER          = 0x00000004,   /*!< TIA 3dB corner calibration */
    FR_DC_OFFSET               = FR_CALI_RX_DC_OFFSET | FR_CALI_ORX_DC_OFFSET,   /*!< DC offset calibration */
    // FR_TX_ATTENUATION_DELAY    = 0x00000010,   /*!< Tx attenuation delay calibration */
    // FR_RX_GAIN_DELAY           = 0x00000020,   /*!< Rx gain delay calibration */
    // FR_FLASH_CAL               = 0x00000040,   /*!< Flash converter comparator calibration */
    FR_PATH_DELAY              = FR_CALI_PATH_DELAY,   /*!< Path delay equalization calibration */
    FR_TX_LO_LEAKAGE_INTERNAL  = FR_CALI_TX_LO_CALI,   /*!< Internal Tx LO leakage calibration */
    FR_TX_LO_LEAKAGE_EXTERNAL  = FR_CALI_TX_LO_CALI | FR_CALI_TX_INIT_EXTERNAL,   /*!< External Tx LO leakage calibration */
    FR_TX_QEC_INIT             = FR_CALI_TX_QEC_INIT,   /*!< Tx quadrature error correction calibration */
    // FR_LOOPBACK_RX_LO_DELAY    = 0x00000800,   /*!< Loopback Rx LO delay path calibration */
    // FR_LOOPBACK_RX_RX_QEC_INIT = 0x00001000,   /*!< Loopback Rx quadrature error correction calibration */
    FR_RX_LO_DELAY             = FR_CALI_RX_LO_DELAY,   /*!< Rx LO delay path calibration */
    FR_RX_QEC_INIT             = FR_CALI_RX_QEC_INIT,   /*!< Rx quadrature error correction calibration */
    // FR_RX_PHASE_CORRECTION     = 0x00008000,   /*!< Rx Phase correction calibration */
    FR_ORX_LO_DELAY            = FR_CALI_ORX_LO_DELAY,   /*!< ORx LO delay path calibration */
    FR_ORX_QEC_INIT            = FR_CALI_ORX_QEC_INIT,   /*!< ORx quadrature error correction calibration */
    // FR_TX_DAC                  = 0x00040000,   /*!< Tx DAC passband calibration */
    // FR_ADC_STITCHING           = 0x00080000,   /*!< ADC stitching calibration */
    FR_FHM_CALS                = FR_CALI_FHM_CALS,    /*!< FHM (Fast Frequency Hopping Mode) Calibrations */
	FR_TX_FLATTEN_INIT         = FR_CALI_TX_FLATTEN_INIT,   /*!< TX flatten calibration */
	FR_RX_FLATTEN_INIT         = FR_CALI_RX_FLATTEN_INIT,   /*!< RX flatten calibration */
	FR_ORX_FLATTEN_INIT        = FR_CALI_ORX_FLATTEN_INIT,   /*!< ORX flatten calibration */
    FR_TX_INIT_INTERNAL_LP2    = 0x00200000,   /*!< TX calibration internal loopback to path2, path1 if no use this mask*/
    FR_TX_INIT_EXTERNAL        = FR_CALI_TX_INIT_EXTERNAL,   /*!< TX calibration external loopback*/
} fr9009InitCalibrations_t;

typedef enum
{
    FR_TRACK_NONE              = 0x00000000,   /*!< Disable all tracking calibrations */
    FR_TRACK_RX1_QEC           = 0x00000001,   /*!< Rx1 quadrature error correction tracking calibration */
    FR_TRACK_RX2_QEC           = 0x00000002,   /*!< Rx2 quadrature error correction tracking calibration */
    FR_TRACK_ORX1_QEC          = 0x00000004,   /*!< ORx1 quadrature error correction tracking calibration */
    FR_TRACK_ORX2_QEC          = 0x00000008,   /*!< ORx2 quadrature error correction tracking calibration */
    FR_TRACK_TX1_LOL           = 0x00000010,   /*!< Tx1 LO leakage tracking calibration */
    FR_TRACK_TX2_LOL           = 0x00000020,   /*!< Tx2 LO leakage tracking calibration */
    FR_TRACK_TX1_QEC           = 0x00000040,   /*!< Tx1 quadrature error correction tracking calibration */
    FR_TRACK_TX2_QEC           = 0x00000080,   /*!< Tx2 quadrature error correction tracking calibration */
    // FR_TRACK_RX1_HD2           = 0x00000100,   /*!< Rx1 HD2 error correction tracking calibration */
    // FR_TRACK_RX2_HD2           = 0x00000200,   /*!< Rx2 HD2 error correction tracking calibration */
    FR_TRACK_ALL               = 0x000003FF    /*!< ENUM specifies all tracking cals */
} fr9009TrackingCalibrations_t;

/**
 *  \brief Enum of possible wait events to use with FR9009_waitForEvent()
 */
typedef enum
{
    FR_CLKPLLCP = 0,               /*!< CLK PLL CP wait event */
    FR_CLKPLL_LOCK,                /*!< CLK PLL lock wait event */
    FR_RFPLLCP,                    /*!< RF PLL CP wait event */
    FR_RFPLL_LOCK,                 /*!< RF PLL lock wait event */
    FR_AUXPLLCP,                   /*!< AUX PLL CP wait event */
    FR_AUXPLL_LOCK,                /*!< AUX PLL LOCK wait event */
    FR_ARMBUSY                     /*!< ARM busy wait event */
} fr9009WaitEvent_t;

/**
 *  \brief Enum of possible Rx Digital DC Offset channels
 */
typedef enum
{
    FR_DC_OFFSET_RX_CHN = 0,
    FR_DC_OFFSET_ORX_CHN = 1
} fr9009DcOffsetChannels_t;

/**
 *  \brief Enum of Rx/ORx channels mask  for configuring (Enable /disable) DC offsets.
 */
typedef enum
{
    FR_DC_OFFSET_ALL_OFF = 0x00,               /*!< Disable all the channels */
    FR_DC_OFFSET_RX1 = 0x01,                   /*!< Enables Rx1  */
    FR_DC_OFFSET_RX2 = 0x02,                   /*!< Enables Rx2  */
    FR_DC_OFFSET_ORX1 = 0x04,                   /*!< Enables ORx1  */
    FR_DC_OFFSET_ORX2 = 0x08,                   /*!< Enables ORx2  */
    FR_DC_OFFSET_ALL_ON = 0x0F              /*!< Enables all the channels  */
}fr9009RxDcOffsettEn_t;

    /**
     *  \brief Enum of Tracking Calibration Batch Size in Micro Seconds 
     */
    typedef enum
    {
        FR_TRACK_BATCH_SIZE_500_US = 0,
        FR_TRACK_BATCH_SIZE_200_US = 1
    } fr9009TrackingCalBatchSize_t;

/**
 * \brief Data structure to hold Tx LOL Status
 */
typedef struct
{
    uint32_t errorCode;         /*!< error code from Tx LOL */
    uint32_t percentComplete;   /*!< percent of required data collected for the current cal. Range 0 to 100 */
    uint32_t varianceMetric;    /*!< metric of how well the tracking cal is performing */
    uint32_t iterCount;         /*!< running counter that increments each time the cal runs to completion */
    uint32_t updateCount;       /*!< running counter that increments each time the cal updates the correction/actuator hardware */
} fr9009TxLolStatus_t;

/**
 * \brief Data structure to hold Tx QEC Status
 */
typedef struct
{
    uint32_t errorCode;         /*!< error code from Tx QEC */
    uint32_t percentComplete;   /*!< percent of required data collected for the current cal. Range 0 to 100 */
    uint32_t correctionMetric;  /*!< metric of how well the tracking cal is performing */
    uint32_t iterCount;         /*!< running counter that increments each time the cal runs to completion */
    uint32_t updateCount;       /*!< running counter that increments each time the cal updates the correction/actuator hardware */
} fr9009TxQecStatus_t;

/**
 * \brief Data structure to hold Rx QEC Status
 */
typedef struct
{
    uint32_t errorCode;         /*!< error code from Rx QEC */
    uint32_t percentComplete;   /*!< percent of required data collected for the current cal. Range 0 to 100 */
    uint32_t selfcheckIrrDb;    /*!<  */
    uint32_t iterCount;         /*!< running counter that increments each time the cal runs to completion */
    uint32_t updateCount;       /*!< running counter that increments each time the cal updates the correction/actuator hardware */
} fr9009RxQecStatus_t;

/**
 * \brief Data structure to hold Orx QEC Status
 */
typedef struct
{
    uint32_t errorCode;         /*!< error code from Orx QEC */
    uint32_t percentComplete;   /*!< percent of required data collected for the current cal. Range 0 to 100 */
    uint32_t selfcheckIrrDb;    /*!<  */
    uint32_t iterCount;         /*!< running counter that increments each time the cal runs to completion */
    uint32_t updateCount;       /*!< running counter that increments each time the cal updates the correction/actuator hardware */
} fr9009OrxQecStatus_t;

/**
 * \brief Data structure to hold Rx HD2 Status
 */
typedef struct
{
    uint32_t errorCode;         /*!< error code from HD2 */
    uint32_t percentComplete;   /*!< NOT APPLICABLE FOR HD2  Dynamic condition of this cal does not allow for convergence */
    uint32_t confidenceLevel;   /*!< Confidence level that cal has accurately indentified the coefficient required to cancel HD2  */
                                /*!<     0 - No Observation, 1 - 7 indicates increasing confidence level */
    uint32_t iterCount;         /*!< running counter that increments each time the cal runs to completion */
    uint32_t updateCount;       /*!< running counter that increments each time the cal updates the correction/actuator hardware */
} fr9009RxHd2Status_t;

/**
 * \brief Data structure to hold Rx HD2 calibration config settings
 *
 * Note: Set posSideBandSel to the side (negative - lower side band or
 * positive - upper side band) that has the desired
 * signal.  Correction will be correcting the 2nd harmonic distortion on the
 * same side of the spectrum.
 */
typedef struct
{
    uint32_t posSideBandSel;     /*!< 1 = Correct HD2 of desired signal on the positive (upper) side of the complex FFT,
                                      0 = correct HD2 in the negative (lower)side of the complex FFT */
} fr9009RxHd2Config_t;

#ifdef __cplusplus
}
#endif

#endif /* FR9009_CALS_TYPES_H_ */
