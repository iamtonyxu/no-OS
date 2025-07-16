/**
* \file adrv9025_external_dpd.h
* \brief Contains ADRV025 init related private function prototypes for
*        adrv9025_external_dpd.c that helps adi_adrv9025_external_dpd.c
*
* ADRV025 API Version: $ADI_ADRV025_API_VERSION$
*/

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the ADRV025 API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/
#ifndef _ADRV025_EXTERNAL_DPD_H_
#define _ADRV025_EXTERNAL_DPD_H_

#include "adi_adrv9025_external_dpd.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
* \brief Performs range check on DPD sample capture config parameters
*
* \pre This function is automatically called as part of adi_adrv9025_ExtDpdCaptureConfigSet()
*      if ADI_ADRV9025_EXT_DPD_RANGE_CHECK is set to 1
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param dpdCaptureConfig Pointer to the ADRV9025 external DPD capture settings (Input)
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_ExtDpdCaptureConfigSetRangeCheck(adi_adrv9025_Device_t*              device,
                                                  adi_adrv9025_ExtDpdCaptureConfig_t* dpdCaptureConfig);

/**
* \brief Performs range check on the GPIO pin for triggering Tx-ORx sample capture for DPD adaptation
*
* \pre This function is automatically called as part of adi_adrv9025_ExtDpdCaptureConfigSet()
*      if ADI_ADRV9025_EXT_DPD_RANGE_CHECK is set to 1
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param extDpdCaptureTriggerPin GPIO to trigger the stream for capturing samples for DPD adaptation (Input)
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_ExtDpdCaptureTriggerGpioRangeCheck(adi_adrv9025_Device_t*    device,
                                                    adi_adrv9025_GpioPinSel_e extDpdCaptureTriggerPin);

/**
* \brief Performs range check on the GPIO pin for reporting Tx-ORx sample capture complete status indication
*
* \pre This function is automatically called as part of adi_adrv9025_ExtDpdCaptureConfigSet()
*      if ADI_ADRV9025_EXT_DPD_RANGE_CHECK is set to 1
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param extDpdCaptureTriggerPin GPIO to report Tx-ORx capture complete (Input)
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_ExtDpdCaptureDoneStatusPinRangeCheck(adi_adrv9025_Device_t*    device,
                                                      adi_adrv9025_GpioPinSel_e extDpdCaptureDoneStatusPin);

/**
* \brief Performs range check on DPD sample capture ctrl config parameters
*
* \pre This function is automatically called as part of adi_adrv9025_ExtDpdCaptureCtrlConfigSet()
*      if ADI_ADRV9025_EXT_DPD_RANGE_CHECK is set to 1
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param dpdCaptureCtrl Pointer to the ADRV9025 external DPD capture control settings (Input)
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_ExtDpdCaptureCtrlConfigSetRangeCheck(adi_adrv9025_Device_t*            device,
                                                      adi_adrv9025_ExtDpdCaptureCtrl_t* dpdCaptureCtrl);

/**
* \brief Performs range check on DPD capture path delay configuration
*
* \pre This function is automatically called as part of adi_adrv9025_ExtDpdCaptureCtrlConfigSet()
*      if ADI_ADRV9025_EXT_DPD_RANGE_CHECK is set to 1
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param dpdPathDelay Pointer to the ADRV9025 external DPD capture path delay setting (Input)
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_ExtDpdCapturePathDelayRangeCheck(adi_adrv9025_Device_t*                 device,
                                                  adi_adrv9025_ExtDpdCapturePathDelay_t* dpdPathDelay);

/**
* \brief Performs range check on DPD actuator gain monitoring config
*
* \pre This function is automatically called as part of adi_adrv9025_ExtDpdActuatorGainMonitorConfigSet()
*      if ADI_ADRV9025_EXT_DPD_RANGE_CHECK is set to 1
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param extDpdGainMonitorConfig An array of gain monitoring configurations to be applied to the target Tx channel's DPD actuators
* \param numGainMonitorConfigs No. of configs contained in extDpdGainMonitorConfig(array size)
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_ExtDpdActuatorGainMonitorConfigSetRangeCheck(adi_adrv9025_Device_t*                    device,
                                                              adi_adrv9025_ExtDpdActGainMonitorConfig_t extDpdActGainMonitorConfig[],
                                                              uint8_t                                   numGainMonitorConfigs);

/**
* \brief Performs range check on DPD capture level config parameters
*
* \pre This function is automatically called as part of adi_adrv9025_ExtDpdCaptureLevelConfigSet()
*      if ADI_ADRV9025_EXT_DPD_RANGE_CHECK is set to 1
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param dpdCaptureLevel Pointer to the ADRV9025 external DPD capture level control settings (Input)
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_ExtDpdCaptureLevelConfigSetRangeCheck(adi_adrv9025_Device_t*             device,
                                                       adi_adrv9025_ExtDpdCaptureLevel_t* dpdCaptureLevel);

/**
* \brief This function performs range check on the power band transition config settings
*
* \pre This function is automatically called as part of adi_adrv9025_ExtDpdCapturePwrThreshSet()
*      if ADI_ADRV9025_EXT_DPD_RANGE_CHECK is set to 1
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param targetDpdCaptureBandSel Target power band select for which the threshold settings need to be updated
* \param dpdCapturePwrThresh Capture power level threshold settings for the requested target power band
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_ExtDpdCapturePwrThreshSetRangeCheck(adi_adrv9025_Device_t*                 device,
                                                     adi_adrv9025_ExtDpdPwrThreshBandPair_e targetDpdCaptureBandSel,
                                                     adi_adrv9025_ExtDpdCapturePwrThresh_t* dpdCapturePwrThresh);

/**
* \brief Sets up the DPD capture done status indicator GPIO pin
*
* \pre This function is automatically called as part of adi_adrv9025_ExtDpdCaptureConfigSet()
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param dpdCaptureDoneStatusPin GPIO pin requested to be assigned for DPD capture done status monitoring (Input)
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_ExtDpdCaptureDoneStatusGpioSetup(adi_adrv9025_Device_t*    device,
                                                  adi_adrv9025_GpioPinSel_e dpdCaptureDoneStatusPin);

/**
* \brief Returns the DPD capture done status indicator GPIO pin
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param dpdCaptureDoneStatusPin Currently configured external DPD capture done indicator GPIO(output)
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_ExtDpdCaptureDoneStatusGpioGet(adi_adrv9025_Device_t*     device,
                                                adi_adrv9025_GpioPinSel_e* dpdCaptureDoneStatusPin);

/**
* \brief Sets the source ctrl of the corresponding nibble to the DPD capture done status indicator GPIO pin output 
*
* \pre This function is automatically called as part of adrv9025_ExtDpdCaptureDoneStatusGpioSetup()
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param dpdCaptureDoneStatusPin GPIO pin requested to be assigned for DPD capture done status monitoring (Input)
* \param srcCtrlVal GPIO output nibble source control for the requested capture done indicator GPIO
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
*/
int32_t adrv9025_ExtDpdCaptureDoneGpioSrcCtrlSet(adi_adrv9025_Device_t*    device,
                                                 adi_adrv9025_GpioPinSel_e dpdCaptureDoneStatusPin,
                                                 uint8_t                   srcCtrlVal);

/**
* \brief Gets the source ctrl of the corresponding nibble to the DPD capture done status indicator GPIO pin output
*
* \pre This function is automatically called as part of adrv9025_ExtDpdCaptureDoneStatusGpioSetup()
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param dpdCaptureDoneStatusPin GPIO pin requested to be assigned for DPD capture done status monitoring (Input)
* \param srcCtrlVal Pointer to memory which will be updated with the active GPIO output nibble source control 
*        for the requested capture done indicator GPIO in the device
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
*/
int32_t adrv9025_ExtDpdCaptureDoneGpioSrcCtrlGet(adi_adrv9025_Device_t*    device,
                                                 adi_adrv9025_GpioPinSel_e dpdCaptureDoneStatusPin,
                                                 uint8_t*                  srcCtrlVal);

/**
* \brief This function loads the ARM mailbox config array with dpd capture config data for ext DPD capture format
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param dpdCaptureConfig Pointer to the ADRV9025 external DPD capture settings (Input)
* \param armData Array which will be updated with byte stream of config parameters (Output)
* \param arraySize Size of armData array
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_ExtDpdCaptureConfigParamArrayLoad(adi_adrv9025_Device_t*              device,
                                                   adi_adrv9025_ExtDpdCaptureConfig_t* dpdCaptureConfig,
                                                   uint8_t                             armData[],
                                                   uint8_t                             arraySize);

/**
* \brief This function parses the byte array read from ARM mailbox to a dpd capture config structure
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param dpdCaptureConfig Pointer to the ADRV9025 external DPD capture settings
*                         which will be updated with the parsed data from armData(Output)
* \param armData Byte array of config params read from ARM mailbox (Input)
* \param arraySize Size of armData array
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_ExtDpdCaptureConfigParamArrayParse(adi_adrv9025_Device_t*              device,
                                                    adi_adrv9025_ExtDpdCaptureConfig_t* dpdCaptureConfig,
                                                    uint8_t                             armData[],
                                                    uint8_t                             arraySize);

/**
* \brief This function parses the byte array read from ARM mailbox to I and Q samples
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param extDpdCaptureIQInterleavedByteArray Byte array consisting of IQ interleaved samples (Input)
* \param extDpdCaptureIData Decoded array of I samples for external DPD adaptation (Output)
* \param extDpdCaptureQData Decoded array of Q samples for external DPD adaptation (Output)
* \param maxSampleSize Maximum no. of samples to be parsed into capture data I and Q arrays (Output)
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_ExtDpdCaptureDataParse(adi_adrv9025_Device_t* device,
                                        uint8_t                extDpdCaptureIQInterleavedByteArray[],
                                        int16_t                extDpdCaptureIData[],
                                        int16_t                extDpdCaptureQData[],
                                        uint16_t               maxSampleSize);

/**
* \brief This function queries the ARM proc whether the Tx-ORx capture for ext DPD adaptation is complete
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param captureCompleteFlag Pointer to the flag which will be updated with done/not done status(Output)
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retVal ADI_ADRV9025_ACT_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_ExtDpdCaptureDoneStatusQuery(adi_adrv9025_Device_t* device,
                                              uint8_t*               captureCompleteFlag);

/**
* \brief This function commands the ARM FW to clear the ext dpd capture done flag 
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retVal ADI_ADRV9025_ACT_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_ExtDpdCaptureDoneStatusClear(adi_adrv9025_Device_t* device);

/**
* \brief This function performs range check on the DPD actuator model config settings
*
* \pre This function is automatically called as part of adi_adrv9025_ExtDpdActuatorModelConfigSet()
*      if ADI_ADRV9025_EXT_DPD_RANGE_CHECK is set to 1
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param txChannel Target Tx Channel for which the DPD model is required to be configured
* \param extDpdModelConfig Pointer to external DPD model config params (Input)
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_ExtDpdActuatorModelConfigSetRangeCheck(adi_adrv9025_Device_t*      device,
                                                        adi_adrv9025_TxChannels_e   txChannel,
                                                        adi_adrv9025_ExtDpdModel_t* extDpdModelConfig);

/**
* \brief This function loads the ARM mailbox config array with dpd actuator model config data for ext DPD
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param txChannel Target Tx channel which for which the DPD model has to be configured
* \param extDpdModelConfig Pointer to the ADRV9025 external DPD model config settings (Input)
* \param armData Array which will be updated with byte stream of config parameters (Output)
* \param arraySize Max array size of armData
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_ExtDpdModelConfigParamArrayLoad(adi_adrv9025_Device_t*      device,
                                                 adi_adrv9025_TxChannels_e   txChannel,
                                                 adi_adrv9025_ExtDpdModel_t* extDpdModelConfig,
                                                 uint8_t                     armData[],
                                                 uint8_t                     arraySize);

/**
* \brief This function parses the DPD model config data received from ARM
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param dpdCaptureConfig Pointer to the ADRV9025 external DPD model config which
*        will be updated with the config settings (Output)
* \param armData Array consisting of DPD model config parameters (Input)
* \param arraySize Maximum size of armData array
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_ExtDpdModelConfigParamArrayParse(adi_adrv9025_Device_t*      device,
                                                  adi_adrv9025_ExtDpdModel_t* extDpdModelConfig,
                                                  uint8_t                     armData[],
                                                  uint8_t                     arraySize);

/**
* \brief This function performs range check on the DPD actuator enable API
*
* \pre This function is automatically called as part of adi_adrv9025_ExtDpdActuatorEnableSet()
*      if ADI_ADRV9025_EXT_DPD_RANGE_CHECK is set to 1
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param txChannelMask Target tx channel(s) for which the actuator needs to be enabled/disabled
*        bit[0] - 1 - Select Tx1 for Enabling/Disabling, 0 - Tx1 remains in extant state
*        bit[1] - 1 - Select Tx2 for Enabling/Disabling, 0 - Tx2 remains in extant state
*   	 bit[2] - 1 - Select Tx3 for Enabling/Disabling, 0 - Tx3 remains in extant state
*		 bit[3] - 1 - Select Tx4 for Enabling/Disabling, 0 - Tx4 remains in extant state
* \param enable Enables(greater than 0) or disables(0) the actuator for a given Tx channel
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_ExtDpdActuatorEnableSetRangeCheck(adi_adrv9025_Device_t* device,
                                                   uint32_t               txChannelMask,
                                                   uint8_t                enable);

/**
* \brief This function performs range check on the DPD actuator LUT programming/read back params
*
* \pre This function is automatically called as part of adi_adrv9025_ExtDpdActuatorLutWrite()/
*      adi_adrv9025_ExtDpdActuatorLutRead() if ADI_ADRV9025_EXT_DPD_RANGE_CHECK is set to 1
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param txChannel Tx channel for which the actuator LUT is supposed to be programmed
* \param extDpdLutSel Target LUT (between LUT0 - LUT31) to be programmed
* \param extDpdLutBankSel Target LUT bank (bank0 - bank4) in the selected LUT to be programmed
* \param lutData LUT data to be programmed for the requested Tx channel's actuator
* \param lutDataArrSize Actuator LUT array size for programming
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_ExtDpdActuatorLutRangeCheck(adi_adrv9025_Device_t*       device,
                                             adi_adrv9025_TxChannels_e    txChannel,
                                             adi_adrv9025_ExtDpdLut_e     extDpdLutSel,
                                             adi_adrv9025_ExtDpdLutBank_e extDpdLutBankSel,
                                             uint8_t                      lutData[]);

/**
* \brief Performs range check on adi_adrv9025_ExtDpdActuatorSwapModeLutWrite() API
*
* \pre This function is automatically called as part of adi_adrv9025_ExtDpdActuatorSwapModeLutWrite() function
*      if ADI_ADRV9025_EXT_DPD_RANGE_CHECK is set to 1
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param txChannel Tx channel for which the actuator LUT is supposed to be programmed
* \param extDpdLutBankSel Target LUT bank (bank0 - bank2) to be programmed. For a 9-bit compander only bank0 is valid.
* \param highestLutIndex highest index of LUT to be programmed between ADI_ADRV9025_EXT_DPD_LUT0 - ADI_ADRV9025_EXT_DPD_LUT30
* \param swapModelutData Array of Swap LUT data pointers where swapModeLutData[i] corresponds to LUT[i]. In case of a sparse
*                        LUT model use case, set swapLutData[unusedLutIndex].swapModeLutData = NULL swapLutData[unusedLutIndex].swapModeLutDataSize = 0
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_ExtDpdActuatorSwapModeLutWriteRangeCheck(adi_adrv9025_Device_t*       device,
                                                          adi_adrv9025_TxChannels_e    txChannel,
                                                          adi_adrv9025_ExtDpdLutBank_e extDpdLutBankSel,
                                                          adi_adrv9025_SwapModeLut_t   swapModeLutData[]);

/**
* \brief This function resolves the start address from which to program/read back the LUT data given the
*        LUT programming/read back parameters
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param txChannel Tx channel for which the actuator LUT is supposed to be programmed
* \param extDpdLutSel Target LUT (between LUT0 - LUT31) to be programmed
* \param extDpdLutBankSel Target LUT bank (bank0 - bank4) in the selected LUT to be programmed
* \param lutBaseAddr Start address from which to start writing/reading the LUT data
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_ExtDpdActuatorLutAddressResolve(adi_adrv9025_Device_t*       device,
                                                 adi_adrv9025_TxChannels_e    txChannel,
                                                 adi_adrv9025_ExtDpdLut_e     extDpdLutSel,
                                                 adi_adrv9025_ExtDpdLutBank_e extDpdLutBankSel,
                                                 uint32_t*                    lutBaseAddr);

/**
* \brief This function powers up/powers down the requested LUT for write or read access
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param txChannel Tx channel for which the actuator LUT power on state is required to be modified
* \param extDpdLutSel Target LUT (between LUT0 - LUT31) whose power on state is required to be modified
* \param lutPowerOnState Power On(0) or Power Off(1) the requested LUT
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_ExtDpdLutPowerOnSet(adi_adrv9025_Device_t*    device,
                                     adi_adrv9025_TxChannels_e txChannel,
                                     adi_adrv9025_ExtDpdLut_e  extDpdLutSel,
                                     uint8_t                   lutPowerOnState);

/**
* \brief This function returns the LUT power on state of the requested LUT
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param txChannel Tx channel for which the actuator LUT power down state is requested
* \param extDpdLutSel Target LUT for which the power down state is requested
* \param lutPowerOnState Pointer to memory which will be updated with the current power on state of the requested LUT
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_ExtDpdLutPowerOnGet(adi_adrv9025_Device_t*    device,
                                     adi_adrv9025_TxChannels_e txChannel,
                                     adi_adrv9025_ExtDpdLut_e  extDpdLutSel,
                                     uint8_t*                  lutPowerOnState);

/**
* \brief This function enables/disables the LUT swap mode operation for all Tx channels
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param lutSwapModeEn Enable(1) or Disable(0) swap mode operation for requested LUT
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_ExtDpdLutSwapModeEnableSet(adi_adrv9025_Device_t* device,
                                            uint8_t                lutSwapModeEn);

/**
* \brief This function gets the current enable/disable state of the LUT swap mode operation
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param lutSwapModeEn Pointer to a memory which will be updated with the current LUT swap mode enable value
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_ExtDpdLutSwapModeEnableGet(adi_adrv9025_Device_t* device,
                                            uint8_t*               lutSwapModeEn);

/**
* \brief Retrieves the current state of actuator enable and the actuator passthru enable for 
*        the requested Tx channel's actuator
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param txChannel Tx channel for which the actuator enable and passthru states are requested
* \param actuatorEnable Returns a 1-Actuator Enabled or 0-Actutor Disabled 
* \param actuatorPassthruEn Returns a 1-Passthru mode Enabled or 0-Passthru mode Disabled
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_ExtDpdActutatorEnPassthruGet(adi_adrv9025_Device_t*    device,
                                              adi_adrv9025_TxChannels_e txChannel,
                                              uint8_t*                  actuatorEnable,
                                              uint8_t*                  actuatorPassthruEn);

/**
* \brief Enables/Disables the actuator and actuator passthru mode for LUT programming
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param txChannel Tx channel for which the actuator enable and passthru states are required to be set
* \param actuatorEnable Flag to enable DPD actuator 1-Actuator Enable or 0-Actutor Disable
* \param actuatorPassthruEn Flag to enable DPD actuator in passthru mode 1-Passthru mode Enable or 0-Passthru mode Disable
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_ExtDpdActutatorEnPassthruSet(adi_adrv9025_Device_t*    device,
                                              adi_adrv9025_TxChannels_e txChannel,
                                              uint8_t                   actuatorEnable,
                                              uint8_t                   actuatorPassthruEn);

/**
* \brief Returns the current value of actuator RAM select bitfield
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param txChannel Tx channel for which the actuator LUT RAM select status is requested
* \param extDpdActuatorRamSel Current value of LUT RAM select in the device
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_ExtDpdActuatorRamSelGet(adi_adrv9025_Device_t*    device,
                                         adi_adrv9025_TxChannels_e txChannel,
                                         uint8_t*                  extDpdActuatorRamSel);

/**
* \brief Sets the DPD actuator LUT RAM select to a specified value for programming/reading back
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param txChannel Tx channel for which the actuator LUT RAM select is required to be updated
* \param extDpdActuatorRamSel LUT RAM select to be updated in the DPD register map
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_ExtDpdActuatorRamSelSet(adi_adrv9025_Device_t*    device,
                                         adi_adrv9025_TxChannels_e txChannel,
                                         uint8_t                   extDpdActuatorRamSel);

/**
* \brief Sets the DPD actuator Tx alternate observation point select
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param txChannel Tx channel for which the Tx alternate observation point select is required to be set
* \param txAlternateObservationSel Selects the Tx alternate observation buffer select
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_ExtDpdTxAlternateObsSelSet(adi_adrv9025_Device_t*                  device,
                                            adi_adrv9025_TxChannels_e               txChannel,
                                            adi_adrv9025_ExtDpdCaptureTxAltObsSel_e txAlternateObservationSel);

/**
* \brief Gets the DPD actuator Tx alternate observation point select
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param txChannel Tx channel for which the Tx alternate observation point select is requested
* \param txAlternateObservationSel Pointer which will be updated with the Tx alternate buffer selection point
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_ExtDpdTxAlternateObsSelGet(adi_adrv9025_Device_t*                   device,
                                            adi_adrv9025_TxChannels_e                txChannel,
                                            adi_adrv9025_ExtDpdCaptureTxAltObsSel_e* txAlternateObservationSel);

/**
* \brief Sets the DPD actuator Tx observation point select.
*
* The user can select between pre DPD and post DPD data to be captured.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param txChannel Tx channel for which the Tx observation point select is required to be set
* \param txObservationSel Selects the Tx alternate observation buffer select
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_ExtDpdTxObsSelSet(adi_adrv9025_Device_t*               device,
                                   adi_adrv9025_TxChannels_e            txChannel,
                                   adi_adrv9025_ExtDpdCaptureTxObsSel_e txObservationSel);

/**
* \brief Sets the DPD actuator Tx observation point select.
*
* The user can select between pre DPD and post DPD data to be captured.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param txChannel Tx channel for which the Tx observation point select is required to be set
* \param txObservationSel Returns the Tx observation buffer select
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_ExtDpdTxObsSelGet(adi_adrv9025_Device_t*                device,
                                   adi_adrv9025_TxChannels_e             txChannel,
                                   adi_adrv9025_ExtDpdCaptureTxObsSel_e* txObservationSel);

/**
* \brief Writes the external DPD actuator gain monitoring config
*
* This private function resolves the gain monitoring offset corresponding to a Tx channel
* and programs the configuration accordingly.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param txChannelMask A mask consisting of 'OR'ed target Tx channels for which the gain monitor settings are requested to be applied
* \param extDpdActGainMonitorConfig Pointer to the gain monitoring config settings to be applied to the requested Tx channels
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_ACT_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_ExtDpdActuatorGainMonitorConfigSet(adi_adrv9025_Device_t*                     device,
                                                    adi_adrv9025_ExtDpdActGainMonitorConfig_t* extDpdActGainMonitorConfig);

/**
* \brief Triggers a TDD Tx ON rising edge synchronized capture
*
* Writes the target Tx channel to the stream debug register which triggers a capture on the
* next occurring Tx ON rising edge
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param txChannel Target Tx channel for which TDD mode Tx ON synchronized capture is supposed to be triggered
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_ExtDpdTddTxOnCaptureTriggerSet(adi_adrv9025_Device_t*    device,
                                                adi_adrv9025_TxChannels_e txChannel);

/**
* \brief Triggers a firmware command based Tx-ORx capture for external DPD adaptation
*
* Issues a firmware command to start a Tx-ORx capture for immediate mode/power/peak detect based capture
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_ACT_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_ExtDpdCaptureStartTriggerSet(adi_adrv9025_Device_t* device);

/**
* \brief Performs range check on the DPD Hard clipper threshold
*
* \pre This function is automatically called as part of adi_adrv9025_ExtDpdHardClipperConfigSet
*      if macro ADI_ADRV9025_CFR_RANGE_CHECK is set to 1
*
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param device Pointer to the device settings structure
* \param extDpdHardClipperConfig Pointer to the DPD hard clipper config structure consisting of the hard clipper config settings.
* \param extDpdHardClipperCfgArraySize Number of DPD hard clipper config structures in the array
*
* \retval ADI_COMMON_ERR_NULL_PARAM Recovery action for NULL parameter check
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_ExtDpdHardClipperConfigSetRangeCheck(adi_adrv9025_Device_t*                 device,
                                                      adi_adrv9025_ExtDpdHardClipperConfig_t extDpdHardClipperConfig[],
                                                      uint8_t                                extDpdHardClipperCfgArraySize);

/**
* \brief Enables/Disables DPD hard clipper and configures the settings
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param extDpdHardClipperConfig External DPD hard clipper configuration to set
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_ExtDpdHardClipperConfigSet(adi_adrv9025_Device_t*                  device,
                                            adi_adrv9025_ExtDpdHardClipperConfig_t* extDpdHardClipperConfig);

/**
* \brief Communicates to the firmware that the captured data has been retrieved by the user
*
* Issues a firmware command to set a flag in the firmware to indicate that the previously captured data has
* been read out by the user and that a dummy capture can be scheduled.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_ACT_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_ExtDpdCaptureDataReadDoneSet(adi_adrv9025_Device_t* device);

/**
* \brief Reads back the captured Tx I-Q sample data for external DPD adaptation
*
* This function reads back a maximum of 4096 samples of Tx I-Q data stored in AHB memory via SPI-DMA.
* This Tx data is captured from the tx channel setup in adi_adrv9025_ExtDpdCaptureConfigSet() function.
* The Tx data captured can either be pre-DPD or post-DPD which can be configured by the user via
* adi_adrv9025_ExtDpdCaptureConfigSet() function. The Tx observation point (pre-DPD actuator vs post DPD actuator)
* is configurable through adi_adrv9025_ExtDpdCaptureCtrl_t.dpdCaptureTxObsSel parameter.
*
* \pre This function must be called after triggering a start capture through adi_adrv9025_ExtDpdCaptureStartTriggerSet()
*      API and verifying that the capture is complete via adi_adrv9025_ExtDpdCaptureDoneStatusGet() in API command mode
*      or verifying that capture is complete via GPIOs in pin mode.
*
* \note In a peak detect capture mode(adi_adrv9025_ExtDpdCaptureCtrl_t.dpdCaptureTypeSel = ADI_ADRV9025_EXT_DPD_CAPTURE_PEAK_DETECTION or ADI_ADRV9025_EXT_DPD_CAPTURE_EXTENDED_PEAK_SEARCH), 
*       the peak is aligned to the pathdelay fifo's delay(adi_adrv9025_ExtDpdCaptureConfig_t.pathDelay) + dpd actuator delay(12 samples) - capture start delay(adi_adrv9025_ExtDpdCaptureCtrl_t.extDpdCaptureAlignDelay)
*
* \note This API function is non re-entrant. This must be used in a single thread by the user.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param extDpdTxCaptureIData Pointer to memory which will be updated with the caputred Tx I data samples for external DPD adaptation (Output)
* \param extDpdTxCaptureQData Pointer to memory which will be updated with the caputred Tx Q data samples for external DPD adaptation (Output)
* \param maxTxCaptureSampleSize Limit for the no. of Tx I-Q capture samples to be retrieved for external DPD adaptation (Input)
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_ExtDpdTxCaptureDataGet(adi_adrv9025_Device_t* device,
                                        int16_t                extDpdTxCaptureIData[],
                                        int16_t                extDpdTxCaptureQData[],
                                        uint16_t               maxTxCaptureSampleSize);

/**
* \brief Reads back the captured Orx I-Q sample data for external DPD adaptation
*
* This function retrieves a maximum of 4096 samples of  captured Orx I-Q data stored 
* in AHB memory via SPI-DMA. By default Tx1 is mapped to ORx1, Tx2 to ORx2,
* Tx3 to ORx3 and Tx4 to ORx4. Currently, configuration of Tx observable on ORx
* via adi_adrv9025_TxToOrxMapping() is not supported.
*
* \pre This function must be called after triggering a start capture through adi_adrv9025_ExtDpdCaptureStartTriggerSet()
*      API and verifying that the capture is complete via adi_adrv9025_ExtDpdCaptureDoneStatusGet() in command mode
*      or verifying that capture is complete via GPIOs in pin mode.
*
* \note This API function is non re-entrant. This must be used in a single thread by the user.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param extDpdOrxCaptureIData Pointer to memory which will be updated with the caputred Orx I data samples for external DPD adaptation (Output)
* \param extDpdOrxCaptureQData Pointer to memory which will be updated with the caputred Orx Q data samples for external DPD adaptation (Output)
* \param maxTxCaptureSampleSize Limit for the no. of Tx I-Q capture samples to be retrieved for external DPD adaptation (Input)

*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_ExtDpdOrxCaptureDataGet(adi_adrv9025_Device_t* device,
                                         int16_t                extDpdOrxCaptureIData[],
                                         int16_t                extDpdOrxCaptureQData[],
                                         uint16_t               maxTxCaptureSampleSize);

/**
* \brief Reads back the captured Tx I-Q sample data for external DPD adaptation
*
* This function reads back a maximum of 4096 samples of Tx I-Q data stored in AHB memory via SPI-DMA.
* This Tx data is captured from the tx channel setup in adi_adrv9025_ExtDpdCaptureConfigSet() function.
* The Tx data captured can either be pre-DPD or post-DPD which can be configured by the user via
* adi_adrv9025_ExtDpdCaptureConfigSet() function. The data in the alternate Tx capture buffer can be either
* pre-DPD, post-DPD or pre CFR data.The Tx Alternate observation point (pre-DPD actuator vs post DPD actuator vs pre CFR)
* is configurable through adi_adrv9025_ExtDpdCaptureCtrl_t.dpdCaptureTxAltObsSel parameter. The table below
* outlines the relationship between capture config parameters and the Tx data captured.
*
* adi_adrv9025_ExtDpdCaptureCtrl_t.dpdCaptureTxObsSel    | adi_adrv9025_ExtDpdCaptureCtrl_t.dpdCaptureTxAltObsSel | Tx Alternate Buffer Data Captured
* -------------------------------------------------------|--------------------------------------------------------|-----------------------------------
*  ADI_ADRV9025_EXT_DPD_CAPTURE_TX_OBS_POST_DPD_ACTUATOR | ADI_ADRV9025_EXT_DPD_CAPTURE_TX_ALT_OBS_DPD_ACTUATOR   | Pre - DPD actuator Tx data
*  ADI_ADRV9025_EXT_DPD_CAPTURE_TX_OBS_PRE_DPD_ACTUATOR  | ADI_ADRV9025_EXT_DPD_CAPTURE_TX_ALT_OBS_DPD_ACTUATOR   | Post- DPD actuator Tx data
*  ADI_ADRV9025_EXT_DPD_CAPTURE_TX_OBS_POST_DPD_ACTUATOR | ADI_ADRV9025_EXT_DPD_CAPTURE_TX_ALT_OBS_PRE_CFR        | Pre - CFR Tx data
*  ADI_ADRV9025_EXT_DPD_CAPTURE_TX_OBS_PRE_DPD_ACTUATOR  | ADI_ADRV9025_EXT_DPD_CAPTURE_TX_ALT_OBS_PRE_CFR        | Pre - CFR Tx data 
*
* \note Please note that the Tx alternate buffer capture is not time aligned with the ORx data.
*
* \note In a peak detect capture mode(adi_adrv9025_ExtDpdCaptureCtrl_t.dpdCaptureTypeSel = ADI_ADRV9025_EXT_DPD_CAPTURE_PEAK_DETECTION or ADI_ADRV9025_EXT_DPD_CAPTURE_EXTENDED_PEAK_SEARCH),
*       the peak should be aligned to the dpd actuator delay(12 samples) + register delay(2 samples) = 14samples.
*
* \pre This function must be called after triggering a start capture through adi_adrv9025_ExtDpdCaptureStartTriggerSet()
*      API and verifying that the capture is complete via adi_adrv9025_ExtDpdCaptureDoneStatusGet() in API command mode
*      or verifying that capture is complete via GPIOs in pin mode.
*
* \note This API function is non re-entrant. This must be used in a single thread by the user.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param extDpdTxCaptureIData Pointer to memory which will be updated with the caputred alternate Tx buffer I data samples (Output)
* \param extDpdTxCaptureQData Pointer to memory which will be updated with the caputred alternate Tx buffer Q data samples (Output)
* \param maxTxCaptureSampleSize Limit for the no. of alternate Tx buffer I-Q capture samples to be retrieved (Input)
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_ExtDpdTxAlternateCaptureDataGet(adi_adrv9025_Device_t* device,
                                                 int16_t                extDpdTxCaptureIData[],
                                                 int16_t                extDpdTxCaptureQData[],
                                                 uint16_t               maxTxCaptureSampleSize);

/**
* \brief Deserialize raw data of ext DPD detailed status into the structure
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param extDpdCaptureDetailedStatus Pointer to adi_adrv9025_ExtDpdCaptureDetailedStatus_t where it will be populated with data deserialized from buffer
* \param buf Pointer to buffer containing the raw data
* \param size size of buffer

*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_ExtDpdCaptureDetailedStatusDeserialize(adi_adrv9025_Device_t*                      device,
                                                        adi_adrv9025_ExtDpdCaptureDetailedStatus_t* extDpdCaptureDetailedStatus,
                                                        uint8_t*                                    buf,
                                                        uint8_t                                     size);

/**
* \brief This function reads capture data when device operates in extended peak capture mode.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param extDpdCaptureData Pointer to adi_adrv9025_ExtDpdCaptureData_t structure holding capture data buffers
* \param numOfLutBanksToRead Total number of LUT banks to read capture data

*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_ExtDpdExtendedPeakSearchCaptureDataGet(adi_adrv9025_Device_t *device,
                                                      adi_adrv9025_ExtDpdCaptureData_t *extDpdCaptureData,
                                                      uint8_t numOfLutBanksToRead);

/**
* \brief Programs LUT(LUT0-25) data to corresponding correlator memory address. This function is used in multi model swap method
* in which correlator memory is used to hold LUT data temporarily, until FW transfers the data to actual LUT memory space.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param lutIndex Target LUT (between LUT0 - LUT25) to be programmed
* \param lutData An array containing the LUT data to be programmed
* \param lutDataSize Actuator LUT array size for programming (in terms of bytes)
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_ExtDpdActuatorMultiModelSwapLutWrite(adi_adrv9025_Device_t *device, adi_adrv9025_ExtDpdLut_e lutIndex, uint8_t lutData[], uint16_t lutDataSize);


/**
* \brief Performs range check on adi_adrv9025_ExtDpdActuatorMultiModelSwapLutWrite() API
*
* \pre This function is automatically called as part of adi_adrv9025_ExtDpdActuatorMultiModelSwapLutWrite() function
*      if ADI_ADRV9025_EXT_DPD_RANGE_CHECK is set to 1
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param multiModelSwapModeLutData Array of LUT data pointers where swapModeLutData[i] corresponds to LUT[i]. In case of a sparse
*                        LUT model use case, set multiModelSwapModeLutData[unusedLutIndex].swapModeLutData = NULL multiModelSwapModeLutData[unusedLutIndex].swapModeLutDataSize = 0
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_ExtDpdActuatorMultiModelSwapLutWriteRangeCheck(adi_adrv9025_Device_t *device, adi_adrv9025_SwapModeLut_t multiModelSwapModeLutData[]);
#ifdef __cplusplus
}
#endif

#endif /* ADRV025_EXTERNAL_DPD_H */
