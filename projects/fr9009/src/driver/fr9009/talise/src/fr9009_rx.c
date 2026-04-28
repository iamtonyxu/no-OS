/**
 * \file fr9009_rx.c
 * \brief Contains functions to support FR9009 Rx and Observation Rx data path
 *        control
 *
 * Copyright 2022 BriRadio.
 * Released under the FR9009 API license, for more information see the "LICENSE.txt" file in this zip file.
 */

#include "fr9009_rx.h"
#include "fr9009_gpio.h"
#include "fr9009_reg_addr_macros.h"
#include "fr9009_arm_macros.h"
#include "fr9009_arm.h"
#include "fr9009_hal.h"
#include "fr9009_user.h"
#include "fr9009_error.h"
#include "fr9009.h"
#include "fr9009_gpio.h"
#include "math.h"

uint32_t FR9009_programRxGainTable(fr9009Device_t *device, fr9009RxGainTable_t *gainTablePtr, uint8_t numGainIndexesInTable, 
									uint8_t MaxGainIndex, uint8_t MinGainIndex, fr9009RxChannels_t rxChannel)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	fRHALErr_t halError = FRHAL_OK;
	uint32_t ext_rsa = 0;
	uint32_t fe_attn = 0;
	uint32_t dig_comp = 0;
	uint32_t i = 0, j = 0, m=0;
	uint32_t addrIndex = 0;
	uint32_t tableRowIndex = 0;
	uint32_t extMin = 0;
	uint32_t feMin = 0;
	uint32_t digMin = 0;
	uint32_t addrBegin = 0;
	uint32_t numF = addrIndex % 8 ;
	uint32_t mask = 0;
	uint8_t min_gain_table_index = 0;
	uint8_t start_gain_table_index = 0;
	uint8_t gain_table_index_step = 64 / (MaxGainIndex - MinGainIndex + 1);
	uint32_t rx_dig_addr = 0;

#if FR9009_VERBOSE
	halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK, "FR9009_programRxGainTable()\n");
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal, TALACT_WARN_RESET_LOG);
#endif
	if ((rxChannel == FR_RX1) ||
		(rxChannel == FR_RX2) ||
	    (rxChannel == FR_RX1RX2)) {
		/* Verify Rx profile is valid before allowing to set the Rx gain table, this
		 * ensures the device->rx structure pointers are valid
		 */
		if ((device->devStateInfo.profilesValid  & RX_PROFILE_VALID) == 0) {
		}
	} else {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_RXGAINTABLE_INV_CHANNEL, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if (gainTablePtr == NULL) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_PROGRAM_RXGAIN_TABLE_NULL_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}
	if((MaxGainIndex - MinGainIndex + 1) > numGainIndexesInTable){
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_RXGAINTABLE_INV_PROFILE, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Check for channel and the logic behind it */
	if ((rxChannel == FR_RX1) || (rxChannel == FR_RX1RX2)) {
		/* normalize min/max index in device structure to 255 index = max gain */
		device->devStateInfo.rxGainCtrl.rx1MaxGainIndex = MaxGainIndex;
		device->devStateInfo.rxGainCtrl.rx1MinGainIndex = MinGainIndex;
		numF = (device->devStateInfo.rxGainCtrl.rx1MaxGainIndex & 0xFF) | (device->devStateInfo.rxGainCtrl.rx1MinGainIndex & 0xFF) << 8;
		retVal = FR9009_armSpiCmd_writeField(device,  FR9009_ADDR_MAX_MIN_GAIN_INDEX_RX1, numF, 0x0000FFFF,0);
		IF_ERR_RETURN_U32(retVal);
	}

	if ((rxChannel == FR_RX2) || (rxChannel == FR_RX1RX2)) {
		/* normalize min/max index in device structure to 255 index = max gain */
		device->devStateInfo.rxGainCtrl.rx2MaxGainIndex = MaxGainIndex;
		device->devStateInfo.rxGainCtrl.rx2MinGainIndex = MinGainIndex;
		numF = (device->devStateInfo.rxGainCtrl.rx2MaxGainIndex & 0xFF) | (device->devStateInfo.rxGainCtrl.rx2MinGainIndex & 0xFF) << 8;
		retVal = FR9009_armSpiCmd_writeField(device,  FR9009_ADDR_MAX_MIN_GAIN_INDEX_RX2, numF, 0x0000FFFF,0);
		IF_ERR_RETURN_U32(retVal);
	}
#if 1
	for(m=0; m < 2; m++){
		if(((m == 0) && ((rxChannel & FR_RX1) ==0)) || ((m == 1) && ((rxChannel & FR_RX2) ==0))){
			continue;
		}
		if(m == 0){//ch1
			min_gain_table_index = device->devStateInfo.rxGainCtrl.rx1MinGainIndex;
			start_gain_table_index = device->devStateInfo.rxGainCtrl.rx1MaxGainIndex;
			rx_dig_addr = FR9009_ADDR_AGC_DIG_LUT_RX1_IDX_0;
		}else{//ch2
			min_gain_table_index = device->devStateInfo.rxGainCtrl.rx2MinGainIndex;
			start_gain_table_index = device->devStateInfo.rxGainCtrl.rx2MaxGainIndex;
			rx_dig_addr = FR9009_ADDR_AGC_DIG_LUT_RX2_IDX_0;;
		}
		addrIndex = min_gain_table_index % 8;
		addrBegin = addrIndex;
		extMin = (min_gain_table_index - min_gain_table_index % 8) / 2;
		feMin = (min_gain_table_index - min_gain_table_index % 4);
		digMin = min_gain_table_index * 4;
		for (i = start_gain_table_index; i > min_gain_table_index-1; i--) {
			tableRowIndex = (start_gain_table_index - i) * gain_table_index_step + gain_table_index_step - 1;
			j = addrIndex % 8;
			ext_rsa |= gainTablePtr[tableRowIndex].extControl << (j * 4);
			j = addrIndex % 4;
			fe_attn |= gainTablePtr[tableRowIndex].rxFeGain << (j * 8);
			dig_comp = ((uint32_t)(gainTablePtr[tableRowIndex].digGain * sinf(gainTablePtr[tableRowIndex].phaseOffset))) << FR9009_ADDR_AGC_DIG_LUT_DIG_COMP_Q_OFFSET | ((uint32_t)(gainTablePtr[tableRowIndex].digGain * cosf(gainTablePtr[tableRowIndex].phaseOffset)));
			/* set current gain table index (address) */
			addrIndex++;
			if ((addrIndex % 8) == 0) {
				retVal = FR9009_armSpiCmd_writeWords(device, FR9009_ADDR_AGC_RF_ATTN_EXT_RSA_IDX_0 + extMin + (addrIndex / 2 - 4), &ext_rsa, 1);
				IF_ERR_RETURN_U32(retVal);
				ext_rsa = 0; // flush and reset
			}

			if ((addrIndex % 4) == 0) {
				retVal = FR9009_armSpiCmd_writeWords(device, FR9009_ADDR_AGC_RF_ATTN_FE_ATTN_IDX_0 + feMin + (addrIndex / 4 - addrBegin / 4 - 1) * 4, &fe_attn, 1);
				IF_ERR_RETURN_U32(retVal);
				fe_attn = 0; // flush and reset
			}
			retVal = FR9009_armSpiCmd_writeWords(device, (rx_dig_addr + digMin + (addrIndex - addrBegin-1) * 4), &dig_comp, 1);
			IF_ERR_RETURN_U32(retVal);
			dig_comp = 0; // flush and reset
		}
		//finishing up register not devisable by 4 or 8
		if ((addrIndex % 8) != 0) {
			for ( i = 0; i < numF; i++) {
				mask |= 0xF<<i*4;
			}
			retVal = FR9009_armSpiCmd_writeField(device, FR9009_ADDR_AGC_RF_ATTN_EXT_RSA_IDX_0 + extMin + ((addrIndex -numF) / 2 ), ext_rsa, mask,0);
			IF_ERR_RETURN_U32(retVal);
		
		}

		if ((addrIndex % 4) != 0) {
			for ( i = 0; i < numF; i++) {
				mask |= 0xFF<<i*8;
			}
			retVal = FR9009_armSpiCmd_writeField(device,  FR9009_ADDR_AGC_RF_ATTN_FE_ATTN_IDX_0 + feMin + ((addrIndex -numF) / 4 - addrBegin / 4 ) * 4, fe_attn, mask,0);
			IF_ERR_RETURN_U32(retVal);
		}
	}
#endif
	return (uint32_t)retVal;
}
				   
#if 0
uint32_t FR9009_programOrxGainTable(fr9009Device_t *device,
				    fr9009OrxGainTable_t *gainTablePtr, uint8_t numGainIndexesInTable,
				    fr9009ObsRxChannels_t orxChannel)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	fRHALErr_t halError = FRHAL_OK;

#if FR9009_VERBOSE
	halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK,
				 "FR9009_programOrxGainTable()\n");
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	if ((orxChannel == FR_ORX1) ||
	    (orxChannel == FR_ORX2) ||
	    (orxChannel == FR_ORX1ORX2)) {
		/* Verify ORx profile is valid before allowing to set the ORx gain table, this
		 * ensures the device->obsRx structure pointers are valid
		 */
		if ((device->devStateInfo.profilesValid  & ORX_PROFILE_VALID) == 0) {
			return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
							  FR_ERR_ORXGAINTABLE_INV_PROFILE, retVal, TALACT_ERR_CHECK_PARAM);
		}

		/* Check for proper mapping for gain table and passed table number of indexes */
		if (numGainIndexesInTable > ((START_ORX_GAIN_INDEX -  MIN_ORX_GAIN_TABLE_INDEX)
					     + 1)) {
			return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
							  FR_ERR_ORXGAINTABLE_INV_GAIN_INDEX_RANGE, retVal, TALACT_ERR_CHECK_PARAM);
		}
	} else {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
						  FR_ERR_ORXGAINTABLE_INV_CHANNEL, retVal, TALACT_ERR_CHECK_PARAM);
	}

#if 0//使用模拟增益控制
#endif
	return (uint32_t)retVal;
}
#endif
uint32_t FR9009_setRxManualGain(fr9009Device_t *device, fr9009RxChannels_t rxChannel, uint8_t gainIndex)
{
//rx_agc 0x54, 0x5c
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	fRHALErr_t halError = FRHAL_OK;
	// uint32_t regdat = 0;
	

#if FR9009_VERBOSE
	halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK, "FR9009_setRxManualGain()\n");
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal,  TALACT_WARN_RESET_LOG);
#endif
//	/* Verify Rx profile is valid */
//	if ((device->devStateInfo.profilesValid & RX_PROFILE_VALID) == 0) {
//		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
//						  FR_ERR_SETRXGAIN_RXPROFILE_INVALID, retVal, TALACT_ERR_CHECK_PARAM);
//	}
	/* writing mode setting to device state information container */
	device->devStateInfo.rxGainCtrl.gainMode = FR_MGC;

	if (rxChannel == FR_RX1) {
		if ((gainIndex < device->devStateInfo.rxGainCtrl.rx1MinGainIndex) ||
		    (gainIndex > device->devStateInfo.rxGainCtrl.rx1MaxGainIndex)) {
			return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_SETRX1GAIN_INV_GAIN_PARM, retVal, TALACT_ERR_CHECK_PARAM);
		}
		retVal = FR9009_armSpiCmd_mgc_agc_mode(device, CHANNEL_1, gainIndex, 0);
		IF_ERR_RETURN_U32(retVal);
	} else if (rxChannel == FR_RX2) {
		if ((gainIndex < device->devStateInfo.rxGainCtrl.rx2MinGainIndex) ||
		    (gainIndex > device->devStateInfo.rxGainCtrl.rx2MaxGainIndex)) {
			return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_SETRX2GAIN_INV_GAIN_PARM, retVal, TALACT_ERR_CHECK_PARAM);
		}
		retVal = FR9009_armSpiCmd_mgc_agc_mode(device, CHANNEL_2, gainIndex, 0);
		IF_ERR_RETURN_U32(retVal);
	} else {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_SETRXGAIN_INV_CHANNEL, retVal, TALACT_ERR_CHECK_PARAM);
	}

	return (uint32_t)retVal;
}

uint32_t FR9009_getRxGain(fr9009Device_t *device, fr9009RxChannels_t rxChannel,  uint8_t *rxGainIndex)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	fRHALErr_t halError = FRHAL_OK;
	uint32_t regdat = 0;
	uint8_t readData = 0;
	uint8_t channelMaxGainIndex = rxChannel == FR_RX2 ? device->devStateInfo.rxGainCtrl.rx2MaxGainIndex : device->devStateInfo.rxGainCtrl.rx1MaxGainIndex;
	uint8_t channelMinGainIndex = rxChannel == FR_RX2 ? device->devStateInfo.rxGainCtrl.rx2MinGainIndex : device->devStateInfo.rxGainCtrl.rx1MinGainIndex;

#if FR9009_VERBOSE
	halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK, "FR9009_getRxGain()\n");
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal, TALACT_WARN_RESET_LOG);
#endif

	if ((device->devStateInfo.profilesValid & RX_PROFILE_VALID) == 0) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_GETRXGAIN_INV_RXPROFILE, retVal, TALACT_ERR_CHECK_PARAM);
	}

	retVal = FR9009_armSpiCmd_readWords(device, FR9009_ADDR_RX1_RX2_GAIN_TAB_IDX, &regdat, 1);
	IF_ERR_RETURN_U32(retVal);
	if (rxChannel == FR_RX1) {
		readData = (regdat >> 0) & 0xFF;
	} else if (rxChannel == FR_RX2) {
		readData = (regdat >> 8) & 0xFF;
	} else {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_GETRXGAIN_INV_CHANNEL, retVal, TALACT_ERR_CHECK_PARAM);
	}
	/* range check the gain against the max and min expected values */
	if (((readData > channelMaxGainIndex) || (readData < channelMinGainIndex)) && readData != 0xff) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_GETRXGAIN_GAIN_RANGE_EXCEEDED, retVal, TALACT_ERR_CHECK_PARAM);
	}
	/* normalize channel's gain index to 255 max gain index */
	// readData = (MAX_GAIN_TABLE_INDEX - (channelMaxGainIndex - readData));
	if (rxGainIndex != NULL) {
		*rxGainIndex = readData;
	}

	return (uint32_t)retVal;
}

uint32_t FR9009_setObsRxManualGain(fr9009Device_t *device, fr9009ObsRxChannels_t obsRxCh, uint16_t orxAttenuation_mdB)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	fRHALErr_t halError = FRHAL_OK;
#if FR9009_VERBOSE
	halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK, "FR9009_setObsRxManualGain()\n");
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal, TALACT_WARN_RESET_LOG);
#endif
	/* Verify ORx profile is valid, also verifies device data struct pointers
	 * used below are valid
	 */
	if ((device->devStateInfo.profilesValid & ORX_PROFILE_VALID) == 0) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_SETORXGAIN_INV_ORXPROFILE, retVal, TALACT_ERR_CHECK_PARAM);
	}
	/* check for valid channel and assign address */
	if(obsRxCh != FR_ORX1 && obsRxCh != FR_ORX2 && obsRxCh != FR_ORX1ORX2) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_SETORXGAIN_INV_CHANNEL, retVal, TALACT_ERR_CHECK_PARAM);

	}
	if(orxAttenuation_mdB < 0 || orxAttenuation_mdB > 48198){
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_SETORXGAIN_INV_ORX2GAIN, retVal, TALACT_ERR_CHECK_PARAM);
	}
	retVal = FR9009_armSpiCmd_orx_gain_set(device, (CHANNEL_t) (obsRxCh >> 2), orxAttenuation_mdB);
	return (uint32_t)retVal;
}

uint32_t FR9009_getObsRxGain(fr9009Device_t *device, fr9009ObsRxChannels_t obsRxChannel, uint16_t *porxAttenuation_mdB)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	fRHALErr_t halError = FRHAL_OK;
	uint32_t regdat = 0;

#if FR9009_VERBOSE
	halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK, "FR9009_getObsRxGain()\n");
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal, TALACT_WARN_RESET_LOG);
#endif

	if ((device->devStateInfo.profilesValid & ORX_PROFILE_VALID) == 0) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_GETOBSRXGAIN_INV_ORXPROFILE, retVal, TALACT_ERR_CHECK_PARAM);
	}
	/* read back the current gain index for the required channel */
	if (obsRxChannel == FR_ORX1) {
		retVal = FR9009_armSpiCmd_readWords(device, FR9009_ADDR_ARMSPI(SPI_RX_ORX_L0_ID, 0x30), &regdat, 1);
		IF_ERR_RETURN_U32(retVal);		
	} else if (obsRxChannel == FR_ORX2) {
		retVal = FR9009_armSpiCmd_readWords(device, FR9009_ADDR_ARMSPI(SPI_RX_ORX_L1_ID, 0x30), &regdat, 1);
		IF_ERR_RETURN_U32(retVal);	
	} else {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_GETOBSRXGAIN_INV_CHANNEL, retVal, TALACT_ERR_CHECK_PARAM);
	}
	retVal = FR9009_armSpiCmd_orx_gain_get(device, (CHANNEL_t) (obsRxChannel >> 2), &regdat);
	if (porxAttenuation_mdB != NULL) {
		*porxAttenuation_mdB = (uint16_t)(regdat);
	}
	return (uint32_t)retVal;
}

uint32_t FR9009_setRxGainControlMode(fr9009Device_t *device, fr9009GainMode_t mode)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	fRHALErr_t halError = FRHAL_OK;
	uint32_t regdat = 0;

#if FR9009_VERBOSE
	halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK, "FR9009_setRxGainControlMode()\n");
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal, TALACT_WARN_RESET_LOG);
#endif

	/* performing AGC type check */
	switch (mode) {
		case FR_MGC:
		case FR_MGC_PIN:
			regdat = (0x3 << 3) | (0 << 2);
			break;
		case FR_AGCSLOW:
		case FR_AGCFAST: /* Fall through to next case */
			regdat = (0x0 << 3) | (1 << 2);
			break;
		default:
			return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
						  FR_ERR_INV_RX_GAIN_MODE_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}
	retVal = FR9009_armSpiCmd_writeField(device, FR9009_ADDR_AGC_MANUAL_GAIN_SET_FSM_CNT_RESET_RX1_RX2, regdat, 0x7 << 2, 0);
	IF_ERR_RETURN_U32(retVal);
	/* writing mode setting to device state information container */
	device->devStateInfo.rxGainCtrl.gainMode = mode;

	return (uint32_t)retVal;
}
					 
#if 0 
uint32_t FR9009_setRxDataFormat (fr9009Device_t *device,
				 fr9009RxDataFormat_t *rxDataFormat)
{
	
	return (uint32_t)retVal;
} /* end FR9009_setRxDataFormat */

uint32_t FR9009_getRxDataFormat(fr9009Device_t *device,
				fr9009RxDataFormat_t *rxDataFormat)
{
	
	return (uint32_t)retVal;
}

uint32_t FR9009_getSlicerPosition(fr9009Device_t *device,
				  uint8_t *rx1SlicerPosition, uint8_t *rx2SlicerPosition)
{

	
	return (uint32_t)retVal;
}
#endif
uint32_t FR9009_setRxGainCtrlPin(fr9009Device_t *device,fr9009RxChannels_t rxChannel, fr9009RxGainCtrlPin_t *rxGainCtrlPin)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	fRHALErr_t halError = FRHAL_OK;
	fr9009GpioShortConfig_t attpin_config;

#if FR9009_VERBOSE
	halError =brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK, "FR9009_setRxGainCtrlPin()\n");
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal, TALACT_WARN_RESET_LOG);
#endif
	if(rxChannel != FR_RX1 && rxChannel != FR_RX2){
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_SETRXMGCPINCTRL_INV_CHANNEL, retVal, TALACT_ERR_CHECK_PARAM);
	}
	retVal = FR9009_setGpioOe(device, 0, (0x1 << rxGainCtrlPin->rxGainIncPin) | (0x1 << rxGainCtrlPin->rxGainDecPin));
	IF_ERR_RETURN_U32(retVal);
	
	retVal = FR9009_setGpioSourceCtrl(device, FR_GPIO_RX_MANUAL_GAIN_CONTROL_MODE_1 << (rxGainCtrlPin->rxGainIncPin / 4));
	IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_getRxMgcSel(device, &attpin_config);
	IF_ERR_RETURN_U32(retVal);
	if(rxGainCtrlPin->enable > 0){
		retVal = FR9009_armSpiCmd_writeField(device, FR9009_ADDR_AGC_MANUAL_GAIN_SET_FSM_CNT_RESET_RX1_RX2, 0, 1 << 2, 2);//配置为MGC模式
		IF_ERR_RETURN_U32(retVal);
	}
	if(rxChannel == FR_RX1){
		attpin_config.p0.gpioPinSel = rxGainCtrlPin->rxGainIncPin;
		attpin_config.p0.enable = rxGainCtrlPin->enable > 0? 1 : 0;
		attpin_config.p1.gpioPinSel = rxGainCtrlPin->rxGainDecPin;
		attpin_config.p1.enable = rxGainCtrlPin->enable > 0? 1 : 0;
		retVal = FR9009_armSpiCmd_writeField(device, FR9009_ADDR_AGC_MANUAL_GAIN_SET_FSM_CNT_RESET_RX1_RX2, rxGainCtrlPin->enable > 0? 0 : 1, 1 << 3, 3);//开关MGC preset
		IF_ERR_RETURN_U32(retVal);
	}else{
		attpin_config.p2.gpioPinSel = rxGainCtrlPin->rxGainIncPin;
		attpin_config.p2.enable = rxGainCtrlPin->enable > 0? 1 : 0;
		attpin_config.p3.gpioPinSel = rxGainCtrlPin->rxGainDecPin;
		attpin_config.p3.enable = rxGainCtrlPin->enable > 0? 1 : 0;	
		retVal = FR9009_armSpiCmd_writeField(device, FR9009_ADDR_AGC_MANUAL_GAIN_SET_FSM_CNT_RESET_RX1_RX2, rxGainCtrlPin->enable > 0? 0 : 1, 1 << 4, 4);	//开关MGC preset
		IF_ERR_RETURN_U32(retVal);
	}
	retVal = FR9009_setRxMgcSel(device, &attpin_config);
	IF_ERR_RETURN_U32(retVal);

	return (uint32_t)retVal;
}

uint32_t FR9009_getRxGainCtrlPin(fr9009Device_t *device, fr9009RxChannels_t rxChannel, fr9009RxGainCtrlPin_t *rxGainCtrlPin)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	fRHALErr_t halError = FRHAL_OK;
	fr9009GpioShortConfig_t attpin_config;
	// uint8_t pinSel = 0;
	// uint32_t regdat = 0;
	// int i =0;

#if FR9009_VERBOSE
	halError =brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK, "FR9009_getRxGainCtrlPin()\n");
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal, TALACT_WARN_RESET_LOG);
#endif

	/* Read current value of Gain inc/dec and enable */
	switch (rxChannel) {
	case FR_RX1:
		break;
	case FR_RX2:
		break;
	default: {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_GETRXMGCPINCTRL_INV_CHANNEL, retVal, TALACT_ERR_CHECK_PARAM);
	}
	}

	/*Check passed pointers for NULL */
	if (rxGainCtrlPin == NULL) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_GETRXMGCPINCTRL_NULL_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}
	retVal = FR9009_getRxMgcSel(device, &attpin_config);
	IF_ERR_RETURN_U32(retVal);
	if(rxChannel == FR_RX1){
		rxGainCtrlPin->rxGainIncPin = attpin_config.p0.gpioPinSel;
		rxGainCtrlPin->enable = attpin_config.p0.enable;
		rxGainCtrlPin->rxGainDecPin = attpin_config.p1.gpioPinSel;
		rxGainCtrlPin->enable = attpin_config.p1.enable;
	}else{
		rxGainCtrlPin->rxGainIncPin = attpin_config.p2.gpioPinSel;
		rxGainCtrlPin->enable = attpin_config.p2.enable;
		rxGainCtrlPin->rxGainDecPin = attpin_config.p3.gpioPinSel;
		rxGainCtrlPin->enable = attpin_config.p3.enable;		
	}
	return (uint32_t)retVal;
}
#if 0
talRecoveryActions_t talSetDualBandSettings(fr9009Device_t *device,
		fr9009Init_t *init)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	fRHALErr_t halError = FRHAL_OK;
	int32_t pfirFs_kHz = 0;
	int32_t pfirFsDiv2_kHz = 0;
	int32_t outFs_kHz = 0;
	int32_t outFsDiv2_kHz = 0;
	int32_t upperBandBandwidthDiv2_kHz = 0;
	int32_t lowerBandBandwidthDiv2_kHz = 0;
	int32_t bandSeparation_kHz = 0;
	int32_t maxLowerBandShiftCenterFreq_kHz = 0;
	int32_t minLowerBandShiftCenterFreq_kHz = 0;
	int32_t lowerBandShiftCenterFreq_kHz = 0;
	int32_t lowerBandNco1Freq_kHz = 0;
	int32_t maxUpperBandShiftCenterFreq_kHz = 0;
	int32_t minUpperBandShiftCenterFreq_kHz = 0;
	int32_t upperBandShiftCenterFreq_kHz = 0;
	int32_t upperBandNco1Freq_kHz = 0;
	int32_t lowerBandNco2Freq_kHz = 0;
	int32_t upperBandNco2Freq_kHz = 0;
	uint32_t lowerBandNco1FreqUnsigned_kHz = 0;
	uint32_t upperBandNco1FreqUnsigned_kHz = 0;
	uint32_t lowerBandNco2FreqUnsigned_kHz = 0;
	uint32_t upperBandNco2FreqUnsigned_kHz = 0;
	uint64_t lowerBandNco1Ftw = 0;
	uint64_t upperBandNco1Ftw = 0;
	uint64_t lowerBandNco2Ftw = 0;
	uint64_t upperBandNco2Ftw = 0;

	int32_t inUpperBandCenterFreq_kHz = 0;
	int32_t inLowerBandCenterFreq_kHz = 0;
	int32_t outUpperBandCenterFreq_kHz = 0;
	int32_t outLowerBandCenterFreq_kHz = 0;

	static const uint64_t POW_2_32 = ((uint64_t)1) << 32;

#if FR9009_VERBOSE
	halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK,
				 "talSetDualBandSettings()\n");
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	inUpperBandCenterFreq_kHz =
		init->rx.rxProfile.rxIfConvCfg.bandAInputCenterFreq_kHz;
	inLowerBandCenterFreq_kHz =
		init->rx.rxProfile.rxIfConvCfg.bandBInputCenterFreq_kHz;

	outUpperBandCenterFreq_kHz = inUpperBandCenterFreq_kHz +
				     init->rx.rxProfile.rxIfConvCfg.bandANco1Freq_kHz +
				     init->rx.rxProfile.rxIfConvCfg.bandANco2Freq_kHz;
	outLowerBandCenterFreq_kHz = inLowerBandCenterFreq_kHz +
				     init->rx.rxProfile.rxIfConvCfg.bandBNco1Freq_kHz +
				     init->rx.rxProfile.rxIfConvCfg.bandBNco2Freq_kHz;

	/* Fs at output of PFIR : Twice output rate due to decimation in DDC */
	pfirFsDiv2_kHz = device->devStateInfo.rxOutputRate_kHz;
	pfirFs_kHz = pfirFsDiv2_kHz << 1;

	/* Output Fs and Fs/2 */
	outFs_kHz = pfirFsDiv2_kHz;
	outFsDiv2_kHz = outFs_kHz >> 1;

	/* Bandwidth /2 */
	upperBandBandwidthDiv2_kHz =
		(init->rx.rxProfile.rxIfConvCfg.bandAInputBandWidth_kHz >> 1);
	lowerBandBandwidthDiv2_kHz =
		(init->rx.rxProfile.rxIfConvCfg.bandBInputBandWidth_kHz >> 1);

	/* Check : Center frequency of input lower band is less than input upper band */
	if (inLowerBandCenterFreq_kHz > inUpperBandCenterFreq_kHz) {
		return talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
					FR_ERR_SETDUALBANDSETTINGS_INV_CENTER_FREQ, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Check : Band should be separated by at least 1 khz */
	if (((inUpperBandCenterFreq_kHz - upperBandBandwidthDiv2_kHz) -
	     (inLowerBandCenterFreq_kHz + lowerBandBandwidthDiv2_kHz)) < 0) {
		return talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
					FR_ERR_SETDUALBANDSETTINGS_INV_BAND_SEP, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Check : Upper end of upper band input is less than +PfirFs/2 */
	if ((inUpperBandCenterFreq_kHz + upperBandBandwidthDiv2_kHz) > pfirFsDiv2_kHz) {
		return talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
					FR_ERR_SETDUALBANDSETTINGS_INV_IN_UPPER_FREQ, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Check : Lower end of lower band input is more than -PfirFs/2 */
	if ((inLowerBandCenterFreq_kHz - lowerBandBandwidthDiv2_kHz) <
	    -pfirFsDiv2_kHz) {
		return talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
					FR_ERR_SETDUALBANDSETTINGS_INV_IN_LOWER_FREQ, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Check : upper band output is between -OutFs/2 and  +OutFs/2 */
	if (((outUpperBandCenterFreq_kHz + upperBandBandwidthDiv2_kHz) > outFsDiv2_kHz)
	    ||
	    ((outUpperBandCenterFreq_kHz - upperBandBandwidthDiv2_kHz) < -outFsDiv2_kHz)) {
		return talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
					FR_ERR_SETDUALBANDSETTINGS_INV_OUT_UPPER_FREQ, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Check : lower band output is between -OutFs/2 and  +OutFs/2 */
	if (((outLowerBandCenterFreq_kHz + lowerBandBandwidthDiv2_kHz) > outFsDiv2_kHz)
	    ||
	    ((outLowerBandCenterFreq_kHz - lowerBandBandwidthDiv2_kHz) < -outFsDiv2_kHz)) {
		return talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
					FR_ERR_SETDUALBANDSETTINGS_INV_OUT_LOWER_FREQ, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Check: Output upper and lower bands dont overlap */
	/* Error if 'lower end of lowerBand' < 'higher end of upperBand' and 'lower end of upperBand' < 'higher end of lowerBand' */
	if (((outLowerBandCenterFreq_kHz - lowerBandBandwidthDiv2_kHz) <
	     (outUpperBandCenterFreq_kHz + upperBandBandwidthDiv2_kHz)) &&
	    ((outUpperBandCenterFreq_kHz - upperBandBandwidthDiv2_kHz) <
	     (outLowerBandCenterFreq_kHz + lowerBandBandwidthDiv2_kHz))) {
		return talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
					FR_ERR_SETDUALBANDSETTINGS_OUT_OVERLAP, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Separation between the two bands */
	bandSeparation_kHz = (pfirFs_kHz - lowerBandBandwidthDiv2_kHz -
			      upperBandBandwidthDiv2_kHz -
			      (inUpperBandCenterFreq_kHz - inLowerBandCenterFreq_kHz));

	/* Move lower band up to the upper end of DEC2 passband. */
	/* This value assumes that the upper band does not alias back into the lower side of DEC2 passband */
	maxLowerBandShiftCenterFreq_kHz = (outFsDiv2_kHz - lowerBandBandwidthDiv2_kHz);

	/* This value assumes that upper band can alias back into lower side of DEC2 passband. The shift is hence limited by band separation  */
	minLowerBandShiftCenterFreq_kHz = (-outFsDiv2_kHz + bandSeparation_kHz +
					   lowerBandBandwidthDiv2_kHz);

	/* The value that the lower band can be shifted upwards is the minimum of the two values */
	lowerBandShiftCenterFreq_kHz = (maxLowerBandShiftCenterFreq_kHz <
					minLowerBandShiftCenterFreq_kHz) ? maxLowerBandShiftCenterFreq_kHz :
				       minLowerBandShiftCenterFreq_kHz;

	/* NCO1 Frequency to tune to for lower band: NCO frequency = Fout - Fin , since I and Q are swapped */
	lowerBandNco1Freq_kHz = (lowerBandShiftCenterFreq_kHz -
				 inLowerBandCenterFreq_kHz);

	/* Move upper band down to the lower end of DEC2 passband. */
	/* This value assumes that the lower band does not alias back into the upper side of DEC2 passband */
	maxUpperBandShiftCenterFreq_kHz = (-outFsDiv2_kHz + upperBandBandwidthDiv2_kHz);

	/* This value assumes that lower band can alias back into upper side of DEC2 passband. The shift is hence limited by band separation  */
	minUpperBandShiftCenterFreq_kHz = (outFsDiv2_kHz - bandSeparation_kHz -
					   upperBandBandwidthDiv2_kHz) ;

	/* The value that the Upper band can be shifted downwards is the max of the two values */
	upperBandShiftCenterFreq_kHz = (maxUpperBandShiftCenterFreq_kHz >
					minUpperBandShiftCenterFreq_kHz) ? maxUpperBandShiftCenterFreq_kHz :
				       minUpperBandShiftCenterFreq_kHz;

	/* NCO1 Frequency to tune to for upper: NCO frequency = Fout - Fin , since I and Q are swapped*/
	upperBandNco1Freq_kHz = (upperBandShiftCenterFreq_kHz -
				 inUpperBandCenterFreq_kHz);

	/* To convert from lowerBandShiftCenterFreq_kHz to outLowerBandCenterFreq_kHz */
	lowerBandNco2Freq_kHz = (outLowerBandCenterFreq_kHz -
				 lowerBandShiftCenterFreq_kHz);

	/* To convert from upperBandShiftCenterFreq_kHz to outUpperBandCenterFreq_kHz */
	upperBandNco2Freq_kHz = (outUpperBandCenterFreq_kHz -
				 upperBandShiftCenterFreq_kHz);

	/* Convert negative to positive frequencies */
	lowerBandNco1FreqUnsigned_kHz = (lowerBandNco1Freq_kHz > 0) ?
					lowerBandNco1Freq_kHz : (pfirFs_kHz + lowerBandNco1Freq_kHz);
	upperBandNco1FreqUnsigned_kHz = (upperBandNco1Freq_kHz > 0) ?
					upperBandNco1Freq_kHz : (pfirFs_kHz + upperBandNco1Freq_kHz);
	lowerBandNco2FreqUnsigned_kHz = (lowerBandNco2Freq_kHz > 0) ?
					lowerBandNco2Freq_kHz : (outFs_kHz + lowerBandNco2Freq_kHz);
	upperBandNco2FreqUnsigned_kHz = (upperBandNco2Freq_kHz > 0) ?
					upperBandNco2Freq_kHz : (outFs_kHz + upperBandNco2Freq_kHz);

	/* Convert positive frequencies to their FTW equivalents */
	lowerBandNco1Ftw = ((((uint64_t)lowerBandNco1FreqUnsigned_kHz * POW_2_32) << 1)
			    + pfirFs_kHz) / (pfirFs_kHz << 1); /* integer rounding */
	upperBandNco1Ftw = ((((uint64_t)upperBandNco1FreqUnsigned_kHz * POW_2_32) << 1)
			    + pfirFs_kHz) / (pfirFs_kHz << 1); /* integer rounding */
	lowerBandNco2Ftw = ((((uint64_t)lowerBandNco2FreqUnsigned_kHz * POW_2_32) << 1)
			    + outFs_kHz) / (outFs_kHz << 1); /* integer rounding */
	upperBandNco2Ftw = ((((uint64_t)upperBandNco2FreqUnsigned_kHz * POW_2_32) << 1)
			    + outFs_kHz) / (outFs_kHz << 1); /* integer rounding */

	/* Check FTW is not more than 32 bits */
	if ((lowerBandNco1Ftw >= POW_2_32) || (upperBandNco1Ftw >= POW_2_32) ||
	    (lowerBandNco2Ftw >= POW_2_32) || (upperBandNco2Ftw >= POW_2_32)) {
		return talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
					FR_ERR_SETDUALBANDSETTINGS_FTW_OVRG, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Write to FTW - lower Band NCO 1*/
	halError = talSpiWriteByte(device->devHalInfo, FR9009_ADDR_RX_NCO_CH1_FTW_BYTE4,
				   (uint8_t)(lowerBandNco1Ftw >> 24));
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	halError = talSpiWriteByte(device->devHalInfo, FR9009_ADDR_RX_NCO_CH1_FTW_BYTE3,
				   (uint8_t)(lowerBandNco1Ftw >> 16));
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	halError = talSpiWriteByte(device->devHalInfo, FR9009_ADDR_RX_NCO_CH1_FTW_BYTE2,
				   (uint8_t)(lowerBandNco1Ftw >> 8));
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	halError = talSpiWriteByte(device->devHalInfo, FR9009_ADDR_RX_NCO_CH1_FTW_BYTE1,
				   (uint8_t)(lowerBandNco1Ftw));
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	halError = talSpiWriteByte(device->devHalInfo, FR9009_ADDR_RX_NCO_CH2_FTW_BYTE4,
				   (uint8_t)(lowerBandNco1Ftw >> 24));
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	halError = talSpiWriteByte(device->devHalInfo, FR9009_ADDR_RX_NCO_CH2_FTW_BYTE3,
				   (uint8_t)(lowerBandNco1Ftw >> 16));
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	halError = talSpiWriteByte(device->devHalInfo, FR9009_ADDR_RX_NCO_CH2_FTW_BYTE2,
				   (uint8_t)(lowerBandNco1Ftw >> 8));
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	halError = talSpiWriteByte(device->devHalInfo, FR9009_ADDR_RX_NCO_CH2_FTW_BYTE1,
				   (uint8_t)(lowerBandNco1Ftw));
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	/* Write to FTW - upper Band NCO 1*/
	halError = talSpiWriteByte(device->devHalInfo,
				   FR9009_ADDR_RX1_BAND_B_NCO1_FTW_BYTE4, (uint8_t)(upperBandNco1Ftw >> 24));
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   FR9009_ADDR_RX1_BAND_B_NCO1_FTW_BYTE3, (uint8_t)(upperBandNco1Ftw >> 16));
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   FR9009_ADDR_RX1_BAND_B_NCO1_FTW_BYTE2, (uint8_t)(upperBandNco1Ftw >> 8));
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   FR9009_ADDR_RX1_BAND_B_NCO1_FTW_BYTE1, (uint8_t)(upperBandNco1Ftw));
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   FR9009_ADDR_RX2_BAND_B_NCO1_FTW_BYTE4, (uint8_t)(upperBandNco1Ftw >> 24));
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   FR9009_ADDR_RX2_BAND_B_NCO1_FTW_BYTE3, (uint8_t)(upperBandNco1Ftw >> 16));
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   FR9009_ADDR_RX2_BAND_B_NCO1_FTW_BYTE2, (uint8_t)(upperBandNco1Ftw >> 8));
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   FR9009_ADDR_RX2_BAND_B_NCO1_FTW_BYTE1, (uint8_t)(upperBandNco1Ftw));
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	/* Write to FTW - lower Band NCO 2*/
	halError = talSpiWriteByte(device->devHalInfo,
				   FR9009_ADDR_RX1_BAND_A_NCO2_FTW_BYTE4, (uint8_t)(lowerBandNco2Ftw >> 24));
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   FR9009_ADDR_RX1_BAND_A_NCO2_FTW_BYTE3, (uint8_t)(lowerBandNco2Ftw >> 16));
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   FR9009_ADDR_RX1_BAND_A_NCO2_FTW_BYTE2, (uint8_t)(lowerBandNco2Ftw >> 8));
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   FR9009_ADDR_RX1_BAND_A_NCO2_FTW_BYTE1, (uint8_t)(lowerBandNco2Ftw));
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   FR9009_ADDR_RX2_BAND_A_NCO2_FTW_BYTE4, (uint8_t)(lowerBandNco2Ftw >> 24));
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   FR9009_ADDR_RX2_BAND_A_NCO2_FTW_BYTE3, (uint8_t)(lowerBandNco2Ftw >> 16));
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   FR9009_ADDR_RX2_BAND_A_NCO2_FTW_BYTE2, (uint8_t)(lowerBandNco2Ftw >> 8));
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   FR9009_ADDR_RX2_BAND_A_NCO2_FTW_BYTE1, (uint8_t)(lowerBandNco2Ftw));
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	/* Write to FTW - upper Band NCO 2*/
	halError = talSpiWriteByte(device->devHalInfo,
				   FR9009_ADDR_RX1_BAND_B_NCO2_FTW_BYTE4, (uint8_t)(upperBandNco2Ftw >> 24));
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   FR9009_ADDR_RX1_BAND_B_NCO2_FTW_BYTE3, (uint8_t)(upperBandNco2Ftw >> 16));
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   FR9009_ADDR_RX1_BAND_B_NCO2_FTW_BYTE2, (uint8_t)(upperBandNco2Ftw >> 8));
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   FR9009_ADDR_RX1_BAND_B_NCO2_FTW_BYTE1, (uint8_t)(upperBandNco2Ftw));
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   FR9009_ADDR_RX2_BAND_B_NCO2_FTW_BYTE4, (uint8_t)(upperBandNco2Ftw >> 24));
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   FR9009_ADDR_RX2_BAND_B_NCO2_FTW_BYTE3, (uint8_t)(upperBandNco2Ftw >> 16));
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   FR9009_ADDR_RX2_BAND_B_NCO2_FTW_BYTE2, (uint8_t)(upperBandNco2Ftw >> 8));
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	halError = talSpiWriteByte(device->devHalInfo,
				   FR9009_ADDR_RX2_BAND_B_NCO2_FTW_BYTE1, (uint8_t)(upperBandNco2Ftw));
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	/* Update FTW words */
	halError = talSpiWriteByte(device->devHalInfo,
				   FR9009_ADDR_RX_NCO_FTW_UPDATE_CONTROL, 0xFF);
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	return retVal;
}

static talRecoveryActions_t talSetRxNcoFtw(fr9009Device_t *device,
		fr9009RxNcoChannel_t rxNcoChannel, uint32_t ncoFtw)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	fRHALErr_t halError = FRHAL_OK;
	uint16_t spiAddr = 0;

	switch(rxNcoChannel) {
	case FR_RX1_NCO1A:
		spiAddr = FR9009_ADDR_RX_NCO_CH1_FTW_BYTE4;
		break;
	case FR_RX1_NCO1B:
		spiAddr = FR9009_ADDR_RX1_BAND_B_NCO1_FTW_BYTE4;
		break;
	case FR_RX1_NCO2A:
		spiAddr = FR9009_ADDR_RX1_BAND_A_NCO2_FTW_BYTE4;
		break;
	case FR_RX1_NCO2B:
		spiAddr = FR9009_ADDR_RX1_BAND_B_NCO2_FTW_BYTE4;
		break;
	case FR_RX2_NCO1A:
		spiAddr = FR9009_ADDR_RX_NCO_CH2_FTW_BYTE4;
		break;
	case FR_RX2_NCO1B:
		spiAddr = FR9009_ADDR_RX2_BAND_B_NCO1_FTW_BYTE4;
		break;
	case FR_RX2_NCO2A:
		spiAddr = FR9009_ADDR_RX2_BAND_A_NCO2_FTW_BYTE4;
		break;
	case FR_RX2_NCO2B:
		spiAddr = FR9009_ADDR_RX2_BAND_B_NCO2_FTW_BYTE4;
		break;
	default:
		return talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
					FR_ERR_RXNCOFTW_INVNCO, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Write to FTW - NCO 1*/
	halError = talSpiWriteByte(device->devHalInfo, spiAddr,
				   (uint8_t)(ncoFtw >> 24));
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	halError = talSpiWriteByte(device->devHalInfo, (spiAddr + 1),
				   (uint8_t)(ncoFtw >> 16));
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	halError = talSpiWriteByte(device->devHalInfo, (spiAddr + 2),
				   (uint8_t)(ncoFtw >> 8));
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	halError = talSpiWriteByte(device->devHalInfo, (spiAddr + 3),
				   (uint8_t)(ncoFtw));
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	return retVal;
}

talRecoveryActions_t talSetupNcoShifter(fr9009Device_t *device,
					fr9009Init_t *init)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	fRHALErr_t halError = FRHAL_OK;
	int32_t pfirFs_kHz = 0;
	int32_t outFs_kHz = 0;
	int32_t ddcOutFsDiv2_kHz = 0;
	uint32_t nco1FreqUnsigned_kHz = 0;
	uint64_t nco1Ftw = 0;
	uint32_t nco2FreqUnsigned_kHz = 0;
	uint64_t nco2Ftw = 0;
	int32_t inputCenterFreq_kHz = 0;
	int32_t nco1OutputCenterFreq_khz = 0;
	int32_t nco2OutputCenterFreq_khz = 0;
	int32_t bandWidthDiv2 = 0;
	uint8_t ddcHbEnabled = 0;
	int32_t ddcHbCorner_kHz = 0;
	int32_t pfirCorner_kHz = 0;
	uint8_t ncoUpdateFtw = 0;

	static const uint64_t POW_2_32 = ((uint64_t)1) << 32;

#if FR9009_VERBOSE
	halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK,
				 "talRecoveryActions_t()\n");
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	bandWidthDiv2 = init->rx.rxProfile.rxIfConvCfg.bandAInputBandWidth_kHz >> 1;

	inputCenterFreq_kHz =
		init->rx.rxProfile.rxIfConvCfg.bandAInputCenterFreq_kHz;
	nco1OutputCenterFreq_khz = inputCenterFreq_kHz +
				   init->rx.rxProfile.rxIfConvCfg.bandANco1Freq_kHz;
	nco2OutputCenterFreq_khz = nco1OutputCenterFreq_khz +
				   init->rx.rxProfile.rxIfConvCfg.bandANco2Freq_kHz;

	/* scale the pfirFs for the selected DDC mode */
	/* Fs at output of PFIR : Twice output rate due to decimation in DDC */
	if ((init->rx.rxProfile.rxIfConvMode == FR_IFCONV_DEC2) ||
	    (init->rx.rxProfile.rxIfConvMode == FR_IFCONV_DEC2_REALIF)) {
		pfirFs_kHz = device->devStateInfo.rxOutputRate_kHz << 1;
		ddcHbEnabled = 1;

		/* DDC HB corner is at 0.4 * rx output rate.  0.4 = 2/5 */
		ddcHbCorner_kHz = (int32_t)(device->devStateInfo.rxOutputRate_kHz << 1) / 5;
	} else if ((init->rx.rxProfile.rxIfConvMode == FR_IFCONV_INT2) ||
		   (init->rx.rxProfile.rxIfConvMode == FR_IFCONV_INT2_REALIF)) {
		pfirFs_kHz = device->devStateInfo.rxOutputRate_kHz >> 1;
		ddcHbEnabled = 1;

		/* DDC HB corner is at 0.4 * (rx output rate/2).  0.4 = 2/5 */
		ddcHbCorner_kHz = (int32_t)(device->devStateInfo.rxOutputRate_kHz) / 5;
	} else if ((init->rx.rxProfile.rxIfConvMode == FR_IFCONV_FILTERONLY) ||
		   (init->rx.rxProfile.rxIfConvMode == FR_IFCONV_FILTERONLY_REALIF)) {
		pfirFs_kHz = device->devStateInfo.rxOutputRate_kHz;
		ddcHbEnabled = 1;

		/* DDC HB corner is at 0.4 * (rx output rate/2).  0.4 = 2/5 */
		ddcHbCorner_kHz = (int32_t)(device->devStateInfo.rxOutputRate_kHz) / 5;
	} else if ((init->rx.rxProfile.rxIfConvMode == FR_IFCONV_BYPASS) ||
		   (init->rx.rxProfile.rxIfConvMode == FR_IFCONV_BYPASS_REALIF)) {
		pfirFs_kHz = device->devStateInfo.rxOutputRate_kHz;
		ddcHbEnabled = 0;

		/* DDC HB corner is at the Pfir corner since HalfBand is disabled */
		ddcHbCorner_kHz = (int32_t)((init->rx.rxProfile.rfBandwidth_Hz / 1000) >> 1);;
	}

	outFs_kHz = device->devStateInfo.rxOutputRate_kHz;
	ddcOutFsDiv2_kHz = (outFs_kHz >> 1);

	pfirCorner_kHz = ((init->rx.rxProfile.rfBandwidth_Hz / 1000) >> 1);


	/* Range check that signal passband edges are within Rx PFIR corners */
	if (((inputCenterFreq_kHz + bandWidthDiv2) > pfirCorner_kHz) ||
	    ((inputCenterFreq_kHz - bandWidthDiv2) < -pfirCorner_kHz)) {
		return talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
					FR_ERR_SETUPNCOSHIFTER_INV_PFIR_CORNER, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* If DDC HB enabled, verify that input signal after possible NCO1 shift is in passband of DDC HB */
	if (ddcHbEnabled > 0) {
		if (((nco1OutputCenterFreq_khz + bandWidthDiv2) > ddcHbCorner_kHz) ||
		    ((nco1OutputCenterFreq_khz - bandWidthDiv2) < -ddcHbCorner_kHz)) {
			return talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
						FR_ERR_SETUPNCOSHIFTER_INV_DDCHB_CORNER, retVal, TALACT_ERR_CHECK_PARAM);
		}
	}

	/* Range check NCO2 shifted output is within -Fs/2 to Fs/2 of Rx output rate */
	if (((nco2OutputCenterFreq_khz + bandWidthDiv2) > ddcOutFsDiv2_kHz) ||
	    ((nco2OutputCenterFreq_khz - bandWidthDiv2) < -ddcOutFsDiv2_kHz)) {
		return talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
					FR_ERR_SETUPNCOSHIFTER_INV_NCO2SHIFT, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* if nco1 shift */
	if (init->rx.rxProfile.rxIfConvCfg.bandANco1Freq_kHz != 0) {
		/* Convert from -ve frequencies to their positive equivalents */
		nco1FreqUnsigned_kHz = (init->rx.rxProfile.rxIfConvCfg.bandANco1Freq_kHz >=
					0)
				       ? init->rx.rxProfile.rxIfConvCfg.bandANco1Freq_kHz
				       : (pfirFs_kHz + init->rx.rxProfile.rxIfConvCfg.bandANco1Freq_kHz) ;

		/* Convert positive frequencies to their FTW equivalents */
		nco1Ftw = ((((uint64_t)nco1FreqUnsigned_kHz * POW_2_32) << 1) + pfirFs_kHz) /
			  (pfirFs_kHz << 1); /* integer rounding */

		/* Check FTW is not more than 32 bits */
		if (nco1Ftw >= POW_2_32) {
			return talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
						FR_ERR_SETUPNCOSHIFTER_FTW_OVRG, retVal, TALACT_ERR_CHECK_PARAM);
		}

		retVal = talSetRxNcoFtw(device, FR_RX1_NCO1A, (uint32_t)nco1Ftw);
		IF_ERR_RETURN(retVal);

		retVal = talSetRxNcoFtw(device, FR_RX2_NCO1A, (uint32_t)nco1Ftw);
		IF_ERR_RETURN(retVal);

		/* Update FTW words - write at bottom of function */
		ncoUpdateFtw |= 0x11;
	}

	/* if nco2 shift */
	if (init->rx.rxProfile.rxIfConvCfg.bandANco2Freq_kHz != 0) {
		/* Convert from -ve frequencies to their positive equivalents */

		nco2FreqUnsigned_kHz = (init->rx.rxProfile.rxIfConvCfg.bandANco2Freq_kHz >=
					0)
				       ? init->rx.rxProfile.rxIfConvCfg.bandANco2Freq_kHz
				       : (outFs_kHz + init->rx.rxProfile.rxIfConvCfg.bandANco2Freq_kHz);

		/* Convert positive frequencies to their FTW equivalents */
		nco2Ftw = ((((uint64_t)nco2FreqUnsigned_kHz * POW_2_32) << 1) + outFs_kHz) /
			  (outFs_kHz << 1); /* integer rounding */

		/* Check FTW is not more than 32 bits */
		if (nco2Ftw >= POW_2_32) {
			return talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
						FR_ERR_SETUPNCOSHIFTER_FTW_OVRG, retVal, TALACT_ERR_CHECK_PARAM);
		}

		retVal = talSetRxNcoFtw(device, FR_RX1_NCO2A, (uint32_t)nco2Ftw);
		IF_ERR_RETURN(retVal);

		retVal = talSetRxNcoFtw(device, FR_RX2_NCO2A, (uint32_t)nco2Ftw);
		IF_ERR_RETURN(retVal);

		/* Update NCO2 FTW words */
		ncoUpdateFtw |= 0x44;
	}

	halError = talSpiWriteByte(device->devHalInfo,
				   FR9009_ADDR_RX_NCO_FTW_UPDATE_CONTROL, ncoUpdateFtw);
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	return retVal;
}

uint32_t FR9009_programDualBandLnaGainTable( fr9009Device_t *device,
		fr9009DualBandLnaGainTable_t *gainTablePtr, uint8_t numGainIndexesInTable,
		fr9009RxChannels_t rxChannel)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	fRHALErr_t halError = FRHAL_OK;
	uint8_t registerValue = 0;
	uint8_t gtConfigReg = 0;
	uint8_t count = 0;

	static const uint8_t MAX_LNA_GAIN_TABLE_ENTRIES = 4;
	static const uint8_t EN_TABLECLK = 0x05;
	static const uint8_t EN_WRSTB = 0x02;

	if ((rxChannel == FR_RX1) ||
	    (rxChannel == FR_RX2) ||
	    (rxChannel == FR_RX1RX2)) {
		/* Verify Rx profile is valid before allowing to set the Rx gain table, this
		 * ensures the device->rx structure pointers are valid
		 */
		if ((device->devStateInfo.profilesValid  & RX_PROFILE_VALID) == 0) {
			return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
							  FR_ERR_DUALBAND_LNA_TABLE_INV_PROFILE, retVal, TALACT_ERR_CHECK_PARAM);
		}

		/* Check for proper mapping for gain table and passed table number of indexes */
		if ((numGainIndexesInTable > MAX_LNA_GAIN_TABLE_ENTRIES) ||
		    (numGainIndexesInTable == 0)) {
			return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
							  FR_ERR_DUALBAND_LNA_TABLE_INV_INDEX, retVal, TALACT_ERR_CHECK_PARAM);
		}
	} else {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
						  FR_ERR_DUALBAND_LNA_TABLE_INV_CHANNEL, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if (gainTablePtr == NULL) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
						  FR_ERR_DUALBAND_LNA_TABLE_NULL_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* starting the gain table clock and read from gain table address bits */
	gtConfigReg = ((uint8_t)rxChannel << 3) | EN_TABLECLK;
	halError = talSpiWriteByte(device->devHalInfo,
				   FR9009_ADDR_GAIN_TABLE_CONFIGURATION, gtConfigReg);
	retVal = talApiErrHandler(device,FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	/* The write strobe to gain table configuration register */
	gtConfigReg |= EN_WRSTB;

	/* Load gain table */
	for (count = 0; count < numGainIndexesInTable; count ++) {
		halError = talSpiWriteByte(device->devHalInfo,
					   FR9009_ADDR_GAIN_TABLE_WORD_ADDRESS, count);
		retVal = talApiErrHandler(device,FR_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN(retVal);

		registerValue = ((gainTablePtr[count].dualbandGain & 0x3F) << 2) |
				(gainTablePtr[count].dualbandControl & 0x03);
		halError = talSpiWriteByte(device->devHalInfo,
					   FR9009_ADDR_GAIN_TABLE_WORD_DATA_DUALBAND, registerValue);
		retVal = talApiErrHandler(device,FR_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN(retVal);

		halError = talSpiWriteByte(device->devHalInfo,
					   FR9009_ADDR_GAIN_TABLE_CONFIGURATION, gtConfigReg);
		retVal = talApiErrHandler(device,FR_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN(retVal);
	}

	/* disabling the gain table clock */
	halError = talSpiWriteByte(device->devHalInfo,
				   FR9009_ADDR_GAIN_TABLE_CONFIGURATION, 0x00);
	retVal = talApiErrHandler(device,FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	/* Set Max LNA indices in Rx1 AGC to match the number of gain table indices */
	if ((rxChannel == FR_RX1) ||
	    (rxChannel == FR_RX1RX2)) {
		halError = talSpiWriteByte(device->devHalInfo, FR9009_ADDR_RX1_SLOWLOOP_CONFIG,
					   (uint8_t)(numGainIndexesInTable-1));
		retVal = talApiErrHandler(device,FR_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN(retVal);
	}

	/* Set Max LNA indices in Rx2 AGC to match the number of gain table indices */
	if ((rxChannel == FR_RX2) ||
	    (rxChannel == FR_RX1RX2)) {
		halError = talSpiWriteByte(device->devHalInfo, FR9009_ADDR_RX2_SLOWLOOP_CONFIG,
					   (uint8_t)(numGainIndexesInTable-1));
		retVal = talApiErrHandler(device,FR_ERRHDL_HAL_SPI, halError, retVal,
					  TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN(retVal);
	}

	return (uint32_t)retVal;
}

uint32_t FR9009_setGainTableExtCtrlPins(fr9009Device_t *device,
					fr9009RxChannels_t rxChannel, uint8_t enable3p3vGpios)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	fRHALErr_t halError = FRHAL_OK;
	uint8_t curr3p3SrcCtrl = 0x00;
	uint16_t currGpio3p3Oe = 0;
	uint16_t usedGpio3p3pins = 0;
	uint16_t gpio3p3FreeMask = 0;
	uint16_t gpio3p3UsedMask = 0;
	static const uint8_t GPIO3P3_RX1_EXT_CTRL_WORD_EN = 0x04;
	static const uint8_t GPIO3P3_RX2_EXT_CTRL_WORD_EN = 0x40;
	static const uint8_t GPIO3P3_RX1_EXT_CTRL_WORD_OE = 0x0F;
	static const uint8_t GPIO3P3_RX2_EXT_CTRL_WORD_OE = 0xF0;
	static const uint8_t GPIO3P3_LOWERBYTE_LOWERNIBBLE_SRCCTRL_MASK = 0x0F;
	static const uint8_t GPIO3P3_LOWERBYTE_UPPERNIBBLE_SRCCTRL_MASK = 0xF0;
	static const uint8_t ENABLE_EXT_CTRL_WORD_GPIOS = 0x01;
	static const uint32_t SWTEST_SIMULATE_RX1_GPIO3P3_INP = 0x01;
	static const uint32_t SWTEST_SIMULATE_RX2_GPIO3P3_INP = 0x02;

	/*Ensure that the requested channel is valid*/
	if ((rxChannel != FR_RX1) && (rxChannel != FR_RX2)
	    && (rxChannel != FR_RX1RX2)) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
						  FR_ERR_SETEXTWORDCTRLGPIO_INV_CHANNEL,
						  retVal, TALACT_ERR_CHECK_PARAM);
	}

	/*Ensure that if channel Rx1 is requested, it is initialized*/
	if (((((uint8_t)rxChannel & (uint8_t)FR_RX1) == (uint8_t)FR_RX1) &&
	     ((device->devStateInfo.initializedChannels & (uint8_t)FR_RX1) !=
	      (uint8_t)FR_RX1))) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
						  FR_ERR_SETEXTWORDCTRLGPIO_UNINITIALIZED_RX1,
						  retVal, TALACT_ERR_CHECK_PARAM);
	}

	/*Ensure that if channel Rx2 is requested, it is initialized*/
	if (((((uint8_t)rxChannel & (uint8_t)FR_RX2) == (uint8_t)FR_RX2) &&
	     ((device->devStateInfo.initializedChannels & (uint8_t)FR_RX2) !=
	      (uint8_t)FR_RX2))) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
						  FR_ERR_SETEXTWORDCTRLGPIO_UNINITIALIZED_RX2,
						  retVal, TALACT_ERR_CHECK_PARAM);
	}

	/*Verify that the GPIO pins requested are not already in use*/

	/* Read current configuration of GPIO 3.3s */
	halError = talSpiReadByte(device->devHalInfo,
				  FR9009_ADDR_GPIO_3P3V_LOWER_BYTE_SOURCE_CONTROL, &curr3p3SrcCtrl);
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN(retVal);

	retVal = (talRecoveryActions_t)FR9009_getGpio3v3Oe(device, &currGpio3p3Oe);
	IF_ERR_RETURN(retVal);

	usedGpio3p3pins = device->devStateInfo.usedGpio3p3pins;

	if(device->devStateInfo.swTest == SWTEST_SIMULATE_RX1_GPIO3P3_INP) {
		curr3p3SrcCtrl = 0;
		usedGpio3p3pins |= 0x01;
	}

	if(device->devStateInfo.swTest == SWTEST_SIMULATE_RX2_GPIO3P3_INP) {
		curr3p3SrcCtrl = 0;
		usedGpio3p3pins |= (0x01 << 4);
	}

	/*Check that if
	 * Channel requested for Ext Ctrl Word GPIO output is Rx1
	 * GPIO 3p3 [3:0] pins are already in use
	 * GPIO 3p3 [3:0] pins are not used by this feature
	 * then throw an exception
	 * */
	if ((((uint8_t)rxChannel & (uint8_t)FR_RX1) == (uint8_t)FR_RX1) &&
	    ((currGpio3p3Oe & GPIO3P3_RX1_EXT_CTRL_WORD_OE) == GPIO3P3_RX1_EXT_CTRL_WORD_OE)
	    &&
	    ((curr3p3SrcCtrl & GPIO3P3_LOWERBYTE_LOWERNIBBLE_SRCCTRL_MASK) !=
	     GPIO3P3_RX1_EXT_CTRL_WORD_EN)) {
		return talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
					FR_ERR_SETEXTWORDCTRLGPIO_GPIO_IN_USE_RX1,
					retVal, TALACT_ERR_CHECK_PARAM);
	}

	if ((((uint8_t)rxChannel & (uint8_t)FR_RX1) == (uint8_t)FR_RX1) &&
	    ((usedGpio3p3pins & GPIO3P3_RX1_EXT_CTRL_WORD_OE) != 0) &&
	    ((curr3p3SrcCtrl & GPIO3P3_LOWERBYTE_LOWERNIBBLE_SRCCTRL_MASK) !=
	     GPIO3P3_RX1_EXT_CTRL_WORD_EN)) {
		return talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
					FR_ERR_SETEXTWORDCTRLGPIO_GPIO_IN_USE_RX1,
					retVal, TALACT_ERR_CHECK_PARAM);
	}

	/*Check that if
	 * Channel requested for Ext Ctrl Word GPIO output is Rx2
	 * GPIO 3p3 [7:4] pins are already in use
	 * GPIO 3p3 [7:4] pins are not used by this feature
	 * then throw an exception
	 * */
	if ((((uint8_t)rxChannel & (uint8_t)FR_RX2) == (uint8_t)FR_RX2) &&
	    ((currGpio3p3Oe & GPIO3P3_RX2_EXT_CTRL_WORD_OE) == GPIO3P3_RX2_EXT_CTRL_WORD_OE)
	    &&
	    ((curr3p3SrcCtrl & GPIO3P3_LOWERBYTE_UPPERNIBBLE_SRCCTRL_MASK) !=
	     GPIO3P3_RX2_EXT_CTRL_WORD_EN)) {
		return talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
					FR_ERR_SETEXTWORDCTRLGPIO_GPIO_IN_USE_RX2,
					retVal, TALACT_ERR_CHECK_PARAM);
	}

	if ((((uint8_t)rxChannel & (uint8_t)FR_RX2) == (uint8_t)FR_RX2) &&
	    ((usedGpio3p3pins & GPIO3P3_RX2_EXT_CTRL_WORD_OE) != 0) &&
	    ((curr3p3SrcCtrl & GPIO3P3_LOWERBYTE_UPPERNIBBLE_SRCCTRL_MASK) !=
	     GPIO3P3_RX2_EXT_CTRL_WORD_EN)) {
		return talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
					FR_ERR_SETEXTWORDCTRLGPIO_GPIO_IN_USE_RX2,
					retVal, TALACT_ERR_CHECK_PARAM);
	}

	/*Clear current GPIO configuration and setup again if enable requested*/
	if ((rxChannel == FR_RX1) || (rxChannel == FR_RX1RX2)) {
		/* Free GPIO3P3[3:0] if used */
		gpio3p3FreeMask |= GPIO3P3_RX1_EXT_CTRL_WORD_OE;
		curr3p3SrcCtrl &= ~GPIO3P3_LOWERBYTE_LOWERNIBBLE_SRCCTRL_MASK;

		if (enable3p3vGpios >= ENABLE_EXT_CTRL_WORD_GPIOS) {
			/* Enable GPIO3P3[3:0] if requested */
			gpio3p3UsedMask |= GPIO3P3_RX1_EXT_CTRL_WORD_OE;
			curr3p3SrcCtrl |= GPIO3P3_RX1_EXT_CTRL_WORD_EN;
		}
	}

	if ((rxChannel == FR_RX2) || (rxChannel == FR_RX1RX2)) {
		/* Free GPIO3P3[7:4] if used */
		gpio3p3FreeMask |= GPIO3P3_RX2_EXT_CTRL_WORD_OE;
		curr3p3SrcCtrl &= ~GPIO3P3_LOWERBYTE_UPPERNIBBLE_SRCCTRL_MASK;

		if (enable3p3vGpios >= ENABLE_EXT_CTRL_WORD_GPIOS) {
			/* Enable GPIO3P3[7:4] if requested */
			gpio3p3UsedMask |= GPIO3P3_RX2_EXT_CTRL_WORD_OE;
			curr3p3SrcCtrl |= GPIO3P3_RX2_EXT_CTRL_WORD_EN;
		}
	}

	usedGpio3p3pins = device->devStateInfo.usedGpio3p3pins & ~gpio3p3FreeMask;
	usedGpio3p3pins |= gpio3p3UsedMask;

	/*Update the Source Control Register*/
	halError = talSpiWriteByte(device->devHalInfo,
				   FR9009_ADDR_GPIO_3P3V_LOWER_BYTE_SOURCE_CONTROL, curr3p3SrcCtrl);
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

	return (uint32_t)retVal;
}
#endif
/**
rxDecPower_mdBFS: -rxDecPower_mdBFS*1000 为dBFS值
*/
uint32_t FR9009_getRxDecPower(fr9009Device_t *device, fr9009RxChannels_t rxChannel, int32_t *rxDecPower_mdBFS)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	fRHALErr_t halError = FRHAL_OK;
	uint32_t regdat = 0;
	uint32_t thres = 0;
	uint32_t rxDecPower = 0;
	uint8_t div = 0;
	uint8_t shiftnum = 0;

#if FR9009_VERBOSE
	halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK,"FR9009_getRxDecPower()\n");
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal,TALACT_WARN_RESET_LOG);
#endif
	/* null pointer check */
	if (rxDecPower_mdBFS == NULL) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
						  FR_ERR_INV_RX_DEC_POWER_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}
	retVal = FR9009_armSpiCmd_readWords(device, APB_RX_PATH_TOP_BASEADDR + 0x8, &regdat, 1);
	IF_ERR_RETURN_U32(retVal);
	div = (regdat >> 12) & 0x3;
	retVal = FR9009_armSpiCmd_readWords(device, APB_RX_AGC_BASEADDR + 0x60, &regdat, 1);
	IF_ERR_RETURN_U32(retVal);
	shiftnum = (regdat) & 0xF;
	retVal = FR9009_armSpiCmd_readWords(device, APB_RX_AGC_BASEADDR + 0x44, &regdat, 1);
	IF_ERR_RETURN_U32(retVal);
	thres = (regdat >> 10) & 0x1FFFFF;
	if (rxChannel == FR_RX1) {
		/* Read Rx1 Dec Power register */
		retVal = FR9009_armSpiCmd_readWords(device, APB_RX_AGC_BASEADDR + 0x80, &regdat, 1);
		IF_ERR_RETURN_U32(retVal);
	} else if (rxChannel == FR_RX2) {
		/* Read Rx2 Dec Power register */
		retVal = FR9009_armSpiCmd_readWords(device, APB_RX_AGC_BASEADDR + 0x9C, &regdat, 1);
		IF_ERR_RETURN_U32(retVal);
	} else {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
						  FR_ERR_GETRXDECPOWER_INV_CHANNEL, retVal, TALACT_ERR_CHECK_PARAM);
	}
	rxDecPower = regdat / ((thres >> (22 - 2 * shiftnum + div)));
	/* Check that rx profile is valid in current config */
	if (((device->devStateInfo.profilesValid & RX_PROFILE_VALID) == 0)
	    || (device->devStateInfo.initializedChannels & (uint32_t)rxChannel) == 0) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
						  FR_ERR_GETRXDECPOWER_INV_PROFILE, retVal, TALACT_ERR_CHECK_PARAM);
	}	
	// printf("\r\nthred: 0x%X shiftnum : %d div:0x%x regdat: 0x%08x\r\n", thres, shiftnum, div, regdat);	
	// printf("\r\n((thres >> (2 * shiftnum))): 0x%X rxDecPower : %d div:0x%x regdat: 0x%08x\r\n", ((thres >> (2 * shiftnum))), rxDecPower, div, regdat);	
	*rxDecPower_mdBFS =  (int32_t)(10*log10f(rxDecPower)*1000 - 72247);

	return (uint32_t)retVal;
}

uint32_t fr9009_setRxIfconv(fr9009Device_t *device, fr9009RxChannels_t ch, uint8_t hb_bypass, fr9009IfConvCfg_t* pIfconvCfg)
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
    retVal = FR9009_armSpiCmd_writeField(device, APB_RX_IFCONV_BASEADDR + 0x8, reg, mask, 0);
	IF_ERR_RETURN_U32(retVal);
	if(bandsel & 0x1){
		retVal = FR9009_armSpiCmd_writeField(device, APB_RX_IFCONV_BASEADDR + 0xC, 0x20002000, 0xffffffff, 0);//A gain
		IF_ERR_RETURN_U32(retVal);
		retVal = FR9009_armSpiCmd_writeField(device, APB_RX_IFCONV_BASEADDR + 0x14, (uint32_t)fe_word_A, 0xffffffff, 0);
		IF_ERR_RETURN_U32(retVal);
		retVal = FR9009_armSpiCmd_writeField(device, APB_RX_IFCONV_BASEADDR + 0x24, (uint32_t)be_word_A, 0xffffffff, 0);
		IF_ERR_RETURN_U32(retVal);
	}
	if(bandsel & 0x2){
		retVal = FR9009_armSpiCmd_writeField(device, APB_RX_IFCONV_BASEADDR + 0x10, 0x20002000, 0xffffffff, 0);//B gain
		IF_ERR_RETURN_U32(retVal);
		retVal = FR9009_armSpiCmd_writeField(device, APB_RX_IFCONV_BASEADDR + 0x18, (uint32_t)fe_word_B, 0xffffffff, 0);
		IF_ERR_RETURN_U32(retVal);
		retVal = FR9009_armSpiCmd_writeField(device, APB_RX_IFCONV_BASEADDR + 0x28, (uint32_t)be_word_B, 0xffffffff, 0);
		IF_ERR_RETURN_U32(retVal);
	}
	if(FR_RX1RX2 & ch){
    	retVal = FR9009_armSpiCmd_writeField(device, APB_RX_PATH_TOP_BASEADDR + 0x1C, (bandsel == 0) << 14, 1<<14,0);
		IF_ERR_RETURN_U32(retVal);
	}
	return retVal;
}

