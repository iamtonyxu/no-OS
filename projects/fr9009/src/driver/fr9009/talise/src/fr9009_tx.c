/**
 * \file fr9009_tx.c
 * \brief Contains functions to support FR9009 Tx data path control
 *
 * Copyright 2022 BriRadio.
 * Released under the FR9009 API license, for more information see the "LICENSE.txt" file in this zip file.
 */

#include "fr9009_tx.h"
#include "fr9009_reg_addr_macros.h"
#include "fr9009_arm_macros.h"
#include "fr9009_hal.h"
#include "fr9009_user.h"
#include "fr9009_error.h"
#include "fr9009.h"
#include "fr9009_gpio.h"
#include "fr9009_arm.h"
#include "math.h"

uint32_t FR9009_setTxAttenuation(fr9009Device_t *device, fr9009TxChannels_t txChannel, uint16_t txAttenuation_mdB)
{
//spi transmitter 0x58
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	fRHALErr_t halError = FRHAL_OK;

	static const uint32_t txMax = 41951;

#if FR9009_VERBOSE
	halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK, "FR9009_setTxAttenuation()\n");
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal, TALACT_WARN_RESET_LOG);
#endif

	/* Check for valid txChannel */
	if ((txChannel != FR_TX1) && (txChannel != FR_TX2)) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_SETTXATTEN_INV_TXCHANNEL, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* check input parameter is in valid range */
	if (txAttenuation_mdB >= txMax) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_SETTXATTEN_INV_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}	
	retVal = FR9009_armSpiCmd_tx_ch_ATT_set(device, txChannel, txAttenuation_mdB);
	return (uint32_t)retVal;
}

uint32_t FR9009_setTxDigAttenuation(fr9009Device_t *device, fr9009TxChannels_t txChannel, uint16_t txAttenuation_mdB)
{
#define NORM_GAIN_BASE_REG 0xC00
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	fRHALErr_t halError = FRHAL_OK;
	uint32_t norm_reg = NORM_GAIN_BASE_REG;
	uint32_t set_reg[2] = {NORM_GAIN_BASE_REG, NORM_GAIN_BASE_REG};
#if FR9009_VERBOSE
	halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK, "FR9009_setTxAttenuation()\n");
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal, TALACT_WARN_RESET_LOG);
#endif

	/* Check for valid txChannel */
	if ((txChannel != FR_TX1) && (txChannel != FR_TX2)) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_SETTXATTEN_INV_TXCHANNEL, retVal, TALACT_ERR_CHECK_PARAM);
	}

	set_reg[0] = (uint32_t)(powf(10,-((float)txAttenuation_mdB/20000))*norm_reg);
	set_reg[1] = set_reg[0];
	retVal = FR9009_armSpiCmd_writeWords(device, APB_TX_DGAIN_BASEADDR + 0xC + ((txChannel - FR_TX1) * 8), set_reg, 2);
	return (uint32_t)retVal;
}

uint32_t FR9009_getTxAttenuation(fr9009Device_t *device, fr9009TxChannels_t txChannel, uint16_t *txAttenuation_mdB)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	fRHALErr_t halError = FRHAL_OK;
	uint32_t regData = 0;
#if FR9009_VERBOSE
	halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK, "FR9009_getTxAttenuation()\n");
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal, TALACT_WARN_RESET_LOG);
#endif
	/* check txAttenuation_mdB for null pointer */
	if (txAttenuation_mdB == NULL) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_GETTXATTEN_NULL_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}
	retVal = FR9009_armSpiCmd_tx_ch_ATT_get(device, txChannel, &regData);
	*txAttenuation_mdB = (uint16_t)regData;
	return (uint32_t)retVal;
}
#if 0
uint32_t FR9009_setDacFullScale(fr9009Device_t *device,
				fr9009DacFullScale_t dacFullScale)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	fRHALErr_t halError = FRHAL_OK;
	uint8_t dacFsBoost = 0;
	uint8_t isArmRunning = 0;
	static const uint8_t TXDAC_GAIN_MASK = 0x1F;

	if ((dacFullScale != FR_DACFS_0DB) &&
	    (dacFullScale != FR_DACFS_3DB)) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
						  FR_ERR_INV_DAC_FULLSCALE_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	halError = brSpiReadField(device->devHalInfo, FR9009_ADDR_ARM_CTL_1,
				   &isArmRunning, 0x01, 0);
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	if (isArmRunning == 1) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_API_FAIL,
						  FR_ERR_DAC_FULLSCALE_INVARMSTATE, retVal, TALACT_ERR_RESET_FULL);
	}

	dacFsBoost = ((uint8_t)dacFullScale) & TXDAC_GAIN_MASK;

	/* Tx1 I data */
	halError = talSpiWriteByte(device->devHalInfo, FR9009_ADDR_TXDAC1_GAIN_I,
				   dacFsBoost );
	retVal = talApiErrHandler(device,FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Tx1 Q data */
	halError = talSpiWriteByte(device->devHalInfo, FR9009_ADDR_TXDAC1_GAIN_Q,
				   dacFsBoost );
	retVal = talApiErrHandler(device,FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Tx2 I data */
	halError = talSpiWriteByte(device->devHalInfo, FR9009_ADDR_TXDAC2_GAIN_I,
				   dacFsBoost );
	retVal = talApiErrHandler(device,FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Tx2 Q data */
	halError = talSpiWriteByte(device->devHalInfo, FR9009_ADDR_TXDAC2_GAIN_Q,
				   dacFsBoost );
	retVal = talApiErrHandler(device,FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	return (uint32_t)retVal;
}
#endif

uint32_t FR9009_enableTxNco(fr9009Device_t *device, fr9009TxNcoTestToneCfg_t *txNcoTestToneCfg)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	fRHALErr_t halError = FRHAL_OK;
	int32_t txInputRateDiv2_kHz = 0;
	int32_t tx1Ncopower = 0;
	int32_t tx2Ncopower = 0;
	uint32_t tx_ch = 0x3;

#if FR9009_VERBOSE
	halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK, "FR9009_enableTxNco()\n");
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal,  TALACT_WARN_RESET_LOG);
#endif

	/* Check valid Tx profile */
	if ((device->devStateInfo.profilesValid & TX_PROFILE_VALID) == 0) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_ENABLETXNCO_INV_PROFILE, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Check for NULL pointer */
	if ( txNcoTestToneCfg == NULL) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_ENABLETXNCO_NULL_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}
	
	if (txNcoTestToneCfg->enable > 0) {
		txInputRateDiv2_kHz = device->devStateInfo.txInputRate_kHz >> 1;
		/* Check Tx1 NCO freq range between Fs/2 and -Fs/2 */
		if ((txNcoTestToneCfg->Tone1Freq_kHz > (int32_t)txInputRateDiv2_kHz) || (txNcoTestToneCfg->Tone2Freq_kHz < -((int32_t)txInputRateDiv2_kHz))) {
			return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_ENABLETXNCO_INV_TX1_FREQ, retVal, TALACT_ERR_CHECK_PARAM);
		}

		/* Check Tx2 NCO freq range between Fs/2 and -Fs/2 */
		if ((txNcoTestToneCfg->Tone1Freq_kHz > (int32_t)txInputRateDiv2_kHz) || (txNcoTestToneCfg->Tone2Freq_kHz < -((int32_t)txInputRateDiv2_kHz))) {
			return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_ENABLETXNCO_INV_TX2_FREQ, retVal, TALACT_ERR_CHECK_PARAM);
		}
		if(txNcoTestToneCfg->enable & 0x1){
			/* Tx1 */
			tx1Ncopower = (uint32_t)(powf(10,((float)txNcoTestToneCfg->Tx1Power/20))*0x8000);
			if(tx1Ncopower >= 0x8000){
			    tx1Ncopower = 0x7FFF;
			}
		}
		if(txNcoTestToneCfg->enable & 0x2){
			/* Tx2 */
			tx2Ncopower = (uint32_t)(powf(10,((float)txNcoTestToneCfg->Tx2Power/20))*0x8000);
			if(tx2Ncopower >= 0x8000){
			    tx2Ncopower = 0x7FFF;
			}
		}
		retVal = FR9009_armSpiCmd_tx_ncopower_set(device, tx_ch, tx1Ncopower, tx2Ncopower);
		IF_ERR_RETURN_U32(retVal);
		retVal = FR9009_armSpiCmd_tx_NCO_set(device,txNcoTestToneCfg->enable&0x3, tx_ch, txNcoTestToneCfg->Tone1Freq_kHz,txNcoTestToneCfg->Tone2Freq_kHz);
		IF_ERR_RETURN_U32(retVal);
	} else {
		/* Disable NCO test mode */
		retVal = FR9009_armSpiCmd_tx_NCO_set(device,0,0, txNcoTestToneCfg->Tone1Freq_kHz,txNcoTestToneCfg->Tone2Freq_kHz);
		IF_ERR_RETURN_U32(retVal);
		//#Enable DP_TX_FIX_EN
		retVal = FR9009_armSpiCmd_tx_ncopower_set(device, 0, 0, 0);
		IF_ERR_RETURN_U32(retVal);
	}

	return (uint32_t)retVal;
}

uint32_t FR9009_setTxAttenCtrlPin(fr9009Device_t *device, fr9009TxChannels_t txChannel, fr9009TxAttenCtrlPin_t *txAttenCtrlPin)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	fRHALErr_t halError = FRHAL_OK;
	fr9009GpioShortConfig_t attpin_config;

	static const uint8_t TX_INCDEC_MASK = 0x1F;

#if FR9009_VERBOSE
	halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK, "FR9009_setTxAttenCtrlPin()\n");
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal, TALACT_WARN_RESET_LOG);
#endif

	/*Check passed pointers for NULL */
	if (txAttenCtrlPin == NULL) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_SETTXATTENCTRLPIN_NULL_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Check out of range */
	if ( txAttenCtrlPin->stepSize > TX_INCDEC_MASK) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_SETTXATTENCTRLPIN_INV_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}
	if((txChannel != FR_TX1) && (txChannel != FR_TX2)){
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_SETTXATTENCTRLPIN_INV_CHANNEL, retVal, TALACT_ERR_CHECK_PARAM);
	}
	retVal = FR9009_setGpioOe(device, 0, (0x1 << txAttenCtrlPin->txAttenIncPin) | (0x1 << txAttenCtrlPin->txAttenDecPin));
	IF_ERR_RETURN_U32(retVal);	
	retVal = FR9009_setGpioSourceCtrl(device, FR_GPIO_TX_ATT_CONTROL_MODE_2 << ((txAttenCtrlPin->txAttenIncPin / 4)*4));
	IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_getTxAttnSel(device, &attpin_config);
	IF_ERR_RETURN_U32(retVal);
	if(txChannel == FR_TX1){
		attpin_config.p0.gpioPinSel = txAttenCtrlPin->txAttenIncPin;
		attpin_config.p0.enable = txAttenCtrlPin->enable > 0 ? 1 : 0;
		attpin_config.p1.gpioPinSel = txAttenCtrlPin->txAttenDecPin;
		attpin_config.p1.enable = txAttenCtrlPin->enable > 0 ? 1 : 0;
	}else{
		attpin_config.p2.gpioPinSel = txAttenCtrlPin->txAttenIncPin;
		attpin_config.p2.enable = txAttenCtrlPin->enable > 0 ? 1 : 0;
		attpin_config.p3.gpioPinSel = txAttenCtrlPin->txAttenDecPin;
		attpin_config.p3.enable = txAttenCtrlPin->enable > 0 ? 1 : 0;		
	}
	retVal = FR9009_setTxAttnSel(device, &attpin_config);
	IF_ERR_RETURN_U32(retVal);
	
	return (uint32_t)retVal;
}

uint32_t FR9009_getTxAttenCtrlPin(fr9009Device_t *device,  fr9009TxChannels_t txChannel, fr9009TxAttenCtrlPin_t *txAttenCtrlPin)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	fRHALErr_t halError = FRHAL_OK;
	// uint32_t regdat = 0;
	// int i =  0;
	// uint8_t fieldValue;
	fr9009GpioShortConfig_t attpin_config;

#if FR9009_VERBOSE
	halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK, "FR9009_getTxAttenCtrlPin()\n");
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal, TALACT_WARN_RESET_LOG);
#endif

	/* Check channel */
	if ((txChannel != FR_TX1) &&
	    (txChannel != FR_TX2)) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_GETTXATTENCTRLPIN_INV_CHANNEL, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/*Check passed pointers for NULL */
	if (txAttenCtrlPin == NULL) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_GETTXATTENCTRLPIN_NULL_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}
	retVal = FR9009_getTxAttnSel(device, &attpin_config);
	IF_ERR_RETURN_U32(retVal);
	if(txChannel == FR_TX1){
		txAttenCtrlPin->txAttenIncPin = attpin_config.p0.gpioPinSel;
		txAttenCtrlPin->enable = attpin_config.p0.enable;
		txAttenCtrlPin->txAttenDecPin = attpin_config.p1.gpioPinSel;
		txAttenCtrlPin->enable = attpin_config.p1.enable;
	}else{
		txAttenCtrlPin->txAttenIncPin = attpin_config.p2.gpioPinSel;
		txAttenCtrlPin->enable = attpin_config.p2.enable;
		txAttenCtrlPin->txAttenDecPin = attpin_config.p3.gpioPinSel;
		txAttenCtrlPin->enable = attpin_config.p3.enable;	
	}

	// /* Get value of GPIO selects */
	// retVal = FR9009_getGpioSourceCtrl(device, &regdat);
	// IF_ERR_RETURN_U32(retVal);
	// for(i = 0; i < 5; i++)
	// {
	// 	if(((regdat >> (i * 4)) & 0xF) == FR_GPIO_TX_ATT_CONTROL_MODE_2)
	// 		break;
	// }
	// if(i >= 5){
	// 	txAttenCtrlPin->txAttenIncPin = 0;
	// 	txAttenCtrlPin->txAttenDecPin = 0;
	// 	return retVal;
	// }
	// regdat = ((regdat >> (i * 4)) & 0xF);
	// /* Value of txAttenIncPin */
	// fieldValue = (txChannel == FR_TX1) ? (regdat & 0x03) : ((
	// 			regdat >> 4) & 0x03);
	// txAttenCtrlPin->txAttenIncPin = (txChannel == FR_TX1) ? ((
	// 					fieldValue == 0x00) ? FR_GPIO_04 : ((fieldValue == 0x01) ? FR_GPIO_12 :
	// 							FR_GPIO_INVALID))
	// 				: ((fieldValue == 0x00) ? FR_GPIO_06 : ((fieldValue == 0x01) ? FR_GPIO_14 :
	// 						FR_GPIO_INVALID));

	// /* Value of txAttenDecPin */
	// fieldValue = (txChannel == FR_TX1) ? ((regdat >> 2) & 0x03) : ((
	// 			regdat >> 6) & 0x03);
	// txAttenCtrlPin->txAttenDecPin = (txChannel == FR_TX1) ? ((
	// 					fieldValue == 0x00) ? FR_GPIO_05 : ((fieldValue == 0x01) ? FR_GPIO_13 :
	// 							FR_GPIO_INVALID))
	// 				: ((fieldValue == 0x00) ? FR_GPIO_07 : ((fieldValue == 0x01) ? FR_GPIO_15 :
	// 						FR_GPIO_INVALID));

	return (uint32_t)retVal;
}
#if 0
uint32_t FR9009_setPaProtectionCfg(fr9009Device_t *device,
				   fr9009TxPaProtectCfg_t *txPaProtectCfg)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	fRHALErr_t halError = FRHAL_OK;
	uint8_t peakCount = 0;
	uint8_t tx1PeakThreshold = 0;
	uint8_t tx2PeakThreshold = 0;
	uint8_t paProtectCfg = 0;
	uint8_t peakCountMax = 31;

	static const uint8_t ENABLE_PA_PROTECTION_MEASUREMENT = 0x01;
	static const uint8_t ENABLE_PEAK_MEASUREMENT = 0x01;
	static const uint8_t ERROR_FLAGS_STICKY = 0x40;
	static const uint8_t TX_ATTEN_REDUCTION_MAX = 127;
	static const uint16_t POWER_THRESHOLD_MAX = 8191;
	static const uint8_t AVG_DURATION_MAX = 14;
	static const uint8_t SILICON_REV_C0 = 0xC0;

#if FR9009_VERBOSE
	halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK,
				 "FR9009_setPaProtectionCfg()\n");
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* If Silicon Revision is less than C0 - Max Peak Count is 30 */
	if (device->devStateInfo.deviceSiRev < SILICON_REV_C0) {
		/*Update the max peak count for silicon revisions lower than C0 to 30*/
		peakCountMax = 30;
	}

	if (txPaProtectCfg == NULL) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
						  FR_ERR_SETPAPRO_NULL_PARAM, retVal,
						  TALACT_ERR_CHECK_PARAM);
	}

	if (txPaProtectCfg->avgDuration > AVG_DURATION_MAX) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
						  FR_ERR_SETPAPRO_INV_AVGDURATION, retVal,
						  TALACT_ERR_CHECK_PARAM);
	}

	if (txPaProtectCfg->peakCount > peakCountMax) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
						  FR_ERR_SETPAPROT_INV_PEAKCNT, retVal,
						  TALACT_ERR_CHECK_PARAM);
	}

	if (txPaProtectCfg->txAttenStep > TX_ATTEN_REDUCTION_MAX) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
						  FR_ERR_SETPAPROT_INV_TXATTENSTEP, retVal,
						  TALACT_ERR_CHECK_PARAM);
	}

	if ((txPaProtectCfg->tx1PowerThreshold == 0) ||
	    (txPaProtectCfg->tx1PowerThreshold > POWER_THRESHOLD_MAX)) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
						  FR_ERR_SETPAPROT_INV_TX1THRESH, retVal,
						  TALACT_ERR_CHECK_PARAM);
	}

	if ((txPaProtectCfg->tx2PowerThreshold == 0) ||
	    (txPaProtectCfg->tx2PowerThreshold > POWER_THRESHOLD_MAX)) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
						  FR_ERR_SETPAPROT_INV_TX2THRESH, retVal,
						  TALACT_ERR_CHECK_PARAM);
	}

	if (txPaProtectCfg->tx1PeakThreshold == 0) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
						  FR_ERR_SETPAPROT_INV_TX1PEAKTHRESH, retVal,
						  TALACT_ERR_CHECK_PARAM);
	}

	if (txPaProtectCfg->tx2PeakThreshold == 0) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
						  FR_ERR_SETPAPROT_INV_TX2PEAKTHRESH, retVal,
						  TALACT_ERR_CHECK_PARAM);
	}

	/* Disable PA Protection before changing settings paProtectCfg[0]=0 */
	/* Set Tx Power average duration paProtectCfg[4:1] */
	/* Allow error flags to be stick (set until cleared by SPI clear bit paProtectCfg[6]=1 */
	paProtectCfg = ((txPaProtectCfg->avgDuration << 1) | ERROR_FLAGS_STICKY);
	halError = talSpiWriteByte(device->devHalInfo,
				   FR9009_ADDR_PA_PROTECTION_CONFIGURATION, paProtectCfg);
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = brSpiWriteField(device->devHalInfo,
				    FR9009_ADDR_PA_PROTECTION_ATTEN_CONTROL, txPaProtectCfg->txAttenStep, 0xFE, 1);
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Set Tx1 average power threshold[7:0] */
	halError = talSpiWriteByte(device->devHalInfo,
				   FR9009_ADDR_PA_PROTECTION_THRESHOLD_0,
				   (txPaProtectCfg->tx1PowerThreshold & 0xFF));
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Set Tx1 average power threshold[12:8] */
	halError = talSpiWriteByte(device->devHalInfo,
				   FR9009_ADDR_PA_PROTECTION_THRESHOLD_1,
				   ((txPaProtectCfg->tx1PowerThreshold >> 8) & 0xFF));
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Set Tx2 average power threshold[7:0] */
	halError = talSpiWriteByte(device->devHalInfo,
				   FR9009_ADDR_PA_PROTECTION_THRESHOLD_2,
				   (txPaProtectCfg->tx2PowerThreshold & 0xFF));
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Set Tx2 average power threshold[12:8] */
	halError = talSpiWriteByte(device->devHalInfo,
				   FR9009_ADDR_PA_PROTECTION_THRESHOLD_3,
				   ((txPaProtectCfg->tx2PowerThreshold >> 8) & 0xFF));
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Set Peak mode settings */
	if (txPaProtectCfg->peakCount == 0) {
		peakCount = 0;
		tx1PeakThreshold = 0;
		tx2PeakThreshold = 0;
	} else {
		peakCount = ((txPaProtectCfg->peakCount << 1) | ENABLE_PEAK_MEASUREMENT);
		tx1PeakThreshold = txPaProtectCfg->tx1PeakThreshold;
		tx2PeakThreshold = txPaProtectCfg->tx2PeakThreshold;
	}

	halError = talSpiWriteByte(device->devHalInfo,
				   FR9009_ADDR_PA_PROTECTION_PEAK_THRESHOLD_CH1, tx1PeakThreshold);
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   FR9009_ADDR_PA_PROTECTION_PEAK_THRESHOLD_CH2, tx2PeakThreshold);
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Set peak count and peak enable bit after peak thresholds */
	halError = talSpiWriteByte(device->devHalInfo,
				   FR9009_ADDR_PA_PROTECTION_PEAK_COUNT, peakCount);
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Enable PA Power Protection measurement */
	halError = brSpiWriteField(device->devHalInfo,
				    FR9009_ADDR_PA_PROTECTION_CONFIGURATION, ENABLE_PA_PROTECTION_MEASUREMENT, 0x01,
				    0);
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Clear PA Protection Error flags */
	halError = brSpiWriteField(device->devHalInfo,
				    FR9009_ADDR_PA_PROTECTION_CONFIGURATION, 1, 0x80, 7);
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	return (uint32_t)retVal;
}

uint32_t FR9009_getPaProtectionCfg(fr9009Device_t *device,
				   fr9009TxPaProtectCfg_t *txPaProtectCfg)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	fRHALErr_t halError = FRHAL_OK;
	uint8_t tx1PowerThresholdLsb = 0;
	uint8_t tx1PowerThresholdMsb = 0;
	uint8_t tx2PowerThresholdLsb = 0;
	uint8_t tx2PowerThresholdMsb = 0;

#if FR9009_VERBOSE
	halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK,
				 "FR9009_getPaProtectionCfg()\n");
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	if (txPaProtectCfg == NULL) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
						  FR_ERR_GETPAPRO_NULL_PARAM, retVal,
						  TALACT_ERR_CHECK_PARAM);
	}

	halError = brSpiReadField(device->devHalInfo,
				   FR9009_ADDR_PA_PROTECTION_CONFIGURATION, &txPaProtectCfg->avgDuration, 0x1E, 1);
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = brSpiReadField(device->devHalInfo,
				   FR9009_ADDR_PA_PROTECTION_ATTEN_CONTROL, &txPaProtectCfg->txAttenStep, 0xFE, 1);
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiReadByte(device->devHalInfo,
				  FR9009_ADDR_PA_PROTECTION_THRESHOLD_0, &tx1PowerThresholdLsb);
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = brSpiReadField(device->devHalInfo,
				   FR9009_ADDR_PA_PROTECTION_THRESHOLD_1, &tx1PowerThresholdMsb, 0x1F, 0);
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	txPaProtectCfg->tx1PowerThreshold = ((uint16_t)tx1PowerThresholdMsb << 8) |
					    (uint16_t)tx1PowerThresholdLsb;

	halError = talSpiReadByte(device->devHalInfo,
				  FR9009_ADDR_PA_PROTECTION_THRESHOLD_2, &tx2PowerThresholdLsb);
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = brSpiReadField(device->devHalInfo,
				   FR9009_ADDR_PA_PROTECTION_THRESHOLD_3, &tx2PowerThresholdMsb, 0x1F, 0);
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	txPaProtectCfg->tx2PowerThreshold = ((uint16_t)tx2PowerThresholdMsb << 8) |
					    (uint16_t)tx2PowerThresholdLsb;

	halError = brSpiReadField(device->devHalInfo,
				   FR9009_ADDR_PA_PROTECTION_PEAK_COUNT, &txPaProtectCfg->peakCount, 0x3E, 1);
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiReadByte(device->devHalInfo,
				  FR9009_ADDR_PA_PROTECTION_PEAK_THRESHOLD_CH1,
				  &txPaProtectCfg->tx1PeakThreshold);
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiReadByte(device->devHalInfo,
				  FR9009_ADDR_PA_PROTECTION_PEAK_THRESHOLD_CH2,
				  &txPaProtectCfg->tx2PeakThreshold);
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	return (uint32_t)retVal;
}

uint32_t FR9009_enablePaProtection(fr9009Device_t *device/*, uint8_t enableTxAttenCtrl*/)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	fRHALErr_t halError = FRHAL_OK;
	uint8_t enTxAttenCtrl = 0;

#if FR9009_VERBOSE
	halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK,
				 "FR9009_enablePaProtection()\n");
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* TODO: Currently do not allow enabling this feature due to a hardware
	 * issue found.  Calling this function will not enable control of TxAttenuation when
	 * the PA Protection error flag asserts.  BBIC will need to take action to reduce
	 * Tx sample power.
	 */

	/* enTxAttenCtrl = (enableTxAttenCtrl > 0) ? 1 : 0; */
	enTxAttenCtrl = 0;

	halError = brSpiWriteField(device->devHalInfo,
				    FR9009_ADDR_PA_PROTECTION_ATTEN_CONTROL, enTxAttenCtrl, 0x01, 0);
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	return (uint32_t)retVal;
}

uint32_t FR9009_getTxSamplePower(fr9009Device_t *device,
				 fr9009TxChannels_t txChannel, uint16_t *channelPower)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	fRHALErr_t halError = FRHAL_OK;
	uint8_t paProtectChannelSel = 0;
	uint8_t chPowerLsb = 0;
	uint8_t chPowerMsb = 0;

#if FR9009_VERBOSE
	halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK,
				 "FR9009_getTxSamplePower()\n");
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	if (channelPower == NULL) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
						  FR_ERR_GETTXSAMPLEPWR_NULL_PARAM, retVal,
						  TALACT_ERR_CHECK_PARAM);
	}

	switch(txChannel) {
	case FR_TX1:
		paProtectChannelSel = 0;
		break;

	case FR_TX2:
		paProtectChannelSel = 1;
		break;

	default:
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
						  FR_ERR_GETTXSAMPLEPWR_INV_TXREADCHAN, retVal,
						  TALACT_ERR_CHECK_PARAM);
	}

	/* Set channel for power read back */
	halError = brSpiWriteField(device->devHalInfo,
				    FR9009_ADDR_PA_PROTECTION_CONFIGURATION, paProtectChannelSel, 0x20, 5);
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Write power readback reg to force the value to update (write strobe) */
	halError = talSpiWriteByte(device->devHalInfo,
				   FR9009_ADDR_PA_PROTECTION_POWER_0, 0x00);
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Read back DAC power for selected channel */
	halError = talSpiReadByte(device->devHalInfo, FR9009_ADDR_PA_PROTECTION_POWER_0,
				  &chPowerLsb);
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = talSpiReadByte(device->devHalInfo, FR9009_ADDR_PA_PROTECTION_POWER_1,
				  &chPowerMsb);
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	*channelPower = ((uint16_t)chPowerLsb | (uint16_t)(chPowerMsb << 8));

	return (uint32_t)retVal;
}

uint32_t FR9009_getPaProtectErrorFlags(fr9009Device_t *device,
				       uint8_t *errorFlags)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	fRHALErr_t halError = FRHAL_OK;

#if FR9009_VERBOSE
	halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK,
				 "FR9009_getPaProtectErrorFlags()\n");
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	if (errorFlags == NULL) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
						  FR_ERR_GETPAERRFLAGS_NULL_PARAM, retVal,
						  TALACT_ERR_CHECK_PARAM);
	}

	/* Read back PA Protection Error flags */
	halError = brSpiReadField(device->devHalInfo, FR9009_ADDR_PA_PROTECTION_ERROR,
				   errorFlags, 0x03, 0);
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	return (uint32_t)retVal;
}

uint32_t FR9009_clearPaProtectErrorFlags(fr9009Device_t *device)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	fRHALErr_t halError = FRHAL_OK;

#if FR9009_VERBOSE
	halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK,
				 "FR9009_clearPaProtectErrorFlags()\n");
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* Clear PA Protection Error flags */
	halError = brSpiWriteField(device->devHalInfo,
				    FR9009_ADDR_PA_PROTECTION_CONFIGURATION, 1, 0x80, 7);
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	return (uint32_t)retVal;
}
#endif

uint32_t fr9009_setTxIfconv(fr9009Device_t *device, fr9009TxChannels_t ch, uint8_t hb_bypass, fr9009IfConvCfg_t* pIfconvCfg)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
    //set tx if convert 
    uint8_t bandsel = pIfconvCfg->bandSel;// select sum A & B 
	uint32_t dpclk_hz = device->devStateInfo.clocks.hsDigClkDiv2_Hz >> 1; //datapath clk
    // uint8_t hb_bypass = 0;
    uint8_t fe_bypass = (pIfconvCfg->bandAFe.freq_Hz == 0) && (pIfconvCfg->bandBFe.freq_Hz == 0) ? 1 : 0;
    uint8_t be_bypass = (pIfconvCfg->bandABe.freq_Hz == 0) && (pIfconvCfg->bandBBe.freq_Hz == 0) ? 1 : 0;
    int64_t fe_word_A = pIfconvCfg->bandAFe.freq_Hz * 4294967296LL / dpclk_hz ;
    int64_t be_word_A = pIfconvCfg->bandABe.freq_Hz * 4294967296LL / dpclk_hz;
    int64_t fe_word_B = pIfconvCfg->bandBFe.freq_Hz * 4294967296LL / dpclk_hz;
    int64_t be_word_B = pIfconvCfg->bandBBe.freq_Hz * 4294967296LL / dpclk_hz;
    uint32_t mask = 0x1F00*((ch & 0x2) != 0) | 0x1F * ((ch & 0x1) != 0);
    uint32_t set_val = fe_bypass | (hb_bypass << 1) | (bandsel << 2) | (be_bypass << 4);
    uint32_t reg = (set_val << 8)*((ch & 0x2) != 0) | set_val * ((ch & 0x1) != 0);
    retVal = FR9009_armSpiCmd_writeField(device, APB_TX_IFCONV_BASEADDR + 0x8, reg, mask, 0);
	IF_ERR_RETURN_U32(retVal);
	if(bandsel & 0x1){
		retVal = FR9009_armSpiCmd_writeField(device, APB_TX_IFCONV_BASEADDR + 0xC, 0x20002000, 0xffffffff, 0);//A gain
		IF_ERR_RETURN_U32(retVal);
		retVal = FR9009_armSpiCmd_writeField(device, APB_TX_IFCONV_BASEADDR + 0x14, (uint32_t)fe_word_A, 0xffffffff, 0);
		IF_ERR_RETURN_U32(retVal);
		retVal = FR9009_armSpiCmd_writeField(device, APB_TX_IFCONV_BASEADDR + 0x24, (uint32_t)be_word_A, 0xffffffff, 0);
		IF_ERR_RETURN_U32(retVal);
	}
	if(bandsel & 0x2){
		retVal = FR9009_armSpiCmd_writeField(device, APB_TX_IFCONV_BASEADDR + 0x10, 0x20002000, 0xffffffff, 0);//B gain
		IF_ERR_RETURN_U32(retVal);
		retVal = FR9009_armSpiCmd_writeField(device, APB_TX_IFCONV_BASEADDR + 0x18, (uint32_t)fe_word_B, 0xffffffff, 0);
		IF_ERR_RETURN_U32(retVal);
		retVal = FR9009_armSpiCmd_writeField(device, APB_TX_IFCONV_BASEADDR + 0x28, (uint32_t)be_word_B, 0xffffffff, 0);
		IF_ERR_RETURN_U32(retVal);
	}
	if(FR_TX1 & ch){
		retVal = FR9009_armSpiCmd_writeField(device, APB_TX_PATH_TOP_BASEADDR + 0x18, (bandsel == 0) << 2, 1<<2,0);
		IF_ERR_RETURN_U32(retVal);
	}
	if(FR_TX2 & ch){
		retVal = FR9009_armSpiCmd_writeField(device, APB_TX_PATH_TOP_BASEADDR + 0x1C, (bandsel == 0) << 2, 1<<2,0);
		IF_ERR_RETURN_U32(retVal);
	}
	return retVal;
}


