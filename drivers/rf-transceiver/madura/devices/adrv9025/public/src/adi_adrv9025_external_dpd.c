/**
* \file adi_adrv9025_external_dpd.c
* \brief Contains external DPD features related function implementation defined in
* adi_adrv9025_external_dpd.h
*
* ADRV9025 API Version: $ADI_ADRV9025_API_VERSION$
*/

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the ADRV9025 API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

#include "adi_adrv9025_user.h"
#include "adi_adrv9025_error.h"
#include "adi_adrv9025_external_dpd.h"
#include "adi_adrv9025_gpio.h"
#include "adi_adrv9025_cpu.h"
#include "adi_adrv9025_hal.h"
#include "../../private/include/adrv9025_tx.h"
#include "../../private/include/adrv9025_init.h"
#include "../../private/include/adrv9025_radioctrl.h"
#include "../../private/include/adrv9025_cpu.h"
#include "../../private/include/adrv9025_cpu_macros.h"
#include "../../private/include/adrv9025_external_dpd.h"
#include "../../private/include/adrv9025_reg_addr_macros.h"
#include "../../private/include/adrv9025_shared_resource_manager.h"

#ifdef _RELEASE_BUILD_
    #line __LINE__ "adi_adrv9025_external_dpd.c"
#endif

int32_t adi_adrv9025_ExtDpdCaptureConfigSet(adi_adrv9025_Device_t*              device,
                                            adi_adrv9025_ExtDpdCaptureConfig_t* dpdCaptureConfig)
{
    #define ARM_EXT_DPD_CAPTURE_CFG_LENGTH_BYTES 58u
    static const uint16_t ARM_EXT_DPD_CAPTURE_PARAM_OFFSET = 0;

    uint8_t armConfigData[ARM_EXT_DPD_CAPTURE_CFG_LENGTH_BYTES] = {0};
    uint8_t signalId                                            = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /*Check dpdCaptureConfig pointer is not null*/
    ADI_NULL_PTR_RETURN(&device->common,
                        dpdCaptureConfig);

#if ADI_ADRV9025_EXT_DPD_RANGE_CHECK > 0
    adrv9025_ExtDpdCaptureConfigSetRangeCheck(device,
                                              dpdCaptureConfig);
    ADI_ERROR_RETURN(device->common.error.newAction);
#endif

    /* Setup the GPIO pin for triggering Tx-ORx DPD sample capture */
    signalId = (dpdCaptureConfig->extDpdCaptureTriggerPin != ADI_ADRV9025_GPIO_INVALID) ? (uint8_t)dpdCaptureConfig->extDpdCaptureTriggerPin : 0;
    adrv9025_StreamGpioPinSet(device,
                              signalId,
                              dpdCaptureConfig->extDpdCaptureTriggerPin);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Store the external DPD capture trigger GPIO in the device struct */
    device->devStateInfo.extDpdCaptureTriggerGpio = dpdCaptureConfig->extDpdCaptureTriggerPin;

    /* Setup the GPIO pin for capture complete indication */
    adrv9025_ExtDpdCaptureDoneStatusGpioSetup(device,
                                              dpdCaptureConfig->extDpdCaptureDoneStatusPin);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Load the ARM mailbox array with config data */
    adrv9025_ExtDpdCaptureConfigParamArrayLoad(device,
                                               dpdCaptureConfig,
                                               &armConfigData[0],
                                               (uint8_t)ARM_EXT_DPD_CAPTURE_CFG_LENGTH_BYTES);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Write the external dpd capture parameters to ARM mailbox */
    adi_adrv9025_CpuConfigWrite(device,
                                ADI_ADRV9025_CPU_TYPE_C,
                                ADRV9025_CPU_OBJECTID_EXT_DPD_CAPTURE,
                                ARM_EXT_DPD_CAPTURE_PARAM_OFFSET,
                                &armConfigData[0],
                                sizeof(armConfigData));
    ADI_ERROR_RETURN(device->common.error.newAction);

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_ExtDpdCaptureConfigGet(adi_adrv9025_Device_t*              device,
                                            adi_adrv9025_ExtDpdCaptureConfig_t* dpdCaptureConfig)
{
    #define ARM_EXT_DPD_CAPTURE_CFG_DATA_GET_LENGTH_BYTES 58u
    static const uint16_t ARM_EXT_DPD_CAPTURE_PARAM_OFFSET = 0;

    uint8_t armConfigDataGet[ARM_EXT_DPD_CAPTURE_CFG_DATA_GET_LENGTH_BYTES] = {0};

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Check that dpdCaptureConfig pointer is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        dpdCaptureConfig);

    /* Retrieve the ARM GPIO pin assigned to Ext DPD Capture Trigger */
    dpdCaptureConfig->extDpdCaptureTriggerPin = device->devStateInfo.extDpdCaptureTriggerGpio;

    /* Retrieve the ARM GPIO pin assigned to Ext DPD Capture Done indication */
    adrv9025_ExtDpdCaptureDoneStatusGpioGet(device,
                                            &(dpdCaptureConfig->extDpdCaptureDoneStatusPin));
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Read ext DPD capture config from ARM mailbox */
    adi_adrv9025_CpuConfigRead(device,
                               ADI_ADRV9025_CPU_TYPE_C,
                               ADRV9025_CPU_OBJECTID_EXT_DPD_CAPTURE,
                               ARM_EXT_DPD_CAPTURE_PARAM_OFFSET,
                               &armConfigDataGet[0],
                               sizeof(armConfigDataGet));
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Parse the config data byte array received from ARM into the dpd config data structure */
    adrv9025_ExtDpdCaptureConfigParamArrayParse(device,
                                                dpdCaptureConfig,
                                                &armConfigDataGet[0],
                                                (uint8_t)ARM_EXT_DPD_CAPTURE_CFG_DATA_GET_LENGTH_BYTES);
    ADI_ERROR_RETURN(device->common.error.newAction);

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_ExtDpdCaptureCtrlConfigSet(adi_adrv9025_Device_t*            device,
                                                adi_adrv9025_ExtDpdCaptureCtrl_t* dpdCaptureCtrl)
{
#define ARM_EXT_DPD_CAPTURE_CTRL_CFG_LENGTH_BYTES 22u
    static const uint16_t ARM_EXT_DPD_CAPTURE_PARAM_OFFSET = 0x24;
    static const uint8_t  TX_CHANNEL1                      = 0;
    static const uint8_t  TX_CHANNEL2                      = 1;
    static const uint8_t  TX_CHANNEL3                      = 2;
    static const uint8_t  TX_CHANNEL4                      = 3;
    static const uint8_t  CAPTURE_TYPE_SEL_MASK            = 0x01;
    static const uint8_t  PEAK_CAPTURE_IIR_MASK            = 0x01;
    static const uint8_t  PEAK_CAPTURE_IIR_BYPASS_SHIFT    = 1;
    static const uint8_t  TDD_TXON_SYNC_CAPTURE_EN         = 0x00;
    static const uint8_t  LEGACY_MODE_CAPTURE_EN           = 0x08;
    static const uint8_t EXTENDED_PEAK_SEARCH_SHIFT = 6;

    uint8_t armConfigData[ARM_EXT_DPD_CAPTURE_CTRL_CFG_LENGTH_BYTES] = {0};
    uint8_t extDpdCaptureTxChannel                                   = 0;
    uint8_t extDpdTddSyncCapEn                                       = (dpdCaptureCtrl->dpdCaptureTypeSel == ADI_ADRV9025_EXT_DPD_CAPTURE_TDD_TXON_SYNC)
                                                                           ? TDD_TXON_SYNC_CAPTURE_EN
                                                                           : LEGACY_MODE_CAPTURE_EN;
    uint8_t extDpdExtendedPeakSearchEn = (dpdCaptureCtrl->dpdCaptureTypeSel == ADI_ADRV9025_EXT_DPD_CAPTURE_EXTENDED_PEAK_SEARCH) ? ((uint8_t)ADI_TRUE << EXTENDED_PEAK_SEARCH_SHIFT) : ((uint8_t)ADI_FALSE << EXTENDED_PEAK_SEARCH_SHIFT);

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Check that dpdCaptureCtrl pointer is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        dpdCaptureCtrl);

#if ADI_ADRV9025_EXT_DPD_RANGE_CHECK > 0
    adrv9025_ExtDpdCaptureCtrlConfigSetRangeCheck(device,
                                                  dpdCaptureCtrl);
    ADI_ERROR_RETURN(device->common.error.newAction);
#endif

    switch (dpdCaptureCtrl->extDpdCapturetxChannelSel)
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
                         dpdCaptureCtrl->extDpdCapturetxChannelSel,
                         "Invalid Tx channel encountered while attempting to write ext DPD capture ctrl config. Tx1-Tx4 are the valid Tx channels");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    }

    /* Load the ARM config array with config data */
    armConfigData[0] = (uint8_t)(dpdCaptureCtrl->dpdPeakCaptureConfig.extDpdPeakExtendedWindowUs & 0xFF);
    armConfigData[1] = (uint8_t)((dpdCaptureCtrl->dpdPeakCaptureConfig.extDpdPeakExtendedWindowUs >> 8) & 0xFF);
    armConfigData[2] = (uint8_t)((dpdCaptureCtrl->dpdPeakCaptureConfig.extDpdPeakExtendedWindowUs >> 16) & 0xFF);
    armConfigData[3] = (uint8_t)((dpdCaptureCtrl->dpdPeakCaptureConfig.extDpdPeakExtendedWindowUs >> 24) & 0xFF);
    armConfigData[4] = ((dpdCaptureCtrl->dpdCaptureTypeSel == ADI_ADRV9025_EXT_DPD_CAPTURE_PEAK_DETECTION) || (dpdCaptureCtrl->dpdCaptureTypeSel == ADI_ADRV9025_EXT_DPD_CAPTURE_EXTENDED_PEAK_SEARCH)) ? 1 : 0;
    armConfigData[5] = dpdCaptureCtrl->dpdPeakCaptureConfig.extDpdPeakDecay;
    armConfigData[6] = (uint8_t)dpdCaptureCtrl->dpdPeakCaptureConfig.extDpdPeakCaptureInputSel;
    armConfigData[7] = ((dpdCaptureCtrl->dpdPeakCaptureConfig.extDpdPeakDetectIIREnable & PEAK_CAPTURE_IIR_MASK) |
        ((dpdCaptureCtrl->dpdPeakCaptureConfig.extDpdPeakDetectIIRBypass & PEAK_CAPTURE_IIR_MASK) << PEAK_CAPTURE_IIR_BYPASS_SHIFT));
    armConfigData[8] = (uint8_t)(dpdCaptureCtrl->dpdPeakCaptureConfig.extDpdPeakWinCnt & 0xFF);
    armConfigData[9] = (uint8_t)((dpdCaptureCtrl->dpdPeakCaptureConfig.extDpdPeakWinCnt >> 8) & 0xFF);
    armConfigData[10] = (uint8_t)((dpdCaptureCtrl->dpdPeakCaptureConfig.extDpdPeakWinCnt >> 16) & 0xFF);
    armConfigData[11] = (uint8_t)((dpdCaptureCtrl->dpdPeakCaptureConfig.extDpdPeakWinCnt >> 24) & 0xFF);
    armConfigData[12] = extDpdCaptureTxChannel;
    armConfigData[13] = ((uint8_t)(dpdCaptureCtrl->dpdCaptureTypeSel & CAPTURE_TYPE_SEL_MASK) |
        (uint8_t)((uint8_t)dpdCaptureCtrl->dpdCaptureTxObsSel << 1) |
        (uint8_t)((uint8_t)dpdCaptureCtrl->dpdCaptureTxAltObsSel << 2) |
        extDpdTddSyncCapEn | extDpdExtendedPeakSearchEn);

    armConfigData[14] = (uint8_t)dpdCaptureCtrl->dpdCaptureSize;
    armConfigData[16] = (uint8_t)(dpdCaptureCtrl->extDpdCaptureDelay & 0xFF);
    armConfigData[17] = (uint8_t)((dpdCaptureCtrl->extDpdCaptureDelay >> 8) & 0xFF);
    armConfigData[18] = (uint8_t)((dpdCaptureCtrl->extDpdCaptureDelay >> 16) & 0xFF);
    armConfigData[19] = (uint8_t)((dpdCaptureCtrl->extDpdCaptureDelay >> 24) & 0xFF);
    armConfigData[20] = (uint8_t)(dpdCaptureCtrl->extDpdCaptureAlignDelay & 0xFF);
    armConfigData[21] = (uint8_t)((dpdCaptureCtrl->extDpdCaptureAlignDelay >> 8) & 0xFF);
    
    /* Pass the external dpd capture ctrl config to ARM */
    adi_adrv9025_CpuConfigWrite(device,
                                ADI_ADRV9025_CPU_TYPE_C,
                                ADRV9025_CPU_OBJECTID_EXT_DPD_CAPTURE,
                                ARM_EXT_DPD_CAPTURE_PARAM_OFFSET,
                                &armConfigData[0],
                                sizeof(armConfigData));
    ADI_ERROR_RETURN(device->common.error.newAction);

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_ExtDpdCaptureCtrlConfigGet(adi_adrv9025_Device_t*            device,
                                                adi_adrv9025_ExtDpdCaptureCtrl_t* dpdCaptureCtrl)
{
#define ARM_EXT_DPD_CAPTURE_CTRL_CFG_GET_LENGTH_BYTES 22u
    static const uint16_t ARM_EXT_DPD_CAPTURE_PARAM_OFFSET = 0x24;
    static const uint8_t  TX_CHANNEL1                      = 0;
    static const uint8_t  TX_CHANNEL2                      = 1;
    static const uint8_t  TX_CHANNEL3                      = 2;
    static const uint8_t  TX_CHANNEL4                      = 3;
    static const uint8_t  CAPTURE_TX_OBS_SEL_MASK          = 0x02;
    static const uint8_t  CAPTURE_TX_OBS_SEL_SHIFT         = 1;
    static const uint8_t  CAPTURE_TX_ALT_OBS_SEL_MASK      = 0x04;
    static const uint8_t  CAPTURE_TX_ALT_OBS_SEL_SHIFT     = 2;
    static const uint8_t  CAPTURE_TYPE_MASK                = 0x01;
    static const uint8_t  PEAK_CAPTURE_IIR_ENABLE_MASK     = 0x01;
    static const uint8_t  PEAK_CAPTURE_IIR_BYPASS_MASK     = 0x02;
    static const uint8_t  PEAK_CAPTURE_IIR_BYPASS_SHIFT    = 1;
    static const uint8_t  TDD_TXON_SYNC_CAPTURE_EN_MASK    = 0x08;
    static const uint8_t  EXTENDED_PEAK_SEARCH_EN_MASK       = 0x40;

    uint8_t armConfigData[ARM_EXT_DPD_CAPTURE_CTRL_CFG_GET_LENGTH_BYTES] = {0};

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Check that dpdCaptureCtrl pointer is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        dpdCaptureCtrl);

    /* Read the Ext DPD ctrl config from ARM */
    adi_adrv9025_CpuConfigRead(device,
                               ADI_ADRV9025_CPU_TYPE_C,
                               ADRV9025_CPU_OBJECTID_EXT_DPD_CAPTURE,
                               ARM_EXT_DPD_CAPTURE_PARAM_OFFSET,
                               &armConfigData[0],
                               sizeof(armConfigData));
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Parse the config data byte array received from ARM into the dpd ctrl config data structure */
    dpdCaptureCtrl->dpdPeakCaptureConfig.extDpdPeakExtendedWindowUs = (((uint32_t)armConfigData[3] << 24) | ((uint32_t)armConfigData[2] << 16) | ((uint32_t)armConfigData[1] << 8) | (uint32_t)armConfigData[0]);
    dpdCaptureCtrl->dpdPeakCaptureConfig.extDpdPeakDecay = armConfigData[5];
    dpdCaptureCtrl->dpdPeakCaptureConfig.extDpdPeakCaptureInputSel = (adi_adrv9025_ExtDpdCapturePeakDetectInputSel_e)armConfigData[6];
    dpdCaptureCtrl->dpdPeakCaptureConfig.extDpdPeakDetectIIREnable = (armConfigData[7] & PEAK_CAPTURE_IIR_ENABLE_MASK);
    dpdCaptureCtrl->dpdPeakCaptureConfig.extDpdPeakDetectIIRBypass = ((armConfigData[7] & PEAK_CAPTURE_IIR_BYPASS_MASK) >> PEAK_CAPTURE_IIR_BYPASS_SHIFT);
    dpdCaptureCtrl->dpdPeakCaptureConfig.extDpdPeakWinCnt = (((uint32_t)armConfigData[11] << 24) | ((uint32_t)armConfigData[10] << 16) | ((uint32_t)armConfigData[9] << 8) | (uint32_t)armConfigData[8]);
    dpdCaptureCtrl->extDpdCapturetxChannelSel = (armConfigData[12] == TX_CHANNEL1) ? ADI_ADRV9025_TX1 :
        (armConfigData[12] == TX_CHANNEL2) ? ADI_ADRV9025_TX2 :
        (armConfigData[12] == TX_CHANNEL3) ? ADI_ADRV9025_TX3 :
        (armConfigData[12] == TX_CHANNEL4) ? ADI_ADRV9025_TX4 :
        (adi_adrv9025_TxChannels_e)armConfigData[12]; /* Invalid Case */
    dpdCaptureCtrl->dpdCaptureTypeSel = ((armConfigData[13] & EXTENDED_PEAK_SEARCH_EN_MASK) != 0) ? ADI_ADRV9025_EXT_DPD_CAPTURE_EXTENDED_PEAK_SEARCH :
        (armConfigData[4] == 1) ? ADI_ADRV9025_EXT_DPD_CAPTURE_PEAK_DETECTION :
        ((armConfigData[13] & TDD_TXON_SYNC_CAPTURE_EN_MASK) == 0) ? ADI_ADRV9025_EXT_DPD_CAPTURE_TDD_TXON_SYNC :
        (adi_adrv9025_ExtDpdCaptureType_e)(armConfigData[13] & CAPTURE_TYPE_MASK);

    dpdCaptureCtrl->dpdCaptureTxObsSel = (adi_adrv9025_ExtDpdCaptureTxObsSel_e)((armConfigData[13] & CAPTURE_TX_OBS_SEL_MASK) >> CAPTURE_TX_OBS_SEL_SHIFT);
    dpdCaptureCtrl->dpdCaptureTxAltObsSel = (adi_adrv9025_ExtDpdCaptureTxAltObsSel_e)((armConfigData[13] & CAPTURE_TX_ALT_OBS_SEL_MASK) >> CAPTURE_TX_ALT_OBS_SEL_SHIFT);
    dpdCaptureCtrl->dpdCaptureSize = (adi_adrv9025_ExtDpdCaptureSize_e)armConfigData[14];
    /* Omit armConfigData[15] for arm structure alignment */
    dpdCaptureCtrl->extDpdCaptureDelay = (((uint32_t)armConfigData[19] << 24) | ((uint32_t)armConfigData[18] << 16) | ((uint32_t)armConfigData[17] << 8) | (uint32_t)armConfigData[16]);
    dpdCaptureCtrl->extDpdCaptureAlignDelay = (((uint16_t)armConfigData[21] << 8) | (uint16_t)armConfigData[20]);

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_ExtDpdCaptureLevelConfigSet(adi_adrv9025_Device_t*             device,
                                                 adi_adrv9025_ExtDpdCaptureLevel_t* dpdCaptureLevel)
{
#define ARM_EXT_DPD_CAPTURE_LVL_CFG_LENGTH_BYTES 4u
    static const uint16_t ARM_EXT_DPD_CAPTURE_PARAM_OFFSET = 0x1C;

    uint8_t armConfigData[ARM_EXT_DPD_CAPTURE_LVL_CFG_LENGTH_BYTES] = {0};

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Check that dpdCaptureLevel pointer is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        dpdCaptureLevel);

#if ADI_ADRV9025_EXT_DPD_RANGE_CHECK > 0
    adrv9025_ExtDpdCaptureLevelConfigSetRangeCheck(device,
                                                   dpdCaptureLevel);
    ADI_ERROR_RETURN(device->common.error.newAction);
#endif

    /* Load the ARM mailbox array with config data */
    armConfigData[0] = (uint8_t)(dpdCaptureLevel->extDpdCaptureTimeOut & 0xFF);
    armConfigData[1] = (uint8_t)((dpdCaptureLevel->extDpdCaptureTimeOut >> 8) & 0xFF);
    armConfigData[2] = (uint8_t)dpdCaptureLevel->dpdCaptureBandDir;
    armConfigData[3] = (uint8_t)dpdCaptureLevel->dpdCaptureBandSel;

    /* Pass the external dpd capture ctrl config to ARM */
    adi_adrv9025_CpuConfigWrite(device,
                                ADI_ADRV9025_CPU_TYPE_C,
                                ADRV9025_CPU_OBJECTID_EXT_DPD_CAPTURE,
                                ARM_EXT_DPD_CAPTURE_PARAM_OFFSET,
                                &armConfigData[0],
                                sizeof(armConfigData));
    ADI_ERROR_RETURN(device->common.error.newAction);

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_ExtDpdCaptureLevelConfigGet(adi_adrv9025_Device_t*             device,
                                                 adi_adrv9025_ExtDpdCaptureLevel_t* dpdCaptureLevel)
{
#define ARM_EXT_DPD_CAPTURE_LVL_CFG_GET_LENGTH_BYTES 4u
    static const uint16_t ARM_EXT_DPD_CAPTURE_PARAM_OFFSET = 0x1C;

    uint8_t armConfigData[ARM_EXT_DPD_CAPTURE_LVL_CFG_GET_LENGTH_BYTES] = {0};

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Check that dpdCaptureCtrl pointer is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        dpdCaptureLevel);

    /* Read the Ext DPD level config from ARM */
    adi_adrv9025_CpuConfigRead(device,
                               ADI_ADRV9025_CPU_TYPE_C,
                               ADRV9025_CPU_OBJECTID_EXT_DPD_CAPTURE,
                               ARM_EXT_DPD_CAPTURE_PARAM_OFFSET,
                               &armConfigData[0],
                               sizeof(armConfigData));
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Parse the config data byte array received from ARM into the dpd level config data structure */
    dpdCaptureLevel->extDpdCaptureTimeOut = (((uint16_t)armConfigData[1] << 8) | (uint16_t)armConfigData[0]);
    dpdCaptureLevel->dpdCaptureBandDir    = (adi_adrv9025_ExtDpdCaptureBandDir_e)armConfigData[2];
    dpdCaptureLevel->dpdCaptureBandSel    = (adi_adrv9025_ExtDpdCaptureBand_e)armConfigData[3];

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_ExtDpdCapturePwrThreshSet(adi_adrv9025_Device_t*                 device,
                                               adi_adrv9025_ExtDpdPwrThreshBandPair_e targetDpdCaptureBandSel,
                                               adi_adrv9025_ExtDpdCapturePwrThresh_t* dpdCapturePwrThresh)
{
#define ARM_EXT_DPD_CAPTURE_PWR_THRESH_CFG_LENGTH_BYTES 8u
    static const uint16_t ARM_EXT_DPD_CAPTURE_PARAM_OFFSET_BAND01 = 0x04;
    static const uint16_t ARM_EXT_DPD_CAPTURE_PARAM_OFFSET_BAND12 = 0x0C;
    static const uint16_t ARM_EXT_DPD_CAPTURE_PARAM_OFFSET_BAND23 = 0x14;
    static const uint16_t RESERVED_DATA                           = 0;

    uint8_t  armConfigData[ARM_EXT_DPD_CAPTURE_PWR_THRESH_CFG_LENGTH_BYTES] = {0};
    uint16_t paramOffset                                                    = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Check that dpdCapturePwrThresh pointer is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        dpdCapturePwrThresh);

#if ADI_ADRV9025_EXT_DPD_RANGE_CHECK > 0
    adrv9025_ExtDpdCapturePwrThreshSetRangeCheck(device,
                                                 targetDpdCaptureBandSel,
                                                 dpdCapturePwrThresh);
    ADI_ERROR_RETURN(device->common.error.newAction);
#endif

    /* Resolve the param offset depending on the power band selected */
    switch (targetDpdCaptureBandSel)
    {
    case(ADI_ADRV9025_EXT_DPD_POWER_BAND_01):
    {
        paramOffset = ARM_EXT_DPD_CAPTURE_PARAM_OFFSET_BAND01;
        break;
    }
    case(ADI_ADRV9025_EXT_DPD_POWER_BAND_12):
    {
        paramOffset = ARM_EXT_DPD_CAPTURE_PARAM_OFFSET_BAND12;
        break;
    }
    case(ADI_ADRV9025_EXT_DPD_POWER_BAND_23):
    {
        paramOffset = ARM_EXT_DPD_CAPTURE_PARAM_OFFSET_BAND23;
        break;
    }
    default:
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         targetDpdCaptureBandSel,
                         "Invalid power band pair selected for configuring power thresholds for DPD sample capture. Band01,12,23 are the valid target bands");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    }

    /* Load the ARM mailbox array with config data */
    armConfigData[0] = dpdCapturePwrThresh->envelopePwrLowToHighLimit;
    armConfigData[1] = dpdCapturePwrThresh->envelopePwrHighToLowLimit;
    armConfigData[2] = (uint8_t)dpdCapturePwrThresh->envelopePwrPulseWidthResolution;
    armConfigData[3] = (uint8_t)RESERVED_DATA;
    armConfigData[4] = (uint8_t)(dpdCapturePwrThresh->envelopePwrLowToHighPulseWidth & 0xFF);
    armConfigData[5] = (uint8_t)((dpdCapturePwrThresh->envelopePwrLowToHighPulseWidth >> 8) & 0xFF);
    armConfigData[6] = (uint8_t)(dpdCapturePwrThresh->envelopePwrHighToLowPulseWidth & 0xFF);
    armConfigData[7] = (uint8_t)((dpdCapturePwrThresh->envelopePwrHighToLowPulseWidth >> 8) & 0xFF);

    /* Pass the external dpd power thresh config to ARM */
    adi_adrv9025_CpuConfigWrite(device,
                                ADI_ADRV9025_CPU_TYPE_C,
                                ADRV9025_CPU_OBJECTID_EXT_DPD_CAPTURE,
                                paramOffset,
                                &armConfigData[0],
                                sizeof(armConfigData));
    ADI_ERROR_RETURN(device->common.error.newAction);

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_ExtDpdCapturePwrThreshGet(adi_adrv9025_Device_t*                 device,
                                               adi_adrv9025_ExtDpdPwrThreshBandPair_e targetDpdCaptureBandSel,
                                               adi_adrv9025_ExtDpdCapturePwrThresh_t* dpdCapturePwrThresh)
{
#define ARM_EXT_DPD_CAPTURE_PWR_THRESH_CFG_GET_LENGTH_BYTES 8u
    static const uint16_t ARM_EXT_DPD_CAPTURE_PARAM_OFFSET_BAND01 = 0x04;
    static const uint16_t ARM_EXT_DPD_CAPTURE_PARAM_OFFSET_BAND12 = 0x0C;
    static const uint16_t ARM_EXT_DPD_CAPTURE_PARAM_OFFSET_BAND23 = 0x14;

    uint8_t  armConfigData[ARM_EXT_DPD_CAPTURE_PWR_THRESH_CFG_GET_LENGTH_BYTES] = {0};
    uint16_t paramOffset                                                        = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Check that dpdCapturePwrThresh pointer is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        dpdCapturePwrThresh);

    /* Resolve the param offset depending on the power band selected */
    switch (targetDpdCaptureBandSel)
    {
    case(ADI_ADRV9025_EXT_DPD_POWER_BAND_01):
    {
        paramOffset = ARM_EXT_DPD_CAPTURE_PARAM_OFFSET_BAND01;
        break;
    }
    case(ADI_ADRV9025_EXT_DPD_POWER_BAND_12):
    {
        paramOffset = ARM_EXT_DPD_CAPTURE_PARAM_OFFSET_BAND12;
        break;
    }
    case(ADI_ADRV9025_EXT_DPD_POWER_BAND_23):
    {
        paramOffset = ARM_EXT_DPD_CAPTURE_PARAM_OFFSET_BAND23;
        break;
    }
    default:
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         targetDpdCaptureBandSel,
                         "Invalid power band pair selected for configuring power thresholds for DPD sample capture. Band01,12,23 are the valid target bands");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    }

    /* Read the Ext DPD level config from ARM */
    adi_adrv9025_CpuConfigRead(device,
                               ADI_ADRV9025_CPU_TYPE_C,
                               ADRV9025_CPU_OBJECTID_EXT_DPD_CAPTURE,
                               paramOffset,
                               &armConfigData[0],
                               sizeof(armConfigData));
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Parse the config data byte array received from ARM into the dpd power thresh config data structure */
    dpdCapturePwrThresh->envelopePwrLowToHighLimit       = armConfigData[0];
    dpdCapturePwrThresh->envelopePwrHighToLowLimit       = armConfigData[1];
    dpdCapturePwrThresh->envelopePwrPulseWidthResolution = (adi_adrv9025_ExtDpdCapturePulseWidthRes_e)armConfigData[2];
    dpdCapturePwrThresh->envelopePwrLowToHighPulseWidth  = (((uint16_t)armConfigData[5] << 8) | (uint16_t)armConfigData[4]);
    dpdCapturePwrThresh->envelopePwrHighToLowPulseWidth  = (((uint16_t)armConfigData[7] << 8) | (uint16_t)armConfigData[6]);

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_ExtDpdCaptureStartTriggerSet(adi_adrv9025_Device_t* device)
{
    adi_adrv9025_ExtDpdCaptureCtrl_t extDpdCaptureCtrl = {
        ADI_ADRV9025_TXOFF,
        ADI_ADRV9025_EXT_DPD_CAPTURE_IMMEDIATE_TRIGGER,
        ADI_ADRV9025_EXT_DPD_CAPTURE_TX_OBS_POST_DPD_ACTUATOR,
        ADI_ADRV9025_EXT_DPD_CAPTURE_TX_ALT_OBS_DPD_ACTUATOR,
        ADI_ADRV9025_EXT_DPD_CAPTURE_SIZE_4096_SAMPLES,
        {ADI_ADRV9025_EXT_DPD_CAPTURE_PEAK_DET_DPD_INPUT, 0, 0, 0, 0},
        0,
        0
    };

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Retrieve the capture control settings */
    adi_adrv9025_ExtDpdCaptureCtrlConfigGet(device,
                                            &extDpdCaptureCtrl);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Verify that the correct Tx channel is configured prior to calling this API */
    if ((extDpdCaptureCtrl.extDpdCapturetxChannelSel != ADI_ADRV9025_TX1) &&
        (extDpdCaptureCtrl.extDpdCapturetxChannelSel != ADI_ADRV9025_TX2) &&
        (extDpdCaptureCtrl.extDpdCapturetxChannelSel != ADI_ADRV9025_TX3) &&
        (extDpdCaptureCtrl.extDpdCapturetxChannelSel != ADI_ADRV9025_TX4))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         extDpdCaptureCtrl.extDpdCapturetxChannelSel,
                         "Invalid Tx channel configured in device for capturing Tx data");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (extDpdCaptureCtrl.dpdCaptureTypeSel == ADI_ADRV9025_EXT_DPD_CAPTURE_TDD_TXON_SYNC)
    {
        adrv9025_ExtDpdTddTxOnCaptureTriggerSet(device,
                                                extDpdCaptureCtrl.extDpdCapturetxChannelSel);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else
    {
        adrv9025_ExtDpdCaptureStartTriggerSet(device);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_ExtDpdCaptureDoneStatusGet(adi_adrv9025_Device_t* device,
                                                uint8_t*               captureCompleteFlag)
{
    static const uint8_t EXT_DPD_CAPTURE_DONE_FLAG = 0x01;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Check that captureCompleteFlag pointer is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        captureCompleteFlag);

    /* Query the ARM for DPD capture done status */
    adrv9025_ExtDpdCaptureDoneStatusQuery(device,
                                          captureCompleteFlag);
    ADI_ERROR_RETURN(device->common.error.newAction);

    if (*captureCompleteFlag == EXT_DPD_CAPTURE_DONE_FLAG)
    {
        adrv9025_ExtDpdCaptureDoneStatusClear(device);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_ExtDpdCaptureDetailedStatusGet(adi_adrv9025_Device_t*                      device,
                                                    adi_adrv9025_TxChannels_e                   txChannel,
                                                    adi_adrv9025_ExtDpdCaptureDetailedStatus_t* extDpdCaptureDetailedStatus)
{
    static const uint8_t CPU_ERR_CODE   = 0x0E;
    int32_t              recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t              extData[3]     = {ADRV9025_CPU_OBJECTID_CAL_STATUS, ADRV9025_CPU_OBJECTID_DPD_INIT, 0};
    uint8_t              cpuData[26]    = {0};
    uint8_t              cmdStatusByte  = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Check that extDpdCaptureDetailedStatus pointer is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        extDpdCaptureDetailedStatus);

    switch (txChannel)
    {
    case(ADI_ADRV9025_TX1):
    {
        extData[2] = 0; /*!< Tx1 channel enabled */
        break;
    }
    case(ADI_ADRV9025_TX2):
    {
        extData[2] = 1; /*!< Tx2 channel enabled */
        break;
    }
    case(ADI_ADRV9025_TX3):
    {
        extData[2] = 2; /*!< Tx3 channel enabled */
        break;
    }
    case(ADI_ADRV9025_TX4):
    {
        extData[2] = 3; /*!< Tx4 channel enabled */
        break;
    }
    default:
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txChannel,
                         "Invalid Tx channel requested for detailed DPD sample capture status. Valid Tx channels are Tx1-Tx4");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    }

    /* Executing extDPD status get command */
    recoveryAction = adi_adrv9025_CpuCmdWrite(device,
                                              ADI_ADRV9025_CPU_TYPE_C,
                                              (uint8_t)ADRV9025_CPU_GET_OPCODE,
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
                                                   (uint8_t)ADRV9025_CPU_GET_OPCODE,
                                                   &cmdStatusByte,
                                                   (uint32_t)ADI_ADRV9025_GETTXDPDSTATUS_TIMEOUT_US,
                                                   (uint32_t)ADI_ADRV9025_GETTXDPDSTATUS_INTERVAL_US);

    /* If cmdStatusByte is non-zero then flag a CPU error */
    if ((cmdStatusByte & CPU_ERR_CODE) > 0)
    {
        adrv9025_CpuCmdErrorHandler(device,
                                    ADI_ADRV9025_CPU_TYPE_C,
                                    ADI_ADRV9025_SRC_CPUCMD,
                                    ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
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

    /* Read the extDPD status from CPU mailbox */
    recoveryAction = adi_adrv9025_CpuMemRead(device,
                                             (uint32_t)ADRV9025_CPU_C_ADDR_MAILBOX_GET,
                                             &cpuData[0],
                                             sizeof(cpuData),
                                             ADI_ADRV9025_CPU_MEM_AUTO_INCR);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     cpuData,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Deserializing extDPD Status into the structure. */
    adrv9025_ExtDpdCaptureDetailedStatusDeserialize(device,
                                                    extDpdCaptureDetailedStatus,
                                                    &cpuData[0],
                                                    sizeof(cpuData));

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_ExtDpdActiveModelStatusGet(adi_adrv9025_Device_t*         device,
                                                adi_adrv9025_TxChannels_e      txChannel,
                                                adi_adrv9025_ExtDpdModelSel_e* extDpdActiveModelSel)
{
    static const uint8_t ACTIVE_DPD_MODEL_MASK = 0x03;

    uint32_t regAddr   = 0;
    uint8_t  regVal[4] = {0};

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Check that extDpdCaptureDetailedStatus pointer is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        extDpdActiveModelSel);

    switch (txChannel)
    {
    case(ADI_ADRV9025_TX1):
    {
        regAddr = ADRV9025_ADDR_DPD_ACT_MMR_STAT_TX1;
        break;
    }
    case(ADI_ADRV9025_TX2):
    {
        regAddr = ADRV9025_ADDR_DPD_ACT_MMR_STAT_TX2;
        break;
    }
    case(ADI_ADRV9025_TX3):
    {
        regAddr = ADRV9025_ADDR_DPD_ACT_MMR_STAT_TX3;
        break;
    }
    case(ADI_ADRV9025_TX4):
    {
        regAddr = ADRV9025_ADDR_DPD_ACT_MMR_STAT_TX4;
        break;
    }
    default:
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txChannel,
                         "Invalid Tx channel requested for DPD active model status. Valid Tx channels are Tx1-Tx4");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    }

    /* Read the 32 bit dpd status register */
    adrv9025_CpuDmaMemRead(device,
                           regAddr,
                           &regVal[0],
                           sizeof(regVal),
                           0);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Load the dpd model select pointer with the updated active model select retrieved from the device */
    *extDpdActiveModelSel = (adi_adrv9025_ExtDpdModelSel_e)(regVal[1] & ACTIVE_DPD_MODEL_MASK);

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_ExtDpdCaptureDataGet(adi_adrv9025_Device_t*            device,
                                          adi_adrv9025_ExtDpdCaptureData_t* extDpdCaptureData)
{
    uint8_t numOfLutBanksToRead = 0;
    adi_adrv9025_ExtDpdCaptureCtrl_t extDpdCaptureCtrl = {
        ADI_ADRV9025_TXOFF,
        ADI_ADRV9025_EXT_DPD_CAPTURE_IMMEDIATE_TRIGGER,
        ADI_ADRV9025_EXT_DPD_CAPTURE_TX_OBS_POST_DPD_ACTUATOR,
        ADI_ADRV9025_EXT_DPD_CAPTURE_TX_ALT_OBS_DPD_ACTUATOR,
        ADI_ADRV9025_EXT_DPD_CAPTURE_SIZE_4096_SAMPLES,
        {ADI_ADRV9025_EXT_DPD_CAPTURE_PEAK_DET_DPD_INPUT, 0, 0, 0, 0},
        0,
        0
    };

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Check that extDpdCaptureData pointer is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        extDpdCaptureData);

    /* Retrieve the ext dpd capture control config to determine capture size */
    adi_adrv9025_ExtDpdCaptureCtrlConfigGet(device,
                                            &extDpdCaptureCtrl);
    ADI_ERROR_RETURN(device->common.error.newAction);

    switch (extDpdCaptureCtrl.dpdCaptureSize)
    {
    case(ADI_ADRV9025_EXT_DPD_CAPTURE_SIZE_4096_SAMPLES):
    {
        extDpdCaptureData->txCaptureData.extDpdCaptureSampleArrSize          = 4096;
        extDpdCaptureData->txAlternateCaptureData.extDpdCaptureSampleArrSize = 4096;
        extDpdCaptureData->orxCaptureData.extDpdCaptureSampleArrSize         = 4096;
        numOfLutBanksToRead = 16;
        break;
    }
    case(ADI_ADRV9025_EXT_DPD_CAPTURE_SIZE_2048_SAMPLES):
    {
        extDpdCaptureData->txCaptureData.extDpdCaptureSampleArrSize          = 2048;
        extDpdCaptureData->txAlternateCaptureData.extDpdCaptureSampleArrSize = 2048;
        extDpdCaptureData->orxCaptureData.extDpdCaptureSampleArrSize         = 2048;
        numOfLutBanksToRead = 8;
        break;
    }
    case(ADI_ADRV9025_EXT_DPD_CAPTURE_SIZE_1024_SAMPLES):
    {
        extDpdCaptureData->txCaptureData.extDpdCaptureSampleArrSize          = 1024;
        extDpdCaptureData->txAlternateCaptureData.extDpdCaptureSampleArrSize = 1024;
        extDpdCaptureData->orxCaptureData.extDpdCaptureSampleArrSize         = 1024;
        numOfLutBanksToRead = 4;
        break;
    }
    case(ADI_ADRV9025_EXT_DPD_CAPTURE_SIZE_512_SAMPLES):
    {
        extDpdCaptureData->txCaptureData.extDpdCaptureSampleArrSize          = 512;
        extDpdCaptureData->txAlternateCaptureData.extDpdCaptureSampleArrSize = 512;
        extDpdCaptureData->orxCaptureData.extDpdCaptureSampleArrSize         = 512;
        numOfLutBanksToRead = 2;
        break;
    }
    case(ADI_ADRV9025_EXT_DPD_CAPTURE_SIZE_256_SAMPLES):
    {
        extDpdCaptureData->txCaptureData.extDpdCaptureSampleArrSize          = 256;
        extDpdCaptureData->txAlternateCaptureData.extDpdCaptureSampleArrSize = 256;
        extDpdCaptureData->orxCaptureData.extDpdCaptureSampleArrSize         = 256;
        numOfLutBanksToRead = 1;
        break;
    }
    case(ADI_ADRV9025_EXT_DPD_CAPTURE_SIZE_128_SAMPLES):
    {
        extDpdCaptureData->txCaptureData.extDpdCaptureSampleArrSize          = 128;
        extDpdCaptureData->txAlternateCaptureData.extDpdCaptureSampleArrSize = 128;
        extDpdCaptureData->orxCaptureData.extDpdCaptureSampleArrSize         = 128;
        numOfLutBanksToRead = 1;
        break;
    }
    case(ADI_ADRV9025_EXT_DPD_CAPTURE_SIZE_64_SAMPLES):
    {
        extDpdCaptureData->txCaptureData.extDpdCaptureSampleArrSize          = 64;
        extDpdCaptureData->txAlternateCaptureData.extDpdCaptureSampleArrSize = 64;
        extDpdCaptureData->orxCaptureData.extDpdCaptureSampleArrSize         = 64;
        numOfLutBanksToRead = 1;
        break;
    }
    case(ADI_ADRV9025_EXT_DPD_CAPTURE_SIZE_32_SAMPLES):
    {
        extDpdCaptureData->txCaptureData.extDpdCaptureSampleArrSize          = 32;
        extDpdCaptureData->txAlternateCaptureData.extDpdCaptureSampleArrSize = 32;
        extDpdCaptureData->orxCaptureData.extDpdCaptureSampleArrSize         = 32;
        numOfLutBanksToRead = 1;
        break;
    }
    default:
    {
        extDpdCaptureData->txCaptureData.extDpdCaptureSampleArrSize          = 4096;
        extDpdCaptureData->txAlternateCaptureData.extDpdCaptureSampleArrSize = 4096;
        extDpdCaptureData->orxCaptureData.extDpdCaptureSampleArrSize         = 4096;
        numOfLutBanksToRead = 16;
        break;
    }
    }

    if (extDpdCaptureCtrl.dpdCaptureTypeSel != ADI_ADRV9025_EXT_DPD_CAPTURE_EXTENDED_PEAK_SEARCH)
    {
    /* Read back Tx capture data */
    adrv9025_ExtDpdTxCaptureDataGet(device,
                                    &extDpdCaptureData->txCaptureData.extDpdCaptureDataI[0],
                                    &extDpdCaptureData->txCaptureData.extDpdCaptureDataQ[0],
                                    extDpdCaptureData->txCaptureData.extDpdCaptureSampleArrSize);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Read back Tx alternate buffer capture data */
    adrv9025_ExtDpdTxAlternateCaptureDataGet(device,
                                             &extDpdCaptureData->txAlternateCaptureData.extDpdCaptureDataI[0],
                                             &extDpdCaptureData->txAlternateCaptureData.extDpdCaptureDataQ[0],
                                             extDpdCaptureData->txAlternateCaptureData.extDpdCaptureSampleArrSize);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Read back Orx capture data */
    adrv9025_ExtDpdOrxCaptureDataGet(device,
                                     &extDpdCaptureData->orxCaptureData.extDpdCaptureDataI[0],
                                     &extDpdCaptureData->orxCaptureData.extDpdCaptureDataQ[0],
                                     extDpdCaptureData->orxCaptureData.extDpdCaptureSampleArrSize);
    ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else
    {
        adrv9025_ExtDpdExtendedPeakSearchCaptureDataGet(device, extDpdCaptureData, numOfLutBanksToRead);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Issue a capture data read done command to the FW  */
    adrv9025_ExtDpdCaptureDataReadDoneSet(device);
    ADI_ERROR_RETURN(device->common.error.newAction);

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_ExtDpdActuatorEnableSet(adi_adrv9025_Device_t* device,
                                             uint32_t               txChannelMask,
                                             uint8_t                enable)
{
    static const uint8_t  EXT_DPD_ENABLE  = 0x80;
    static const uint8_t  EXT_DPD_DISABLE = 0x00;
    static const uint32_t ALL_TX_MASK     = (uint32_t)(ADI_ADRV9025_TX1 |
        ADI_ADRV9025_TX2 |
        ADI_ADRV9025_TX3 |
        ADI_ADRV9025_TX4);
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t extDpdEnable   = (enable > 0) ? EXT_DPD_ENABLE : EXT_DPD_DISABLE;
    uint8_t extData[2]     = {0};
    uint8_t cmdStatusByte  = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

#if ADI_ADRV9025_EXT_DPD_RANGE_CHECK > 0
    adrv9025_ExtDpdActuatorEnableSetRangeCheck(device,
                                               txChannelMask,
                                               enable);
    ADI_ERROR_RETURN(device->common.error.newAction);
#endif

    /* Mask upper bits of the Tx channel mask */
    txChannelMask &= ALL_TX_MASK;

    /* Construct the ARM command to enable/disable DPD actuator */
    extData[0] = (uint8_t)ADRV9025_CPU_OBJECTID_EXT_DPD_ENABLE_DISABLE;
    extData[1] = (uint8_t)(extDpdEnable | (uint8_t)txChannelMask);

    /* Command the ARM to enable/disable DPD actuator */
    adi_adrv9025_CpuCmdWrite(device,
                             ADI_ADRV9025_CPU_TYPE_C,
                             (uint8_t)ADRV9025_CPU_SET_OPCODE,
                             &extData[0],
                             sizeof(extData));
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Wait for command to finish executing */
    adi_adrv9025_CpuCmdStatusWait(device,
                                  ADI_ADRV9025_CPU_TYPE_C,
                                  (uint8_t)ADRV9025_CPU_SET_OPCODE,
                                  &cmdStatusByte,
                                  (uint32_t)ADI_ADRV9025_SETEXTDPDENABLE_TIMEOUT_US,
                                  (uint32_t)ADI_ADRV9025_SETEXTDPDENABLE_INTERVAL_US);

    /* If cmdStatusByte[3:1] is non-zero then flag an ARM error */
    if ((cmdStatusByte >> 1) > 0)
    {
        adrv9025_CpuCmdErrorHandler(device,
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

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_ExtDpdActuatorEnableGet(adi_adrv9025_Device_t*    device,
                                             adi_adrv9025_TxChannels_e txChannel,
                                             uint8_t*                  enable)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t extData[2]     = {ADRV9025_CPU_OBJECTID_EXT_DPD_ENABLE_DISABLE, (uint8_t)txChannel};
    uint8_t cmdStatusByte  = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Check that enable ptr is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        enable);

    /* Check that the requested Tx channel is valid */
    if ((txChannel != ADI_ADRV9025_TX1) &&
        (txChannel != ADI_ADRV9025_TX2) &&
        (txChannel != ADI_ADRV9025_TX3) &&
        (txChannel != ADI_ADRV9025_TX4))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         ADI_COMMON_ACT_NO_ACTION,
                         txChannel,
                         "Invalid Tx channel requested for retrieving the DPD actuator status");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Execute the actuator enable status get command */
    adi_adrv9025_CpuCmdWrite(device,
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
                                                   (uint32_t)ADI_ADRV9025_GETEXTDPDENABLE_TIMEOUT_US,
                                                   (uint32_t)ADI_ADRV9025_GETEXTDPDENABLE_INTERVAL_US);

    /* If cmdStatusByte[3:1] is non-zero then flag an ARM error */
    if ((cmdStatusByte >> 1) > 0)
    {
        adrv9025_CpuCmdErrorHandler(device,
                                    ADI_ADRV9025_CPU_TYPE_C,
                                    ADI_ADRV9025_SRC_CPUCMD,
                                    ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
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

    /* Read the external DPD enable status from the ARM mailbox */
    adi_adrv9025_CpuMemRead(device,
                            (uint32_t)ADRV9025_CPU_C_ADDR_MAILBOX_GET,
                            enable,
                            1,
                            0);
    ADI_ERROR_RETURN(device->common.error.newAction);

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_ExtDpdActuatorModelConfigSet(adi_adrv9025_Device_t*      device,
                                                  adi_adrv9025_TxChannels_e   txChannel,
                                                  adi_adrv9025_ExtDpdModel_t* extDpdModelConfig)
{
#define ARM_EXT_DPD_MODEL_CFG_LENGTH_BYTES 48
    static const uint16_t PARAM_OFFSET_BYTES_TX1 = 0;
    static const uint16_t PARAM_OFFSET_BYTES_TX2 = 48;
    static const uint16_t PARAM_OFFSET_BYTES_TX3 = 96;
    static const uint16_t PARAM_OFFSET_BYTES_TX4 = 144;

    uint8_t  armConfigData[ARM_EXT_DPD_MODEL_CFG_LENGTH_BYTES] = {0};
    uint16_t extDpdModelParamOffsetBytes                       = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Check that extDpdModelConfig pointer is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        extDpdModelConfig);

#if ADI_ADRV9025_EXT_DPD_RANGE_CHECK > 0
    adrv9025_ExtDpdActuatorModelConfigSetRangeCheck(device,
                                                    txChannel,
                                                    extDpdModelConfig);
    ADI_ERROR_RETURN(device->common.error.newAction);
#endif

    switch (txChannel)
    {
    case(ADI_ADRV9025_TX1):
    {
        extDpdModelParamOffsetBytes = PARAM_OFFSET_BYTES_TX1;
        break;
    }
    case(ADI_ADRV9025_TX2):
    {
        extDpdModelParamOffsetBytes = PARAM_OFFSET_BYTES_TX2;
        break;
    }
    case(ADI_ADRV9025_TX3):
    {
        extDpdModelParamOffsetBytes = PARAM_OFFSET_BYTES_TX3;
        break;
    }
    case(ADI_ADRV9025_TX4):
    {
        extDpdModelParamOffsetBytes = PARAM_OFFSET_BYTES_TX4;
        break;
    }
    default:
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txChannel,
                         "Invalid target Tx channel selected for DPD actuator model config set. Valid Tx channels are Tx1-Tx4");
        ADI_ERROR_RETURN(device->common.error.newAction);
        break;
    }
    }

    /* Build the DPD model config array for ARM */
    adrv9025_ExtDpdModelConfigParamArrayLoad(device,
                                             txChannel,
                                             extDpdModelConfig,
                                             &armConfigData[0],
                                             (uint8_t)ARM_EXT_DPD_MODEL_CFG_LENGTH_BYTES);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Pass the DPD model config to ARM */
    adi_adrv9025_CpuConfigWrite(device,
                                ADI_ADRV9025_CPU_TYPE_C,
                                ADRV9025_CPU_OBJECTID_EXT_DPD_MODEL,
                                extDpdModelParamOffsetBytes,
                                &armConfigData[0],
                                ARM_EXT_DPD_MODEL_CFG_LENGTH_BYTES);
    ADI_ERROR_RETURN(device->common.error.newAction);

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_ExtDpdActuatorModelConfigGet(adi_adrv9025_Device_t*      device,
                                                  adi_adrv9025_TxChannels_e   txChannel,
                                                  adi_adrv9025_ExtDpdModel_t* extDpdModelConfig)
{
#define ARM_EXT_DPD_MODEL_CFG_GET_LENGTH_BYTES 48
    static const uint16_t PARAM_OFFSET_BYTES_TX1 = 0;
    static const uint16_t PARAM_OFFSET_BYTES_TX2 = 48;
    static const uint16_t PARAM_OFFSET_BYTES_TX3 = 96;
    static const uint16_t PARAM_OFFSET_BYTES_TX4 = 144;

    uint8_t  armConfigData[ARM_EXT_DPD_MODEL_CFG_GET_LENGTH_BYTES] = {0};
    uint16_t extDpdModelParamOffsetBytes                           = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Check that extDpdModelConfig pointer is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        extDpdModelConfig);

    switch (txChannel)
    {
    case(ADI_ADRV9025_TX1):
    {
        extDpdModelParamOffsetBytes = PARAM_OFFSET_BYTES_TX1;
        break;
    }
    case(ADI_ADRV9025_TX2):
    {
        extDpdModelParamOffsetBytes = PARAM_OFFSET_BYTES_TX2;
        break;
    }
    case(ADI_ADRV9025_TX3):
    {
        extDpdModelParamOffsetBytes = PARAM_OFFSET_BYTES_TX3;
        break;
    }
    case(ADI_ADRV9025_TX4):
    {
        extDpdModelParamOffsetBytes = PARAM_OFFSET_BYTES_TX4;
        break;
    }
    default:
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txChannel,
                         "Invalid target Tx channel selected for DPD actuator model config get. Valid Tx channels are Tx1-Tx4");
        ADI_ERROR_RETURN(device->common.error.newAction);
        break;
    }
    }

    /* Read ext DPD capture config from ARM mailbox */
    adi_adrv9025_CpuConfigRead(device,
                               ADI_ADRV9025_CPU_TYPE_C,
                               ADRV9025_CPU_OBJECTID_EXT_DPD_MODEL,
                               extDpdModelParamOffsetBytes,
                               &armConfigData[0],
                               sizeof(armConfigData));
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Parse the config array received from ARM */
    adrv9025_ExtDpdModelConfigParamArrayParse(device,
                                              extDpdModelConfig,
                                              &armConfigData[0],
                                              (uint8_t)ARM_EXT_DPD_MODEL_CFG_GET_LENGTH_BYTES);
    ADI_ERROR_RETURN(device->common.error.newAction);

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_ExtDpdActuatorModelSelGpiosSet(adi_adrv9025_Device_t*    device,
                                                    adi_adrv9025_GpioPinSel_e extDpdModelSelPin0,
                                                    adi_adrv9025_GpioPinSel_e extDpdModelSelPin1)
{
    static const uint8_t ARM_GPIO_PIN_POLARITY_NORMAL = 0;
    static const uint8_t ARM_GPIO_PIN_ENABLE          = 1;
    static const uint8_t ARM_GPIO_PIN_DISABLE         = 0;

    adi_adrv9025_ArmGpioPinCfg_t armGpioCfgPin0 = {ADI_ADRV9025_GPIO_INVALID, ARM_GPIO_PIN_POLARITY_NORMAL, ARM_GPIO_PIN_DISABLE};
    adi_adrv9025_ArmGpioPinCfg_t armGpioCfgPin1 = {ADI_ADRV9025_GPIO_INVALID, ARM_GPIO_PIN_POLARITY_NORMAL, ARM_GPIO_PIN_DISABLE};

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Setup DPD Model select pin0 */
    armGpioCfgPin0.gpioPinSel = extDpdModelSelPin0;
    armGpioCfgPin0.enable     = (extDpdModelSelPin0 == ADI_ADRV9025_GPIO_INVALID) ? ARM_GPIO_PIN_DISABLE : ARM_GPIO_PIN_ENABLE;
    armGpioCfgPin0.polarity   = ARM_GPIO_PIN_POLARITY_NORMAL;
    adrv9025_ArmGpioPinSet(device,
                           (uint8_t)ADRV9025_CPU_EXT_DPD_MODEL_SEL0_SIGNALID,
                           &armGpioCfgPin0);
    ADI_ERROR_RETURN(device->common.error.newAction);

    if ((extDpdModelSelPin0 >= ADI_ADRV9025_GPIO_00) && (extDpdModelSelPin0 <= ADI_ADRV9025_GPIO_INVALID))
    {
        /* Setup DPD Model select pin1 */
        armGpioCfgPin1.gpioPinSel = extDpdModelSelPin1;
        armGpioCfgPin1.enable     = (extDpdModelSelPin1 == ADI_ADRV9025_GPIO_INVALID) ? ARM_GPIO_PIN_DISABLE : ARM_GPIO_PIN_ENABLE;
        armGpioCfgPin1.polarity   = ARM_GPIO_PIN_POLARITY_NORMAL;
        adrv9025_ArmGpioPinSet(device,
                               (uint8_t)ADRV9025_CPU_EXT_DPD_MODEL_SEL1_SIGNALID,
                               &armGpioCfgPin1);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    else
    {
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM, ADI_COMMON_ACT_ERR_CHECK_PARAM, extDpdModelSelPin0,
            "Invalid GPIO PIN selected for extDpdModelSelPin0");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_ExtDpdActuatorModelSelGpiosGet(adi_adrv9025_Device_t*     device,
                                                    adi_adrv9025_GpioPinSel_e* extDpdModelSelPin0,
                                                    adi_adrv9025_GpioPinSel_e* extDpdModelSelPin1)
{
    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Check that extDpdModelSelPin0 pointer is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        extDpdModelSelPin0);

    /* Check that extDpdModelSelPin1 pointer is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        extDpdModelSelPin1);

    /* Retrieve DPD Model select pin0 GPIO assignment */
    adrv9025_ArmGpioPinGet(device,
                           (uint8_t)ADRV9025_CPU_EXT_DPD_MODEL_SEL0_SIGNALID,
                           extDpdModelSelPin0);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Retrieve DPD Model select pin1 GPIO assignment */
    adrv9025_ArmGpioPinGet(device,
                           (uint8_t)ADRV9025_CPU_EXT_DPD_MODEL_SEL1_SIGNALID,
                           extDpdModelSelPin1);
    ADI_ERROR_RETURN(device->common.error.newAction);

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_ExtDpdActuatorModelSelectSet(adi_adrv9025_Device_t*        device,
                                                  uint32_t                      txChannelMask,
                                                  adi_adrv9025_ExtDpdModelSel_e extDpdModelSel)
{
    static const uint8_t  ARM_ERR_MASK = 0x0E;
    static const uint32_t ALL_TX_MASK  = (uint32_t)(ADI_ADRV9025_TX1 |
        ADI_ADRV9025_TX2 |
        ADI_ADRV9025_TX3 |
        ADI_ADRV9025_TX4);

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t extData[2]     = {0};
    uint8_t cmdStatusByte  = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Check that the DPD model is valid */
    if ((extDpdModelSel != ADI_ADRV9025_EXT_DPD_MODEL0) &&
        (extDpdModelSel != ADI_ADRV9025_EXT_DPD_MODEL1) &&
        (extDpdModelSel != ADI_ADRV9025_EXT_DPD_MODEL2) &&
        (extDpdModelSel != ADI_ADRV9025_EXT_DPD_MODEL3))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         extDpdModelSel,
                         "Invalid target DPD model selected for DPD model select configuration. Valid DPD models are Model0-Model3");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Mask the upper bits of the tx Channel select mask */
    txChannelMask &= ALL_TX_MASK;

    /* Construct the ARM command to activate the model for DPD actuator */
    extData[0] = (uint8_t)ADRV9025_CPU_OBJECTID_EXT_DPD_MODEL_SELECT;
    extData[1] = (((uint8_t)extDpdModelSel << 4) | (uint8_t)txChannelMask);

    /* Command the ARM to enable/disable DPD actuatorto activate the model for DPD actuator */
    adi_adrv9025_CpuCmdWrite(device,
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
                                                   (uint32_t)ADI_ADRV9025_SETEXTDMODELSELECT_TIMEOUT_US,
                                                   (uint32_t)ADI_ADRV9025_SETEXTDMODELSELECT_INTERVAL_US);

    /* If cmdStatusByte[3:1] is non-zero then flag an ARM error */
    if ((cmdStatusByte & ARM_ERR_MASK) > 0)
    {
        adrv9025_CpuCmdErrorHandler(device,
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

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_ExtDpdActuatorModelSelectGet(adi_adrv9025_Device_t*         device,
                                                  adi_adrv9025_TxChannels_e      txChannel,
                                                  adi_adrv9025_ExtDpdModelSel_e* extDpdModelSel)
{
    static const uint8_t DPD_MODEL_SEL_MASK = 0x03;

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t extData[2]     = {0};
    uint8_t armData        = 0;
    uint8_t cmdStatusByte  = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Check that extDpdModelSel pointer is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        extDpdModelSel);

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
                         "Invalid Tx channel encountered while attempting to read DPD model select. Valid Tx channels are Tx1-Tx4");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Send a cmd to ARM to retrieve DPD actuator model select for the requested Tx channel */
    extData[0] = ADRV9025_CPU_OBJECTID_EXT_DPD_MODEL_SELECT;
    extData[1] = (uint8_t)txChannel;
    adi_adrv9025_CpuCmdWrite(device,
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
                                                   (uint32_t)ADI_ADRV9025_GETEXTDMODELSELECT_TIMEOUT_US,
                                                   (uint32_t)ADI_ADRV9025_GETEXTDMODELSELECT_INTERVAL_US);

    /* If cmdStatusByte is non-zero then flag an ARM error */
    if ((cmdStatusByte >> 1) > 0)
    {
        adrv9025_CpuCmdErrorHandler(device,
                                    ADI_ADRV9025_CPU_TYPE_C,
                                    ADI_ADRV9025_SRC_CPUCMD,
                                    ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
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

    /* Read DPD actuator model select from ARM mailbox */
    adi_adrv9025_CpuMemRead(device,
                            (uint32_t)ADRV9025_CPU_C_ADDR_MAILBOX_GET,
                            &armData,
                            sizeof(armData),
                            0);
    ADI_ERROR_RETURN(device->common.error.newAction);

    *extDpdModelSel = (adi_adrv9025_ExtDpdModelSel_e)(armData & DPD_MODEL_SEL_MASK);

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_ExtDpdLutSwapModeEnableSet(adi_adrv9025_Device_t* device,
                                                uint32_t               txChannelMask,
                                                uint8_t                lutSwapModeEn)
{
    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* check for invalid channel mask */
    if (txChannelMask > (uint32_t)ADI_ADRV9025_TXALL)
    {
        device->common.error.newAction = ADI_COMMON_ACT_ERR_CHECK_PARAM;
        return (device->common.error.newAction);
    }

    /* Configure the LUT swap mode enable setting */
    adrv9025_ExtDpdLutSwapModeEnableSet(device,
                                        lutSwapModeEn);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Store the LUT swap mode enable setting in the device structure for use in LUT write */
    device->devStateInfo.extDpdLutSwapModeEn = lutSwapModeEn;

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_ExtDpdLutSwapModeEnableGet(adi_adrv9025_Device_t*    device,
                                                adi_adrv9025_TxChannels_e txChannel,
                                                uint8_t*                  lutSwapModeEn)
{
    /* This constant can be used to control how the swap mode enable setting is resolved.
       Setting this constant to 1 queries the swap mode enable setting from the device via a FW command 
       and setting this constant to 0 queries the swap mode enable setting from the device data structure */
    static const uint8_t READ_SWAP_MODE_EN_FROM_DEVICE = 0x00;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Check that lutSwapModeEn is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        lutSwapModeEn);

    /* check for invalid channel mask */
    if (txChannel > (uint32_t)ADI_ADRV9025_TXALL)
    {
        device->common.error.newAction = ADI_COMMON_ACT_ERR_CHECK_PARAM;
        return (device->common.error.newAction);
    }

    if (READ_SWAP_MODE_EN_FROM_DEVICE != 0)
    {
        adrv9025_ExtDpdLutSwapModeEnableGet(device,
                                            lutSwapModeEn);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else
    {
        *lutSwapModeEn = device->devStateInfo.extDpdLutSwapModeEn;
    }

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_ExtDpdActuatorClkSet(adi_adrv9025_Device_t *device, adi_adrv9025_TxChannels_e txChannel, uint8_t enable)
{
    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API);

    static const uint8_t DPD_ACTUATOR_CLK_ENABLE_STREAMID = 0x02;
    static const uint8_t DPD_ACTUATOR_CLK_DISABLE_STREAMID = 0x01;
    static const uint8_t ACTUATOR_CLK_ENABLE_POLL_TIMEOUT = 40;
    static const uint32_t RX_CH1_STREAM_PROC_DEBUG_REG6 = 0x15B7;
    static const uint32_t RX_CH2_STREAM_PROC_DEBUG_REG6 = 0x17B7;
    static const uint8_t CORE_STREAM_DISPATCH_ID = 0x9C;

    adrv9025_BfTxChanAddr_e txBitfieldBaseAddr = ADRV9025_BF_TX_CH0;
    uint8_t actuatorClkStatus = 0;
    uint8_t actuatorPollCount = 0;
    uint8_t clkCommand = (enable == 0) ? DPD_ACTUATOR_CLK_DISABLE_STREAMID : DPD_ACTUATOR_CLK_ENABLE_STREAMID;

    if ((enable != ADI_TRUE) && (enable != ADI_FALSE))
    {
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM, ADI_COMMON_ACT_ERR_CHECK_PARAM, txChannel,
            "Invalid enable selection.It should be either 0 or 1.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((txChannel != ADI_ADRV9025_TX1) && (txChannel != ADI_ADRV9025_TX2) && (txChannel != ADI_ADRV9025_TX3) && (txChannel != ADI_ADRV9025_TX4))
    {
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM, ADI_COMMON_ACT_ERR_CHECK_PARAM, txChannel,
            "Invalid Tx channel encountered while attempting to program/read back DPD LUTs. Valid Tx channels are Tx1-Tx4");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* retrieve tx slice base address */
    txBitfieldBaseAddr = (adrv9025_BfTxChanAddr_e)adrv9025_TxAddrDecode(device, (uint32_t)txChannel);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Set the channel to enable/disable clk */
    adi_adrv9025_SpiByteWrite(device, RX_CH1_STREAM_PROC_DEBUG_REG6, (uint8_t)txChannel);
    ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM, device->common.error.newAction, NULL, "Error while writing channel mask to RX_CH1_STREAM_PROC_DEBUG_REG6");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* write clkCommand stream ID */
    adi_adrv9025_SpiByteWrite(device, RX_CH2_STREAM_PROC_DEBUG_REG6, clkCommand);
    ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM, device->common.error.newAction, NULL, "Error while writing stream ID to RX_CH2_STREAM_PROC_DEBUG_REG6");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Command ARM to execute the stream */
    adrv9025_StreamTrigger(device, CORE_STREAM_DISPATCH_ID);
    ADI_ERROR_RETURN(device->common.error.newAction);

    for (actuatorPollCount = 0; actuatorPollCount < ACTUATOR_CLK_ENABLE_POLL_TIMEOUT; actuatorPollCount++)
    {
        /* Poll actuator clk to determine stream has completed */
        adrv9025_TxDpdActClkEnableBfGet(device, txBitfieldBaseAddr, &actuatorClkStatus);
        ADI_ERROR_RETURN(device->common.error.newAction);

        if (enable == actuatorClkStatus)
        {
            break;
        }

        /* delay one millisecond */
        adi_common_hal_Wait_ms(&device->common, 1);
        ADI_ERROR_REPORT(&device->common, ADI_ADRV9025_SRC_TX, device->common.error.newAction, ADI_COMMON_ACT_ERR_CHECK_TIMER, device,
            "Timer not working in ExtDpdActuatorLutRead()");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (actuatorPollCount >= ACTUATOR_CLK_ENABLE_POLL_TIMEOUT)
    {
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM, ADI_COMMON_ACT_ERR_RESET_FEATURE, NULL, "Error: Actuator clock failed to set");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return device->common.error.newAction;
}

int32_t adi_adrv9025_ExtDpdActuatorLutWrite(adi_adrv9025_Device_t*       device,
                                            adi_adrv9025_TxChannels_e    txChannel,
                                            adi_adrv9025_ExtDpdLut_e     extDpdLutSel,
                                            adi_adrv9025_ExtDpdLutBank_e extDpdLutBankSel,
                                            uint8_t                      lutData[],
                                            uint16_t                     lutDataArrSizeBytes)
{
    static const uint8_t DPD_ACTUATOR_ENABLE               = 0x01;
    static const uint8_t LUT_POWER_ON                      = 0;
    static const uint8_t LUT_SWAP_MODE_DISABLE             = 0;
    
    uint32_t                lutBaseAddr             = 0;
    uint8_t                 actuatorEnableRegVal    = 0;
    uint8_t                 actuatorPassthruRegVal  = 0;
    uint8_t                 actuatorLutRamSelRegVal = 0;
    uint8_t                 currentLutPowerOnState  = 0;
    uint8_t                 currentLutSwapModeEn    = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Check that lutData is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        lutData);

#if ADI_ADRV9025_EXT_DPD_RANGE_CHECK > 0
    adrv9025_ExtDpdActuatorLutRangeCheck(device,
                                         txChannel,
                                         extDpdLutSel,
                                         extDpdLutBankSel,
                                         lutData);
    ADI_ERROR_RETURN(device->common.error.newAction);
#endif

    /* Get the current power on state of the requested LUT */
    adrv9025_ExtDpdLutPowerOnGet(device,
                                 txChannel,
                                 extDpdLutSel,
                                 &currentLutPowerOnState);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Power up the requested LUT */
    adrv9025_ExtDpdLutPowerOnSet(device,
                                 txChannel,
                                 extDpdLutSel,
                                 LUT_POWER_ON);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Get the current DPD actuator enable and passthru values */
    adrv9025_ExtDpdActutatorEnPassthruGet(device,
                                          txChannel,
                                          &actuatorEnableRegVal,
                                          &actuatorPassthruRegVal);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Enable DPD to program LUTs */
    adrv9025_ExtDpdActutatorEnPassthruSet(device,
                                          txChannel,
                                          DPD_ACTUATOR_ENABLE,
                                          actuatorPassthruRegVal);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Enable actuator clock */
    adi_adrv9025_ExtDpdActuatorClkSet(device, txChannel, ADI_TRUE);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Get the current value of LUT RAM select */
    adrv9025_ExtDpdActuatorRamSelGet(device,
                                     txChannel,
                                     &actuatorLutRamSelRegVal);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Set the RAM select corresponding to the requested LUT */
    adrv9025_ExtDpdActuatorRamSelSet(device,
                                     txChannel,
                                     (uint8_t)extDpdLutSel);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Resolve the LUT start address to program */
    adrv9025_ExtDpdActuatorLutAddressResolve(device,
                                             txChannel,
                                             extDpdLutSel,
                                             extDpdLutBankSel,
                                             &lutBaseAddr);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Program the LUT via SPI-AHB DMA */
    adrv9025_CpuDmaMemWrite(device,
                            lutBaseAddr,
                            &lutData[0],
                            lutDataArrSizeBytes,
                            ADI_ADRV9025_CPU_MEM_AUTO_INCR);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Set the DPD actuator enable and passthru to default values */
    adrv9025_ExtDpdActutatorEnPassthruSet(device,
                                          txChannel,
                                          actuatorEnableRegVal,
                                          actuatorPassthruRegVal);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Restore the Power on state of the LUT */
    /* TODO : Uncomment the 2 lines below when firmware power on for swap LUT is fixed */
    /*recoveryAction = adrv9025_ExtDpdLutPowerOnSet(device, txChannel, extDpdLutSel, currentLutPowerOnState);
    ADI_ERROR_RETURN(device->common.error.newAction);*/

    /* Get the current state of LUT swap mode enable */
    adrv9025_ExtDpdLutSwapModeEnableGet(device,
                                        &currentLutSwapModeEn);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Restore LUT RAM select if LUT swap mode is not enabled */
    if (currentLutSwapModeEn == LUT_SWAP_MODE_DISABLE)
    {
        adrv9025_ExtDpdActuatorRamSelSet(device,
                                         txChannel,
                                         actuatorLutRamSelRegVal);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Disable actuator clock */
    adi_adrv9025_ExtDpdActuatorClkSet(device, txChannel, ADI_FALSE);
    ADI_ERROR_RETURN(device->common.error.newAction);

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_ExtDpdActuatorLutRead(adi_adrv9025_Device_t*       device,
                                           adi_adrv9025_TxChannels_e    txChannel,
                                           adi_adrv9025_ExtDpdLut_e     extDpdLutSel,
                                           adi_adrv9025_ExtDpdLutBank_e extDpdLutBankSel,
                                           uint8_t                      lutData[],
                                           uint16_t                     lutDataArrSizeBytes)
{
    static const uint8_t DPD_ACTUATOR_ENABLE               = 0x01;
    static const uint8_t LUT_POWER_ON                      = 0x00;
    
    uint32_t                lutBaseAddr             = 0;
    uint8_t                 actuatorEnableRegVal    = 0;
    uint8_t                 actuatorPassthruRegVal  = 0;
    uint8_t                 actuatorLutRamSelRegVal = 0;
    uint8_t                 currentLutPowerOnState  = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Check that lutData is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        lutData);

#if ADI_ADRV9025_EXT_DPD_RANGE_CHECK > 0
    adrv9025_ExtDpdActuatorLutRangeCheck(device,
                                         txChannel,
                                         extDpdLutSel,
                                         extDpdLutBankSel,
                                         lutData);
    ADI_ERROR_RETURN(device->common.error.newAction);
#endif

    /* Get the current power on state of the requested LUT */
    adrv9025_ExtDpdLutPowerOnGet(device,
                                 txChannel,
                                 extDpdLutSel,
                                 &currentLutPowerOnState);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Power up the requested LUT */
    adrv9025_ExtDpdLutPowerOnSet(device,
                                 txChannel,
                                 extDpdLutSel,
                                 LUT_POWER_ON);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Get the current DPD actuator enable and passthru values */
    adrv9025_ExtDpdActutatorEnPassthruGet(device,
                                          txChannel,
                                          &actuatorEnableRegVal,
                                          &actuatorPassthruRegVal);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Enable DPD to read back LUTs */
    adrv9025_ExtDpdActutatorEnPassthruSet(device,
                                          txChannel,
                                          DPD_ACTUATOR_ENABLE,
                                          actuatorPassthruRegVal);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Enable actuator clock */
    adi_adrv9025_ExtDpdActuatorClkSet(device, txChannel, ADI_TRUE);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Get the current value of LUT RAM select */
    adrv9025_ExtDpdActuatorRamSelGet(device,
                                     txChannel,
                                     &actuatorLutRamSelRegVal);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Set the RAM select corresponding to the requested LUT */
    adrv9025_ExtDpdActuatorRamSelSet(device,
                                     txChannel,
                                     (uint8_t)extDpdLutSel);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Resolve the LUT start address to program */
    adrv9025_ExtDpdActuatorLutAddressResolve(device,
                                             txChannel,
                                             extDpdLutSel,
                                             extDpdLutBankSel,
                                             &lutBaseAddr);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Read the LUT via SPI-AHB DMA */
    adrv9025_CpuDmaMemRead(device,
                           lutBaseAddr,
                           &lutData[0],
                           lutDataArrSizeBytes,
                           1);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Restore the Power on state of the LUT */
    adrv9025_ExtDpdLutPowerOnSet(device,
                                 txChannel,
                                 extDpdLutSel,
                                 currentLutPowerOnState);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Set the DPD actuator enable and passthru to default values */
    adrv9025_ExtDpdActutatorEnPassthruSet(device,
                                          txChannel,
                                          actuatorEnableRegVal,
                                          actuatorPassthruRegVal);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Restore the LUT RAM select */
    adrv9025_ExtDpdActuatorRamSelSet(device,
                                     txChannel,
                                     actuatorLutRamSelRegVal);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Disable actuator clock */
    adi_adrv9025_ExtDpdActuatorClkSet(device, txChannel, ADI_FALSE);
    ADI_ERROR_RETURN(device->common.error.newAction);

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_ExtDpdActuatorLutClear(adi_adrv9025_Device_t* device)
{
    static const uint8_t EXT_DPD_ACTUATOR_LUT_CLEAR = 0x13;
    static const uint8_t ARM_ERR_CODE               = 0x0E;

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t extData[4]     = {ADRV9025_CPU_OBJECTID_TRACKING_CAL_CTRL, ADRV9025_CPU_OBJECTID_DPD_INIT, 1u, EXT_DPD_ACTUATOR_LUT_CLEAR};
    uint8_t cmdStatusByte  = 0u;

    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

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
                                                   (uint8_t)ADRV9025_CPU_SET_OPCODE,
                                                   &cmdStatusByte,
                                                   ADI_ADRV9025_SETEXTDLUTCLEAR_TIMEOUT_US,
                                                   (uint32_t)ADI_ADRV9025_SETEXTDLUTCLEAR_INTERVAL_US);

    /* If cmdStatusByte is non-zero then flag an ARM error */
    if ((cmdStatusByte & ARM_ERR_CODE) > 0)
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

    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     cmdStatusByte,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    return recoveryAction;
}

int32_t adi_adrv9025_ExtDpdActuatorSwapModeLutWrite(adi_adrv9025_Device_t*       device,
                                                    adi_adrv9025_TxChannels_e    txChannel,
                                                    adi_adrv9025_ExtDpdLutBank_e extDpdLutBankSel,
                                                    adi_adrv9025_SwapModeLut_t   swapModeLutData[])
{
    static const uint8_t                      SWAP_MODE_LUT_RAM_SEL             = 31;
    static const adi_adrv9025_ExtDpdLutBank_e LUT_SWAP_BANK_8BIT_COMPANDER_MODE = ADI_ADRV9025_EXT_DPD_LUT_BANK3;
    static const adi_adrv9025_ExtDpdLutBank_e LUT_SWAP_BANK_9BIT_COMPANDER_MODE = ADI_ADRV9025_EXT_DPD_LUT_BANK2;

    int8_t                       lutIndex          = 0;
    adi_adrv9025_ExtDpdLutBank_e lutSwapBank       = ADI_ADRV9025_EXT_DPD_LUT_BANK3;
    adi_adrv9025_ExtDpdModel_t   extDpdModelConfig = {
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
                           ADI_COMMON_LOG_API);

    /* Check that swapModeLutData is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        swapModeLutData);

#if ADI_ADRV9025_EXT_DPD_RANGE_CHECK > 0
    adrv9025_ExtDpdActuatorSwapModeLutWriteRangeCheck(device,
                                                      txChannel,
                                                      extDpdLutBankSel,
                                                      swapModeLutData);
    ADI_ERROR_RETURN(device->common.error.newAction);
#endif

    /* Read the active actuator model config for the target Tx channel */
    adi_adrv9025_ExtDpdActuatorModelConfigGet(device,
                                              txChannel,
                                              &extDpdModelConfig);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Resolve the swap LUT bank */
    if (extDpdModelConfig.extDpdActuatorCompanderSize == ADI_ADRV9025_EXT_DPD_ACT_COMPANDER_8_BITS)
    {
        lutSwapBank = LUT_SWAP_BANK_8BIT_COMPANDER_MODE;
    }
    else if (extDpdModelConfig.extDpdActuatorCompanderSize == ADI_ADRV9025_EXT_DPD_ACT_COMPANDER_9_BITS)
    {
        lutSwapBank = LUT_SWAP_BANK_9BIT_COMPANDER_MODE;
    }

    /* Program the active LUT banks */
    for (lutIndex = (int8_t)(ADI_ADRV9025_EXT_DPD_NUM_LUTS - 2); lutIndex >= 0; lutIndex--)
    {
        if ((swapModeLutData[lutIndex].extDpdSwapModeLutDataPtr != NULL) &&
            (swapModeLutData[lutIndex].extDpdSwapModeLutDataSizeBytes > 0))
        {
            adi_adrv9025_ExtDpdActuatorLutWrite(device,
                                                txChannel,
                                                (adi_adrv9025_ExtDpdLut_e)lutIndex,
                                                extDpdLutBankSel,
                                                swapModeLutData[lutIndex].extDpdSwapModeLutDataPtr,
                                                swapModeLutData[lutIndex].extDpdSwapModeLutDataSizeBytes);
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    /* Park the RAM select to swap LUT (ADI_ADRV9025_EXT_DPD_LUT31) */
    adrv9025_ExtDpdActuatorRamSelSet(device,
                                     txChannel,
                                     SWAP_MODE_LUT_RAM_SEL);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Program the swap LUT banks */
    for (lutIndex = (int8_t)(ADI_ADRV9025_EXT_DPD_NUM_LUTS - 2); lutIndex >= 0; lutIndex--)
    {
        if ((swapModeLutData[lutIndex].extDpdSwapModeLutDataPtr != NULL) &&
            (swapModeLutData[lutIndex].extDpdSwapModeLutDataSizeBytes > 0))
        {
            adi_adrv9025_ExtDpdActuatorLutWrite(device,
                                                txChannel,
                                                (adi_adrv9025_ExtDpdLut_e)(lutIndex + 1),
                                                lutSwapBank,
                                                swapModeLutData[lutIndex].extDpdSwapModeLutDataPtr,
                                                swapModeLutData[lutIndex].extDpdSwapModeLutDataSizeBytes);
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    /* Park the RAM select to swap LUT (ADI_ADRV9025_EXT_DPD_LUT31) */
    adrv9025_ExtDpdActuatorRamSelSet(device,
                                     txChannel,
                                     SWAP_MODE_LUT_RAM_SEL);
    ADI_ERROR_RETURN(device->common.error.newAction);

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_ExtDpdActuatorGainMonitorConfigSet(adi_adrv9025_Device_t*                    device,
                                                        adi_adrv9025_ExtDpdActGainMonitorConfig_t extDpdActGainMonitorConfig[],
                                                        uint8_t                                   numGainMonitorConfigs)
{
    int32_t  recoveryAction      = ADI_COMMON_ACT_NO_ACTION;
    uint32_t gainMonitorCfgIndex = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Check that extDpdActGainMonitorConfig is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        extDpdActGainMonitorConfig);

#if ADI_ADRV9025_EXT_DPD_RANGE_CHECK > 0
    adrv9025_ExtDpdActuatorGainMonitorConfigSetRangeCheck(device,
                                                          extDpdActGainMonitorConfig,
                                                          numGainMonitorConfigs);
    ADI_ERROR_RETURN(device->common.error.newAction);
#endif

    for (gainMonitorCfgIndex = 0; gainMonitorCfgIndex < numGainMonitorConfigs; gainMonitorCfgIndex++)
    {
        recoveryAction = adrv9025_ExtDpdActuatorGainMonitorConfigSet(device,
                                                                     &extDpdActGainMonitorConfig[gainMonitorCfgIndex]);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         extDpdActGainMonitorConfig[gainMonitorCfgIndex],
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_ExtDpdActuatorGainMonitorConfigGet(adi_adrv9025_Device_t*                     device,
                                                        adi_adrv9025_TxChannels_e                  txChannel,
                                                        adi_adrv9025_ExtDpdActGainMonitorConfig_t* extDpdActGainMonitorConfig)
{
    static const uint16_t PARAM_OFFSET_BYTES_TX1 = 0;
    static const uint16_t PARAM_OFFSET_BYTES_TX2 = 12;
    static const uint16_t PARAM_OFFSET_BYTES_TX3 = 24;
    static const uint16_t PARAM_OFFSET_BYTES_TX4 = 36;

    uint16_t extDpdActGainMonitorConfigOffsetBytes = 0;
    uint8_t  armConfigData[12]                     = {0};

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    /* Check that extDpdActGainMonitorConfig is not null */
    ADI_NULL_PTR_RETURN(&device->common,
                        extDpdActGainMonitorConfig);

    switch (txChannel)
    {
    case(ADI_ADRV9025_TX1):
    {
        extDpdActGainMonitorConfigOffsetBytes = PARAM_OFFSET_BYTES_TX1;
        break;
    }
    case(ADI_ADRV9025_TX2):
    {
        extDpdActGainMonitorConfigOffsetBytes = PARAM_OFFSET_BYTES_TX2;
        break;
    }
    case(ADI_ADRV9025_TX3):
    {
        extDpdActGainMonitorConfigOffsetBytes = PARAM_OFFSET_BYTES_TX3;
        break;
    }
    case(ADI_ADRV9025_TX4):
    {
        extDpdActGainMonitorConfigOffsetBytes = PARAM_OFFSET_BYTES_TX4;
        break;
    }
    default:
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txChannel,
                         "Invalid target Tx channel selected for DPD actuator gain monitor config get. Valid Tx channels are Tx1-Tx4");
        ADI_ERROR_RETURN(device->common.error.newAction);
        break;
    }
    }

    /* Read the DPD actuator gain monitor config from CPU */
    adi_adrv9025_CpuConfigRead(device,
                               ADI_ADRV9025_CPU_TYPE_C,
                               ADRV9025_CPU_OBJECTID_EXT_DPD_GAIN_MONITOR,
                               extDpdActGainMonitorConfigOffsetBytes,
                               &armConfigData[0],
                               sizeof(armConfigData));
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Deserialize the CPU config data into the DPD gain monitor config array */
    extDpdActGainMonitorConfig->txChannelMask                                       = (uint32_t)txChannel;
    extDpdActGainMonitorConfig->extDpdGainMonitorThresh.extDpdGainMonitorQualThresh = (uint16_t)(((uint16_t)armConfigData[1] << 8) | ((uint16_t)armConfigData[0]
    ));
    extDpdActGainMonitorConfig->extDpdGainMonitorThresh.extDpdGainMonitorLowerThresh       = armConfigData[2];
    extDpdActGainMonitorConfig->extDpdGainMonitorThresh.extDpdGainMonitorUpperThresh       = armConfigData[3];
    extDpdActGainMonitorConfig->extDpdGainMonitorCtrl.extDpdGainMonitorEn                  = armConfigData[8];
    extDpdActGainMonitorConfig->extDpdGainMonitorCtrl.extDpdGainMonitorLowerThreshModelSel = (adi_adrv9025_ExtDpdModelSel_e)armConfigData[9];
    extDpdActGainMonitorConfig->extDpdGainMonitorCtrl.extDpdGainMonitorUpperThreshModelSel = (adi_adrv9025_ExtDpdModelSel_e)armConfigData[10];
    extDpdActGainMonitorConfig->extDpdGainMonitorCtrl.extDpdGainMonitorIIRDecay            = armConfigData[11];

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_ExtDpdHardClipperConfigSet(adi_adrv9025_Device_t*                 device,
                                                adi_adrv9025_ExtDpdHardClipperConfig_t extDpdHardClipperConfig[],
                                                uint8_t                                extDpdHardClipperCfgArraySize)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t configIndex    = 0u;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common,
                        extDpdHardClipperConfig);

#if ADI_ADRV9025_CFR_RANGE_CHECK > 0
    adrv9025_ExtDpdHardClipperConfigSetRangeCheck(device,
                                                  extDpdHardClipperConfig,
                                                  extDpdHardClipperCfgArraySize);
    ADI_ERROR_RETURN(device->common.error.newAction);
#endif

    for (configIndex = 0u; configIndex < extDpdHardClipperCfgArraySize; configIndex++)
    {
        recoveryAction = adrv9025_ExtDpdHardClipperConfigSet(device,
                                                             &extDpdHardClipperConfig[configIndex]);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         extDpdHardClipperConfig[configIndex],
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_ExtDpdHardClipperConfigGet(adi_adrv9025_Device_t*                  device,
                                                adi_adrv9025_TxChannels_e               txChannel,
                                                adi_adrv9025_ExtDpdHardClipperConfig_t* extDpdHardClipperConfig)
{
    static const float   FIXED_POINT_SCALER = 32768; /* The post DPD hard clipper threshold is in 1.15 format */
    static const uint8_t ARM_ERR_CODE       = 0x0E;

    int32_t  recoveryAction      = ADI_COMMON_ACT_NO_ACTION;
    uint8_t  extData[4]          = {ADRV9025_CPU_OBJECTID_TRACKING_CAL_CTRL, ADRV9025_CPU_OBJECTID_DPD_INIT, (uint8_t)txChannel, 0u};
    uint8_t  cmdStatusByte       = 0u;
    uint8_t  armData[3]          = {0};
    uint16_t fixedPointThreshold = 0u;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_NULL_PTR_RETURN(&device->common, extDpdHardClipperConfig);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    if (txChannel != ADI_ADRV9025_TX1 &&
        txChannel != ADI_ADRV9025_TX2 &&
        txChannel != ADI_ADRV9025_TX3 &&
        txChannel != ADI_ADRV9025_TX4)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txChannel,
                         "Invalid Tx Channel requested.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

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
                                                   (uint8_t)ADRV9025_CPU_GET_OPCODE,
                                                   &cmdStatusByte,
                                                   ADI_ADRV9025_GETEXTDHARDCLIPPER_TIMEOUT_US,
                                                   (uint32_t)ADI_ADRV9025_GETEXTDHARDCLIPPER_INTERVAL_US);

    /* If cmdStatusByte is non-zero then flag an ARM error */
    if ((cmdStatusByte & ARM_ERR_CODE) > 0)
    {
        adrv9025_CpuCmdErrorHandler(device,
                                    ADI_ADRV9025_CPU_TYPE_C,
                                    ADI_ADRV9025_SRC_CPUCMD,
                                    ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
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

    /* Read the values from ARM mailbox */
    recoveryAction = adi_adrv9025_CpuMemRead(device,
                                             ADRV9025_CPU_C_ADDR_MAILBOX_SET,
                                             &armData[0],
                                             sizeof(armData),
                                             ADI_ADRV9025_CPU_MEM_AUTO_INCR);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     armData,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Parse the values */
    fixedPointThreshold = (uint16_t)(((uint16_t)armData[2] << 8) | (uint16_t)armData[1]);

    extDpdHardClipperConfig->txChannelMask              = (uint32_t)txChannel;
    extDpdHardClipperConfig->extDpdHardClipperEnable    = armData[0];
    extDpdHardClipperConfig->extDpdHardClipperThreshold = (float)fixedPointThreshold / FIXED_POINT_SCALER;

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_ExtDpdHardClipperCounterReset(adi_adrv9025_Device_t* device,
                                                   uint32_t               txChannelMask)
{
    static const uint8_t EXT_DPD_HARD_CLIPPER_COUNTER_RESET = 0x11;
    static const uint8_t ARM_ERR_CODE                       = 0x0E;

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t extData[4]     = {ADRV9025_CPU_OBJECTID_TRACKING_CAL_CTRL, ADRV9025_CPU_OBJECTID_DPD_INIT, 0u, EXT_DPD_HARD_CLIPPER_COUNTER_RESET};
    uint8_t txChannelId    = 0u;
    uint8_t txChannel      = 0u;
    uint8_t cmdStatusByte  = 0u;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    for (txChannelId = 0u; txChannelId < ADI_ADRV9025_MAX_TXCHANNELS; txChannelId++)
    {
        txChannel = 0x1 << txChannelId;
        if (txChannel & txChannelMask)
        {
            extData[2] = txChannel;

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
                                                           (uint8_t)ADRV9025_CPU_SET_OPCODE,
                                                           &cmdStatusByte,
                                                           ADI_ADRV9025_GETEXTDHARDCLIPPER_TIMEOUT_US,
                                                           (uint32_t)ADI_ADRV9025_GETEXTDHARDCLIPPER_INTERVAL_US);

            /* If cmdStatusByte is non-zero then flag an ARM error */
            if ((cmdStatusByte & ARM_ERR_CODE) > 0)
            {
                adrv9025_CpuCmdErrorHandler(device,
                                            ADI_ADRV9025_CPU_TYPE_C,
                                            ADI_ADRV9025_SRC_CPUCMD,
                                            ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
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

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_ExtDpdHardClipperCounterGet(adi_adrv9025_Device_t*    device,
                                                 adi_adrv9025_TxChannels_e txChannel,
                                                 uint32_t*                 counter)
{
    static const uint8_t EXT_DPD_HARD_CLIPPER_COUNTER_GET = 0x01;
    static const uint8_t ARM_ERR_CODE                     = 0x0E;

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t extData[4]     = {ADRV9025_CPU_OBJECTID_TRACKING_CAL_CTRL, ADRV9025_CPU_OBJECTID_DPD_INIT, (uint8_t)txChannel, EXT_DPD_HARD_CLIPPER_COUNTER_GET};
    uint8_t armData[4]     = {0};
    uint8_t cmdStatusByte  = 0u;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_NULL_PTR_RETURN(&device->common, counter);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API);

    if (txChannel != ADI_ADRV9025_TX1 &&
        txChannel != ADI_ADRV9025_TX2 &&
        txChannel != ADI_ADRV9025_TX3 &&
        txChannel != ADI_ADRV9025_TX4)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         txChannel,
                         "Invalid Tx Channel requested.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

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
                                                   (uint8_t)ADRV9025_CPU_GET_OPCODE,
                                                   &cmdStatusByte,
                                                   ADI_ADRV9025_GETEXTDHARDCLIPPER_TIMEOUT_US,
                                                   (uint32_t)ADI_ADRV9025_GETEXTDHARDCLIPPER_INTERVAL_US);

    /* If cmdStatusByte is non-zero then flag an ARM error */
    if ((cmdStatusByte & ARM_ERR_CODE) > 0)
    {
        adrv9025_CpuCmdErrorHandler(device,
                                    ADI_ADRV9025_CPU_TYPE_C,
                                    ADI_ADRV9025_SRC_CPUCMD,
                                    ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_GET_OPCODE,
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

    /* Read the values from ARM mailbox */
    recoveryAction = adi_adrv9025_CpuMemRead(device,
                                             ADRV9025_CPU_C_ADDR_MAILBOX_SET,
                                             &armData[0],
                                             sizeof(armData),
                                             ADI_ADRV9025_CPU_MEM_AUTO_INCR);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     armData,
                     device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Parse the values */
    *counter = ((uint32_t)armData[3] << 24) |
    ((uint32_t)armData[2] << 16) |
    ((uint32_t)armData[1] << 8) |
    ((uint32_t)armData[0]);

    return (device->common.error.newAction);
}


int32_t adi_adrv9025_ExtDpdActutatorPassthruSet(adi_adrv9025_Device_t *device, uint32_t txChannelMask, uint8_t actuatorPassthruEn)
{
    uint8_t actuatorEnableReadBack = 0;
    uint8_t actuatorPassthruReadBack = 0;
    uint8_t k = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API);

    if ((txChannelMask == 0) || ((txChannelMask & (~(uint32_t)ADI_ADRV9025_TXALL)) != 0))
    {
        /* Tx channel selection is not valid */
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM, ADI_COMMON_ACT_ERR_CHECK_PARAM, txChannelMask,
            "Invalid Tx channel selection, any combination of Tx1/2/3/4 channels are valid");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    for (k = 0; k < ADI_ADRV9025_MAX_TXCHANNELS; k++)
    {
        if (ADRV9025_BF_EQUAL(txChannelMask, ((uint32_t)1 << (uint32_t)k)))
        {
            /* Get the current DPD actuator enable and passthru values */
            adrv9025_ExtDpdActutatorEnPassthruGet(device, (adi_adrv9025_TxChannels_e)((uint32_t)1 << (uint32_t)k), &actuatorEnableReadBack, &actuatorPassthruReadBack);
            ADI_ERROR_RETURN(device->common.error.newAction);

            /* Enable DPD to program LUTs */
            adrv9025_ExtDpdActutatorEnPassthruSet(device, (adi_adrv9025_TxChannels_e)((uint32_t)1 << (uint32_t)k), actuatorEnableReadBack, actuatorPassthruEn);
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    return  (device->common.error.newAction);
}

int32_t adi_adrv9025_ExtDpdActutatorPassthruGet(adi_adrv9025_Device_t *device, adi_adrv9025_TxChannels_e txChannel, uint8_t *actuatorPassthruEnRead)
{
    uint8_t actuatorEnableReadBack = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API);

    ADI_NULL_PTR_RETURN(&device->common, actuatorPassthruEnRead);

    if (txChannel != ADI_ADRV9025_TX1 &&
        txChannel != ADI_ADRV9025_TX2 &&
        txChannel != ADI_ADRV9025_TX3 &&
        txChannel != ADI_ADRV9025_TX4)
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            txChannel,
            "Invalid Tx Channel requested.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Get the current DPD actuator enable and passthru values */
    adrv9025_ExtDpdActutatorEnPassthruGet(device, txChannel, &actuatorEnableReadBack, actuatorPassthruEnRead);
    ADI_ERROR_RETURN(device->common.error.newAction);

    return  (device->common.error.newAction);
}

int32_t adi_adrv9025_ExtDpdActuatorMultiModelSwapLutWrite(adi_adrv9025_Device_t *device,
                                                          adi_adrv9025_SwapModeLut_t multiModelSwapModeLutData[])
{
    uint8_t lutIndex = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API);

    /* Check that swapModeLutData is not null */
    ADI_NULL_PTR_RETURN(&device->common, multiModelSwapModeLutData);

#if ADI_ADRV9025_EXT_DPD_RANGE_CHECK > 0	
    adrv9025_ExtDpdActuatorMultiModelSwapLutWriteRangeCheck(device, multiModelSwapModeLutData);
    ADI_ERROR_RETURN(device->common.error.newAction);
#endif

    /* Program the active LUT banks */
    for (lutIndex = 0; lutIndex <= (uint8_t)ADI_ADRV9025_EXT_DPD_LUT25; lutIndex++)
    {
        adrv9025_ExtDpdActuatorMultiModelSwapLutWrite(device, (adi_adrv9025_ExtDpdLut_e)lutIndex, multiModelSwapModeLutData[lutIndex].extDpdSwapModeLutDataPtr, multiModelSwapModeLutData[lutIndex].extDpdSwapModeLutDataSizeBytes);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_ExtDpdActuatorMultiModelSwapLutUpdate(adi_adrv9025_Device_t *device,
                                                           adi_adrv9025_TxChannels_e txChannel,
                                                           adi_adrv9025_ExtDpdModelSel_e modelSel)
{
    static const uint8_t ARM_ERR_CODE = 0x0E;
    static const uint8_t CAL_CTRL_SET_BF = 0x14;

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t extData[4] = { ADRV9025_CPU_OBJECTID_TRACKING_CAL_CTRL, ADRV9025_CPU_OBJECTID_DPD_INIT, (uint8_t)ADI_ADRV9025_TXOFF, CAL_CTRL_SET_BF };
    uint8_t cmdStatusByte = 0;
    uint8_t modelSelection = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API_PRIV);

    if (txChannel != ADI_ADRV9025_TX1 &&
        txChannel != ADI_ADRV9025_TX2 &&
        txChannel != ADI_ADRV9025_TX3 &&
        txChannel != ADI_ADRV9025_TX4)
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            txChannel,
            "Invalid Tx Channel requested.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((modelSel != ADI_ADRV9025_EXT_DPD_MODEL0) &&
        (modelSel != ADI_ADRV9025_EXT_DPD_MODEL1))
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            txChannel,
            "Invalid Model selected. Only Model0 and Model1 is supported for this mode.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    extData[2] = (uint8_t)txChannel;
    modelSelection = (uint8_t)modelSel;

    /* Write the LUT update model index to ARM mailbox */
    recoveryAction = adi_adrv9025_CpuMemWrite(device, (uint32_t)ADRV9025_CPU_C_ADDR_MAILBOX_SET, &modelSelection, 1, ADI_ADRV9025_CPU_MEM_AUTO_INCR);
    ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, device->common.error.errCode, recoveryAction, armCtrlData, device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Executing LUT update Arm Command */
    recoveryAction = adi_adrv9025_CpuCmdWrite(device, ADI_ADRV9025_CPU_TYPE_C, (uint8_t)ADRV9025_CPU_SET_OPCODE, &extData[0], sizeof(extData));
    ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, device->common.error.errCode, recoveryAction, extData, device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Wait for command to finish executing */
    recoveryAction = adi_adrv9025_CpuCmdStatusWait(device, ADI_ADRV9025_CPU_TYPE_C, (uint8_t)ADRV9025_CPU_SET_OPCODE, &cmdStatusByte, (uint32_t)ADI_ADRV9025_SETEXTDLUTUPDATE_TIMEOUT_US, (uint32_t)ADI_ADRV9025_SETEXTDLUTUPDATE_INTERVAL_US);

    /* If cmdStatusByte is non-zero then flag an ARM error */
    if ((cmdStatusByte & ARM_ERR_CODE) > 0)
    {
        recoveryAction = adrv9025_CpuCmdErrorHandler(device, ADI_ADRV9025_CPU_TYPE_C, ADI_ADRV9025_SRC_CPUCMD, ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE, extData[0], cmdStatusByte), recoveryAction);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, device->common.error.errCode, recoveryAction, cmdStatusByte, device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    return recoveryAction;
}

int32_t adi_adrv9025_ExtDpdActuatorMultiModelSwapLutUpdateStatusGet(adi_adrv9025_Device_t *device,
                                                                    adi_adrv9025_TxChannels_e txChannel,
                                                                    uint8_t *updateStatus,
                                                                    adi_adrv9025_ExtDpdLutBank_e *bankIndex)
{
    static const uint8_t ARM_ERR_CODE = 0x0E;
    static const uint8_t CAL_CTRL_GET_BF = 0x04;

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t extData[4] = { ADRV9025_CPU_OBJECTID_TRACKING_CAL_CTRL, ADRV9025_CPU_OBJECTID_DPD_INIT, (uint8_t)ADI_ADRV9025_TXOFF, CAL_CTRL_GET_BF };
    uint8_t cmdStatusByte = 0;
    uint8_t armData[2] = { 0 };

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API_PRIV);

    if (txChannel != ADI_ADRV9025_TX1 &&
        txChannel != ADI_ADRV9025_TX2 &&
        txChannel != ADI_ADRV9025_TX3 &&
        txChannel != ADI_ADRV9025_TX4)
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            txChannel,
            "Invalid Tx Channel requested.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    extData[2] = (uint8_t)txChannel;

    /* Executing LUT update status get Arm Command */	
    recoveryAction = adi_adrv9025_CpuCmdWrite(device, ADI_ADRV9025_CPU_TYPE_C, (uint8_t)ADRV9025_CPU_SET_OPCODE, &extData[0], sizeof(extData));
    ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, device->common.error.errCode, recoveryAction, extData, device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Wait for command to finish executing */
    recoveryAction = adi_adrv9025_CpuCmdStatusWait(device, ADI_ADRV9025_CPU_TYPE_C, (uint8_t)ADRV9025_CPU_SET_OPCODE, &cmdStatusByte, (uint32_t)ADI_ADRV9025_SETEXTDLUTUPDATE_TIMEOUT_US, (uint32_t)ADI_ADRV9025_SETEXTDLUTUPDATE_INTERVAL_US);

    /* If cmdStatusByte is non-zero then flag an ARM error */
    if ((cmdStatusByte & ARM_ERR_CODE) > 0)
    {
        recoveryAction = adrv9025_CpuCmdErrorHandler(device, ADI_ADRV9025_CPU_TYPE_C, ADI_ADRV9025_SRC_CPUCMD, ADRV9025_CPU_CMD_ERRCODE(ADRV9025_CPU_SET_OPCODE, extData[0], cmdStatusByte), recoveryAction);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, device->common.error.errCode, recoveryAction, cmdStatusByte, device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Read the values from ARM mailbox */
    recoveryAction = adi_adrv9025_CpuMemRead(device, (uint32_t)ADRV9025_CPU_C_ADDR_MAILBOX_SET, &armData[0], sizeof(armData), ADI_ADRV9025_CPU_MEM_AUTO_INCR);
    ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, device->common.error.errCode, recoveryAction, armData, device->common.error.errormessage);
    ADI_ERROR_RETURN(device->common.error.newAction);

    *updateStatus = armData[0];
    *bankIndex = (adi_adrv9025_ExtDpdLutBank_e)armData[1];

    return recoveryAction;
}


int32_t adi_adrv9025_ExtDpdActuatorMultiModelSwapModelConfigSet(adi_adrv9025_Device_t *device,
                                                                adi_adrv9025_TxChannels_e txChannel,
                                                                adi_adrv9025_ExtDpdModel_t *extDpdModelConfig)
{
#define ARM_EXT_DPD_MODEL_CFG_LENGTH_BYTES 48
    static const uint16_t PARAM_OFFSET_BYTES_TX1 = 0;
    static const uint16_t PARAM_OFFSET_BYTES_TX2 = 96;
    static const uint16_t PARAM_OFFSET_BYTES_TX3 = 192;
    static const uint16_t PARAM_OFFSET_BYTES_TX4 = 288;
    static const uint8_t MAX_ACTUATOR_OUTPUT_SHIFT = 15;

    uint8_t armConfigData[ARM_EXT_DPD_MODEL_CFG_LENGTH_BYTES] = { 0 };
    uint16_t extDpdModelParamOffsetBytes = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API);

    /* Check that extDpdModelConfig pointer is not null */
    ADI_NULL_PTR_RETURN(&device->common, extDpdModelConfig);

    /* Check that the DPD model is valid */
    if ((extDpdModelConfig->extDpdModelSel != ADI_ADRV9025_EXT_DPD_MODEL0) &&
        (extDpdModelConfig->extDpdModelSel != ADI_ADRV9025_EXT_DPD_MODEL1))
    {
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM, ADI_COMMON_ACT_ERR_CHECK_PARAM, extDpdModelConfig->extDpdModelSel,
            "Invalid target DPD model selected for DPD model configuration. Valid DPD models are Model0-Model1");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Check that actuator output shift is less than 16 */
    if (extDpdModelConfig->extDpdActuatorOutputShift > MAX_ACTUATOR_OUTPUT_SHIFT)
    {
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM, ADI_COMMON_ACT_ERR_CHECK_PARAM, extDpdModelConfig->extDpdActuatorOutputShift,
            "Actuator output shift should be less than 15");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    switch (txChannel)
    {
    case(ADI_ADRV9025_TX1):
    {
        extDpdModelParamOffsetBytes = PARAM_OFFSET_BYTES_TX1;
        break;
    }
    case(ADI_ADRV9025_TX2):
    {
        extDpdModelParamOffsetBytes = PARAM_OFFSET_BYTES_TX2;
        break;
    }
    case(ADI_ADRV9025_TX3):
    {
        extDpdModelParamOffsetBytes = PARAM_OFFSET_BYTES_TX3;
        break;
    }
    case(ADI_ADRV9025_TX4):
    {
        extDpdModelParamOffsetBytes = PARAM_OFFSET_BYTES_TX4;
        break;
    }
    default:
    {
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM, ADI_COMMON_ACT_ERR_CHECK_PARAM, txChannel,
            "Invalid target Tx channel selected for DPD actuator model config set. Valid Tx channels are Tx1-Tx4");
        ADI_ERROR_RETURN(device->common.error.newAction);
        break;
    }
    }

    if (extDpdModelConfig->extDpdModelSel == ADI_ADRV9025_EXT_DPD_MODEL1)
    {
        /* Model1 data should have additional 48 byte offset */
        extDpdModelParamOffsetBytes += ARM_EXT_DPD_MODEL_CFG_LENGTH_BYTES;
    }

    /* Build the DPD model config array for ARM */
    adrv9025_ExtDpdModelConfigParamArrayLoad(device, txChannel, extDpdModelConfig, &armConfigData[0], (uint8_t)ARM_EXT_DPD_MODEL_CFG_LENGTH_BYTES);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Pass the DPD model config to ARM */
    adi_adrv9025_CpuConfigWrite(device, ADI_ADRV9025_CPU_TYPE_C, ADRV9025_CPU_OBJECTID_EXT_DPD_MULTIMODEL, extDpdModelParamOffsetBytes, &armConfigData[0], ARM_EXT_DPD_MODEL_CFG_LENGTH_BYTES);
    ADI_ERROR_RETURN(device->common.error.newAction);

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_ExtDpdActuatorMultiModelSwapModelConfigGet(adi_adrv9025_Device_t *device,
                                                                adi_adrv9025_TxChannels_e txChannel,
                                                                adi_adrv9025_ExtDpdModel_t *extDpdModelConfig)
{
#define ARM_EXT_DPD_MODEL_CFG_GET_LENGTH_BYTES 48
    static const uint16_t PARAM_OFFSET_BYTES_TX1 = 0;
    static const uint16_t PARAM_OFFSET_BYTES_TX2 = 96;
    static const uint16_t PARAM_OFFSET_BYTES_TX3 = 192;
    static const uint16_t PARAM_OFFSET_BYTES_TX4 = 288;

    uint8_t armConfigData[ARM_EXT_DPD_MODEL_CFG_GET_LENGTH_BYTES] = { 0 };
    uint16_t extDpdModelParamOffsetBytes = 0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API);

    /* Check that extDpdModelConfig pointer is not null */
    ADI_NULL_PTR_RETURN(&device->common, extDpdModelConfig);

    /* Check that the DPD model is valid */
    if ((extDpdModelConfig->extDpdModelSel != ADI_ADRV9025_EXT_DPD_MODEL0) &&
        (extDpdModelConfig->extDpdModelSel != ADI_ADRV9025_EXT_DPD_MODEL1))
    {
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM, ADI_COMMON_ACT_ERR_CHECK_PARAM, extDpdModelConfig->extDpdModelSel,
            "Invalid target DPD model selected for DPD model configuration. Valid DPD models are Model0-Model1");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    switch (txChannel)
    {
    case(ADI_ADRV9025_TX1):
    {
        extDpdModelParamOffsetBytes = PARAM_OFFSET_BYTES_TX1;
        break;
    }
    case(ADI_ADRV9025_TX2):
    {
        extDpdModelParamOffsetBytes = PARAM_OFFSET_BYTES_TX2;
        break;
    }
    case(ADI_ADRV9025_TX3):
    {
        extDpdModelParamOffsetBytes = PARAM_OFFSET_BYTES_TX3;
        break;
    }
    case(ADI_ADRV9025_TX4):
    {
        extDpdModelParamOffsetBytes = PARAM_OFFSET_BYTES_TX4;
        break;
    }
    default:
    {
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_INV_PARAM, ADI_COMMON_ACT_ERR_CHECK_PARAM, txChannel,
            "Invalid target Tx channel selected for DPD actuator model config get. Valid Tx channels are Tx1-Tx4");
        ADI_ERROR_RETURN(device->common.error.newAction);
        break;
    }
    }

    if (extDpdModelConfig->extDpdModelSel == ADI_ADRV9025_EXT_DPD_MODEL1)
    {
        /* Model1 data should have additional 48 byte offset */
        extDpdModelParamOffsetBytes += ARM_EXT_DPD_MODEL_CFG_LENGTH_BYTES;
    }

    /* Read ext DPD capture config from ARM mailbox */
    adi_adrv9025_CpuConfigRead(device, ADI_ADRV9025_CPU_TYPE_C, ADRV9025_CPU_OBJECTID_EXT_DPD_MULTIMODEL, extDpdModelParamOffsetBytes, &armConfigData[0], sizeof(armConfigData));
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Parse the config array received from ARM */
    adrv9025_ExtDpdModelConfigParamArrayParse(device, extDpdModelConfig, &armConfigData[0], (uint8_t)ARM_EXT_DPD_MODEL_CFG_GET_LENGTH_BYTES);
    ADI_ERROR_RETURN(device->common.error.newAction);

    return (device->common.error.newAction);
}
