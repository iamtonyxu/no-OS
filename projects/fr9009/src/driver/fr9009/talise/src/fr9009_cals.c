/**
 * \file fr9009_cals.c
 * \brief Contains functions to support FR9009 init and tracking calibrations
 *
 * Copyright 2022 BriRadio.
 * Released under the FR9009 API license, for more information see the "LICENSE.txt" file in this zip file.
 */

#include "fr9009_cals.h"
#include "fr9009_reg_addr_macros.h"
#include "fr9009_arm_macros.h"
#include "fr9009_hal.h"
#include "fr9009_user.h"
#include "fr9009_error.h"
#include "fr9009_radioctrl.h"
#include "fr9009_arm.h"
#include "fr9009.h"

uint32_t FR9009_runInitCals(fr9009Device_t *device, CHANNEL_t tx_ch, CHANNEL_t rx_ch, CHANNEL_t orx_ch, CHANNEL_t tx1_lp2orx_ch, CHANNEL_t tx2_lp2orx_ch, uint32_t calMask) 
{
    talRecoveryActions_t retVal = TALACT_NO_ACTION;
    // CHANNEL_t tx_ch = (device->devStateInfo.initializedChannels >> 4) & 0x3;
    // CHANNEL_t rx_ch = device->devStateInfo.initializedChannels & 0x3;
    // CHANNEL_t orx_ch = (device->devStateInfo.initializedChannels >> 2) & 0x3;
    // CHANNEL_t tx1_lp2orx_ch = 0;
    // CHANNEL_t tx2_lp2orx_ch = 0;
    uint8_t external_lp = 0;
    uint32_t curcalMask = calMask;
    enum TX_LOOPMODE_t tx_internal_lpmode = (calMask & FR_TX_INIT_INTERNAL_LP2) ? TX_LOOP2ORX2_MODE : TX_LOOP2ORX_MODE;
#if FR9009_VERBOSE
    fRHALErr_t halError = FRHAL_OK;
    halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK, "FR9009_runInitCals()\n");
    retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal, TALACT_WARN_RESET_LOG);
#endif
    if(calMask & FR_TX_INIT_EXTERNAL){
        external_lp = 1;
    }
    // if(((device->devStateInfo.initializedChannels >> 2) & 0x3) == 0x1){
    //     tx1_lp2orx_ch = 0x1;
    //     tx2_lp2orx_ch = 0x1;
    // }else if(((device->devStateInfo.initializedChannels >> 2) & 0x3) == 0x2){
    //     tx1_lp2orx_ch = 0x2;
    //     tx2_lp2orx_ch = 0x2;
    // }
    calMask = 0;
    if(curcalMask & FR_PATH_DELAY){
        calMask |= FR_CALI_PATH_DELAY;
    }    
    if(curcalMask & FR_TX_QEC_INIT){
        calMask |= FR_CALI_TX_QEC_INIT;
    }
    if(curcalMask & (FR_TX_LO_LEAKAGE_INTERNAL | FR_TX_LO_LEAKAGE_EXTERNAL)){
        calMask |= FR_CALI_TX_LO_CALI;
    }
    if(curcalMask & FR_RX_LO_DELAY){
        calMask |= FR_CALI_RX_LO_DELAY;
    }
    if(curcalMask & FR_RX_QEC_INIT){
        calMask |= FR_CALI_RX_QEC_INIT;
    }
    if(curcalMask & FR_DC_OFFSET){
        calMask |= FR_CALI_RX_DC_OFFSET;
        calMask |= FR_CALI_ORX_DC_OFFSET;
    }
    if(curcalMask & FR_ORX_LO_DELAY){
        calMask |= FR_CALI_ORX_LO_DELAY;
    }
    if(curcalMask & FR_ORX_QEC_INIT){
        calMask |= FR_CALI_ORX_QEC_INIT;
    }
    if(curcalMask & FR_FHM_CALS){
        calMask |= FR_CALI_FHM_CALS;
    }
    if(curcalMask & FR_TX_FLATTEN_INIT){
        calMask |= FR_CALI_TX_FLATTEN_INIT;
    }
    if(curcalMask & FR_RX_FLATTEN_INIT){
        calMask |= FR_CALI_RX_FLATTEN_INIT;
    }
    if(curcalMask & FR_ORX_FLATTEN_INIT){
        calMask |= FR_CALI_ORX_FLATTEN_INIT;
    }
    if(device->devStateInfo.fhm_enable){
        if((device->devStateInfo.fhmtype == FR_FHM_AUTO_NCO_MODE || device->devStateInfo.fhmtype == FR_FHM_MANUAL_NCO_MODE) 
                && device->devStateInfo.fhm_enable){ 
            calMask = 0; 
            if(tx_ch != 0){
                calMask |= FR_CALI_TX_LO_CALI;
            }      
            calMask |= FR_CALI_NCO_CALS;
        }else{// pre cali
            calMask |= FR_CALI_FHM_CALS;
        }
    }
    if(device->devStateInfo.predict_cali){
        calMask |= FR_CALI_PRECALI_START;
    }
    retVal = (talRecoveryActions_t)FR9009_armSpiCmd_Initical_cali(device, tx_ch, rx_ch, orx_ch, tx1_lp2orx_ch, tx2_lp2orx_ch, external_lp, tx_internal_lpmode, calMask);
    IF_ERR_RETURN_U32(retVal);
    device->devStateInfo.devState = (fr9009States_t)(device->devStateInfo.devState | FR_STATE_CALS_RUN);
    return (uint32_t)retVal;
}


uint32_t  FR9009_waitInitCals (fr9009Device_t *device, uint32_t timeoutMs, uint8_t *errorFlag) 
{
    talRecoveryActions_t retVal = TALACT_NO_ACTION;
    talRecoveryActions_t retValWarn = TALACT_NO_ACTION;
    uint32_t cmdstatword = 0;
    uint8_t _errFlag = 0;

    static const uint16_t TIMEOUT_MS_FACTOR = 1000;
    static const uint32_t CODECHECK_PARAM_WAITINITCALS_ERR = 2;

#if FR9009_VERBOSE
    fRHALErr_t halError = FRHAL_OK;
    halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK, "FR9009_waitInitCals()\n");
    retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal, TALACT_WARN_RESET_LOG);
#endif

    if (errorFlag == NULL)
    {
        return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
                FR_ERR_WAIT_INITCALS_NULL_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
    }

    retValWarn = retVal;
	retVal = FR9009_waitArmCmdStatus(device, TALAPI_ARMSPI_CMD_INIT_CAL, &cmdstatword,  (timeoutMs * TIMEOUT_MS_FACTOR), WAITINITCALS_INTERVAL_US);
	IF_ERR_RETURN_U32(retVal);	
	cmdstatword &= ~TALAPI_ARMSPI_READY;//此处进行ready对比
    _errFlag = (cmdstatword >> 1);

    /* SW Test */
    if (device->devStateInfo.swTest == CODECHECK_PARAM_WAITINITCALS_ERR)
    {
        retVal = TALACT_ERR_RESET_SPI;
        cmdstatword = 2;
        _errFlag = (cmdstatword >> 1);
    }

    /* Don't update errorFlag if SPI error because errorFlag could be a random */
    /* value but update error flag for other recovery action types */
    if (retVal == TALACT_ERR_RESET_SPI)
    {
        *errorFlag = 0;
    }
    else
    {
        *errorFlag = _errFlag;
    }

    /* ARM error handler to provide valid recovery action based on ARM error
     * code */
    if (_errFlag > 0)
    {
        return (uint32_t)talApiErrHandler(device, FR_ERRHDL_ARM_CMD_ERR,
                ARMCMD_ERRCODE(TALAPI_ARMSPI_CMD_INIT_CAL, 0, cmdstatword), retVal, TALACT_ERR_RESET_ARM);
    }
    else
    {
        IF_ERR_RETURN_U32(retVal);
    }

    /* if no error from higher priority calls, return possible log warning */
    if (retVal == TALACT_NO_ACTION)
    {
        retVal = retValWarn;
    }

    return (uint32_t)retVal;
}

uint32_t  FR9009_setDigDcOffsetMShift (fr9009Device_t *device, fr9009DcOffsetChannels_t channel, uint8_t mShift) 
{
    talRecoveryActions_t retVal = TALACT_NO_ACTION;
    // talRecoveryActions_t retValWarn = TALACT_NO_ACTION;
    uint32_t dcOffset = 0;

#if FR9009_VERBOSE
    fRHALErr_t halError = FRHAL_OK;
    halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK, "FR9009_setDigDcOffsetMShift()\n");
    retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal, TALACT_WARN_RESET_LOG);
#endif

    dcOffset = (1 << (mShift + 1)) - 1;
    switch (channel) {
        case FR_DC_OFFSET_RX_CHN:
            retVal = FR9009_armSpiCmd_mem_mask_write(device, mShift << 4, FR9009_ADDR_DC_CORR_MAN_MODE_SETTING, 0x000001F0);
            IF_ERR_RETURN_U32(retVal);
            retVal = FR9009_armSpiCmd_writeWords(device, APB_RX_DC_CORR_BASEADDR + 0xC, &dcOffset, 1);
            IF_ERR_RETURN_U32(retVal);
            break;

        case FR_DC_OFFSET_ORX_CHN:
            retVal = FR9009_armSpiCmd_mem_mask_write(device, mShift << 12, FR9009_ADDR_DC_CORR_MAN_MODE_SETTING, 0x0001F000);
            IF_ERR_RETURN_U32(retVal);
            retVal = FR9009_armSpiCmd_writeWords(device, APB_RX_DC_CORR_BASEADDR + 0x10, &dcOffset, 1);
            IF_ERR_RETURN_U32(retVal);
            break;
        
        default:
            return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_SETDCOFFSET_INV_CHN, retVal, TALACT_ERR_CHECK_PARAM);
            break;
    }
    
    return (uint32_t)retVal;

}

uint32_t  FR9009_getDigDcOffsetMShift (fr9009Device_t *device, fr9009DcOffsetChannels_t channel, uint8_t *mShift) 
{
    talRecoveryActions_t retVal = TALACT_NO_ACTION;
    uint32_t dcOffset = 0;

#if FR9009_VERBOSE
    fRHALErr_t halError = FRHAL_OK;
    halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK, "FR9009_getDigDcOffsetMShift()\n");
    retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal, TALACT_WARN_RESET_LOG);
#endif

    if (mShift == NULL)
    {
        return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_GETDCOFFSET_NULL_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
    }
    
    switch (channel) {
        case FR_DC_OFFSET_RX_CHN:
        case FR_DC_OFFSET_ORX_CHN:
            retVal = FR9009_armSpiCmd_readWords(device, FR9009_ADDR_DC_CORR_MAN_MODE_SETTING, &dcOffset, 1);
            IF_ERR_RETURN_U32(retVal);
            break;
        
        default:
            return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_GETDCOFFSET_INV_CHN, retVal, TALACT_ERR_CHECK_PARAM);
            break;
    }

    *mShift = (uint8_t)((dcOffset >> (4 + 8 * channel)) & 0x1F);
    
    return (uint32_t)retVal;
}

/**
 *     Channel              |  Value  |  Channel description
 * -------------------------|---------|--------------------------
 *  FR_DC_OFFSET_ALL_OFF   |   0x00  | Disable all the channels
 *  FR_DC_OFFSET_RX1       |   0x01  | Enables Rx1 manual mode
 *  FR_DC_OFFSET_RX2       |   0x02  | Enables Rx1 manual mode
 *  FR_DC_OFFSET_ORX1      |   0x04  | Enables ORx1 manual mode
 *  FR_DC_OFFSET_ORX2      |   0x08  | Enables ORx2 manual mode
 *  FR_DC_OFFSET_ALL_ON    |   0x0F  | Enables all the channels manual mode
*/
uint32_t  FR9009_setDigDcOffsetEn (fr9009Device_t *device, uint8_t enableMask) 
{
    talRecoveryActions_t retVal = TALACT_NO_ACTION;
    uint32_t enableVal = 0;
    uint32_t wrMask = 0;

#if FR9009_VERBOSE
    fRHALErr_t halError = FRHAL_OK;
    halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK, "FR9009_setDigDcOffsetEn()\n");
    retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal, TALACT_WARN_RESET_LOG);
#endif

    wrMask |= FR9009_ADDR_DC_CORR_MAN_MODE_RX_CH1_EN_MASK;
    wrMask |= FR9009_ADDR_DC_CORR_MAN_MODE_RX_CH2_EN_MASK;
    wrMask |= FR9009_ADDR_DC_CORR_MAN_MODE_ORX_CH1_EN_MASK;
    wrMask |= FR9009_ADDR_DC_CORR_MAN_MODE_ORX_CH2_EN_MASK;

    if (enableMask & FR_DC_OFFSET_RX1) {
        enableVal |= FR9009_ADDR_DC_CORR_MAN_MODE_RX_CH1_EN_MASK;
    }
    if (enableMask & FR_DC_OFFSET_RX2) {
        enableVal |= FR9009_ADDR_DC_CORR_MAN_MODE_RX_CH2_EN_MASK;
    }
    if (enableMask & FR_DC_OFFSET_ORX1) {
        enableVal |= FR9009_ADDR_DC_CORR_MAN_MODE_ORX_CH1_EN_MASK;
    }
    if (enableMask & FR_DC_OFFSET_ORX2) {
        enableVal |= FR9009_ADDR_DC_CORR_MAN_MODE_ORX_CH2_EN_MASK;
    }

    retVal = FR9009_armSpiCmd_writeField(device,FR9009_ADDR_DC_CORR_MAN_MODE_SETTING,enableVal,wrMask,0);
    IF_ERR_RETURN_U32(retVal);
    return (uint32_t) retVal;
}

/**
 *     Channel              |  Value  |  Channel description
 * -------------------------|---------|--------------------------
 *  FR_DC_OFFSET_ALL_OFF   |   0x00  | Disable all the channels
 *  FR_DC_OFFSET_RX1       |   0x01  | Enables Rx1 manual mode
 *  FR_DC_OFFSET_RX2       |   0x02  | Enables Rx1 manual mode
 *  FR_DC_OFFSET_ORX1      |   0x04  | Enables ORx1 manual mode
 *  FR_DC_OFFSET_ORX2      |   0x08  | Enables ORx2 manual mode
 *  FR_DC_OFFSET_ALL_ON    |   0x0F  | Enables all the channels
*/

uint32_t  FR9009_getDigDcOffsetEn (fr9009Device_t *device, uint8_t *enableMask) 
{
    talRecoveryActions_t retVal = TALACT_NO_ACTION;
    uint32_t enableVal = 0;
    uint32_t rdMask = 0;

#if FR9009_VERBOSE
    fRHALErr_t halError = FRHAL_OK;
    halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK, "FR9009_setDigDcOffsetEn()\n");
    retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal, TALACT_WARN_RESET_LOG);
#endif

    if (enableMask == NULL) {
        return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_GETDCOFFSETEN_NULL_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
    }

    retVal = FR9009_armSpiCmd_readWords(device,FR9009_ADDR_DC_CORR_MAN_MODE_SETTING,&enableVal,1);
    IF_ERR_RETURN_U32(retVal);
    rdMask |= FR9009_ADDR_DC_CORR_MAN_MODE_RX_CH1_EN_MASK;
    rdMask |= FR9009_ADDR_DC_CORR_MAN_MODE_RX_CH2_EN_MASK;
    rdMask |= FR9009_ADDR_DC_CORR_MAN_MODE_ORX_CH1_EN_MASK;
    rdMask |= FR9009_ADDR_DC_CORR_MAN_MODE_ORX_CH2_EN_MASK;

    *enableMask = (uint8_t)(enableVal & rdMask);

    return (uint32_t) retVal;
}

#if 0
uint32_t  FR9009_checkInitCalComplete (fr9009Device_t *device, uint8_t *areCalsRunning, uint8_t *errorFlag) 
{
	return 123;
}


uint32_t  FR9009_abortInitCals (fr9009Device_t *device, uint32_t *calsCompleted) 
{
	return 123;
}

uint32_t  FR9009_getInitCalStatus (fr9009Device_t *device, uint32_t *calsSincePowerUp, uint32_t *calsLastRun, uint32_t *calsMinimum, uint8_t *initErrCal, uint8_t *initErrCode) 
{
	return 123;
}
#endif
uint32_t  FR9009_enableTrackingCals (fr9009Device_t *device, uint32_t enableMask) 
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	retVal = FR9009_armSpiCmd_track_cali_En(device, enableMask);
    device->devStateInfo.tracking_en = enableMask;
	return retVal;
}

#if 0
uint32_t  FR9009_getEnabledTrackingCals (fr9009Device_t *device, uint32_t *enableMask) 
{
	return 123;
}


uint32_t  FR9009_getPendingTrackingCals (fr9009Device_t *device, uint32_t *pendingCalMask) 
{
	return 123;
}

uint32_t  FR9009_rescheduleTrackingCal (fr9009Device_t *device, fr9009TrackingCalibrations_t trackingCal) 
{
	return 123;
}
#endif
uint32_t  FR9009_setAllTrackCalState (fr9009Device_t *device, uint32_t calSubsetMask, uint32_t resumeCalMask) 
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
    uint32_t enableMask = 0;  
    enableMask = (device->devStateInfo.tracking_en & (~calSubsetMask)) | (device->devStateInfo.tracking_en & (calSubsetMask&resumeCalMask));
	retVal = FR9009_enableTrackingCals(device, enableMask);
    IF_ERR_RETURN_U32(retVal);
	return retVal;
}

#if 0
uint32_t  FR9009_getAllTrackCalState (fr9009Device_t *device, uint32_t *resumeCalMask) 
{
	return 123;
}

uint32_t  FR9009_getTxLolStatus (fr9009Device_t *device, fr9009TxChannels_t channelSel, fr9009TxLolStatus_t *txLolStatus) 
{
return 123;
} 
uint32_t  FR9009_getTxQecStatus (fr9009Device_t *device, fr9009TxChannels_t channelSel, fr9009TxQecStatus_t *txQecStatus) 
{
	return 123;
}


uint32_t  FR9009_getRxQecStatus (fr9009Device_t *device, fr9009RxChannels_t channelSel, fr9009RxQecStatus_t *rxQecStatus) 
{
	return 123;
}


uint32_t  FR9009_getOrxQecStatus (fr9009Device_t *device, fr9009ObsRxChannels_t channelSel, fr9009OrxQecStatus_t *orxQecStatus) 
{
	return 123;
}


uint32_t  FR9009_getRxHd2Status (fr9009Device_t *device, fr9009RxChannels_t channelSel, fr9009RxHd2Status_t *rxHd2Status) 
{
	return 123;
}

uint32_t  FR9009_waitForEvent (fr9009Device_t *device, fr9009WaitEvent_t waitEvent, uint32_t timeout_us, uint32_t waitInterval_us) 
{
	return 123;
}


uint32_t  FR9009_readEventStatus (fr9009Device_t *device, fr9009WaitEvent_t waitEvent, uint8_t *eventDone) 
{
return 10;
}


uint32_t  FR9009_resetExtTxLolChannel (fr9009Device_t *device, fr9009TxChannels_t channelSel) 
{
return 10;
}

uint32_t  FR9009_setRxHd2Config (fr9009Device_t *device, fr9009RxHd2Config_t *hd2CalConfig) 
{
	return 10;
}


uint32_t  FR9009_getRxHd2Config (fr9009Device_t *device, fr9009RxHd2Config_t *hd2CalConfig) 
{
	return 10;
}



uint32_t  FR9009_getTrackingCalsBatchSize (fr9009Device_t *device, fr9009TrackingCalBatchSize_t *batchsize_us) 
{
	return 10;
}

uint32_t  FR9009_setTrackingCalsBatchSize (fr9009Device_t *device, fr9009TrackingCalBatchSize_t batchsize_us) 
{
	return 10;
}

const char *  talGetCalErrorMessage (uint32_t errSrc, uint32_t errCode) 
{
	return 10;
}
#endif
