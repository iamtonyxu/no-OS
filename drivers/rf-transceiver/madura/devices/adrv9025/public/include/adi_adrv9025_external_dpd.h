/**
* \file adi_adrv9025_external_dpd.h
* \brief Contains ADRV9025 External DPD related function prototypes for
*        adi_adrv9025_external_dpd.c
*
* ADRV9025 API Version: $ADI_ADRV9025_API_VERSION$
*/

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the ADRV9025 API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

#ifndef _ADI_ADRV9025_EXTERNAL_DPD_H_
#define _ADI_ADRV9025_EXTERNAL_DPD_H_

#include "adi_adrv9025_external_dpd_types.h"
#include "adi_adrv9025_types.h"
#include "adi_common_error_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
 * External DPD Tx-ORx data capture Ctrl Functions
 ****************************************************************************
 */

/**
 * \brief Configures the capture settings for Tx-Orx data capture for external DPD adaptation
 *
 * Commands the ARM firmware to configure the DPD capture parameters such as timeouts, capture pwr level thresholds, capture ctrl
 * Also, configures the GPIO pin to trigger DPD capture and GPIO pin output from ADRV9025 for capture complete status indication.
 * This function can be called once during init and the capture ctrl and power thresholds can be adjusted during runtime
 * 
 * \pre This function must be called after Tx-ORx mapping stream has been executed. The Tx to ORx mapping
 *      API can be found in adi_adrv9025_RadioCtrl.c. This function also requires that the external dpd init cal
 *      cal(mask specified by adi_adrv9025_InitCalibrations_e.ADI_ADRV9025_DPD) be executed prior to calling this function.
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
 * \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
 * \retval Add ADI_ADRV9025_ACT_ERR_RESET_CPU Recovery action for ARM reset required
 * \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
 */
int32_t adi_adrv9025_ExtDpdCaptureConfigSet(adi_adrv9025_Device_t*              device,
                                            adi_adrv9025_ExtDpdCaptureConfig_t* dpdCaptureConfig);

/**
* \brief Retrieves the currently configured DPD capture settings for external adaptation
*
* \pre This function can be called anytime after device initialization and after loading of ARM is complete
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param dpdCaptureConfig Pointer to the ADRV9025 external DPD capture settings
*        which will be updated with the settings active in the device (Output)
*        which will be updated with the current settings in the device (Output)
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval Add ADI_ADRV9025_ACT_ERR_RESET_CPU Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_ExtDpdCaptureConfigGet(adi_adrv9025_Device_t*              device,
                                            adi_adrv9025_ExtDpdCaptureConfig_t* dpdCaptureConfig);

/**
* \brief Configures/Updates the DPD capture control settings during runtime.
*
* This function is typically used to switch target Tx channel for capturing data during runtime.
* This function commands the ARM firmware to update DPD capture control parameters during runtime.
*
* \pre This function can be called after DPD capture settings have been configured through
*      adi_adrv9025_ExtDpdCaptureConfigSet() function
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
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval Add ADI_ADRV9025_ACT_ERR_RESET_CPU Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_ExtDpdCaptureCtrlConfigSet(adi_adrv9025_Device_t*            device,
                                                adi_adrv9025_ExtDpdCaptureCtrl_t* dpdCaptureCtrl);

/**
* \brief Retrieves the current DPD capture control settings
*
* Commands the ARM firmware to read back the current DPD capture ctrl settings
*
* \pre This function can be anytime after initialization and loading of ARM is complete
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param dpdCaptureCtrl Pointer to the ADRV9025 external DPD capture control settings which will be updated
*                       with the current device settings (Output)
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval Add ADI_ADRV9025_ACT_ERR_RESET_CPU Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_ExtDpdCaptureCtrlConfigGet(adi_adrv9025_Device_t*            device,
                                                adi_adrv9025_ExtDpdCaptureCtrl_t* dpdCaptureCtrl);

/**
* \brief Configures/Updates the DPD sample capture level control settings during runtime.
*
* This function can be used to update the target Tx data power band based on which a Tx-ORx
* data capture will be initiated when a new capture is triggered via adi_adrv9025_ExtDpdCaptureStartTriggerSet() API.
* The thresholds for different power bands can be configured via adi_adrv9025_ExtDpdCapturePwrThreshSet() API.
*
* \pre This function can be called after DPD capture settings have been configured through
*      adi_adrv9025_ExtDpdCaptureConfigSet() function initially. This function is primarily intended to used when the 
*      capture type(adi_adrv9025_ExtDpdCaptureCtrl_t.dpdCaptureTypeSel) is set to ADI_ADRV9025_EXT_DPD_CAPTURE_POWER_LEVEL_TRIGGER 
*      in the device via adi_adrv9025_ExtDpdCaptureConfigSet() API.
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
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval Add ADI_ADRV9025_ACT_ERR_RESET_CPU Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_ExtDpdCaptureLevelConfigSet(adi_adrv9025_Device_t*             device,
                                                 adi_adrv9025_ExtDpdCaptureLevel_t* dpdCaptureLevel);

/**
* \brief Retrieves the current external DPD sample capture level settings.
*
* Commands the ARM firmware to readback the currently configured capture level settings
*
* \pre This function can be anytime after initialization and loading of ARM is complete
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param dpdCaptureLevel Pointer to the ADRV9025 external DPD capture level control settings
*        which will be updated with current DPD capture level settings in the device (Output)
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval Add ADI_ADRV9025_ACT_ERR_RESET_CPU Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_ExtDpdCaptureLevelConfigGet(adi_adrv9025_Device_t*             device,
                                                 adi_adrv9025_ExtDpdCaptureLevel_t* dpdCaptureLevel);

/**
* \brief Configures/Updates the DPD capture power level thresholds during runtime.
*
* Configures the power thresholds for the requested target power band which can then be used
* to initiate a Tx-ORx data capture. The target power band that will be used to initiate a
* capture can be configured via adi_adrv9025_ExtDpdCaptureLevelConfigSet() API.
*
* \pre This function can be called after DPD capture settings have been configured through
*      adi_adrv9025_ExtDpdCaptureConfigSet() function. This function is valid only if the 
*      capture type(adi_adrv9025_ExtDpdCaptureCtrl_t.dpdCaptureTypeSel) is set to ADI_ADRV9025_EXT_DPD_CAPTURE_POWER_LEVEL_TRIGGER 
*      in the device via adi_adrv9025_ExtDpdCaptureConfigSet() API.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param targetDpdCaptureBandSel Target power band select for which the threshold settings need to be updated
* \param dpdCapturePwrThresh Capture power level threshold settings for the requested target power band (Input)
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval Add ADI_ADRV9025_ACT_ERR_RESET_CPU Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_ExtDpdCapturePwrThreshSet(adi_adrv9025_Device_t*                 device,
                                               adi_adrv9025_ExtDpdPwrThreshBandPair_e targetDpdCaptureBandSel,
                                               adi_adrv9025_ExtDpdCapturePwrThresh_t* dpdCapturePwrThresh);

/**
* \brief Retrieves the DPD sample capture power threshold settings.
*
* Commands the ARM to return the power threshold settings for the requested pair of power bands
*
* \pre This function can be anytime after initialization and loading of ARM is complete
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param targetDpdCaptureBandSel Target power band select for which the threshold settings need to be updated
* \param dpdCapturePwrThresh Capture power level threshold settings for the requested target power band (Output)
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval Add ADI_ADRV9025_ACT_ERR_RESET_CPU Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_ExtDpdCapturePwrThreshGet(adi_adrv9025_Device_t*                 device,
                                               adi_adrv9025_ExtDpdPwrThreshBandPair_e targetDpdCaptureBandSel,
                                               adi_adrv9025_ExtDpdCapturePwrThresh_t* dpdCapturePwrThresh);

/**
* \brief Triggers Tx-ORx data capture for external DPD adaptation through this command instead of a GPIO pin
*
* In case of a non-GPIO triggered DPD capture, this API commands the ARM firmware to trigger Tx-Orx capture.
* The captured Tx and ORx data can be retrieved via adi_adrv9025_ExtDpdCaptureDataGet() API. This runtime 
* API can be used trigger Tx-ORx capture for the following use cases :-
*
* 1) Immediate capture in command mode. A new Tx-ORx data capture is initiated unconditionally once this 
*    command is issued. The parameter adi_adrv9025_ExtDpdCaptureCtrl_t.dpdCaptureTypeSel is required to be
*    set to ADI_ADRV9025_EXT_DPD_CAPTURE_IMMEDIATE_TRIGGER via adi_adrv9025_ExtDpdCaptureConfigSet() API
*    to exercise this mode of operation.
* 2) Peak detection based capture. Once a capture is triggered by this function, a new Tx-ORx data capture 
*    is initiated when a new peak in the Tx data is encountered in the time interval specified by 
*    adi_adrv9025_ExtDpdCaptureCtrl_t.dpdPeakCaptureConfig.extDpdPeakWinCnt configured via adi_adrv9025_ExtDpdCaptureConfigSet() API.
*    The parameter adi_adrv9025_ExtDpdCaptureCtrl_t.dpdCaptureTypeSel is required to be set to ADI_ADRV9025_EXT_DPD_CAPTURE_PEAK_DETECTION
*    via adi_adrv9025_ExtDpdCaptureConfigSet() API to exercise this mode of operation.
* 3) Power level based capture trigger. Once a capture is triggered by this function, a new Tx-ORx data capture
*    is initiated when a target power level on the Tx data is reached which can be configured via adi_adrv9025_ExtDpdCaptureLevelConfigSet() API
*    and the thresholds for different power bands are configured via adi_adrv9025_ExtDpdCapturePwrThreshConfigSet() API.
*    The parameter adi_adrv9025_ExtDpdCaptureCtrl_t.dpdCaptureTypeSel is required to be set to ADI_ADRV9025_EXT_DPD_CAPTURE_POWER_LEVEL_TRIGGER
*    via adi_adrv9025_ExtDpdCaptureConfigSet() API to exercise this mode of operation.
* 4) TDD mode Tx ON synchronized capture trigger. Once a capture is triggered by this function, a new Tx-ORx data capture is initiated on the
*    next occurring TDD TxON rising edge for a target Tx channel pre-configured via adi_adrv9025_ExtDpdCaptureConfigSet() API.
*    The parameter adi_adrv9025_ExtDpdCaptureCtrl_t.dpdCaptureTypeSel is required to be set to ADI_ADRV9025_EXT_DPD_CAPTURE_TDD_TXON_SYNC
*    via adi_adrv9025_ExtDpdCaptureConfigSet() API to exercise this mode of operation.
* 5) Extended Peak detection based capture. Once a capture is triggered by this function, a new Tx-ORx data capture
*    is initiated when a new peak in the Tx data is encountered in the time interval specified by
*    adi_adrv9025_ExtDpdCaptureCtrl_t.dpdPeakCaptureConfig.extDpdPeakExtendedWindowUs configured via adi_adrv9025_ExtDpdCaptureConfigSet() API.
*    The parameter adi_adrv9025_ExtDpdCaptureCtrl_t.dpdCaptureTypeSel is required to be set to ADI_ADRV9025_EXT_DPD_CAPTURE_EXTENDED_PEAK_SEARCH
*    via adi_adrv9025_ExtDpdCaptureConfigSet() API to exercise this mode of operation. This mode is valid only in TDD mode of operation.
*
* \pre This function can be called after DPD capture settings have been configured through
*      adi_adrv9025_ExtDpdCaptureConfigSet() function. It is also required to enable the Tx channel
*      and the observable ORx channel before triggering the capture via adi_adrv9025_RxTxEnableSet() API.
*      This function also requires the external dpd init cal mask specified by adi_adrv9025_InitCalibrations_e.ADI_ADRV9025_DPD.
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
* \retval Add ADI_ADRV9025_ACT_ERR_RESET_CPU Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_ExtDpdCaptureStartTriggerSet(adi_adrv9025_Device_t* device);

/**
* \brief Returns the completion status of a previously triggered Tx-Orx data capture for external DPD adaptation
*
* This function also clears the completion status flag by writing a '1' to the 'capture done' bitfield
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param captureCompleteFlag Returns a capture complete(1) or capture in progress/not started(0) (Output)
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval Add ADI_ADRV9025_ACT_ERR_RESET_CPU Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_ExtDpdCaptureDoneStatusGet(adi_adrv9025_Device_t* device,
                                                uint8_t*               captureCompleteFlag);

/**
* \brief Returns the detailed status of a previously triggered capture
*
* This function returns a detailed status of a previously triggered capture including a timeout.
* Detailed status will be invalid after adi_adrv9025_ExtDpdCaptureDataGet() is called.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param txChannel Tx channel for which detailed capture status is requested for (Input)
* \param extDpdCaptureDetailedStatus Pointer to memory updated with the detailed capture status (Output)
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_ExtDpdCaptureDetailedStatusGet(adi_adrv9025_Device_t*                      device,
                                                    adi_adrv9025_TxChannels_e                   txChannel,
                                                    adi_adrv9025_ExtDpdCaptureDetailedStatus_t* extDpdCaptureDetailedStatus);

/**
* \brief Reads back the captured Tx and ORx I-Q sample data for external DPD adaptation
*
* This function can be used to read the capture data from all 3 capture buffers - i.e. Tx capture buffer,
* Tx alternate capture buffer and ORx capture buffer. Spi streaming can be enabled to read captured data from device.
*
* This function reads back a maximum of 4096 samples of Tx and ORx I-Q data stored in AHB memory via SPI-DMA.
* The Tx data is captured from the tx channel setup in adi_adrv9025_ExtDpdCaptureConfigSet() function.
* The Tx data captured can either be pre-DPD or post-DPD which can be configured by the user via
* adi_adrv9025_ExtDpdCaptureConfigSet() function. The Tx observation point (pre-DPD actuator vs post DPD actuator)
* is configurable through adi_adrv9025_ExtDpdCaptureCtrl_t.dpdCaptureTxObsSel parameter. The Tx alternate buffer
* observation point(pre/post-DPD actuator vs pre CFR data) is configurable through adi_adrv9025_ExtDpdCaptureCtrl_t.dpdCaptureTxAltObsSel
* parameter. The combination of adi_adrv9025_ExtDpdCaptureCtrl_t.dpdCaptureTxObsSel and adi_adrv9025_ExtDpdCaptureCtrl_t.dpdCaptureTxAltObsSel
* determines the observation point of the Tx alternate capture buffer represented in the table below :-
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
* \note It is required to read out the previously captured data from the capture buffers through this function before
*       scheduling the next Tx-ORx data capture for external DPD adaptation.
*
* \note This API function is non re-entrant. This must be used in a single thread by the user.
*
* \pre This function must be called after triggering a start capture through adi_adrv9025_ExtDpdCaptureStartTriggerSet()
*      API and verifying that the capture is complete via adi_adrv9025_ExtDpdCaptureDoneStatusGet() in API command mode
*      or verifying that capture is complete via GPIOs in pin mode.
*
* \note In extended peak capture mode(ADI_ADRV9025_EXT_DPD_CAPTURE_EXTENDED_PEAK_SEARCH), this function returns data only for adi_adrv9025_ExtDpdCaptureDataRow_t.txCaptureData and adi_adrv9025_ExtDpdCaptureDataRow_t.orxCaptureData.
*       The point of capture for adi_adrv9025_ExtDpdCaptureDataRow_t.txCaptureData(Tx observation buffer) depends on adi_adrv9025_ExtDpdCaptureCtrl_t.dpdCaptureTxObsSel
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param extDpdCaptureData Pointer to external DPD 3 way capture read back memory
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_ExtDpdCaptureDataGet(adi_adrv9025_Device_t*            device,
                                          adi_adrv9025_ExtDpdCaptureData_t* extDpdCaptureData);

/****************************************************************************
* External DPD actuator Ctrl Functions
****************************************************************************
*/

/**
* \brief Enables the actuator of a selected Tx channel
*
* This function enables / disables the actuator for a selected Tx channel
*
* \pre This function can be called anytime after device initialization and
*      after ARM has been loaded. For the actuator to function it is also
*      required that the Tx Channel is enabled. The function adi_adrv9025_RxTxEnableSet()
*      function must be used to enable the requisite Tx channel.
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
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval Add ADI_ADRV9025_ACT_ERR_RESET_CPU Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_ExtDpdActuatorEnableSet(adi_adrv9025_Device_t* device,
                                             uint32_t               txChannelMask,
                                             uint8_t                enable);

/**
* \brief Retrieves the actuator enable status of a selected Tx channel
*
* This function reads the current enable status of the actuator for a requested Tx channel
*
* \pre This function can be called anytime after device initialization and
*      after ARM has been loaded.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param txChannel Target Tx channel whose dpd actuator enable status is requested
* \param enable Pointer to the dpd actuator enable status which will be updated with the current DPD actuator enable status
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval Add ADI_ADRV9025_ACT_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_ExtDpdActuatorEnableGet(adi_adrv9025_Device_t*    device,
                                             adi_adrv9025_TxChannels_e txChannel,
                                             uint8_t*                  enable);

/**
* \brief Configures the DPD model for a selected Tx channel's actuator
*
* This function commands the ARM to configure the DPD model settings for a selected Tx Channel's actuator.
* The GMP model is defined by summation(a(i,j,k) * |x(n-i)|^k * x(n-j)) for i,j,k in the range 0 to 15 where a(i,j,k) is the 
* complex co-efficient, x(n) is the input, i is the memory term, j is the cross term and k is the power term.
*
* The GMP model to LUT mapping restrictions are shown below for a given memory term(i) represented by adi_adrv9025_ExtDpdModel_t.extDpdISel
* and the cross term(j) represented by adi_adrv9025_ExtDpdModel_t.extDpdJSel. Each LUT can hold up to 4 models
* for a compander size of 8 and 2 models for a compander size of 9. This function can be used to select the target LUTs,
* model select and configure the memory term(i) for the target LUTs of the model and the cross term(j) for each row of LUTs selected in the model.
* Please note that the LUTs in the same row shown below share the same j(cross term) value. The roaming LUTs are a special
* case where it can be assigned to one of the 4 j columns via adi_adrv9025_ExtDpdModel_t.extDpdLutISum. Each LUT consists of
* the partial sum of products of summation(a(i,j,k) * |x(n-i)|^k) for k in the range 0 to 15.
*
* Each entry to the LUT is a 32 bit complex value with the MSB 16 bits consisting of imaginary part and the LSB 16 bits consisting of the real part
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
* \pre This function can be called anytime after device initialization and
*      after ARM has been loaded and this function also requires that external dpd init cal
*      has run. The external dpd init cal mask is specified by adi_adrv9025_InitCalibrations_e.ADI_ADRV9025_DPD.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param txChannel Target Tx channel for which the DPD model is required to be configured
* \param extDpdModelConfig Pointer to external DPD model config params (Input)
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval Add ADI_ADRV9025_ACT_ERR_RESET_CPU Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_ExtDpdActuatorModelConfigSet(adi_adrv9025_Device_t*      device,
                                                  adi_adrv9025_TxChannels_e   txChannel,
                                                  adi_adrv9025_ExtDpdModel_t* extDpdModelConfig);

/**
* \brief Retrieves the current DPD model configuration in the device
*
* \pre This function can be called anytime after device initialization and
*      after ARM has been loaded
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param txChannel Target Tx channel whose DPD model settings are requested to be retrieved
* \param extDpdModelConfig Pointer to external DPD model config params 
*                          which will be updated with current DPD model settings (Output)
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval Add ADI_ADRV9025_ACT_ERR_RESET_CPU Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_ExtDpdActuatorModelConfigGet(adi_adrv9025_Device_t*      device,
                                                  adi_adrv9025_TxChannels_e   txChannel,
                                                  adi_adrv9025_ExtDpdModel_t* extDpdModelConfig);

/**
* \brief Configures the GPIO pins to activate one of the four possible actuator models 
* 
* The user is required to setup 2 GPIO pins to switch b/w one of the four possible DPD models
* to the actuator if it is desired to control the model selects through the GPIO mechanism.
* This function sets up the GPIO inputs to ADRV9025. ARM processor on the ADRV9025 responds
* to the changes in the level of these GPIO pins. Logic 0 corresponds to low signal level on the pins
* and Logic 1 corresponds to high signal level on the pins.
*
* The correspondence b/w the logic level on the pins and the model selected is listed below :-
*
*                  extDpdModelSelPin1 | extDpdModelSelPin0 | DPD Model Selected
*               ----------------------|--------------------|---------------------
*                         0           |         0          |      Model 0
*                         0           |         1          |      Model 1
*                         1           |         0          |      Model 2
*                         1           |         1          |      Model 3
*
* To dissociate GPIOs with the model select functionality set the pin select to ADI_ADRV9025_GPIO_INVALID
* Alternatively the models can be switched through an API command adi_adrv9025_ExtDpdModelSelectSet().
* 
* \pre This function can be called anytime after device initialization and
*      after ARM has been loaded
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param extDpdModelSelPin0 GPIO corresponding to lsb of the actuator DPD model select
* \param extDpdModelSelPin1 GPIO corresponding to msb of the actuator DPD model select
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval Add ADI_ADRV9025_ACT_ERR_RESET_CPU Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_ExtDpdActuatorModelSelGpiosSet(adi_adrv9025_Device_t*    device,
                                                    adi_adrv9025_GpioPinSel_e extDpdModelSelPin0,
                                                    adi_adrv9025_GpioPinSel_e extDpdModelSelPin1);

/**
* \brief Retrieves the currently configured GPIO pins for DPD model switching
*
* \pre This function can be called anytime after device initialization and
*      after ARM has been loaded
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param extDpdModelSelPin0 Pointer to GPIO select pin0 for switching models which will
*                           be updated with the current GPIO assigned for model switching (Output)
* \param extDpdModelSelPin1 Pointer to GPIO select pin1 for switching models which will
*                           be updated with the current GPIO assigned for model switching (Output)
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval Add ADI_ADRV9025_ACT_ERR_RESET_CPU Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_ExtDpdActuatorModelSelGpiosGet(adi_adrv9025_Device_t*     device,
                                                    adi_adrv9025_GpioPinSel_e* extDpdModelSelPin0,
                                                    adi_adrv9025_GpioPinSel_e* extDpdModelSelPin1);

/**
* \brief Selects the active DPD model for the requested Tx Channel's actuator
*
* This API command is an alternative to switch DPD models via GPIO input pins to ADRV9025.
*
* \pre This function requires that the DPD Model is configured through adi_adrv9025_ExtDpdModelConfigSet()
*      API and the requisite LUT data is written to the corresponding LUT via adi_adrv9025_ExtDpdLutWrite()
*      OR adi_adrv9025_ExtDpdSwapModeLutWrite() API function.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param txChannelMask Mask consisting of Tx channels for which the model needs to be switched.
*        Setting the mask bits to 1(as shown below) enables the corresponding Tx actuator model.
*        bit[0] - Tx1 actuator Sel, bit[1] - Tx2 actutor Sel, bit[2] - Tx3 actuator Sel, bit[3] - Tx4 actuator sel
* \param extDpdModelSel Target model (b/w Model0-Model3) to switch to 
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval Add ADI_ADRV9025_ACT_ERR_RESET_CPU Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_ExtDpdActuatorModelSelectSet(adi_adrv9025_Device_t*        device,
                                                  uint32_t                      txChannelMask,
                                                  adi_adrv9025_ExtDpdModelSel_e extDpdModelSel);

/**
* \brief Retrieves the currently active DPD model for the requested Tx channel's actuator
*
* Please note that this API indicates the model select that was configured last manually.
* However, in case of power based model switching the active DPD model can be obtained via
* adi_adrv9025_ExtDpdActiveModelStatusGet() API.
*
* \pre This function can be called anytime after device initialization and
*      after ARM has been loaded
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param txChannel Tx channel whose DPD model has to be retrieved 
* \param extDpdModelSel Pointer to model select enum which will be updated
*        with the currently active model
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval Add ADI_ADRV9025_ACT_ERR_RESET_CPU Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_ExtDpdActuatorModelSelectGet(adi_adrv9025_Device_t*         device,
                                                  adi_adrv9025_TxChannels_e      txChannel,
                                                  adi_adrv9025_ExtDpdModelSel_e* extDpdModelSel);

/**
* \brief This function enables/disables the LUT swap mode operation for all Tx channels
*
* \pre This function can be used to configure the LUTs in swap mode when the no. of 
*      active LUTs used are greater than 16. This function is an init time function
*      and cannot be called when the actuator is enabled
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param txChannelMask Reserved parameter.
* \param lutSwapModeEn Enable(1) or Disable(0) swap mode operation
*
* \note The value passed to param lutSwapModeEn is applied all the Tx channels regardless
*       of the value passed to the param txChannelMask. Currently, lutSwapModeEn = 1 is 
*       the only supported mode.
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_ExtDpdLutSwapModeEnableSet(adi_adrv9025_Device_t* device,
                                                uint32_t               txChannelMask,
                                                uint8_t                lutSwapModeEn);

/**
* \brief This function gets the current enables/disable state of the LUT swap mode operation for all Tx channels
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param txChannel Tx channel whose LUT swap mode enable status has to be retrieved
* \param lutSwapModeEn Pointer to a memory which will be updated with the current LUT swap mode enable value
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_ExtDpdLutSwapModeEnableGet(adi_adrv9025_Device_t*    device,
                                                adi_adrv9025_TxChannels_e txChannel,
                                                uint8_t*                  lutSwapModeEn);

/**
* \brief This function enables/disables DPD actuator clock for selected Tx channel
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param txChannel Tx channel whose LUT actuator clock will be enabled/disabled
* \param enable 1:Enable actuator clock // 0:Disable actuator clock
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_ExtDpdActuatorClkSet(adi_adrv9025_Device_t *device, adi_adrv9025_TxChannels_e txChannel, uint8_t enable);

/**
* \brief Programs the actuator LUT bank for a requested Tx channel
*
* This function programs and enables the actuator LUTs for a requested Tx Channel via SPI-AHB DMA interface.
* Each model can utilize up to 31 look-up-tables (LUT) on one specified bank. LUTs are 1k samples deep(4 kiloBytes), 
* organized as two banks of 9-bit compander tables (512 entries(2 kiloBytes)) or four banks of 8-bit compander tables (256 entries(1kiloBytes)) 
* For the 9-bit compander valid bank selections are ADI_ADRV9025_EXT_DPD_LUT_BANK0 and ADI_ADRV9025_EXT_DPD_LUT_BANK2 only.
* Each entry to the LUT is a 32 bit complex value with the MSB 16 bits consisting of imaginary part and the LSB 16 bits consisting of the real part.
*
* \pre This function can be used to program individual LUT banks when the LUT is not required to be 
*      functioning in a swap mode(i.e. when the no. of LUTs in use are less than or equal to 16).      
*      This function requires that active DPD model corresponding to the requisite LUT 
*      is configured first through adi_adrv9025_ExtDpdModelConfigSet API. 
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param txChannel Tx channel for which the actuator LUT is supposed to be programmed
* \param extDpdLutSel Target LUT (between LUT0 - LUT31) to be programmed
* \param extDpdLutBankSel Target LUT bank (bank0 - bank3) to be programmed
* \param lutData An array containing the LUT data to be programmed for the requested Tx channel's actuator
* \param lutDataArrSizeBytes Actuator LUT array size for programming (in terms of bytes)
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_ExtDpdActuatorLutWrite(adi_adrv9025_Device_t*       device,
                                            adi_adrv9025_TxChannels_e    txChannel,
                                            adi_adrv9025_ExtDpdLut_e     extDpdLutSel,
                                            adi_adrv9025_ExtDpdLutBank_e extDpdLutBankSel,
                                            uint8_t                      lutData[],
                                            uint16_t                     lutDataArrSizeBytes);

/**
* \brief Retrieves the currently programmed LUT entries for the requested Tx channel's actuator
*
* This function reads from the bank of the requested LUT of the Tx Channel's actuator
*
* \pre This function can be called anytime after device initialization and
*      after ARM has been loaded.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param txChannel Tx channel for which the actuator LUT is supposed to be retrieved
* \param extDpdLutSel Target LUT (between LUT0 - LUT31) to be retrieved
* \param extDpdLutBankSel Target LUT bank (bank0 - bank4) to be retrieved
* \param lutData LUT data array which will be updated with the currently programmed
*                LUT in the requested Tx channel's actuator (Output)
* \param lutDataArrSizeBytes Maximum no. of bytes which will be retrieved from the LUT memory
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_ExtDpdActuatorLutRead(adi_adrv9025_Device_t*       device,
                                           adi_adrv9025_TxChannels_e    txChannel,
                                           adi_adrv9025_ExtDpdLut_e     extDpdLutSel,
                                           adi_adrv9025_ExtDpdLutBank_e extDpdLutBankSel,
                                           uint8_t                      lutData[],
                                           uint16_t                     lutDataArrSizeBytes);

/**
* \brief Clears all the LUT entries
*
* This function sends a request to FW to clear all LUT entries
*
* \pre This function can be called anytime after device initialization and
*      after ARM has been loaded.
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
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_ExtDpdActuatorLutClear(adi_adrv9025_Device_t* device);

/**
* \brief Programs the actuator LUT bank for a requested Tx channel in a swap LUT mode(i.e. when more than 16 LUTs are required to be used).
*
* This function programs the actuator LUTs for a requested Tx Channel via SPI-AHB DMA interface.
* Each model can utilize up to 31 look-up-tables (LUT) on one specified bank. LUTs are 1k samples deep(4 kiloBytes), 
* organized as two banks of 9-bit compander tables (512 entries(2 kiloBytes)) or four banks of 8-bit compander tables (256 entries(1kiloBytes)) 
* In the swap LUT mode, bank2 in a 9-bit compander mode and bank3 in an 8-bit compander mode are used as swap LUTs
* and cannot be used as target banks to program LUT data. Also, ADI_ADRV9025_EXT_DPD_LUT31 cannot be used as target LUT.
* Please refer to adi_adrv9025_ExtDpdActuatorModelConfigSet() API for more information regarding the relationship b/w LUTs and the DPD model.
* Each entry to the LUT is a 32 bit complex value with the MSB 16 bits consisting of imaginary part and the LSB 16 bits consisting of the real part.
*
* \pre This function can be used to program the LUT when the LUT is required to be 
*      functioning in a swap mode(i.e. when the no. of LUTs in use are more than 16).
*      It is also required to enable the LUT in swap mode via adi_adrv9025_LutSwapModeEnableSet API.
*      This function requires that active DPD model corresponding to the requisite LUT 
*      is configured first through adi_adrv9025_ExtDpdModelConfigSet API. 
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param txChannel Tx channel for which the actuator LUT is supposed to be programmed
* \param extDpdLutBankSel Target LUT bank (bank0 - bank2) to be programmed. For a 9-bit compander only bank0 is valid.
* \param swapModeLutData Array of Swap LUT data pointers where swapModeLutData[i] corresponds to LUT[i]. In case of a sparse
*                        LUT model use case, set swapLutData[unusedLutIndex].swapModeLutData = NULL and swapLutData[unusedLutIndex].swapModeLutDataSize = 0.
*                        The user is always expected to provide a 31-deep swapModelutData array which corresponds to LUT0 - LUT30
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_ExtDpdActuatorSwapModeLutWrite(adi_adrv9025_Device_t*       device,
                                                    adi_adrv9025_TxChannels_e    txChannel,
                                                    adi_adrv9025_ExtDpdLutBank_e extDpdLutBankSel,
                                                    adi_adrv9025_SwapModeLut_t   swapModeLutData[]);

/**
* \brief Programs the actuator LUT in multi model swap mode. In this mode, LUT data is written to a temporary memory location
* in transceiver device and FW transfers this data to actual LUT memory when Tx channel is not in use. User can program LUTs up to LUT25 and
* FW selects which LUT bank to use while transferring the LUT data. In multi model swap mode, only Model0 and Model1 are supported. User is expected to write
* a 26-deep multiModelSwapModeLutData array which corresponds to LUT0 - LUT25. Prior calling this function, user is expected to run ext dpd init calibration
*
* In multi model swap mode, user is expected to follow the sequence below;
*
* 1) Write the LUT data to device by using adi_adrv9025_ExtDpdActuatorMultiModelSwapLutWrite
* 2) Program this LUT data for selected model of desired Tx channel by using adi_adrv9025_ExtDpdActuatorMultiModelSwapLutUpdate
* 3) Wait until LUT update is complete. Bank selection and LUT update status of selected model can be polled by using adi_adrv9025_ExtDpdActuatorLutUpdateStatus
* 4) Set model configuration by using adi_adrv9025_ExtDpdActuatorMultiModelSwapConfigSet
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param multiModelSwapModeLutData Array of Multi Mode swap LUT data pointers where multiModelSwapModeLutData[i] corresponds to LUT[i]. In case of a sparse
*                        LUT model use case, set multiModelSwapModeLutData[unusedLutIndex].swapModeLutData = NULL and multiModelSwapModeLutData[unusedLutIndex].swapModeLutDataSize = 0.
*                        User is always expected to provide a 26-deep swapModelutData array which corresponds to LUT0 - LUT25
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_ExtDpdActuatorMultiModelSwapLutWrite(adi_adrv9025_Device_t *device,
                                                          adi_adrv9025_SwapModeLut_t multiModelSwapModeLutData[]);


/**
* \brief Updates the actuator LUTs in multi model swap mode. After writing the LUT data to transceiver by using adi_adrv9025_ExtDpdActuatorMultiModelSwapLutWrite,
*  user needs to update LUTs for selected Tx channel and model with this function. In multi model swap mode, only Model0 and Model1 are supported.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param txChannel Target Tx channel to update LUTs
* \param modelSel Model selection for which LUTs will be updated
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_ExtDpdActuatorMultiModelSwapLutUpdate(adi_adrv9025_Device_t *device,
                                                           adi_adrv9025_TxChannels_e txChannel,
                                                           adi_adrv9025_ExtDpdModelSel_e modelSel);


/**
* \brief This functions reads the LUT update status from ARM in multi model swap mode. User needs to wait
* until LUT update is complete, after they trigger LUT update with adi_adrv9025_ExtDpdActuatorMultiModelSwapLutUpdate.
* Once LUT update is complete, user can configure DPD model with adi_adrv9025_ExtDpdActuatorMultiModelSwapConfigSet function.
*
* This function also reads which bank is selected by FW to store the LUT data for selected Tx channel and model
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param txChannel Target Tx channel to read LUT update status and bank index
* \param updateStatus Pointer to LUT update status readback value
* \param bankIndex Pointer to bank index readback value
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_ExtDpdActuatorMultiModelSwapLutUpdateStatusGet(adi_adrv9025_Device_t *device,
                                                                    adi_adrv9025_TxChannels_e txChannel,
                                                                    uint8_t *updateStatus,
                                                                    adi_adrv9025_ExtDpdLutBank_e *bankIndex);

/**
* \brief Configures the DPD model for a selected Tx channel's actuator in multi model swap mode. Each model can utilize up to 26 LUTs (LUT[0-25])
* and only Model0 and Model1 are supported in this mode. Please note, adi_adrv9025_ExtDpdModel_t.extDpdActuatorCompanderSize
* adi_adrv9025_ExtDpdModel_t.extDpdLutBankSel variables are ignored in multi model swap mode.
*
* This function commands the ARM to configure the DPD model settings for a selected Tx Channel's actuator.
* The GMP model is defined by summation(a(i,j,k) * |x(n-i)|^k * x(n-j)) for i,j,k in the range 0 to 15 where a(i,j,k) is the
* complex co-efficient, x(n) is the input, i is the memory term, j is the cross term and k is the power term.
*
* The GMP model to LUT mapping restrictions are shown below for a given memory term(i) represented by adi_adrv9025_ExtDpdModel_t.extDpdISel
* and the cross term(j) represented by adi_adrv9025_ExtDpdModel_t.extDpdJSel. This function can be used to select the target LUTs,
* model select and configure the memory term(i) for the target LUTs of the model and the cross term(j) for each row of LUTs selected in the model.
* Please note that the LUTs in the same row shown below share the same j(cross term) value. The roaming LUTs are a special
* case where it can be assigned to one of the 4 j columns via adi_adrv9025_ExtDpdModel_t.extDpdLutISum. Each LUT consists of
* the partial sum of products of summation(a(i,j,k) * |x(n-i)|^k) for k in the range 0 to 15.
*
* Each entry to the LUT is a 32 bit complex value with the MSB 16 bits consisting of imaginary part and the LSB 16 bits consisting of the real part
*
*       j' <= 8 |  LUT0  |  LUT1  |  LUT2  |  LUT3  |  LUT4  |  LUT5  |  LUT6  |  LUT7  |  LUT26  |  LUT27  |
*               | (i<=8) | (i<=8) | (i<=10)| (i<=11)| (i<=12)| (i<=13)| (i<=14)| (i<=15)| (i<=15) | (i<=15) |
*     ----------|--------|--------|--------|--------|--------|--------|--------|--------|(Roaming |(Roaming |
*       j' <= 9 |  LUT8  |  LUT9  |  LUT10 |  LUT11 |  LUT12 |  LUT13 |        |        |  LUTA)  |  LUTB)  |
*               | (i<=10)| (i<=11)| (i<=12)| (i<=13)| (i<=14)| (i<=15)|        |        |   NOT   |   NOT   |
*     ----------|--------|--------|--------|--------|--------|--------|--------|--------|SUPPORTED|SUPPORTED|
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
*               | (i<=15)|        |        |        |        |        |        |        |   NOT   |   NOT   |
*     ----------|--------|--------|--------|--------|--------|--------|--------|--------|SUPPORTED|SUPPORTED|
*       j' <= 14|  LUT24 |        |        |        |        |        |        |        |         |         |
*               | (i<=15)|        |        |        |        |        |        |        |         |         |
*     ----------|--------|--------|--------|--------|--------|--------|--------|--------|         |         |
*       j' <= 15|  LUT25 |        |        |        |        |        |        |        |         |         |
*               | (i<=15)|        |        |        |        |        |        |        |         |         |
*
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param txChannel Target Tx channel for which the DPD model is required to be configured
* \param extDpdModelConfig Pointer to external DPD model config params (Input)
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval Add ADI_ADRV9025_ACT_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_ExtDpdActuatorMultiModelSwapModelConfigSet(adi_adrv9025_Device_t *device,
                                                                adi_adrv9025_TxChannels_e txChannel,
                                                                adi_adrv9025_ExtDpdModel_t *extDpdModelConfig);

/**
* \brief Retrieves DPD model configuration in multi model swap mode.Only Model0 and Model1 are supported in this mode.
* User is expected to populate adi_adrv9025_ExtDpdModel_t.extDpdModelSel (either Model0 or Model1) to select the model which configuration
* will be read from. Each model can utilize up to 26 LUTs (LUT[0-25]). Please note, adi_adrv9025_ExtDpdModel_t.extDpdActuatorCompanderSize
* adi_adrv9025_ExtDpdModel_t.extDpdLutBankSel variables are ignored in multi model swap mode.
*
* \pre This function can be called anytime after device initialization and
*      after ARM has been loaded
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param txChannel Target Tx channel whose DPD model settings are requested to be retrieved
* \param extDpdModelConfig Pointer to external DPD model config params
*                          which will be updated with current DPD model settings (Output)
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval Add ADI_ADRV9025_ACT_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_ExtDpdActuatorMultiModelSwapModelConfigGet(adi_adrv9025_Device_t *device,
                                                                adi_adrv9025_TxChannels_e txChannel,
                                                                adi_adrv9025_ExtDpdModel_t *extDpdModelConfig);

/**
* \brief Retrieves the active DPD model that is autoloaded by the power level of DPD actuator output
*
* In a gain monitoring mode or envelope based capture, it is possible for the user to
* configure automatic loading of a DPD model when a power limit is violated. This API
* enables the user to read back the currently active dpd model.
*
* \pre This function can be called anytime after initialization and loading of ARM is complete
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param txChannel Target Tx channel whose active DPD model is requested
* \param extDpdActiveModelSel Pointer to the ADRV9025 external DPD model select enum
*                             which will be updated with the currently active dpd model
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval Add ADI_ADRV9025_ACT_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_ExtDpdActiveModelStatusGet(adi_adrv9025_Device_t*         device,
                                                adi_adrv9025_TxChannels_e      txChannel,
                                                adi_adrv9025_ExtDpdModelSel_e* extDpdActiveModelSel);

/**
* \brief Configures the external DPD gain monitoring thresholds and model selects on gain threshold violation
*
* The power monitors in the DPD actuator can be used to monitor the DPD output gain and optionally switch to
* a pre-configured DPD model select on gain threshold violation.
*
* An upper threshold limit violation is triggered if DPD_OUT > (DPD_IN x adi_adrv9025_ExtDpdGainMonitorThresh_t.extDpdGainMonitorUpperThresh) 
* which is then used to auto load a model specified by adi_adrv9025_ExtDpdGainMonitorCtrl_t.extDpdGainMonitorUpperThreshModelSel
*
* A lower threshold limit violation is triggered if DPD_OUT < (DPD_IN x adi_adrv9025_ExtDpdGainMonitorThresh_t.extDpdGainMonitorLowerThresh)
* which is then used to auto load a model specified by adi_adrv9025_ExtDpdGainMonitorCtrl_t.extDpdGainMonitorLowerThreshModelSel
*
* The active model status at any point of time can be obtained by adi_adrv9025_ExtDpdActiveModelStatusGet() API
*
* \pre This function can be called anytime after initialization and loading of ARM is complete
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param extDpdActGainMonitorConfig An array of DPD actuator gain monitor settings which will be applied to the DPD actuator 
*                                of the requested Tx channel
* \param numGainMonitorConfigs No. of configs(array size) of the DPD actuator gain monitor config array
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADRV9025_ACT_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_ExtDpdActuatorGainMonitorConfigSet(adi_adrv9025_Device_t*                    device,
                                                        adi_adrv9025_ExtDpdActGainMonitorConfig_t extDpdActGainMonitorConfig[],
                                                        uint8_t                                   numGainMonitorConfigs);

/**
* \brief Retrieves the current DPD actuator gain monitor settings.
*
* This function can be used to retrieve the currently active DPD actuator gain monitor settings in the device
*
* \pre This function can be called anytime after initialization and loading of ARM is complete
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param txChannel The target Tx channel whose dpd actuator gain monitoring settings are requested
* \param extDpdActGainMonitorConfig Pointer to memory which will be updated with the active DPD gain monitor settings in the device
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval Add ADI_ADRV9025_ACT_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_ExtDpdActuatorGainMonitorConfigGet(adi_adrv9025_Device_t*                     device,
                                                        adi_adrv9025_TxChannels_e                  txChannel,
                                                        adi_adrv9025_ExtDpdActGainMonitorConfig_t* extDpdActGainMonitorConfig);

/**
* \brief Enables/disables the post DPD hard clipper and sets up the hard clipper threshold for the requested tx channels
*
* The DPD hard clipper is placed after the DPD actuator and is intended as a fail-safe power amplifier 
* protection feature that prevents high DPD gain expansion peak from damaging power amplifiers.
* The DPD hard clipper threshold is applied to an approximation of SQRT(I^2 + Q^2). The thresholdis 
* normalized to 1 and is relative to 0dBFS which means that a threshold of 1 corresponds to a 
* threshold of 0dBFS. 
*
* The hard clipper has a dependency on the DPD actuator being enabled in order to function.
* Typically, this API can be called to setup the hard clipper before enabling the DPD actuator.
*
* \pre This function can be called anytime after initialization and loading of ARM is complete
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param extDpdHardClipperConfig An array of DPD hard clipper config structures
* \param extDpdHardClipperCfgArraySize No. of configs in extDpdHardClipperConfig array
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval Add ADI_ADRV9025_ACT_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_ExtDpdHardClipperConfigSet(adi_adrv9025_Device_t*                 device,
                                                adi_adrv9025_ExtDpdHardClipperConfig_t extDpdHardClipperConfig[],
                                                uint8_t                                extDpdHardClipperCfgArraySize);

/**
* \brief This function retrieves the DPD Hard clipper setting for the requested Tx channel
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function may be called any time after device initialization and the ARM boot up is complete
*
* \param device Pointer to the device settings structure
* \param txChannel Target Tx channel for which the DPD Hard clipper status is requested
* \param extDpdHardClipperConfig Pointer to the DPD hard clipper config which will be updated with the device
*                             hard clipper config settings
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADI_COMMON_ACT_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_ExtDpdHardClipperConfigGet(adi_adrv9025_Device_t*                  device,
                                                adi_adrv9025_TxChannels_e               txChannel,
                                                adi_adrv9025_ExtDpdHardClipperConfig_t* extDpdHardClipperConfig);

/**
* \brief Resets the DPD hard clipper counter
* 
* This function can be used to reset the DPD hard clipper counter. The counter is
* accumulated in a 32 bit register. When the  counter is sufficiently high, 
* the user can reset the counter through this function to avoid overflow of the 
* register. 
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function may be called any time after device initialization and the ARM boot up is complete
*
* \param device Pointer to the device settings structure
* \param txChannelMask A mask consisting of 'OR'ed tx channels for which the DPD hard clipper counters are required to be reset.
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADI_COMMON_ACT_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_ExtDpdHardClipperCounterReset(adi_adrv9025_Device_t* device,
                                                   uint32_t               txChannelMask);

/**
* \brief Returns the no. of samples clipped by the post DPD actuator hard clipper
* 
* The post DPD hard clipper counter can be used to monitor the health of the DPD output
* and is accumulated in a 32 bit register. When the counter is sufficiently high, 
* the user can reset the counter through adi_adrv9025_ExtDpdHardClipperCounterReset() 
* to avoid overflow of the register. 
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function may be called any time after device initialization and the ARM boot up is complete
*
* \param device Pointer to the device settings structure
* \param txChannel Tx channel for which the DPD hard clipper counters are required to be obtained.
* \param counter Pointer to where the counter will be returned
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_ADI_COMMON_ACT_ERR_RESET_ARM Recovery action for ARM reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_ExtDpdHardClipperCounterGet(adi_adrv9025_Device_t*    device,
                                                 adi_adrv9025_TxChannels_e txChannel,
                                                 uint32_t*                 counter);

/**
* \brief Enables/Disables the actuator passthrough mode for selected Tx channel. When passthrough mode is enabled,
* unity gain is applied in DPD actuator, i.e. DPD actuator output = DPD actuator input
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param txChannelMask Tx channel mask to select tx channel(s) for which the actuator passthru state is required to be set
* \param actuatorPassthruEn Flag to set DPD actuator passthru mode 1:Passthru mode Enable or 0:Passthru mode Disable
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_ExtDpdActutatorPassthruSet(adi_adrv9025_Device_t *device, uint32_t txChannelMask, uint8_t actuatorPassthruEn);

/**
* \brief Retrieves the current state of actuator passthru mode for the requested Tx channel's actuator.
* When passthrough mode is enabled, unity gain is applied in DPD actuator, i.e. DPD actuator output = DPD actuator input
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device settings data structure
* \param txChannel Tx channel for which the passthru state is requested
* \param actuatorPassthruEnRead Returns a 1-Passthru mode Enabled or 0-Passthru mode Disabled
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adi_adrv9025_ExtDpdActutatorPassthruGet(adi_adrv9025_Device_t *device, adi_adrv9025_TxChannels_e txChannel, uint8_t *actuatorPassthruEnRead);

#ifdef __cplusplus
}
#endif

#endif /* _ADI_ADRV9025_EXTERRNAL_DPD_H_ */
