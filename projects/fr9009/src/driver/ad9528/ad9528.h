/**
 * \file ad9528.h
 * \brief Contains function declarations and ad9528Device_t structure typedef for ad9528.c
 */

/**
* \page Disclaimer Legal Disclaimer
* Copyright 2015-2017 Analog Devices Inc.
* Released under the AD9371 API license.
*
*/

#ifndef _AD9528_H_
#define _AD9528_H_


#include "t_ad9528.h"
#include "fr_hal.h"

//Helper function to setup the 9528 device data structure specifically for the ADI transceiver evaluation cards
fRHALErr_t AD9528_initDeviceDataStruct(ad9528Device_t *device,
					uint32_t vcxoFrequency_Hz, uint32_t refAFrequency_Hz,
					uint32_t outputDeviceClock_Hz);

//AD9528 API functions
fRHALErr_t AD9528_resetDevice(ad9528Device_t *device);
fRHALErr_t AD9528_initialize(ad9528Device_t
			      *device); //writes all registers to desired values, leaves clock outputs off.
fRHALErr_t AD9528_setupSYSREF(ad9528Device_t *device,
			       uint16_t divideFromPll1Out,  sysrefPatternMode_t sysrefPatternMode,
			       sysrefNshotMode_t nShotPulses);
fRHALErr_t AD9528_requestSysref(ad9528Device_t *device, uint8_t enableSYSREF);
fRHALErr_t AD9528_sysrefReq(ad9528Device_t *device,
			     ad9528SysrefReqMode_t mode);
fRHALErr_t AD9528_remove(ad9528Device_t *device);


fRHALErr_t  AD9528_spiWriteByte(ad9528Device_t *device, uint16_t addr,
				 uint8_t data);
fRHALErr_t  AD9528_spiReadByte(ad9528Device_t *device, uint16_t addr,
				uint8_t *readdata);

#endif
