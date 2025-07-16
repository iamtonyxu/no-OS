/**
* \file adi_adrv9025_external_dpd_types.h
* \brief Contains ADRV9025 external DPD data types
*
* ADRV9025 API Version: $ADI_ADRV9025_API_VERSION$
*/

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the ADRV9025 API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

#ifndef _ADI_ADRV9025_EXTERNAL_DPD_TYPES_H_
#define _ADI_ADRV9025_EXTERNAL_DPD_TYPES_H_

#include "adi_adrv9025_tx_types.h"
#include "adi_adrv9025_gpio_types.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ADRV9025_MAX_EXT_DPD_MODEL_MEMORY_TERM_SIZE 32
#define ADRV9025_MAX_EXT_DPD_MODEL_CROSS_TERM_SIZE  8
#define ADRV9025_MAX_EXT_DPD_CAPTURE_SAMPLE_SIZE 4096

/**
* \brief Enumerated list of external DPD capture type select
*/
typedef enum adi_adrv9025_ExtDpdCaptureType
{
    ADI_ADRV9025_EXT_DPD_CAPTURE_IMMEDIATE_TRIGGER      = 0,    /*!< In this mode, capture buffer will start to fill immediately */
    ADI_ADRV9025_EXT_DPD_CAPTURE_POWER_LEVEL_TRIGGER    = 1,    /*!< In this mode, capture is delayed until the power threshold is reached */
    ADI_ADRV9025_EXT_DPD_CAPTURE_PEAK_DETECTION         = 2,    /*!< Initiates a new capture sequence whenever a higher peak is detected within the peak capture window specified by adi_adrv9025_ExtDpdCaptureCtrl_t.dpdPeakCaptureConfig.extDpdPeakWinCnt
                                                                     after a capture is triggered via adi_adrv9025_ExtDpdCaptureStartTriggerSet() API. This mode can be used in both TDD and FDD operations. */
    ADI_ADRV9025_EXT_DPD_CAPTURE_TDD_TXON_SYNC          = 4,    /*!< Initiates a new capture sequence on the next rising edge of TDD Tx-ON cycle after a capture is triggered via adi_adrv9025_ExtDpdCaptureStartTriggerSet() API. The target
                                                                     Tx channel is configured via adi_adrv9025_ExtDpdCaptureCtrlSet() API */
    ADI_ADRV9025_EXT_DPD_CAPTURE_EXTENDED_PEAK_SEARCH     = 8     /*!< Initiates a new capture sequence whenever a higher peak is detected within the extended peak capture window specified by adi_adrv9025_ExtDpdCaptureCtrl_t.dpdPeakCaptureConfig.extDpdPeakExtendedWindowUs
                                                                     after a capture is triggered via adi_adrv9025_ExtDpdCaptureStartTriggerSet() API. This mode allows user to perform a peak capture within multiple Tx frames time period in TDD mode. */
} adi_adrv9025_ExtDpdCaptureType_e;

/**
* \brief Enumerated list of external DPD capture Tx observation points
*/
typedef enum adi_adrv9025_ExtDpdCaptureTxObsSel
{
    ADI_ADRV9025_EXT_DPD_CAPTURE_TX_OBS_POST_DPD_ACTUATOR   = 0,    /*!< Sets the Tx observation point for capture as post DPD actuator */
    ADI_ADRV9025_EXT_DPD_CAPTURE_TX_OBS_PRE_DPD_ACTUATOR    = 1     /*!< Sets the Tx observation point for capture as pre DPD actuator */
} adi_adrv9025_ExtDpdCaptureTxObsSel_e;

/**
* \brief Enumerated list of external DPD capture Tx alternate observation points
*/
typedef enum adi_adrv9025_ExtDpdCaptureTxAltObsSel
{
    /*!< Sets the Tx alternate observation point for capture as pre/post DPD actuator. The observation
    point for the aternate Tx capture depends on Tx observation select adi_adrv9025_ExtDpdCaptureCtrl_t.dpdCaptureTxObsSel.
    If adi_adrv9025_ExtDpdCaptureCtrl_t.dpdCaptureTxObsSel is set to pre-dpd actuator, then this enum selects the alternate
    Tx buffer capture point as post-dpd actuator. If adi_adrv9025_ExtDpdCaptureCtrl_t.dpdCaptureTxObsSel is set to post-dpd actuator,
    then this enum selects the alternate Tx buffer capture point as pre-dpd actuator. Please refer to the documentation of
    adi_adrv9025_ExtDpdTxAlternateCaptureDataGet() API for a detailed description*/
    ADI_ADRV9025_EXT_DPD_CAPTURE_TX_ALT_OBS_DPD_ACTUATOR    = 0,
    ADI_ADRV9025_EXT_DPD_CAPTURE_TX_ALT_OBS_PRE_CFR         = 1     /*!< Sets the Tx alternate observation point for capture as pre CFR Tx data */
} adi_adrv9025_ExtDpdCaptureTxAltObsSel_e;

/**
* \brief Enumerated list of no. of samples to capture for external DPD adaptation
*/
typedef enum adi_adrv9025_ExtDpdCaptureSize
{
    /*!< Sets the capture size of DPD Tx and ORx sample capture data to 32 samples each of I and Q data for external adaptation */
    ADI_ADRV9025_EXT_DPD_CAPTURE_SIZE_32_SAMPLES    = 7,
    /*!< Sets the capture size of DPD Tx and ORx sample capture data to 64 samples each of I and Q data for external adaptation */
    ADI_ADRV9025_EXT_DPD_CAPTURE_SIZE_64_SAMPLES    = 6,
    /*!< Sets the capture size of DPD Tx and ORx sample capture data to 128 samples each of I and Q data for external adaptation */
    ADI_ADRV9025_EXT_DPD_CAPTURE_SIZE_128_SAMPLES   = 5,
    /*!< Sets the capture size of DPD Tx and ORx sample capture data to 256 samples each of I and Q data for external adaptation */
    ADI_ADRV9025_EXT_DPD_CAPTURE_SIZE_256_SAMPLES   = 4,
    /*!< Sets the capture size of DPD Tx and ORx sample capture data to 512 samples each of I and Q data for external adaptation */
    ADI_ADRV9025_EXT_DPD_CAPTURE_SIZE_512_SAMPLES   = 3,
    /*!< Sets the capture size of DPD Tx and ORx sample capture data to 1024 samples each of I and Q data for external adaptation */
    ADI_ADRV9025_EXT_DPD_CAPTURE_SIZE_1024_SAMPLES  = 2,
    /*!< Sets the capture size of DPD Tx and ORx sample capture data to 2048 samples each of I and Q data for external adaptation */
    ADI_ADRV9025_EXT_DPD_CAPTURE_SIZE_2048_SAMPLES  = 1,
    /*!< Sets the capture size of DPD Tx and ORx sample capture data to 4096 samples each of I and Q data for external adaptation */
    ADI_ADRV9025_EXT_DPD_CAPTURE_SIZE_4096_SAMPLES  = 0
} adi_adrv9025_ExtDpdCaptureSize_e;

/**
* \brief Enumerated list of target capture power level bands to select from for capturing Tx and ORx data for external DPD adaptation
*/
typedef enum adi_adrv9025_ExtDpdCaptureBand
{
    ADI_ADRV9025_EXT_DPD_CAPTURE_POWER_BAND_0 = 0,  /*!< Selects capture power level band 0 */
    ADI_ADRV9025_EXT_DPD_CAPTURE_POWER_BAND_1 = 1,  /*!< Selects capture power level band 1 */
    ADI_ADRV9025_EXT_DPD_CAPTURE_POWER_BAND_2 = 2,  /*!< Selects capture power level band 2 */
    ADI_ADRV9025_EXT_DPD_CAPTURE_POWER_BAND_3 = 3   /*!< Selects capture power level band 3 */
} adi_adrv9025_ExtDpdCaptureBand_e;

/**
* \brief Enumerated list of target capture power level bands to select from for capturing Tx and ORx data for external DPD adaptation
*/
typedef enum adi_adrv9025_ExtDpdPwrThreshBandPair
{
    ADI_ADRV9025_EXT_DPD_POWER_BAND_01 = 0, /*!< Selects power thresh band pair 01 */
    ADI_ADRV9025_EXT_DPD_POWER_BAND_12 = 1, /*!< Selects power thresh band pair 12 */
    ADI_ADRV9025_EXT_DPD_POWER_BAND_23 = 2  /*!< Selects power thresh band pair 23 */
} adi_adrv9025_ExtDpdPwrThreshBandPair_e;

/**
* \brief Enumerated list of direction in which the target power level is reached for triggering capture of Tx and ORx data
*/
typedef enum adi_adrv9025_ExtDpdCaptureBandDir
{
    ADI_ADRV9025_EXT_DPD_CAPTURE_BAND_REACHED_ANY_ORDER     = 0,    /*!< In this mode, capture is triggered regardless of whether the target power level was reached from a higher or lower level */
    ADI_ADRV9025_EXT_DPD_CAPTURE_BAND_REACHED_ASCENDING     = 2,    /*!< In this mode, capture is triggered only if the target power level was reached from a lower band */
    ADI_ADRV9025_EXT_DPD_CAPTURE_BAND_REACHED_DESCENDING    = 3     /*!< In this mode, capture is triggered only if the target power level was reached from a higher band */
} adi_adrv9025_ExtDpdCaptureBandDir_e;

/**
* \brief Enumerated list of options to select the resolution of pulse width counter in terms of DPD sample clock
*/
typedef enum adi_adrv9025_ExtDpdCapturePulseWidthRes
{
    ADI_ADRV9025_EXT_DPD_CAPTURE_PW_RES_1X          = 0,    /*!< Pulse width counter increments every DPD clock */
    ADI_ADRV9025_EXT_DPD_CAPTURE_PW_RES_256X        = 1,    /*!< Pulse width counter increments once every 256 DPD clocks */
    ADI_ADRV9025_EXT_DPD_CAPTURE_PW_RES_65536X      = 2,    /*!< Pulse width counter increments once every 2^16 DPD clocks */
    ADI_ADRV9025_EXT_DPD_CAPTURE_PW_RES_16777216X   = 3     /*!< Pulse width counter increments once every 2^24 DPD clocks */
} adi_adrv9025_ExtDpdCapturePulseWidthRes_e;

/**
* \brief Enumerated list of options to select dpd capture peak detection input
*/
typedef enum adi_adrv9025_ExtDpdCapturePeakDetectInputSel
{
    ADI_ADRV9025_EXT_DPD_CAPTURE_PEAK_DET_DPD_INPUT = 0,    /*!< Use DPD actuator input data as the source for peak detection */
    ADI_ADRV9025_EXT_DPD_CAPTURE_PEAK_DET_CFR_INPUT = 1     /*!< Use CFR engine input data as the source for peak detection */
} adi_adrv9025_ExtDpdCapturePeakDetectInputSel_e;

/**
* \brief Enumerated list of options to select external DPD model for programming
*/
typedef enum adi_adrv9025_ExtDpdModelSel
{
    ADI_ADRV9025_EXT_DPD_MODEL0 = 0,    /*!< Selects DPD Model 0 */
    ADI_ADRV9025_EXT_DPD_MODEL1 = 1,    /*!< Selects DPD Model 1 */
    ADI_ADRV9025_EXT_DPD_MODEL2 = 2,    /*!< Selects DPD Model 2 */
    ADI_ADRV9025_EXT_DPD_MODEL3 = 3     /*!< Selects DPD Model 3 */
} adi_adrv9025_ExtDpdModelSel_e;

/**
* \brief Enumerated list of options to select external DPD LUT for programming
*/
typedef enum adi_adrv9025_ExtDpdLut
{
    ADI_ADRV9025_EXT_DPD_LUT0 = 0,      /*!< Selects DPD LUT 0 */
    ADI_ADRV9025_EXT_DPD_LUT1 = 1,      /*!< Selects DPD LUT 1 */
    ADI_ADRV9025_EXT_DPD_LUT2 = 2,      /*!< Selects DPD LUT 2 */
    ADI_ADRV9025_EXT_DPD_LUT3 = 3,      /*!< Selects DPD LUT 3 */
    ADI_ADRV9025_EXT_DPD_LUT4 = 4,      /*!< Selects DPD LUT 4 */
    ADI_ADRV9025_EXT_DPD_LUT5 = 5,      /*!< Selects DPD LUT 5 */
    ADI_ADRV9025_EXT_DPD_LUT6 = 6,      /*!< Selects DPD LUT 6 */
    ADI_ADRV9025_EXT_DPD_LUT7 = 7,      /*!< Selects DPD LUT 7 */
    ADI_ADRV9025_EXT_DPD_LUT8 = 8,      /*!< Selects DPD LUT 8 */
    ADI_ADRV9025_EXT_DPD_LUT9 = 9,      /*!< Selects DPD LUT 9 */
    ADI_ADRV9025_EXT_DPD_LUT10 = 10,    /*!< Selects DPD LUT 10 */
    ADI_ADRV9025_EXT_DPD_LUT11 = 11,    /*!< Selects DPD LUT 11 */
    ADI_ADRV9025_EXT_DPD_LUT12 = 12,    /*!< Selects DPD LUT 12 */
    ADI_ADRV9025_EXT_DPD_LUT13 = 13,    /*!< Selects DPD LUT 13 */
    ADI_ADRV9025_EXT_DPD_LUT14 = 14,    /*!< Selects DPD LUT 14 */
    ADI_ADRV9025_EXT_DPD_LUT15 = 15,    /*!< Selects DPD LUT 15 */
    ADI_ADRV9025_EXT_DPD_LUT16 = 16,    /*!< Selects DPD LUT 16 */
    ADI_ADRV9025_EXT_DPD_LUT17 = 17,    /*!< Selects DPD LUT 17 */
    ADI_ADRV9025_EXT_DPD_LUT18 = 18,    /*!< Selects DPD LUT 18 */
    ADI_ADRV9025_EXT_DPD_LUT19 = 19,    /*!< Selects DPD LUT 19 */
    ADI_ADRV9025_EXT_DPD_LUT20 = 20,    /*!< Selects DPD LUT 20 */
    ADI_ADRV9025_EXT_DPD_LUT21 = 21,    /*!< Selects DPD LUT 21 */
    ADI_ADRV9025_EXT_DPD_LUT22 = 22,    /*!< Selects DPD LUT 22 */
    ADI_ADRV9025_EXT_DPD_LUT23 = 23,    /*!< Selects DPD LUT 23 */
    ADI_ADRV9025_EXT_DPD_LUT24 = 24,    /*!< Selects DPD LUT 24 */
    ADI_ADRV9025_EXT_DPD_LUT25 = 25,    /*!< Selects DPD LUT 25 */
    ADI_ADRV9025_EXT_DPD_LUT26 = 26,    /*!< Selects DPD LUT 26 */
    ADI_ADRV9025_EXT_DPD_LUT27 = 27,    /*!< Selects DPD LUT 27 */
    ADI_ADRV9025_EXT_DPD_LUT28 = 28,    /*!< Selects DPD LUT 28 */
    ADI_ADRV9025_EXT_DPD_LUT29 = 29,    /*!< Selects DPD LUT 29 */
    ADI_ADRV9025_EXT_DPD_LUT30 = 30,    /*!< Selects DPD LUT 30 */
    ADI_ADRV9025_EXT_DPD_LUT31 = 31,    /*!< Selects DPD LUT 31 */
    ADI_ADRV9025_EXT_DPD_NUM_LUTS
} adi_adrv9025_ExtDpdLut_e;

/**
* \brief Enumerated list of options to select external DPD LUT bank for programming
*/
typedef enum adi_adrv9025_ExtDpdLutBank
{
    ADI_ADRV9025_EXT_DPD_LUT_BANK0 = 0, /*!< Selects DPD LUT read bank 0 - valid for 8bit and 9 bit compander sizes*/
    ADI_ADRV9025_EXT_DPD_LUT_BANK1 = 1, /*!< Selects DPD LUT read bank 1 - valid only for 8 bit compander */
    ADI_ADRV9025_EXT_DPD_LUT_BANK2 = 2, /*!< Selects DPD LUT read bank 2 - valid for 8bit and 9 bit compander sizes*/
    ADI_ADRV9025_EXT_DPD_LUT_BANK3 = 3  /*!< Selects DPD LUT read bank 3 - valid only for 8 bit compander */
} adi_adrv9025_ExtDpdLutBank_e;

/**
* \brief Enumerated list of options to select external DPD LUT model compander size for programming
*/
typedef enum adi_adrv9025_ExtDpdActuatorCompanderSize
{
    ADI_ADRV9025_EXT_DPD_ACT_COMPANDER_8_BITS = 0,  /*!< Selects an 8 bit compander. 4 banks / LUT memory for an 8 bit compander */
    ADI_ADRV9025_EXT_DPD_ACT_COMPANDER_9_BITS = 1   /*!< Selects a 9 bit compander. 2 banks / LUT memory for a 9 bit compander */
} adi_adrv9025_ExtDpdActuatorCompanderSize_e;

/**
* \brief Enumerated list of external DPD Errors
*/
typedef enum adi_adrv9025_ExtDpdError
{
    ADI_ADRV9025_EXT_DPD_NO_ERROR           = 0,
    ADI_ADRV9025_EXT_DPD_HW_IN_USE_ERROR    = 0x1401,   /*!< Error code to convey that DPD hardware is being used by other cals */
    ADI_ADRV9025_EXT_DPD_STREAM_ERROR       = 0x1402,   /*!< Error code to convey that there is an error from stream processor */
    ADI_ADRV9025_EXT_DPD_CAPTURE_ERROR      = 0x1403    /*!< Error code to convey that a Tx transition occurs during capture */
} adi_adrv9025_ExtDpdError_e;

/**
* \brief Data structure to hold External DPD peak capture mode configurations
*/
typedef struct adi_adrv9025_ExtDpdPeakCaptureConfig
{
    adi_adrv9025_ExtDpdCapturePeakDetectInputSel_e extDpdPeakCaptureInputSel;   /*!< Capture Peak Detection Window Count */
    uint32_t                                       extDpdPeakWinCnt;            /*!< Capture Peak Detection Window Count. A maximum count of 2^24 is supported at DPD clock rate */
    uint8_t extDpdPeakDetectIIREnable;                                          /*!< 0 - Disables the peak detection IIR for sample by sample peak detection
                                                                                     1 - Enables the peak detection IIR and can be used to average samples for peak detection */
    uint8_t extDpdPeakDetectIIRBypass;                                          /*!< 1 - Bypass peak detection IIR for sample by sample peak detection, 0 - Peak Detection IIR active if enabled */
    uint8_t extDpdPeakDecay;                                                    /*!< Capture Peak Detection IIR Decay Rate */
    uint32_t extDpdPeakExtendedWindowUs;                                          /*!< Extended capture window size across multiple tx frames in microseconds. Max allowed value is 15sec?. This variable is used only when
                                                                                     capture type(adi_adrv9025_ExtDpdCaptureCtrl_t.dpdCaptureTypeSel) is selected ADI_ADRV9025_EXT_DPD_CAPTURE_EXTENDED_PEAK_SEARCH*/
} adi_adrv9025_ExtDpdPeakCaptureConfig_t;

/**
* \brief Data structure to hold External DPD capture control configurations
*/
typedef struct adi_adrv9025_ExtDpdCaptureCtrl
{
    adi_adrv9025_TxChannels_e        extDpdCapturetxChannelSel;         /*!< Configuration to select Tx channel to capture data from for external DPD adaptation */
    adi_adrv9025_ExtDpdCaptureType_e dpdCaptureTypeSel;                 /*!< Configuration to select if capture trigger is immediate,power level, peak detection, TDD Tx On sync triggered or extended peak search */
    adi_adrv9025_ExtDpdCaptureTxObsSel_e dpdCaptureTxObsSel;            /*!< Configuration to select the dpd capture Tx observation point (Pre DPD actuator vs Post DPD actuator) */
    adi_adrv9025_ExtDpdCaptureTxAltObsSel_e dpdCaptureTxAltObsSel;      /*!< Configuration to select the dpd capture Tx alternate observation point (Pre/Post DPD actuator vs Pre CFR ) */
    adi_adrv9025_ExtDpdCaptureSize_e dpdCaptureSize;                    /*!< User can select a capture size between 32 and 4096 samples specified by this member for external DPD adaptation */
    adi_adrv9025_ExtDpdPeakCaptureConfig_t dpdPeakCaptureConfig;        /*!< Configuration for peak detect capture mode. This config can be used to setup a search window count for detecting peaks,
                                                                            peak detection input select and peak capture IIR filter decay rate */
  
    uint16_t  extDpdCaptureAlignDelay;                              /*!< Delay count(in units of DPD sample clock) to delay start of capture post triggering. This is useful in a peak 
                                                                            detect capture mode (adi_adrv9025_ExtDpdCaptureCtrl_t.dpdCaptureTypeSel = ADI_ADRV9025_EXT_DPD_CAPTURE_PEAK_DETECTION)
                                                                            to align the Tx capture buffer (Tx obs buffer) and ORx capture buffer data peak according to the equation 
                                                                            pathdelay fifo's delay(adi_adrv9025_ExtDpdCaptureConfig_t.pathDelay) + dpd actuator delay(12 samples) - capture alignment delay(adi_adrv9025_ExtDpdCaptureCtrl_t.extDpdCaptureAlignDelay) */
    uint32_t extDpdCaptureDelay;                                        /*!< Delay count(in units of DPD sample clock) to reject initial transient response while capturing. It is recommended that
                                                                            this value be set to 0 */
} adi_adrv9025_ExtDpdCaptureCtrl_t;

/**
* \brief Data structure to hold External DPD capture level configurations
*/
typedef struct adi_adrv9025_ExtDpdCaptureLevel
{
    uint16_t extDpdCaptureTimeOut;                          /*!< A timeout counter value(in units of dpd clock cycles) that can be used to prevent an unreachable target band causing the trigger to never fire */
    adi_adrv9025_ExtDpdCaptureBand_e dpdCaptureBandSel;     /*!< Target DPD capture band if extDpdCaptureCtrl.dpdCaptureTypeSel is set to ADI_ADRV9025_EXT_DPD_CAPTURE_POWER_LEVEL_TRIGGER */
    adi_adrv9025_ExtDpdCaptureBandDir_e dpdCaptureBandDir; /*!< Setting to select how the DPD capture target power band is reached */
}  adi_adrv9025_ExtDpdCaptureLevel_t;

/**
* \brief Data structure to hold External DPD capture power level thresholds
*/
typedef struct adi_adrv9025_ExtDpdCapturePwrThresh
{
    uint8_t envelopePwrLowToHighLimit;                                          /*!< Threshold for the selected band transition from a lower band aligned to MSB 8 bits of I^2 + Q^2 to trigger data capture in the band */
    uint8_t envelopePwrHighToLowLimit;                                          /*!< Threshold for the selected band transition from a higher band aligned to MSB 8 bits of I^2 + Q^2 to trigger data capture in the band */
    adi_adrv9025_ExtDpdCapturePulseWidthRes_e envelopePwrPulseWidthResolution;  /*!< Pulse width resolution counter resolution from 1x, 256x, 65536x or 2^24x DPD clocks*/
    uint16_t envelopePwrLowToHighPulseWidth;                                    /*!< Minimum pulse width for band transition to a higher band */
    uint16_t envelopePwrHighToLowPulseWidth;                                    /*!< Minimum pulse width for band transition to a lower band */
} adi_adrv9025_ExtDpdCapturePwrThresh_t;

/**
* \brief Data structure to hold Tx-ORx capture external path delay
*/
typedef struct adi_adrv9025_ExtDpdCapturePathDelay
{
    uint8_t extDpdCaptureFifoDelay;          /*!< Tx-ORx capture path FIFO delay */
    uint8_t extDpdCaptureInterpolationIndex; /*!< Tx-ORx capture path Interpolation Index */
} adi_adrv9025_ExtDpdCapturePathDelay_t;

/**
* \brief Data structure to hold all DPD actuator gain monitoring thresholds
*/
typedef struct adi_adrv9025_ExtDpdActGainMonitorThresh
{
    uint16_t extDpdGainMonitorQualThresh;  /*!< DPD Power Monitor Qualifying Threshold Limit. 
                                                      This threshold corresponds to the MSB 16 bits of (I^2 + Q^2) 32 bit data */
    uint8_t  extDpdGainMonitorLowerThresh; /*!< DPD Gain Monitor Minimum Gain Threshold Limit. 
                                                      A threshold violation is triggered if DPD_OUT < (DPD_IN x extDpdGainMonitorLowerThresh) 
                                                      which can be used to auto load a DPD model specified by adi_adrv9025_ExtDpdActGainMonitorThresh_t.extDpdGainMonitorLowerThreshModelSel.
                                                      The gain value is specified in frac 2.6 fixed point format. */
    uint8_t  extDpdGainMonitorUpperThresh; /*!< DPD Gain Monitor Maximum Gain Threshold Limit.
                                                      A threshold violation is triggered if DPD_OUT > (DPD_IN x extDpdGainMonitorUpperThresh)
                                                      which can be used to auto load a DPD model specified by adi_adrv9025_ExtDpdActGainMonitorThresh_t.extDpdGainMonitorUpperThreshModelSel.
                                                      The gain value is in frac 2.6 fixed point format. */
} adi_adrv9025_ExtDpdActGainMonitorThresh_t;

/**
* \brief Data structure to hold all DPD actuator gain monitor control configuration
*/
typedef struct adi_adrv9025_ExtDpdActGainMonitorCtrl
{
    uint8_t                       extDpdGainMonitorEn;                  /*!< 1 - DPD Gain monitoring enable, 0 - DPD Gain monitoring disable */
    adi_adrv9025_ExtDpdModelSel_e extDpdGainMonitorLowerThreshModelSel; /*!< DPD model select when a DPD output gain underrange is detected. 
                                                                              If gain monitoring is enabled, a threshold violation is triggered if DPD_OUT < (DPD_IN x adi_adrv9025_ExtDpdActGainMonitorThresh_t.extDpdGainMonitorLowerThresh) 
                                                                              which can be used to auto load a model specified by this param */
    adi_adrv9025_ExtDpdModelSel_e extDpdGainMonitorUpperThreshModelSel; /*!< DPD model select when a DPD output gain overrange is detected.
                                                                              If gain monitoring is enabled A threshold violation is triggered if DPD_OUT > (DPD_IN x adi_adrv9025_ExtDpdActGainMonitorThresh_t.extDpdGainMonitorUpperThresh) 
                                                                              which can be used to auto load a model specified by this param */
    uint8_t                       extDpdGainMonitorIIRDecay;            /*!< DPD Power Monitor IIR Decay rate in the range of 0-15. Controls the decay rate of the DPD power monitor IIR filters to average samples for gain threshold violation detection 
                                                                               A value of 0 written to this param exercises sample by sample gain threshold violation detection if gain monitoring is enabled */
} adi_adrv9025_ExtDpdActGainMonitorCtrl_t;

/**
* \brief Data structure to hold the DPD actuator Gain Monitoring control and threshold configurations
*/
typedef struct adi_adrv9025_ExtDpdActGainMonitorConfig
{
    uint32_t txChannelMask;                                             /*!< A mask consisting of the OR'ed Tx channels for which the gain monitor configuration is requested to be applied */
    adi_adrv9025_ExtDpdActGainMonitorCtrl_t   extDpdGainMonitorCtrl;    /*!< Gain monitoring control can be used to enable/disable gain monitoring, 
                                                                            setup gain overrange and underrange model selects and setup the gain monitoring IIR decay rate  */
    adi_adrv9025_ExtDpdActGainMonitorThresh_t extDpdGainMonitorThresh;  /*!< Sets up the gain monitoring threshold qualifying limit, upper gain threshold and the lower gain threshold */
} adi_adrv9025_ExtDpdActGainMonitorConfig_t;

/**
* \brief Data structure to hold all external DPD capture related configurations
*/
typedef struct adi_adrv9025_ExtDpdCaptureConfig
{
    adi_adrv9025_GpioPinSel_e extDpdCaptureTriggerPin;              /*!< GPIO input Pin to ADRV9025 to trigger Tx-ORx data capture for external DPD adaptation. The GPIO can be set to ADI_ADRV9025_GPIO_INVALID for triggering Tx-ORx capture through an API command */
    adi_adrv9025_GpioPinSel_e extDpdCaptureDoneStatusPin;           /*!< GPIO output Pin from ADRV9025 to indicate Tx-ORx data capture complete. The GPIO can be set to ADI_ADRV9025_GPIO_INVALID for monitoring capture complete through an API function */
    adi_adrv9025_ExtDpdCapturePathDelay_t pathDelay;                /*!< DPD capture path delay settings for the Tx-ORx external path delay. The path delay can be estimated by running the external path delay init cal and can be retrieved via
                                                                        adi_adrv9025_ExtPathDelayGet() API. It is recommended that the users run their own path delay cal to estimate the path delay. */
    adi_adrv9025_ExtDpdCaptureCtrl_t      extDpdCaptureCtrl;        /*!< Control config parameters for Tx-ORx DPD data capture */
    adi_adrv9025_ExtDpdCaptureLevel_t     extDpdCaptureLevel;       /*!< Control config parameters for Tx-ORx DPD data capture Level */
    adi_adrv9025_ExtDpdCapturePwrThresh_t extDpdCapPwrThreshBand01; /*!< Capture level threshold settings for power band 0-1. Ignored if capture type is immediate trigger */
    adi_adrv9025_ExtDpdCapturePwrThresh_t extDpdCapPwrThreshBand12; /*!< Capture level threshold settings for power band 1-2. Ignored if capture type is immediate trigger */
    adi_adrv9025_ExtDpdCapturePwrThresh_t extDpdCapPwrThreshBand23; /*!< Capture level threshold settings for power band 2-3. Ignored if capture type is immediate trigger */
    uint16_t envelopePowerDecay;                                    /*!< Power Envelope Decay Rate. Controls the decay rate of the envelope IIR feedback function. 0=slowest decay of 1LSB,  0xFFFF=fastest decay of fullscale (FS). For decay rate setting of n, the feedback gain = (FS-(n+1))/FS */
} adi_adrv9025_ExtDpdCaptureConfig_t;

/**
* \brief Data structure to hold status related information for external DPD data capture
*/
typedef struct adi_adrv9025_ExtDpdCaptureDetailedStatus
{
    adi_adrv9025_ExtDpdError_e extDpdCaptureErrorCode;       /*!< Reports any error code encountered during external DPD operation */
    uint8_t                    extDpdCaptureTimeoutStatus;  /*!< Indicates timeout was reached before target count was reached. Timeout can be configured as part of capture level settings. 1 - Timeout encountered, 0 - Previous capture completed before timeout */
    uint8_t  extDpdCaptureBandTransitionCount;              /*!< Transition count for transient bands reached during buffer capture */
    uint8_t  extDpdCapturePowerBandStatus;                  /*!< Power Band Registered at capture buffer trigger */
    uint16_t extDpdCaptureCount;                            /*!< Count for the initial groups of samples that remains in target power band */
} adi_adrv9025_ExtDpdCaptureDetailedStatus_t;

/**
* \brief Data structure to hold the external DPD model config params. The GMP Model to LUT mapping is shown here
*    
*       j' <= 8 |  LUT0  |  LUT1  |  LUT2  |  LUT3  |  LUT4  |  LUT5  |  LUT6  |  LUT7  |  LUT26  |  LUT27  |
*               | (i<=8) | (i<=8) | (i<=10)| (i<=11)| (i<=12)| (i<=13)| (i<=14)| (i<=15)| (i<=15) | (i<=15) |
*     ----------|--------|--------|--------|--------|--------|--------|--------|--------|(Roaming |(Roaming |
*       j' <= 9 |  LUT8  |  LUT9  |  LUT10 |  LUT11 |  LUT12 |  LUT13 |        |        |  LUTA)  |  LUTB)  |
*               | (i<=10)| (i<=11)| (i<=12)| (i<=13)| (i<=14)| (i<=15)|        |        |         |         |
*     ----------|--------|--------|--------|--------|--------|--------|--------|--------|         |         |
*       j' <= 10|  LUT14 |  LUT15 |  LUT16 |  LUT17 |        |        |        |        |         |         |
*               | (i<=12)| (i<=13)| (i<=14)| (i<=15)|        |        |        |        |         |         |
*     ----------|--------|--------|--------|--------|--------|--------|--------|--------|         |         |
*       j' <= 11|  LUT18 |  LUT19 |  LUT20 |  LUT21 |        |        |        |        |         |         |
*               | (i<=12)| (i<=13)| (i<=14)| (i<=15)|        |        |        |        |         |         |
*     ----------|--------|--------|--------|--------|--------|--------|--------|--------|---------|---------|
*       j' <= 12|  LUT22 |        |        |        |        |        |        |        |  LUT28  |  LUT29  |
*               | (i<=15)|        |        |        |        |        |        |        | (i<=15) | (i<=15) |
*     ----------|--------|--------|--------|--------|--------|--------|--------|--------|(Roaming |(Roaming |
*       j' <= 13|  LUT23 |        |        |        |        |        |        |        |  LUTC)  |  LUTD)  |
*               | (i<=15)|        |        |        |        |        |        |        |         |         |
*     ----------|--------|--------|--------|--------|--------|--------|--------|--------|         |         |
*       j' <= 14|  LUT24 |        |        |        |        |        |        |        |         |         |
*               | (i<=15)|        |        |        |        |        |        |        |         |         |
*     ----------|--------|--------|--------|--------|--------|--------|--------|--------|         |         |
*       j' <= 15|  LUT25 |        |        |        |        |        |        |        |         |         |
*               | (i<=15)|        |        |        |        |        |        |        |         |         |
*
*/
typedef struct adi_adrv9025_ExtDpdModel
{
    adi_adrv9025_ExtDpdModelSel_e extDpdModelSel;                           /*!< DPD model which is required to be programmed */
    adi_adrv9025_ExtDpdLutBank_e  extDpdLutBankSel;                         /*!< LUT read bank which is sourced for this given model. 4 Banks / LUT for 8 bit and 2 Banks / LUT for 9 bit companders respectively */
    adi_adrv9025_ExtDpdActuatorCompanderSize_e extDpdActuatorCompanderSize; /*!< Selects 8-bit/9-bit compander size for actuator LUTs */
    uint8_t                                    extDpdMagSqGain;             /*!< Actuator magnitude square input gain */
    uint8_t                                    extDpdActuatorOutputShift;   /*!< Actuator output shift (0 - 15). Add one to double actuator output gain */
    uint16_t                                   extDpdLutISum;               /*!< i-sum enable for roaming LUTs */

    /*!< Array holding memory term value per LUT (i). Valid range 0 - 15. For an invalid I select, please set the isel value to 0xFE(-1) */
    uint8_t                                    extDpdISel[ADRV9025_MAX_EXT_DPD_MODEL_MEMORY_TERM_SIZE];
    uint8_t extDpdJSel[ADRV9025_MAX_EXT_DPD_MODEL_CROSS_TERM_SIZE];         /*!< Array holding cross term (j) per row. Valid range 0 - 15 */
} adi_adrv9025_ExtDpdModel_t;

/**
* \brief Data structure to hold a pointer to a swap mode Look Up Table 
*/
typedef struct adi_adrv9025_SwapModeLut
{
    uint8_t* extDpdSwapModeLutDataPtr;       /*!< Pointer to a swap mode LUT data */
    uint16_t extDpdSwapModeLutDataSizeBytes; /*!< Size of the swap mode LUT data which is referenced by swapModeLutDataPtr pointer */
} adi_adrv9025_SwapModeLut_t;

/**
* \brief Data structure to hold a row of external DPD 3-way capture data (Tx buffer, Tx alternate buffer and ORx buffer)
*/
typedef struct adi_adrv9025_ExtDpdCaptureDataRow
{
    int16_t  extDpdCaptureDataI[ADRV9025_MAX_EXT_DPD_CAPTURE_SAMPLE_SIZE]; /*!< Array consisting of I samples of external dpd capture data */
    int16_t  extDpdCaptureDataQ[ADRV9025_MAX_EXT_DPD_CAPTURE_SAMPLE_SIZE]; /*!< Array consisting of Q samples of external dpd capture data */
    uint16_t extDpdCaptureSampleArrSize;                                   /*!< No. of valid IQ samples in extDpdCaptureDataI and extDpdCaptureDataQ arrays */
} adi_adrv9025_ExtDpdCaptureDataRow_t;

/**
* \brief Data structure to hold a external DPD 3-way capture data (Tx buffer, Tx alternate buffer and ORx buffer)
*/
typedef struct adi_adrv9025_ExtDpdCaptureData
{
    adi_adrv9025_ExtDpdCaptureDataRow_t txCaptureData;              /*!< This member holds Tx capture data for external DPD adaptation. The data captured can be either pre-DPD actuator or post DPD acuator data
                                                                        configurable via adi_adrv9025_ExtDpdCaptureCtrl_t.dpdCaptureTxObsSel param through adi_adrv9025_ExtDpdCaptureConfigSet() API*/
    adi_adrv9025_ExtDpdCaptureDataRow_t txAlternateCaptureData;     /*!< This member holds Tx alternate buffer capture data for external DPD adaptation. The data captured can be either pre-DPD/post-DPD actuator or pre CFR data
                                                                        configurable via adi_adrv9025_ExtDpdCaptureCtrl_t.dpdCaptureTxAltObsSel param through adi_adrv9025_ExtDpdCaptureConfigSet() API */
    adi_adrv9025_ExtDpdCaptureDataRow_t orxCaptureData;             /*!< This member holds the ORx capture data for external DPD adaptation */
} adi_adrv9025_ExtDpdCaptureData_t;

/**
* \brief Data structure to hold external DPD Hard Clipper config
*/
typedef struct adi_adrv9025_ExtDpdHardClipperConfig
{
    uint32_t txChannelMask;              /*!< Mask consisting of 'OR'ed Tx channels for which the hard clipper config will be applied */
    uint8_t  extDpdHardClipperEnable;    /*!< 1- Enable hard clipper on the channels requested, 0 - Disable hard clipper */
    float    extDpdHardClipperThreshold; /*!< Normalized threshold for the hard clipper in the range 0 to 1.
                                              The threshold is relative to 0dBFS which means that a threshold value of 1 corresponds to 0dBFS.
                                              The threshold is applied to an approximation of SQRT(I^2 + Q^2). The hard clipper threshold can
                                              be calculated as extDpdHardClipperThreshold = 10^(threshold_dBFS/20)*/
} adi_adrv9025_ExtDpdHardClipperConfig_t;

#ifdef __cplusplus
}
#endif

#endif /* _ADI_ADRV9025_EXTERNAL_DPD_TYPES_H_ */
