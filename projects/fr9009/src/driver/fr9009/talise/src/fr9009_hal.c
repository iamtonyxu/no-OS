/**
 * \file fr9009_hal.c
 * \brief Contains TALISE API Hardware Abstraction Layer (HAL) functions
 *
 * Copyright 2022 BriRadio.
 * Released under the FR9009 API license, for more information see the "LICENSE.txt" file in this zip file.
 */

#include "fr9009_hal.h"

fRHALErr_t brSpiReadWord(void *devHalInfo, uint32_t addr, uint32_t *readdata)
{
	fRHALErr_t halError = FRHAL_OK;

	halError = FRHAL_spiReadWord(devHalInfo, addr, readdata);	
	if (halError == FRHAL_WAIT_TIMEOUT) {
		FRHAL_setTimeout(devHalInfo, HAL_TIMEOUT_DEFAULT * HAL_TIMEOUT_MULT);
		halError = FRHAL_spiReadWord(devHalInfo, addr, readdata);
	}

	FRHAL_setTimeout(devHalInfo, HAL_TIMEOUT_DEFAULT);
	return halError;
}

fRHALErr_t brSpiWriteWord(void *devHalInfo, uint32_t addr, uint32_t data)
{
	fRHALErr_t halError = FRHAL_OK;

	halError = FRHAL_spiWriteWord(devHalInfo, addr, data);
	if (halError == FRHAL_WAIT_TIMEOUT) {
		FRHAL_setTimeout(devHalInfo, HAL_TIMEOUT_DEFAULT * HAL_TIMEOUT_MULT);
		halError = FRHAL_spiWriteWord(devHalInfo, addr, data);
	}

	FRHAL_setTimeout(devHalInfo, HAL_TIMEOUT_DEFAULT);
	return halError;
}

fRHALErr_t brSpiBlockReadWords(void *devHalInfo, uint32_t addr, uint32_t *readdata, uint32_t count)
{
	fRHALErr_t halError = FRHAL_OK;

	halError = FRHAL_spiBlockReadWords(devHalInfo, addr, readdata, count);
	if (halError == FRHAL_WAIT_TIMEOUT) {
		FRHAL_setTimeout(devHalInfo, HAL_TIMEOUT_DEFAULT * HAL_TIMEOUT_MULT);
		halError = FRHAL_spiBlockReadWords(devHalInfo, addr, readdata, count);
	}

	FRHAL_setTimeout(devHalInfo, HAL_TIMEOUT_DEFAULT);
	return halError;
}

fRHALErr_t brSpiBlockWriteWords(void *devHalInfo, uint32_t addr, uint32_t *data, uint32_t count)
{
	fRHALErr_t halError = FRHAL_OK;

	halError = FRHAL_spiBlockWriteWords(devHalInfo, addr, data, count);
	if (halError == FRHAL_WAIT_TIMEOUT) {
		FRHAL_setTimeout(devHalInfo, HAL_TIMEOUT_DEFAULT * HAL_TIMEOUT_MULT);
		halError = FRHAL_spiBlockWriteWords(devHalInfo, addr, data, count);
	}

	FRHAL_setTimeout(devHalInfo, HAL_TIMEOUT_DEFAULT);
	return halError;
}

// fRHALErr_t brSpiWriteWords(void *devHalInfo, uint32_t *addr, uint32_t *data,
// 			     uint32_t count)
// {
// 	fRHALErr_t halError = FRHAL_OK;

// 	halError = FRHAL_spiWriteWords(devHalInfo, addr, data, count);
// 	if (halError == FRHAL_WAIT_TIMEOUT) {
// 		FRHAL_setTimeout(devHalInfo, HAL_TIMEOUT_DEFAULT * HAL_TIMEOUT_MULT);
// 		halError = FRHAL_spiWriteWords(devHalInfo, addr, data, count);
// 	}

// 	FRHAL_setTimeout(devHalInfo, HAL_TIMEOUT_DEFAULT);
// 	return halError;
// }

// fRHALErr_t brSpiReadWords(void *devHalInfo, uint32_t *addr, uint32_t *readdata,
// 			    uint32_t count)
// {
// 	fRHALErr_t halError = FRHAL_OK;

// 	halError = FRHAL_spiReadWords(devHalInfo, addr, readdata, count);
// 	if (halError == FRHAL_WAIT_TIMEOUT) {
// 		FRHAL_setTimeout(devHalInfo, HAL_TIMEOUT_DEFAULT * HAL_TIMEOUT_MULT);
// 		halError = FRHAL_spiReadWords(devHalInfo, addr, readdata, count);
// 	}

// 	FRHAL_setTimeout(devHalInfo, HAL_TIMEOUT_DEFAULT);
// 	return halError;
// }

fRHALErr_t brSpiReadField(void *devHalInfo, uint32_t addr, uint32_t *fieldVal,
			    uint32_t mask, uint32_t startBit)
{
	fRHALErr_t halError = FRHAL_OK;

	halError = FRHAL_spiReadField(devHalInfo, addr, fieldVal, mask, startBit);
	if (halError == FRHAL_WAIT_TIMEOUT) {
		FRHAL_setTimeout(devHalInfo, HAL_TIMEOUT_DEFAULT * HAL_TIMEOUT_MULT);
		halError = FRHAL_spiReadField(devHalInfo, addr, fieldVal, mask, startBit);
	}

	FRHAL_setTimeout(devHalInfo, HAL_TIMEOUT_DEFAULT);
	return halError;
}

fRHALErr_t brSpiWriteField(void *devHalInfo, uint32_t addr, uint32_t fieldVal,
			     uint32_t mask, uint32_t startBit)
{
	fRHALErr_t halError = FRHAL_OK;

	halError = FRHAL_spiWriteField(devHalInfo, addr, fieldVal, mask, startBit);
	if (halError == FRHAL_WAIT_TIMEOUT) {
		FRHAL_setTimeout(devHalInfo, HAL_TIMEOUT_DEFAULT * HAL_TIMEOUT_MULT);
		halError = FRHAL_spiWriteField(devHalInfo, addr, fieldVal, mask, startBit);
	}

	FRHAL_setTimeout(devHalInfo, HAL_TIMEOUT_DEFAULT);
	return halError;
}

fRHALErr_t brWriteToLog(void *devHalInfo, brLogLevel_t logLevel,
			  uint32_t errorCode, const char *comment)
{
	fRHALErr_t halError = FRHAL_OK;

	halError = FRHAL_writeToLog(devHalInfo, logLevel, errorCode, comment);

	return halError;
}
