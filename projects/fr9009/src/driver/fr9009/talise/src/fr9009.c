/**
 * \file fr9009.c
 * \brief Contains top level functions to support initialization and ctrol of
 *         the FR9009 transceiver device.
 *
 * Copyright 2022 BriRadio.
 * Released under the FR9009 API license, for more information see the "LICENSE.txt" file in this zip file.
 */

/**
 * \mainpage Overview
 *
 * This document is intended for use by software engineering professionals
 * and includes detailed information regarding the data types and
 * function calls which comprise the FR9009 ANSI C API
 *
 * Copyright 2022 BriRadio.
 * Released under the FR9009 API license, for more information see the "LICENSE.txt" file in this zip file.
 *
 */

/**
 * \page Suggested Use
 * The purpose of the FR9009 Application Programming Interface (API) is to abstract the low-level
 * SPI configuration, control, and related calculations required for the FR9009 family of transceiver devices
 *
 * Add the included source code as required to your baseband processor software build. Please reference the integration document
 * as well for further instructions.
 */

/**
 * \page Architecture FR9009 API Software Architecture
 * \image html software_architecture.png
 */

/**
 * \page Folders FR9009 API Folder Structure
 * \image html folder_structure.png
 */

#include "fr9009.h"
#include "fr9009_reg_addr_macros.h"
#include "fr9009_arm.h"
#include "fr9009_hal.h"
#include "fr9009_user.h"
#include "fr9009_radioctrl.h"
#include "fr9009_error.h"
#include "fr9009_version.h"
#include "fr9009_rx.h"
#include "fr9009_tx.h"
#include "fr9009_jesd204.h"
#include "fr9009_cals.h"
#include "fr9009_gpio.h"
#include "fr9009.h"
#include "fr9009_config.h"

#define FR9009_VERBOSE 			1
#define FR9009_LOGGING 			0xF     /*LogLevel Set to All*/
#define FR9009_RESET_ON_ERR  	1   	/*API Reset on Severe Errors*/
#define FR9009_FIR_MAX_LEN 		64 		// Max Lenght of Channel Fir

static const uint32_t txProfileNum = 12;
static const uint32_t txProfileMatrix[12][4] = {
	/*--------code-------------bypass----------vld_sel_H----vld_sel_L--------*/						/**th1,2,3/5 th0 fir*/	
	{(  4<<8)|( 1<<0), (0x0<<28)|(0x12<<9), 0x00000000,	0x00000000  },	// totalIntpRatio =   4 = 2x2x1
	{(  8<<8)|( 1<<0), (0x2<<28)|(0x10<<9), 0x00000249,	0x09249249  },	// totalIntpRatio =   8 = 4x2x1
	{( 16<<8)|( 1<<0), (0x3<<28)|(0x10<<9), 0x09249492,	0x09249249*2},	// totalIntpRatio =  16 = 8x2x1
	{( 32<<8)|( 2<<0), (0x3<<28)|(0x10<<9), 0x0924949B,	0x09249249*3},	// totalIntpRatio =  32 = 8x2x2
	{( 64<<8)|( 4<<0), (0x3<<28)|(0x10<<9), 0x092494A4,	0x09249249*4},	// totalIntpRatio =  64 = 8x2x4
	{(128<<8)|( 8<<0), (0x3<<28)|(0x10<<9), 0x092494AD,	0x09249249*5},	// totalIntpRatio = 128	= 8x2x8
	{(  5<<8)|( 1<<0), (0x2<<28)|(0x0F<<9), 0x00000000,	0x00000000  },	// totalIntpRatio =   5	= 5x1x1
	{( 10<<8)|( 1<<0), (0x2<<28)|(0x0E<<9), 0x00000249,	0x09249249  },	// totalIntpRatio =  10	= 5x2x1
	{( 20<<8)|( 2<<0), (0x2<<28)|(0x0E<<9), 0x00000252,	0x09249249*2},	// totalIntpRatio =  20	= 5x2x2
	{( 40<<8)|( 4<<0), (0x2<<28)|(0x0E<<9), 0x0000025B,	0x09249249*3},	// totalIntpRatio =  40	= 5x2x4
	{( 80<<8)|( 8<<0), (0x2<<28)|(0x0E<<9), 0x00000264,	0x09249249*4},	// totalIntpRatio =  80	= 5x2x8
	{(160<<8)|(16<<0), (0x2<<28)|(0x0E<<9), 0x0000026D,	0x09249249*5},	// totalIntpRatio = 160	= 5x2x16
};

static const uint32_t rxProfileNum = 24;
static const uint32_t rxProfileMatrix[24][4] = {
	/*--------code--------------------bypass--vld_sel_H-vld_sel_L*/
	{(0<<24)|(  4<<8)|( 1<<0), (0xD<<2), 0x000  , 0x000000},	//  4 sub-ADC, totalDecRatio =   4  = 2x2x1
	{(0<<24)|(  8<<8)|( 1<<0), (0x9<<2), 0x249*1, 0x249200},	//  4 sub-ADC, totalDecRatio =   8  = 4x2x1
	{(0<<24)|( 16<<8)|( 1<<0), (0x1<<2), 0x249*2, 0x492448},	//  4 sub-ADC, totalDecRatio =  16  = 8x2x1
	{(0<<24)|( 32<<8)|( 2<<0), (0x1<<2), 0x249*3, 0x6DA448},	//  4 sub-ADC, totalDecRatio =  32  = 8x2x2
	{(0<<24)|( 64<<8)|( 4<<0), (0x1<<2), 0x249*4, 0x922448},	//  4 sub-ADC, totalDecRatio =  64  = 8x2x4
	{(0<<24)|(128<<8)|( 8<<0), (0x1<<2), 0x249*5, 0xB6A448},	//  4 sub-ADC, totalDecRatio = 128	= 8x2x8
	{(0<<24)|(  5<<8)|( 1<<0),(0x1E<<2), 0x000  , 0x000000},	//  4 sub-ADC, totalDecRatio =   5	= 5x1x1
	{(0<<24)|( 10<<8)|( 1<<0), (0xE<<2), 0x249*1, 0x249200},	//  4 sub-ADC, totalDecRatio =  10	= 5x2x1
	{(0<<24)|( 20<<8)|( 2<<0), (0xE<<2), 0x249*2, 0x491200},	//  4 sub-ADC, totalDecRatio =  20	= 5x2x2
	{(0<<24)|( 40<<8)|( 4<<0), (0xE<<2), 0x249*3, 0x6D9200},	//  4 sub-ADC, totalDecRatio =  40	= 5x2x4
	{(0<<24)|( 80<<8)|( 8<<0), (0xE<<2), 0x249*4, 0x921200},	//  4 sub-ADC, totalDecRatio =  80	= 5x2x8
	{(0<<24)|(160<<8)|(16<<0), (0xE<<2), 0x249*5, 0xB69200},	//  4 sub-ADC, totalDecRatio = 160	= 5x2x16
	{(1<<24)|(  2<<8)|( 1<<0), (0xF<<2), 0x000  , 0x000000},	//  2 sub-ADC, totalDecRatio =   2  = 2x1x1
	{(1<<24)|(  4<<8)|( 1<<0), (0xB<<2), 0x249*1, 0x249200},	//  2 sub-ADC, totalDecRatio =   4  = 2x2x1
	{(1<<24)|(  8<<8)|( 1<<0), (0x3<<2), 0x249*2, 0x492448},	//  2 sub-ADC, totalDecRatio =   8  = 4x2x1
	{(1<<24)|( 16<<8)|( 2<<0), (0x3<<2), 0x249*3, 0x6DA448},	//  2 sub-ADC, totalDecRatio =  16  = 4x2x2
	{(1<<24)|( 32<<8)|( 4<<0), (0x3<<2), 0x249*4, 0x922448},	//  2 sub-ADC, totalDecRatio =  32  = 4x2x4
	{(1<<24)|( 64<<8)|( 8<<0), (0x3<<2), 0x249*5, 0xB6A448},	//  2 sub-ADC, totalDecRatio =  64	= 4x2x8
	{(2<<24)|(  1<<8)|( 1<<0),(0x1F<<2), 0x000  , 0x000000},	//  1 sub-ADC, totalDecRatio =   1  = 1x1x1
	{(2<<24)|(  2<<8)|( 1<<0),(0x17<<2), 0x249*1, 0x249248},	//  1 sub-ADC, totalDecRatio =   2  = 2x1x1
	{(2<<24)|(  4<<8)|( 1<<0), (0x7<<2), 0x249*2, 0x492448},	//  1 sub-ADC, totalDecRatio =   4  = 2x2x1
	{(2<<24)|(  8<<8)|( 2<<0), (0x7<<2), 0x249*3, 0x6DA448},	//  1 sub-ADC, totalDecRatio =   8  = 2x2x2
	{(2<<24)|( 16<<8)|( 4<<0), (0x7<<2), 0x249*4, 0x922448},	//  1 sub-ADC, totalDecRatio =  16  = 2x2x4
	{(2<<24)|( 32<<8)|( 8<<0), (0x7<<2), 0x249*5, 0xB6A448},	//  1 sub-ADC, totalDecRatio =  32	= 2x2x8
};

uint32_t FR9009_openHw(fr9009Device_t *device )
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	fRHALErr_t halError = FRHAL_OK;

	FRHAL_setLogLevel(device->devHalInfo, FR9009_LOGGING);   
	halError = FRHAL_openHw(device->devHalInfo,100);
	if (halError != FRHAL_OK) {
		switch(halError) {
		case FRHAL_SPI_FAIL:
			return (uint32_t)talApiErrHandler(device, FR_ERRHDL_HAL_SPI,  halError, retVal, TALACT_ERR_RESET_SPI);
		case FRHAL_GPIO_FAIL:
			return (uint32_t)talApiErrHandler(device, FR_ERRHDL_HAL_GPIO, halError, retVal, TALACT_ERR_RESET_GPIO);
		case FRHAL_TIMER_FAIL:
			return (uint32_t)talApiErrHandler(device, FR_ERRHDL_HAL_WAIT, halError, retVal, TALACT_ERR_CHECK_TIMER);
		case FRHAL_GEN_SW:
		default:
			return (uint32_t)TALACT_ERR_CHECK_PARAM;
		}
	}

	return (uint32_t)retVal;
}

uint32_t FR9009_closeHw(fr9009Device_t *device)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	fRHALErr_t halError = FRHAL_OK;

	halError = FRHAL_closeHw(device->devHalInfo);

	if (halError != FRHAL_OK) {
		switch(halError) {
		case FRHAL_SPI_FAIL:
			return (uint32_t)talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal, TALACT_ERR_RESET_SPI);
		case FRHAL_GPIO_FAIL:
			return (uint32_t)talApiErrHandler(device, FR_ERRHDL_HAL_GPIO, halError, retVal, TALACT_ERR_RESET_GPIO);
		case FRHAL_TIMER_FAIL:
			return (uint32_t)talApiErrHandler(device, FR_ERRHDL_HAL_WAIT, halError, retVal, TALACT_ERR_CHECK_TIMER);
		case FRHAL_GEN_SW:
		default:
			return (uint32_t)TALACT_ERR_CHECK_PARAM;
		}
	}

	return (uint32_t)retVal;
}

uint32_t FR9009_resetDevice(fr9009Device_t *device)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	fRHALErr_t halError = FRHAL_OK;
	
#if FR9009_VERBOSE
		halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK, "FR9009_resetDevice()\n");
		retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal, TALACT_WARN_RESET_LOG);
#endif
	
		/* toggle RESETB on device with matching spi chip select index */
		halError = FRHAL_resetHw(device->devHalInfo);
		if (halError == FRHAL_WAIT_TIMEOUT)
		{
			/*Retry*/
			halError = FRHAL_resetHw(device->devHalInfo);
		}
	
		if (halError != FRHAL_OK)
		{
			retVal = talApiErrHandler(device, FR_ERRHDL_HAL_GPIO, halError, retVal, TALACT_ERR_RESET_GPIO);
		}

	device->devStateInfo.devState = FR_STATE_POWERONRESET;
//	delay_us(1000);
	return (uint32_t)retVal;
}

uint32_t FR9009_DeResetDevice(fr9009Device_t *device)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	fRHALErr_t halError = FRHAL_OK;
	uint32_t log_level = FR9009_MCU_LOG_LEVEV;//0:打开所有log， 10：关闭所有log
#if FR9009_VERBOSE
	halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK, "FR9009_DeResetDevice()\n");
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal, TALACT_WARN_RESET_LOG);
#endif

	halError = brSpiWriteWord(device->devHalInfo, 0xFFC, log_level);
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal, TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);
	halError = brSpiWriteWord(device->devHalInfo, FR9009_ADDR_SPI_INIT_RESET, 0);
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal, TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);
	// delay_ms(20);   
	return (uint32_t)retVal;
}

static talRecoveryActions_t talSetupNco(fr9009Device_t *device, uint32_t ftwRegAddr, uint32_t pwordRegAddr, fr9009NcoConfig_t ncoCfg, uint64_t ncoFs)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	fRHALErr_t halError = FRHAL_OK;

	uint32_t freqUnsign = ncoCfg.freq_Hz > 0 ? ncoCfg.freq_Hz : (ncoCfg.freq_Hz + ncoFs);
	uint32_t freqAbs = ncoCfg.freq_Hz > 0 ? (uint32_t)ncoCfg.freq_Hz : (uint32_t)(-ncoCfg.freq_Hz);

	uint64_t ftw64 = DIV_U64(((((uint64_t) freqUnsign) << 33) + ncoFs) , (ncoFs << 1)); /* integer rounding */
	uint32_t ftw = (uint32_t)ftw64;

	if(freqAbs*2 > ncoFs)
	{
		return TALACT_ERR_CHECK_PARAM;
	}

	halError = FR9009_armSpiCmd_writeWords(device, ftwRegAddr, &ftw, 1);
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal, TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = FR9009_armSpiCmd_writeWords(device, pwordRegAddr, &ncoCfg.phase, 1);
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal, TALACT_ERR_RESET_SPI);
	return retVal;
}

static talRecoveryActions_t talInitializeIfConv(fr9009Device_t *device, uint32_t chanSel, uint32_t regBaseAddr, fr9009IfConvMode_t mode, fr9009IfConvCfg_t cfg, uint32_t ncoClkSel)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;

	uint8_t fe_mix_bypass = 1;
	uint8_t hb_bypass = 1;
	uint8_t sum_en = cfg.bandSel & 0x3;
	uint8_t be_mix_bypass = 1;
	uint64_t ncoFs = device->devStateInfo.clocks.hsDigClkDiv4or5_Hz >> (ncoClkSel & 0x7);	
	uint32_t regVal = 0;
	uint32_t bitMask = 0;

	if(chanSel > 0x3)
		return TALACT_ERR_CHECK_PARAM;
	
	if(chanSel == 0)
		return TALACT_NO_ACTION;
	

	hb_bypass = ((cfg.bandSel & 0x3) == 0) || !(mode == FR_IFCONV_FILTERONLY || mode == FR_IFCONV_INT2 || mode == FR_IFCONV_DEC2);

	fe_mix_bypass = (cfg.bandAFe.freq_Hz == 0 || (cfg.bandSel & 0x1) == 0) && 
					(cfg.bandBFe.freq_Hz == 0 || (cfg.bandSel & 0x2) == 0);

	be_mix_bypass = (cfg.bandABe.freq_Hz == 0 || (cfg.bandSel & 0x1) == 0) && 
					(cfg.bandBBe.freq_Hz == 0 || (cfg.bandSel & 0x2) == 0);
	
	regVal = fe_mix_bypass 
			| (hb_bypass << 1)
			| (sum_en << 2)
			| (be_mix_bypass << 4);
	
	if(chanSel & 0x1) {
		regVal  |= regVal;
		bitMask |= 0x1F;
	}
	if(chanSel & 0x2) {
		regVal  |= regVal << 8;
		bitMask |= 0x1F << 8;
	}
	
	
	retVal = FR9009_armSpiCmd_writeField(device, regBaseAddr+8, regVal, bitMask, 0);
	IF_ERR_RETURN_U32(retVal);	
	retVal = talSetupNco(device, regBaseAddr+0x14, regBaseAddr+0x1C, cfg.bandAFe, ncoFs);
	IF_ERR_RETURN_U32(retVal);
	retVal = talSetupNco(device, regBaseAddr+0x18, regBaseAddr+0x20, cfg.bandBFe, ncoFs);
	IF_ERR_RETURN_U32(retVal);
	retVal = talSetupNco(device, regBaseAddr+0x24, regBaseAddr+0x2C, cfg.bandABe, ncoFs);
	IF_ERR_RETURN_U32(retVal);
	retVal = talSetupNco(device, regBaseAddr+0x28, regBaseAddr+0x30, cfg.bandBBe, ncoFs);
	IF_ERR_RETURN_U32(retVal);
	return retVal;
}

static talRecoveryActions_t talInitializeTxMixer(fr9009Device_t *device, fr9009NcoConfig_t ncoCfg)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint64_t ncoFs = device->devStateInfo.clocks.hsDigClkDiv2_Hz>>1; // DAC freq / 4	

	retVal = talSetupNco(device, APB_TX_PATH_TOP_BASEADDR+0x48, APB_TX_PATH_TOP_BASEADDR+0x44, ncoCfg, ncoFs);
	IF_ERR_RETURN_U32(retVal);

	if(ncoCfg.freq_Hz){
		retVal = FR9009_armSpiCmd_writeField(device, APB_TX_PATH_TOP_BASEADDR+0x54, 1, 1, 0); // set nco en
		IF_ERR_RETURN_U32(retVal);
	}
	
	return retVal;
}

static talRecoveryActions_t talInitializeRxMixer(fr9009Device_t *device, fr9009NcoConfig_t ncoCfg)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint64_t ncoFs = device->devStateInfo.clocks.hsDigClkDiv2_Hz>>1; // ADC freq / 4	

	retVal = talSetupNco(device, APB_RX_PATH_TOP_BASEADDR+0x44, APB_RX_PATH_TOP_BASEADDR+0x40, ncoCfg, ncoFs);
	IF_ERR_RETURN_U32(retVal);

	if(ncoCfg.freq_Hz){
		retVal = FR9009_armSpiCmd_writeField(device, APB_RX_PATH_TOP_BASEADDR+0x54, 1, 1, 0); // set nco en
		IF_ERR_RETURN_U32(retVal);
	}
	
	return retVal;
}

uint32_t FR9009_txdp_initialize(fr9009Device_t *device, fr9009Init_t *init)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;

	uint32_t 	profileIndex = 0;
	uint32_t 	regBypassVal = 0;
	uint32_t 	regVldSelValH = 0;
	uint32_t 	regVldSelValL = 0;
	uint32_t 	reg = 0;	

	/* Verify pointers to *device and *init are not NULL */
	if (device == NULL) {
		/* Can not write to log since log function requires device data structure */
		return (uint32_t)TALACT_ERR_CHECK_PARAM;
	}

	if (init == NULL) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_INIT_NULLPARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Tx profilesValid Check */
	if ((device->devStateInfo.profilesValid & TX_PROFILE_VALID) == 0) {
		return (uint32_t)TALACT_ERR_CHECK_PARAM;
	}
	
	profileIndex = device->devStateInfo.txProfileMtxIdx;

	regVldSelValH = txProfileMatrix[profileIndex][2];
	regVldSelValL = txProfileMatrix[profileIndex][3];
	// Common Module
	// retVal = talInitializeTxMixer(device, init->tx.txProfile.txMixer);
	// IF_ERR_RETURN_U32(retVal);	

	// retVal = talInitializeIfConv(device, init->tx.txChannels, APB_TX_IFCONV_BASEADDR, init->tx.txProfile.txIfConvMode, init->tx.txProfile.txIfConvCfg, (regVldSelValL >> 6)&0x7);
	// IF_ERR_RETURN_U32(retVal);

	// Channel 1 settings
	if (init->tx.txChannels & FR_TX1){		
		regBypassVal = txProfileMatrix[profileIndex][1];
		regBypassVal |= (1 << 0) | (1 << 5) | (1 << 6); 																			// bypass FLT2FIX, QEC_QFIR and PHSCMPS
		regBypassVal |= (init->tx.txProfile.txIfConvMode[0] == FR_IFCONV_BYPASS || init->tx.txProfile.txIfConvCfg[0].bandSel == 0) << 2;	// bypass IF_CONV	
		regBypassVal |= ((init->tx.txProfile.txFir.coefs == NULL) || (device->devStateInfo.txFirInterpolation == 1)) << 7;			// bypass TCFIR
		regBypassVal |= (init->tx.txProfile.txMixer.freq_Hz == 0) << 15;															// bypass TMIX
		regBypassVal |= (3 << 24); 							// set tmix dac IQ Sel
		// CFIR
		if ((init->tx.txProfile.txFir.numFirCoefs > 0) && init->tx.txProfile.txFir.coefs != NULL ) {
			retVal = FR9009_programFir(device, FR_TX1_FIR, &init->tx.txProfile.txFir);
			IF_ERR_RETURN_U32(retVal);	
		}
		// COMPLEX FIR
		if ((init->tx.txProfile.txFlatnessFir[0].numFirCoefs > 0) && init->tx.txProfile.txFlatnessFir[0].coefs_II != NULL ) {
			retVal = FR9009_programCplxFir(device, FR_TX1_CPLX_FIR, &init->tx.txProfile.txFlatnessFir[0]);
			IF_ERR_RETURN_U32(retVal);	
		}
		// tx analog attenuation
		retVal = (talRecoveryActions_t)FR9009_setTxAttenuation(device, FR_TX1, init->tx.tx1Atten_mdB);
		IF_ERR_RETURN_U32(retVal);
		// tx digital gain
		uint32_t regdat[4] = {0x00000C00, 0x00000C00};  				// 2.5dbs attenuation
		retVal = FR9009_armSpiCmd_writeWords(device, APB_TX_DGAIN_BASEADDR + 0x0C, regdat, 2);
		IF_ERR_RETURN_U32(retVal);	
		// din/dout vld sel
		reg = regVldSelValL + (init->tx.txProfile.txIfConvMode[0] == FR_IFCONV_INT2 ? 1 : 0);
		retVal = FR9009_armSpiCmd_writeWords(device, APB_TX_PATH_TOP_BASEADDR+0x8, &reg, 1); 
		IF_ERR_RETURN_U32(retVal);	
		retVal = FR9009_armSpiCmd_writeWords(device, APB_TX_PATH_TOP_BASEADDR+0xC, &regVldSelValH, 1);
		IF_ERR_RETURN_U32(retVal);
		// set bypass reg
		retVal = FR9009_armSpiCmd_writeWords(device, APB_TX_PATH_TOP_BASEADDR+0x18, &regBypassVal, 1);
		IF_ERR_RETURN_U32(retVal);
	}

	// Channel 2 settings
	if (init->tx.txChannels & FR_TX2){
		regBypassVal = txProfileMatrix[profileIndex][1];
		regBypassVal |= (1 << 0) | (1 << 5) | (1 << 6); 																			// bypass FLT2FIX, QEC_QFIR and PHSCMPS
		regBypassVal |= (init->tx.txProfile.txIfConvMode[1] == FR_IFCONV_BYPASS) << 2;	// bypass IF_CONV	
		regBypassVal |= ((init->tx.txProfile.txFir.coefs == NULL) && (device->devStateInfo.txFirInterpolation == 1)) << 7;			// bypass TCFIR
		regBypassVal |= (init->tx.txProfile.txMixer.freq_Hz == 0) << 15;															// bypass TMIX
		regBypassVal |= (3 << 24); 							// set tmix dac IQ Sel
		// CFIR
		if ((init->tx.txProfile.txFir.numFirCoefs > 0) && init->tx.txProfile.txFir.coefs != NULL ) {
			retVal = FR9009_programFir(device, FR_TX2_FIR, &init->tx.txProfile.txFir);
			IF_ERR_RETURN_U32(retVal);	
		}
		// COMPLEX FIR
		if ((init->tx.txProfile.txFlatnessFir[1].numFirCoefs > 0) && init->tx.txProfile.txFlatnessFir[1].coefs_II != NULL ) {
			retVal = FR9009_programCplxFir(device, FR_TX2_CPLX_FIR, &init->tx.txProfile.txFlatnessFir[1]);
			IF_ERR_RETURN_U32(retVal);	
		}
		// tx analog attenuation
		retVal = (talRecoveryActions_t)FR9009_setTxAttenuation(device, FR_TX2, init->tx.tx2Atten_mdB);
		IF_ERR_RETURN_U32(retVal);
		// tx digital gain
		uint32_t regdat[4] = {0x00000C00, 0x00000C00};  				// 2.5dbs attenuation
		retVal = FR9009_armSpiCmd_writeWords(device, APB_TX_DGAIN_BASEADDR + 0x14, regdat, 2);
		IF_ERR_RETURN_U32(retVal);		
		// din/dout vld sel
		reg = regVldSelValL + (init->tx.txProfile.txIfConvMode[1] == FR_IFCONV_INT2 ? 1 : 0);
		retVal = FR9009_armSpiCmd_writeField(device, APB_TX_PATH_TOP_BASEADDR+0x10, reg, device->devStateInfo.deviceSiRev == 1 ? 0x3FFFFE38UL : 0x3FFFFE3FUL, 0);
		IF_ERR_RETURN_U32(retVal);	
		retVal = FR9009_armSpiCmd_writeWords(device, APB_TX_PATH_TOP_BASEADDR+0x14, &regVldSelValH, 1);
		IF_ERR_RETURN_U32(retVal);
		// set bypass reg
		retVal = FR9009_armSpiCmd_writeWords(device, APB_TX_PATH_TOP_BASEADDR+0x1C, &regBypassVal, 1);
		IF_ERR_RETURN_U32(retVal);
	}
	return retVal;
}

uint32_t FR9009_rxdp_initialize(fr9009Device_t *device, fr9009Init_t *init)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;

	uint32_t 	profileIndex = device->devStateInfo.rxProfileMtxIdx;
	uint32_t 	regBypassVal = 0;
	uint32_t 	regVldSelValH = 0;
	uint32_t 	regVldSelValL = 0;
	uint32_t 	regVldSelValIfNco = 0;
	uint32_t 	reg = 0;
	
	/* Verify pointers to *device and *init are not NULL */
	if (device == NULL) {
		/* Can not write to log since log function requires device data structure */
		return (uint32_t)TALACT_ERR_CHECK_PARAM;
	}

	if (init == NULL) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_INIT_NULLPARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Rx profilesValid Check */
	if ((device->devStateInfo.profilesValid & RX_PROFILE_VALID) == 0) {
		return (uint32_t)TALACT_ERR_CHECK_PARAM;
	}

	regBypassVal = rxProfileMatrix[profileIndex][1];
	regVldSelValH = rxProfileMatrix[profileIndex][2];
	regVldSelValL = rxProfileMatrix[profileIndex][3];
	regVldSelValIfNco = 0; 		// ifconv_nco_sel

	regBypassVal |= (1 << 10) | (1 << 11); 																									// bypass QEC_QFIR and PHSCMPS
	regBypassVal |= (init->rx.rxProfile.rxIfConvMode[0] == FR_IFCONV_BYPASS) << 14; 	// bypass IF_CONV only when ORX and RX both don't use IF_CONV
	regBypassVal |= ((init->rx.rxProfile.rxFir.coefs == NULL) && (device->devStateInfo.rxFirDecimation == 1)) << 7;							// bypass RCFIR
	//regBypassVal |= (init->rx.rxProfile.rxMixer.freq_Hz == 0) << 0;																			// bypass RMIX
	regBypassVal |= 1 << 0;																			// bypass RMIX

	// Common Module
	// retVal = talInitializeRxMixer(device, init->rx.rxProfile.rxMixer);
	// IF_ERR_RETURN_U32(retVal);	
	
	// retVal = talInitializeIfConv(device, CHANNEL_1, APB_RX_IFCONV_BASEADDR, init->rx.rxProfile.rxIfConvMode[0], init->rx.rxProfile.rxIfConvCfg[0], regVldSelValIfNco);
	// IF_ERR_RETURN_U32(retVal);
	// retVal = talInitializeIfConv(device, CHANNEL_2, APB_RX_IFCONV_BASEADDR, init->rx.rxProfile.rxIfConvMode[1], init->rx.rxProfile.rxIfConvCfg[1], regVldSelValIfNco);
	// IF_ERR_RETURN_U32(retVal);

	// if (init->rx.rxProfile.rxFir.numFirCoefs > 0) {
	// 	retVal = (talRecoveryActions_t)FR9009_programFir(device, FR_RX1RX2_FIR, &init->rx.rxProfile.rxFir);
	// 	IF_ERR_RETURN_U32(retVal);
	// }

	// din/dout vld sel
	retVal = FR9009_armSpiCmd_writeWords(device, APB_RX_PATH_TOP_BASEADDR+0x8, &regVldSelValL, 1); 
	IF_ERR_RETURN_U32(retVal);	
	reg = regVldSelValH + (init->rx.rxProfile.rxIfConvMode[0] == FR_IFCONV_DEC2 ? 0x200 : 0);
	retVal = FR9009_armSpiCmd_writeWords(device, APB_RX_PATH_TOP_BASEADDR+0xC, &reg, 1);
	IF_ERR_RETURN_U32(retVal);	
	retVal = FR9009_armSpiCmd_writeWords(device, APB_RX_PATH_TOP_BASEADDR+0x18, &regVldSelValIfNco, 1);
	IF_ERR_RETURN_U32(retVal);

	// set bypass reg
	retVal = FR9009_armSpiCmd_writeWords(device, APB_RX_PATH_TOP_BASEADDR+0x1C, &regBypassVal, 1);
	IF_ERR_RETURN_U32(retVal);	
		
	// Channel 1 settings
	if (init->rx.rxChannels & FR_RX1){		
		// COMPLEX FIR
		if ((init->rx.rxProfile.rxFlatnessFir[0].numFirCoefs > 0) && init->rx.rxProfile.rxFlatnessFir[0].coefs_II != NULL ) {
			retVal = FR9009_programCplxFir(device, FR_RX1_CPLX_FIR, &init->rx.rxProfile.rxFlatnessFir[0]);
			IF_ERR_RETURN_U32(retVal);	
		}
		/* Load Rx gain table */
		retVal = (talRecoveryActions_t)FR9009_programRxGainTable(device, &rxGainTable[0], (sizeof(rxGainTable) / sizeof(fr9009RxGainTable_t)), 
									init->rx.rxGainCtrl.rx1MaxGainIndex, init->rx.rxGainCtrl.rx1MinGainIndex, FR_RX1);
		IF_ERR_RETURN_U32(retVal);
		// MGC
		if(init->rx.rxGainCtrl.gainMode == FR_MGC || init->rx.rxGainCtrl.gainMode == FR_MGC_PIN){
			retVal = (talRecoveryActions_t)FR9009_setRxManualGain(device, FR_RX1, init->rx.rxGainCtrl.rx1GainIndex);
			IF_ERR_RETURN_U32(retVal);
		}
		/*ADC mode setting*/
		// TODO
	}

	// Channel 2 settings
	if (init->rx.rxChannels & FR_RX2){
		// COMPLEX FIR
		if ((init->rx.rxProfile.rxFlatnessFir[1].numFirCoefs > 0) && init->rx.rxProfile.rxFlatnessFir[1].coefs_II != NULL ) {
			retVal = FR9009_programCplxFir(device, FR_RX2_CPLX_FIR, &init->rx.rxProfile.rxFlatnessFir[1]);
			IF_ERR_RETURN_U32(retVal);	
		}		
		/* Load Rx gain table */
		retVal = (talRecoveryActions_t)FR9009_programRxGainTable(device, &rxGainTable[0], (sizeof(rxGainTable) / sizeof(fr9009RxGainTable_t)), 
									init->rx.rxGainCtrl.rx2MaxGainIndex, init->rx.rxGainCtrl.rx2MinGainIndex, FR_RX2);
		IF_ERR_RETURN_U32(retVal);
		// MGC
		if(init->rx.rxGainCtrl.gainMode == FR_MGC || init->rx.rxGainCtrl.gainMode == FR_MGC_PIN){
			retVal = (talRecoveryActions_t)FR9009_setRxManualGain(device, FR_RX2, init->rx.rxGainCtrl.rx2GainIndex);
			IF_ERR_RETURN_U32(retVal);
		}
		/*ADC mode setting*/
		// TODO
	}
	
	if(init->rx.rxGainCtrl.FeAttCaliEn != 0){
		retVal = FR9009_armSpiCmd_AGC_CALI(device, init->rx.rxChannels);
		IF_ERR_RETURN_U32(retVal);
	}
	return retVal;
}


uint32_t FR9009_orxdp_initialize(fr9009Device_t *device, fr9009Init_t *init)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;

	uint32_t 	profileIndex = device->devStateInfo.orxProfileMtxIdx;
	uint32_t 	regBypassVal = 0;
	uint32_t 	regVldSelValH = 0;
	uint32_t 	regVldSelValL = 0;
	uint32_t 	regVldSelValIfNco = 0;
	uint32_t	reg = 0;
	
	/* Verify pointers to *device and *init are not NULL */
	if (device == NULL) {
		/* Can not write to log since log function requires device data structure */
		return (uint32_t)TALACT_ERR_CHECK_PARAM;
	}

	if (init == NULL) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_INIT_NULLPARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Rx profilesValid Check */
	if ((device->devStateInfo.profilesValid & ORX_PROFILE_VALID) == 0) {
		return (uint32_t)TALACT_ERR_CHECK_PARAM;
	}

	regBypassVal = rxProfileMatrix[profileIndex][1];
	regVldSelValH = rxProfileMatrix[profileIndex][2];
	regVldSelValL = rxProfileMatrix[profileIndex][3];
	regVldSelValIfNco = 0; 		// ifconv_nco_sel

	regBypassVal |= (1 << 10) | (1 << 11); 																									// bypass QEC_QFIR and PHSCMPS
	regBypassVal |= (init->obsRx.orxProfile.rxIfConvMode[0] == FR_IFCONV_BYPASS) << 14; 	// bypass IF_CONV only when ORX and RX both don't use IF_CONV
	regBypassVal |= ((init->obsRx.orxProfile.rxFir.coefs == NULL) && (device->devStateInfo.orxFirDecimation == 1)) << 7;					// bypass RCFIR
	// regBypassVal |= (init->obsRx.orxProfile.rxMixer.freq_Hz == 0) << 0;																		// bypass RMIX
	regBypassVal |= 1 << 0;																		// bypass RMIX

	// Common Module
	// retVal = talInitializeRxMixer(device, init->obsRx.orxProfile.rxMixer);
	// IF_ERR_RETURN_U32(retVal);	

	if (init->obsRx.orxProfile.rxFir.numFirCoefs > 0) {
		retVal = (talRecoveryActions_t)FR9009_programFir(device, FR_OBSRX1RX2_FIR, &init->obsRx.orxProfile.rxFir);
		IF_ERR_RETURN_U32(retVal);
	}
	
	// din/dout vld sel
	retVal = FR9009_armSpiCmd_writeWords(device, APB_RX_PATH_TOP_BASEADDR+0x10, &regVldSelValL, 1); 
	IF_ERR_RETURN_U32(retVal);	
	reg = regVldSelValH + (init->obsRx.orxProfile.rxIfConvMode[0] == FR_IFCONV_DEC2 ? 0x200 : 0);
	retVal = FR9009_armSpiCmd_writeWords(device, APB_RX_PATH_TOP_BASEADDR+0x14, &reg, 1);
	IF_ERR_RETURN_U32(retVal);	
	retVal = FR9009_armSpiCmd_writeWords(device, APB_RX_PATH_TOP_BASEADDR+0x18, &regVldSelValIfNco, 1);
	IF_ERR_RETURN_U32(retVal);

	// set bypass reg
	retVal = FR9009_armSpiCmd_writeWords(device, APB_RX_PATH_TOP_BASEADDR+0x20, &regBypassVal, 1);
	IF_ERR_RETURN_U32(retVal);	
		
	// Channel 1 settings
	if (init->obsRx.obsRxChannelsEnable & FR_ORX1){		
		// COMPLEX FIR
		if ((init->obsRx.orxProfile.rxFlatnessFir[0].numFirCoefs > 0) && init->obsRx.orxProfile.rxFlatnessFir[0].coefs_II != NULL ) {
			retVal = FR9009_programCplxFir(device, FR_OBSRX1_CPLX_FIR, &init->obsRx.orxProfile.rxFlatnessFir[0]);
			IF_ERR_RETURN_U32(retVal);	
		}
		retVal = (talRecoveryActions_t)FR9009_setObsRxManualGain(device, FR_ORX1, init->obsRx.orxGainCtrl.ORx1Atten_mdB);
		IF_ERR_RETURN_U32(retVal);
		/*ADC mode setting*/
		// TODO
	}

	// Channel 2 settings
	if (init->obsRx.obsRxChannelsEnable & FR_ORX2){
		if ((init->obsRx.orxProfile.rxFlatnessFir[1].numFirCoefs > 0) && init->obsRx.orxProfile.rxFlatnessFir[1].coefs_II != NULL ) {
			retVal = FR9009_programCplxFir(device, FR_OBSRX1_CPLX_FIR, &init->obsRx.orxProfile.rxFlatnessFir[1]);
			IF_ERR_RETURN_U32(retVal);	
		}
		retVal = (talRecoveryActions_t)FR9009_setObsRxManualGain(device, FR_ORX2, init->obsRx.orxGainCtrl.ORx2Atten_mdB);
		IF_ERR_RETURN_U32(retVal);
		/*ADC mode setting*/
		// TODO
	}
	return retVal;
}

uint32_t __FR9009_rx_orx_jesdmap_set(fr9009Device_t *device, fr9009FramerMapSel_t MapSel, fr9009Jesd204bFramerConfig_t *pframer)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	fr9009FrameXbar_t FrameXbar;
	CHANNEL_t RxOrxch = MapSel == FR_FRAMER_MAP_RX ? (device->devStateInfo.initializedChannels & 0x3) : ((device->devStateInfo.initializedChannels >> 2) & 0x3);
	int i = 0, j= 0;
	uint8_t map_offset = RxOrxch == CHANNEL_2 ? (FR_ADC_RX2_I_IND0 - FR_ADC_RX1_I_IND0) : 0;
	int8_t ch_map_offset = RxOrxch == CHANNEL_2 ? (FR_ADC_RX1_I_IND0 - FR_ADC_RX2_I_IND0) : (FR_ADC_RX2_I_IND0 - FR_ADC_RX1_I_IND0);
	uint8_t L = 0;
	uint8_t LineEnIdex[4]={0};
	for (i = 0; i < 4; i++) {
		if((pframer->serializerLanesEnabled >> i) & 0x01){
			LineEnIdex[L] = i;
			L += 1;
		}
	}
	for (i = 0, j = 0; i < 4; i++) {
		if(((pframer->serializerLanesEnabled >> i) & 0x01) == 0){
			LineEnIdex[L + j] = i;
			j++;
		}
	}
	if(pframer->M / L == 0x2){// M=4, L=2; M=2,L=1  // lane data  [32:0][Q0[7:0],Q0[15:8],I0[7:0],I0[15:8]] [64:0][Q1[7:0],Q1[15:8],I1[7:0],I1[15:8], Q0[7:0],Q0[15:8],I0[7:0],I0[15:8]]  
		for(i = 0; i < 4; i++){//优先配置使用的lane i % 2 == 1 代表使用第二个通道
			FrameXbar.lane[LineEnIdex[i]][0] = FR_ADC_RX1_I_IND0 + map_offset + ch_map_offset * (i % 2 == 1);
			FrameXbar.lane[LineEnIdex[i]][1] = FR_ADC_RX1_Q_IND0 + map_offset + ch_map_offset * (i % 2 == 1);
			FrameXbar.lane[LineEnIdex[i]][2] = FR_ADC_RX1_I_IND0 + map_offset + ch_map_offset * (i % 2 == 1) + FR_ADC_RX_SAMEDATA_OFFSET;
			FrameXbar.lane[LineEnIdex[i]][3] = FR_ADC_RX1_Q_IND0 + map_offset + ch_map_offset * (i % 2 == 1) + FR_ADC_RX_SAMEDATA_OFFSET;
		}
		// FrameXbar.lane[0][0] = FR_ADC_RX1_I_IND0;
		// FrameXbar.lane[0][1] = FR_ADC_RX1_Q_IND0;
		// FrameXbar.lane[0][2] = FR_ADC_RX1_I_IND0 + FR_ADC_RX_SAMEDATA_OFFSET;
		// FrameXbar.lane[0][3] = FR_ADC_RX1_Q_IND0 + FR_ADC_RX_SAMEDATA_OFFSET;
		// FrameXbar.lane[1][0] = FR_ADC_RX2_I_IND0;
		// FrameXbar.lane[1][1] = FR_ADC_RX2_Q_IND0;
		// FrameXbar.lane[1][2] = FR_ADC_RX2_I_IND0 + FR_ADC_RX_SAMEDATA_OFFSET;
		// FrameXbar.lane[1][3] = FR_ADC_RX2_Q_IND0 + FR_ADC_RX_SAMEDATA_OFFSET;
		// FrameXbar.lane[2][0] = FR_ADC_RX1_I_IND0;
		// FrameXbar.lane[2][1] = FR_ADC_RX1_Q_IND0;
		// FrameXbar.lane[2][2] = FR_ADC_RX1_I_IND0 + FR_ADC_RX_SAMEDATA_OFFSET;
		// FrameXbar.lane[2][3] = FR_ADC_RX1_Q_IND0 + FR_ADC_RX_SAMEDATA_OFFSET;
		// FrameXbar.lane[3][0] = FR_ADC_RX2_I_IND0;
		// FrameXbar.lane[3][1] = FR_ADC_RX2_Q_IND0;
		// FrameXbar.lane[3][2] = FR_ADC_RX2_I_IND0 + FR_ADC_RX_SAMEDATA_OFFSET;
		// FrameXbar.lane[3][3] = FR_ADC_RX2_Q_IND0 + FR_ADC_RX_SAMEDATA_OFFSET;
	}else if(pframer->M / L == 0x1){ // M=4,L=4; M=2,l=2; M=1,L=1 //lane data [32:0][I1[7:0],I1[15:8],I0[7:0],I0[15:8]]
		for(i = 0; i < 4; i+=2){//优先配置使用的lane i>=2 代表使用第二个通道
			FrameXbar.lane[LineEnIdex[i  ]][0] = FR_ADC_RX1_I_IND0  + map_offset + ch_map_offset * (i >= 2) + FR_ADC_RX_SAMEDATA_OFFSET;
			FrameXbar.lane[LineEnIdex[i  ]][1] = FR_ADC_RX1_I_IND0  + map_offset + ch_map_offset * (i >= 2) ;
			FrameXbar.lane[LineEnIdex[i  ]][2] = FR_ADC_RX1_I_IND0  + map_offset + ch_map_offset * (i >= 2) + FR_ADC_RX_SAMEDATA_OFFSET * 3;
			FrameXbar.lane[LineEnIdex[i  ]][3] = FR_ADC_RX1_I_IND0  + map_offset + ch_map_offset * (i >= 2) + FR_ADC_RX_SAMEDATA_OFFSET * 2;

			FrameXbar.lane[LineEnIdex[i + 1]][0] = FR_ADC_RX1_Q_IND0  + map_offset + ch_map_offset * (i >= 2) + FR_ADC_RX_SAMEDATA_OFFSET;
			FrameXbar.lane[LineEnIdex[i + 1]][1] = FR_ADC_RX1_Q_IND0  + map_offset + ch_map_offset * (i >= 2) ;
			FrameXbar.lane[LineEnIdex[i + 1]][2] = FR_ADC_RX1_Q_IND0  + map_offset + ch_map_offset * (i >= 2) + FR_ADC_RX_SAMEDATA_OFFSET * 3;
			FrameXbar.lane[LineEnIdex[i + 1]][3] = FR_ADC_RX1_Q_IND0  + map_offset + ch_map_offset * (i >= 2) + FR_ADC_RX_SAMEDATA_OFFSET * 2;
		}
		// FrameXbar.lane[0][0] = FR_ADC_RX1_I_IND0 + FR_ADC_RX_SAMEDATA_OFFSET;
		// FrameXbar.lane[0][1] = FR_ADC_RX1_I_IND0;
		// FrameXbar.lane[0][2] = FR_ADC_RX1_I_IND0 + FR_ADC_RX_SAMEDATA_OFFSET * 3;
		// FrameXbar.lane[0][3] = FR_ADC_RX1_I_IND0 + FR_ADC_RX_SAMEDATA_OFFSET * 2;
		// FrameXbar.lane[1][0] = FR_ADC_RX1_Q_IND0 + FR_ADC_RX_SAMEDATA_OFFSET;
		// FrameXbar.lane[1][1] = FR_ADC_RX1_Q_IND0;
		// FrameXbar.lane[1][2] = FR_ADC_RX1_Q_IND0 + FR_ADC_RX_SAMEDATA_OFFSET * 3;
		// FrameXbar.lane[1][3] = FR_ADC_RX1_Q_IND0 + FR_ADC_RX_SAMEDATA_OFFSET * 2;
		// FrameXbar.lane[2][0] = FR_ADC_RX2_I_IND0 + FR_ADC_RX_SAMEDATA_OFFSET;
		// FrameXbar.lane[2][1] = FR_ADC_RX2_I_IND0;
		// FrameXbar.lane[2][2] = FR_ADC_RX2_I_IND0 + FR_ADC_RX_SAMEDATA_OFFSET * 3;
		// FrameXbar.lane[2][3] = FR_ADC_RX2_I_IND0 + FR_ADC_RX_SAMEDATA_OFFSET * 2;
		// FrameXbar.lane[3][0] = FR_ADC_RX2_Q_IND0 + FR_ADC_RX_SAMEDATA_OFFSET;
		// FrameXbar.lane[3][1] = FR_ADC_RX2_Q_IND0;
		// FrameXbar.lane[3][2] = FR_ADC_RX2_Q_IND0 + FR_ADC_RX_SAMEDATA_OFFSET * 3;
		// FrameXbar.lane[3][3] = FR_ADC_RX2_Q_IND0 + FR_ADC_RX_SAMEDATA_OFFSET * 2;
	}else{//L=1 M=4 
		for(i = 0; i < 4; i++){//每条lane上的数据都一样
			FrameXbar.lane[i][0] = FR_ADC_RX1_I_IND0;
			FrameXbar.lane[i][1] = FR_ADC_RX1_Q_IND0;
			FrameXbar.lane[i][2] = FR_ADC_RX2_I_IND0;
			FrameXbar.lane[i][3] = FR_ADC_RX2_Q_IND0;
		}
		// FrameXbar.lane[0][0] = FR_ADC_RX1_I_IND0;
		// FrameXbar.lane[0][1] = FR_ADC_RX1_Q_IND0;
		// FrameXbar.lane[0][2] = FR_ADC_RX2_I_IND0;
		// FrameXbar.lane[0][3] = FR_ADC_RX2_Q_IND0;
		// FrameXbar.lane[1][0] = FR_ADC_RX1_I_IND0;
		// FrameXbar.lane[1][1] = FR_ADC_RX1_Q_IND0;
		// FrameXbar.lane[1][2] = FR_ADC_RX2_I_IND0;
		// FrameXbar.lane[1][3] = FR_ADC_RX2_Q_IND0;
		// FrameXbar.lane[2][0] = FR_ADC_RX1_I_IND0;
		// FrameXbar.lane[2][1] = FR_ADC_RX1_Q_IND0;
		// FrameXbar.lane[2][2] = FR_ADC_RX2_I_IND0;
		// FrameXbar.lane[2][3] = FR_ADC_RX2_Q_IND0;
		// FrameXbar.lane[3][0] = FR_ADC_RX1_I_IND0;
		// FrameXbar.lane[3][1] = FR_ADC_RX1_Q_IND0;
		// FrameXbar.lane[3][2] = FR_ADC_RX2_I_IND0;
		// FrameXbar.lane[3][3] = FR_ADC_RX2_Q_IND0;
	} 
	retVal = FR9009_setupAdcSampleXbar(device, MapSel, &FrameXbar);
	IF_ERR_RETURN_U32(retVal);
	return retVal;
}

uint32_t __FR9009_jesd_initialize(fr9009Device_t *device, fr9009Init_t *init)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	fr9009Jesd204bFramerConfig_t *pframer = NULL;
	fr9009DeframeXbar_t dacXbar;
	uint8_t LineEnIdex[4]={0};
	int i = 0;
	uint8_t L = 0;
	uint8_t M = 0;
	FR9009_armSpiCmd_writeField(device, FR9009_ADDR_JESD_FRAMER_CONFIG4_0, 0x00000000, 0x0000000F, 0);
	FR9009_armSpiCmd_writeField(device, FR9009_ADDR_JESD_FRAMER_CONFIG4_0 + FR9009_JESD_FRAMERB_OFFSET, 0x00000000, 0x0000000F, 0);
	/* If Valid Rx Profile or valid ObsRx profile, setup serializers */
	if (((device->devStateInfo.profilesValid & RX_PROFILE_VALID) && (init->rx.rxChannels != FR_RXOFF)) 
			|| ((device->devStateInfo.profilesValid & ORX_PROFILE_VALID) && (init->obsRx.obsRxChannelsEnable != FR_ORXOFF))) {
		retVal = (talRecoveryActions_t)FR9009_setupSerializers(device, init);
		IF_ERR_RETURN_U32(retVal);
		
		if ((device->devStateInfo.profilesValid & RX_PROFILE_VALID) && (init->rx.rxChannels != FR_RXOFF)) {
			/* save the Np states for use in slicer gain compensation management */
			if ((init->rx.framerSel == FR_FRAMER_A) || (init->rx.framerSel == FR_FRAMER_A_AND_B)) {
				device->devStateInfo.rxFramerNp = init->jesd204Settings.framerA.Np;
				pframer = &init->jesd204Settings.framerA;
			} else {
				device->devStateInfo.rxFramerNp = init->jesd204Settings.framerB.Np;
				pframer = &init->jesd204Settings.framerB;
			}
			retVal = (talRecoveryActions_t)FR9009_setupJesd204bFramer(device, init, init->rx.framerSel);
			IF_ERR_RETURN_U32(retVal);
			retVal = __FR9009_rx_orx_jesdmap_set(device, FR_FRAMER_MAP_RX, pframer);
			IF_ERR_RETURN_U32(retVal);
		}			

		if ((device->devStateInfo.profilesValid & ORX_PROFILE_VALID) && (init->obsRx.obsRxChannelsEnable != FR_ORXOFF)) {
			/* save the Np states for use in slicer gain compensation management */
			if ((init->obsRx.framerSel == FR_FRAMER_A) ||
				(init->obsRx.framerSel == FR_FRAMER_A_AND_B)) {
				/* Only Setup JESD Framer if serializer Lanes are enabled for Framer A ORx path */
				if (init->jesd204Settings.framerA.serializerLanesEnabled > 0) {
					device->devStateInfo.orxFramerNp = init->jesd204Settings.framerA.Np;
					pframer = &init->jesd204Settings.framerA;
					retVal = (talRecoveryActions_t)FR9009_setupJesd204bFramer(device, init,
							init->obsRx.framerSel);
					IF_ERR_RETURN_U32(retVal);
				}
			} else {
				/* Only Setup JESD Framer if serializer Lanes are enabled for Framer B ORx path */
				if (init->jesd204Settings.framerB.serializerLanesEnabled > 0) {
					device->devStateInfo.orxFramerNp = init->jesd204Settings.framerB.Np;
					pframer = &init->jesd204Settings.framerB;
					retVal = (talRecoveryActions_t)FR9009_setupJesd204bFramer(device, init,
							init->obsRx.framerSel);
					IF_ERR_RETURN_U32(retVal);
				}
			}	
			retVal = __FR9009_rx_orx_jesdmap_set(device, FR_FRAMER_MAP_ORX, pframer);
			IF_ERR_RETURN_U32(retVal);
		}
	}


	if ((device->devStateInfo.profilesValid & TX_PROFILE_VALID) &&
		(init->tx.txChannels != FR_TXOFF)) {
		retVal = (talRecoveryActions_t)FR9009_setupDeserializers(device, init);
		IF_ERR_RETURN_U32(retVal);

		for (i = 0, L = 0; i < 4; i++) {
			if((init->jesd204Settings.deframerA.deserializerLanesEnabled >> i) & 0x01){
				LineEnIdex[L] = i;
				L += 1;
			}
		}
		M = init->jesd204Settings.deframerA.M;
		/* set the DAC sample crossbars */
		if (init->tx.deframerSel == FR_DEFRAMER_A) {
			if(init->tx.txChannels & FR_TX1){
				if (((M == 4) && (L == 4)) || ((M == 2) && (L == 2)) || ((M == 1) && (L == 1))){
					dacXbar.dacChanI[0] = FR_DEFRAMER_L0_OUT1 + FR_DEFRAMER_LANE_OUT_DIFF * (LineEnIdex[0] - 0);
					dacXbar.dacChanI[1] = FR_DEFRAMER_L0_OUT0 + FR_DEFRAMER_LANE_OUT_DIFF * (LineEnIdex[0] - 0);
					dacXbar.dacChanQ[0] = FR_DEFRAMER_L1_OUT1 + FR_DEFRAMER_LANE_OUT_DIFF * (LineEnIdex[1] - 1);
					dacXbar.dacChanQ[1] = FR_DEFRAMER_L1_OUT0 + FR_DEFRAMER_LANE_OUT_DIFF * (LineEnIdex[1] - 1);
				}else if (((M == 4) && (L == 2)) || ((M == 2) && (L == 1))){
					dacXbar.dacChanI[0] = FR_DEFRAMER_L0_OUT0 + FR_DEFRAMER_LANE_OUT_DIFF * (LineEnIdex[0] - 0);
					dacXbar.dacChanI[1] = FR_DEFRAMER_L0_OUT0 + FR_DEFRAMER_LANE_OUT_DIFF * (LineEnIdex[0] - 0) + FR_DEFRAMER_OUT_ONE;
					dacXbar.dacChanQ[0] = FR_DEFRAMER_L0_OUT1 + FR_DEFRAMER_LANE_OUT_DIFF * (LineEnIdex[0] - 0);
					dacXbar.dacChanQ[1] = FR_DEFRAMER_L0_OUT1 + FR_DEFRAMER_LANE_OUT_DIFF * (LineEnIdex[0] - 0) + FR_DEFRAMER_OUT_ONE;
				}else{// M=4, L =1
					dacXbar.dacChanI[0] = FR_DEFRAMER_L0_OUT0 + FR_DEFRAMER_LANE_OUT_DIFF * (LineEnIdex[0] - 0) + FR_DEFRAMER_OUT_ONE * 2;
					dacXbar.dacChanI[1] = FR_DEFRAMER_L0_OUT0 + FR_DEFRAMER_LANE_OUT_DIFF * (LineEnIdex[0] - 0) + FR_DEFRAMER_OUT_ONE;
					dacXbar.dacChanQ[0] = FR_DEFRAMER_L0_OUT1 + FR_DEFRAMER_LANE_OUT_DIFF * (LineEnIdex[0] - 0) + FR_DEFRAMER_OUT_ONE * 2;
					dacXbar.dacChanQ[1] = FR_DEFRAMER_L0_OUT1 + FR_DEFRAMER_LANE_OUT_DIFF * (LineEnIdex[0] - 0) + FR_DEFRAMER_OUT_ONE;
				}
				retVal = FR9009_setupDacSampleXbar(device,  FR_TX1, &dacXbar);
				IF_ERR_RETURN_U32(retVal);
			}
			if(init->tx.txChannels & FR_TX2){
				if (((M == 4) && (L == 4)) || ((M == 2) && (L == 2)) || ((M == 1) && (L == 1))){
					dacXbar.dacChanI[0] = FR_DEFRAMER_L2_OUT1 + FR_DEFRAMER_LANE_OUT_DIFF * (LineEnIdex[2 % L] - 2);
					dacXbar.dacChanI[1] = FR_DEFRAMER_L2_OUT0 + FR_DEFRAMER_LANE_OUT_DIFF * (LineEnIdex[2 % L] - 2);
					dacXbar.dacChanQ[0] = FR_DEFRAMER_L3_OUT1 + FR_DEFRAMER_LANE_OUT_DIFF * (LineEnIdex[3 % L] - 3);
					dacXbar.dacChanQ[1] = FR_DEFRAMER_L3_OUT0 + FR_DEFRAMER_LANE_OUT_DIFF * (LineEnIdex[3 % L] - 3);
				}else if (((M == 4) && (L == 2)) || ((M == 2) && (L == 1))){
					dacXbar.dacChanI[0] = FR_DEFRAMER_L1_OUT0 + FR_DEFRAMER_LANE_OUT_DIFF * (LineEnIdex[1 % L] - 1);
					dacXbar.dacChanI[1] = FR_DEFRAMER_L1_OUT0 + FR_DEFRAMER_LANE_OUT_DIFF * (LineEnIdex[1 % L] - 1) + FR_DEFRAMER_OUT_ONE;
					dacXbar.dacChanQ[0] = FR_DEFRAMER_L1_OUT1 + FR_DEFRAMER_LANE_OUT_DIFF * (LineEnIdex[1 % L] - 1);
					dacXbar.dacChanQ[1] = FR_DEFRAMER_L1_OUT1 + FR_DEFRAMER_LANE_OUT_DIFF * (LineEnIdex[1 % L] - 1) + FR_DEFRAMER_OUT_ONE;
				}else{// M=4, L =1
					dacXbar.dacChanI[0] = FR_DEFRAMER_L0_OUT0 + FR_DEFRAMER_LANE_OUT_DIFF * (LineEnIdex[0] - 0) + FR_DEFRAMER_OUT_ONE * 3;
					dacXbar.dacChanI[1] = FR_DEFRAMER_L0_OUT0 + FR_DEFRAMER_LANE_OUT_DIFF * (LineEnIdex[0] - 0) + FR_DEFRAMER_OUT_ONE * 2;
					dacXbar.dacChanQ[0] = FR_DEFRAMER_L0_OUT1 + FR_DEFRAMER_LANE_OUT_DIFF * (LineEnIdex[0] - 0) + FR_DEFRAMER_OUT_ONE * 3;
					dacXbar.dacChanQ[1] = FR_DEFRAMER_L0_OUT1 + FR_DEFRAMER_LANE_OUT_DIFF * (LineEnIdex[0] - 0) + FR_DEFRAMER_OUT_ONE * 2;
				}
				retVal = FR9009_setupDacSampleXbar(device,  FR_TX2, &dacXbar);
				IF_ERR_RETURN_U32(retVal);
			}
		}

		if (init->tx.deframerSel == FR_DEFRAMER_A){
			retVal = (talRecoveryActions_t)FR9009_setupJesd204bDeframer(device, init, FR_DEFRAMER_A);
			IF_ERR_RETURN_U32(retVal);
		}
		if(init->jesd204Settings.deframerA.externalSysref == 0){
			retVal = FR9009_internalSysrefClock(device, init->clocks.sysrefDiv);
			IF_ERR_RETURN_U32(retVal);
		}
	}
	return retVal;
}

uint32_t FR9009_initialize(fr9009Device_t *device, fr9009Init_t *init)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
    talRecoveryActions_t retValWarn = TALACT_NO_ACTION;
	fRHALErr_t          halError = FRHAL_OK;
	uint8_t adda_status = 0;
	uint8_t pllsel=0;
	uint8_t dpDec5DecimationEn = 0;
    fr9009Info_t clearInfo ={
       // .devState = FR_STATE_POWERONRESET,
        .initializedChannels = 0,
        .profilesValid = 0,
        .errSource = 0,
        .errCode = 0,
        .clocks = 
        {
            .deviceClock_kHz = 0,
            .clkPllVcoFreq_kHz = 0,
            .clkPllHsDiv = 0,
            .hsDigClkDiv2_Hz = 0,
            .hsDigClkDiv4or5_Hz = 0,
            .rfPllUseExternalLo = 0
        },
        // .gainMode = FR_MGC,
        // .gainIndexes = 
        // {
        //     .rx1MinGainIndex = 0,
        //     .rx1MaxGainIndex = 0,
        //     .rx2MinGainIndex = 0,
        //     .rx2MaxGainIndex = 0
        // },
        .txAttenStepSize = FR_TXATTEN_0P05_DB,
        .usedGpiopins = 0,
        .usedGpio3p3pins = 0,
        .rxFramerNp = 0,    
        .orxFramerNp = 0,        
        .rxOutputRate_kHz = 0,    
        .txInputRate_kHz = 0,
        .rxDualBandEnabled = 0,
        .rxTotalM = 0,
        .swTest = 0,
        .deviceSiRev = 0,
        .talErrFunctionTable = 
        {
            .talErrorFunctionTable ={{0},{0}}
        },
        .talFhmFreqRange = 
        {
            .fhmMinFreq_MHz = 0,
         .fhmMaxFreq_MHz = 0
        },
    };

    // uint32_t agcClock_Hz = 0;
    // uint32_t gainUpdateCount = 0;
    uint32_t regData = 0;
	uint8_t subadc_num = 4;

	/* Verify pointers to *device and *init are not NULL */
	if (device == NULL) {
		/* Can not write to log since log function requires device data structure */
		return (uint32_t)TALACT_ERR_CHECK_PARAM;
	}

	if (init == NULL) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_INIT_NULLPARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

#if FR9009_VERBOSE
	halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK, "FR9009_initialize()\n");
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal, TALACT_WARN_RESET_LOG);
#endif

    retValWarn = retVal;

    /* clear memory of FR9009 API state container */
	clearInfo.devState = device->devStateInfo.devState;
    device->devStateInfo = clearInfo;
	device->devStateInfo.rxGainCtrl = init->rx.rxGainCtrl;
    device->devStateInfo.txAttenStepSize = init->tx.txAttenStepSize;

	device->devStateInfo.txInputRate_kHz = init->tx.txProfile.txInputRate_kHz;
	device->devStateInfo.rxOutputRate_kHz = init->rx.rxProfile.rxOutputRate_kHz;
	device->devStateInfo.orxOutputRate_kHz = init->obsRx.orxProfile.rxOutputRate_kHz;
	device->devStateInfo.clocks.deviceClock_kHz = init->clocks.deviceClock_kHz;

	device->devStateInfo.predict_cali = init->fhm.predict_cali;
	device->devStateInfo.fhmtype = init->fhm.fhmType;
	device->devStateInfo.fhm_enable = init->fhm.fhmEnable;
	device->devStateInfo.rxframerSel = init->rx.framerSel;
	device->devStateInfo.orxframerSel = init->obsRx.framerSel;
	device->devStateInfo.txdeframerSel = init->tx.deframerSel;

	device->devStateInfo.initializedChannels = (init->rx.rxChannels & 3);//jesd 使用
	device->devStateInfo.initializedChannels |= init->obsRx.obsRxChannelsEnable & FR_ORX1ORX2;//jesd 使用
	device->devStateInfo.initializedChannels |= ((init->tx.txChannels & 3) << 4);//jesd 使用
	device->devStateInfo.rxIfConvMode[0] = init->rx.rxProfile.rxIfConvMode[0]; 
	device->devStateInfo.rxIfConvMode[1] = init->rx.rxProfile.rxIfConvMode[1]; 
	device->devStateInfo.orxIfConvMode[0] = init->obsRx.orxProfile.rxIfConvMode[0];
	device->devStateInfo.orxIfConvMode[1] = init->obsRx.orxProfile.rxIfConvMode[1];
	device->devStateInfo.txIfConvMode[0] = init->tx.txProfile.txIfConvMode[0];
	device->devStateInfo.txIfConvMode[1] = init->tx.txProfile.txIfConvMode[1];

	dpDec5DecimationEn = init->clocks.clkPllHsDiv == 5 ? 0:1;
	retVal = FR9009_armSpiCmd_decmode_set(device, dpDec5DecimationEn);
    IF_ERR_RETURN_U32(retVal);

	/* SWITCH arm clk*/
	// if(device->devStateInfo.clocks.deviceClock_kHz < 100000)
	{
		FR9009_armSpiCmd_arm_clkSel(device, 1);// select BB clk for mcu
	}
	/* calibration ADDA*/
	retVal = FR9009_armSpiCmd_Init_ADDAcali(device, init->tx.txChannels, init->tx.txChannels | init->rx.rxChannels | (init->obsRx.obsRxChannelsEnable >> 2), &adda_status);
	IF_ERR_RETURN_U32(retVal);
	if(adda_status){
		return FR_ERR_INIT_ADDA_CALIBRATION;
	}
	
	if(init->rx.rxProfile.rxAdcMode == FR_ADC_IQ4ITLV){
		subadc_num = 4;
	}else if(init->rx.rxProfile.rxAdcMode == FR_ADC_IQ2ITLV){
		subadc_num = 2;
	}else if(init->rx.rxProfile.rxAdcMode == FR_ADC_IQ){
		subadc_num = 1;
	}else{
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_INIT_NULLPARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}
	retVal = FR9009_armSpiCmd_Init_ADNum(device, init->tx.txChannels | init->rx.rxChannels | (init->obsRx.obsRxChannelsEnable >> 2), subadc_num, NULL);
	IF_ERR_RETURN_U32(retVal);
	/*初始化寄存器默认配置 */
	retVal = FR9009_armSpiCmd_Init_Reg(device, 0);
	IF_ERR_RETURN_U32(retVal);
	/* Calculate digital clocks based on information in init structure */
	retVal = (talRecoveryActions_t)FR9009_calculateDigitalClocks(device, &init->clocks);
	IF_ERR_RETURN_U32(retVal);
	
	/* Verify Rx/Tx and ObsRx profiles are valid combinations
	 * and set device->devStateInfo.profilesValid member based on init struct settings
	 */
	retVal = (talRecoveryActions_t)FR9009_verifyProfiles(device, init);
	IF_ERR_RETURN_U32(retVal);

	/* Read device silicon revision */
	retVal = (talRecoveryActions_t)FR9009_getDeviceRev(device, &regData);
	device->devStateInfo.deviceSiRev = regData;
	IF_ERR_RETURN_U32(retVal);

	
	/* Set lo*/
	if(init->rx.rxLoSource[0] == init->rx.rxLoSource[1]){
		retVal = FR9009_armSpiCmd_rx_Lo_set(device, CHANNEL_1_2, init->rx.rxLoSource[0]);
		IF_ERR_RETURN_U32(retVal);
	}else{
		retVal = FR9009_armSpiCmd_rx_Lo_set(device, CHANNEL_1, init->rx.rxLoSource[0]);
		IF_ERR_RETURN_U32(retVal);
		retVal = FR9009_armSpiCmd_rx_Lo_set(device, CHANNEL_2, init->rx.rxLoSource[1]);
		IF_ERR_RETURN_U32(retVal);		
	}
	if(init->obsRx.obsRxLoSource[0] == init->obsRx.obsRxLoSource[1]){
		retVal = FR9009_armSpiCmd_orx_Lo_set(device, CHANNEL_1_2, init->obsRx.obsRxLoSource[0]);
		IF_ERR_RETURN_U32(retVal);
	}else{
		retVal = FR9009_armSpiCmd_orx_Lo_set(device, CHANNEL_1, init->obsRx.obsRxLoSource[0]);
		IF_ERR_RETURN_U32(retVal);
		retVal = FR9009_armSpiCmd_orx_Lo_set(device, CHANNEL_2, init->obsRx.obsRxLoSource[1]);
		IF_ERR_RETURN_U32(retVal);		
	}
	if(init->tx.txLoSource[0] == init->tx.txLoSource[1]){
		retVal = FR9009_armSpiCmd_tx_Lo_set(device, CHANNEL_1_2, init->tx.txLoSource[0]);
		IF_ERR_RETURN_U32(retVal);
	}else{
		retVal = FR9009_armSpiCmd_tx_Lo_set(device, CHANNEL_1, init->tx.txLoSource[0]);
		IF_ERR_RETURN_U32(retVal);
		retVal = FR9009_armSpiCmd_tx_Lo_set(device, CHANNEL_2, init->tx.txLoSource[1]);
		IF_ERR_RETURN_U32(retVal);		
	}
	/* Set ADDA clk and Datapath clk*/
#if 1
	/* Tx DataPath settings */
	if(device->devStateInfo.profilesValid & TX_PROFILE_VALID){
		retVal = FR9009_txdp_initialize(device, init);
		IF_ERR_RETURN_U32(retVal);
	}
	/* RX DataPath settings */
	if(device->devStateInfo.profilesValid & RX_PROFILE_VALID){
		retVal = FR9009_rxdp_initialize(device, init);
		IF_ERR_RETURN_U32(retVal);
	}
	/* ORX DataPath settings */
	if(device->devStateInfo.profilesValid & ORX_PROFILE_VALID){
		retVal = FR9009_orxdp_initialize(device, init);
		IF_ERR_RETURN_U32(retVal);
	}
#else
	retVal = FR9009_ORXBandSet(device);   
	IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_RXBandSet(device);   
	IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_TxBandSet(device, CHANNEL_1_2);   
	IF_ERR_RETURN_U32(retVal);
#endif		
	//Fast hop mode set
	retVal = FR9009_setFhmConfig(device, &init->fhm);
	IF_ERR_RETURN_U32(retVal);		

	retVal = __FR9009_jesd_initialize(device, init);
	IF_ERR_RETURN_U32(retVal);		

	pllsel = 0;
	if(init->clocks.bbpllStatusGpio.en){
		pllsel = 0x1;
	}
	if(init->clocks.rfpllStatusGpio.en){
		pllsel |= 0x2;
	}
	if(init->clocks.orfpllStatusGpio.en){
		pllsel |= 0x4;
	}
	if(pllsel){
		retVal = FR9009_armSpiCmd_pll_status_gpio_set(device, pllsel, init->clocks.bbpllStatusGpio.gpio_num, 
					init->clocks.rfpllStatusGpio.gpio_num, init->clocks.orfpllStatusGpio.gpio_num);
		IF_ERR_RETURN_U32(retVal);		
	}
	/* Set gain update counter for 1ms. */
	/* Calculate AGC clock rate - Use same equation as used for txAttenClock_Hz in FR9009_initDigitalClocks*/
	// if (device->devStateInfo.clocks.hsDigClkDiv4or5_Hz > 500000000) {
	// 	agcClock_Hz = (device->devStateInfo.clocks.hsDigClkDiv4or5_Hz / 4); /* div 4 */
	// } else if (device->devStateInfo.clocks.hsDigClkDiv4or5_Hz > 250000000) {
	// 	agcClock_Hz = (device->devStateInfo.clocks.hsDigClkDiv4or5_Hz / 2); /* div 2 */
	// } else {
	// 	agcClock_Hz = device->devStateInfo.clocks.hsDigClkDiv4or5_Hz; /* div 1 */
	// }

	// gainUpdateCount = (agcClock_Hz / 1000);

	/* Set FR9009 API state container information */
	device->devStateInfo.devState = (fr9009States_t)(device->devStateInfo.devState | FR_STATE_INITIALIZED);

	/* Find the Rx Framer and save the Rx Framer M value */
	if (init->rx.framerSel == FR_FRAMER_A) {
		device->devStateInfo.rxTotalM = init->jesd204Settings.framerA.M;
	} else if (init->rx.framerSel == FR_FRAMER_B) {
		device->devStateInfo.rxTotalM = init->jesd204Settings.framerB.M;
	} else if (init->rx.framerSel == FR_FRAMER_A_AND_B) {
		device->devStateInfo.rxTotalM = init->jesd204Settings.framerA.M + init->jesd204Settings.framerB.M;
	}


	// TODO: Disable the orx channel data switch by orx_tx_sel0
	retVal = FR9009_armSpiCmd_writeField(device, FR9009_ADDR_APB_DIG_MUX_LOOPBACK_FIXDATA, 0, (0x1 << 16), 0);
	IF_ERR_RETURN_U32(retVal);

	/* If higher priority retVal has no error, allow possible lower priority warning to be returned */
	if (retVal == TALACT_NO_ACTION) {
		retVal = (talRecoveryActions_t)retValWarn;
	}
	return (uint32_t)retVal;
}


uint32_t FR9009_shutdown(fr9009Device_t *device)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	retVal = (talRecoveryActions_t)FR9009_resetDevice(device);
	IF_ERR_RETURN_U32(retVal);

	return (uint32_t)retVal;
}

uint32_t FR9009_getMultiChipSyncStatus(fr9009Device_t *device, uint8_t *mcsStatus)
{
	talRecoveryActions_t retVal   = TALACT_NO_ACTION;
	fRHALErr_t          halError = FRHAL_OK;
	uint32_t regdata;

#if FR9009_VERBOSE
	halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK,  "FR9009_getMultiChipSyncStatus()\n");

	retVal   = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal,  TALACT_WARN_RESET_LOG);
#endif
	/* null pointer check */
	if (mcsStatus == NULL) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,  FR_ERR_CHECKGETMCS_STATUS_NULL_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	} else {
		retVal = FR9009_armSpiCmd_sync_state_get(device,  &regdata);
		IF_ERR_RETURN_U32(retVal);
		*mcsStatus = regdata;//sync one shot done
	}
	return (uint32_t)retVal;
}

uint32_t FR9009_enableMultichipSync(fr9009Device_t *device, uint8_t enableMcs, uint8_t mcs_type)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	fRHALErr_t halError = FRHAL_OK;

#if FR9009_VERBOSE
	halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK,  "FR9009_enableMultichipSync()\n");
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal,  TALACT_WARN_RESET_LOG);
#endif

		if(mcs_type) {			
			retVal =FR9009_armSpiCmd_sync_en(device, enableMcs, mcs_type);
			IF_ERR_RETURN_U32(retVal);
		}
		// }else if(mcs_type == MCS_TYPE_ORFPLL){			
		// 	retVal =FR9009_armSpiCmd_sync_tx_en(device, 1);
		// 	IF_ERR_RETURN_U32(retVal);
		// }else if(mcs_type == MCS_TYPE_NCO){			
		// 	retVal =FR9009_armSpiCmd_LIF_mode_set(device, 0x1, 0x3, 1);
		// 	retVal =FR9009_armSpiCmd_sync_nco_en(device, 1, 0);
		// 	IF_ERR_RETURN_U32(retVal);
		// }else{
		// 	return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,  FR_ERR_CHECKGETMCS_STATUS_NULL_PARM, retVal, TALACT_ERR_CHECK_PARAM);
		// }
	

	return (uint32_t)retVal;
}

uint32_t FR9009_enableMultichipRfLOPhaseSync(fr9009Device_t *device, uint8_t enableDigTestClk)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	fRHALErr_t halError = FRHAL_OK;

#if FR9009_VERBOSE
	halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK, "FR9009_enableMultichipRfLOPhaseSync()\n");
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal, TALACT_WARN_RESET_LOG);
#endif
//TODO

	return (uint32_t)retVal;
}

uint32_t FR9009_serializerReset(fr9009Device_t *device)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	fRHALErr_t halError = FRHAL_OK;

#if FR9009_VERBOSE
	halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK,
				 "FR9009_SerializerReset()\n");
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif
	retVal = FR9009_armSpiCmd_writeField(device, FR9009_ADDR_APB_CLK_RST_FUNC_RESET, 0x3, 0x0000C000, 14);
	IF_ERR_RETURN_U32(retVal);

	/* Allow framer data to output to serializer (clear reset) */
	retVal = FR9009_armSpiCmd_writeField(device, FR9009_ADDR_APB_CLK_RST_FUNC_RESET, 0x0, 0x0000C000, 14);
	IF_ERR_RETURN_U32(retVal);

	return (uint32_t)retVal;
}

uint32_t FR9009_programFir(fr9009Device_t *device, fr9009firName_t filterToProgram, fr9009Fir_t *firFilter)
{
	// every coef = 16bit
	// max num of coeffs = 64
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	fRHALErr_t halError = FRHAL_OK;
	uint16_t regOffset = 0;
	uint16_t bufOffset = 0;
	uint8_t wrLength = 0; // blk_wr_length, in 32 bit
	uint8_t post_gain = 0;
	uint16_t i= 0;

	uint16_t dataArray[FR9009_FIR_MAX_LEN] = {0};
	uint32_t cfg = 0, cfgMask = 0;

#if FR9009_VERBOSE
	halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK, "FR9009_programFir()\n");
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal, TALACT_WARN_RESET_LOG);
#endif

	/* Error checking, #taps less than max number of taps allowed.*/
	if ((firFilter->numFirCoefs == 0) || (FR9009_FIR_MAX_LEN < firFilter->numFirCoefs)){
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_PROGRAMFIR_COEFS_NULL, retVal, TALACT_ERR_CHECK_PARAM);
	}

	uint32_t coefBitCnt = (firFilter->numFirCoefs + 7) / 8; // ceil(numFirCoefs/8)
    uint32_t vldBit = (~((1 << (8-coefBitCnt))-1)) & 0xff;

	// 计算顺序写入的长度和开始地址
	wrLength = coefBitCnt * 4;
	bufOffset = FR9009_FIR_MAX_LEN - wrLength * 2;
	regOffset = bufOffset * 2;
	
	for (i= 0; i < firFilter->numFirCoefs; i++){
		dataArray[FR9009_FIR_MAX_LEN - 1 - i] = firFilter->coefs[firFilter->numFirCoefs - 1 - i]; // 对齐最高位
	}

	// select gain 
	switch (firFilter->gain_dB) {
	case FR_FIR_GAIN_0_DB:
		post_gain = 0x0;
		break;
	case FR_FIR_GAIN_POS6_DB:
		post_gain = 0x1;
		break;
	case FR_FIR_GAIN_NEG6_DB:
		post_gain = 0x3;
		break;
	case FR_FIR_GAIN_NEG12_DB:
		post_gain = 0x2;
		break;	
	default:
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_PROGRAMFIR_INV_FIRNAME_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}
	// set register
	switch (filterToProgram) {
		case FR_TX1_FIR:
			// write coefficient
			retVal = FR9009_armSpiCmd_writeWords(device, FR9009_ADDR_TX_CH1_CFIR_RX_COEF_START + regOffset, (uint32_t*)&dataArray[bufOffset], wrLength);
			IF_ERR_RETURN_U32(retVal);
			// write cfg field
			cfg = (vldBit << FR9009_ADDR_RX_COEF_VLD_OFFSET) | 
				  (post_gain << FR9009_ADDR_CFIR_POST_GAIN_OFFSET) | 
				  ((firFilter->symmetric != 0) << FR9009_ADDR_RX_COEF_SYMM_OFFSET) |
				  ((firFilter->even != 0) << FR9009_ADDR_RX_COEF_EVEN_OFFSET);
			cfgMask = FR9009_ADDR_RX_COEF_VLD_MASK | FR9009_ADDR_CFIR_POST_GAIN_MASK | FR9009_ADDR_RX_COEF_SYMM_MASK | FR9009_ADDR_RX_COEF_EVEN_MASK;
			retVal = FR9009_armSpiCmd_writeField(device, FR9009_ADDR_TX_CH1_CFIR_RX_COEF_CFG, cfg, cfgMask, 0);
			IF_ERR_RETURN_U32(retVal);
			break;
		case FR_TX2_FIR:
			// write coefficient
			retVal = FR9009_armSpiCmd_writeWords(device, FR9009_ADDR_TX_CH2_CFIR_ORX_COEF_START + regOffset, (uint32_t*)&dataArray[bufOffset], wrLength);
			IF_ERR_RETURN_U32(retVal);
			// write cfg field
			cfg = (vldBit << FR9009_ADDR_ORX_COEF_VLD_OFFSET) | 
				  (post_gain << FR9009_ADDR_CFIR_POST_GAIN_OFFSET) | 
				  ((firFilter->symmetric != 0) << FR9009_ADDR_ORX_COEF_SYMM_OFFSET) |
				  ((firFilter->even != 0) << FR9009_ADDR_ORX_COEF_EVEN_OFFSET);
			cfgMask = FR9009_ADDR_ORX_COEF_VLD_MASK | FR9009_ADDR_CFIR_POST_GAIN_MASK | FR9009_ADDR_ORX_COEF_SYMM_MASK | FR9009_ADDR_ORX_COEF_EVEN_MASK;
			retVal = FR9009_armSpiCmd_writeField(device, FR9009_ADDR_TX_CH2_CFIR_ORX_COEF_CFG, cfg, cfgMask, 0);
			IF_ERR_RETURN_U32(retVal);
			break;
		case FR_TX1TX2_FIR:
			// write coefficient
			retVal = FR9009_armSpiCmd_writeWords(device, FR9009_ADDR_TX_CH1_CFIR_RX_COEF_START + regOffset, (uint32_t*)&dataArray[bufOffset], wrLength);
			IF_ERR_RETURN_U32(retVal);
			retVal = FR9009_armSpiCmd_writeWords(device, FR9009_ADDR_TX_CH2_CFIR_ORX_COEF_START + regOffset, (uint32_t*)&dataArray[bufOffset], wrLength);
			IF_ERR_RETURN_U32(retVal);
			// write cfg field
			cfg = (vldBit << FR9009_ADDR_ORX_COEF_VLD_OFFSET)| 
				  (vldBit << FR9009_ADDR_RX_COEF_VLD_OFFSET) | 
				  (post_gain << FR9009_ADDR_CFIR_POST_GAIN_OFFSET) | 
				  ((firFilter->symmetric != 0) << FR9009_ADDR_ORX_COEF_SYMM_OFFSET) |
				  ((firFilter->symmetric != 0) << FR9009_ADDR_RX_COEF_SYMM_OFFSET) |
				  ((firFilter->even != 0) << FR9009_ADDR_ORX_COEF_EVEN_OFFSET) |
				  ((firFilter->even != 0) << FR9009_ADDR_RX_COEF_EVEN_OFFSET);
			cfgMask = FR9009_ADDR_ORX_COEF_VLD_MASK | FR9009_ADDR_RX_COEF_VLD_MASK | 
					  FR9009_ADDR_CFIR_POST_GAIN_MASK | 
					  FR9009_ADDR_ORX_COEF_SYMM_MASK | FR9009_ADDR_RX_COEF_SYMM_MASK | 
					  FR9009_ADDR_ORX_COEF_EVEN_MASK | FR9009_ADDR_RX_COEF_EVEN_MASK;
			retVal = FR9009_armSpiCmd_writeField(device, FR9009_ADDR_TX_CH1_CFIR_RX_COEF_CFG, cfg, cfgMask, 0);
			IF_ERR_RETURN_U32(retVal);
			break;
		case FR_RX1_FIR:
		case FR_RX2_FIR:
		case FR_RX1RX2_FIR:
			// write coefficient
			retVal = FR9009_armSpiCmd_writeWords(device, FR9009_ADDR_RX_CFIR_RX_COEF_START + regOffset, (uint32_t*)&dataArray[bufOffset], wrLength);
			IF_ERR_RETURN_U32(retVal);
			// write cfg field
			cfg = (vldBit << FR9009_ADDR_RX_COEF_VLD_OFFSET) | 
				  (post_gain << FR9009_ADDR_CFIR_POST_GAIN_OFFSET) | 
				  ((firFilter->symmetric != 0) << FR9009_ADDR_RX_COEF_SYMM_OFFSET) |
				  ((firFilter->even != 0) << FR9009_ADDR_RX_COEF_EVEN_OFFSET);
			cfgMask = FR9009_ADDR_RX_COEF_VLD_MASK | FR9009_ADDR_CFIR_POST_GAIN_MASK | FR9009_ADDR_RX_COEF_SYMM_MASK | FR9009_ADDR_RX_COEF_EVEN_MASK;
			retVal = FR9009_armSpiCmd_writeField(device, FR9009_ADDR_RX_CFIR_RX_COEF_CFG, cfg, cfgMask, 0);
			IF_ERR_RETURN_U32(retVal);
			break;
		case FR_OBSRX1_FIR:
		case FR_OBSRX2_FIR:
		case FR_OBSRX1RX2_FIR:
			// write coefficient
			retVal = FR9009_armSpiCmd_writeWords(device, FR9009_ADDR_RX_CFIR_ORX_COEF_START + regOffset, (uint32_t*)&dataArray[bufOffset], wrLength);
			IF_ERR_RETURN_U32(retVal);
			// write cfg field
			cfg = (vldBit << FR9009_ADDR_ORX_COEF_VLD_OFFSET) | 
				  (post_gain << FR9009_ADDR_CFIR_POST_GAIN_OFFSET) | 
				  ((firFilter->symmetric != 0) << FR9009_ADDR_ORX_COEF_SYMM_OFFSET) |
				  ((firFilter->even != 0) << FR9009_ADDR_ORX_COEF_EVEN_OFFSET);
			cfgMask = FR9009_ADDR_ORX_COEF_VLD_MASK | FR9009_ADDR_CFIR_POST_GAIN_MASK | FR9009_ADDR_ORX_COEF_SYMM_MASK | FR9009_ADDR_ORX_COEF_EVEN_MASK;
			retVal = FR9009_armSpiCmd_writeField(device, FR9009_ADDR_RX_CFIR_ORX_COEF_CFG, cfg, cfgMask, 0);
			IF_ERR_RETURN_U32(retVal);
			break;

		default:
			return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_PROGRAMFIR_INV_FIRNAME_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}
	return (uint32_t)retVal;
}


uint32_t FR9009_programCplxFir(fr9009Device_t *device, fr9009cplxFirName_t filterToProgram, fr9009CplxFir_t *firFilter)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	fRHALErr_t halError = FRHAL_OK;

#if FR9009_VERBOSE
	halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK, "FR9009_programCplxFir()\n");
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal, TALACT_WARN_RESET_LOG);
#endif

	/* Error checking, #taps less than max number of taps allowed.*/
	if ((firFilter->numFirCoefs == 0) || (32 < firFilter->numFirCoefs) || firFilter->numFirCoefs % 4 != 0){
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_PROGRAMFIR_COEFS_NULL, retVal, TALACT_ERR_CHECK_PARAM);
	}

	uint16_t  coeff_vld = ((1 << (firFilter->numFirCoefs / 4)) - 1) & 0xff;
	uint32_t  addr_base[2] = {0, 0};
	uint8_t   orx_sel[2] = {0, 0};
	int ch = 0;

	switch (filterToProgram) {
	case FR_TX1_CPLX_FIR 		:
		addr_base[0] = APB_TX_CPLXCFIR_CH1_BASEADDR;
		orx_sel[0] = 1;
		break;
	case FR_TX2_CPLX_FIR 		:
		addr_base[1] = APB_TX_CPLXCFIR_CH2_BASEADDR;
		orx_sel[1] = 1;
		break;
	case FR_TX1TX2_CPLX_FIR 	:
		addr_base[0] = APB_TX_CPLXCFIR_CH1_BASEADDR;
		addr_base[1] = APB_TX_CPLXCFIR_CH2_BASEADDR;
		orx_sel[0] = 1;
		orx_sel[1] = 1;
		break;
	case FR_RX1_CPLX_FIR 		:
		addr_base[0] = APB_RX_CPLXCFIR_CH1_BASEADDR;
		orx_sel[0] = 0;
		break;
	case FR_RX2_CPLX_FIR 		:
		addr_base[1] = APB_RX_CPLXCFIR_CH2_BASEADDR;
		orx_sel[1] = 0;
		break;
	case FR_RX1RX2_CPLX_FIR 	:
		addr_base[0] = APB_RX_CPLXCFIR_CH1_BASEADDR;
		addr_base[1] = APB_RX_CPLXCFIR_CH2_BASEADDR;	
		orx_sel[0] = 0;
		orx_sel[1] = 0;
		break;
	case FR_OBSRX1_CPLX_FIR 	:
		addr_base[0] = APB_RX_CPLXCFIR_CH1_BASEADDR;
		orx_sel[0] = 1;
		break;
	case FR_OBSRX2_CPLX_FIR 	:
		addr_base[1] = APB_RX_CPLXCFIR_CH2_BASEADDR;
		orx_sel[1] = 1;
		break;
	case FR_OBSRX1RX2_CPLX_FIR 	:
		addr_base[0] = APB_RX_CPLXCFIR_CH1_BASEADDR;
		addr_base[1] = APB_RX_CPLXCFIR_CH2_BASEADDR;
		orx_sel[0] = 1;
		orx_sel[1] = 1;
		break;
	default:
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_PROGRAMFIR_INV_FIRNAME_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}
	
	for(ch = 0; ch < 2; ch++)
	{
		if(addr_base[ch] == 0)
			continue;
		
		retVal = FR9009_armSpiCmd_writeField(device, addr_base[ch]+0x8, coeff_vld, 0xff << (16+orx_sel[ch]*8), (16+orx_sel[ch]*8));
		IF_ERR_RETURN_U32(retVal);
		retVal = FR9009_armSpiCmd_writeField(device, addr_base[ch]+0xc, coeff_vld, 0xff << (0+orx_sel[ch]*8), (0+orx_sel[ch]*8));
		IF_ERR_RETURN_U32(retVal);
		retVal = FR9009_armSpiCmd_writeField(device, addr_base[ch]+0xc, coeff_vld, 0xff << (16+orx_sel[ch]*8), (16+orx_sel[ch]*8));
		IF_ERR_RETURN_U32(retVal);
		retVal = FR9009_armSpiCmd_writeWords(device, addr_base[ch]+0x18+orx_sel[ch]*64, (uint32_t*)firFilter->coefs_II, firFilter->numFirCoefs/2);
		IF_ERR_RETURN_U32(retVal);
		retVal = FR9009_armSpiCmd_writeWords(device, addr_base[ch]+0x98+orx_sel[ch]*64, (uint32_t*)firFilter->coefs_IQ, firFilter->numFirCoefs/2);
		IF_ERR_RETURN_U32(retVal);
		retVal = FR9009_armSpiCmd_writeWords(device, addr_base[ch]+0x118+orx_sel[ch]*64, (uint32_t*)firFilter->coefs_QI, firFilter->numFirCoefs/2);
		IF_ERR_RETURN_U32(retVal);
		retVal = FR9009_armSpiCmd_writeWords(device, addr_base[ch]+0x198+orx_sel[ch]*64, (uint32_t*)firFilter->coefs_QQ, firFilter->numFirCoefs/2);
		IF_ERR_RETURN_U32(retVal);
	}
	
	return (uint32_t)retVal;
}

uint32_t FR9009_internalSysrefClock(fr9009Device_t *device,  uint32_t sysrefIsDeviceClkDiv)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	fRHALErr_t halError = FRHAL_OK;
#if FR9009_VERBOSE
	halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK, "FR9009_internalSysrefClock()\n");
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal,  TALACT_WARN_RESET_LOG);
#endif
	retVal = FR9009_armSpiCmd_writeField(device, APB_CLK_RST_BASEADDR + 0x80, 0x11000000 | (sysrefIsDeviceClkDiv-1), 0x117FFFFF, 0);
	IF_ERR_RETURN_U32(retVal); 

	return (uint32_t)retVal;
}

uint32_t FR9009_calculateDigitalClocks(fr9009Device_t *device,  fr9009DigClocks_t *digClocks)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	fRHALErr_t halError = FRHAL_OK;
	uint32_t hsClkDivHsDigClk4or5 =  1;
	uint32_t localHsDigClkDiv2_Hz = 0;
	uint32_t localHsDigClk4or5_Hz = 0;
#if FR9009_VERBOSE
	halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK, "FR9009_calculateDigitalClocks()\n");
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal,  TALACT_WARN_RESET_LOG);
#endif

	switch(digClocks->clkPllHsDiv) {
		case 8:
			hsClkDivHsDigClk4or5 = 8 * 5;
			break;
		case 5:
			hsClkDivHsDigClk4or5 = 5 * 4;
			break;
		default:
			return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,  FR_ERR_CLKPLL_INV_HSDIV, retVal, TALACT_ERR_CHECK_PARAM );
	}

	device->devStateInfo.clocks.clkPllVcoFreq_kHz = digClocks->clkPllVcoFreq_kHz;
	device->devStateInfo.clocks.clkPllHsDiv = digClocks->clkPllHsDiv;
	device->devStateInfo.clocks.deviceClock_kHz = digClocks->deviceClock_kHz;

	localHsDigClkDiv2_Hz = (uint32_t)DIV_U64(((uint64_t)(digClocks->clkPllVcoFreq_kHz) * 500) , digClocks->clkPllHsDiv);
	device->devStateInfo.clocks.hsDigClkDiv2_Hz = localHsDigClkDiv2_Hz;
	localHsDigClk4or5_Hz = (uint32_t)DIV_U64(((uint64_t)(digClocks->clkPllVcoFreq_kHz) * 1000) , hsClkDivHsDigClk4or5);
	device->devStateInfo.clocks.hsDigClkDiv4or5_Hz = localHsDigClk4or5_Hz;
	return (uint32_t)retVal;
}

/**
 * \brief Verifies the Rx profile members are valid (in range) and calculates HS Dig Clock require for the Rx Profile
 *
 * Private helper function to verify the Rx profile members are valid (in range)
 * and calculates HS Dig Clock require for the Rx Profile
 * If the Rx profile IQ data rate = 0, it is assumed that the Rx profile is
 * not used.  If Rx IQ data rate > 0, and Rx profile members are out of range,
 * the function stores the error code describing the error, and returns a
 * Recovery action to check the configuration settings.
 *
 * \pre This function is private and is not called directly by the user.
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Structure pointer to FR9009 device data structure
 * \param rxProfile rxProfile settings to be verified
 * \param rxHsDigClk_kHz Return value of the calculated HS Dig Clock required by the Rx profile
 *
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
static talRecoveryActions_t talVerifyRxProfile(fr9009Device_t *device, fr9009RxProfile_t *rxProfile, uint32_t *rxHsDigClk_kHz, uint32_t *profileMtxIdx)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint8_t ddcMultiply = 1;
	uint8_t ddcDivide = 1;
	uint32_t hsDiv = device->devStateInfo.clocks.clkPllHsDiv;
	uint32_t adcClk_KHz = device->devStateInfo.clocks.hsDigClkDiv2_Hz/500;
	uint32_t decRatio = 1;
	uint32_t cfirDecRatio = 1;
	uint32_t modeCode = 0;
	int8_t modeHit = 0;
	uint32_t i = 0;
	*rxHsDigClk_kHz = 0;

	/********************************/
	/* Check for a valid Rx profile */
	/********************************/
	if ((rxProfile->rxOutputRate_kHz < 15000) || (rxProfile->rxOutputRate_kHz > 500000)) {
		return talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_VERRXPFILE_INV_IQRATE, retVal, TALACT_ERR_CHECK_PARAM);
	}	
	switch(rxProfile->rxIfConvMode[0]) {
		case FR_IFCONV_BYPASS:
		case FR_IFCONV_FILTERONLY:
		case FR_IFCONV_SHIFTONLY:	
		case FR_IFCONV_FILTERONLY_REALIF:
		case FR_IFCONV_BYPASS_REALIF:
			ddcMultiply = 1;
			ddcDivide = 1;
			break;		
		case FR_IFCONV_DEC2_REALIF:
		case FR_IFCONV_DEC2:
			ddcMultiply = 1;
			ddcDivide = 2;
			break;
		case FR_IFCONV_INT2_REALIF:
		case FR_IFCONV_INT2:
			ddcMultiply = 2;
			ddcDivide = 1;
			break;
		default:
			return talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_VERRXPFILE_INV_DDC, retVal, TALACT_ERR_CHECK_PARAM );
	}
	decRatio = adcClk_KHz / rxProfile->rxOutputRate_kHz / ddcDivide * ddcMultiply;
   	/* when hsDiv == 5, Dec5 must be 8/4/2; when hsDiv != 5, Dec5 must be 5.*/
	if ((hsDiv * decRatio) % 10 != 0) {
		return talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_VERRXPFILE_INV_DEC5, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if(rxProfile->rxAdcMode == FR_ADC_IQ4ITLV){
		decRatio = decRatio;
	}else if(rxProfile->rxAdcMode == FR_ADC_IQ2ITLV){
		decRatio = decRatio >> 1;
	}else if(rxProfile->rxAdcMode == FR_ADC_IQ){
		decRatio = decRatio >> 2;
	}else{
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_INIT_NULLPARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}
	
	modeCode = (((uint32_t)rxProfile->rxAdcMode) << 24) | (decRatio << 8);
	for(i = 0; i < rxProfileNum; i++){
		if(modeCode == ((rxProfileMatrix[i][0]) & 0xFFFFFF00)){
			modeHit = 1;
			*profileMtxIdx = i;
			cfirDecRatio = rxProfileMatrix[i][0] & 0xFF;
			break;
		}
	}
	if (modeHit == 0) {
		return talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_VERRXPFILE_INV_RHB0, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if ((rxProfile->rxFir.coefs == NULL) && (cfirDecRatio != 1)) {
		return talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_VERRXPFILE_INV_COEF, retVal, TALACT_ERR_CHECK_PARAM);
	}

	*rxHsDigClk_kHz = adcClk_KHz;

	return retVal;
}

/**
 * \brief Verifies the ORx profile members are valid (in range) in the init structure
 *
 * If the ORx profile IQ data rate = 0, it is assumed that the ORx profile is
 * not used.  If ORx IQ data rate > 0, and ORx profile members are out of range,
 * the function stores the error code describing the error, and returns a
 * Recovery action to check the configuration settings.
 *
 * \pre This function is private and is not called directly by the user.
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Structure pointer to FR9009 device data structure
 * \param orxProfile orxProfile settings to be verified
 * \param orxHsDigClk_kHz Return value of the calculated HS Dig Clock required by the ORx profile
 *
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
static talRecoveryActions_t talVerifyOrxProfile(fr9009Device_t *device, fr9009RxProfile_t *orxProfile, uint32_t *orxHsDigClk_kHz, uint32_t *profileMtxIdx)
{
	return talVerifyRxProfile(device, orxProfile, orxHsDigClk_kHz, profileMtxIdx);
}

/**
 * \brief Verifies the Tx profile members are valid (in range) in the init structure
 *
 * If the Tx profile IQ data rate = 0, it is assumed that the Tx profile is
 * not used.  If Tx IQ data rate > 0, and Tx profile members are out of range,
 * the function stores the error code describing the error, and returns a
 * Recovery action to check the configuration settings.
 *
 * \pre This function is private and is not called directly by the user.
 *
 * \dep_begin
 * \dep{device->devHalInfo}
 * \dep_end
 *
 * \param device Structure pointer to FR9009 device data structure
 * \param txProfile txProfile settings to be verified
 * \param txHsDigClk_kHz Return value of the calculated HS Dig Clock required by the Tx profile
 *
 * \retval TALACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval TALACT_NO_ACTION Function completed successfully, no action required
 */
static talRecoveryActions_t talVerifyTxProfile(fr9009Device_t *device, fr9009TxProfile_t *txProfile, uint32_t *txHsDigClk_kHz, uint32_t *profileMtxIdx)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint8_t ducMultiply = 1;
	uint8_t ducDivide = 1;	
	uint32_t hsDiv = device->devStateInfo.clocks.clkPllHsDiv;
	uint32_t dacClk_KHz = device->devStateInfo.clocks.hsDigClkDiv2_Hz/500;
	uint32_t interpolationRatio = 1;
	uint32_t cfirInterpolationRatio = 1;
	uint32_t modeCode = 0;
	uint8_t  modeHit = 0;
	uint32_t i = 0;
	*txHsDigClk_kHz = 0;
	/********************************/
	/* Check for a valid Tx profile */
	/********************************/

	if ((txProfile->txInputRate_kHz < 15000) || (txProfile->txInputRate_kHz > 500000)) {
		return talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
					FR_ERR_VERTXPFILE_INV_IQRATE, retVal, TALACT_ERR_CHECK_PARAM);
	}

	switch(txProfile->txIfConvMode[0]) {
		case FR_IFCONV_BYPASS:
		case FR_IFCONV_FILTERONLY:
		case FR_IFCONV_SHIFTONLY:	
		case FR_IFCONV_BYPASS_REALIF:
			ducMultiply = 1;
			ducDivide = 1;
			break;		
		case FR_IFCONV_DEC2:
		case FR_IFCONV_DEC2_REALIF:
			ducMultiply = 1;
			ducDivide = 2;
			break;		
		case FR_IFCONV_INT2:
		case FR_IFCONV_INT2_REALIF:
			ducMultiply = 2;
			ducDivide = 1;
			break;	
		case FR_IFCONV_FILTERONLY_REALIF:
		default:
			return talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_VERRXPFILE_INV_DDC, retVal, TALACT_ERR_CHECK_PARAM );
	}

	interpolationRatio = dacClk_KHz / txProfile->txInputRate_kHz * ducDivide / ducMultiply;
    /* when hsDiv == 5, Inpt5 must be 8/4/2; when hsDiv != 5, Inpt5 must be 5 */
	if ((hsDiv * interpolationRatio) % 10 != 0){
		return talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_VERTXPFILE_INV_HBMUTEX, retVal, TALACT_ERR_CHECK_PARAM);
	}

	modeCode = interpolationRatio << 8;
	for(i = 0; i < txProfileNum; i++){
		if(modeCode == ((txProfileMatrix[i][0]) & 0xFFFFFF00)){
			modeHit = 1;
			*profileMtxIdx = i;
			cfirInterpolationRatio = txProfileMatrix[i][0] & 0xFF;
			break;
		}
	}
	if (modeHit == 0) {
		return talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_VERTXPFILE_INV_THB0, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if ((txProfile->txFir.coefs == NULL) && (cfirInterpolationRatio != 1)) {
		return talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_VERTXPFILE_INV_COEF, retVal, TALACT_ERR_CHECK_PARAM);
	}

	*txHsDigClk_kHz = dacClk_KHz;

	return retVal;
}

uint32_t FR9009_verifyProfiles(fr9009Device_t *device, fr9009Init_t *init)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t rxHsDigClk_kHz = 0;
	uint32_t orxHsDigClk_kHz = 0;
	uint32_t txHsDigClk_kHz = 0;
	// uint32_t hsDigClk_kHz = 0;
	fr9009RxProfile_t *rxProfile  = NULL;
	fr9009TxProfile_t *txProfile = NULL;
	fr9009RxProfile_t *orxProfile  = NULL;

	if (device == NULL) {
		/* Can not write to log since log function requires device data structure */
		return (uint32_t)TALACT_ERR_CHECK_PARAM;
	}

	if (init == NULL) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
						  FR_ERR_INV_NULL_INIT_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	device->devStateInfo.profilesValid = 0;
#if 1
	if(init->tx.txChannels != FR_TXOFF) {
		txProfile = &init->tx.txProfile;
		retVal = talVerifyTxProfile(device, txProfile, &txHsDigClk_kHz, &device->devStateInfo.txProfileMtxIdx);
		IF_ERR_RETURN_U32(retVal);
		device->devStateInfo.txFirInterpolation = txProfileMatrix[device->devStateInfo.txProfileMtxIdx][0] & 0xFF;
	}

	if(init->rx.rxChannels != FR_RXOFF) {
		rxProfile = &init->rx.rxProfile;
		retVal = talVerifyRxProfile(device, rxProfile, &rxHsDigClk_kHz, &device->devStateInfo.rxProfileMtxIdx);
		IF_ERR_RETURN_U32(retVal);
		device->devStateInfo.rxFirDecimation = rxProfileMatrix[device->devStateInfo.rxProfileMtxIdx][0] & 0xFF;
	}

	if((init->obsRx.obsRxChannelsEnable != FR_ORXOFF) || (init->tx.txChannels != FR_TXOFF)) {
		// if(init->tx.txChannels != FR_TXOFF)
		// {
		// 	if(init->tx.txProfile.txInputRate_kHz != init->obsRx.orxProfile.rxOutputRate_kHz)
		// 	{
		// 		UART_Printf("ORX IQ rate is not equal to TX IQ rate\r\n");
		// 	}
		// }
		if(init->rx.rxChannels != FR_RXOFF)
		{
			if(init->rx.rxProfile.rxAdcMode != init->obsRx.orxProfile.rxAdcMode)
			{
				return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_INV_SUBADC_NUM, retVal, TALACT_ERR_CHECK_PARAM);
			}
		}
		orxProfile = &init->obsRx.orxProfile;
		retVal = talVerifyOrxProfile(device, orxProfile, &orxHsDigClk_kHz, &device->devStateInfo.orxProfileMtxIdx);
		IF_ERR_RETURN_U32(retVal);
		device->devStateInfo.orxFirDecimation = rxProfileMatrix[device->devStateInfo.orxProfileMtxIdx][0] & 0xFF;
	}

	if ((init->clocks.rfPllPhaseSyncMode != FR_RFPLLMCS_NOSYNC)  
		&& (init->clocks.rfPllPhaseSyncMode != FR_RFPLLMCS_INIT_AND_SYNC)
	    && (init->clocks.rfPllPhaseSyncMode != FR_RFPLLMCS_INIT_AND_1TRACK) 
		&& (init->clocks.rfPllPhaseSyncMode != FR_RFPLLMCS_INIT_AND_CONTTRACK)) 
	{
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_VERPFILE_INV_RFPLLMCSMODE, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Verify Tx profile is valid */
	if (txHsDigClk_kHz > 0) 
	{
		device->devStateInfo.profilesValid |= TX_PROFILE_VALID;
	}

	/* Verify Rx profile is valid */
	if (rxHsDigClk_kHz > 0) 
	{
		device->devStateInfo.profilesValid |= RX_PROFILE_VALID;
	}

	/* Verify ORx profile is valid */
	if (orxHsDigClk_kHz > 0) 
	{
		device->devStateInfo.profilesValid |= ORX_PROFILE_VALID;
	}
#else
		device->devStateInfo.profilesValid |= TX_PROFILE_VALID;
		device->devStateInfo.profilesValid |= RX_PROFILE_VALID;
		device->devStateInfo.profilesValid |= ORX_PROFILE_VALID;
#endif
	return (uint32_t)retVal;
}

uint32_t FR9009_setSpiSettings(fr9009Device_t *device,fr9009SpiSettings_t *spi)
{
    talRecoveryActions_t retVal = TALACT_NO_ACTION;
    fRHALErr_t halError = FRHAL_OK;

#if FR9009_VERBOSE
	halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK, "TALISE_setSpiSettings()\n");
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal, TALACT_WARN_RESET_LOG);
#endif

	if (spi->MSBFirst == 0) {
		(uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERRHDL_HAL_SPI, retVal, TALACT_ERR_CHECK_PARAM);/* SPI bit is 1, only MSB*/
	}

	if (spi->autoIncAddrUp == 0) {
		(uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERRHDL_HAL_SPI, retVal, TALACT_ERR_CHECK_PARAM);/* SPI bit is 1, only autoIncAddrUp*/
	}

	retVal = FR9009_armSpiCmd_writeField(device, FR9009_ADDR_SPI_SDIO_BDIR_OFF, spi->fourWireMode > 0 ? 1:0, FR9009_ADDR_SPI_SDIO_BDIR_OFF_MASK, FR9009_ADDR_SPI_SDIO_BDIR_OFF_OFFSET);
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal, TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Not Supported
	if (( spi->cmosPadDrvStrength != FR_CMOSPAD_DRV_1X) &&
	    ( spi->cmosPadDrvStrength != FR_CMOSPAD_DRV_2X) &&
	    ( spi->cmosPadDrvStrength != FR_CMOSPAD_DRV_3X) &&
	    ( spi->cmosPadDrvStrength != FR_CMOSPAD_DRV_4X) &&
	    ( spi->cmosPadDrvStrength != FR_CMOSPAD_DRV_5X) &&
	    ( spi->cmosPadDrvStrength != FR_CMOSPAD_DRV_6X) &&
	    ( spi->cmosPadDrvStrength != FR_CMOSPAD_DRV_8X) &&
	    ( spi->cmosPadDrvStrength != FR_CMOSPAD_DRV_10X)) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
						  FR_ERR_SETSPI_INV_CMOS_DRV_STR, retVal, TALACT_ERR_CHECK_PARAM);
	} else {		
	}*/

	/* Check if SPI reads and writes work after changing settings */
	retVal = (talRecoveryActions_t)FR9009_verifySpiReadWrite (device);
	IF_ERR_RETURN_U32(retVal);

	return (uint32_t)retVal;
}

uint32_t FR9009_verifySpiReadWrite (fr9009Device_t *device)
{
    talRecoveryActions_t retVal = TALACT_NO_ACTION;
    fRHALErr_t halError = FRHAL_OK;
	int i = 0;
#if FR9009_VERBOSE
	halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK, "FR9009_verifySpiReadWrite()\n");
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal, TALACT_WARN_RESET_LOG);
#endif
	uint32_t wbuf[5] = {0x12345678, 0x2, 0x3, 0x4, 0x5};
	uint32_t rbuf[5] = {0};
	uint32_t addr = FR9009_ADDR_EXTDATA_FLAG | 0;
    retVal = FR9009_armSpiCmd_writeWords(device, addr, wbuf, 5);
    IF_ERR_RETURN_U32(retVal);
    retVal = FR9009_armSpiCmd_readWords(device, addr, rbuf, 5);
    IF_ERR_RETURN_U32(retVal);
    for(i = 0; i < 5; i++){
    	if(rbuf[i] != wbuf[i]){
    		// UART_Printf("[%d] rbuf	0x%08X,	wbuf	0x%08X\r\n",i, rbuf[i], wbuf[i]);
			return FRHAL_SPI_FAIL;
    	}
    }
	return FRHAL_OK;
}

uint32_t FR9009_initDigitalClocks(fr9009Device_t *device, fr9009DigClocks_t *clockSettings)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	fRHALErr_t halError = FRHAL_OK;
	// uint32_t regda = 0;
	uint32_t decRatio = 0;
	uint32_t HsDivCode = 0;

#if FR9009_VERBOSE
	halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK, "FR9009_initDigitalClocks()\n");
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal, TALACT_WARN_RESET_LOG);
#endif
	if (clockSettings->deviceClock_kHz > 10000 && clockSettings->deviceClock_kHz <= 1000000) {
		// regda = clockSettings->deviceClock_kHz*1000;
		// retVal = FR9009_armSpiCmd_writeWords(device, FR9009_ADDR_REFCLK_FREQ,  &regda, 1);
		// IF_ERR_RETURN_U32(retVal);

		decRatio = clockSettings->clkPllHsDiv == 5 ? 1:2;
		retVal = FR9009_armSpiCmd_writeField(device, APB_CLK_RST_BASEADDR+0x30, decRatio, 0x3, 0);
		IF_ERR_RETURN_U32(retVal);		
		
		switch (clockSettings->clkPllHsDiv)
		{
		case 5:
			HsDivCode = 8;
			break;
		case 8:
			HsDivCode = 7;
			break;	
		case 4:
			HsDivCode = 6;
			break;	
		case 2:
			HsDivCode = 4;
		break;	
		default:
			return (uint32_t)talApiErrHandler(device,FR_ERRHDL_INVALID_PARAM, FR_ERR_CLKPLL_INV_HSDIV, retVal, TALACT_ERR_CHECK_PARAM);
			break;
		}
		retVal = FR9009_armSpiCmd_writeField(device, FR9009_ADDR_ARMSPI(SPI_BBPLL_ID, 0x3C), HsDivCode, 0xF, 0);
		IF_ERR_RETURN_U32(retVal);	
	} else {
		return (uint32_t)talApiErrHandler(device,FR_ERRHDL_INVALID_PARAM, FR_ERR_INV_SCALEDDEVCLK_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	return (uint32_t)retVal;
}

uint32_t FR9009_setTxPfirSyncClk(fr9009Device_t *device,  fr9009TxProfile_t *txProfile)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	// fRHALErr_t halError = FRHAL_OK;
	// static const uint8_t NUM_MULT_PER_ROW = 20;
	// static const uint8_t MAX_NUM_TAPS = 80;
	// uint8_t effectiveRows = 0;
	// uint8_t numRows = 0;
	// uint32_t dpClk_kHz = 0;
	// uint32_t syncClk_kHz = 0;
	// uint32_t hsDigClkDiv4or5_kHz = 0;
	// uint8_t syncDiv = 0;
//TODO

	return (uint32_t)retVal;
}

uint32_t FR9009_setRxPfirSyncClk(fr9009Device_t *device,  fr9009RxProfile_t *rxProfile, fr9009RxProfile_t *orxProfile)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	/* Write Rx/Orx PFIR SYNC Clock divider */
//TODO
	IF_ERR_RETURN_U32(retVal);

	return (uint32_t)retVal;
}

uint32_t FR9009_getApiVersion (fr9009Device_t *device, uint32_t *siVer,  uint32_t *majorVer, uint32_t *minorVer, uint32_t *buildVer)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	fRHALErr_t halError = FRHAL_OK;

#if FR9009_VERBOSE
	halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK,  "FR9009_getApiVersion()\n");
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal,  TALACT_WARN_RESET_LOG);
#endif

	if ((siVer == NULL) || (majorVer == NULL) || (minorVer == NULL) || (buildVer == NULL)) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,  FR_ERR_GETAPIVERSION_NULLPARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	*siVer = FR_CURRENT_SI_VERSION;
	*majorVer = FR_CURRENT_MAJOR_VERSION;
	*minorVer = FR_CURRENT_MINOR_VERSION;
	*buildVer = FR_CURRENT_BUILD_VERSION;
	return (uint32_t)retVal;
}

uint32_t FR9009_getDeviceRev(fr9009Device_t *device, uint32_t *revision)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	fRHALErr_t halError = FRHAL_OK;

#if FR9009_VERBOSE
	halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK, "FR9009_getDeviceRev()\n");
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal, TALACT_WARN_RESET_LOG);
#endif

	if (revision == NULL) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_GETDEVICEREV_NULLPARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}
	halError = FR9009_armSpiCmd_readWords(device, FR9009_ADDR_HW_REVISION, revision, 1);
    retVal = talApiErrHandler(device,FR_ERRHDL_HAL_SPI, halError, retVal, TALACT_ERR_RESET_SPI);
//TODO
	return (uint32_t)retVal;
}

uint32_t FR9009_getProductId(fr9009Device_t *device, uint8_t *productId)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	fRHALErr_t halError = FRHAL_OK;
	uint32_t regdat;
#if FR9009_VERBOSE
	halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK, "FR9009_getProductId()\n");
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal, TALACT_WARN_RESET_LOG);
#endif

	if (productId == NULL) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_GETPRODUCTID_NULLPARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}
	halError = FR9009_armSpiCmd_readWords(device, FR9009_ADDR_HW_REVISION, &regdat, 1);
    retVal = talApiErrHandler(device,FR_ERRHDL_HAL_SPI, halError, retVal, TALACT_ERR_RESET_SPI);
	*productId = regdat;
	return (uint32_t)retVal;
}
