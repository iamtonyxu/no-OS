/**
 * \file headless.c
 *
 * \brief Contains example code for user integration with their application
 *
 * Copyright 2023-2027 BRDevices Inc.
 * for more information see the "LICENSE.txt" file in this zip file.
 *
 */
#include "fr9009.h"
#include "fr9009_jesd204.h"
#include "fr9009_arm.h"
#include "fr9009_radioctrl.h"
#include "fr9009_cals.h"
	
#include "fr9009_config.h"
#include "fr9009_error.h"
	/****< Insert User Includes Here >***/
#include <stdio.h>
#include "fr_hal.h"
#include "headless.h"

#include "fr9009_firmware.h"
struct fr_hal frhal[DEVICE_NUMS] = {
	{
		.devIndex = FRHAL_CHIP0,
	},
};

fr9009Device_t brDev[DEVICE_NUMS] = {
	{
		.devHalInfo = &frhal[0],
		.devStateInfo = {0},
	},
};

enum FR9009_INIT_STEP_t{
	FR_STEP_OPEN_HW = 1,
	FR_STEP_DEV_RESET,
	FR_STEP_DEV_SPI_SET,
	FR_STEP_ARM_CLK_INIT,
	FR_STEP_ARM_CODELOAD,
	FR_STEP_DEV_DERESET,
	FR_STEP_ARM_CHECKSUM,
	FR_STEP_BB_CLK_SET,
	FR_STEP_PARAM_INIT,
	FR_STEP_VERSION_SHOW,
	FR_STEP_CH_MANUAL_EN,
	FR_STEP_DIG_SYNC,
	FR_STEP_LOFREQ_SET,
	FR_STEP_LO_STATUS_CHECK,
	FR_STEP_INITCALI,
	FR_STEP_WAIT_CALI_DONE,
	FR_STEP_LIF_INIT,
	FR_STEP_MCS_LIF_SETUP,
	FR_STEP_MCS_SETUP,
	FR_STEP_MCS_LIF_OFF,
	FR_STEP_MCS_WAIT_OK,
	FR_STEP_FRAMER_SETUP,
	FR_STEP_DEFRAMER_SETUP,
	FR_STEP_FRAMER_STATUS_CHECK,
	FR_STEP_DEFRAMER_STATUS_CHECK,
	FR_STEP_ENDSET,
	FR_STEP_OFF = 0xFFFF,//此步骤无效，结束标志
};

#define UNLOCK_RETURN	0x80000000
/**
 * @description: 设置本振频率
 * @return {*}
 * @param {fr9009Device_t} device  设备结构体指针
 * @param {fr9009RfPllName_t} pllName 选择本振名称，参考结构体
 * @param {uint64_t} rfPllLoFrequency_Hz 设置的本振频率，单位Hz
 */
int32_t FR9009_PLLSetWithRetry(fr9009Device_t  *device, fr9009RfPllName_t pllName, uint64_t rfPllLoFrequency_Hz, uint8_t retry_times)
{
	int ret = SUCCESS;
	uint8_t chip = ((struct fr_hal*)device->devHalInfo)->devIndex;
	uint8_t pllLockStatus = 0;
	uint8_t pll_lock_mask = 1 << pllName;
	// uint8_t retry_times = 2;
	int i = 0;
	for(i = 0; i < retry_times; i++){
		ret = FR9009_armSpiCmd_pll_freq_set(device, pllName, rfPllLoFrequency_Hz);
		if(ret != SUCCESS){
			UART_Printf("set chip %d lo 0x%x freq err\r\n", chip, pllName);
			goto error_1;
		}
		FRHAL_wait_us(device->devHalInfo, 10000);
		ret = FR9009_getPllsLockStatus(device, &pllLockStatus);
		if(ret != SUCCESS){
			UART_Printf("get chip %d lo status err\r\n", chip);
			goto error_1;

		}
		if((pllLockStatus & pll_lock_mask) < pll_lock_mask){
			continue;
		}else{
			break;
		}
	}
	if((pllLockStatus & pll_lock_mask) < pll_lock_mask){
		ret = UNLOCK_RETURN;
		UART_Printf("chip %d lo 0x%x not lock 0x%08X\r\n", chip, pllName, pllLockStatus);
	}
error_1:
    return ret;
}
/**
 * @description: 
 * @return {*}
 * @param {uint16_t} chip_index 指定芯片重新建链， 0xffff是所有芯片
 */
int32_t fr9009_JesdRelink(uint16_t chip_index)
{
	uint32_t talAction = TALACT_NO_ACTION;
	uint8_t chip = chip_index == 0xFFFF ? 0 :  chip_index;
	uint8_t chip_nums = chip_index == 0xFFFF ? DEVICE_NUMS : 1;
	fr9009Device_t  *device = &brDev[chip];
	uint32_t framerStatus = 0;
	uint32_t deframerStatus = 0;
	if(DEVICE_NUMS == 1){
		chip_index = 0;
	}
	for(chip = 0; chip < chip_nums; chip ++){ // dev struct init
		if(chip_nums == 1){
			chip = chip_index;
		}
		device = &brDev[chip];
		fr9009FramerSel_t framerSel = (device->devStateInfo.initializedChannels & 0x3) ? device->devStateInfo.rxframerSel :  FR_FRAMER_NONE;
		if((device->devStateInfo.initializedChannels & 0xC) != 0 && device->devStateInfo.orxframerSel != FR_FRAMER_NONE){
			framerSel = framerSel == device->devStateInfo.orxframerSel ? framerSel : FR_FRAMER_A_AND_B;
		}
		/***************************************************/
		/**** Enable  FR9009 JESD204B Framer ***/
		/***************************************************/
		talAction = FR9009_enableFramerLink(device, FR_FRAMER_A_AND_B, 0);
		if (talAction != TALACT_NO_ACTION) {
			UART_Printf("error: FR9009_%d_enableFramerLink() framerA failed\r\n", chip_index);
			// goto error_1;
		}
		if((device->devStateInfo.initializedChannels & 0xF) != 0){
			talAction = FR9009_enableFramerLink(device, framerSel, 1);
			if (talAction != TALACT_NO_ACTION) {
				UART_Printf("error: FR9009_%d_enableFramerLink() framerA failed\r\n", chip_index);
				// goto error_1;
			}
			/*************************************************/
			/**** Enable SYSREF to FR9009 JESD204B Framer ***/
			/*************************************************/
			talAction = FR9009_enableSysrefToFramer(device, FR_FRAMER_A, 1);
			if (talAction != TALACT_NO_ACTION) {
				UART_Printf("error: FR9009_%d_enableSysrefToFramer() framerA failed\r\n", chip_index);
				// goto error_1;
			}
		}
		/***************************************************/
		/**** Enable  FR9009 JESD204B Deframer ***/
		/***************************************************/
		talAction = FR9009_enableDeframerLink(device, FR_DEFRAMER_A, 0);
		if (talAction != TALACT_NO_ACTION) {
			UART_Printf("error: FR9009_%d_enableDeframerLink() failed\r\n", chip_index);
		}

		if((device->devStateInfo.initializedChannels & 0x30) != 0){
			talAction = FR9009_enableDeframerLink(device, FR_DEFRAMER_A, 1);
			if ((talAction != TALACT_NO_ACTION) && (talAction != TALACT_ERR_RESET_JESD204DEFRAMERA)) {
				UART_Printf("error: FR9009_%d_enableDeframerLink() failed\r\n", chip_index);
				// goto error_1;
			}
			/***************************************************/
			/**** Enable SYSREF to FR9009 JESD204B Deframer ***/
			/***************************************************/
			talAction = FR9009_enableSysrefToDeframer(device, FR_DEFRAMER_A, 1);
			if (talAction != TALACT_NO_ACTION) {
				UART_Printf("error: FR9009_%d_enableSysrefToDeframer() failed\r\n", chip_index);
				// goto error_1;
			}
		}
		/************************************/
		/**** Check FR9009 Framer Status ***/
		/************************************/
		if((device->devStateInfo.initializedChannels & 0xF) != 0){
			talAction = FR9009_readFramerStatus(device, FR_FRAMER_A, &framerStatus);
			if (talAction != TALACT_NO_ACTION) {
				UART_Printf("error: FR9009_%d_readFramerStatus() framerA failed\r\n", chip_index);
				// goto error_1;
			}
			// UART_Printf("FR_FRAMER_A status : 0x%X\r\n", framerStatus);
			UART_Printf("FR_%d_FRAMER status :0x%08x\r\n", chip_index, framerStatus);
		}
		/************************************/
		/**** Check FR9009 Framer Status ***/
		/************************************/
		if((device->devStateInfo.initializedChannels & 0x30) != 0){
			talAction = FR9009_readDeframerStatus(device, FR_DEFRAMER_A, &deframerStatus);
			if (talAction != TALACT_NO_ACTION) {
				UART_Printf("error: FR9009_%d_readDeframerStatus() failed\r\n", chip_index);
				// goto error_1;
			}
			UART_Printf("FR_%d_DEFRAMER status:0x%08x\r\n", chip_index, deframerStatus);
		}
	}
	return SUCCESS;
}

int32_t	fr9009_rf_endsettings(fr9009Device_t *device, fr9009Init_t *pInitPara)
{	
	int ret = 0;
	CHANNEL_t ch;	
	uint8_t chip_index = ((struct fr_hal*)device->devHalInfo)->devIndex;
	if(pInitPara->rx.rxGainCtrl.gainMode == FR_MGC_PIN){
		fr9009RxGainCtrlPin_t rxGainCtrlPin;
		rxGainCtrlPin.rxGainDecPin = pInitPara->rx.rxGainCtrl.rx1GainDecPin;
		rxGainCtrlPin.rxGainIncPin = pInitPara->rx.rxGainCtrl.rx1GainIncPin;
		rxGainCtrlPin.enable = 1;
		ret = FR9009_setRxGainCtrlPin(device, FR_RX1, &rxGainCtrlPin);
		if(ret != SUCCESS){
			UART_Printf("FR9009_setRxGainCtrlPinfail return\r\n");
			goto error_1;
		}
		rxGainCtrlPin.rxGainDecPin = pInitPara->rx.rxGainCtrl.rx2GainDecPin;
		rxGainCtrlPin.rxGainIncPin = pInitPara->rx.rxGainCtrl.rx2GainIncPin;
		ret = FR9009_setRxGainCtrlPin(device, FR_RX2, &rxGainCtrlPin);
		if(ret != SUCCESS){
			UART_Printf("FR9009_setRxGainCtrlPinfail return\r\n");
			goto error_1;
		}
	}
	//tdd mode
	if(0){
		ret = FR9009_radioOn(device);
		if(ret != SUCCESS){
			UART_Printf("FR9009_radioOn fail return\r\n");
			goto error_1;
		}
		fr9009GpioShortPinSel_t orxpin = {
			.enable = 1,
			.gpioPinSel = FR_GPIO_04,
		};
		ret = FR9009_setRadioCtlPinMode(device, FR_ORX1, &orxpin);
		if(ret != SUCCESS){
			UART_Printf("FR9009_setRadioCtlPinMode fail return\r\n");
			goto error_1;
		}
		orxpin.gpioPinSel = FR_GPIO_05,
		ret = FR9009_setRadioCtlPinMode(device, FR_ORX2, &orxpin);
		if(ret != SUCCESS){
			UART_Printf("FR9009_setRadioCtlPinMode fail return\r\n");
			goto error_1;
		}
	}
	if(pInitPara->fhm.fhmEnable){
		ret = FR9009_FhmStart(device, 1);
		if(ret != SUCCESS){
			UART_Printf("fr9009 fhm start fail,return\r\n");
			goto error_1;
		}
	}
	/***************************************射频直采配置*****************************************/
	ch = (pInitPara->rx.rxProfile.rfDirectModeEn[0] != 0 ?  CHANNEL_1 : 0)
			| (pInitPara->rx.rxProfile.rfDirectModeEn[1] != 0 ?  CHANNEL_2 : 0);		
	if(ch){
		ret = FR9009_armSpiCmd_RFDirect_mode_set(device, CHANNEL_TYPE_RX, ch, 1);
		if(ret != SUCCESS){
			UART_Printf("FR9009_armSpiCmd_RFDirect_mode_set fail\r\n");
		}
	}
	ch = (pInitPara->obsRx.orxProfile.rfDirectModeEn[0] != 0 ?  CHANNEL_1 : 0)
			| (pInitPara->obsRx.orxProfile.rfDirectModeEn[1] != 0 ?  CHANNEL_2 : 0);		
	if(ch){
		ret = FR9009_armSpiCmd_RFDirect_mode_set(device, CHANNEL_TYPE_ORX, ch, 1);
		if(ret != SUCCESS){
			UART_Printf("FR9009_armSpiCmd_RFDirect_mode_set fail\r\n");
		}
	}
	ch = (pInitPara->tx.txProfile.rfDirectModeEn[0] != 0 ?  CHANNEL_1 : 0)
			| (pInitPara->tx.txProfile.rfDirectModeEn[1] != 0 ?  CHANNEL_2 : 0);		
	if(ch){
		ret = FR9009_armSpiCmd_RFDirect_mode_set(device, CHANNEL_TYPE_TX, ch, 1);
		if(ret != SUCCESS){
			UART_Printf("FR9009_armSpiCmd_RFDirect_mode_set fail\r\n");
		}
	}
	/***************************************ddc 及 ifconv 配置*****************************************/
	if(pInitPara->rx.rxProfile.rxIfConvMode[0] != FR_IFCONV_BYPASS || pInitPara->rx.rxProfile.rxIfConvMode[1] != FR_IFCONV_BYPASS){//rx ch1 ch2 共用ifconv 开关
		ret = fr9009_setRxIfconv(device, FR_RX1, pInitPara->rx.rxProfile.rxIfConvMode[0] <= FR_IFCONV_SHIFTONLY ? 1 : 0, &pInitPara->rx.rxProfile.rxIfConvCfg[0]);
		if(ret != SUCCESS){
			UART_Printf("fr9009_%d_setRxIfconv fail\r\n", chip_index);
		}
		ret = fr9009_setRxIfconv(device, FR_RX2, pInitPara->rx.rxProfile.rxIfConvMode[1] <= FR_IFCONV_SHIFTONLY ? 1 : 0, &pInitPara->rx.rxProfile.rxIfConvCfg[1]);
		if(ret != SUCCESS){
			UART_Printf("fr9009_%d_setRxIfconv fail\r\n", chip_index);
		}
	}
	if(pInitPara->tx.txProfile.txIfConvMode[0] != FR_IFCONV_BYPASS){
		ret = fr9009_setTxIfconv(device, FR_TX1, pInitPara->tx.txProfile.txIfConvMode[0] <= FR_IFCONV_SHIFTONLY ? 1 : 0, &pInitPara->tx.txProfile.txIfConvCfg[0]);
		if(ret != SUCCESS){
			UART_Printf("fr9009_%d_setTxIfconv fail\r\n", chip_index);
		}
	}
	if(pInitPara->tx.txProfile.txIfConvMode[1] != FR_IFCONV_BYPASS){
		ret = fr9009_setTxIfconv(device, FR_TX2, pInitPara->tx.txProfile.txIfConvMode[1] <= FR_IFCONV_SHIFTONLY ? 1 : 0, &pInitPara->tx.txProfile.txIfConvCfg[1]);
		if(ret != SUCCESS){
			UART_Printf("fr9009_%d_setTxIfconv fail\r\n", chip_index);
		}
	}
	/***************************************如果使能 orx配置*****************************************/
	if(0){//#if defined(EN_ORX_CH)
		fr9009RxORxChannels_t rxOrxChannel = device->devStateInfo.initializedChannels & 0xF;
		fr9009TxChannels_t txChannel = (device->devStateInfo.initializedChannels >> 4) & 0x3;
		ret = FR9009_setRxTxEnable(device, rxOrxChannel, txChannel);
		if(ret != SUCCESS){
			UART_Printf("FR9009_setRxTxEnable fail\r\n");
		}
	}
	/***************************************设置tx PA 保护模式与配置*****************************************/
	if(0){//#if defined(TX_PA_PROTECTION_EN)
		uint32_t perr;
		ret = FR9009_armSpiCmd_tx_PowerCtrl_set(device, 0x3, 1, 6000, 0x4000, 0xff);
		if(ret != SUCCESS){
			UART_Printf("FR9009_armSpiCmd_tx_PowerCtrl_set fail\r\n");
		}
		ret = FR9009_armSpiCmd_tx_PowerCtrl_ErrGet(device, &perr);
		if(ret != SUCCESS){
			UART_Printf("FR9009_armSpiCmd_tx_PowerCtrl_ErrGet fail\r\n");
		}
		UART_Printf("fr9009power err: 0x%08X\r\n", perr);
		ret = FR9009_armSpiCmd_tx_PowerCtrl_ErrReset(device, 1);
		if(ret != SUCCESS){
			UART_Printf("FR9009_armSpiCmd_tx_PowerCtrl_ErrReset fail\r\n");
		}
	}
	/***************************************外部补偿平坦度配置*****************************************/
	if(0){//#if defined(FLATNESS_EXT_EN)
		int32_t tx1_flatness_pwr[]={10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000};
		int32_t tx2_flatness_pwr[]={10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000};
		ret = FR9009_armSpiCmd_ch_signal_ripple_param_set(device, CHANNEL_TYPE_TX, CHANNEL_1, tx1_flatness_pwr, 16);
		if(ret != SUCCESS){
			UART_Printf("FR9009_signal_ripple_param_set fail\r\n");
		}
		ret = FR9009_armSpiCmd_ch_signal_ripple_param_set(device, CHANNEL_TYPE_TX, CHANNEL_2, tx2_flatness_pwr, 16);
		if(ret != SUCCESS){
			UART_Printf("FR9009_signal_ripple_param_set fail\r\n");
		}
	}
	if(0){//#if defined(FLATNESS_DEFAULT_EN)
		ret = FR9009_armSpiCmd_flatness_default_set(device, CHANNEL_TYPE_RX | CHANNEL_TYPE_TX, CHANNEL_1_2);
		if(ret != SUCCESS){
			UART_Printf("FR9009_flatness_default_set fail\r\n");
		}
	}
	if(0){
		uint8_t fir_len;
		uint8_t symmetric = 1;
		uint8_t even = 1;
		int16_t * coefs = NULL;
		const int16_t cfirCoefs_100MHz[62] = {
			-63, -54, 12, 27, 19, -42, -26, 20, 54, -5, -57, -33, 54, 61, -24, -88,
			-16, 89, 69, -66, -112, 13, 135, 57, -121, -130, 67, 182, 22, -194, -128, 149,
			224, -48, -278, -95, 263, 248, -164, -369, -12, 413, 236, -345, -457, 151, 612, 157,
			-633, -534, 463, 905, -63, -1170, -591, 1203, 1533, -807, -2950, -621, 6388, 12760
		};
		const int16_t cfirCoefs_50MHz[62] = {
			-55, -1, 15, 36, 51, 50, 31, -1, -34, -51, -43, -10, 35, 70, 78, 48,
			-8, -69, -104, -92, -34, 49, 119, 141, 97, 0, -110, -180, -171, -78, 65, 194,
			245, 183, 24, -167, -301, -306, -162, 80, 315, 429, 348, 85, -259, -529, -580, -352,
			91, 571, 858, 772, 272, -496, -1223, -1534, -1129, 97, 1980, 4114, 5967, 7042
		};
		coefs = (int16_t *)cfirCoefs_100MHz;
		fir_len = 62;
		FR9009_armSpiCmd_cfir_coefs_set(device, 0, symmetric, even, fir_len, coefs);
		coefs = (int16_t *)cfirCoefs_50MHz;
		fir_len = 62;
		FR9009_armSpiCmd_cfir_coefs_set(device, 1, symmetric, even, fir_len, coefs);
	}
	/***************************************使用外部本振配置*****************************************/
	if(pInitPara->clocks.rfPllUseExternalLo){
		FR9009_setExtLoCfg(device, 0x1, FR_EXTLO_RFPLLVCO_DIV2);
	}
	/***************************************使用AGC配置*****************************************/
#if 0
	extern fr9009AgcCfg_t rxAgcCtrl;
	FR9009_setupRxAgc(device, &rxAgcCtrl); 
	FR9009_setRxGainControlMode(device, FR_AGCFAST); 
#endif
	return SUCCESS;

error_1:
    UART_Printf("fr9009 %d initialize step5 fail,return\r\n",((struct fr_hal*)device->devHalInfo)->devIndex);
	return FAILURE;
}

int32_t fr9009_rf_setup(fr9009Device_t  *device, fr9009Init_t *pInitPara, uint16_t step)
{
	uint32_t talAction = TALACT_NO_ACTION;
	uint8_t pllLockStatus = 0;
	static uint32_t chip_checksum[DEVICE_NUMS] = {0};
	uint8_t chip_index = ((struct fr_hal*)device->devHalInfo)->devIndex;
	/*******************************/
	/**** Talise Initialization ***/
	/*******************************/
	switch (step)
	{
		case FR_STEP_OPEN_HW:{
			UART_Printf("\r\n=======================FR9009_%d_INITIALIZE============= \r\n", chip_index);    
			/*Open FR9009 Hw Device*/
			talAction = FR9009_openHw(device);
			if(talAction != TALACT_NO_ACTION) {
				UART_Printf("error: FR9009_%d_openHw() failed\r\n",chip_index);
				goto error_1;
			}
		}break;
		case FR_STEP_DEV_RESET:{
			/* Toggle RESETB pin on FR9009 device */
			talAction = FR9009_resetDevice(device);
			if (talAction != TALACT_NO_ACTION) {
				UART_Printf("error: FR9009_%d_resetDevice() failed\r\n", chip_index);
				goto error_1;
			}
		}break;
		case FR_STEP_DEV_SPI_SET:{
			talAction = FR9009_setSpiSettings(device, &pInitPara->spiSettings);
			if (talAction != TALACT_NO_ACTION) {
				UART_Printf("error: FR9009_%d_setSpiSettings() failed\r\n", chip_index);
				goto error_1;
			}
		}break;
		case FR_STEP_ARM_CLK_INIT:{
			talAction = FR9009_initArmclk(device, pInitPara);
			if (talAction != TALACT_NO_ACTION) {
				UART_Printf("error: FR9009_%d_initArm() failed\r\n", chip_index);
				goto error_1;
			}
		}break;
		case FR_STEP_ARM_CODELOAD:{
			uint32_t hw_version = 0;
			uint32_t *bin_code;
			uint32_t bin_wordsize;			
			talAction = FR9009_getDeviceRev(device, &hw_version);
			if (talAction != TALACT_NO_ACTION) {
				UART_Printf("error: FR9009_%d_getDeviceRev failed\r\n", chip_index);
				goto error_1;
			}
			bin_code = (uint32_t *)FR9009_FIRMWARE;
			bin_wordsize = sizeof(FR9009_FIRMWARE)/4;	
			chip_checksum[chip_index] = FR9009_FIRMWARE[bin_wordsize - 1];			
			/*******************************************************/
			/**** Prepare FR9009 Arm binary and Load Arm and	****/
			/**** Stream processor Binaryes 					****/
			/*******************************************************/
			talAction = FR9009_loadArmFromBinary(device, bin_code, bin_wordsize);
			if (talAction != TALACT_NO_ACTION) {
				UART_Printf("error: FR9009_%d_loadArmFromBinary() failed\r\n", chip_index);
				goto error_1;
			}
		}//break;
		case FR_STEP_DEV_DERESET:{
			talAction = FR9009_DeResetDevice(device);
			if (talAction != TALACT_NO_ACTION) {
				UART_Printf("error: FR9009_%d_DeResetDevice() failed\r\n", chip_index);
				goto error_1;
			}
		}break;
		case FR_STEP_ARM_CHECKSUM:{			
			uint32_t checksum = chip_checksum[chip_index];
			talAction = FR9009_verifyArmChecksum(device, checksum);
			if (talAction != TALACT_NO_ACTION) {
				UART_Printf("error: FR9009_%d_verifyArmChecksum() failed : %d\r\n", chip_index, talAction);
				goto error_1;
			}else{
				UART_Printf("FR9009_%d_verifyArmChecksum() ok\r\n", chip_index);
			}
#if defined(FR9009_LOG_TORAM) 
			FR9009_armSpiCmd_log_opt_set(device, 1);
#endif
		}break;
		case FR_STEP_BB_CLK_SET:{
			uint8_t retry_bb_pll_times = 5;
			talAction = FR9009_PLLSetWithRetry(device, FR_CLK_PLL, (uint64_t)pInitPara->clocks.clkPllVcoFreq_kHz * 1000, retry_bb_pll_times);
			if (talAction != TALACT_NO_ACTION) {
				UART_Printf("error: FR9009_%d_BBPLL set() failed : %d\r\n", chip_index, talAction);
				goto error_1;
			}
#if defined(FR9009_LOG_TORAM) 
			uint32_t buf[512];
			FR9009_armSpiCmd_log_read_from_ram(device, buf, 512);
			UART_Printf("\r\n%s\r\n", (char *)buf);
#endif
			UART_Printf("\r\nFR9009 %d BB PLL OK\r\n", chip_index);
		}break;
		case FR_STEP_PARAM_INIT:{
			/* FR9009_initialize() loads the FR9009 device data structure
			* settings for the Rx/Tx/ORx profiles, FIR filters, digital
			* filter enables, calibrates the CLKPLL, loads the user provided Rx
			* gain tables, and configures the JESD204b serializers/framers/deserializers
			* and deframers.
			*/
			talAction = FR9009_initialize(device, pInitPara);//TODO in func addc other dig
			if (talAction != TALACT_NO_ACTION) {
				UART_Printf("error: FR9009_%d_initialize() failed %d\r\n", chip_index, talAction);
				goto error_1;
			}
		}break;
		case FR_STEP_VERSION_SHOW:{
			fr9009ArmVersionInfo_t talArmVersionInfo;
			uint32_t api_vers[4];
			FR9009_getArmVersion_v2(device, &talArmVersionInfo);
			FR9009_getApiVersion(device,
						&api_vers[0], &api_vers[1], &api_vers[2], &api_vers[3]);
			UART_Printf("fr9009_%d: API Vers: %u.%u.%u.%u\r\n",chip_index,api_vers[0], api_vers[1], api_vers[2], api_vers[3]);				
			UART_Printf("\tDevice Revision: %d\r\n\tFirmware Vers: %u.%u.%u\r\n",device->devStateInfo.deviceSiRev, talArmVersionInfo.majorVer,
				talArmVersionInfo.minorVer, talArmVersionInfo.rcVer);
			UART_Printf("\tArm Building Time: %s\r\n", talArmVersionInfo.build_time_str);
			UART_Printf("\tArm Customer Info: %s\r\n", talArmVersionInfo.customer_info_str);
		}break;
		case FR_STEP_CH_MANUAL_EN:{
			fr9009RxORxChannels_t rxOrxChannel = device->devStateInfo.initializedChannels & 0x3;
			fr9009TxChannels_t txChannel = (device->devStateInfo.initializedChannels >> 4) & 0x3;
			talAction = FR9009_setRxTxEnable(device, rxOrxChannel, txChannel);
			if(talAction != TALACT_NO_ACTION){
				UART_Printf("error: FR9009_%d_setRxTxEnable() failed\r\n", chip_index);
				goto error_1;
			} 
		}break;
		case FR_STEP_DIG_SYNC:{
			talAction = FR9009_armSpiCmd_sync_dig_en(device, 1);
			if(talAction != TALACT_NO_ACTION){
				UART_Printf("error: FR9009_%d_dig sync Enable() failed\r\n", chip_index);
				goto error_1;
			} 
		}break;
		case FR_STEP_LOFREQ_SET:{
			/*******************************/
			/**Set LO Frequencies 		***/
			/*******************************/
			uint64_t rf_hz = pInitPara->clocks.rfpllFreq_hz;
			uint64_t orf_hz = pInitPara->clocks.orfpllFreq_hz;
			if(rf_hz != 0){
				talAction = FR9009_setRfPllFrequency(device, FR_RF_PLL, rf_hz);
				if(talAction != TALACT_NO_ACTION){
					UART_Printf("error: FR9009_%d_setRfPllFrequency() failed\r\n", chip_index);
					goto error_1;
				}
			}
			if(orf_hz != 0){
				talAction = FR9009_setRfPllFrequency(device, FR_ORF_PLL, orf_hz);
				if (talAction != TALACT_NO_ACTION) {
					UART_Printf("error: FR9009_%d_setRfPllFrequency() failed\r\n", chip_index);
					goto error_1;
				}
			}
		}break;
		case FR_STEP_LO_STATUS_CHECK:{
			#define MAX_CHECK_TIMES	20
			uint32_t pll_lock_mask = 0x1 | (pInitPara->clocks.rfpllFreq_hz ? 0x2 : 0) | (pInitPara->clocks.orfpllFreq_hz ? 0x4: 0);
			int try_times = MAX_CHECK_TIMES;
			for(try_times = 0; try_times < MAX_CHECK_TIMES; try_times++){
				talAction = FR9009_getPllsLockStatus(device, &pllLockStatus);
				if(((pllLockStatus & pll_lock_mask) < pll_lock_mask) || talAction){
					// UART_Printf("error: FR9009_%d PLL not locked pllLockStatus 0x%08X, try : %d \r\n", chip_index, pllLockStatus, try_times);
					FRHAL_wait_us(device->devHalInfo, 3000);
					continue;
				}else{
					break;
				}
			}
			if(try_times == MAX_CHECK_TIMES){
				UART_Printf("error: FR9009_%d PLL not locked pllLockStatus 0x%08X\r\n", chip_index, pllLockStatus);
				goto error_1;
			}
		}break;
		case FR_STEP_INITCALI:{
			CHANNEL_t tx_ch = (device->devStateInfo.initializedChannels >> 4) & 0x3;
			CHANNEL_t rx_ch = (pInitPara->rx.rxProfile.rfDirectModeEn[0] | pInitPara->rx.rxProfile.rfDirectModeEn[1]) ? 0 : (device->devStateInfo.initializedChannels & 0x3);
			CHANNEL_t orx_ch = (pInitPara->obsRx.orxProfile.rfDirectModeEn[0] | pInitPara->obsRx.orxProfile.rfDirectModeEn[1]) ? 0 : ((device->devStateInfo.initializedChannels >> 2) & 0x3);
			CHANNEL_t tx1_lp2orx_ch = 0;
			CHANNEL_t tx2_lp2orx_ch = 0;	
			uint32_t initCalMask = (FR_DC_OFFSET | FR_PATH_DELAY | FR_TX_LO_LEAKAGE_INTERNAL | FR_TX_QEC_INIT | FR_RX_LO_DELAY | FR_RX_QEC_INIT| FR_ORX_LO_DELAY| FR_ORX_QEC_INIT);
			if(tx_ch == 0 && rx_ch == 0 && orx_ch == 0){
				break;
			}
			if(pInitPara->rx.rxProfile.rxMixer.freq_Hz){
				initCalMask |= FR_CALI_NCO_CALS;
				initCalMask &= ~(FR_RX_QEC_INIT);
			}	
			if(pInitPara->tx.txProfile.txMixer.freq_Hz){
				initCalMask |= FR_CALI_NCO_CALS;
				initCalMask &= ~(FR_TX_QEC_INIT|FR_PATH_DELAY);
			}
			if(((device->devStateInfo.initializedChannels >> 2) & 0x3) == 0x1){
				tx1_lp2orx_ch = 0x1;
				tx2_lp2orx_ch = 0x1;
			}else if(((device->devStateInfo.initializedChannels >> 2) & 0x3) == 0x2){
				tx1_lp2orx_ch = 0x2;
				tx2_lp2orx_ch = 0x2;
			}else{
				tx1_lp2orx_ch = 0x1;
				tx2_lp2orx_ch = 0x2;
			}
			/****************************************************/
			/**** Run FR9009 ARM Initialization Calibrations ***/
			/****************************************************/
			/**
			 * 如果使用外环tx校准，可以两个tx通道分别校准一次，即一次调用FR9009_runInitCals时，配置initCalMask只选择tx校准项，
			 * 使用外环校准FR_TX_INIT_EXTERNAL必须选择上
			*/
			talAction = FR9009_runInitCals(device, tx_ch, rx_ch, orx_ch, tx1_lp2orx_ch, tx2_lp2orx_ch, initCalMask);
			if(talAction != TALACT_NO_ACTION){
				UART_Printf("error: FR9009_%d_runInitCals() failed\r\n", chip_index);
				goto error_1;
			}
		}break;
		case FR_STEP_WAIT_CALI_DONE:{
			uint8_t errorFlag = 0;
			uint32_t wait_time_ms = pInitPara->fhm.predict_cali == 1 ? ((pInitPara->fhm.fhmRf.caliFreqNums + pInitPara->fhm.fhmORf.caliFreqNums) * 5000) : 0;
			talAction = FR9009_waitInitCals(device, 20000 + wait_time_ms, &errorFlag);
			if(errorFlag || talAction){
				UART_Printf("error:FR9009_%d Calibrations not completed\r\n", chip_index);
				goto error_1;
			}else{
				/*< user code - Calibrations completed successfully > */
				UART_Printf("fr9009_%d: Calibrations completed successfully\r\n", chip_index);
			}
		}break;
		case FR_STEP_LIF_INIT:{
			uint8_t ch_type = 0;
			if(pInitPara->rx.rxProfile.rxMixer.freq_Hz){
				talAction = FR9009_armSpiCmd_LIF_freq_set(device, CHANNEL_TYPE_RX, CHANNEL_1_2, pInitPara->rx.rxProfile.rxMixer.freq_Hz);
				if(talAction != TALACT_NO_ACTION){
					UART_Printf("error: FR9009_%d_LIF_freq_set() failed\r\n", chip_index);
					goto error_1;
				}
				ch_type = CHANNEL_TYPE_RX;
			}
			if(pInitPara->tx.txProfile.txMixer.freq_Hz){
				talAction = FR9009_armSpiCmd_LIF_freq_set(device, CHANNEL_TYPE_TX, CHANNEL_1_2, pInitPara->rx.rxProfile.rxMixer.freq_Hz);
				if(talAction != TALACT_NO_ACTION){
					UART_Printf("error: FR9009_%d_LIF_freq_set() failed\r\n", chip_index);
					goto error_1;
				}
				ch_type |= CHANNEL_TYPE_TX;
			}
			if(ch_type){
				talAction = FR9009_armSpiCmd_LIF_mode_set(device, ch_type, CHANNEL_1_2, 1);
				if(talAction != TALACT_NO_ACTION){
					UART_Printf("error: FR9009_%d_LIF_mode_set() failed\r\n", chip_index);
					goto error_1;
				}
			}
		}break;
		case FR_STEP_MCS_LIF_SETUP:{
			uint8_t ch_type = 0;
			if(pInitPara->rx.rxProfile.rxMixer.freq_Hz){
				ch_type = CHANNEL_TYPE_RX;
			}
			if(pInitPara->tx.txProfile.txMixer.freq_Hz){
				ch_type |= CHANNEL_TYPE_TX;
			}
			if(ch_type){
				talAction = FR9009_armSpiCmd_sync_nco(device, ch_type, 1, 0x1);
				if(talAction != TALACT_NO_ACTION){
					UART_Printf("error: FR9009_%d_sync_nco() failed\r\n", chip_index);
					goto error_1;
				}
			}

		}break;
		case FR_STEP_MCS_LIF_OFF:{
			uint8_t ch_type = 0;
			if(pInitPara->rx.rxProfile.rxMixer.freq_Hz){
				ch_type = CHANNEL_TYPE_RX;
			}
			if(pInitPara->tx.txProfile.txMixer.freq_Hz){
				ch_type |= CHANNEL_TYPE_TX;
			}
			if(ch_type){
				talAction = FR9009_armSpiCmd_sync_nco(device, ch_type, 0, 0x8);
				if(talAction != TALACT_NO_ACTION){
					UART_Printf("error: FR9009_%d_sync_nco() failed\r\n", chip_index);
					goto error_1;
				}
			}
		}break;
		case FR_STEP_MCS_SETUP:{
			uint8_t sync_step = pInitPara->rx.rxLoSource[0];
			uint32_t sync_type[3]={MCS_TYPE_RFPLL, MCS_TYPE_ORFPLL, MCS_TYPE_NCO};
			talAction = FR9009_armSpiCmd_sync_offAfterSync_set(device, 2);
			talAction = FR9009_armSpiCmd_sync_temp_track_en(device, 0, 1500);
			if(talAction != TALACT_NO_ACTION){
				UART_Printf("error: FR9009_%d_armSpiCmd_sync_temp_track_en() failed\r\n", chip_index);
				goto error_1;
			}
			talAction = FR9009_enableMultichipSync(device, 1,  sync_type[sync_step]);
			if(talAction != TALACT_NO_ACTION){
				UART_Printf("error: FR9009_%d_enableMultichipSync() failed\r\n", chip_index);
				goto error_1;
			}
		}break;
		case FR_STEP_MCS_WAIT_OK:{
			uint8_t mcsStatus= 0;
			int timeout_ms = 1000;
			do{
				talAction = FR9009_getMultiChipSyncStatus(device, &mcsStatus);
				if(talAction != TALACT_NO_ACTION){
					UART_Printf("error: FR9009_%d_getMultiChipSyncStatus() failed\r\n", chip_index);
					goto error_1;
				}
				if((mcsStatus & 0x6) == 0x0){				
					FRHAL_wait_us(device->devHalInfo, 5000);
					timeout_ms -= 5;
				}else{
					break;
				}
			}while (timeout_ms > 0);
			if(timeout_ms <= 0){
				UART_Printf("error: FR9009_%d Sync status err:0x%x\r\n", chip_index, mcsStatus);
				goto error_1;
			}else{
				UART_Printf("FR9009_%d Sync status 0x%x\r\n", chip_index, mcsStatus);				
			}			
		}break;
		case FR_STEP_FRAMER_SETUP:{
			fr9009FramerSel_t framerSel = (device->devStateInfo.initializedChannels & 0x3) ? device->devStateInfo.rxframerSel :  FR_FRAMER_NONE;
			if((device->devStateInfo.initializedChannels & 0xC) != 0 && device->devStateInfo.orxframerSel != FR_FRAMER_NONE){
				framerSel = framerSel == device->devStateInfo.orxframerSel ? framerSel : FR_FRAMER_A_AND_B;
			}
			/***************************************************/
			/**** Enable  FR9009 JESD204B Framer ***/
			/***************************************************/
			talAction = FR9009_enableFramerLink(device, FR_FRAMER_A_AND_B, 0);
			if (talAction != TALACT_NO_ACTION) {
				UART_Printf("error: FR9009_%d_enableFramerLink() framerA failed\r\n", chip_index);
				goto error_1;
			}
			if((device->devStateInfo.initializedChannels & 0xF) != 0){
				talAction = FR9009_enableFramerLink(device, framerSel, 1);
				if (talAction != TALACT_NO_ACTION) {
					UART_Printf("error: FR9009_%d_enableFramerLink() framerA failed\r\n", chip_index);
					goto error_1;
				}
				/*************************************************/
				/**** Enable SYSREF to FR9009 JESD204B Framer ***/
				/*************************************************/
				talAction = FR9009_enableSysrefToFramer(device, FR_FRAMER_A, 1);
				if (talAction != TALACT_NO_ACTION) {
					UART_Printf("error: FR9009_%d_enableSysrefToFramer() framerA failed\r\n", chip_index);
					goto error_1;
				}
			}
		}break;
		case FR_STEP_DEFRAMER_SETUP:{
			/***************************************************/
			/**** Enable  FR9009 JESD204B Deframer ***/
			/***************************************************/
			talAction = FR9009_enableDeframerLink(device, FR_DEFRAMER_A, 0);
			if (talAction != TALACT_NO_ACTION) {
				UART_Printf("error: FR9009_%d_enableDeframerLink() failed\r\n", chip_index);
			}

			if((device->devStateInfo.initializedChannels & 0x30) != 0){
				talAction = FR9009_enableDeframerLink(device, FR_DEFRAMER_A, 1);
				if ((talAction != TALACT_NO_ACTION) && (talAction != TALACT_ERR_RESET_JESD204DEFRAMERA)) {
					UART_Printf("error: FR9009_%d_enableDeframerLink() failed\r\n", chip_index);
					goto error_1;
				}
				/***************************************************/
				/**** Enable SYSREF to FR9009 JESD204B Deframer ***/
				/***************************************************/
				talAction = FR9009_enableSysrefToDeframer(device, FR_DEFRAMER_A, 1);
				if (talAction != TALACT_NO_ACTION) {
					UART_Printf("error: FR9009_%d_enableSysrefToDeframer() failed\r\n", chip_index);
					goto error_1;
				}
			}
		}break;
		case FR_STEP_FRAMER_STATUS_CHECK:{
			uint32_t framerStatus = 0;
			/************************************/
			/**** Check FR9009 Framer Status ***/
			/************************************/
			if((device->devStateInfo.initializedChannels & 0xF) != 0){
				talAction = FR9009_readFramerStatus(device, FR_FRAMER_A, &framerStatus);
				if (talAction != TALACT_NO_ACTION) {
					UART_Printf("error: FR9009_%d_readFramerStatus() framerA failed\r\n", chip_index);
					goto error_1;
				}
				// UART_Printf("FR_FRAMER_A status : 0x%X\r\n", framerStatus);
				UART_Printf("FR_%d_FRAMER status :0x%08x\r\n", chip_index, framerStatus);
				UART_Printf("\t[sync1,sync0]:\t\t\t\t0x%x\r\n", (framerStatus >> 28) & 0x3);
				UART_Printf("\t[mapB_sync mapA_sync]:\t\t\t0x%x\r\n", (framerStatus >> 24) & 0x3);
				UART_Printf("\tlane[3:0] fifo full err:\t\t0x%x\r\n", (framerStatus >> 8) & 0xF);
				UART_Printf("\tlane[3:0] fifo empty err:\t\t0x%x\r\n", (framerStatus >> 4) & 0xF);
				UART_Printf("\tSYSREF align error received by framerB:\t0x%x\r\n", (framerStatus >> 3) & 0x1);
				UART_Printf("\tSYSREF align error received by framerA:\t0x%x\r\n", (framerStatus >> 2) & 0x1);
				UART_Printf("\tSYSREF received by framerB:\t\t0x%x\r\n", (framerStatus >> 1) & 0x1);
				UART_Printf("\tSYSREF received by framerA:\t\t0x%x\r\n", (framerStatus >> 0) & 0x1);
			}
		}break;
		case FR_STEP_DEFRAMER_STATUS_CHECK:{
			uint32_t deframerStatus = 0;
			/************************************/
			/**** Check FR9009 Framer Status ***/
			/************************************/
			if((device->devStateInfo.initializedChannels & 0x30) != 0){
				talAction = FR9009_readDeframerStatus(device, FR_DEFRAMER_A, &deframerStatus);
				if (talAction != TALACT_NO_ACTION) {
					UART_Printf("error: FR9009_%d_readDeframerStatus() failed\r\n", chip_index);
					goto error_1;
				}
				UART_Printf("FR_%d_DEFRAMER status:0x%08x\r\n", chip_index, deframerStatus);
				UART_Printf("\tlanes fifo full:\t\t0x%x\r\n",(deframerStatus >> 28) & 0xF);
				UART_Printf("\trx_valid:\t\t\t0x%x\r\n",(deframerStatus >> 24) & 0x1);
				UART_Printf("\tlane[3:0] ilas config valid:\t0x%x\r\n",(deframerStatus >> 20) & 0xF);
				UART_Printf("\tlane[3:0] ifs ready:\t\t0x%x\r\n",(deframerStatus >> 16) & 0xF);
				UART_Printf("\tlane[3:0] not in table:\t\t0x%x\r\n",(deframerStatus >> 12) & 0xF);
				UART_Printf("\tlane[3:0] band disparity:\t0x%x\r\n",(deframerStatus >> 8) & 0xF);
				UART_Printf("\tlane[3:0] fifo empty:\t\t0x%x\r\n",(deframerStatus >> 4) & 0xF);
				UART_Printf("\tSYSREF Received:\t\t0x%x\r\n",(deframerStatus >> 1) & 0x1);
				UART_Printf("\tSysref phs Error:\t\t0x%x\r\n",(deframerStatus >> 0) & 0x1);
			}
		}break;
		case FR_STEP_ENDSET:{
			talAction = fr9009_rf_endsettings(device, pInitPara);
			if (talAction != TALACT_NO_ACTION) {
				UART_Printf("error: fr9009_%d_rf_endsettings() failed\r\n", chip_index);
				goto error_1;
			}
		}
		default:
			break;
	}
	return SUCCESS;

error_1:
    UART_Printf("fr9009 %d initialize step %d fail,return\r\n", chip_index, step);
	return step;
}

int32_t fr9009_rf_init(uint16_t chip_index, uint16_t *pInitStep, fr9009Init_t *pInitPara_new, fr9009Device_t  *tdevice)
{
	int i = 0;
	int32_t ret = SUCCESS;
	int32_t func_ret = SUCCESS;
	uint8_t chip = chip_index == 0xFFFF ? 0 :  chip_index;
	uint8_t chip_nums = chip_index == 0xFFFF ? DEVICE_NUMS : 1;
	int32_t init_ret[DEVICE_NUMS] = {0};
	fr9009Init_t *pInitPara = pInitPara_new == NULL ? &talInit : pInitPara_new;
	fr9009Device_t  *device = tdevice == NULL ?  &brDev[chip] : tdevice;
	uint16_t init_step_default[] = {
		FR_STEP_OPEN_HW,
		FR_STEP_DEV_RESET,
		FR_STEP_DEV_SPI_SET,
		FR_STEP_ARM_CLK_INIT,
		FR_STEP_ARM_CODELOAD,
		FR_STEP_ARM_CHECKSUM,
		FR_STEP_BB_CLK_SET,
		FR_STEP_PARAM_INIT,
		FR_STEP_VERSION_SHOW,
		FR_STEP_CH_MANUAL_EN,
		FR_STEP_DIG_SYNC,
		FR_STEP_LOFREQ_SET,
		FR_STEP_LO_STATUS_CHECK,
		FR_STEP_INITCALI,
		FR_STEP_WAIT_CALI_DONE,
		FR_STEP_LIF_INIT,
		FR_STEP_MCS_LIF_SETUP,
		FR_STEP_LOFREQ_SET,
		FR_STEP_MCS_LIF_OFF,
		FR_STEP_MCS_SETUP,
		FR_STEP_MCS_WAIT_OK,
		FR_STEP_FRAMER_SETUP,
		FR_STEP_DEFRAMER_SETUP,
		FR_STEP_FRAMER_STATUS_CHECK,
		FR_STEP_DEFRAMER_STATUS_CHECK,
		FR_STEP_ENDSET,
		FR_STEP_OFF,//此步骤无效，结束标志
	};
	uint16_t *init_step = pInitStep == NULL ? init_step_default : pInitStep;
	uint8_t step_len = sizeof(init_step_default)/sizeof(uint16_t)-1;
	for(i = 0; i < 100 && pInitStep != NULL; i++){
		if(init_step[i] == FR_STEP_OFF){
			step_len = i;
			break;
		}
	}
	if(DEVICE_NUMS == 1){
		chip_index = 0;
	}
	if(tdevice == NULL){
		for(chip = 0; chip < chip_nums; chip ++){ // dev struct init
			if(chip_nums == 1){
				chip = chip_index;
			}
			device = &brDev[chip];
			device->devHalInfo = &frhal[chip];
			((struct fr_hal*)device->devHalInfo)->devIndex = chip;
		}
	}
	for(i =0; i < step_len; i++){
		if(init_step[i] == FR_STEP_MCS_SETUP || init_step[i] == FR_STEP_MCS_WAIT_OK || init_step[i] == FR_STEP_DIG_SYNC
			|| init_step[i] == FR_STEP_MCS_LIF_SETUP || init_step[i] == FR_STEP_MCS_LIF_OFF){
			if(pInitPara->clocks.rfPllPhaseSyncMode == FR_RFPLLMCS_NOSYNC || DEVICE_NUMS == 1){
				continue; //配置中不需要同步，不设置同步步骤
			}
			if(init_step[i] == FR_STEP_MCS_SETUP || init_step[i] == FR_STEP_MCS_LIF_OFF){
				/*******************************************************/
				/**** 注意这里要先停掉sysref ***/
				/*******************************************************/
				/*** < User stop SYSREF Here > ***/
				FRHAL_sysrefReq(device->devHalInfo, __SYSREF_CONT_GLOBAL_OFF);
				if(init_step[i] == FR_STEP_MCS_LIF_OFF){
					FRHAL_wait_us(device->devHalInfo, 100);
				}
			}else if(init_step[i] == FR_STEP_DIG_SYNC){
				/*** < User Sends SYSREF Here > ***/
				FRHAL_sysrefReq(device->devHalInfo, __SYSREF_CONT_LOCAL_ON);
			}
		}else if(init_step[i] == FR_STEP_FRAMER_SETUP){
			if(pInitPara->clocks.rfPllPhaseSyncMode == FR_RFPLLMCS_NOSYNC || DEVICE_NUMS == 1){//在未进行同步的状态下，jesd建链前需要开启sysref
				/*** < User Sends SYSREF Here > ***/
				FRHAL_sysrefReq(device->devHalInfo, __SYSREF_CONT_LOCAL_ON);
			}
		}else if(init_step[i] == FR_STEP_DEFRAMER_SETUP){
				/*******************************************************/
				/**** 最好在这里reset fpga jesd ***/
				/*******************************************************/
		}
		for(chip = 0; chip < chip_nums; chip ++){
			if(chip_nums == 1){
				chip = chip_index;
			}
			if(tdevice == NULL){
				device = &brDev[chip];
			}
			if(init_ret[chip] < FR_STEP_BB_CLK_SET && init_ret[chip] > 0){
				continue; //  在BB clk lock之前出错，后边的过程不再执行
			}
			func_ret = fr9009_rf_setup(device, pInitPara, init_step[i]);
			if(func_ret != SUCCESS){
				if(init_ret[chip] == 0){
					init_ret[chip] = i + 1;					
				}
				UART_Printf("fr9009_%d initialize step %d fail\r\n", chip, func_ret);
				continue;
			}
		}
		if(init_step[i] == FR_STEP_MCS_SETUP){
			/*** < User Sends SYSREF Here > ***/
			FRHAL_sysrefReq(device->devHalInfo, __SYSREF_CONT_GLOBAL_ON);
			FRHAL_wait_us(device->devHalInfo, 5000);
		}		
	}
	for(chip = 0; chip < chip_nums; chip ++){
		if(chip_nums == 1){
			chip = chip_index;
		}
		if(init_ret[chip] == 0){
			UART_Printf("rf %d initialize success\r\n", chip);
		}else{
			UART_Printf("rf %d initialize err %d\r\n", chip, init_ret[chip]);	
			// ret |= init_ret[chip] << (chip * 5);//每颗chip占用5bit的错误码位置
		}
	}
	UART_Printf("date:%s %s\r\n",__DATE__, __TIME__);
    return ret;
}

int32_t rf_initialize(void)
{
	return fr9009_rf_init(0xFFFF, NULL, NULL, NULL);
}
int32_t adrv_rf_init(void)
{
    int ret = SUCCESS;
    ret = rf_initialize();    
    return ret;
}

//int main(void)
//{
//	return adrv_rf_init();
//}
