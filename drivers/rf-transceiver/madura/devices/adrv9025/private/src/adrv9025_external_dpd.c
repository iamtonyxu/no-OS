/**
* \file adrv9025_external_dpd.c
* \brief Contains ADRV9025 external dpd related private function implementations
*
* ADRV9025 API Version: $ADI_ADRV9025_API_VERSION$
*/

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the ADRV9025 API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

#include "adi_adrv9025_user.h"
#include "adi_adrv9025_cpu.h"
#include "adi_adrv9025_error.h"
#include "adi_adrv9025_gpio.h"
#include "adi_adrv9025_hal.h"
#include "../../private/include/adrv9025_external_dpd.h"
#include "../../private/include/adrv9025_cpu_macros.h"
#include "../../private/include/adrv9025_reg_addr_macros.h"
#include "../../private/include/adrv9025_cpu.h"
#include "../../private/include/adrv9025_shared_resource_manager.h"
#include "../../private/include/adrv9025_radioctrl.h"
#include "../../private/include/adrv9025_bf_core.h"

int32_t adrv9025_ExtDpdCaptureConfigSetRangeCheck(adi_adrv9025_Device_t*              device,
                                                  adi_adrv9025_ExtDpdCaptureConfig_t* dpdCaptureConfig)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /* Check that dpdCaptureConfig is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        dpdCaptureConfig);

    /* Check that dpd capture ctrl parameters are in range */
    recoveryAction = adrv9025_ExtDpdCaptureCtrlConfigSetRangeCheck(device,
                                                                   &(dpdCaptureConfig->extDpdCaptureCtrl));
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Check that dpd capture level parameters are in range */
    recoveryAction = adrv9025_ExtDpdCaptureLevelConfigSetRangeCheck(device,
                                                                    &(dpdCaptureConfig->extDpdCaptureLevel));
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Check that dpd capture path delay parameters are in range */
    recoveryAction = adrv9025_ExtDpdCapturePathDelayRangeCheck(device,
                                                               &(dpdCaptureConfig->pathDelay));
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Check that power measurement thresholds are in range */
    recoveryAction = adrv9025_ExtDpdCapturePwrThreshSetRangeCheck(device,
                                                                  ADI_ADRV9025_EXT_DPD_POWER_BAND_01,
                                                                  &(dpdCaptureConfig->extDpdCapPwrThreshBand01));
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_ExtDpdCapturePwrThreshSetRangeCheck(device,
                                                                  ADI_ADRV9025_EXT_DPD_POWER_BAND_12,
                                                                  &(dpdCaptureConfig->extDpdCapPwrThreshBand12));
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_ExtDpdCapturePwrThreshSetRangeCheck(device,
                                                                  ADI_ADRV9025_EXT_DPD_POWER_BAND_23,
                                                                  &(dpdCaptureConfig->extDpdCapPwrThreshBand23));
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Check that the GPIO pin for triggering DPD capture is valid and not already in use */
    recoveryAction = adrv9025_ExtDpdCaptureTriggerGpioRangeCheck(device,
                                                                 dpdCaptureConfig->extDpdCaptureTriggerPin);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Check that the GPIO pin for triggering DPD capture is valid and not already in use */
    recoveryAction = adrv9025_ExtDpdCaptureDoneStatusPinRangeCheck(device,
                                                                   dpdCaptureConfig->extDpdCaptureDoneStatusPin);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Check that capture trigger and capture done signals are not assigned to the same GPIO */
    if ((dpdCaptureConfig->extDpdCaptureTriggerPin != ADI_ADRV9025_GPIO_INVALID) &&
        (dpdCaptureConfig->extDpdCaptureDoneStatusPin != ADI_ADRV9025_GPIO_INVALID) &&
        (dpdCaptureConfig->extDpdCaptureTriggerPin == dpdCaptureConfig->extDpdCaptureDoneStatusPin))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         dpdCaptureConfig->extDpdCaptureTriggerPin,
                         "DPD sample capture trigger and DPD capture done indicator signals assigned to the same GPIO pin");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return recoveryAction;
}

int32_t adrv9025_ExtDpdCaptureTriggerGpioRangeCheck(adi_adrv9025_Device_t*    device,
                                                    adi_adrv9025_GpioPinSel_e extDpdCaptureTriggerPin)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    if ((extDpdCaptureTriggerPin != ADI_ADRV9025_GPIO_INVALID) &&
        (extDpdCaptureTriggerPin != ADI_ADRV9025_GPIO_00) &&
        (extDpdCaptureTriggerPin != ADI_ADRV9025_GPIO_01) &&
        (extDpdCaptureTriggerPin != ADI_ADRV9025_GPIO_02) &&
        (extDpdCaptureTriggerPin != ADI_ADRV9025_GPIO_03) &&
        (extDpdCaptureTriggerPin != ADI_ADRV9025_GPIO_04) &&
        (extDpdCaptureTriggerPin != ADI_ADRV9025_GPIO_05) &&
        (extDpdCaptureTriggerPin != ADI_ADRV9025_GPIO_06) &&
        (extDpdCaptureTriggerPin != ADI_ADRV9025_GPIO_07) &&
        (extDpdCaptureTriggerPin != ADI_ADRV9025_GPIO_08) &&
        (extDpdCaptureTriggerPin != ADI_ADRV9025_GPIO_09) &&
        (extDpdCaptureTriggerPin != ADI_ADRV9025_GPIO_10) &&
        (extDpdCaptureTriggerPin != ADI_ADRV9025_GPIO_11) &&
        (extDpdCaptureTriggerPin != ADI_ADRV9025_GPIO_12) &&
        (extDpdCaptureTriggerPin != ADI_ADRV9025_GPIO_13) &&
        (extDpdCaptureTriggerPin != ADI_ADRV9025_GPIO_14) &&
        (extDpdCaptureTriggerPin != ADI_ADRV9025_GPIO_15))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         extDpdCaptureTriggerPin,
                         "Invalid DPD sample capture trigger GPIO pin selected. GPIO00-GPIO15 are valid values. For non-gpio mode, please select GPIO_INVALID");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return recoveryAction;
}

int32_t adrv9025_ExtDpdCaptureDoneStatusPinRangeCheck(adi_adrv9025_Device_t*    device,
                                                      adi_adrv9025_GpioPinSel_e extDpdCaptureDoneStatusPin)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    if ((extDpdCaptureDoneStatusPin != ADI_ADRV9025_GPIO_INVALID) &&
        (extDpdCaptureDoneStatusPin != ADI_ADRV9025_GPIO_00) &&
        (extDpdCaptureDoneStatusPin != ADI_ADRV9025_GPIO_01) &&
        (extDpdCaptureDoneStatusPin != ADI_ADRV9025_GPIO_02) &&
        (extDpdCaptureDoneStatusPin != ADI_ADRV9025_GPIO_03) &&
        (extDpdCaptureDoneStatusPin != ADI_ADRV9025_GPIO_04) &&
        (extDpdCaptureDoneStatusPin != ADI_ADRV9025_GPIO_05) &&
        (extDpdCaptureDoneStatusPin != ADI_ADRV9025_GPIO_06) &&
        (extDpdCaptureDoneStatusPin != ADI_ADRV9025_GPIO_07) &&
        (extDpdCaptureDoneStatusPin != ADI_ADRV9025_GPIO_08) &&
        (extDpdCaptureDoneStatusPin != ADI_ADRV9025_GPIO_09) &&
        (extDpdCaptureDoneStatusPin != ADI_ADRV9025_GPIO_10) &&
        (extDpdCaptureDoneStatusPin != ADI_ADRV9025_GPIO_11) &&
        (extDpdCaptureDoneStatusPin != ADI_ADRV9025_GPIO_12) &&
        (extDpdCaptureDoneStatusPin != ADI_ADRV9025_GPIO_13) &&
        (extDpdCaptureDoneStatusPin != ADI_ADRV9025_GPIO_14) &&
        (extDpdCaptureDoneStatusPin != ADI_ADRV9025_GPIO_15) &&
        (extDpdCaptureDoneStatusPin != ADI_ADRV9025_GPIO_16) &&
        (extDpdCaptureDoneStatusPin != ADI_ADRV9025_GPIO_17) &&
        (extDpdCaptureDoneStatusPin != ADI_ADRV9025_GPIO_18))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         extDpdCaptureDoneStatusPin,
                         "Invalid DPD sample capture done indicator GPIO pin selected. GPIO00-GPIO18 are the valid values. For non-gpio mode, please select GPIO_INVALID");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return recoveryAction;
}

int32_t adrv9025_ExtDpdCapturePathDelayRangeCheck(adi_adrv9025_Device_t*                 device,
                                                  adi_adrv9025_ExtDpdCapturePathDelay_t* dpdPathDelay)
{
    static uint8_t MAX_INTERPOLATION_INDEX = 15;
    int32_t        recoveryAction          = ADI_COMMON_ACT_NO_ACTION;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /* Check that dpdPathDelay is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        dpdPathDelay);

    if (dpdPathDelay->extDpdCaptureInterpolationIndex > MAX_INTERPOLATION_INDEX)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         dpdPathDelay->extDpdCaptureInterpolationIndex,
                         "Invalid DPD Tx-Orx capture path interpolation index. Range is between 0-15");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return recoveryAction;
}

int32_t adrv9025_ExtDpdActuatorGainMonitorConfigSetRangeCheck(adi_adrv9025_Device_t*                    device,
                                                              adi_adrv9025_ExtDpdActGainMonitorConfig_t extDpdActGainMonitorConfig[],
                                                              uint8_t                                   numGainMonitorConfigs)
{
    static const uint8_t MAX_GAIN_MONITOR_IIR_DECAY = 15;
    int32_t              recoveryAction             = ADI_COMMON_ACT_NO_ACTION;
    uint8_t              gainMonitorConfigIndex     = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /* Check that extDpdActGainMonitorConfig is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        extDpdActGainMonitorConfig);

    for (gainMonitorConfigIndex = 0; gainMonitorConfigIndex < numGainMonitorConfigs; gainMonitorConfigIndex++)
    {
        if (extDpdActGainMonitorConfig[gainMonitorConfigIndex].extDpdGainMonitorCtrl.extDpdGainMonitorEn > 0)
        {
            if ((extDpdActGainMonitorConfig[gainMonitorConfigIndex].extDpdGainMonitorCtrl.extDpdGainMonitorLowerThreshModelSel != ADI_ADRV9025_EXT_DPD_MODEL0)
                &&
                (extDpdActGainMonitorConfig[gainMonitorConfigIndex].extDpdGainMonitorCtrl.extDpdGainMonitorLowerThreshModelSel != ADI_ADRV9025_EXT_DPD_MODEL1)
                &&
                (extDpdActGainMonitorConfig[gainMonitorConfigIndex].extDpdGainMonitorCtrl.extDpdGainMonitorLowerThreshModelSel != ADI_ADRV9025_EXT_DPD_MODEL2)
                &&
                (extDpdActGainMonitorConfig[gainMonitorConfigIndex].extDpdGainMonitorCtrl.extDpdGainMonitorLowerThreshModelSel != ADI_ADRV9025_EXT_DPD_MODEL3))
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 extDpdActGainMonitorConfig[gainMonitorConfigIndex].extDpdGainMonitorCtrl.extDpdGainMonitorLowerThreshModelSel,
                                 "Invalid DPD gain monitor underrange target model selected. Valid models are 0-3");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            if ((extDpdActGainMonitorConfig[gainMonitorConfigIndex].extDpdGainMonitorCtrl.extDpdGainMonitorUpperThreshModelSel != ADI_ADRV9025_EXT_DPD_MODEL0)
                &&
                (extDpdActGainMonitorConfig[gainMonitorConfigIndex].extDpdGainMonitorCtrl.extDpdGainMonitorUpperThreshModelSel != ADI_ADRV9025_EXT_DPD_MODEL1)
                &&
                (extDpdActGainMonitorConfig[gainMonitorConfigIndex].extDpdGainMonitorCtrl.extDpdGainMonitorUpperThreshModelSel != ADI_ADRV9025_EXT_DPD_MODEL2)
                &&
                (extDpdActGainMonitorConfig[gainMonitorConfigIndex].extDpdGainMonitorCtrl.extDpdGainMonitorUpperThreshModelSel != ADI_ADRV9025_EXT_DPD_MODEL3))
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 extDpdActGainMonitorConfig[gainMonitorConfigIndex].extDpdGainMonitorCtrl.extDpdGainMonitorUpperThreshModelSel,
                                 "Invalid DPD gain monitor overrange target model selected. Valid models are 0-3");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            if (extDpdActGainMonitorConfig[gainMonitorConfigIndex].extDpdGainMonitorCtrl.extDpdGainMonitorIIRDecay > MAX_GAIN_MONITOR_IIR_DECAY)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 extDpdActGainMonitorConfig[gainMonitorConfigIndex].extDpdGainMonitorCtrl.extDpdGainMonitorIIRDecay,
                                 "Invalid DPD gain monitor IIR decay rate selected. Range is between 0-15");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            if (extDpdActGainMonitorConfig[gainMonitorConfigIndex].extDpdGainMonitorThresh.extDpdGainMonitorLowerThresh > extDpdActGainMonitorConfig
                                                                                                                          ->extDpdGainMonitorThresh.
                                                                                                                          extDpdGainMonitorUpperThresh)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 extDpdActGainMonitorConfig[gainMonitorConfigIndex].extDpdGainMonitorThresh.extDpdGainMonitorLowerThresh,
                                 "The lower gain violation threshold exceeds the upper gain violation threshold");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
        }
    }

    return recoveryAction;
}

int32_t adrv9025_ExtDpdCaptureCtrlConfigSetRangeCheck(adi_adrv9025_Device_t*            device,
                                                      adi_adrv9025_ExtDpdCaptureCtrl_t* dpdCaptureCtrl)
{
    static uint8_t  MAX_PEAK_DECAY       = 15;
    static uint32_t MAX_PEAK_WIN_COUNTER = 16777215;
    static uint32_t MAX_EXTENDED_PEAK_SEARCH_WINDOW = 15000000;
    static uint16_t MAX_DPD_CAP2DLY = 8191;
    int32_t         recoveryAction       = ADI_COMMON_ACT_NO_ACTION;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /* Check that dpdCaptureCtrl is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        dpdCaptureCtrl);

    if (dpdCaptureCtrl->dpdPeakCaptureConfig.extDpdPeakDecay > MAX_PEAK_DECAY)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         dpdCaptureCtrl->dpdPeakCaptureConfig.extDpdPeakDecay,
                         "Invalid DPD Peak Detection IIR decay rate selected. Range is between 0-15");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (dpdCaptureCtrl->dpdPeakCaptureConfig.extDpdPeakWinCnt > MAX_PEAK_WIN_COUNTER)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         dpdCaptureCtrl->dpdPeakCaptureConfig.extDpdPeakWinCnt,
                         "Invalid DPD Peak Detection window counter selected. Range is between 0-16777215");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((dpdCaptureCtrl->extDpdCapturetxChannelSel != ADI_ADRV9025_TX1) &&
        (dpdCaptureCtrl->extDpdCapturetxChannelSel != ADI_ADRV9025_TX2) &&
        (dpdCaptureCtrl->extDpdCapturetxChannelSel != ADI_ADRV9025_TX3) &&
        (dpdCaptureCtrl->extDpdCapturetxChannelSel != ADI_ADRV9025_TX4))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         dpdCaptureCtrl->extDpdCapturetxChannelSel,
                         "Invalid Tx Channel selected for DPD sample capture. Tx1-Tx4 are the valid Tx channels.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((dpdCaptureCtrl->dpdCaptureTypeSel != ADI_ADRV9025_EXT_DPD_CAPTURE_IMMEDIATE_TRIGGER) &&
        (dpdCaptureCtrl->dpdCaptureTypeSel != ADI_ADRV9025_EXT_DPD_CAPTURE_POWER_LEVEL_TRIGGER) &&
        (dpdCaptureCtrl->dpdCaptureTypeSel != ADI_ADRV9025_EXT_DPD_CAPTURE_PEAK_DETECTION) &&
        (dpdCaptureCtrl->dpdCaptureTypeSel != ADI_ADRV9025_EXT_DPD_CAPTURE_TDD_TXON_SYNC) &&
        (dpdCaptureCtrl->dpdCaptureTypeSel != ADI_ADRV9025_EXT_DPD_CAPTURE_EXTENDED_PEAK_SEARCH))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         dpdCaptureCtrl->dpdCaptureTypeSel,
                         "Invalid DPD sample capture type selected. Immediate/Pwr Level capture/Peak detect capture/Tdd Tx On Sync/Extended peak detect capture are the only valid modes.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((dpdCaptureCtrl->dpdCaptureTxObsSel != ADI_ADRV9025_EXT_DPD_CAPTURE_TX_OBS_POST_DPD_ACTUATOR) &&
        (dpdCaptureCtrl->dpdCaptureTxObsSel != ADI_ADRV9025_EXT_DPD_CAPTURE_TX_OBS_PRE_DPD_ACTUATOR))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         dpdCaptureCtrl->dpdCaptureTxObsSel,
                         "Invalid DPD sample capture Tx observation point selected. Pre/Post DPD actuator are the valid observation points");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((dpdCaptureCtrl->dpdCaptureTxAltObsSel != ADI_ADRV9025_EXT_DPD_CAPTURE_TX_ALT_OBS_DPD_ACTUATOR) &&
        (dpdCaptureCtrl->dpdCaptureTxAltObsSel != ADI_ADRV9025_EXT_DPD_CAPTURE_TX_ALT_OBS_PRE_CFR))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         dpdCaptureCtrl->dpdCaptureTxAltObsSel,
                         "Invalid DPD sample capture Tx alternate observation point selected. DPD actuator/Pre-CFR are the valid observation points");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((dpdCaptureCtrl->dpdCaptureSize != ADI_ADRV9025_EXT_DPD_CAPTURE_SIZE_32_SAMPLES) &&
        (dpdCaptureCtrl->dpdCaptureSize != ADI_ADRV9025_EXT_DPD_CAPTURE_SIZE_64_SAMPLES) &&
        (dpdCaptureCtrl->dpdCaptureSize != ADI_ADRV9025_EXT_DPD_CAPTURE_SIZE_128_SAMPLES) &&
        (dpdCaptureCtrl->dpdCaptureSize != ADI_ADRV9025_EXT_DPD_CAPTURE_SIZE_256_SAMPLES) &&
        (dpdCaptureCtrl->dpdCaptureSize != ADI_ADRV9025_EXT_DPD_CAPTURE_SIZE_512_SAMPLES) &&
        (dpdCaptureCtrl->dpdCaptureSize != ADI_ADRV9025_EXT_DPD_CAPTURE_SIZE_1024_SAMPLES) &&
        (dpdCaptureCtrl->dpdCaptureSize != ADI_ADRV9025_EXT_DPD_CAPTURE_SIZE_2048_SAMPLES) &&
        (dpdCaptureCtrl->dpdCaptureSize != ADI_ADRV9025_EXT_DPD_CAPTURE_SIZE_4096_SAMPLES))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         dpdCaptureCtrl->dpdCaptureSize,
                         "Invalid DPD sample capture size selected. Valid sizes are 2^N where N = 5 to 12");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (dpdCaptureCtrl->extDpdCaptureAlignDelay > MAX_DPD_CAP2DLY)
    {
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM, ADI_COMMON_ACT_ERR_CHECK_PARAM, dpdCaptureCtrl->extDpdCaptureAlignDelay,
                         "Invalid DPD capture align delay selected. Valid range is between 0 - 8191");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (dpdCaptureCtrl->extDpdCaptureDelay > MAX_PEAK_WIN_COUNTER)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         dpdCaptureCtrl->extDpdCaptureDelay,
                         "Invalid DPD Peak Detection delay count. Range is between 0-16777215");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (((dpdCaptureCtrl->dpdCaptureTypeSel == ADI_ADRV9025_EXT_DPD_CAPTURE_PEAK_DETECTION) || (dpdCaptureCtrl->dpdCaptureTypeSel == ADI_ADRV9025_EXT_DPD_CAPTURE_EXTENDED_PEAK_SEARCH)) &&
        (dpdCaptureCtrl->dpdPeakCaptureConfig.extDpdPeakCaptureInputSel != ADI_ADRV9025_EXT_DPD_CAPTURE_PEAK_DET_DPD_INPUT) &&
        (dpdCaptureCtrl->dpdPeakCaptureConfig.extDpdPeakCaptureInputSel != ADI_ADRV9025_EXT_DPD_CAPTURE_PEAK_DET_CFR_INPUT))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         dpdCaptureCtrl->dpdPeakCaptureConfig.extDpdPeakCaptureInputSel,
                         "Invalid Ext DPD peak detect capture input point selected");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((dpdCaptureCtrl->dpdCaptureTypeSel == ADI_ADRV9025_EXT_DPD_CAPTURE_EXTENDED_PEAK_SEARCH) &&
        (dpdCaptureCtrl->dpdPeakCaptureConfig.extDpdPeakExtendedWindowUs > MAX_EXTENDED_PEAK_SEARCH_WINDOW))
    {
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM, ADI_COMMON_ACT_ERR_CHECK_PARAM, dpdCaptureCtrl->dpdPeakCaptureConfig.extDpdPeakExtendedWindowUs,
                         "Invalid DPD Peak Detection extended peak window size. Range is between 0-15000000");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return recoveryAction;
}

int32_t adrv9025_ExtDpdCaptureLevelConfigSetRangeCheck(adi_adrv9025_Device_t*             device,
                                                       adi_adrv9025_ExtDpdCaptureLevel_t* dpdCaptureLevel)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /* Check that dpdCaptureLevel is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        dpdCaptureLevel);

    if ((dpdCaptureLevel->dpdCaptureBandDir != ADI_ADRV9025_EXT_DPD_CAPTURE_BAND_REACHED_ASCENDING) &&
        (dpdCaptureLevel->dpdCaptureBandDir != ADI_ADRV9025_EXT_DPD_CAPTURE_BAND_REACHED_DESCENDING) &&
        (dpdCaptureLevel->dpdCaptureBandDir != ADI_ADRV9025_EXT_DPD_CAPTURE_BAND_REACHED_ANY_ORDER))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         dpdCaptureLevel->dpdCaptureBandDir,
                         "Invalid DPD sample capture level target band dir selected. 0-ANY, 2-ASC, 3-DESC are the valid values");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((dpdCaptureLevel->dpdCaptureBandSel != ADI_ADRV9025_EXT_DPD_CAPTURE_POWER_BAND_0) &&
        (dpdCaptureLevel->dpdCaptureBandSel != ADI_ADRV9025_EXT_DPD_CAPTURE_POWER_BAND_1) &&
        (dpdCaptureLevel->dpdCaptureBandSel != ADI_ADRV9025_EXT_DPD_CAPTURE_POWER_BAND_2) &&
        (dpdCaptureLevel->dpdCaptureBandSel != ADI_ADRV9025_EXT_DPD_CAPTURE_POWER_BAND_3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         dpdCaptureLevel->dpdCaptureBandSel,
                         "Invalid DPD sample capture level target band dir selected. Power Bands 0/1/2/3 are the valid bands");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return recoveryAction;
}

int32_t adrv9025_ExtDpdCapturePwrThreshSetRangeCheck(adi_adrv9025_Device_t*                 device,
                                                     adi_adrv9025_ExtDpdPwrThreshBandPair_e targetDpdCaptureBandSel,
                                                     adi_adrv9025_ExtDpdCapturePwrThresh_t* dpdCapturePwrThresh)
{
    static uint16_t MAX_PWR_PULSE_WIDTH = 4096;
    int32_t         recoveryAction      = ADI_COMMON_ACT_NO_ACTION;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /* Check that dpdCapturePwrThresh is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        dpdCapturePwrThresh);

    if ((targetDpdCaptureBandSel != ADI_ADRV9025_EXT_DPD_POWER_BAND_01) &&
        (targetDpdCaptureBandSel != ADI_ADRV9025_EXT_DPD_POWER_BAND_12) &&
        (targetDpdCaptureBandSel != ADI_ADRV9025_EXT_DPD_POWER_BAND_23))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         targetDpdCaptureBandSel,
                         "Invalid power band pair selected for configuring power thresholds for DPD sample capture. Band01,12,23 are the valid target bands");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((dpdCapturePwrThresh->envelopePwrPulseWidthResolution != ADI_ADRV9025_EXT_DPD_CAPTURE_PW_RES_1X) &&
        (dpdCapturePwrThresh->envelopePwrPulseWidthResolution != ADI_ADRV9025_EXT_DPD_CAPTURE_PW_RES_256X) &&
        (dpdCapturePwrThresh->envelopePwrPulseWidthResolution != ADI_ADRV9025_EXT_DPD_CAPTURE_PW_RES_65536X) &&
        (dpdCapturePwrThresh->envelopePwrPulseWidthResolution != ADI_ADRV9025_EXT_DPD_CAPTURE_PW_RES_16777216X))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         dpdCapturePwrThresh->envelopePwrPulseWidthResolution,
                         "Invalid pulse width resolution selected for DPD envelope power measuring pulse width counter. Valid values are 2^N for n=0,8,16,24.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (dpdCapturePwrThresh->envelopePwrLowToHighPulseWidth > MAX_PWR_PULSE_WIDTH)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         dpdCapturePwrThresh->envelopePwrLowToHighPulseWidth,
                         "Invalid pulse width selected for DPD envelope low to high power. Range is between 0-4096");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (dpdCapturePwrThresh->envelopePwrHighToLowPulseWidth > MAX_PWR_PULSE_WIDTH)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         dpdCapturePwrThresh->envelopePwrHighToLowPulseWidth,
                         "Invalid pulse width selected for DPD envelope high to low power. Range is between 0-4096");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return recoveryAction;
}

int32_t adrv9025_ExtDpdCaptureDoneStatusGpioSetup(adi_adrv9025_Device_t*    device,
                                                  adi_adrv9025_GpioPinSel_e dpdCaptureDoneStatusPin)
{
    static const uint8_t DEFAULT_GPIO_SRC_CTRL        = 0;
    static const uint8_t MEMORY_ELEMENT_GPIO_SRC_CTRL = 3;

    int32_t              recoveryAction                 = ADI_COMMON_ACT_NO_ACTION;
    uint32_t             gpioPinMask                    = 0;
    uint8_t              gpioSrcCtrlRegVal              = 0;
    uint8_t              sharedResourceAcqReleaseStatus = 0;
    uint32_t             sharedResourceId               = 0;
    adrv9025_FeatureID_e sharedResourceFeatureId        = ADRV9025_FEATURE_INVALID;
    int32_t              sharedResourcesArr[1]          = {(int32_t)dpdCaptureDoneStatusPin};

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /* Retrieve the source control for the requested GPIO */
    recoveryAction = adrv9025_ExtDpdCaptureDoneGpioSrcCtrlGet(device,
                                                              dpdCaptureDoneStatusPin,
                                                              &gpioSrcCtrlRegVal);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Check that the GPIO source ctrl is not assigned to another feature */
    if ((gpioSrcCtrlRegVal != DEFAULT_GPIO_SRC_CTRL) &&
        (gpioSrcCtrlRegVal != MEMORY_ELEMENT_GPIO_SRC_CTRL))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         ADI_COMMON_ACT_ERR_RESET_INTERFACE,
                         dpdCaptureDoneStatusPin,
                         "GPIO source ctrl for the selected DPD capture done status indicator GPIO set to another feature.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Setup the GPIO pin for monitoring Tx-ORx DPD sample capture complete */
    /* Get the current pin assignment for external DPD capture complete indication and release the same */
    for (sharedResourceId = (uint32_t)ADRV9025_GPIO_00; sharedResourceId < (uint32_t)ADRV9025_GPIO_15; sharedResourceId++)
    {
        recoveryAction = adrv9025_SharedResourceFeatureGet(device,
                                                           (adrv9025_SharedResourceID_e)sharedResourceId,
                                                           &sharedResourceFeatureId);
        ADI_ERROR_RETURN(device->common.error.newAction);

        if (sharedResourceFeatureId == ADRV9025_FEATURE_EXT_DPD_CAPTURE_DONE_PIN)
        {
            recoveryAction = adrv9025_SharedResourceRelease(device,
                                                            (adrv9025_SharedResourceID_e)sharedResourceId,
                                                            ADRV9025_FEATURE_EXT_DPD_CAPTURE_DONE_PIN,
                                                            &sharedResourceAcqReleaseStatus);

            if (sharedResourceAcqReleaseStatus == ADI_FAILURE)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_API_FAIL,
                                 ADI_COMMON_ACT_ERR_RESET_INTERFACE,
                                 sharedResourceAcqReleaseStatus,
                                 "Attempting to release external DPD capture done indication GPIO shared resource already in use by another feature");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            ADI_ERROR_RETURN(device->common.error.newAction);

            /* Set the direction of Released GPIO to input */
            gpioPinMask = ((uint32_t)0x00000001 << sharedResourceId);

            if ((recoveryAction = adi_adrv9025_GpioInputDirSet(device,
                                                               gpioPinMask)) != ADI_COMMON_ACT_NO_ACTION)
            {
                /* Clear the error so that clean up functions execute */
                //recoveryAction = adi_common_ErrorClear(&device->common);
                ADI_ERROR_RETURN(device->common.error.newAction);

                /* Re-acquire shared resource before returning */
                recoveryAction = adrv9025_SharedResourceAcquire(device,
                                                                (adrv9025_SharedResourceID_e)sharedResourceId,
                                                                ADRV9025_FEATURE_EXT_DPD_CAPTURE_DONE_PIN,
                                                                &sharedResourceAcqReleaseStatus);

                if (sharedResourceAcqReleaseStatus == ADI_FAILURE)
                {
                    ADI_ERROR_REPORT(&device->common,
                                     ADI_COMMON_ERRSRC_API,
                                     ADI_COMMON_ERR_API_FAIL,
                                     ADI_COMMON_ACT_ERR_RESET_INTERFACE,
                                     sharedResourceAcqReleaseStatus,
                                     "Attempting to release external DPD capture done indication GPIO shared resource already in use by another feature");
                    ADI_ERROR_RETURN(device->common.error.newAction);
                }

                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_API_FAIL,
                                 ADI_COMMON_ACT_ERR_RESET_INTERFACE,
                                 gpioPinMask,
                                 "Unable to set the previously assigned external DPD capture done indication GPIO direction to input");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            break;
        }
    }

    /* Set the direction of the GPIO to output and GPIO source ctrl to Memory Element sourcing if a valid GPIO is requested */
    if (dpdCaptureDoneStatusPin != ADI_ADRV9025_GPIO_INVALID)
    {
        /* Acquire the shared resource GPIO for the requested capture done indication pin */
        recoveryAction = adrv9025_SharedResourcesAcquire(device,
                                                         ADRV9025_SHARED_RESOURCE_GPIO,
                                                         &sharedResourcesArr[0],
                                                         1,
                                                         ADRV9025_FEATURE_EXT_DPD_CAPTURE_DONE_PIN,
                                                         &sharedResourceAcqReleaseStatus);

        if (sharedResourceAcqReleaseStatus == ADI_FAILURE)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_API_FAIL,
                             ADI_COMMON_ACT_ERR_RESET_INTERFACE,
                             sharedResourceAcqReleaseStatus,
                             "Attempting to assign external DPD capture done indication GPIO shared resource already in use by another feature");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        ADI_ERROR_RETURN(device->common.error.newAction);

        gpioPinMask = ((uint32_t)1 << (uint8_t)dpdCaptureDoneStatusPin);
        if ((recoveryAction = adi_adrv9025_GpioOutputDirSet(device,
                                                            gpioPinMask)) != ADI_COMMON_ACT_NO_ACTION)
        {
            /* Clear the error so that clean up functions execute */
            //recoveryAction = adi_common_ErrorClear(&device->common);
            ADI_ERROR_RETURN(device->common.error.newAction);

            /* Unassign external DPD capture done GPIO */
            recoveryAction = adrv9025_SharedResourcesRelease(device,
                                                             ADRV9025_SHARED_RESOURCE_GPIO,
                                                             &sharedResourcesArr[0],
                                                             1,
                                                             ADRV9025_FEATURE_EXT_DPD_CAPTURE_DONE_PIN,
                                                             &sharedResourceAcqReleaseStatus);

            if (sharedResourceAcqReleaseStatus == ADI_FAILURE)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_API_FAIL,
                                 ADI_COMMON_ACT_ERR_RESET_INTERFACE,
                                 sharedResourceAcqReleaseStatus,
                                 "Error while attempting to release shared resource GPIO due to GPIO output direction set error");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
            ADI_ERROR_RETURN(device->common.error.newAction);

            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_API_FAIL,
                             ADI_COMMON_ACT_ERR_RESET_INTERFACE,
                             dpdCaptureDoneStatusPin,
                             "Unable to set GPIO output direction for external DPD capture done status indication");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        if ((recoveryAction = adrv9025_ExtDpdCaptureDoneGpioSrcCtrlSet(device,
                                                                       dpdCaptureDoneStatusPin,
                                                                       MEMORY_ELEMENT_GPIO_SRC_CTRL)) != ADI_COMMON_ACT_NO_ACTION)
        {
            /* Clear the error so that clean up functions execute */
            //recoveryAction = adi_common_ErrorClear(&device->common);
            ADI_ERROR_RETURN(device->common.error.newAction);

            recoveryAction = adrv9025_SharedResourcesRelease(device,
                                                             ADRV9025_SHARED_RESOURCE_GPIO,
                                                             &sharedResourcesArr[0],
                                                             1,
                                                             ADRV9025_FEATURE_EXT_DPD_CAPTURE_DONE_PIN,
                                                             &sharedResourceAcqReleaseStatus);

            if (sharedResourceAcqReleaseStatus == ADI_FAILURE)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_API_FAIL,
                                 ADI_COMMON_ACT_ERR_RESET_INTERFACE,
                                 sharedResourceAcqReleaseStatus,
                                 "Error while attempting to release shared resource GPIO due to GPIO source control setting error");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
            ADI_ERROR_RETURN(device->common.error.newAction);

            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_API_FAIL,
                             ADI_COMMON_ACT_ERR_RESET_INTERFACE,
                             dpdCaptureDoneStatusPin,
                             "Unable to set GPIO src ctrl for external DPD capture done status indication");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }
    else if (dpdCaptureDoneStatusPin == ADI_ADRV9025_GPIO_INVALID) /* Set the GPIO source ctrl to default */
    {
        recoveryAction = adrv9025_ExtDpdCaptureDoneGpioSrcCtrlSet(device,
                                                                  dpdCaptureDoneStatusPin,
                                                                  DEFAULT_GPIO_SRC_CTRL);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         ADI_COMMON_ACT_ERR_RESET_INTERFACE,
                         dpdCaptureDoneStatusPin,
                         "Unable to set GPIO src ctrl to default while attempting to disable external DPD capture done status indication GPIO config");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         ADI_COMMON_ACT_ERR_RESET_INTERFACE,
                         dpdCaptureDoneStatusPin,
                         "Invalid GPIO pin requested for external DPD capture done status indication");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return recoveryAction;
}

int32_t adrv9025_ExtDpdCaptureDoneStatusGpioGet(adi_adrv9025_Device_t*     device,
                                                adi_adrv9025_GpioPinSel_e* dpdCaptureDoneStatusPin)
{
    int32_t              recoveryAction          = ADI_COMMON_ACT_NO_ACTION;
    uint32_t             sharedResourceId        = 0;
    adrv9025_FeatureID_e sharedResourceFeatureId = ADRV9025_FEATURE_INVALID;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /* Initialize the capture done GPIO pin to invalid */
    *dpdCaptureDoneStatusPin = ADI_ADRV9025_GPIO_INVALID;

    /* Get the current pin assignment for external DPD capture complete indication and release the same */
    for (sharedResourceId = (uint32_t)ADRV9025_GPIO_00; sharedResourceId < (uint32_t)ADRV9025_GPIO_15; sharedResourceId++)
    {
        recoveryAction = adrv9025_SharedResourceFeatureGet(device,
                                                           (adrv9025_SharedResourceID_e)sharedResourceId,
                                                           &sharedResourceFeatureId);
        ADI_ERROR_RETURN(device->common.error.newAction);

        if (sharedResourceFeatureId == ADRV9025_FEATURE_EXT_DPD_CAPTURE_DONE_PIN)
        {
            *dpdCaptureDoneStatusPin = (adi_adrv9025_GpioPinSel_e)sharedResourceId;
            break;
        }
    }

    return recoveryAction;
}

int32_t adrv9025_ExtDpdCaptureDoneGpioSrcCtrlSet(adi_adrv9025_Device_t*    device,
                                                 adi_adrv9025_GpioPinSel_e dpdCaptureDoneStatusPin,
                                                 uint8_t                   srcCtrlVal)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API_PRIV);

    switch (dpdCaptureDoneStatusPin)
    {
    case(ADI_ADRV9025_GPIO_00): /* Fall Through */
    case(ADI_ADRV9025_GPIO_01): /* Fall Through */
    case(ADI_ADRV9025_GPIO_02): /* Fall Through */
    case(ADI_ADRV9025_GPIO_03):
    {
        recoveryAction = adrv9025_CoreGpioLowerByteLowerNibbleSourceControlBfSet(device,
                                                                                 ADRV9025_BF_CORE,
                                                                                 srcCtrlVal);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         dpdCaptureDoneStatusPin,
                         "Error while attempting to set source ctrl of external DPD capture done for GPIO[3:0]");
        break;
    }
    case(ADI_ADRV9025_GPIO_04): /* Fall Through */
    case(ADI_ADRV9025_GPIO_05): /* Fall Through */
    case(ADI_ADRV9025_GPIO_06): /* Fall Through */
    case(ADI_ADRV9025_GPIO_07):
    {
        recoveryAction = adrv9025_CoreGpioLowerByteUpperNibbleSourceControlBfSet(device,
                                                                                 ADRV9025_BF_CORE,
                                                                                 srcCtrlVal);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         dpdCaptureDoneStatusPin,
                         "Error while attempting to set source ctrl of external DPD capture done for GPIO[7:4]");
        break;
    }
    case(ADI_ADRV9025_GPIO_08): /* Fall Through */
    case(ADI_ADRV9025_GPIO_09): /* Fall Through */
    case(ADI_ADRV9025_GPIO_10): /* Fall Through */
    case(ADI_ADRV9025_GPIO_11):
    {
        recoveryAction = adrv9025_CoreGpioUpperByteLowerNibbleSourceControlBfSet(device,
                                                                                 ADRV9025_BF_CORE,
                                                                                 srcCtrlVal);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         dpdCaptureDoneStatusPin,
                         "Error while attempting to set source ctrl of external DPD capture done for GPIO[11:8]");
        break;
    }
    case(ADI_ADRV9025_GPIO_12): /* Fall Through */
    case(ADI_ADRV9025_GPIO_13): /* Fall Through */
    case(ADI_ADRV9025_GPIO_14): /* Fall Through */
    case(ADI_ADRV9025_GPIO_15):
    {
        recoveryAction = adrv9025_CoreGpioUpperByteUpperNibbleSourceControlBfSet(device,
                                                                                 ADRV9025_BF_CORE,
                                                                                 srcCtrlVal);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         dpdCaptureDoneStatusPin,
                         "Error while attempting to set source ctrl of external DPD capture done for GPIO[15:12]");
        break;
    }
    case(ADI_ADRV9025_GPIO_16): /* Fall Through */
    case(ADI_ADRV9025_GPIO_17): /* Fall Through */
    case(ADI_ADRV9025_GPIO_18):
    {
        recoveryAction = adrv9025_CoreGpioExtraBitsSourceControlBfSet(device,
                                                                      ADRV9025_BF_CORE,
                                                                      srcCtrlVal);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         dpdCaptureDoneStatusPin,
                         "Error while attempting to set source ctrl of external DPD capture done for GPIO[18:16]");
        break;
    }
    default:
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         dpdCaptureDoneStatusPin,
                         "Invalid GPIO encountered while attempting to set the source control for DPD capture done GPIO");
        break;
    }
    }

    return (device->common.error.newAction);
}

int32_t adrv9025_ExtDpdCaptureDoneGpioSrcCtrlGet(adi_adrv9025_Device_t*    device,
                                                 adi_adrv9025_GpioPinSel_e dpdCaptureDoneStatusPin,
                                                 uint8_t*                  srcCtrlVal)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    switch (dpdCaptureDoneStatusPin)
    {
    case(ADI_ADRV9025_GPIO_00): /* Fall Through */
    case(ADI_ADRV9025_GPIO_01): /* Fall Through */
    case(ADI_ADRV9025_GPIO_02): /* Fall Through */
    case(ADI_ADRV9025_GPIO_03):
    {
        recoveryAction = adrv9025_CoreGpioLowerByteLowerNibbleSourceControlBfGet(device,
                                                                                 ADRV9025_BF_CORE,
                                                                                 srcCtrlVal);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         dpdCaptureDoneStatusPin,
                         "Error while attempting to get source ctrl of external DPD capture done for GPIO[3:0]");
        break;
    }
    case(ADI_ADRV9025_GPIO_04): /* Fall Through */
    case(ADI_ADRV9025_GPIO_05): /* Fall Through */
    case(ADI_ADRV9025_GPIO_06): /* Fall Through */
    case(ADI_ADRV9025_GPIO_07):
    {
        recoveryAction = adrv9025_CoreGpioLowerByteUpperNibbleSourceControlBfGet(device,
                                                                                 ADRV9025_BF_CORE,
                                                                                 srcCtrlVal);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         dpdCaptureDoneStatusPin,
                         "Error while attempting to get source ctrl of external DPD capture done for GPIO[7:4]");
        break;
    }
    case(ADI_ADRV9025_GPIO_08): /* Fall Through */
    case(ADI_ADRV9025_GPIO_09): /* Fall Through */
    case(ADI_ADRV9025_GPIO_10): /* Fall Through */
    case(ADI_ADRV9025_GPIO_11):
    {
        recoveryAction = adrv9025_CoreGpioUpperByteLowerNibbleSourceControlBfGet(device,
                                                                                 ADRV9025_BF_CORE,
                                                                                 srcCtrlVal);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         dpdCaptureDoneStatusPin,
                         "Error while attempting to get source ctrl of external DPD capture done for GPIO[11:8]");
        break;
    }
    case(ADI_ADRV9025_GPIO_12): /* Fall Through */
    case(ADI_ADRV9025_GPIO_13): /* Fall Through */
    case(ADI_ADRV9025_GPIO_14): /* Fall Through */
    case(ADI_ADRV9025_GPIO_15):
    {
        recoveryAction = adrv9025_CoreGpioUpperByteUpperNibbleSourceControlBfGet(device,
                                                                                 ADRV9025_BF_CORE,
                                                                                 srcCtrlVal);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         dpdCaptureDoneStatusPin,
                         "Error while attempting to get source ctrl of external DPD capture done for GPIO[15:12]");
        break;
    }
    case(ADI_ADRV9025_GPIO_16): /* Fall Through */
    case(ADI_ADRV9025_GPIO_17): /* Fall Through */
    case(ADI_ADRV9025_GPIO_18):
    {
        recoveryAction = adrv9025_CoreGpioExtraBitsSourceControlBfGet(device,
                                                                      ADRV9025_BF_CORE,
                                                                      srcCtrlVal);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         dpdCaptureDoneStatusPin,
                         "Error while attempting to get source ctrl of external DPD capture done for GPIO[18:16]");
        break;
    }
    default:
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         dpdCaptureDoneStatusPin,
                         "Invalid GPIO encountered while attempting to get the GPIO source control for DPD capture done GPIO");
    }
    }

    return (device->common.error.newAction);
}

int32_t adrv9025_ExtDpdCaptureConfigParamArrayLoad(adi_adrv9025_Device_t*              device,
                                                   adi_adrv9025_ExtDpdCaptureConfig_t* dpdCaptureConfig,
                                                   uint8_t                             armData[],
                                                   uint8_t                             arraySize)
{
    static const uint8_t TX_CHANNEL1                     = 0;
    static const uint8_t TX_CHANNEL2                     = 1;
    static const uint8_t TX_CHANNEL3                     = 2;
    static const uint8_t TX_CHANNEL4                     = 3;
    static const uint8_t RESERVED_DATA                   = 0;
    static const uint8_t CAPTURE_CONFIG_ARRAY_SIZE_BYTES = 58;
    static const uint8_t CAPTURE_TYPE_MASK               = 0x01;
    static const uint8_t TDD_TXON_SYNC_CAPTURE_EN        = 0x00;
    static const uint8_t LEGACY_MODE_CAPTURE_EN          = 0x08;
    static const uint8_t EXTENDED_PEAK_SEARCH_SHIFT        = 6;

    int32_t recoveryAction         = ADI_COMMON_ACT_NO_ACTION;
    uint8_t extDpdCaptureTxChannel = 0;
    uint8_t extDpdTddSyncCapEn     = (dpdCaptureConfig->extDpdCaptureCtrl.dpdCaptureTypeSel == ADI_ADRV9025_EXT_DPD_CAPTURE_TDD_TXON_SYNC)
                                         ? TDD_TXON_SYNC_CAPTURE_EN : LEGACY_MODE_CAPTURE_EN;
    uint8_t extDpdExtendedPeakSearchEn = (dpdCaptureConfig->extDpdCaptureCtrl.dpdCaptureTypeSel == ADI_ADRV9025_EXT_DPD_CAPTURE_EXTENDED_PEAK_SEARCH) ? ((uint8_t)ADI_TRUE << EXTENDED_PEAK_SEARCH_SHIFT) : ((uint8_t)ADI_FALSE << EXTENDED_PEAK_SEARCH_SHIFT);

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /* Check that dpdCaptureConfig is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        dpdCaptureConfig);

    /* Check that armData is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        armData);

    if (arraySize < CAPTURE_CONFIG_ARRAY_SIZE_BYTES)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         armData,
            "External DPD capture config param array size lesser than expected size of 58 Bytes");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    switch (dpdCaptureConfig->extDpdCaptureCtrl.extDpdCapturetxChannelSel)
    {
    case(ADI_ADRV9025_TX1):
    {
        extDpdCaptureTxChannel = TX_CHANNEL1;
        break;
    }
    case(ADI_ADRV9025_TX2):
    {
        extDpdCaptureTxChannel = TX_CHANNEL2;
        break;
    }
    case(ADI_ADRV9025_TX3):
    {
        extDpdCaptureTxChannel = TX_CHANNEL3;
        break;
    }
    case(ADI_ADRV9025_TX4):
    {
        extDpdCaptureTxChannel = TX_CHANNEL4;
        break;
    }
    default:
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         dpdCaptureConfig->extDpdCaptureCtrl.extDpdCapturetxChannelSel,
                         "Invalid Tx channel encountered while attempting to write ext DPD capture config. Tx1-Tx4 are the valid Tx channels");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    }

    /* Load the ext dpd capture config data into a byte array */
    armData[0]  = dpdCaptureConfig->pathDelay.extDpdCaptureFifoDelay;
    armData[1]  = dpdCaptureConfig->pathDelay.extDpdCaptureInterpolationIndex;
    armData[2]  = (uint8_t)(dpdCaptureConfig->envelopePowerDecay & 0xFF);
    armData[3]  = (uint8_t)((dpdCaptureConfig->envelopePowerDecay >> 8) & 0xFF);
    armData[4]  = dpdCaptureConfig->extDpdCapPwrThreshBand01.envelopePwrLowToHighLimit;
    armData[5]  = dpdCaptureConfig->extDpdCapPwrThreshBand01.envelopePwrHighToLowLimit;
    armData[6]  = (uint8_t)dpdCaptureConfig->extDpdCapPwrThreshBand01.envelopePwrPulseWidthResolution;
    armData[7]  = RESERVED_DATA;
    armData[8]  = (uint8_t)(dpdCaptureConfig->extDpdCapPwrThreshBand01.envelopePwrLowToHighPulseWidth & 0xFF);
    armData[9]  = (uint8_t)((dpdCaptureConfig->extDpdCapPwrThreshBand01.envelopePwrLowToHighPulseWidth >> 8) & 0xFF);
    armData[10] = (uint8_t)(dpdCaptureConfig->extDpdCapPwrThreshBand01.envelopePwrHighToLowPulseWidth & 0xFF);
    armData[11] = (uint8_t)((dpdCaptureConfig->extDpdCapPwrThreshBand01.envelopePwrHighToLowPulseWidth >> 8) & 0xFF);
    armData[12] = dpdCaptureConfig->extDpdCapPwrThreshBand12.envelopePwrLowToHighLimit;
    armData[13] = dpdCaptureConfig->extDpdCapPwrThreshBand12.envelopePwrHighToLowLimit;
    armData[14] = (uint8_t)dpdCaptureConfig->extDpdCapPwrThreshBand12.envelopePwrPulseWidthResolution;
    armData[15] = RESERVED_DATA;
    armData[16] = (uint8_t)(dpdCaptureConfig->extDpdCapPwrThreshBand12.envelopePwrLowToHighPulseWidth & 0xFF);
    armData[17] = (uint8_t)((dpdCaptureConfig->extDpdCapPwrThreshBand12.envelopePwrLowToHighPulseWidth >> 8) & 0xFF);
    armData[18] = (uint8_t)(dpdCaptureConfig->extDpdCapPwrThreshBand12.envelopePwrHighToLowPulseWidth & 0xFF);
    armData[19] = (uint8_t)((dpdCaptureConfig->extDpdCapPwrThreshBand12.envelopePwrHighToLowPulseWidth >> 8) & 0xFF);
    armData[20] = dpdCaptureConfig->extDpdCapPwrThreshBand23.envelopePwrLowToHighLimit;
    armData[21] = dpdCaptureConfig->extDpdCapPwrThreshBand23.envelopePwrHighToLowLimit;
    armData[22] = (uint8_t)dpdCaptureConfig->extDpdCapPwrThreshBand23.envelopePwrPulseWidthResolution;
    armData[23] = RESERVED_DATA;
    armData[24] = (uint8_t)(dpdCaptureConfig->extDpdCapPwrThreshBand23.envelopePwrLowToHighPulseWidth & 0xFF);
    armData[25] = (uint8_t)((dpdCaptureConfig->extDpdCapPwrThreshBand23.envelopePwrLowToHighPulseWidth >> 8) & 0xFF);
    armData[26] = (uint8_t)(dpdCaptureConfig->extDpdCapPwrThreshBand23.envelopePwrHighToLowPulseWidth & 0xFF);
    armData[27] = (uint8_t)((dpdCaptureConfig->extDpdCapPwrThreshBand23.envelopePwrHighToLowPulseWidth >> 8) & 0xFF);
    armData[28] = (uint8_t)(dpdCaptureConfig->extDpdCaptureLevel.extDpdCaptureTimeOut & 0xFF);
    armData[29] = (uint8_t)((dpdCaptureConfig->extDpdCaptureLevel.extDpdCaptureTimeOut >> 8) & 0xFF);
    armData[30] = (uint8_t)(dpdCaptureConfig->extDpdCaptureLevel.dpdCaptureBandDir);
    armData[31] = (uint8_t)(dpdCaptureConfig->extDpdCaptureLevel.dpdCaptureBandSel);
    armData[32] = RESERVED_DATA;
    armData[33] = RESERVED_DATA;
    armData[34] = RESERVED_DATA;
    armData[35] = RESERVED_DATA;
    armData[36] = (uint8_t)((dpdCaptureConfig->extDpdCaptureCtrl.dpdPeakCaptureConfig.extDpdPeakExtendedWindowUs) & 0xFF);
    armData[37] = (uint8_t)((dpdCaptureConfig->extDpdCaptureCtrl.dpdPeakCaptureConfig.extDpdPeakExtendedWindowUs >> 8) & 0xFF);
    armData[38] = (uint8_t)((dpdCaptureConfig->extDpdCaptureCtrl.dpdPeakCaptureConfig.extDpdPeakExtendedWindowUs >> 16) & 0xFF);
    armData[39] = (uint8_t)((dpdCaptureConfig->extDpdCaptureCtrl.dpdPeakCaptureConfig.extDpdPeakExtendedWindowUs >> 24) & 0xFF);
    armData[40] = ((dpdCaptureConfig->extDpdCaptureCtrl.dpdCaptureTypeSel == ADI_ADRV9025_EXT_DPD_CAPTURE_PEAK_DETECTION) || (dpdCaptureConfig->extDpdCaptureCtrl.dpdCaptureTypeSel == ADI_ADRV9025_EXT_DPD_CAPTURE_EXTENDED_PEAK_SEARCH)) ? 1 : 0;
    armData[41] = dpdCaptureConfig->extDpdCaptureCtrl.dpdPeakCaptureConfig.extDpdPeakDecay;
    armData[42] = (uint8_t)(dpdCaptureConfig->extDpdCaptureCtrl.dpdPeakCaptureConfig.extDpdPeakCaptureInputSel);
    armData[43] = (uint8_t)((dpdCaptureConfig->extDpdCaptureCtrl.dpdPeakCaptureConfig.extDpdPeakDetectIIREnable & 0x01) |
        ((dpdCaptureConfig->extDpdCaptureCtrl.dpdPeakCaptureConfig.extDpdPeakDetectIIRBypass & 0x01) << 1));
    armData[44] = (uint8_t)(dpdCaptureConfig->extDpdCaptureCtrl.dpdPeakCaptureConfig.extDpdPeakWinCnt & 0xFF);
    armData[45] = (uint8_t)((dpdCaptureConfig->extDpdCaptureCtrl.dpdPeakCaptureConfig.extDpdPeakWinCnt >> 8) & 0xFF);
    armData[46] = (uint8_t)((dpdCaptureConfig->extDpdCaptureCtrl.dpdPeakCaptureConfig.extDpdPeakWinCnt >> 16) & 0xFF);
    armData[47] = (uint8_t)((dpdCaptureConfig->extDpdCaptureCtrl.dpdPeakCaptureConfig.extDpdPeakWinCnt >> 24) & 0xFF);
    armData[48] = extDpdCaptureTxChannel;
    armData[49] = ((uint8_t)(dpdCaptureConfig->extDpdCaptureCtrl.dpdCaptureTypeSel & CAPTURE_TYPE_MASK) |
        (uint8_t)((uint8_t)dpdCaptureConfig->extDpdCaptureCtrl.dpdCaptureTxObsSel << 1) |
        (uint8_t)((uint8_t)dpdCaptureConfig->extDpdCaptureCtrl.dpdCaptureTxAltObsSel << 2) |
        extDpdTddSyncCapEn | extDpdExtendedPeakSearchEn);
    armData[50] = (uint8_t)(dpdCaptureConfig->extDpdCaptureCtrl.dpdCaptureSize);
    armData[52] = (uint8_t)(dpdCaptureConfig->extDpdCaptureCtrl.extDpdCaptureDelay & 0xFF);
    armData[53] = (uint8_t)((dpdCaptureConfig->extDpdCaptureCtrl.extDpdCaptureDelay >> 8) & 0xFF);
    armData[54] = (uint8_t)((dpdCaptureConfig->extDpdCaptureCtrl.extDpdCaptureDelay >> 16) & 0xFF);
    armData[55] = (uint8_t)((dpdCaptureConfig->extDpdCaptureCtrl.extDpdCaptureDelay >> 24) & 0xFF);
    armData[56] = (uint8_t)(dpdCaptureConfig->extDpdCaptureCtrl.extDpdCaptureAlignDelay & 0xFF);
    armData[57] = (uint8_t)((dpdCaptureConfig->extDpdCaptureCtrl.extDpdCaptureAlignDelay >> 8) & 0xFF);

    return recoveryAction;
}

int32_t adrv9025_ExtDpdCaptureConfigParamArrayParse(adi_adrv9025_Device_t*              device,
                                                    adi_adrv9025_ExtDpdCaptureConfig_t* dpdCaptureConfig,
                                                    uint8_t                             armData[],
                                                    uint8_t                             arraySize)
{
    static const uint8_t  TX_CHANNEL1                     = 0;
    static const uint8_t  TX_CHANNEL2                     = 1;
    static const uint8_t  TX_CHANNEL3                     = 2;
    static const uint8_t  TX_CHANNEL4                     = 3;
    static const uint8_t  CAPTURE_TYPE_MASK               = 0x01;
    static const uint8_t  CAPTURE_TX_OBS_SEL_MASK         = 0x02;
    static const uint8_t  CAPTURE_TX_OBS_SEL_SHIFT        = 1;
    static const uint8_t  CAPTURE_TX_ALT_OBS_SEL_MASK     = 0x04;
    static const uint8_t  CAPTURE_TX_ALT_OBS_SEL_SHIFT    = 2;
    static const uint32_t CAPTURE_CONFIG_ARRAY_SIZE_BYTES = 58;
    static const uint8_t  TDD_TXON_SYNC_CAPTURE_EN_MASK   = 0x08;
    static const uint8_t  EXTENDED_PEAK_SEARCH_EN_MASK      = 0x40;

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /* Check that dpdCaptureConfig is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        dpdCaptureConfig);

    /* Check that armData is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        armData);

    if (arraySize < CAPTURE_CONFIG_ARRAY_SIZE_BYTES)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         armData,
            "External DPD capture config param array size lesser than expected size of 58 Bytes");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    dpdCaptureConfig->pathDelay.extDpdCaptureFifoDelay          = armData[0];
    dpdCaptureConfig->pathDelay.extDpdCaptureInterpolationIndex = armData[1];
    dpdCaptureConfig->envelopePowerDecay                        = (((uint16_t)armData[3] << 8) | (uint16_t)armData[2]);

    dpdCaptureConfig->extDpdCapPwrThreshBand01.envelopePwrLowToHighLimit       = armData[4];
    dpdCaptureConfig->extDpdCapPwrThreshBand01.envelopePwrHighToLowLimit       = armData[5];
    dpdCaptureConfig->extDpdCapPwrThreshBand01.envelopePwrPulseWidthResolution = (adi_adrv9025_ExtDpdCapturePulseWidthRes_e)armData[6];
    dpdCaptureConfig->extDpdCapPwrThreshBand01.envelopePwrLowToHighPulseWidth  = (((uint16_t)armData[9] << 8) | (uint16_t)armData[8]);
    dpdCaptureConfig->extDpdCapPwrThreshBand01.envelopePwrHighToLowPulseWidth  = (((uint16_t)armData[11] << 8) | (uint16_t)armData[10]);

    dpdCaptureConfig->extDpdCapPwrThreshBand12.envelopePwrLowToHighLimit       = armData[12];
    dpdCaptureConfig->extDpdCapPwrThreshBand12.envelopePwrHighToLowLimit       = armData[13];
    dpdCaptureConfig->extDpdCapPwrThreshBand12.envelopePwrPulseWidthResolution = (adi_adrv9025_ExtDpdCapturePulseWidthRes_e)armData[14];
    dpdCaptureConfig->extDpdCapPwrThreshBand12.envelopePwrLowToHighPulseWidth  = (((uint16_t)armData[17] << 8) | (uint16_t)armData[16]);
    dpdCaptureConfig->extDpdCapPwrThreshBand12.envelopePwrHighToLowPulseWidth  = (((uint16_t)armData[19] << 8) | (uint16_t)armData[18]);

    dpdCaptureConfig->extDpdCapPwrThreshBand23.envelopePwrLowToHighLimit       = armData[20];
    dpdCaptureConfig->extDpdCapPwrThreshBand23.envelopePwrHighToLowLimit       = armData[21];
    dpdCaptureConfig->extDpdCapPwrThreshBand23.envelopePwrPulseWidthResolution = (adi_adrv9025_ExtDpdCapturePulseWidthRes_e)armData[22];
    dpdCaptureConfig->extDpdCapPwrThreshBand23.envelopePwrLowToHighPulseWidth  = (((uint16_t)armData[25] << 8) | (uint16_t)armData[24]);
    dpdCaptureConfig->extDpdCapPwrThreshBand23.envelopePwrHighToLowPulseWidth  = (((uint16_t)armData[27] << 8) | (uint16_t)armData[26]);

    dpdCaptureConfig->extDpdCaptureLevel.extDpdCaptureTimeOut = (((uint16_t)armData[29] << 8) | (uint16_t)armData[28]);
    dpdCaptureConfig->extDpdCaptureLevel.dpdCaptureBandDir    = (adi_adrv9025_ExtDpdCaptureBandDir_e)armData[30];
    dpdCaptureConfig->extDpdCaptureLevel.dpdCaptureBandSel    = (adi_adrv9025_ExtDpdCaptureBand_e)armData[31];
    dpdCaptureConfig->extDpdCaptureCtrl.dpdPeakCaptureConfig.extDpdPeakExtendedWindowUs = (((uint32_t)armData[39] << 24) | ((uint32_t)armData[38] << 16) | ((uint32_t)armData[37] << 8) | (uint32_t)armData[36]);


    dpdCaptureConfig->extDpdCaptureCtrl.dpdPeakCaptureConfig.extDpdPeakDecay           = armData[41];
    dpdCaptureConfig->extDpdCaptureCtrl.dpdPeakCaptureConfig.extDpdPeakCaptureInputSel = (adi_adrv9025_ExtDpdCapturePeakDetectInputSel_e)armData[42];
    dpdCaptureConfig->extDpdCaptureCtrl.dpdPeakCaptureConfig.extDpdPeakDetectIIREnable = (armData[43] & 0x01);
    dpdCaptureConfig->extDpdCaptureCtrl.dpdPeakCaptureConfig.extDpdPeakDetectIIRBypass = ((armData[43] & 0x02) >> 1);
    dpdCaptureConfig->extDpdCaptureCtrl.dpdPeakCaptureConfig.extDpdPeakWinCnt          = (((uint32_t)armData[47] << 24) | ((uint32_t)armData[46] << 16) | ((
            uint32_t)
        armData[45] << 8) | (uint32_t)armData[44]);
    dpdCaptureConfig->extDpdCaptureCtrl.extDpdCapturetxChannelSel = (armData[48] == TX_CHANNEL1) ? ADI_ADRV9025_TX1 :
                                                                        (armData[48] == TX_CHANNEL2) ? ADI_ADRV9025_TX2 :
                                                                            (armData[48] == TX_CHANNEL3) ? ADI_ADRV9025_TX3 :
                                                                                (armData[48] == TX_CHANNEL4) ? ADI_ADRV9025_TX4 :
                                                                                    (adi_adrv9025_TxChannels_e)armData[48]; /* Invalid Case */
    dpdCaptureConfig->extDpdCaptureCtrl.dpdCaptureTypeSel = ((armData[49] & EXTENDED_PEAK_SEARCH_EN_MASK) != 0) ? ADI_ADRV9025_EXT_DPD_CAPTURE_EXTENDED_PEAK_SEARCH :
                                                            (armData[40] == 1) ? ADI_ADRV9025_EXT_DPD_CAPTURE_PEAK_DETECTION :
                                                            ((armData[49] & TDD_TXON_SYNC_CAPTURE_EN_MASK) == 0) ? ADI_ADRV9025_EXT_DPD_CAPTURE_TDD_TXON_SYNC :
                                                                    (adi_adrv9025_ExtDpdCaptureType_e)(armData[49] & CAPTURE_TYPE_MASK);
    dpdCaptureConfig->extDpdCaptureCtrl.dpdCaptureTxObsSel = (adi_adrv9025_ExtDpdCaptureTxObsSel_e)((armData[49] & CAPTURE_TX_OBS_SEL_MASK) >> CAPTURE_TX_OBS_SEL_SHIFT);
    dpdCaptureConfig->extDpdCaptureCtrl.dpdCaptureTxAltObsSel = (adi_adrv9025_ExtDpdCaptureTxAltObsSel_e)((armData[49] & CAPTURE_TX_ALT_OBS_SEL_MASK) >> CAPTURE_TX_ALT_OBS_SEL_SHIFT);
    dpdCaptureConfig->extDpdCaptureCtrl.dpdCaptureSize          = (adi_adrv9025_ExtDpdCaptureSize_e)armData[50];
    /* Omit armData[51] for arm structure alignment */
    dpdCaptureConfig->extDpdCaptureCtrl.extDpdCaptureDelay = (((uint32_t)armData[55] << 24) | ((uint32_t)armData[54] << 16) | ((uint32_t)armData[53] << 8) | (uint32_t)armData[52]);
    dpdCaptureConfig->extDpdCaptureCtrl.extDpdCaptureAlignDelay = (((uint16_t)armData[57] << 8) | (uint16_t)armData[56]);

    return recoveryAction;
}

int32_t adrv9025_ExtDpdCaptureDataParse(adi_adrv9025_Device_t* device,
                                        uint8_t                extDpdCaptureIQInterleavedByteArray[],
                                        int16_t                extDpdCaptureIData[],
                                        int16_t                extDpdCaptureQData[],
                                        uint16_t               maxSampleSize)
{
    static const uint32_t NUM_BYTES_PER_SAMPLE = 4;

    int32_t  recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint32_t sampleIndex    = 0;
    uint32_t byteIndexILsb  = 0;
    uint32_t byteIndexIMsb  = 0;
    uint32_t byteIndexQLsb  = 0;
    uint32_t byteIndexQMsb  = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /* Check that dpdCaptureConfig is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        extDpdCaptureIQInterleavedByteArray);

    /* Check that armData is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        extDpdCaptureIData);

    /* Check that armData is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        extDpdCaptureQData);

    for (sampleIndex = 0; sampleIndex < maxSampleSize; sampleIndex++)
    {
        byteIndexQLsb                   = (sampleIndex * NUM_BYTES_PER_SAMPLE);
        byteIndexQMsb                   = ((sampleIndex * NUM_BYTES_PER_SAMPLE) + 1);
        byteIndexILsb                   = ((sampleIndex * NUM_BYTES_PER_SAMPLE) + 2);
        byteIndexIMsb                   = ((sampleIndex * NUM_BYTES_PER_SAMPLE) + 3);
        extDpdCaptureIData[sampleIndex] = (int16_t)((uint16_t)(extDpdCaptureIQInterleavedByteArray[byteIndexIMsb] << 8) |
            (uint16_t)(extDpdCaptureIQInterleavedByteArray[byteIndexILsb]));
        extDpdCaptureQData[sampleIndex] = (int16_t)((uint16_t)(extDpdCaptureIQInterleavedByteArray[byteIndexQMsb] << 8) |
            (uint16_t)(extDpdCaptureIQInterleavedByteArray[byteIndexQLsb]));
    }

    return recoveryAction;
}

int32_t adrv9025_ExtDpdCaptureDoneStatusQuery(adi_adrv9025_Device_t* device,
                                              uint8_t*               captureCompleteFlag)
{
    static const uint8_t EXT_DPD_CAPTURE_DONE_MASK = 0x01;

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t extData[1]     = {ADRV9025_CPU_OBJECTID_EXT_DPD_CAPTURE_DONE};
    uint8_t armData[1]     = {0};
    uint8_t cmdStatusByte  = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /* Check that captureCompleteFlag is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        captureCompleteFlag);

    /* Execute the capture done status get command */
    recoveryAction = adi_adrv9025_CpuCmdWrite(device,
                                              ADI_ADRV9025_CPU_TYPE_C,
                                              (uint8_t)ADRV9025_CPU_GET_OPCODE,
                                              &extData[0],
                                              sizeof(extData));
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Wait for command to finish executing */
    recoveryAction = adi_adrv9025_CpuCmdStatusWait(device,
                                                   ADI_ADRV9025_CPU_TYPE_C,
                                                   (uint8_t)ADRV9025_CPU_GET_OPCODE,
                                                   &cmdStatusByte,
                                                   (uint32_t)ADI_ADRV9025_GETEXTDPDCAPTUREDONE_TIMEOUT_US,
                                                   (uint32_t)ADI_ADRV9025_GETEXTDPDCAPTUREDONE_INTERVAL_US);

    /* If cmdStatusByte[3:1] is non-zero then flag an ARM error */
    if ((cmdStatusByte >> 1) > 0)
    {
        recoveryAction = adrv9025_CpuCmdErrorHandler(device,
                                                     ADI_ADRV9025_CPU_TYPE_C,
                                                     ADI_ADRV9025_SRC_CPUCMD,
                                                     ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                                                              extData[0],
                                                                              cmdStatusByte),
                                                     recoveryAction);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Read DPD capture complete from ARM mailbox */
    recoveryAction = adi_adrv9025_CpuMemRead(device,
                                             (uint32_t)ADRV9025_CPU_C_ADDR_MAILBOX_GET,
                                             &armData[0],
                                             sizeof(armData),
                                             0);
    ADI_ERROR_RETURN(device->common.error.newAction);

    *captureCompleteFlag = (armData[0] & EXT_DPD_CAPTURE_DONE_MASK);

    return recoveryAction;
}

int32_t adrv9025_ExtDpdCaptureDoneStatusClear(adi_adrv9025_Device_t* device)
{
    static const uint8_t EXT_DPD_CAPTURE_DONE_CLEAR = 0x00;

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t extData[2]     = {ADRV9025_CPU_OBJECTID_EXT_DPD_CAPTURE_DONE, EXT_DPD_CAPTURE_DONE_CLEAR};
    uint8_t cmdStatusByte  = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /* Execute the capture done status clear command */
    recoveryAction = adi_adrv9025_CpuCmdWrite(device,
                                              ADI_ADRV9025_CPU_TYPE_C,
                                              (uint8_t)ADRV9025_CPU_SET_OPCODE,
                                              &extData[0],
                                              sizeof(extData));
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Wait for command to finish executing */
    recoveryAction = adi_adrv9025_CpuCmdStatusWait(device,
                                                   ADI_ADRV9025_CPU_TYPE_C,
                                                   (uint8_t)ADRV9025_CPU_GET_OPCODE,
                                                   &cmdStatusByte,
                                                   (uint32_t)ADI_ADRV9025_SETEXTDPDCAPTUREDONE_TIMEOUT_US,
                                                   (uint32_t)ADI_ADRV9025_SETEXTDPDCAPTUREDONE_INTERVAL_US);

    /* If cmdStatusByte[3:1] is non-zero then flag an ARM error */
    if ((cmdStatusByte >> 1) > 0)
    {
        recoveryAction = adrv9025_CpuCmdErrorHandler(device,
                                                     ADI_ADRV9025_CPU_TYPE_C,
                                                     ADI_ADRV9025_SRC_CPUCMD,
                                                     ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
                                                                              extData[0],
                                                                              cmdStatusByte),
                                                     recoveryAction);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    ADI_ERROR_RETURN(device->common.error.newAction);

    return recoveryAction;
}

int32_t adrv9025_ExtDpdActuatorModelConfigSetRangeCheck(adi_adrv9025_Device_t*      device,
                                                        adi_adrv9025_TxChannels_e   txChannel,
                                                        adi_adrv9025_ExtDpdModel_t* extDpdModelConfig)
{
    static const uint8_t MAX_ACTUATOR_OUTPUT_SHIFT = 15;

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /* Check that dpdCaptureConfig is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        extDpdModelConfig);

    /* Check that the target Tx channel is valid */
    if ((txChannel != ADI_ADRV9025_TX1) &&
        (txChannel != ADI_ADRV9025_TX2) &&
        (txChannel != ADI_ADRV9025_TX3) &&
        (txChannel != ADI_ADRV9025_TX4))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         extDpdModelConfig->extDpdModelTxChannelSel,
                         "Invalid Tx channel encountered while attempting to set DPD model config. Valid Tx channels are Tx1-Tx4");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Check that the DPD model is valid */
    if ((extDpdModelConfig->extDpdModelSel != ADI_ADRV9025_EXT_DPD_MODEL0) &&
        (extDpdModelConfig->extDpdModelSel != ADI_ADRV9025_EXT_DPD_MODEL1) &&
        (extDpdModelConfig->extDpdModelSel != ADI_ADRV9025_EXT_DPD_MODEL2) &&
        (extDpdModelConfig->extDpdModelSel != ADI_ADRV9025_EXT_DPD_MODEL3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         extDpdModelConfig->extDpdModelSel,
                         "Invalid target DPD model selected for DPD model configuration. Valid DPD models are Model0-Model3");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Check that the target DPD LUT bank is valid */
    if ((extDpdModelConfig->extDpdLutBankSel != ADI_ADRV9025_EXT_DPD_LUT_BANK0) &&
        (extDpdModelConfig->extDpdLutBankSel != ADI_ADRV9025_EXT_DPD_LUT_BANK1) &&
        (extDpdModelConfig->extDpdLutBankSel != ADI_ADRV9025_EXT_DPD_LUT_BANK2) &&
        (extDpdModelConfig->extDpdLutBankSel != ADI_ADRV9025_EXT_DPD_LUT_BANK3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         extDpdModelConfig->extDpdLutBankSel,
                         "Invalid target DPD LUT bank selected for DPD model configuration. Valid LUT banks are Bank0-Bank3");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Check that the actuator compander size is valid */
    if ((extDpdModelConfig->extDpdActuatorCompanderSize != ADI_ADRV9025_EXT_DPD_ACT_COMPANDER_8_BITS) &&
        (extDpdModelConfig->extDpdActuatorCompanderSize != ADI_ADRV9025_EXT_DPD_ACT_COMPANDER_9_BITS))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         extDpdModelConfig->extDpdActuatorCompanderSize,
                         "Invalid target DPD actuator compander size selected for DPD model configuration. Valid actuator compander sizes are 8 bits and 9 bits");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Check that if compander size is 9 bits the LUT bank selected is either 0 or 2 */
    if (extDpdModelConfig->extDpdActuatorCompanderSize == ADI_ADRV9025_EXT_DPD_ACT_COMPANDER_9_BITS)
    {
        if ((extDpdModelConfig->extDpdLutBankSel != ADI_ADRV9025_EXT_DPD_LUT_BANK0) &&
            (extDpdModelConfig->extDpdLutBankSel != ADI_ADRV9025_EXT_DPD_LUT_BANK2))
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             extDpdModelConfig->extDpdLutBankSel,
                             "Invalid LUT bank encountered for compander size 9 bits while attempting to configure the DPD model. Bank0 and Bank2 are the valid bank");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    /* Check that actuator output shift is less than 16 */
    if (extDpdModelConfig->extDpdActuatorOutputShift > MAX_ACTUATOR_OUTPUT_SHIFT)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         extDpdModelConfig->extDpdActuatorOutputShift,
                         "Actuator output shift should be less than 15");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return recoveryAction;
}

int32_t adrv9025_ExtDpdModelConfigParamArrayLoad(adi_adrv9025_Device_t*      device,
                                                 adi_adrv9025_TxChannels_e   txChannel,
                                                 adi_adrv9025_ExtDpdModel_t* extDpdModelConfig,
                                                 uint8_t                     armData[],
                                                 uint8_t                     arraySize)
{
    static const uint8_t TX_CHANNEL1                           = 0;
    static const uint8_t TX_CHANNEL2                           = 1;
    static const uint8_t TX_CHANNEL3                           = 2;
    static const uint8_t TX_CHANNEL4                           = 3;
    static const uint8_t EXT_DPD_MODEL_CONFIG_PARAM_ARRAY_SIZE = 48;

    int32_t recoveryAction       = ADI_COMMON_ACT_NO_ACTION;
    uint8_t extDpdModelTxChannel = 0;
    uint8_t i                    = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /* Check that extDpdModelConfig is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        extDpdModelConfig);

    /* Check that armData is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        armData);

    if (arraySize < EXT_DPD_MODEL_CONFIG_PARAM_ARRAY_SIZE)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         armData,
                         "Size of Model config array param less than expected size of 48 bytes");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    switch (txChannel)
    {
    case(ADI_ADRV9025_TX1):
    {
        extDpdModelTxChannel = TX_CHANNEL1;
        break;
    }
    case(ADI_ADRV9025_TX2):
    {
        extDpdModelTxChannel = TX_CHANNEL2;
        break;
    }
    case(ADI_ADRV9025_TX3):
    {
        extDpdModelTxChannel = TX_CHANNEL3;
        break;
    }
    case(ADI_ADRV9025_TX4):
    {
        extDpdModelTxChannel = TX_CHANNEL4;
        break;
    }
    default:
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         extDpdModelConfig->extDpdModelTxChannelSel,
                         "Invalid Tx channel encountered while attempting to program DPD model. Tx1-Tx4 are the valid Tx channels");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    }

    armData[0] = extDpdModelTxChannel;
    armData[1] = (uint8_t)extDpdModelConfig->extDpdModelSel;
    armData[2] = (uint8_t)extDpdModelConfig->extDpdLutBankSel;
    armData[3] = extDpdModelConfig->extDpdMagSqGain;
    armData[4] = (uint8_t)extDpdModelConfig->extDpdActuatorCompanderSize;
    armData[5] = (uint8_t)extDpdModelConfig->extDpdActuatorOutputShift;
    armData[6] = (uint8_t)(extDpdModelConfig->extDpdLutISum & 0xFF);
    armData[7] = (uint8_t)((extDpdModelConfig->extDpdLutISum >> 8) & 0xFF);

    for (i = 0; i < ADRV9025_MAX_EXT_DPD_MODEL_MEMORY_TERM_SIZE; i++)
    {
        armData[8 + i] = (uint8_t)extDpdModelConfig->extDpdISel[i];
    }

    for (i = 0; i < ADRV9025_MAX_EXT_DPD_MODEL_CROSS_TERM_SIZE; i++)
    {
        armData[40 + i] = (uint8_t)extDpdModelConfig->extDpdJSel[i];
    }

    return recoveryAction;
}

int32_t adrv9025_ExtDpdModelConfigParamArrayParse(adi_adrv9025_Device_t*      device,
                                                  adi_adrv9025_ExtDpdModel_t* extDpdModelConfig,
                                                  uint8_t                     armData[],
                                                  uint8_t                     arraySize)
{
    static const uint8_t EXT_DPD_MODEL_CONFIG_PARAM_ARRAY_SIZE = 48;

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t i              = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /* Check that armData is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        armData);

    /* Check that extDpdModelConfig is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        extDpdModelConfig);

    /* Skipping ARM data 0 as it contains the tx channel param which is redundant */
    if (arraySize < EXT_DPD_MODEL_CONFIG_PARAM_ARRAY_SIZE)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         armData,
                         "Size of Model config array param less than expected size of 48 bytes");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    extDpdModelConfig->extDpdModelSel              = (adi_adrv9025_ExtDpdModelSel_e)armData[1];
    extDpdModelConfig->extDpdLutBankSel            = (adi_adrv9025_ExtDpdLutBank_e)armData[2];
    extDpdModelConfig->extDpdMagSqGain             = armData[3];
    extDpdModelConfig->extDpdActuatorCompanderSize = (adi_adrv9025_ExtDpdActuatorCompanderSize_e)armData[4];
    extDpdModelConfig->extDpdActuatorOutputShift   = armData[5];
    extDpdModelConfig->extDpdLutISum               = (((uint16_t)armData[7] << 8) | (uint16_t)armData[6]);

    for (i = 0; i < ADRV9025_MAX_EXT_DPD_MODEL_MEMORY_TERM_SIZE; i++)
    {
        extDpdModelConfig->extDpdISel[i] = armData[8 + i];
    }

    for (i = 0; i < ADRV9025_MAX_EXT_DPD_MODEL_CROSS_TERM_SIZE; i++)
    {
        extDpdModelConfig->extDpdJSel[i] = armData[40 + i];
    }

    return recoveryAction;
}

int32_t adrv9025_ExtDpdActuatorEnableSetRangeCheck(adi_adrv9025_Device_t* device,
                                                   uint32_t               txChannelMask,
                                                   uint8_t                enable)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    if (enable > 0)
    {
        /* Check that Tx profile is valid */
        if ((device->devStateInfo.profilesValid & ADI_ADRV9025_TX_PROFILE_VALID) != ADI_ADRV9025_TX_PROFILE_VALID)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             txChannelMask,
                             "Tx DPD actuator enable requested Tx profile is invalid or channel not initialized in device structure");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        /* Check that requested Tx channels are initialized */
        if ((txChannelMask & (device->devStateInfo.initializedChannels >> ADI_ADRV9025_TX_INITIALIZED_CH_OFFSET)) != txChannelMask)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             txChannelMask,
                             "Requested Tx channels to enable are not initialized");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    return recoveryAction;
}

int32_t adrv9025_ExtDpdActuatorLutRangeCheck(adi_adrv9025_Device_t*       device,
                                             adi_adrv9025_TxChannels_e    txChannel,
                                             adi_adrv9025_ExtDpdLut_e     extDpdLutSel,
                                             adi_adrv9025_ExtDpdLutBank_e extDpdLutBankSel,
                                             uint8_t                      lutData[])
{
    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /* Check that lutData is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        lutData);

    /* Check that the target Tx channel is valid */
    if ((txChannel != ADI_ADRV9025_TX1) &&
        (txChannel != ADI_ADRV9025_TX2) &&
        (txChannel != ADI_ADRV9025_TX3) &&
        (txChannel != ADI_ADRV9025_TX4))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txChannel,
                         "Invalid Tx channel encountered while attempting to program/read back DPD LUTs. Valid Tx channels are Tx1-Tx4");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((extDpdLutSel != ADI_ADRV9025_EXT_DPD_LUT0) &&
        (extDpdLutSel != ADI_ADRV9025_EXT_DPD_LUT1) &&
        (extDpdLutSel != ADI_ADRV9025_EXT_DPD_LUT2) &&
        (extDpdLutSel != ADI_ADRV9025_EXT_DPD_LUT3) &&
        (extDpdLutSel != ADI_ADRV9025_EXT_DPD_LUT4) &&
        (extDpdLutSel != ADI_ADRV9025_EXT_DPD_LUT5) &&
        (extDpdLutSel != ADI_ADRV9025_EXT_DPD_LUT6) &&
        (extDpdLutSel != ADI_ADRV9025_EXT_DPD_LUT7) &&
        (extDpdLutSel != ADI_ADRV9025_EXT_DPD_LUT8) &&
        (extDpdLutSel != ADI_ADRV9025_EXT_DPD_LUT9) &&
        (extDpdLutSel != ADI_ADRV9025_EXT_DPD_LUT10) &&
        (extDpdLutSel != ADI_ADRV9025_EXT_DPD_LUT11) &&
        (extDpdLutSel != ADI_ADRV9025_EXT_DPD_LUT12) &&
        (extDpdLutSel != ADI_ADRV9025_EXT_DPD_LUT13) &&
        (extDpdLutSel != ADI_ADRV9025_EXT_DPD_LUT14) &&
        (extDpdLutSel != ADI_ADRV9025_EXT_DPD_LUT15) &&
        (extDpdLutSel != ADI_ADRV9025_EXT_DPD_LUT16) &&
        (extDpdLutSel != ADI_ADRV9025_EXT_DPD_LUT17) &&
        (extDpdLutSel != ADI_ADRV9025_EXT_DPD_LUT18) &&
        (extDpdLutSel != ADI_ADRV9025_EXT_DPD_LUT19) &&
        (extDpdLutSel != ADI_ADRV9025_EXT_DPD_LUT20) &&
        (extDpdLutSel != ADI_ADRV9025_EXT_DPD_LUT21) &&
        (extDpdLutSel != ADI_ADRV9025_EXT_DPD_LUT22) &&
        (extDpdLutSel != ADI_ADRV9025_EXT_DPD_LUT23) &&
        (extDpdLutSel != ADI_ADRV9025_EXT_DPD_LUT24) &&
        (extDpdLutSel != ADI_ADRV9025_EXT_DPD_LUT25) &&
        (extDpdLutSel != ADI_ADRV9025_EXT_DPD_LUT26) &&
        (extDpdLutSel != ADI_ADRV9025_EXT_DPD_LUT27) &&
        (extDpdLutSel != ADI_ADRV9025_EXT_DPD_LUT28) &&
        (extDpdLutSel != ADI_ADRV9025_EXT_DPD_LUT29) &&
        (extDpdLutSel != ADI_ADRV9025_EXT_DPD_LUT30) &&
        (extDpdLutSel != ADI_ADRV9025_EXT_DPD_LUT31))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         extDpdLutSel,
                         "Invalid target DPD LUT selected for DPD actuator LUT programming/read back. Valid LUTs are LUT0-LUT31");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Check that the target DPD actuator LUT bank is valid */
    if ((extDpdLutBankSel != ADI_ADRV9025_EXT_DPD_LUT_BANK0) &&
        (extDpdLutBankSel != ADI_ADRV9025_EXT_DPD_LUT_BANK1) &&
        (extDpdLutBankSel != ADI_ADRV9025_EXT_DPD_LUT_BANK2) &&
        (extDpdLutBankSel != ADI_ADRV9025_EXT_DPD_LUT_BANK3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         extDpdLutBankSel,
                         "Invalid target DPD LUT bank selected for DPD actuator LUT programming/read back. Valid LUT banks are Bank0-Bank3");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return (device->common.error.newAction);
}

int32_t adrv9025_ExtDpdActuatorSwapModeLutWriteRangeCheck(adi_adrv9025_Device_t*       device,
                                                          adi_adrv9025_TxChannels_e    txChannel,
                                                          adi_adrv9025_ExtDpdLutBank_e extDpdLutBankSel,
                                                          adi_adrv9025_SwapModeLut_t   swapModeLutData[])
{
    static const uint8_t LUT_SWAP_MODE_EN = 1;

    int32_t                    recoveryAction       = ADI_COMMON_ACT_NO_ACTION;
    uint8_t                    lutSwapModeEnableVal = 0;
    uint8_t                    lutIndex             = 0;
    adi_adrv9025_ExtDpdModel_t extDpdModelConfig    = {
        ADI_ADRV9025_EXT_DPD_MODEL0,
        ADI_ADRV9025_EXT_DPD_LUT_BANK0,
        ADI_ADRV9025_EXT_DPD_ACT_COMPANDER_8_BITS,
        0,
        0,
        0,
        {0},
        {0}
    };

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /* Check that lutData is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        swapModeLutData);

    /* Check that the target Tx channel is valid */
    if ((txChannel != ADI_ADRV9025_TX1) &&
        (txChannel != ADI_ADRV9025_TX2) &&
        (txChannel != ADI_ADRV9025_TX3) &&
        (txChannel != ADI_ADRV9025_TX4))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txChannel,
                         "Invalid target Tx channel selected for swap mode LUT programming");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Throw an error if Swap LUT mode operation is not enabled */
    recoveryAction = adrv9025_ExtDpdLutSwapModeEnableGet(device,
                                                         &lutSwapModeEnableVal);
    ADI_ERROR_RETURN(device->common.error.newAction);

    if (lutSwapModeEnableVal != LUT_SWAP_MODE_EN)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         lutSwapModeEnableVal,
                         "Swap mode LUT not enabled for programming the LUT in swap mode.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    recoveryAction = adi_adrv9025_ExtDpdActuatorModelConfigGet(device,
                                                               txChannel,
                                                               &extDpdModelConfig);
    ADI_ERROR_RETURN(device->common.error.newAction);

    if (extDpdModelConfig.extDpdActuatorCompanderSize == ADI_ADRV9025_EXT_DPD_ACT_COMPANDER_9_BITS)
    {
        if (extDpdLutBankSel != ADI_ADRV9025_EXT_DPD_LUT_BANK0)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             extDpdLutBankSel,
                             "Swap mode LUT programming for a 9-bit compander mode requires that the target LUT bank is Bank0");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }
    else if (extDpdModelConfig.extDpdActuatorCompanderSize == ADI_ADRV9025_EXT_DPD_ACT_COMPANDER_8_BITS)
    {
        if ((extDpdLutBankSel != ADI_ADRV9025_EXT_DPD_LUT_BANK0) &&
            (extDpdLutBankSel != ADI_ADRV9025_EXT_DPD_LUT_BANK1) &&
            (extDpdLutBankSel != ADI_ADRV9025_EXT_DPD_LUT_BANK2))
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             extDpdLutBankSel,
                             "Swap mode LUT programming for an 8-bit compander mode requires that the target LUT bank is Bank0/Bank1/Bank2");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }
    else
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         extDpdLutBankSel,
                         "Please verify if the actuator model has been configured to a valid mode");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    for (lutIndex = 0; lutIndex < (uint8_t)ADI_ADRV9025_EXT_DPD_NUM_LUTS; lutIndex++)
    {
        /* If an LUT is requested to be programmed */
        if ((swapModeLutData[lutIndex].extDpdSwapModeLutDataPtr != NULL) &&
            (swapModeLutData[lutIndex].extDpdSwapModeLutDataSizeBytes > 0))
        {
            /* Check that the no. of LUT entries do not exceed 256 entries(1024 bytes) for an 8 bit compander */
            if ((extDpdModelConfig.extDpdActuatorCompanderSize == ADI_ADRV9025_EXT_DPD_ACT_COMPANDER_8_BITS) &&
                (swapModeLutData[lutIndex].extDpdSwapModeLutDataSizeBytes > 1024))
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 extDpdLutBankSel,
                                 "DPD model is configured for an 8-bit compander operation but array size is greater than 256 entries(1024 Bytes)");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            /* Check that the no. of LUT entries do not exceed 512 entries(2048 bytes) for a 9 bit compander */
            if ((extDpdModelConfig.extDpdActuatorCompanderSize == ADI_ADRV9025_EXT_DPD_ACT_COMPANDER_9_BITS) &&
                (swapModeLutData[lutIndex].extDpdSwapModeLutDataSizeBytes > 2048))
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 extDpdLutBankSel,
                                 "DPD model is configured for a 9-bit compander operation but array size is greater than 512 entries(2048 Bytes)");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
        }
    }

    return recoveryAction;
}

int32_t adrv9025_ExtDpdActuatorLutAddressResolve(adi_adrv9025_Device_t*       device,
                                                 adi_adrv9025_TxChannels_e    txChannel,
                                                 adi_adrv9025_ExtDpdLut_e     extDpdLutSel,
                                                 adi_adrv9025_ExtDpdLutBank_e extDpdLutBankSel,
                                                 uint32_t*                    lutBaseAddr)
{
    static const uint32_t LUT_OFFSET_BYTES      = 4096;
    static const uint32_t LUT_BANK_OFFSET_BYTES = 1024;

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /* Check that lutBaseAddr is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        lutBaseAddr);

    switch (txChannel)
    {
    case(ADI_ADRV9025_TX1):
    {
        *lutBaseAddr = (uint32_t)ADRV9025_EXT_DPD_ACTUATOR_TX1_LUT_BASEADDR;
        break;
    }
    case(ADI_ADRV9025_TX2):
    {
        *lutBaseAddr = (uint32_t)ADRV9025_EXT_DPD_ACTUATOR_TX2_LUT_BASEADDR;
        break;
    }
    case(ADI_ADRV9025_TX3):
    {
        *lutBaseAddr = (uint32_t)ADRV9025_EXT_DPD_ACTUATOR_TX3_LUT_BASEADDR;
        break;
    }
    case(ADI_ADRV9025_TX4):
    {
        *lutBaseAddr = (uint32_t)ADRV9025_EXT_DPD_ACTUATOR_TX4_LUT_BASEADDR;
        break;
    }
    default:
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txChannel,
                         "Invalid target Tx channel selected for DPD actuator LUT programming/read back. Valid Tx channels are Tx1-Tx4");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    }

    *lutBaseAddr += (uint32_t)((uint32_t)extDpdLutSel * LUT_OFFSET_BYTES);
    *lutBaseAddr += (uint32_t)((uint32_t)extDpdLutBankSel * LUT_BANK_OFFSET_BYTES);

    return recoveryAction;
}

int32_t adrv9025_ExtDpdLutPowerOnSet(adi_adrv9025_Device_t*    device,
                                     adi_adrv9025_TxChannels_e txChannel,
                                     adi_adrv9025_ExtDpdLut_e  extDpdLutSel,
                                     uint8_t                   lutPowerOnState)
{
    static const uint8_t DMA_AUTO_INCR = 0;
    static const uint8_t LUT_POWER_OFF = 1;

    int32_t  recoveryAction                                = ADI_COMMON_ACT_NO_ACTION;
    uint32_t lutPowerOnMask                                = ((uint32_t)0x00000001 << (uint8_t)extDpdLutSel);
    uint32_t lutPowerOnRegAddr                             = 0;
    uint32_t lutPowerOnRegVal                              = 0;
    uint8_t  lutPowerOnRegValArr[sizeof(lutPowerOnRegVal)] = {0};

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /* Resolve the register address depending on the Tx channel */
    switch (txChannel)
    {
    case(ADI_ADRV9025_TX1):
    {
        lutPowerOnRegAddr = (uint32_t)ADRV9025_ADDR_DPD_ACT_MMR_LUT_OFF_TX1;
        break;
    }
    case(ADI_ADRV9025_TX2):
    {
        lutPowerOnRegAddr = (uint32_t)ADRV9025_ADDR_DPD_ACT_MMR_LUT_OFF_TX2;
        break;
    }
    case(ADI_ADRV9025_TX3):
    {
        lutPowerOnRegAddr = (uint32_t)ADRV9025_ADDR_DPD_ACT_MMR_LUT_OFF_TX3;
        break;
    }
    case(ADI_ADRV9025_TX4):
    {
        lutPowerOnRegAddr = (uint32_t)ADRV9025_ADDR_DPD_ACT_MMR_LUT_OFF_TX4;
        break;
    }
    default:
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txChannel,
                         "Invalid target Tx channel selected for DPD actuator LUT power ON. Valid Tx channels are Tx1-Tx4");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    }

    /* Read the DPD LUT_OFF register */
    recoveryAction = adrv9025_CpuDmaMemRead(device,
                                            lutPowerOnRegAddr,
                                            &lutPowerOnRegValArr[0],
                                            sizeof(lutPowerOnRegValArr),
                                            DMA_AUTO_INCR);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Assemble the byte array into a 32 bit val register */
    /* Please note that it is required to read byte wise and then assemble so that the endian-ness does not matter*/
    lutPowerOnRegVal = (((uint32_t)lutPowerOnRegValArr[3] << 24) |
        ((uint32_t)lutPowerOnRegValArr[2] << 16) |
        ((uint32_t)lutPowerOnRegValArr[1] << 8) |
        (uint32_t)lutPowerOnRegValArr[0]);

    /* Power on the target LUT by the clearing the bit corresponding to the LUT */
    lutPowerOnRegVal &= ~lutPowerOnMask;

    if (lutPowerOnState == LUT_POWER_OFF)
    {
        lutPowerOnRegVal |= lutPowerOnMask;
    }

    /* Disassemble the register value into a byte array for writing back */
    lutPowerOnRegValArr[0] = (uint8_t)(lutPowerOnRegVal & 0xFF);
    lutPowerOnRegValArr[1] = (uint8_t)((lutPowerOnRegVal >> 8) & 0xFF);
    lutPowerOnRegValArr[2] = (uint8_t)((lutPowerOnRegVal >> 16) & 0xFF);
    lutPowerOnRegValArr[3] = (uint8_t)((lutPowerOnRegVal >> 24) & 0xFF);

    /* Read the DPD LUT_OFF register */
    recoveryAction = adrv9025_CpuDmaMemWrite(device,
                                             lutPowerOnRegAddr,
                                             &lutPowerOnRegValArr[0],
                                             sizeof(lutPowerOnRegValArr),
                                             ADI_ADRV9025_CPU_MEM_AUTO_INCR);
    ADI_ERROR_RETURN(device->common.error.newAction);

    return recoveryAction;
}

int32_t adrv9025_ExtDpdLutPowerOnGet(adi_adrv9025_Device_t*    device,
                                     adi_adrv9025_TxChannels_e txChannel,
                                     adi_adrv9025_ExtDpdLut_e  extDpdLutSel,
                                     uint8_t*                  extDpdLutPowerOnRegVal)
{
    static const uint8_t DMA_AUTO_INCR = 0;

    int32_t  recoveryAction                                = ADI_COMMON_ACT_NO_ACTION;
    uint32_t lutPowerOnMask                                = ((uint32_t)0x00000001 << (uint8_t)extDpdLutSel);
    uint32_t lutPowerOnRegAddr                             = 0;
    uint32_t lutPowerOnRegVal                              = 0;
    uint8_t  lutPowerOnRegValArr[sizeof(lutPowerOnRegVal)] = {0};

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /* Resolve the register address depending on the Tx channel */
    switch (txChannel)
    {
    case(ADI_ADRV9025_TX1):
    {
        lutPowerOnRegAddr = (uint32_t)ADRV9025_ADDR_DPD_ACT_MMR_LUT_OFF_TX1;
        break;
    }
    case(ADI_ADRV9025_TX2):
    {
        lutPowerOnRegAddr = (uint32_t)ADRV9025_ADDR_DPD_ACT_MMR_LUT_OFF_TX2;
        break;
    }
    case(ADI_ADRV9025_TX3):
    {
        lutPowerOnRegAddr = (uint32_t)ADRV9025_ADDR_DPD_ACT_MMR_LUT_OFF_TX3;
        break;
    }
    case(ADI_ADRV9025_TX4):
    {
        lutPowerOnRegAddr = (uint32_t)ADRV9025_ADDR_DPD_ACT_MMR_LUT_OFF_TX4;
        break;
    }
    default:
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txChannel,
                         "Invalid target Tx channel selected for DPD actuator LUT power ON state get. Valid Tx channels are Tx1-Tx4");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    }

    /* Read the DPD LUT_OFF register */
    recoveryAction = adrv9025_CpuDmaMemRead(device,
                                            lutPowerOnRegAddr,
                                            &lutPowerOnRegValArr[0],
                                            sizeof(lutPowerOnRegValArr),
                                            DMA_AUTO_INCR);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Assemble the byte array into a 32 bit val register */
    /* Please note that it is required to read byte wise and then assemble so that the endian-ness does not matter*/
    lutPowerOnRegVal = (((uint32_t)lutPowerOnRegValArr[3] << 24) |
        ((uint32_t)lutPowerOnRegValArr[2] << 16) |
        ((uint32_t)lutPowerOnRegValArr[1] << 8) |
        (uint32_t)lutPowerOnRegValArr[0]);

    *extDpdLutPowerOnRegVal = (uint8_t)((uint32_t)(lutPowerOnRegVal & lutPowerOnMask) >> (uint8_t)extDpdLutSel);

    return recoveryAction;
}

int32_t adrv9025_ExtDpdLutSwapModeEnableSet(adi_adrv9025_Device_t* device,
                                            uint8_t                lutSwapModeEn)
{
    static const uint8_t LUT_SWAP_MODE_ENABLE                      = 1;
    static const uint8_t LUT_SWAP_MODE_DISABLE                     = 0;
    static const uint8_t ARM_EXT_DPD_LUT_SWAP_MODE_EN_PARAM_OFFSET = 0;

    int32_t recoveryAction   = ADI_COMMON_ACT_NO_ACTION;
    uint8_t armConfigData[1] = {0};

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /* Write the swap mode enable config to ARM mailbox */
    armConfigData[0] = (lutSwapModeEn > 0) ? LUT_SWAP_MODE_ENABLE : LUT_SWAP_MODE_DISABLE;
    recoveryAction   = adi_adrv9025_CpuConfigWrite(device,
                                                   ADI_ADRV9025_CPU_TYPE_C,
                                                   ADRV9025_CPU_OBJECTID_EXT_DPD_SWAP_MODE_EN,
                                                   ARM_EXT_DPD_LUT_SWAP_MODE_EN_PARAM_OFFSET,
                                                   &armConfigData[0],
                                                   sizeof(armConfigData));
    ADI_ERROR_RETURN(device->common.error.newAction);

    return recoveryAction;
}

int32_t adrv9025_ExtDpdLutSwapModeEnableGet(adi_adrv9025_Device_t* device,
                                            uint8_t*               lutSwapModeEn)
{
    static const uint8_t LUT_SWAP_MODE_ENABLE                      = 1;
    static const uint8_t LUT_SWAP_MODE_DISABLE                     = 0;
    static const uint8_t ARM_EXT_DPD_LUT_SWAP_MODE_EN_PARAM_OFFSET = 0;

    int32_t recoveryAction   = ADI_COMMON_ACT_NO_ACTION;
    uint8_t armConfigData[1] = {0};

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /* Check that lutSwapModeEn is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        lutSwapModeEn);

    /* Read the swap mode enable config to ARM mailbox */
    recoveryAction = adi_adrv9025_CpuConfigRead(device,
                                                ADI_ADRV9025_CPU_TYPE_C,
                                                ADRV9025_CPU_OBJECTID_EXT_DPD_SWAP_MODE_EN,
                                                ARM_EXT_DPD_LUT_SWAP_MODE_EN_PARAM_OFFSET,
                                                &armConfigData[0],
                                                sizeof(armConfigData));
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Update the pointer to lutSwapModeEn variable */
    *lutSwapModeEn = (armConfigData[0] > 0) ? LUT_SWAP_MODE_ENABLE : LUT_SWAP_MODE_DISABLE;

    return recoveryAction;
}

int32_t adrv9025_ExtDpdActutatorEnPassthruGet(adi_adrv9025_Device_t*    device,
                                              adi_adrv9025_TxChannels_e txChannel,
                                              uint8_t*                  actuatorEnable,
                                              uint8_t*                  actuatorPassthruEn)
{
    static const uint8_t  DMA_AUTO_INCR              = 0;
    static const uint32_t DPD_CFG_ACT_EN_MASK        = 0x00000001;
    static const uint32_t DPD_CFG_ACT_PASSTHRU_MASK  = 0x00000002;
    static const uint32_t DPD_CFG_ACT_PASSTHRU_SHIFT = 1;

    int32_t  recoveryAction                        = ADI_COMMON_ACT_NO_ACTION;
    uint32_t dpdCfgRegAddr                         = 0;
    uint32_t dpdCfgRegVal                          = 0;
    uint8_t  dpdCfgRegValArr[sizeof(dpdCfgRegVal)] = {0};

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /* Check that actuatorEnable is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        actuatorEnable);

    /* Check that actuatorPassthruEn is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        actuatorPassthruEn);

    /* Resolve the register address depending on the Tx channel */
    switch (txChannel)
    {
    case(ADI_ADRV9025_TX1):
    {
        dpdCfgRegAddr = (uint32_t)ADRV9025_ADDR_DPD_ACT_MMR_DPD_CFG_TX1;
        break;
    }
    case(ADI_ADRV9025_TX2):
    {
        dpdCfgRegAddr = (uint32_t)ADRV9025_ADDR_DPD_ACT_MMR_DPD_CFG_TX2;
        break;
    }
    case(ADI_ADRV9025_TX3):
    {
        dpdCfgRegAddr = (uint32_t)ADRV9025_ADDR_DPD_ACT_MMR_DPD_CFG_TX3;
        break;
    }
    case(ADI_ADRV9025_TX4):
    {
        dpdCfgRegAddr = (uint32_t)ADRV9025_ADDR_DPD_ACT_MMR_DPD_CFG_TX4;
        break;
    }
    default:
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txChannel,
                         "Invalid target Tx channel selected for DPD actuator enable state get. Valid Tx channels are Tx1-Tx4");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    }

    /* Read the DPD CFG register */
    recoveryAction = adrv9025_CpuDmaMemRead(device,
                                            dpdCfgRegAddr,
                                            &dpdCfgRegValArr[0],
                                            sizeof(dpdCfgRegValArr),
                                            DMA_AUTO_INCR);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Assemble the byte array into a 32 bit val register */
    /* Please note that it is required to read byte wise and then assemble so that the endian-ness does not matter */
    dpdCfgRegVal = (((uint32_t)dpdCfgRegValArr[3] << 24) |
        ((uint32_t)dpdCfgRegValArr[2] << 16) |
        ((uint32_t)dpdCfgRegValArr[1] << 8) |
        (uint32_t)dpdCfgRegValArr[0]);

    /* dpdCfgRegVal-Bit[0] represents DPD actutor enable and dpdCfgRegValBit[1] represents passthru en */
    *actuatorEnable     = (uint8_t)(dpdCfgRegVal & DPD_CFG_ACT_EN_MASK);
    *actuatorPassthruEn = (uint8_t)((dpdCfgRegVal & DPD_CFG_ACT_PASSTHRU_MASK) >> DPD_CFG_ACT_PASSTHRU_SHIFT);

    return recoveryAction;
}

int32_t adrv9025_ExtDpdActutatorEnPassthruSet(adi_adrv9025_Device_t*    device,
                                              adi_adrv9025_TxChannels_e txChannel,
                                              uint8_t                   actuatorEnable,
                                              uint8_t                   actuatorPassthruEn)
{
    static const uint8_t  DMA_AUTO_INCR                             = 0;
    static const uint32_t DPD_CFG_ACTUATOR_ENABLE_MASK              = 0x00000001;
    static const uint32_t DPD_CFG_ACTUATOR_PASSTHRU_MASK            = 0x00000002;
    static const uint32_t DPD_CFG_ACTUATOR_ENABLE_AND_PASSTHRU_MASK = 0x00000003;
    static const uint32_t DPD_CFG_ACTUATOR_PASSTHRU_SHIFT           = 1;

    int32_t  recoveryAction                        = ADI_COMMON_ACT_NO_ACTION;
    uint32_t dpdCfgRegAddr                         = 0;
    uint32_t dpdCfgRegVal                          = 0;
    uint8_t  dpdCfgRegValArr[sizeof(dpdCfgRegVal)] = {0};

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /* Resolve the register address depending on the Tx channel */
    switch (txChannel)
    {
    case(ADI_ADRV9025_TX1):
    {
        dpdCfgRegAddr = (uint32_t)ADRV9025_ADDR_DPD_ACT_MMR_DPD_CFG_TX1;
        break;
    }
    case(ADI_ADRV9025_TX2):
    {
        dpdCfgRegAddr = (uint32_t)ADRV9025_ADDR_DPD_ACT_MMR_DPD_CFG_TX2;
        break;
    }
    case(ADI_ADRV9025_TX3):
    {
        dpdCfgRegAddr = (uint32_t)ADRV9025_ADDR_DPD_ACT_MMR_DPD_CFG_TX3;
        break;
    }
    case(ADI_ADRV9025_TX4):
    {
        dpdCfgRegAddr = (uint32_t)ADRV9025_ADDR_DPD_ACT_MMR_DPD_CFG_TX4;
        break;
    }
    default:
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txChannel,
                         "Invalid target Tx channel selected for DPD actuator enable state set. Valid Tx channels are Tx1-Tx4");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    }

    /* Read the DPD CFG register */
    recoveryAction = adrv9025_CpuDmaMemRead(device,
                                            dpdCfgRegAddr,
                                            &dpdCfgRegValArr[0],
                                            sizeof(dpdCfgRegValArr),
                                            DMA_AUTO_INCR);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Assemble the byte array into a 32 bit val register */
    /* Please note that it is required to read byte wise and then assemble so that the endian-ness does not matter */
    dpdCfgRegVal = (((uint32_t)dpdCfgRegValArr[3] << 24) |
        ((uint32_t)dpdCfgRegValArr[2] << 16) |
        ((uint32_t)dpdCfgRegValArr[1] << 8) |
        (uint32_t)dpdCfgRegValArr[0]);

    /* Clear the Actuator Enable bit and passthru enable bit from the register value */
    dpdCfgRegVal &= ~(DPD_CFG_ACTUATOR_ENABLE_AND_PASSTHRU_MASK);

    /* Set the DPD enable bit to the requested value */
    dpdCfgRegVal |= (uint32_t)(actuatorEnable & DPD_CFG_ACTUATOR_ENABLE_MASK);

    /* Set the DPD passthru bit to the requested value */
    dpdCfgRegVal |= (uint32_t)((actuatorPassthruEn << DPD_CFG_ACTUATOR_PASSTHRU_SHIFT) & DPD_CFG_ACTUATOR_PASSTHRU_MASK);

    /* Disassemble the register value into a byte array for writing back */
    dpdCfgRegValArr[0] = (uint8_t)(dpdCfgRegVal & 0xFF);
    dpdCfgRegValArr[1] = (uint8_t)((dpdCfgRegVal >> 8) & 0xFF);
    dpdCfgRegValArr[2] = (uint8_t)((dpdCfgRegVal >> 16) & 0xFF);
    dpdCfgRegValArr[3] = (uint8_t)((dpdCfgRegVal >> 24) & 0xFF);

    /* Write to the DPD CFG register */
    recoveryAction = adrv9025_CpuDmaMemWrite(device,
                                             dpdCfgRegAddr,
                                             &dpdCfgRegValArr[0],
                                             sizeof(dpdCfgRegValArr),
                                             ADI_ADRV9025_CPU_MEM_AUTO_INCR);
    ADI_ERROR_RETURN(device->common.error.newAction);

    return recoveryAction;
}

int32_t adrv9025_ExtDpdActuatorRamSelGet(adi_adrv9025_Device_t*    device,
                                         adi_adrv9025_TxChannels_e txChannel,
                                         uint8_t*                  extDpdActuatorRamSel)
{
    static const uint8_t  DMA_AUTO_INCR            = 0;
    static const uint32_t EXT_DPD_ACT_RAM_SEL_MASK = 0x0000001F;

    int32_t  recoveryAction                        = ADI_COMMON_ACT_NO_ACTION;
    uint32_t dpdModRegAddr                         = 0;
    uint32_t dpdModRegVal                          = 0;
    uint8_t  dpdModRegValArr[sizeof(dpdModRegVal)] = {0};

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /* Check that extDpdActuatorRamSel is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        extDpdActuatorRamSel);

    /* Resolve the register address depending on the Tx channel */
    switch (txChannel)
    {
    case(ADI_ADRV9025_TX1):
    {
        dpdModRegAddr = (uint32_t)ADRV9025_ADDR_DPD_ACT_MMR_DPD_MOD_TX1;
        break;
    }
    case(ADI_ADRV9025_TX2):
    {
        dpdModRegAddr = (uint32_t)ADRV9025_ADDR_DPD_ACT_MMR_DPD_MOD_TX2;
        break;
    }
    case(ADI_ADRV9025_TX3):
    {
        dpdModRegAddr = (uint32_t)ADRV9025_ADDR_DPD_ACT_MMR_DPD_MOD_TX3;
        break;
    }
    case(ADI_ADRV9025_TX4):
    {
        dpdModRegAddr = (uint32_t)ADRV9025_ADDR_DPD_ACT_MMR_DPD_MOD_TX4;
        break;
    }
    default:
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txChannel,
                         "Invalid target Tx channel selected for DPD actuator LUT RAM select get. Valid Tx channels are Tx1-Tx4");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    }

    /* Read the DPD CFG register */
    recoveryAction = adrv9025_CpuDmaMemRead(device,
                                            dpdModRegAddr,
                                            &dpdModRegValArr[0],
                                            sizeof(dpdModRegValArr),
                                            DMA_AUTO_INCR);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Assemble the byte array into a 32 bit val register */
    /* Please note that it is required to read byte wise and then assemble so that the endian-ness does not matter */
    dpdModRegVal = (((uint32_t)dpdModRegValArr[3] << 24) |
        ((uint32_t)dpdModRegValArr[2] << 16) |
        ((uint32_t)dpdModRegValArr[1] << 8) |
        (uint32_t)dpdModRegValArr[0]);

    *extDpdActuatorRamSel = (uint8_t)(dpdModRegVal & EXT_DPD_ACT_RAM_SEL_MASK);

    return recoveryAction;
}

int32_t adrv9025_ExtDpdActuatorRamSelSet(adi_adrv9025_Device_t*    device,
                                         adi_adrv9025_TxChannels_e txChannel,
                                         uint8_t                   extDpdActuatorRamSel)
{
    static const uint8_t  DMA_AUTO_INCR            = 0;
    static const uint32_t EXT_DPD_ACT_RAM_SEL_MASK = 0x0000001F;

    int32_t  recoveryAction                        = ADI_COMMON_ACT_NO_ACTION;
    uint32_t dpdModRegAddr                         = 0;
    uint32_t dpdModRegVal                          = 0;
    uint8_t  dpdModRegValArr[sizeof(dpdModRegVal)] = {0};

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /* Resolve the register address depending on the Tx channel */
    switch (txChannel)
    {
    case(ADI_ADRV9025_TX1):
    {
        dpdModRegAddr = (uint32_t)ADRV9025_ADDR_DPD_ACT_MMR_DPD_MOD_TX1;
        break;
    }
    case(ADI_ADRV9025_TX2):
    {
        dpdModRegAddr = (uint32_t)ADRV9025_ADDR_DPD_ACT_MMR_DPD_MOD_TX2;
        break;
    }
    case(ADI_ADRV9025_TX3):
    {
        dpdModRegAddr = (uint32_t)ADRV9025_ADDR_DPD_ACT_MMR_DPD_MOD_TX3;
        break;
    }
    case(ADI_ADRV9025_TX4):
    {
        dpdModRegAddr = (uint32_t)ADRV9025_ADDR_DPD_ACT_MMR_DPD_MOD_TX4;
        break;
    }
    default:
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txChannel,
                         "Invalid target Tx channel selected for DPD actuator LUT RAM select set. Valid Tx channels are Tx1-Tx4");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    }

    /* Read the DPD CFG register */
    recoveryAction = adrv9025_CpuDmaMemRead(device,
                                            dpdModRegAddr,
                                            &dpdModRegValArr[0],
                                            sizeof(dpdModRegValArr),
                                            DMA_AUTO_INCR);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Assemble the byte array into a 32 bit val register */
    /* Please note that it is required to read byte wise and then assemble so that the endian-ness does not matter */
    dpdModRegVal = (((uint32_t)dpdModRegValArr[3] << 24) |
        ((uint32_t)dpdModRegValArr[2] << 16) |
        ((uint32_t)dpdModRegValArr[1] << 8) |
        (uint32_t)dpdModRegValArr[0]);

    /* Clear the RAM SEL bits from DPD MOD register value */
    dpdModRegVal &= ~EXT_DPD_ACT_RAM_SEL_MASK;

    /* Update the RAM SEL mask */
    dpdModRegVal |= (uint32_t)((uint32_t)extDpdActuatorRamSel & EXT_DPD_ACT_RAM_SEL_MASK);

    /* Disassemble the 32 bit register value into a byte array for writing back */
    dpdModRegValArr[0] = (uint8_t)(dpdModRegVal & 0xFF);
    dpdModRegValArr[1] = (uint8_t)((dpdModRegVal >> 8) & 0xFF);
    dpdModRegValArr[2] = (uint8_t)((dpdModRegVal >> 16) & 0xFF);
    dpdModRegValArr[3] = (uint8_t)((dpdModRegVal >> 24) & 0xFF);

    /* Write to the DPD MOD register */
    recoveryAction = adrv9025_CpuDmaMemWrite(device,
                                             dpdModRegAddr,
                                             &dpdModRegValArr[0],
                                             sizeof(dpdModRegValArr),
                                             ADI_ADRV9025_CPU_MEM_AUTO_INCR);
    ADI_ERROR_RETURN(device->common.error.newAction);

    return recoveryAction;
}

int32_t adrv9025_ExtDpdTxAlternateObsSelSet(adi_adrv9025_Device_t*                  device,
                                            adi_adrv9025_TxChannels_e               txChannel,
                                            adi_adrv9025_ExtDpdCaptureTxAltObsSel_e txAlternateObservationSel)
{
    static const uint8_t  DMA_AUTO_INCR                = 0;
    static const uint32_t EXT_DPD_TX_ALT_OBS_SEL_MASK  = 0x00200000;
    static const uint8_t  EXT_DPD_TX_ALT_OBS_SEL_SHIFT = 21;

    int32_t  recoveryAction                        = ADI_COMMON_ACT_NO_ACTION;
    uint32_t dpdCfgRegAddr                         = 0;
    uint32_t dpdCfgRegVal                          = 0;
    uint8_t  dpdCfgRegValArr[sizeof(dpdCfgRegVal)] = {0};

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /* Resolve the register address depending on the Tx channel */
    switch (txChannel)
    {
    case(ADI_ADRV9025_TX1):
    {
        dpdCfgRegAddr = (uint32_t)ADRV9025_ADDR_DPD_ACT_MMR_DPD_CFG_TX1;
        break;
    }
    case(ADI_ADRV9025_TX2):
    {
        dpdCfgRegAddr = (uint32_t)ADRV9025_ADDR_DPD_ACT_MMR_DPD_CFG_TX2;
        break;
    }
    case(ADI_ADRV9025_TX3):
    {
        dpdCfgRegAddr = (uint32_t)ADRV9025_ADDR_DPD_ACT_MMR_DPD_CFG_TX3;
        break;
    }
    case(ADI_ADRV9025_TX4):
    {
        dpdCfgRegAddr = (uint32_t)ADRV9025_ADDR_DPD_ACT_MMR_DPD_CFG_TX4;
        break;
    }
    default:
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txChannel,
                         "Invalid target Tx channel selected for DPD actuator Tx alternate observation point set. Valid Tx channels are Tx1-Tx4");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    }

    /* Read the DPD CFG register */
    recoveryAction = adrv9025_CpuDmaMemRead(device,
                                            dpdCfgRegAddr,
                                            &dpdCfgRegValArr[0],
                                            sizeof(dpdCfgRegValArr),
                                            DMA_AUTO_INCR);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Assemble the byte array into a 32 bit val register */
    /* Please note that it is required to read byte wise and then assemble so that the endian-ness does not matter */
    dpdCfgRegVal = (((uint32_t)dpdCfgRegValArr[3] << 24) |
        ((uint32_t)dpdCfgRegValArr[2] << 16) |
        ((uint32_t)dpdCfgRegValArr[1] << 8) |
        (uint32_t)dpdCfgRegValArr[0]);

    /* Clear the RAM SEL bits from DPD MOD register value */
    dpdCfgRegVal &= ~EXT_DPD_TX_ALT_OBS_SEL_MASK;

    /* Update the RAM SEL mask */
    dpdCfgRegVal |= (uint32_t)((uint32_t)txAlternateObservationSel << EXT_DPD_TX_ALT_OBS_SEL_SHIFT);

    /* Disassemble the 32 bit register value into a byte array for writing back */
    dpdCfgRegValArr[0] = (uint8_t)(dpdCfgRegVal & 0xFF);
    dpdCfgRegValArr[1] = (uint8_t)((dpdCfgRegVal >> 8) & 0xFF);
    dpdCfgRegValArr[2] = (uint8_t)((dpdCfgRegVal >> 16) & 0xFF);
    dpdCfgRegValArr[3] = (uint8_t)((dpdCfgRegVal >> 24) & 0xFF);

    /* Write to the DPD MOD register */
    recoveryAction = adrv9025_CpuDmaMemWrite(device,
                                             dpdCfgRegAddr,
                                             &dpdCfgRegValArr[0],
                                             sizeof(dpdCfgRegValArr),
                                             ADI_ADRV9025_CPU_MEM_AUTO_INCR);
    ADI_ERROR_RETURN(device->common.error.newAction);

    return recoveryAction;
}

int32_t adrv9025_ExtDpdTxAlternateObsSelGet(adi_adrv9025_Device_t*                   device,
                                            adi_adrv9025_TxChannels_e                txChannel,
                                            adi_adrv9025_ExtDpdCaptureTxAltObsSel_e* txAlternateObservationSel)
{
    static const uint8_t  DMA_AUTO_INCR                = 0;
    static const uint32_t EXT_DPD_TX_ALT_OBS_SEL_MASK  = 0x00200000;
    static const uint8_t  EXT_DPD_TX_ALT_OBS_SEL_SHIFT = 21;

    int32_t  recoveryAction                        = ADI_COMMON_ACT_NO_ACTION;
    uint32_t dpdCfgRegAddr                         = 0;
    uint32_t dpdCfgRegVal                          = 0;
    uint8_t  dpdCfgRegValArr[sizeof(dpdCfgRegVal)] = {0};

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /* Check that txAlternateObservationSel is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        txAlternateObservationSel);

    /* Resolve the register address depending on the Tx channel */
    switch (txChannel)
    {
    case(ADI_ADRV9025_TX1):
    {
        dpdCfgRegAddr = (uint32_t)ADRV9025_ADDR_DPD_ACT_MMR_DPD_CFG_TX1;
        break;
    }
    case(ADI_ADRV9025_TX2):
    {
        dpdCfgRegAddr = (uint32_t)ADRV9025_ADDR_DPD_ACT_MMR_DPD_CFG_TX2;
        break;
    }
    case(ADI_ADRV9025_TX3):
    {
        dpdCfgRegAddr = (uint32_t)ADRV9025_ADDR_DPD_ACT_MMR_DPD_CFG_TX3;
        break;
    }
    case(ADI_ADRV9025_TX4):
    {
        dpdCfgRegAddr = (uint32_t)ADRV9025_ADDR_DPD_ACT_MMR_DPD_CFG_TX4;
        break;
    }
    default:
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txChannel,
                         "Invalid target Tx channel selected for DPD Tx alternate observation select get. Valid Tx channels are Tx1-Tx4");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    }

    /* Read the DPD CFG register */
    recoveryAction = adrv9025_CpuDmaMemRead(device,
                                            dpdCfgRegAddr,
                                            &dpdCfgRegValArr[0],
                                            sizeof(dpdCfgRegValArr),
                                            DMA_AUTO_INCR);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Assemble the byte array into a 32 bit val register */
    /* Please note that it is required to read byte wise and then assemble so that the endian-ness does not matter */
    dpdCfgRegVal = (((uint32_t)dpdCfgRegValArr[3] << 24) |
        ((uint32_t)dpdCfgRegValArr[2] << 16) |
        ((uint32_t)dpdCfgRegValArr[1] << 8) |
        (uint32_t)dpdCfgRegValArr[0]);

    *txAlternateObservationSel = (adi_adrv9025_ExtDpdCaptureTxAltObsSel_e)((dpdCfgRegVal & EXT_DPD_TX_ALT_OBS_SEL_MASK) >> EXT_DPD_TX_ALT_OBS_SEL_SHIFT);

    return recoveryAction;
}

int32_t adrv9025_ExtDpdTxObsSelSet(adi_adrv9025_Device_t*               device,
                                   adi_adrv9025_TxChannels_e            txChannel,
                                   adi_adrv9025_ExtDpdCaptureTxObsSel_e txObservationSel)
{
    static const uint8_t  DMA_AUTO_INCR            = 0;
    static const uint32_t EXT_DPD_TX_OBS_SEL_MASK  = 0x00010000;
    static const uint8_t  EXT_DPD_TX_OBS_SEL_SHIFT = 16;

    int32_t  recoveryAction                        = ADI_COMMON_ACT_NO_ACTION;
    uint32_t dpdCfgRegAddr                         = 0;
    uint32_t dpdCfgRegVal                          = 0;
    uint8_t  dpdCfgRegValArr[sizeof(dpdCfgRegVal)] = {0};

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /* Resolve the register address depending on the Tx channel */
    switch (txChannel)
    {
    case(ADI_ADRV9025_TX1):
    {
        dpdCfgRegAddr = (uint32_t)ADRV9025_ADDR_DPD_ACT_MMR_DPD_CFG_TX1;
        break;
    }
    case(ADI_ADRV9025_TX2):
    {
        dpdCfgRegAddr = (uint32_t)ADRV9025_ADDR_DPD_ACT_MMR_DPD_CFG_TX2;
        break;
    }
    case(ADI_ADRV9025_TX3):
    {
        dpdCfgRegAddr = (uint32_t)ADRV9025_ADDR_DPD_ACT_MMR_DPD_CFG_TX3;
        break;
    }
    case(ADI_ADRV9025_TX4):
    {
        dpdCfgRegAddr = (uint32_t)ADRV9025_ADDR_DPD_ACT_MMR_DPD_CFG_TX4;
        break;
    }
    default:
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txChannel,
                         "Invalid target Tx channel selected for DPD actuator Tx alternate observation point set. Valid Tx channels are Tx1-Tx4");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    }

    /* Read the DPD CFG register */
    recoveryAction = adrv9025_CpuDmaMemRead(device,
                                            dpdCfgRegAddr,
                                            &dpdCfgRegValArr[0],
                                            sizeof(dpdCfgRegValArr),
                                            DMA_AUTO_INCR);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Assemble the byte array into a 32 bit val register */
    /* Please note that it is required to read byte wise and then assemble so that the endian-ness does not matter */
    dpdCfgRegVal = (((uint32_t)dpdCfgRegValArr[3] << 24) |
        ((uint32_t)dpdCfgRegValArr[2] << 16) |
        ((uint32_t)dpdCfgRegValArr[1] << 8) |
        (uint32_t)dpdCfgRegValArr[0]);

    /* Clear the RAM SEL bits from DPD MOD register value */
    dpdCfgRegVal &= ~EXT_DPD_TX_OBS_SEL_MASK;

    /* Update the RAM SEL mask */
    dpdCfgRegVal |= (uint32_t)((uint32_t)txObservationSel << EXT_DPD_TX_OBS_SEL_SHIFT);

    /* Disassemble the 32 bit register value into a byte array for writing back */
    dpdCfgRegValArr[0] = (uint8_t)(dpdCfgRegVal & 0xFF);
    dpdCfgRegValArr[1] = (uint8_t)((dpdCfgRegVal >> 8) & 0xFF);
    dpdCfgRegValArr[2] = (uint8_t)((dpdCfgRegVal >> 16) & 0xFF);
    dpdCfgRegValArr[3] = (uint8_t)((dpdCfgRegVal >> 24) & 0xFF);

    /* Write to the DPD MOD register */
    recoveryAction = adrv9025_CpuDmaMemWrite(device,
                                             dpdCfgRegAddr,
                                             &dpdCfgRegValArr[0],
                                             sizeof(dpdCfgRegValArr),
                                             ADI_ADRV9025_CPU_MEM_AUTO_INCR);
    ADI_ERROR_RETURN(device->common.error.newAction);

    return recoveryAction;
}

int32_t adrv9025_ExtDpdTxObsSelGet(adi_adrv9025_Device_t*                device,
                                   adi_adrv9025_TxChannels_e             txChannel,
                                   adi_adrv9025_ExtDpdCaptureTxObsSel_e* txObservationSel)
{
    static const uint8_t  DMA_AUTO_INCR            = 0;
    static const uint32_t EXT_DPD_TX_OBS_SEL_MASK  = 0x00010000;
    static const uint8_t  EXT_DPD_TX_OBS_SEL_SHIFT = 16;

    int32_t  recoveryAction                        = ADI_COMMON_ACT_NO_ACTION;
    uint32_t dpdCfgRegAddr                         = 0;
    uint32_t dpdCfgRegVal                          = 0;
    uint8_t  dpdCfgRegValArr[sizeof(dpdCfgRegVal)] = {0};

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /* Check that txObservationSel is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        txObservationSel);

    /* Resolve the register address depending on the Tx channel */
    switch (txChannel)
    {
    case(ADI_ADRV9025_TX1):
    {
        dpdCfgRegAddr = (uint32_t)ADRV9025_ADDR_DPD_ACT_MMR_DPD_CFG_TX1;
        break;
    }
    case(ADI_ADRV9025_TX2):
    {
        dpdCfgRegAddr = (uint32_t)ADRV9025_ADDR_DPD_ACT_MMR_DPD_CFG_TX2;
        break;
    }
    case(ADI_ADRV9025_TX3):
    {
        dpdCfgRegAddr = (uint32_t)ADRV9025_ADDR_DPD_ACT_MMR_DPD_CFG_TX3;
        break;
    }
    case(ADI_ADRV9025_TX4):
    {
        dpdCfgRegAddr = (uint32_t)ADRV9025_ADDR_DPD_ACT_MMR_DPD_CFG_TX4;
        break;
    }
    default:
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txChannel,
                         "Invalid target Tx channel selected for DPD Tx alternate observation select get. Valid Tx channels are Tx1-Tx4");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    }

    /* Read the DPD CFG register */
    recoveryAction = adrv9025_CpuDmaMemRead(device,
                                            dpdCfgRegAddr,
                                            &dpdCfgRegValArr[0],
                                            sizeof(dpdCfgRegValArr),
                                            DMA_AUTO_INCR);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Assemble the byte array into a 32 bit val register */
    /* Please note that it is required to read byte wise and then assemble so that the endian-ness does not matter */
    dpdCfgRegVal = (((uint32_t)dpdCfgRegValArr[3] << 24) |
        ((uint32_t)dpdCfgRegValArr[2] << 16) |
        ((uint32_t)dpdCfgRegValArr[1] << 8) |
        (uint32_t)dpdCfgRegValArr[0]);

    *txObservationSel = (adi_adrv9025_ExtDpdCaptureTxObsSel_e)((dpdCfgRegVal & EXT_DPD_TX_OBS_SEL_MASK) >> EXT_DPD_TX_OBS_SEL_SHIFT);

    return recoveryAction;
}

int32_t adrv9025_ExtDpdActuatorGainMonitorConfigSet(adi_adrv9025_Device_t*                     device,
                                                    adi_adrv9025_ExtDpdActGainMonitorConfig_t* extDpdActGainMonitorConfig)
{
    static const uint8_t                   RESERVED_DATA            = 0;
    static const uint8_t                   GAIN_MONITOR_ENABLE_MASK = 0x01;
    static const uint8_t                   TX1_CHANNEL_MASK         = 0;
    static const uint8_t                   TX2_CHANNEL_MASK         = 1;
    static const uint8_t                   TX3_CHANNEL_MASK         = 2;
    static const uint8_t                   TX4_CHANNEL_MASK         = 3;
    static const uint16_t                  PARAM_OFFSET_BYTES_TX1   = 0;
    static const uint16_t                  PARAM_OFFSET_BYTES_TX2   = 12;
    static const uint16_t                  PARAM_OFFSET_BYTES_TX3   = 24;
    static const uint16_t                  PARAM_OFFSET_BYTES_TX4   = 36;
    static const adi_adrv9025_TxChannels_e txChannelsArr[]          = {ADI_ADRV9025_TX1, ADI_ADRV9025_TX2, ADI_ADRV9025_TX3, ADI_ADRV9025_TX4};

    int32_t  recoveryAction                        = ADI_COMMON_ACT_NO_ACTION;
    uint16_t extDpdActGainMonitorConfigOffsetBytes = 0;
    uint8_t  txChannelMask                         = 0;
    uint8_t  armConfigData[12]                     = {0};
    uint32_t txChannelIndex                        = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /* Check that extDpdActGainMonitorConfig is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        extDpdActGainMonitorConfig);

    for (txChannelIndex = 0; txChannelIndex < (uint32_t)(sizeof(txChannelsArr) / sizeof(txChannelsArr[0])); txChannelIndex++)
    {
        if ((extDpdActGainMonitorConfig->txChannelMask & (uint32_t)txChannelsArr[txChannelIndex]) != ADI_ADRV9025_TXOFF)
        {
            switch (txChannelsArr[txChannelIndex])
            {
            case(ADI_ADRV9025_TX1):
            {
                extDpdActGainMonitorConfigOffsetBytes = PARAM_OFFSET_BYTES_TX1;
                txChannelMask                         = TX1_CHANNEL_MASK;
                break;
            }
            case(ADI_ADRV9025_TX2):
            {
                extDpdActGainMonitorConfigOffsetBytes = PARAM_OFFSET_BYTES_TX2;
                txChannelMask                         = TX2_CHANNEL_MASK;
                break;
            }
            case(ADI_ADRV9025_TX3):
            {
                extDpdActGainMonitorConfigOffsetBytes = PARAM_OFFSET_BYTES_TX3;
                txChannelMask                         = TX3_CHANNEL_MASK;
                break;
            }
            case(ADI_ADRV9025_TX4):
            {
                extDpdActGainMonitorConfigOffsetBytes = PARAM_OFFSET_BYTES_TX4;
                txChannelMask                         = TX4_CHANNEL_MASK;
                break;
            }
            default:
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 txChannel,
                                 "Invalid target Tx channel selected for DPD actuator gain monitor config set. Valid Tx channels are Tx1-Tx4");
                ADI_ERROR_RETURN(device->common.error.newAction);
                break;
            }
            }

            /* Serialize the gain monitor config */
            armConfigData[0]  = (uint8_t)((uint16_t)extDpdActGainMonitorConfig->extDpdGainMonitorThresh.extDpdGainMonitorQualThresh & (uint16_t)0x00FF);
            armConfigData[1]  = (uint8_t)(((uint16_t)extDpdActGainMonitorConfig->extDpdGainMonitorThresh.extDpdGainMonitorQualThresh >> 8) & (uint16_t)0x00FF);
            armConfigData[2]  = extDpdActGainMonitorConfig->extDpdGainMonitorThresh.extDpdGainMonitorLowerThresh;
            armConfigData[3]  = extDpdActGainMonitorConfig->extDpdGainMonitorThresh.extDpdGainMonitorUpperThresh;
            armConfigData[4]  = txChannelMask;
            armConfigData[5]  = RESERVED_DATA;
            armConfigData[6]  = RESERVED_DATA;
            armConfigData[7]  = RESERVED_DATA;
            armConfigData[8]  = (extDpdActGainMonitorConfig->extDpdGainMonitorCtrl.extDpdGainMonitorEn & GAIN_MONITOR_ENABLE_MASK);
            armConfigData[9]  = (uint8_t)extDpdActGainMonitorConfig->extDpdGainMonitorCtrl.extDpdGainMonitorLowerThreshModelSel;
            armConfigData[10] = (uint8_t)extDpdActGainMonitorConfig->extDpdGainMonitorCtrl.extDpdGainMonitorUpperThreshModelSel;
            armConfigData[11] = (uint8_t)extDpdActGainMonitorConfig->extDpdGainMonitorCtrl.extDpdGainMonitorIIRDecay;

            /* Pass the DPD actuator gain monitor config to CPU */
            recoveryAction = adi_adrv9025_CpuConfigWrite(device,
                                                         ADI_ADRV9025_CPU_TYPE_C,
                                                         ADRV9025_CPU_OBJECTID_EXT_DPD_GAIN_MONITOR,
                                                         extDpdActGainMonitorConfigOffsetBytes,
                                                         &armConfigData[0],
                                                         sizeof(armConfigData));
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    return recoveryAction;
}

int32_t adrv9025_ExtDpdTddTxOnCaptureTriggerSet(adi_adrv9025_Device_t*    device,
                                                adi_adrv9025_TxChannels_e txChannel)
{
    static const uint16_t STREAM_DEBUG_REG0_ADDR = 0x00EE;

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /* Write the Tx channel to stream debug register 0 */
    recoveryAction = adi_adrv9025_SpiByteWrite(device,
                                               STREAM_DEBUG_REG0_ADDR,
                                               (uint8_t)((uint8_t)0x10 | txChannel));
    ADI_ERROR_RETURN(device->common.error.newAction);

    return recoveryAction;
}

int32_t adrv9025_ExtDpdCaptureStartTriggerSet(adi_adrv9025_Device_t* device)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t extData[1]     = {ADRV9025_CPU_OBJECTID_EXT_DPD_CAPTURE_TRIG};
    uint8_t cmdStatusByte  = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /* Command the ARM to trigger a Tx-ORx capture */
    recoveryAction = adi_adrv9025_CpuCmdWrite(device,
                                              ADI_ADRV9025_CPU_TYPE_C,
                                              (uint8_t)ADRV9025_CPU_SET_OPCODE,
                                              &extData[0],
                                              sizeof(extData));
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Wait for command to finish executing */
    recoveryAction = adi_adrv9025_CpuCmdStatusWait(device,
                                                   ADI_ADRV9025_CPU_TYPE_C,
                                                   (uint8_t)ADRV9025_CPU_SET_OPCODE,
                                                   &cmdStatusByte,
                                                   (uint32_t)ADI_ADRV9025_SETEXTDPDCAPTURETRIG_TIMEOUT_US,
                                                   (uint32_t)ADI_ADRV9025_SETEXTDPDCAPTURETRIG_INTERVAL_US);

    /* If cmdStatusByte[3:1] is non-zero then flag an ARM error */
    if ((cmdStatusByte >> 1) > 0)
    {
        recoveryAction = adrv9025_CpuCmdErrorHandler(device,
                                                     ADI_ADRV9025_CPU_TYPE_C,
                                                     ADI_ADRV9025_SRC_CPUCMD,
                                                     ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                                                              extData[0],
                                                                              cmdStatusByte),
                                                     recoveryAction);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    ADI_ERROR_RETURN(device->common.error.newAction);

    return recoveryAction;
}

int32_t adrv9025_ExtDpdHardClipperConfigSetRangeCheck(adi_adrv9025_Device_t*                 device,
                                                      adi_adrv9025_ExtDpdHardClipperConfig_t extDpdHardClipperConfig[],
                                                      uint8_t                                extDpdHardClipperCfgArraySize)
{
    static const float EXT_DPD_HARD_CLIPPER_MIN_THRESH = 0.0;
    static const float EXT_DPD_HARD_CLIPPER_MAX_THRESH = 1.0;

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t i              = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common, extDpdHardClipperConfig);

    if (extDpdHardClipperCfgArraySize == 0 || extDpdHardClipperCfgArraySize > ADI_ADRV9025_MAX_TXCHANNELS)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         extDpdHardClipperCfgArraySize,
                         "Invalid number of external DPD hard clipper configurations");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    for (i = 0; i < extDpdHardClipperCfgArraySize; i++)
    {
        if ((extDpdHardClipperConfig[i].extDpdHardClipperEnable > 0) &&
            (extDpdHardClipperConfig[i].txChannelMask != (uint32_t)ADI_ADRV9025_TXOFF))
        {
            if (extDpdHardClipperConfig[i].extDpdHardClipperThreshold > EXT_DPD_HARD_CLIPPER_MAX_THRESH ||
                extDpdHardClipperConfig[i].extDpdHardClipperThreshold < EXT_DPD_HARD_CLIPPER_MIN_THRESH)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 extDpdHardClipperConfig[i].extDpdHardClipperThreshold,
                                 "Invalid External DPD Hard clipper threshold encountered. Valid range is 0 to 1");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
        }
    }

    return recoveryAction;
}

int32_t adrv9025_ExtDpdHardClipperConfigSet(adi_adrv9025_Device_t*                  device,
                                            adi_adrv9025_ExtDpdHardClipperConfig_t* extDpdHardClipperConfig)
{
    static const float   FIXED_POINT_SCALER       = 32768; /* The post DPD hard clipper threshold is in 1.15 format */
    static const uint8_t EXT_DPD_HARD_CLIPPER_SET = 0x10;
    static const uint8_t ARM_ERR_CODE             = 0x0E;

    int32_t  recoveryAction      = ADI_COMMON_ACT_NO_ACTION;
    uint8_t  extData[4]          = {ADRV9025_CPU_OBJECTID_TRACKING_CAL_CTRL, ADRV9025_CPU_OBJECTID_DPD_INIT, 0u, EXT_DPD_HARD_CLIPPER_SET};
    uint8_t  cmdStatusByte       = 0u;
    uint8_t  txChannelIndex      = 0u;
    uint8_t  txChannelMask       = 0u;
    uint8_t  armData[3]          = {0u};
    uint16_t fixedPointThreshold = 0u;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /* Check that clgcConfig is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        extDpdHardClipperConfig);

    fixedPointThreshold = (uint16_t)(extDpdHardClipperConfig->extDpdHardClipperThreshold * FIXED_POINT_SCALER);

    armData[0] = extDpdHardClipperConfig->extDpdHardClipperEnable ? 1u : 0u;
    armData[1] = (uint8_t)(fixedPointThreshold & 0xFF);
    armData[2] = (uint8_t)((fixedPointThreshold >> 8) & 0xFF);

    for (txChannelIndex = 0u; txChannelIndex < ADI_ADRV9025_MAX_TXCHANNELS; txChannelIndex++)
    {
        txChannelMask = 0x1 << txChannelIndex;
        if (extDpdHardClipperConfig->txChannelMask & txChannelMask)
        {
            extData[2] = txChannelMask;

            /* Write the hard clipper settings to ARM mailbox */
            recoveryAction = adi_adrv9025_CpuMemWrite(device,
                                                      ADRV9025_CPU_C_ADDR_MAILBOX_SET,
                                                      &armData[0],
                                                      sizeof(armData),
                                                      ADI_ADRV9025_CPU_MEM_AUTO_INCR);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             device->common.error.errCode,
                             recoveryAction,
                             armCtrlData,
                             device->common.error.errormessage);
            ADI_ERROR_RETURN(device->common.error.newAction);

            /* Executing Arm Command */
            recoveryAction = adi_adrv9025_CpuCmdWrite(device,
                                                      ADI_ADRV9025_CPU_TYPE_C,
                                                      (uint8_t)ADRV9025_CPU_SET_OPCODE,
                                                      &extData[0],
                                                      sizeof(extData));
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             device->common.error.errCode,
                             recoveryAction,
                             extData,
                             device->common.error.errormessage);
            ADI_ERROR_RETURN(device->common.error.newAction);

            /* Wait for command to finish executing */
            recoveryAction = adi_adrv9025_CpuCmdStatusWait(device,
                                                           ADI_ADRV9025_CPU_TYPE_C,
                                                           ADRV9025_CPU_SET_OPCODE,
                                                           &cmdStatusByte,
                                                           (uint32_t)ADI_ADRV9025_SETEXTDHARDCLIPPER_TIMEOUT_US,
                                                           (uint32_t)ADI_ADRV9025_SETEXTDHARDCLIPPER_INTERVAL_US);

            /* If cmdStatusByte is non-zero then flag an ARM error */
            if ((cmdStatusByte & ARM_ERR_CODE) > 0)
            {
                recoveryAction = adrv9025_CpuCmdErrorHandler(device,
                                                             ADI_ADRV9025_CPU_TYPE_C,
                                                             ADI_ADRV9025_SRC_CPUCMD,
                                                             ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                                                                      extData[0],
                                                                                      cmdStatusByte),
                                                             recoveryAction);
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             device->common.error.errCode,
                             recoveryAction,
                             cmdStatusByte,
                             device->common.error.errormessage);
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    return recoveryAction;
}

int32_t adrv9025_ExtDpdCaptureDataReadDoneSet(adi_adrv9025_Device_t* device)
{
    static const uint8_t CAPTURE_READ_DONE_CMD_CTRL_ID = 0x12;
    static const uint8_t ARM_ERROR_STATUS              = 0x0E;

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t extData[4]     = {ADRV9025_CPU_OBJECTID_TRACKING_CAL_CTRL, ADRV9025_CPU_OBJECTID_DPD_INIT, 0x00, CAPTURE_READ_DONE_CMD_CTRL_ID};
    uint8_t cmdStatusByte  = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /* Issue a command to the FW to indicate end of capture data read */
    recoveryAction = adi_adrv9025_CpuCmdWrite(device,
                                              ADI_ADRV9025_CPU_TYPE_C,
                                              (uint8_t)ADRV9025_CPU_SET_OPCODE,
                                              &extData[0],
                                              sizeof(extData));
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Wait for command to finish executing */
    recoveryAction = adi_adrv9025_CpuCmdStatusWait(device,
                                                   ADI_ADRV9025_CPU_TYPE_C,
                                                   (uint8_t)ADRV9025_CPU_SET_OPCODE,
                                                   &cmdStatusByte,
                                                   (uint32_t)ADI_ADRV9025_SETEXTDPDCAPTUREDONE_TIMEOUT_US,
                                                   (uint32_t)ADI_ADRV9025_GETEXTDPDCAPTUREDONE_INTERVAL_US);

    /* If cmdStatusByte[3:1] is non-zero then flag an ARM error */
    if ((cmdStatusByte & ARM_ERROR_STATUS) > 0)
    {
        recoveryAction = adrv9025_CpuCmdErrorHandler(device,
                                                     ADI_ADRV9025_CPU_TYPE_C,
                                                     ADI_ADRV9025_SRC_CPUCMD,
                                                     ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE,
                                                                              extData[0],
                                                                              cmdStatusByte),
                                                     recoveryAction);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    ADI_ERROR_RETURN(device->common.error.newAction);

    return recoveryAction;
}

int32_t adrv9025_ExtDpdTxCaptureDataGet(adi_adrv9025_Device_t* device,
                                        int16_t                extDpdTxCaptureIData[],
                                        int16_t                extDpdTxCaptureQData[],
                                        uint16_t               maxTxCaptureSampleSize)
{
    static const uint8_t  DMA_AUTO_INCR                   = 1;
    static const uint16_t MAX_SAMPLES                     = 4096;
    static const uint32_t SAMPLE_IQ_MULTIPLICATION_FACTOR = 2;
    static const uint32_t BYTES_PER_SAMPLE                = 2;
#define MAX_CAPTURE_BUFFER_READ_SIZE_BYTES 16384U

    int32_t        recoveryAction                                    = ADI_COMMON_ACT_NO_ACTION;
    uint32_t       captureBufferReadSize                             = 0;
    static uint8_t captureBuffer[MAX_CAPTURE_BUFFER_READ_SIZE_BYTES] = {0};

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /* Check that extDpdTxCaptureIData, extDpdTxCaptureQData pointers are not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        extDpdTxCaptureIData);
    ADI_NULL_PTR_RETURN(&device->common,
                        extDpdTxCaptureQData);

    if (maxTxCaptureSampleSize > MAX_SAMPLES) /* Set the maximum read size to 4k samples */
    {
        maxTxCaptureSampleSize = MAX_SAMPLES;

        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ACT_WARN_CHECK_PARAM,
                         ADI_COMMON_ACT_NO_ACTION,
                         maxTxCaptureSampleSize,
                         "Maximum sample size that can be read back is 4096. A maximum of 4096 IQ samples will be retrieved from DPD capture memory.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Each sample contains an I and Q data, so multiply the read size by a factor of 2 */
    captureBufferReadSize = (maxTxCaptureSampleSize * SAMPLE_IQ_MULTIPLICATION_FACTOR);

    /* Each I and Q sample consist of 2 bytes of data, so multiply the read size by a factor of 2 */
    captureBufferReadSize *= BYTES_PER_SAMPLE;

    if (captureBufferReadSize != 0)
    {
        recoveryAction = adrv9025_CpuDmaMemRead(device,
                                                ADRV9025_ADDR_DPD_TX_CAPTURE_DATA,
                                                captureBuffer,
                                                captureBufferReadSize,
                                                DMA_AUTO_INCR);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_ExtDpdCaptureDataParse(device,
                                                         captureBuffer,
                                                         extDpdTxCaptureIData,
                                                         extDpdTxCaptureQData,
                                                         maxTxCaptureSampleSize);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return recoveryAction;
}

int32_t adrv9025_ExtDpdTxAlternateCaptureDataGet(adi_adrv9025_Device_t* device,
                                                 int16_t                extDpdTxCaptureIData[],
                                                 int16_t                extDpdTxCaptureQData[],
                                                 uint16_t               maxTxCaptureSampleSize)
{
    static const uint8_t  DMA_AUTO_INCR                   = 1;
    static const uint16_t MAX_SAMPLES                     = 4096;
    static const uint32_t SAMPLE_IQ_MULTIPLICATION_FACTOR = 2;
    static const uint32_t BYTES_PER_SAMPLE                = 2;
#define MAX_CAPTURE_BUFFER_READ_SIZE_BYTES 16384U

    int32_t        recoveryAction                                    = ADI_COMMON_ACT_NO_ACTION;
    uint32_t       captureBufferReadSize                             = 0;
    static uint8_t captureBuffer[MAX_CAPTURE_BUFFER_READ_SIZE_BYTES] = {0};

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /* Check that extDpdTxCaptureIData, extDpdTxCaptureQData pointers are not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        extDpdTxCaptureIData);
    ADI_NULL_PTR_RETURN(&device->common,
                        extDpdTxCaptureQData);

    if (maxTxCaptureSampleSize > MAX_SAMPLES) /* Set the maximum read size to 4k samples */
    {
        maxTxCaptureSampleSize = MAX_SAMPLES;

        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ACT_WARN_CHECK_PARAM,
                         ADI_COMMON_ACT_NO_ACTION,
                         maxTxCaptureSampleSize,
                         "Maximum sample size that can be read back is 4096. A maximum of 4096 IQ samples will be retrieved from DPD capture memory.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Each sample contains an I and Q data, so multiply the read size by a factor of 2 */
    captureBufferReadSize = (maxTxCaptureSampleSize * SAMPLE_IQ_MULTIPLICATION_FACTOR);

    /* Each I and Q sample consist of 2 bytes of data, so multiply the read size by a factor of 2 */
    captureBufferReadSize *= BYTES_PER_SAMPLE;

    if (captureBufferReadSize != 0)
    {
        recoveryAction = adrv9025_CpuDmaMemRead(device,
                                                ADRV9025_ADDR_DPD_ALT_TX_CAPTURE_DATA,
                                                captureBuffer,
                                                captureBufferReadSize,
                                                DMA_AUTO_INCR);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_ExtDpdCaptureDataParse(device,
                                                         captureBuffer,
                                                         extDpdTxCaptureIData,
                                                         extDpdTxCaptureQData,
                                                         maxTxCaptureSampleSize);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return recoveryAction;
}

int32_t adrv9025_ExtDpdOrxCaptureDataGet(adi_adrv9025_Device_t* device,
                                         int16_t                extDpdOrxCaptureIData[],
                                         int16_t                extDpdOrxCaptureQData[],
                                         uint16_t               maxOrxCaptureSampleSize)
{
    static const uint8_t  DMA_AUTO_INCR                   = 1;
    static const uint16_t MAX_SAMPLES                     = 4096;
    static const uint32_t SAMPLE_IQ_MULTIPLICATION_FACTOR = 2;
    static const uint32_t BYTES_PER_SAMPLE                = 2;
#define MAX_CAPTURE_BUFFER_READ_SIZE_BYTES 16384U
    static uint8_t captureBuffer[MAX_CAPTURE_BUFFER_READ_SIZE_BYTES] = {0};

    int32_t  recoveryAction        = ADI_COMMON_ACT_NO_ACTION;
    uint32_t captureBufferReadSize = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    /* Check that extDpdOrxCaptureIData, extDpdOrxCaptureQData pointers are not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        extDpdOrxCaptureIData);
    ADI_NULL_PTR_RETURN(&device->common,
                        extDpdOrxCaptureQData);

    if (maxOrxCaptureSampleSize > MAX_SAMPLES) /* Set the maximum read size to 4k samples */
    {
        maxOrxCaptureSampleSize = MAX_SAMPLES;

        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ACT_WARN_CHECK_PARAM,
                         ADI_COMMON_ACT_NO_ACTION,
                         maxTxCaptureSampleSize,
                         "Maximum sample size that can be read back is 4096. A maximum of 4096 IQ samples will be retrieved from DPD capture memory.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Each sample contains an I and Q data, so multiply the read size by a factor of 2 */
    captureBufferReadSize = (maxOrxCaptureSampleSize * SAMPLE_IQ_MULTIPLICATION_FACTOR);

    /* Each I and Q sample consist of 2 bytes of data, so multiply the read size by a factor of 2 */
    captureBufferReadSize *= BYTES_PER_SAMPLE;

    if (captureBufferReadSize != 0)
    {
        recoveryAction = adrv9025_CpuDmaMemRead(device,
                                                ADRV9025_ADDR_DPD_ORX_CAPTURE_DATA,
                                                captureBuffer,
                                                captureBufferReadSize,
                                                DMA_AUTO_INCR);
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_ExtDpdCaptureDataParse(device,
                                                         captureBuffer,
                                                         extDpdOrxCaptureIData,
                                                         extDpdOrxCaptureQData,
                                                         maxOrxCaptureSampleSize);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return recoveryAction;
}

int32_t adrv9025_ExtDpdCaptureDetailedStatusDeserialize(adi_adrv9025_Device_t*                      device,
                                                        adi_adrv9025_ExtDpdCaptureDetailedStatus_t* extDpdCaptureDetailedStatus,
                                                        uint8_t*                                    buf,
                                                        uint8_t                                     size)
{
    static const uint8_t EXT_DPD_STATUS_SIZE = 26u;
    int32_t              recoveryAction      = ADI_COMMON_ACT_NO_ACTION;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Check that extDpdCaptureDetailedStatus and buf pointer is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        extDpdCaptureDetailedStatus);
    ADI_NULL_PTR_RETURN(&device->common,
                        buf);

    if (size < EXT_DPD_STATUS_SIZE)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         size,
                         "Invalid buffer size. Buffer needs to be 26 bytes or larger");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    extDpdCaptureDetailedStatus->extDpdCaptureErrorCode = (adi_adrv9025_ExtDpdError_e)((uint32_t)buf[0] |
        (uint32_t)buf[1] << 8 |
        (uint32_t)buf[2] << 16 |
        (uint32_t)buf[3] << 24);
    extDpdCaptureDetailedStatus->extDpdCaptureTimeoutStatus       = buf[20];
    extDpdCaptureDetailedStatus->extDpdCaptureBandTransitionCount = buf[21];
    extDpdCaptureDetailedStatus->extDpdCapturePowerBandStatus     = buf[22];
    extDpdCaptureDetailedStatus->extDpdCaptureCount               = (uint16_t)((uint16_t)buf[24] | (uint16_t)buf[25] << 8);

    return recoveryAction;
}

int32_t adrv9025_ExtDpdExtendedPeakSearchCaptureDataGet(adi_adrv9025_Device_t *device, adi_adrv9025_ExtDpdCaptureData_t *extDpdCaptureData, uint8_t numOfLutBanksToRead)
{
    #define NUM_OF_BYTES_IN_A_BANK 1024u
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    static const uint8_t MAX_NUM_OF_BANKS_TO_READ = 16;
    static const uint8_t NUM_OF_BANKS_IN_LUT = 4;
    static const uint8_t LUT_OFFSET_TO_READ = 27;
    uint8_t lutDataArray[NUM_OF_BYTES_IN_A_BANK] = { 0 };
    uint8_t i = 0;
    uint16_t j = 0;
    uint8_t remainder = 0;
    uint16_t sampleCounter = 0;

    adi_adrv9025_ExtDpdLut_e lutSel = ADI_ADRV9025_EXT_DPD_LUT27;
    adi_adrv9025_ExtDpdLutBank_e bankSel = ADI_ADRV9025_EXT_DPD_LUT_BANK0;
    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API_PRIV);

    /* Check that extDpdCaptureData pointer is not null */
    ADI_NULL_PTR_RETURN(&device->common, extDpdCaptureData);

    if (numOfLutBanksToRead > MAX_NUM_OF_BANKS_TO_READ)
    {
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM, ADI_COMMON_ACT_ERR_CHECK_PARAM, numOfLutBanksToRead,
            "Max number of banks to read should be equal or less than 16");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Read Tx capture data from Tx1 LUT memories */
    for (i = 0; i < numOfLutBanksToRead; ++i)
    {
        remainder = i % NUM_OF_BANKS_IN_LUT;
        bankSel = (adi_adrv9025_ExtDpdLutBank_e)(remainder);
        lutSel = (adi_adrv9025_ExtDpdLut_e)(((i - remainder) / NUM_OF_BANKS_IN_LUT) + LUT_OFFSET_TO_READ);

        recoveryAction = adi_adrv9025_ExtDpdActuatorLutRead(device, ADI_ADRV9025_TX1, lutSel, bankSel, &lutDataArray[0], (uint16_t)sizeof(lutDataArray));
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, device->common.error.errCode, recoveryAction, NULL, device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);

        for (j = 0; j < (uint16_t)sizeof(lutDataArray); j += 4)
        {
            extDpdCaptureData->txCaptureData.extDpdCaptureDataQ[sampleCounter] = (int16_t)((uint16_t)lutDataArray[j] | ((uint16_t)lutDataArray[j + 1] << 8));
            extDpdCaptureData->txCaptureData.extDpdCaptureDataI[sampleCounter++] = (int16_t)((uint16_t)lutDataArray[j + 2] | ((uint16_t)lutDataArray[j + 3] << 8));
            if (sampleCounter >= extDpdCaptureData->txCaptureData.extDpdCaptureSampleArrSize)
            {
                break;
            }
        }
    }

    /* Read Orx capture data from Tx2 LUT memories */
    sampleCounter = 0;
    for (i = 0; i < numOfLutBanksToRead; ++i)
    {
        remainder = i % NUM_OF_BANKS_IN_LUT;
        bankSel = (adi_adrv9025_ExtDpdLutBank_e)(remainder);
        lutSel = (adi_adrv9025_ExtDpdLut_e)(((i - remainder) / NUM_OF_BANKS_IN_LUT) + LUT_OFFSET_TO_READ);

        recoveryAction = adi_adrv9025_ExtDpdActuatorLutRead(device, ADI_ADRV9025_TX2, lutSel, bankSel, &lutDataArray[0], (uint16_t)sizeof(lutDataArray));
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, device->common.error.errCode, recoveryAction, NULL, device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);

        for (j = 0; j < (uint16_t)sizeof(lutDataArray); j += 4)
        {
            extDpdCaptureData->orxCaptureData.extDpdCaptureDataQ[sampleCounter] = (int16_t)((uint16_t)lutDataArray[j] | ((uint16_t)lutDataArray[j + 1] << 8));
            extDpdCaptureData->orxCaptureData.extDpdCaptureDataI[sampleCounter++] = (int16_t)((uint16_t)lutDataArray[j + 2] | ((uint16_t)lutDataArray[j + 3] << 8));
            if (sampleCounter >= extDpdCaptureData->orxCaptureData.extDpdCaptureSampleArrSize)
            {
                break;
            }
        }
    }

    return recoveryAction;
}

static int32_t adrv9025_CorrMemWriteAddrOffsetGet(uint16_t row, uint32_t *addressOffset)
{
    static const uint32_t CORR_MEMORY_BASE_ADDR = 0x62020000;
    static const uint32_t NUMBER_OF_BYTES_IN_A_ROW = 1024;

    *addressOffset = (uint32_t)row * NUMBER_OF_BYTES_IN_A_ROW;
    *addressOffset += CORR_MEMORY_BASE_ADDR;
    return ADI_COMMON_ACT_NO_ACTION;
}

int32_t adrv9025_ExtDpdActuatorMultiModelSwapLutWrite(adi_adrv9025_Device_t *device, adi_adrv9025_ExtDpdLut_e lutIndex, uint8_t lutData[], uint16_t lutDataSize)
{
    static const uint16_t NUM_OF_BYTES_FOR_LUT = 1024;
    static const uint16_t SHIFT_FOR_NUM_OF_WORDS = 3;
    static const uint16_t MASK_FOR_COLUMN = 0x003F;
    static const uint16_t NUM_OF_BYTES_IN_A_ROW = 512;
    static const uint8_t zeroEntryToWrite[512] = { 0, 0, 0, 0 };
    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    if (lutDataSize > 0)
    {
        /* User is asked to pass null pointer for the LUT's they don't want to configure , i.e. if lutDataSize == 0 */
        ADI_NULL_PTR_RETURN(&device->common, lutData);
    }

    uint16_t i = 0;
    uint16_t row = 0;
    uint16_t wordIndex = 0;
    uint32_t addressOffset = 0;

    for (i = 0; i < NUM_OF_BYTES_FOR_LUT; i += NUM_OF_BYTES_IN_A_ROW)
    {

        wordIndex = i >> SHIFT_FOR_NUM_OF_WORDS;

        if (wordIndex > MASK_FOR_COLUMN)
        {
            row = lutIndex * 2 + 1;
        }
        else
        {
            row = lutIndex * 2;
        }

        adrv9025_CorrMemWriteAddrOffsetGet(row, &addressOffset);

        if (lutDataSize >= NUM_OF_BYTES_IN_A_ROW)
        {
            adrv9025_CpuDmaMemWrite(device, addressOffset, &lutData[i], NUM_OF_BYTES_IN_A_ROW, ADI_ADRV9025_CPU_MEM_AUTO_INCR);
            ADI_ERROR_RETURN(device->common.error.newAction);
            lutDataSize -= NUM_OF_BYTES_IN_A_ROW;
        }
        else if (lutDataSize > 0) 
        {
            /* First clear the row */
            adrv9025_CpuDmaMemWrite(device, addressOffset, &zeroEntryToWrite[0], NUM_OF_BYTES_IN_A_ROW, ADI_ADRV9025_CPU_MEM_AUTO_INCR);
            ADI_ERROR_RETURN(device->common.error.newAction);

            /* Then write first n bytes of row */
            adrv9025_CpuDmaMemWrite(device, addressOffset, &lutData[i], lutDataSize, ADI_ADRV9025_CPU_MEM_AUTO_INCR);
            ADI_ERROR_RETURN(device->common.error.newAction);
            lutDataSize = 0;
        }
        else
        {
            /* Clear the row */
            adrv9025_CpuDmaMemWrite(device, addressOffset, &zeroEntryToWrite[0], NUM_OF_BYTES_IN_A_ROW, ADI_ADRV9025_CPU_MEM_AUTO_INCR);
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    return device->common.error.newAction;
}

int32_t adrv9025_ExtDpdActuatorMultiModelSwapLutWriteRangeCheck(adi_adrv9025_Device_t *device, adi_adrv9025_SwapModeLut_t multiModelSwapModeLutData[])
{
    uint8_t lutIndex = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API_PRIV);

    /* Check that lutData is not null */
    ADI_NULL_PTR_RETURN(&device->common, multiModelSwapModeLutData);


    for (lutIndex = 0; lutIndex <= (uint8_t)ADI_ADRV9025_EXT_DPD_LUT25; lutIndex++)
    {
        /* If an LUT is requested to be programmed */
        if ((multiModelSwapModeLutData[lutIndex].extDpdSwapModeLutDataPtr != NULL) &&
            (multiModelSwapModeLutData[lutIndex].extDpdSwapModeLutDataSizeBytes > 0))
        {
            /* Check that the no. of LUT entries do not exceed 256 entries(1024 bytes) */
            if (multiModelSwapModeLutData[lutIndex].extDpdSwapModeLutDataSizeBytes > 1024)
            {
                ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM, ADI_COMMON_ACT_ERR_CHECK_PARAM, multiModelSwapModeLutData[lutIndex].extDpdSwapModeLutDataSizeBytes,
                    "LUT data size cannot be larger than 1024");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            if ((multiModelSwapModeLutData[lutIndex].extDpdSwapModeLutDataSizeBytes % 4) != 0)
            {
                ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM, ADI_COMMON_ACT_ERR_CHECK_PARAM, multiModelSwapModeLutData[lutIndex].extDpdSwapModeLutDataSizeBytes,
                    "LUT data size should be multiple of 4 because each entry has 4 bytes.");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

        }
    }

    return (device->common.error.newAction);
}

