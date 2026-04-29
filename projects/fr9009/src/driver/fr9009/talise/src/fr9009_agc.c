/**
 * \file fr9009_agc.c
 * \brief Contains FR9009 API AGC function calls
 *
 * Copyright 2022 BriRadio.
 * Released under the FR9009 API license, for more information see the "LICENSE.txt" file in this zip file.
 */

#include "fr9009_agc.h"
#include "fr9009_reg_addr_macros.h"
#include "fr9009_user.h"
#include "fr9009_hal.h"
#include "fr9009_error.h"
#include "fr9009_rx_types.h"
#include "fr9009_gpio.h"
#include "fr9009_arm.h"

uint32_t FR9009_setupRxAgc(fr9009Device_t *device, fr9009AgcCfg_t *rxAgcCtrl)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	fRHALErr_t halError = FRHAL_OK;
	AgcCtrl_t agcCtrlReg={
		.cmdMode = AGC_SET_CONTROL,
	};
	AgcPower_t agcPowerReg ={
		.cmdMode = AGC_SET_POWER,
	};
	AgcPeak_t agcPeakReg = {
		.cmdMode = AGC_SET_PEAK,
	};

	static const uint8_t agcPeakWaitTimeBitMask = 0x1F;
	// static const uint32_t agcGainUpdateCounterBitMask = 0x003FFFFF;
	static const uint8_t powerEnableMeasurementBitMask = 0x01;
//	static const uint8_t powerUseRfirOutBitMask = 0x02;
//	static const uint8_t powerUseBBDC2BitMask = 0x08;
	// static const uint32_t underRangeHighPowerThreshBitMask = 0x0FFFFFFF;
	// static const uint32_t underRangeLowPowerThreshBitMask = 0x0FFFFFFF;
	static const uint8_t underRangeHighPowerGainStepRecoveryBitMask = 0x3F;
	static const uint8_t underRangeLowPowerGainStepRecoveryBitMask = 0x3F;
	// static const uint32_t powerMeasurementDurationBitMask = 0x1FFFF;
	static const uint8_t agcSlowLoopSettlingDelayBitMask = 0xFF;
	static const uint8_t apdHighThreshMin = 0x07;
	static const uint8_t apdHighThreshMax = 0x31;
	static const uint8_t apdLowThreshMin = 0x07;
	static const uint8_t apdLowThreshMax = 0x31;
	static const uint8_t apdGainStepAttackBitMask = 0x1F;
	static const uint8_t apdGainStepRecoveryBitMask = 0x1F;
	static const uint8_t enableHb2OverloadBitMask = 0x01;
	static const uint8_t hb2OverloadDurationCntBitMask = 0x7F;
	static const uint8_t hb2OverloadThreshCntBitMask = 0x0F;
	static const uint8_t hb2GainStepHighRecoveryBitMask = 0x1F;
	static const uint8_t hb2GainStepLowRecoveryBitMask = 0x1F;
	static const uint8_t hb2GainStepAttackBitMask = 0x1F;
	static const uint8_t hb2GainStepMidRecoveryBitMask = 0x1F;
	static const uint8_t hb2OverloadPowerModeBitMask = 0x01;
	static const uint8_t hb2OvrgSelBitMask = 0x01;
	static const uint8_t hb2ThreshConfigBitMask = 0x03;
	// static const uint32_t upper0PowerThreshBitMask = 0x0FFFFFFF;
	// static const uint32_t upper1PowerThreshBitMask = 0x0FFFFFFF;
	// static const uint8_t powerLogShiftBitMask = 0x0F;
	static const uint8_t overRangeLowPowerGainStepAttackBitMask = 0x1F;
	static const uint8_t overRangeHighPowerGainStepAttackBitMask = 0x1F;

	static const uint32_t agcLowThreshPreventGainBitMask = (0x1 << 8);
	static const uint32_t agcChangeGainIfThreshHighBitMask = (0x3 << 12);
	static const uint32_t agcEnableSyncPulseForGainCounterBitMask = (0x01 << 18);
	static const uint32_t agcEnableIp3OptimizationThreshBitMask = 0x80;
	static const uint32_t agcEnableFastRecoveryLoopBitMask = (0x1 << 19);
	static const uint8_t agcRx1AttackDelayBitMask = 0xFF;
	static const uint8_t agcRx2AttackDelayBitMask = 0xFF;
	// static const uint32_t agcUnderRangeLowIntervalMask = 0x0000FFFF;
	// static const uint8_t agcUnderRangeMidIntervalMask = 0x3F;
	// static const uint8_t agcUnderRangeHighIntervalMask = 0x3F;

//	static const uint8_t MIN_SUPPORTED_SIREV = 0xC0;
//	static const uint8_t ENABLE_IP3_OPTIMIZATION_MASK = 0x80;

#if FR9009_VERBOSE
	halError =brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK,"FR9009_setupRxAgc()\n");
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal,TALACT_WARN_RESET_LOG);
#endif

	/* checking for valid fr9009AgcCfg_t device->rx->rxAgcCtrl pointer to determine if it has been initialized */
	if (rxAgcCtrl == NULL) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,FR_ERR_INV_AGC_RX_STRUCT_INIT, retVal, TALACT_ERR_CHECK_PARAM);
	}
	/* performing AGC peak wait time value check */
	if (rxAgcCtrl->agcPeakWaitTime & ~agcPeakWaitTimeBitMask) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,FR_ERR_INV_AGC_RX_PEAK_WAIT_TIME_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}
	/* performing range check for Rx1 and Rx2 max gain exceedence */
	if ((rxAgcCtrl->agcRx1MaxGainIndex > device->devStateInfo.rxGainCtrl.rx1MaxGainIndex) || (rxAgcCtrl->agcRx2MaxGainIndex > device->devStateInfo.rxGainCtrl.rx2MaxGainIndex) ||
	    (rxAgcCtrl->agcRx1MinGainIndex < device->devStateInfo.rxGainCtrl.rx1MinGainIndex) || (rxAgcCtrl->agcRx2MinGainIndex < device->devStateInfo.rxGainCtrl.rx2MinGainIndex)) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_INV_AGC_RX_MIN_MAX_GAIN_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	} 
	if (rxAgcCtrl->agcRx1AttackDelay & ~agcRx1AttackDelayBitMask) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_INV_AGC_RX1ATTACKDELAY_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	} 
	if (rxAgcCtrl->agcRx2AttackDelay & ~agcRx2AttackDelayBitMask) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_INV_AGC_RX2ATTACKDELAY_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* If peak detect data structure is not included into project */
	if (&rxAgcCtrl->agcPeak == NULL) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_INV_AGC_PKK_STRUCT_INIT, retVal, TALACT_ERR_CHECK_PARAM);
	} 
	if ((rxAgcCtrl->agcPeak.apdHighThresh < apdHighThreshMin) || (rxAgcCtrl->agcPeak.apdHighThresh > apdHighThreshMax)) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_INV_AGC_PKK_HIGHTHRSH_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	} else if ((rxAgcCtrl->agcPeak.apdHighThresh < rxAgcCtrl->agcPeak.apdLowThresh)) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_INV_AGC_RX_APD_HIGH_LOW_THRESH, retVal, TALACT_ERR_CHECK_PARAM);
	} else if ((rxAgcCtrl->agcPeak.apdLowThresh < apdLowThreshMin) || (rxAgcCtrl->agcPeak.apdLowThresh > apdLowThreshMax)) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_INV_AGC_PKK_LOWGAINHIGHTHRSH_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}
	agcCtrlReg.agcGainUpdateCounter_us = bswap_32bit(rxAgcCtrl->agcGainUpdateCounter_us);
	agcCtrlReg.agcPeakWaitTime = rxAgcCtrl->agcPeakWaitTime;
	agcCtrlReg.agcRx1MaxGainIndex = rxAgcCtrl->agcRx1MaxGainIndex;
	agcCtrlReg.agcRx1MinGainIndex = rxAgcCtrl->agcRx1MinGainIndex;
	agcCtrlReg.agcRx2MaxGainIndex = rxAgcCtrl->agcRx2MaxGainIndex;
	agcCtrlReg.agcRx2MinGainIndex = rxAgcCtrl->agcRx2MinGainIndex;
	agcCtrlReg.agcRx1AttackDelay = rxAgcCtrl->agcRx1AttackDelay;
	agcCtrlReg.agcRx2AttackDelay = rxAgcCtrl->agcRx2AttackDelay;
	agcCtrlReg.agcSlowLoopSettlingDelay = rxAgcCtrl->agcSlowLoopSettlingDelay;	
	agcCtrlReg.agcLowThreshPreventGain = rxAgcCtrl->agcLowThreshPreventGain;
	agcCtrlReg.agcChangeGainIfThreshHigh = rxAgcCtrl->agcChangeGainIfThreshHigh;
	agcCtrlReg.agcPeakThreshGainControlMode = rxAgcCtrl->agcPeakThreshGainControlMode;
	agcCtrlReg.agcResetOnRxon = rxAgcCtrl->agcResetOnRxon;
	agcCtrlReg.agcEnableSyncPulseForGainCounter = rxAgcCtrl->agcEnableSyncPulseForGainCounter;
	agcCtrlReg.agcEnableFastRecoveryLoop = rxAgcCtrl->agcEnableFastRecoveryLoop;
	retVal = FR9009_armSpiCmd_agc_config_set(device, (uint32_t *)&agcCtrlReg, (sizeof(agcCtrlReg) + 3) / sizeof(uint32_t));
	IF_ERR_RETURN_U32(retVal);

	if (rxAgcCtrl->agcPeak.apdGainStepAttack & ~apdGainStepAttackBitMask) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_INV_AGC_PKK_GAINSTEPATTACK_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	} 
	if (rxAgcCtrl->agcPeak.apdGainStepRecovery & ~apdGainStepRecoveryBitMask) {
			return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_INV_AGC_PKK_GAINSTEPRECOVERY_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}
	/* HB2 Configuration */
	if ((rxAgcCtrl->agcPeak.enableHb2Overload) & ~enableHb2OverloadBitMask) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_INV_AGC_PKK_HB2OVRLD_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	} else if ((rxAgcCtrl->agcPeak.hb2OverloadDurationCnt) & ~ (hb2OverloadDurationCntBitMask)) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_INV_AGC_PKK_HB2OVRLDDURATION_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	} else if (rxAgcCtrl->agcPeak.hb2OverloadThreshCnt & ~hb2OverloadThreshCntBitMask) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_INV_AGC_PKK_HB2OVRLDTHRSHCNT_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if (rxAgcCtrl->agcPeak.hb2GainStepHighRecovery & ~hb2GainStepHighRecoveryBitMask) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_INV_AGC_PKK_HB2GAINSTEPRECOVERY_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if (rxAgcCtrl->agcPeak.hb2GainStepLowRecovery & ~hb2GainStepLowRecoveryBitMask) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_INV_AGC_PKK_HB2GAINSTEP0RECOVERY_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if (rxAgcCtrl->agcPeak.hb2GainStepMidRecovery & ~hb2GainStepMidRecoveryBitMask) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_INV_AGC_PKK_HB2GAINSTEP1RECOVERY_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if (rxAgcCtrl->agcPeak.hb2GainStepAttack & ~hb2GainStepAttackBitMask) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_INV_AGC_PKK_HB2GAINSTEPATTACK_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	} 
	if ((rxAgcCtrl->agcPeak.hb2OverloadPowerMode) & ~hb2OverloadPowerModeBitMask) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_INV_AGC_PKK_HB2OVRLDPWRMODE_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	} 

	if ((rxAgcCtrl->agcPeak.hb2OvrgSel) & ~hb2OvrgSelBitMask) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_INV_AGC_PKK_HB2OVRLDSEL_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	} 
	if (rxAgcCtrl->agcPeak.hb2ThreshConfig & ~hb2ThreshConfigBitMask) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_INV_AGC_PKK_HB2THRSHCFG_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}
	/* Because this field was added after production, preserve the past behavior if 0 is passed by setting to default */
	if (rxAgcCtrl->agcPeak.hb2UnderRangeLowThreshExceededCnt == 0) {
		rxAgcCtrl->agcPeak.hb2UnderRangeLowThreshExceededCnt = 3;
	}

	/* Because this field was added after production, preserve the past behavior if 0 is passed by setting to default */
	if (rxAgcCtrl->agcPeak.hb2UnderRangeMidThreshExceededCnt == 0) {
		rxAgcCtrl->agcPeak.hb2UnderRangeMidThreshExceededCnt = 3;
	}
	
	if (&rxAgcCtrl->agcPower == NULL) { /* Check for null power data structure pointer */
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_INV_AGC_PWR_STRUCT_INIT, retVal, TALACT_ERR_CHECK_PARAM);
	}else if (rxAgcCtrl->agcPower.powerEnableMeasurement & ~powerEnableMeasurementBitMask) { /* Check for null power data structure pointer */
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_INV_AGC_PWR_STRUCT_INIT, retVal, TALACT_ERR_CHECK_PARAM);
	}
	agcPeakReg.agcUnderRangeLowInterval_ns = bswap_32bit(rxAgcCtrl->agcPeak.agcUnderRangeLowInterval_ns);
	agcPeakReg.hb2HighThresh = bswap_16bit(rxAgcCtrl->agcPeak.hb2HighThresh);
	agcPeakReg.hb2UnderRangeLowThresh = bswap_16bit(rxAgcCtrl->agcPeak.hb2UnderRangeLowThresh);
	agcPeakReg.hb2UnderRangeMidThresh = bswap_16bit(rxAgcCtrl->agcPeak.hb2UnderRangeMidThresh);
	agcPeakReg.hb2UnderRangeHighThresh = bswap_16bit(rxAgcCtrl->agcPeak.hb2UnderRangeHighThresh);
	agcPeakReg.agcUnderRangeHighInterval = rxAgcCtrl->agcPeak.agcUnderRangeHighInterval;
	agcPeakReg.agcUnderRangeMidInterval = rxAgcCtrl->agcPeak.agcUnderRangeMidInterval;
	agcPeakReg.apdHighThresh = rxAgcCtrl->agcPeak.apdHighThresh;
	agcPeakReg.apdLowGainModeHighThresh = rxAgcCtrl->agcPeak.apdLowGainModeHighThresh;
	agcPeakReg.apdLowThresh = rxAgcCtrl->agcPeak.apdLowThresh;
	agcPeakReg.apdLowGainModeLowThresh = rxAgcCtrl->agcPeak.apdLowGainModeLowThresh;
	agcPeakReg.apdUpperThreshPeakExceededCnt = rxAgcCtrl->agcPeak.apdUpperThreshPeakExceededCnt;
	agcPeakReg.apdLowerThreshPeakExceededCnt = rxAgcCtrl->agcPeak.apdLowerThreshPeakExceededCnt;
	agcPeakReg.apdGainStepAttack = rxAgcCtrl->agcPeak.apdGainStepAttack;
	agcPeakReg.apdGainStepRecovery = rxAgcCtrl->agcPeak.apdGainStepRecovery;
	agcPeakReg.enableHb2Overload = rxAgcCtrl->agcPeak.enableHb2Overload;
	agcPeakReg.hb2OverloadDurationCnt = rxAgcCtrl->agcPeak.hb2OverloadDurationCnt;
	agcPeakReg.hb2OverloadThreshCnt = rxAgcCtrl->agcPeak.hb2OverloadThreshCnt;
	agcPeakReg.hb2UpperThreshPeakExceededCnt = rxAgcCtrl->agcPeak.hb2UpperThreshPeakExceededCnt;
	agcPeakReg.hb2LowerThreshPeakExceededCnt = rxAgcCtrl->agcPeak.hb2LowerThreshPeakExceededCnt;
	agcPeakReg.hb2GainStepHighRecovery = rxAgcCtrl->agcPeak.hb2GainStepHighRecovery;
	agcPeakReg.hb2GainStepLowRecovery = rxAgcCtrl->agcPeak.hb2GainStepLowRecovery;
	agcPeakReg.hb2GainStepMidRecovery = rxAgcCtrl->agcPeak.hb2GainStepMidRecovery;
	agcPeakReg.hb2GainStepAttack = rxAgcCtrl->agcPeak.hb2GainStepAttack;
	agcPeakReg.hb2OverloadPowerMode = rxAgcCtrl->agcPeak.hb2OverloadPowerMode;
	agcPeakReg.hb2OvrgSel = rxAgcCtrl->agcPeak.hb2OvrgSel;
	agcPeakReg.hb2ThreshConfig = rxAgcCtrl->agcPeak.hb2ThreshConfig;
	agcPeakReg.hb2UnderRangeMidThreshExceededCnt = rxAgcCtrl->agcPeak.hb2UnderRangeMidThreshExceededCnt;
	agcPeakReg.hb2UnderRangeLowThreshExceededCnt = rxAgcCtrl->agcPeak.hb2UnderRangeLowThreshExceededCnt;
	retVal = FR9009_armSpiCmd_agc_config_set(device, (uint32_t *)&agcPeakReg, (sizeof(agcPeakReg) + 3) / sizeof(uint32_t));
	IF_ERR_RETURN_U32(retVal);

	if (rxAgcCtrl->agcPower.underRangeHighPowerGainStepRecovery & ~underRangeHighPowerGainStepRecoveryBitMask) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_INV_AGC_PWR_LWR0PWRGAINSTEP_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}else if (rxAgcCtrl->agcPower.underRangeLowPowerGainStepRecovery & ~underRangeLowPowerGainStepRecoveryBitMask) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_INV_AGC_PWR_LWR1PWRGAINSTEP_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}	

	if(rxAgcCtrl->agcPower.rx1TddPowerMeasDuration & ~0xFFFF){
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_INV_AGC_PWR_MSR_DURATION_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}else if(rxAgcCtrl->agcPower.rx1TddPowerMeasDelay & ~0xFFFF){
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_INV_AGC_PWR_MSR_DURATION_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if(rxAgcCtrl->agcPower.rx2TddPowerMeasDuration & ~0xFFFF){
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_INV_AGC_PWR_MSR_DURATION_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}else if(rxAgcCtrl->agcPower.rx2TddPowerMeasDelay & ~0xFFFF){
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_INV_AGC_PWR_MSR_DURATION_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Because this field was added after production, preserve the past behavior if 0 is passed by setting to default */
	if (rxAgcCtrl->agcPower.overRangeLowPowerGainStepAttack == 0) {
		rxAgcCtrl->agcPower.overRangeLowPowerGainStepAttack = 4;
	}
	if ((rxAgcCtrl->agcPower.overRangeLowPowerGainStepAttack) & ~overRangeLowPowerGainStepAttackBitMask) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_INV_AGC_PWR_UPPWR0GAINSTEP_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	} 
	/* Because this field was added after production, preserve the past behavior if 0 is passed by setting to default */
	if (rxAgcCtrl->agcPower.overRangeHighPowerGainStepAttack == 0) {
		rxAgcCtrl->agcPower.overRangeHighPowerGainStepAttack = 4;
	}
	if ((rxAgcCtrl->agcPower.overRangeHighPowerGainStepAttack) & ~overRangeHighPowerGainStepAttackBitMask) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_INV_AGC_PWR_UPPWR1GAINSTEP_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}		
	agcPowerReg.rx1TddPowerMeasDuration = bswap_16bit(rxAgcCtrl->agcPower.rx1TddPowerMeasDuration);
	agcPowerReg.rx1TddPowerMeasDelay = bswap_16bit(rxAgcCtrl->agcPower.rx1TddPowerMeasDelay);
	agcPowerReg.rx2TddPowerMeasDuration = bswap_16bit(rxAgcCtrl->agcPower.rx2TddPowerMeasDuration);
	agcPowerReg.rx2TddPowerMeasDelay = bswap_16bit(rxAgcCtrl->agcPower.rx2TddPowerMeasDelay);
	agcPowerReg.powerUseRfirOut = rxAgcCtrl->agcPower.powerUseRfirOut;
	agcPowerReg.powerUseBBDC2 = rxAgcCtrl->agcPower.powerUseBBDC2;
	agcPowerReg.powerEnableMeasurement = rxAgcCtrl->agcPower.powerEnableMeasurement;
	agcPowerReg.underRangeHighPowerThresh = rxAgcCtrl->agcPower.underRangeHighPowerThresh;
	agcPowerReg.underRangeLowPowerThresh = rxAgcCtrl->agcPower.underRangeLowPowerThresh;
	agcPowerReg.underRangeHighPowerGainStepRecovery = rxAgcCtrl->agcPower.underRangeHighPowerGainStepRecovery;
	agcPowerReg.underRangeLowPowerGainStepRecovery = rxAgcCtrl->agcPower.underRangeLowPowerGainStepRecovery;
	agcPowerReg.powerMeasurementDuration = rxAgcCtrl->agcPower.powerMeasurementDuration;
	agcPowerReg.upper0PowerThresh = rxAgcCtrl->agcPower.upper0PowerThresh;
	agcPowerReg.upper1PowerThresh = rxAgcCtrl->agcPower.upper1PowerThresh;
	agcPowerReg.powerLogShift = rxAgcCtrl->agcPower.powerLogShift;
	agcPowerReg.overRangeHighPowerGainStepAttack = rxAgcCtrl->agcPower.overRangeHighPowerGainStepAttack;
	agcPowerReg.overRangeLowPowerGainStepAttack = rxAgcCtrl->agcPower.overRangeLowPowerGainStepAttack;
	retVal = FR9009_armSpiCmd_agc_config_set(device, (uint32_t *)&agcPowerReg, (sizeof(agcPowerReg) + 3) / sizeof(uint32_t));
	IF_ERR_RETURN_U32(retVal);
	return (uint32_t)retVal;
}

uint32_t FR9009_getAgcCtrlRegisters(fr9009Device_t *device, fr9009AgcCfg_t *agcCtrl)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	fRHALErr_t halError = FRHAL_OK;
	uint32_t regbuf[12] ={AGC_GET_CONTROL};
	AgcCtrl_t * pagcCtrlReg = (AgcCtrl_t * )regbuf;

#if FR9009_VERBOSE
	halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK, "FR9009_getAgcCtrlRegisters()\n");
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal, TALACT_WARN_RESET_LOG);
#endif

	/* checking for valid fr9009AgcCfg_t device->rx->rxAgcCtrl pointer to determine if it has been initialized */
	if (agcCtrl == NULL) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_INV_AGC_RX_STRUCT_INIT, retVal, TALACT_ERR_CHECK_PARAM);
	}
	
	retVal = FR9009_armSpiCmd_agc_config_get(device, regbuf, (sizeof(AgcCtrl_t) + 3) / sizeof(uint32_t));
	IF_ERR_RETURN_U32(retVal);
	agcCtrl->agcGainUpdateCounter_us=	bswap_32bit(pagcCtrlReg->agcGainUpdateCounter_us);
	agcCtrl->agcPeakWaitTime=	pagcCtrlReg->agcPeakWaitTime;
	agcCtrl->agcRx1MaxGainIndex=	pagcCtrlReg->agcRx1MaxGainIndex;
	agcCtrl->agcRx1MinGainIndex=	pagcCtrlReg->agcRx1MinGainIndex;
	agcCtrl->agcRx2MaxGainIndex=	pagcCtrlReg->agcRx2MaxGainIndex;
	agcCtrl->agcRx2MinGainIndex=	pagcCtrlReg->agcRx2MinGainIndex;
	agcCtrl->agcRx1AttackDelay=	pagcCtrlReg->agcRx1AttackDelay;
	agcCtrl->agcRx2AttackDelay=	pagcCtrlReg->agcRx2AttackDelay;
	agcCtrl->agcSlowLoopSettlingDelay=	pagcCtrlReg->agcSlowLoopSettlingDelay;	
	agcCtrl->agcLowThreshPreventGain=	pagcCtrlReg->agcLowThreshPreventGain;
	agcCtrl->agcChangeGainIfThreshHigh=	pagcCtrlReg->agcChangeGainIfThreshHigh;
	agcCtrl->agcPeakThreshGainControlMode=	pagcCtrlReg->agcPeakThreshGainControlMode;
	agcCtrl->agcResetOnRxon=	pagcCtrlReg->agcResetOnRxon;
	agcCtrl->agcEnableSyncPulseForGainCounter=	pagcCtrlReg->agcEnableSyncPulseForGainCounter;
	agcCtrl->agcEnableFastRecoveryLoop=	pagcCtrlReg->agcEnableFastRecoveryLoop;
	return (uint32_t)retVal;
}

uint32_t FR9009_getAgcPeakRegisters(fr9009Device_t *device, fr9009AgcPeak_t *agcPeak)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	fRHALErr_t halError = FRHAL_OK;
	uint32_t regbuf[12] ={AGC_GET_PEAK};
	AgcPeak_t * pagcPeakReg = (AgcPeak_t * )regbuf;

#if FR9009_VERBOSE
	halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK, "FR9009_getAgcPeakRegisters()\n");
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal, TALACT_WARN_RESET_LOG);
#endif

	/* checking for valid fr9009AgcCfg_t device->rx->rxAgcCtrl pointer to determine if it has been initialized */
	if (agcPeak == NULL){
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_INV_AGC_PKK_STRUCT_INIT, retVal, TALACT_ERR_CHECK_PARAM);
	}
	retVal = FR9009_armSpiCmd_agc_config_get(device, regbuf, (sizeof(AgcPeak_t) + 3) / sizeof(uint32_t));
	IF_ERR_RETURN_U32(retVal);
	agcPeak->agcUnderRangeLowInterval_ns=	bswap_32bit(pagcPeakReg->agcUnderRangeLowInterval_ns);
	agcPeak->hb2HighThresh=	bswap_16bit(pagcPeakReg->hb2HighThresh);
	agcPeak->hb2UnderRangeLowThresh=	bswap_16bit(pagcPeakReg->hb2UnderRangeLowThresh);
	agcPeak->hb2UnderRangeMidThresh=	bswap_16bit(pagcPeakReg->hb2UnderRangeMidThresh);
	agcPeak->hb2UnderRangeHighThresh=	bswap_16bit(pagcPeakReg->hb2UnderRangeHighThresh);
	agcPeak->agcUnderRangeHighInterval=	pagcPeakReg->agcUnderRangeHighInterval;
	agcPeak->agcUnderRangeMidInterval=	pagcPeakReg->agcUnderRangeMidInterval;
	agcPeak->apdHighThresh=	pagcPeakReg->apdHighThresh;
	agcPeak->apdLowGainModeHighThresh=	pagcPeakReg->apdLowGainModeHighThresh;
	agcPeak->apdLowThresh=	pagcPeakReg->apdLowThresh;
	agcPeak->apdLowGainModeLowThresh=	pagcPeakReg->apdLowGainModeLowThresh;
	agcPeak->apdUpperThreshPeakExceededCnt=	pagcPeakReg->apdUpperThreshPeakExceededCnt;
	agcPeak->apdLowerThreshPeakExceededCnt=	pagcPeakReg->apdLowerThreshPeakExceededCnt;
	agcPeak->apdGainStepAttack=	pagcPeakReg->apdGainStepAttack;
	agcPeak->apdGainStepRecovery=	pagcPeakReg->apdGainStepRecovery;
	agcPeak->enableHb2Overload=	pagcPeakReg->enableHb2Overload;
	agcPeak->hb2OverloadDurationCnt=	pagcPeakReg->hb2OverloadDurationCnt;
	agcPeak->hb2OverloadThreshCnt=	pagcPeakReg->hb2OverloadThreshCnt;
	agcPeak->hb2UpperThreshPeakExceededCnt=	pagcPeakReg->hb2UpperThreshPeakExceededCnt;
	agcPeak->hb2LowerThreshPeakExceededCnt=	pagcPeakReg->hb2LowerThreshPeakExceededCnt;
	agcPeak->hb2GainStepHighRecovery=	pagcPeakReg->hb2GainStepHighRecovery;
	agcPeak->hb2GainStepLowRecovery=	pagcPeakReg->hb2GainStepLowRecovery;
	agcPeak->hb2GainStepMidRecovery=	pagcPeakReg->hb2GainStepMidRecovery;
	agcPeak->hb2GainStepAttack=	pagcPeakReg->hb2GainStepAttack;
	agcPeak->hb2OverloadPowerMode=	pagcPeakReg->hb2OverloadPowerMode;
	agcPeak->hb2OvrgSel=	pagcPeakReg->hb2OvrgSel;
	agcPeak->hb2ThreshConfig=	pagcPeakReg->hb2ThreshConfig;
	agcPeak->hb2UnderRangeMidThreshExceededCnt=	pagcPeakReg->hb2UnderRangeMidThreshExceededCnt;
	agcPeak->hb2UnderRangeLowThreshExceededCnt=	pagcPeakReg->hb2UnderRangeLowThreshExceededCnt;
	return (uint32_t)retVal;
}

uint32_t FR9009_getAgcPowerRegisters(fr9009Device_t *device, fr9009AgcPower_t *agcPower)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	fRHALErr_t halError = FRHAL_OK;
	uint32_t regbuf[12] ={AGC_GET_POWER};
	AgcPower_t * pagcPowerReg = (AgcPower_t * )regbuf;

#if FR9009_VERBOSE
	halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK, "FR9009_getAgcPowerRegisters()\n");
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal, TALACT_WARN_RESET_LOG);
#endif

	/* checking for valid fr9009AgcCfg_t device->rx->rxAgcCtrl pointer to determine if it has been initialized */
	if (agcPower == NULL){
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_INV_AGC_PWR_STRUCT_INIT, retVal, TALACT_ERR_CHECK_PARAM);
	}
	retVal = FR9009_armSpiCmd_agc_config_get(device, regbuf, (sizeof(AgcPower_t) + 3) / sizeof(uint32_t));
	IF_ERR_RETURN_U32(retVal);
	agcPower->rx1TddPowerMeasDuration=	bswap_16bit(pagcPowerReg->rx1TddPowerMeasDuration);
	agcPower->rx1TddPowerMeasDelay=	bswap_16bit(pagcPowerReg->rx1TddPowerMeasDelay);
	agcPower->rx2TddPowerMeasDuration=	bswap_16bit(pagcPowerReg->rx2TddPowerMeasDuration);
	agcPower->rx2TddPowerMeasDelay=	bswap_16bit(pagcPowerReg->rx2TddPowerMeasDelay);
	agcPower->powerUseRfirOut=	pagcPowerReg->powerUseRfirOut;
	agcPower->powerUseBBDC2=	pagcPowerReg->powerUseBBDC2;
	agcPower->powerEnableMeasurement=	pagcPowerReg->powerEnableMeasurement;
	agcPower->underRangeHighPowerThresh=	pagcPowerReg->underRangeHighPowerThresh;
	agcPower->underRangeLowPowerThresh=	pagcPowerReg->underRangeLowPowerThresh;
	agcPower->underRangeHighPowerGainStepRecovery=	pagcPowerReg->underRangeHighPowerGainStepRecovery;
	agcPower->underRangeLowPowerGainStepRecovery=	pagcPowerReg->underRangeLowPowerGainStepRecovery;
	agcPower->powerMeasurementDuration=	pagcPowerReg->powerMeasurementDuration;
	agcPower->upper0PowerThresh=	pagcPowerReg->upper0PowerThresh;
	agcPower->upper1PowerThresh=	pagcPowerReg->upper1PowerThresh;
	agcPower->powerLogShift=	pagcPowerReg->powerLogShift;
	agcPower->overRangeHighPowerGainStepAttack=	pagcPowerReg->overRangeHighPowerGainStepAttack;
	agcPower->overRangeLowPowerGainStepAttack=	pagcPowerReg->overRangeLowPowerGainStepAttack;
	return (uint32_t)retVal;
}

#if 0
static talRecoveryActions_t talSetupDualBandRx1Agc(fr9009Device_t *device,
		fr9009AgcDualBandCfg_t *rxAgcCtrlDualBand)
{
	uint8_t registerValue = 0;
	uint16_t currGpio3p3Oe = 0;
	uint16_t usedGpio3p3pins = 0;
	uint16_t gpio3p3FreeMask = 0;
	uint16_t gpio3p3UsedMask = 0;
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	fRHALErr_t halError = FRHAL_OK;

	const uint8_t RX1_DUALBAND_EXT_LNA_SOURCE_CONTROL = 0x05;

	/* range check the gain against the max and min expected values */
	if ((rxAgcCtrlDualBand->agcRxDualbandExtTableUpperIndex >
	     device->devStateInfo.gainIndexes.rx1MaxGainIndex)
	    || (rxAgcCtrlDualBand->agcRxDualbandExtTableLowerIndex <
		device->devStateInfo.gainIndexes.rx1MinGainIndex)) {
		return talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
					FR_ERR_SETUPDUALBANDRXAGC_GAIN_OUT_OF_RANGE, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Check current configuration of GPIO 3.3s */
	halError = talSpiReadByte(device->devHalInfo,
				  FR9009_ADDR_GPIO_3P3V_LOWER_BYTE_SOURCE_CONTROL, &registerValue);
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	retVal = (talRecoveryActions_t)FR9009_getGpio3v3Oe(device, &currGpio3p3Oe);
	IF_ERR_RETURN(retVal);

	/* Free GPIO3.3[3:0] if used */
	if (((registerValue & 0x0F) == RX1_DUALBAND_EXT_LNA_SOURCE_CONTROL) &&
	    ((currGpio3p3Oe & 0x0F) == 0x0F)) {
		gpio3p3FreeMask = 0x0F;
		usedGpio3p3pins = device->devStateInfo.usedGpio3p3pins & ~gpio3p3FreeMask;
	}

	/* Update GPIO3p3 source control and used masks if enabled*/
	if (rxAgcCtrlDualBand->dualBandGpioEnable > 0) {
		/* check if GPIO's are available for the new configuration*/
		gpio3p3UsedMask = 0x0F;
		if ((usedGpio3p3pins & gpio3p3UsedMask) != 0) {
			return talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
						FR_ERR_SETUPDUALBANDRX1AGC_GPIO3P3_IN_USE, retVal, TALACT_ERR_CHECK_PARAM);
		}

		usedGpio3p3pins |= gpio3p3UsedMask;
	}

	/* Set the source control */
	registerValue = (registerValue & ~0x0F ) | ((
				rxAgcCtrlDualBand->dualBandGpioEnable > 0) ?
			RX1_DUALBAND_EXT_LNA_SOURCE_CONTROL : 0x00);
	halError = talSpiWriteByte(device->devHalInfo,
				   FR9009_ADDR_GPIO_3P3V_LOWER_BYTE_SOURCE_CONTROL, registerValue);
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	/* Set GPIO input enables */
	/* - Set OE for all freed GPIO3p3's to 0 (input) */
	/* - Set OE for all used GPIO3p3's to 1 (output) */
	retVal = (talRecoveryActions_t)FR9009_setGpio3v3Oe(device, gpio3p3UsedMask,
			(gpio3p3UsedMask | gpio3p3FreeMask));
	IF_ERR_RETURN(retVal);

	/* Update used and freed GPIO's */
	device->devStateInfo.usedGpio3p3pins = usedGpio3p3pins;

	/* Set upper LNA index */
	halError = talSpiWriteByte(device->devHalInfo,
				   FR9009_ADDR_RX1_AGC_DUALBAND_INDEX_X,
				   rxAgcCtrlDualBand->agcRxDualbandExtTableUpperIndex );
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	/* Set lower LNA index */
	halError = talSpiWriteByte(device->devHalInfo,
				   FR9009_ADDR_RX1_AGC_DUALBAND_INDEX_Y,
				   rxAgcCtrlDualBand->agcRxDualbandExtTableLowerIndex );
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	/* Enable Dec power for the channel. Also clear bitfield to ensure that Dec power calculation is done before compensation */
	registerValue = (rxAgcCtrlDualBand->dualBandGpioEnable > 0) ? 0x03 : 0x00;
	halError = brSpiWriteField(device->devHalInfo,
				    FR9009_ADDR_DDC_DEC_POWER_CONFIG, registerValue, 0x33, 0);
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);

	return retVal;
}

static talRecoveryActions_t talSetupDualBandRx2Agc(fr9009Device_t *device,
		fr9009AgcDualBandCfg_t *rxAgcCtrlDualBand)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	fRHALErr_t halError = FRHAL_OK;
	uint8_t registerValue = 0;
	uint16_t currGpio3p3Oe = 0;
	uint16_t usedGpio3p3pins = 0;
	uint16_t gpio3p3FreeMask = 0;
	uint16_t gpio3p3UsedMask = 0;

	const uint8_t RX2_DUALBAND_EXT_LNA_SOURCE_CONTROL = 0x05;

	/* range check the gain against the max and min expected values */
	if ((rxAgcCtrlDualBand->agcRxDualbandExtTableUpperIndex >
	     device->devStateInfo.gainIndexes.rx2MaxGainIndex)
	    || (rxAgcCtrlDualBand->agcRxDualbandExtTableLowerIndex <
		device->devStateInfo.gainIndexes.rx2MinGainIndex)) {
		return talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
					FR_ERR_SETUPDUALBANDRXAGC_GAIN_OUT_OF_RANGE, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Check current configuration of GPIO 3.3s */
	halError = talSpiReadByte(device->devHalInfo,
				  FR9009_ADDR_GPIO_3P3V_LOWER_BYTE_SOURCE_CONTROL, &registerValue);
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	retVal = (talRecoveryActions_t)FR9009_getGpio3v3Oe(device, &currGpio3p3Oe);
	IF_ERR_RETURN(retVal);

	/* Free GPIO3.3[7:4] if used */
	if ((((registerValue & 0xF0) >> 4) == RX2_DUALBAND_EXT_LNA_SOURCE_CONTROL) &&
	    ((currGpio3p3Oe & 0xF0) == 0xF0)) {
		gpio3p3FreeMask = 0xF0;
		usedGpio3p3pins = device->devStateInfo.usedGpio3p3pins & ~gpio3p3FreeMask;
	}

	/* Update GPIO3p3 source control and used masks if enabled*/
	if (rxAgcCtrlDualBand->dualBandGpioEnable > 0) {
		/* check if GPIO's are available for the new configuration*/
		gpio3p3UsedMask = 0xF0;
		if ((usedGpio3p3pins & gpio3p3UsedMask) != 0) {
			return talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
						FR_ERR_SETUPDUALBANDRX2AGC_GPIO3P3_IN_USE, retVal, TALACT_ERR_CHECK_PARAM);
		}

		usedGpio3p3pins |= gpio3p3UsedMask;
	}

	/* Set the source control */
	registerValue = (registerValue & ~0xF0 ) | (((
				rxAgcCtrlDualBand->dualBandGpioEnable > 0) ?
			RX2_DUALBAND_EXT_LNA_SOURCE_CONTROL : 0x00) << 4);
	halError = talSpiWriteByte(device->devHalInfo,
				   FR9009_ADDR_GPIO_3P3V_LOWER_BYTE_SOURCE_CONTROL, registerValue);
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	/* Set GPIO input enables */
	/* - Set OE for all freed GPIO3p3's to 0 (input) */
	/* - Set OE for all used GPIO3p3's to 1 (output) */
	retVal = (talRecoveryActions_t)FR9009_setGpio3v3Oe(device, gpio3p3UsedMask,
			(gpio3p3UsedMask | gpio3p3FreeMask));
	IF_ERR_RETURN(retVal);

	/* Update used and freed GPIO's */
	device->devStateInfo.usedGpio3p3pins = usedGpio3p3pins;

	/* Set upper LNA index */
	halError = talSpiWriteByte(device->devHalInfo,
				   FR9009_ADDR_RX2_AGC_DUALBAND_INDEX_X,
				   rxAgcCtrlDualBand->agcRxDualbandExtTableUpperIndex );
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	/* Set lower LNA index */
	halError = talSpiWriteByte(device->devHalInfo,
				   FR9009_ADDR_RX2_AGC_DUALBAND_INDEX_Y,
				   rxAgcCtrlDualBand->agcRxDualbandExtTableLowerIndex );
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	/* Enable Dec power for the channel. Also clear bitfield to ensure that Dec power calculation is done before compensation */
	registerValue = (rxAgcCtrlDualBand->dualBandGpioEnable > 0) ? 0x0C : 0x00;
	halError = brSpiWriteField(device->devHalInfo,
				    FR9009_ADDR_DDC_DEC_POWER_CONFIG, registerValue, 0xCC, 0);
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);

	return retVal;
}

uint32_t FR9009_setupDualBandRxAgc( fr9009Device_t *device,
				    fr9009RxChannels_t rxChannel, fr9009AgcDualBandCfg_t *rxAgcCtrlDualBand)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	fRHALErr_t halError = FRHAL_OK;

#if FR9009_VERBOSE
	halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK,
				 "FR9009_setupDualBandRxAgc()\n");
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* Check: Channel */
	if (rxChannel == FR_RXOFF) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
						  FR_ERR_SETUPDUALBANDRXAGC_INV_CHANNEL, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Check: Null pointer */
	if ( rxAgcCtrlDualBand == NULL) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
						  FR_ERR_SETUPDUALBANDRXAGC_NULL_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Check: Power margin */
	if (rxAgcCtrlDualBand->agcDualbandPwrMargin > 0x1F) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
						  FR_ERR_SETUPDUALBANDRXAGC_INV_PWRMARGIN, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Check: Dec power measurement duration */
	if (rxAgcCtrlDualBand->decPowerDdcMeasurementDuration > 0x1F) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
						  FR_ERR_SETUPDUALBANDRXAGC_INV_DECPWR, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* range check the gain table indices upper > lower */
	if ((rxAgcCtrlDualBand->agcRxDualbandExtTableUpperIndex <=
	     rxAgcCtrlDualBand->agcRxDualbandExtTableLowerIndex)) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
						  FR_ERR_SETUPDUALBANDRXAGC_GAIN_RANGE_MISMATCH, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Write Rx1 specific data */
	if ((rxChannel == FR_RX1) ||
	    (rxChannel == FR_RX1RX2)) {
		retVal = talSetupDualBandRx1Agc(device, rxAgcCtrlDualBand);
	}

	/* Write Rx2 specific data */
	if ((rxChannel == FR_RX2) ||
	    (rxChannel == FR_RX1RX2)) {
		retVal = talSetupDualBandRx2Agc(device, rxAgcCtrlDualBand);
	}

	/* Enable dualband mode */
	halError = talSpiWriteByte(device->devHalInfo, FR9009_ADDR_SLOWLOOP_CONFIG2,
				   (uint8_t)((rxAgcCtrlDualBand->agcDualBandEnable > 0) ? 1 : 0));
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	/* Power margin */
	halError = talSpiWriteByte(device->devHalInfo,
				   FR9009_ADDR_AGC_DUALBAND_TOFR_PWR_MARGIN,
				   rxAgcCtrlDualBand->agcDualbandPwrMargin);
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	/* Band power margin */
	halError = talSpiWriteByte(device->devHalInfo,
				   FR9009_ADDR_AGC_DUALBAND_BAND_PWR_MARGIN,
				   rxAgcCtrlDualBand->agcDualbandLnaStep);
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	/* High LNA threshold */
	halError = talSpiWriteByte(device->devHalInfo,
				   FR9009_ADDR_AGC_DUALBAND_HIGH_LNA_THRESHOLD,
				   rxAgcCtrlDualBand->agcDualbandHighLnaThreshold);
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	/* Low LNA threshold */
	halError = talSpiWriteByte(device->devHalInfo,
				   FR9009_ADDR_AGC_DUALBAND_LOW_LNA_THRESHOLD,
				   rxAgcCtrlDualBand->agcDualbandLowLnaThreshold);
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	/* Dec power measurement duration */
	/* Also enable measurement control through AGC, and improved dynamic range of measured power */
	halError = talSpiWriteByte(device->devHalInfo, FR9009_ADDR_DDC_DEC_POWER_MEAS,
				   ((rxAgcCtrlDualBand->decPowerDdcMeasurementDuration & 0x1F) | 0x60));
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);

	return (uint32_t)retVal;
}

uint32_t FR9009_getDualBandLnaControls(fr9009Device_t *device,
				       fr9009RxChannels_t rxChannel,
				       fr9009DualBandLnaControls_t *rxDualBandLnaControls)
{
	uint16_t lowerBandAddr = 0;
	uint16_t upperBandAddr = 0;
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	fRHALErr_t halError = FRHAL_OK;

#if FR9009_VERBOSE
	halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK,
				 "FR9009_getDualBandLnaControls()\n");
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* Read current value of Gain inc/dec and enable */
	switch (rxChannel) {
	case FR_RX1:
		lowerBandAddr = FR9009_ADDR_GAIN_TABLE_DATA_OUTPUT_DUALBAND_CH1_BAND_A;
		upperBandAddr = FR9009_ADDR_GAIN_TABLE_DATA_OUTPUT_DUALBAND_CH1_BAND_B;
		break;

	case FR_RX2:
		lowerBandAddr = FR9009_ADDR_GAIN_TABLE_DATA_OUTPUT_DUALBAND_CH2_BAND_A;
		upperBandAddr = FR9009_ADDR_GAIN_TABLE_DATA_OUTPUT_DUALBAND_CH2_BAND_B;
		break;

	default: {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
						  FR_ERR_GETDUALBANDLNA_INV_CHANNEL, retVal, TALACT_ERR_CHECK_PARAM);
	}
	}

	/*Check passed pointers for NULL */
	if (rxDualBandLnaControls == NULL) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
						  FR_ERR_GETDUALBANDLNA_NULL_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Disable gain table read - Otherwise, the readback will read a forced index */
	halError = brSpiWriteField(device->devHalInfo,
				    FR9009_ADDR_GAIN_TABLE_CONFIGURATION, 0x0, 0x04, 2);
	retVal = talApiErrHandler(device,FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	/* Read control word for the current LNA index */
	halError = brSpiReadField(device->devHalInfo, lowerBandAddr,
				   &rxDualBandLnaControls->rxLowerBandLnaControl, 0x03, 0);
	retVal = talApiErrHandler(device,FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	halError = brSpiReadField(device->devHalInfo, upperBandAddr,
				   &rxDualBandLnaControls->rxUpperBandLnaControl, 0x03, 0);
	retVal = talApiErrHandler(device,FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	return (uint32_t)retVal;
}
#endif
uint32_t FR9009_setRxAgcMinMaxGainIndex(fr9009Device_t *device, fr9009RxChannels_t rxChannel, uint8_t maxGainIndex, uint8_t minGainIndex)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	fRHALErr_t halError = FRHAL_OK;
	uint32_t regdat = 0;

#if FR9009_VERBOSE
	halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK, "FR9009_setRxAgcMinMaxGainIndex()\n");
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal, TALACT_WARN_RESET_LOG);
#endif

	/* Ensure that Max Gain Index is always greater than Min Gain Index */
	if(minGainIndex >= maxGainIndex){
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_INV_AGC_RX_MIN_GAIN_GRT_THAN_MAX_GAIN_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if((rxChannel == FR_RX1) || (rxChannel == FR_RX1RX2)){
		/*Ensure that requested min and max gain indexes are within the range supported by the current chip config*/
		if ((maxGainIndex > device->devStateInfo.rxGainCtrl.rx1MaxGainIndex) ||
		    (minGainIndex < device->devStateInfo.rxGainCtrl.rx1MinGainIndex)){
			return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_INV_AGC_RX_MIN_MAX_GAIN_PARM, retVal, TALACT_ERR_CHECK_PARAM);
		}
		/* Writing Rx1 max and min gain indices to the SPI registers*/
		regdat = maxGainIndex | (minGainIndex << 8);
		halError = FR9009_armSpiCmd_writeField(device, FR9009_ADDR_MAX_MIN_GAIN_INDEX_RX1, regdat, 0xFFFF, 0);
		IF_ERR_RETURN_U32(retVal);
	}

	if((rxChannel == FR_RX2) || (rxChannel == FR_RX1RX2)){
		/*Ensure that requested min and max gain indexes are within the range supported by the current chip config*/
		if ((maxGainIndex > device->devStateInfo.rxGainCtrl.rx2MaxGainIndex) ||
		    (minGainIndex < device->devStateInfo.rxGainCtrl.rx2MinGainIndex)){
			return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_INV_AGC_RX_MIN_MAX_GAIN_PARM, retVal, TALACT_ERR_CHECK_PARAM);
		}

		/* Writing Rx2 max and min gain indices to the SPI registers*/
		regdat = maxGainIndex | (minGainIndex << 8);
		halError = FR9009_armSpiCmd_writeField(device, FR9009_ADDR_MAX_MIN_GAIN_INDEX_RX2, regdat, 0xFFFF, 0);
		IF_ERR_RETURN_U32(retVal);
	}

	return (uint32_t)retVal;
}

uint32_t FR9009_resetRxAgc(fr9009Device_t *device)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	fRHALErr_t halError = FRHAL_OK;
	static const uint8_t AGC_RESET = 1;
	static const uint8_t AGC_RESET_CLEAR = 0;
	static const uint32_t AGC_RESET_MASK = 0x20000;
	static const uint8_t AGC_START_BIT = 17;

#if FR9009_VERBOSE
	halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK,
				 "FR9009_resetRxAgc()\n");
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* Soft Reset AGC */
	halError = FR9009_armSpiCmd_writeField(device, FR9009_ADDR_SLOWLOOP_CONFIG,
				    AGC_RESET, AGC_RESET_MASK, AGC_START_BIT);
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/*Release AGC Reset*/
	halError = brSpiWriteField(device->devHalInfo, FR9009_ADDR_SLOWLOOP_CONFIG,
				    AGC_RESET_CLEAR, AGC_RESET_MASK, AGC_START_BIT);
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	return (uint32_t)retVal;
}
