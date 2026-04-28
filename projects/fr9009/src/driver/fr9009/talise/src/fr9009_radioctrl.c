/**
 * \file fr9009_radioctrl.c
 * \brief Contains functions to support FR9009 radio control and pin control
 *        functions
 *
 * Copyright 2022 BriRadio.
 * Released under the FR9009 API license, for more information see the "LICENSE.txt" file in this zip file.
 */

#include "fr9009_cals.h"
#include "fr9009_radioctrl.h"
#include "fr9009_gpio.h"
#include "fr9009_reg_addr_macros.h"
#include "fr9009_arm_macros.h"
#include "fr9009_hal.h"
#include "fr9009_user.h"
#include "fr9009_error.h"
#include "fr9009_arm.h"
#include "fr9009.h"
#include "fr9009_arm_spi_cmd.h"


#if 0
uint32_t  FR9009_loadStreamFromBinary (fr9009Device_t *device, uint8_t *binary) 
{
	return 123;
} 
#endif

/*设置sel0和sel1使用gpio*/
uint32_t  FR9009_setArmGpioPins(fr9009Device_t *device, fr9009ArmGpioConfig_t *armGpio) 
{
	fr9009GpioShortConfig_t armpin_config;

	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	fRHALErr_t halError = FRHAL_OK;

#if FR9009_VERBOSE
	halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK, "FR9009_setArmGpioPins()\n");
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal, TALACT_WARN_RESET_LOG);
#endif

    /*Check passed pointers for NULL */
	if (armGpio == NULL){
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_SET_ARMGPIO_NULLPARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Update all used GPIO's */	
	armpin_config.p0.gpioPinSel = armGpio->orx1TxSel0Pin.gpioPinSel;
	armpin_config.p0.enable = (armGpio->orx1TxSel0Pin.enable > 0) ? 1 : 0;
	armpin_config.p1.gpioPinSel = armGpio->orx1TxSel1Pin.gpioPinSel;
	armpin_config.p1.enable = (armGpio->orx1TxSel1Pin.enable > 0) ? 1 : 0;
	armpin_config.p2.gpioPinSel = armGpio->orx2TxSel0Pin.gpioPinSel;
	armpin_config.p2.enable = (armGpio->orx2TxSel0Pin.enable > 0) ? 1 : 0;
	armpin_config.p3.gpioPinSel = armGpio->orx2TxSel1Pin.gpioPinSel;
	armpin_config.p3.enable = (armGpio->orx2TxSel1Pin.enable > 0) ? 1 : 0;

	/* Set FR9009 GPIO pins used by ARM */
	retVal = FR9009_setOrxTxPinSel(device, &armpin_config);
	IF_ERR_RETURN_U32(retVal);

	retVal = FR9009_armSpiCmd_writeField(device, FR9009_ADDR_TDD_RF_CONFIG1, (0 << 1), (1 << 1), 0);  //orxtx_sel enable 外部控制模式
	IF_ERR_RETURN_U32(retVal);		

	return (uint32_t)retVal;
}


uint32_t  FR9009_setTxRxEnablePinMode(fr9009Device_t *device) 
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	fRHALErr_t halError = FRHAL_OK;

#if FR9009_VERBOSE
	halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK, "FR9009_setTxRxEnablePins()\n");
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal, TALACT_WARN_RESET_LOG);
#endif

    /*config tx/rx enable pin mode*/
	retVal = FR9009_armSpiCmd_writeField(device, FR9009_ADDR_TDD_RF_CONFIG0, (1 << 0)|(1 << 1)|(1 << 2)|(1 << 3)|(0 << 7)|(0 << 10)|(0 << 21)|(0 << 22)|(0 << 23)|(0 << 24), (1 << 0)|(1 << 1)|(1 << 2)|(1 << 3)|(1 << 7)|(1 << 10)|(1 << 21)|(1 << 22)|(1 << 23)|(1 << 24), 0);  //tx rx pin mode
	IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_armSpiCmd_writeField(device, FR9009_ADDR_TDD_RF_CONFIG1, (0 << 0), (1 << 0), 0);//enable 外部控制模式
	IF_ERR_RETURN_U32(retVal);	
	retVal = FR9009_armSpiCmd_writeField(device, FR9009_ADDR_TDD_RF_CONFIG2, (0 << 0)|(0 << 1)|(0 << 4)|(0 << 5)|(1 << 8)|(1 << 9)|(1 << 10)|(1 << 11), (1 << 0)|(1 << 1)|(1 << 4)|(1 << 5)|(1 << 8)|(1 << 9)|(1 << 10)|(1 << 11), 0);  //tx rx pin mode
	IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_armSpiCmd_writeField(device, FR9009_ADDR_TDD_RF_CONFIG3, (0 << 0)|(0 << 1)|(0 << 2)|(0 << 3)|(1 << 8)|(1 << 9)|(1 << 10)|(1 << 11)|(0x3 << 14), (1 << 0)|(1 << 1)|(1 << 2)|(1 << 3)|(1 << 8)|(1 << 9)|(1 << 10)|(1 << 11)|(0x3 << 14), 0);  //tx rx pin mode
	IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_armSpiCmd_writeField(device, FR9009_ADDR_TDD_RF_CONFIG4, (0 << 0)|(0 << 1)|(0 << 2)|(0 << 3), (1 << 0)|(1 << 1)|(1 << 2)|(1 << 3), 0);  //tx rx pin mode
	IF_ERR_RETURN_U32(retVal);	
	retVal = FR9009_armSpiCmd_writeField(device, FR9009_ADDR_TDD_RF_CONFIG5, (0 << 0)|(0 << 4)|(1 << 8)|(1 << 9)|(1 << 10)|(1 << 11), (1 << 0)|(1 << 4)|(1 << 8)|(1 << 9)|(1 << 10)|(1 << 11), 0);  //tx rx pin mode
	IF_ERR_RETURN_U32(retVal);

	return (uint32_t)retVal;
}



uint32_t  FR9009_setRadioCtlPinMode (fr9009Device_t *device, fr9009ObsRxChannels_t orxChannel, fr9009GpioShortPinSel_t *orxEnGpioPinSel) 
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	fRHALErr_t halError = FRHAL_OK;
	fr9009GpioConfigOrxGpio_t config;

#if FR9009_VERBOSE
	halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK, "FR9009_setRadioCtlPinMode()\n");
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal, TALACT_WARN_RESET_LOG);
#endif
	if(((orxChannel != FR_ORX1) && (orxChannel != FR_ORX2)) || orxEnGpioPinSel == NULL){
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_INV_RADIO_CTL_MASK_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}
	/* checking for valid bit mask settings */
	if (orxEnGpioPinSel->gpioPinSel > FR_GPIO_INVALID) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_INV_RADIO_CTL_MASK_PARM, retVal, TALACT_ERR_CHECK_PARAM);
	}
	/* Disable ability for partial stream processor power down from Rx to
		* ORx or ORx to Rx  until stream processor is updated to support it
		*/
	retVal = FR9009_setGpioOe(device, (0x0 << orxEnGpioPinSel->gpioPinSel), (0x1 << orxEnGpioPinSel->gpioPinSel));
	IF_ERR_RETURN_U32(retVal);	
	retVal = FR9009_setGpioSourceCtrl(device, FR_GPIO_TX_ATT_CONTROL_MODE_2 << ((orxEnGpioPinSel->gpioPinSel / 4)*4));
	IF_ERR_RETURN_U32(retVal);
	retVal =FR9009_getOrxEnableGpio(device, &config);
	IF_ERR_RETURN_U32(retVal);
	
	if(orxChannel == FR_ORX1){
		config.orx1_enble_sel = orxEnGpioPinSel->enable > 0 ? 1 : 0;
		config.orx1_enable_gpio_sel.enable = orxEnGpioPinSel->enable > 0 ? 1 : 0;
		config.orx1_enable_gpio_sel.gpioPinSel = orxEnGpioPinSel->gpioPinSel;
		if(orxEnGpioPinSel->enable > 0){			
			retVal = FR9009_armSpiCmd_writeField(device, FR9009_ADDR_TDD_RF_CONFIG0, (0 << 25)|(1 << 20), (1 << 25)|(1 << 20), 0);//orx 外部控制模式
			IF_ERR_RETURN_U32(retVal);
			retVal = FR9009_armSpiCmd_writeField(device, FR9009_ADDR_TDD_RF_CONFIG1, (0 << 0), (1 << 0), 0);//orx 外部控制模式
			IF_ERR_RETURN_U32(retVal);
			retVal = FR9009_armSpiCmd_writeField(device, FR9009_ADDR_TDD_RF_CONFIG2, (0 << 0)|(1 << 8), (1 << 0)|(1 << 8), 0);//orx adc 外部控制模式
			IF_ERR_RETURN_U32(retVal);			
			retVal = FR9009_armSpiCmd_writeField(device, FR9009_ADDR_TDD_RF_CONFIG3, (0 << 4)|(1<< 12), (1 << 4)|(1<< 12), 0);//orx dp 外部控制模式
			IF_ERR_RETURN_U32(retVal);
			retVal = FR9009_armSpiCmd_writeField(device, FR9009_ADDR_TDD_RF_CONFIG5, (0 << 1), (1 << 1), 0);//orx dc 外部控制模式
			IF_ERR_RETURN_U32(retVal);
		}
	}else if(orxChannel == FR_ORX2){
		config.orx2_enble_sel = orxEnGpioPinSel->enable > 0 ? 1 : 0;
		config.orx2_enable_gpio_sel.enable = orxEnGpioPinSel->enable > 0 ? 1 : 0;
		config.orx2_enable_gpio_sel.gpioPinSel = orxEnGpioPinSel->gpioPinSel;
		if(orxEnGpioPinSel->enable > 0){
			retVal = FR9009_armSpiCmd_writeField(device, FR9009_ADDR_TDD_RF_CONFIG0, (0 << 26)|(1 << 20), (1 << 26)|(1 << 20), 0);//orx 外部控制模式
			IF_ERR_RETURN_U32(retVal);
			retVal = FR9009_armSpiCmd_writeField(device, FR9009_ADDR_TDD_RF_CONFIG1, (0 << 0), (1 << 0), 0);//orx 外部控制模式
			IF_ERR_RETURN_U32(retVal);
			retVal = FR9009_armSpiCmd_writeField(device, FR9009_ADDR_TDD_RF_CONFIG2, (0 << 1)|(1 << 9), (1 << 1)|(1 << 9), 0);//orx adc 外部控制模式
			IF_ERR_RETURN_U32(retVal);	
			retVal = FR9009_armSpiCmd_writeField(device, FR9009_ADDR_TDD_RF_CONFIG3, (0 << 5)|(1<< 13), (1 << 5)|(1<< 13), 0);//orx dp 外部控制模式
			IF_ERR_RETURN_U32(retVal);
			retVal = FR9009_armSpiCmd_writeField(device, FR9009_ADDR_TDD_RF_CONFIG5, (0 << 5), (1 << 5), 0);//orx dp 外部控制模式
			IF_ERR_RETURN_U32(retVal);
		}
	}
	
	retVal = FR9009_setOrxEnableGpio(device, &config);
	IF_ERR_RETURN_U32(retVal);

	retVal = FR9009_setTxRxEnablePinMode(device);
	IF_ERR_RETURN_U32(retVal);
	
	return (uint32_t)retVal;
}


uint32_t  FR9009_getRadioCtlPinMode (fr9009Device_t *device, fr9009ObsRxChannels_t orxChannel, fr9009GpioShortPinSel_t *orxEnGpioPinSel) 
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	fRHALErr_t halError = FRHAL_OK;
	fr9009GpioConfigOrxGpio_t config;

#if FR9009_VERBOSE
	halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK, "FR9009_getRadioCtlPinMode()\n");
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal, TALACT_WARN_RESET_LOG);
#endif
	/* checking for NULL pointers in function parameters */
	if(((orxChannel != FR_ORX1) && (orxChannel != FR_ORX2)) || orxEnGpioPinSel == NULL){
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_GETPINMODE_NULLPARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}
	retVal =FR9009_getOrxEnableGpio(device, &config);
	IF_ERR_RETURN_U32(retVal);
	if(orxChannel == FR_ORX1){
		orxEnGpioPinSel->enable = config.orx1_enable_gpio_sel.enable;
		orxEnGpioPinSel->gpioPinSel = config.orx1_enable_gpio_sel.gpioPinSel;
	}else if(orxChannel == FR_ORX2){
		orxEnGpioPinSel->enable = config.orx2_enable_gpio_sel.enable;
		orxEnGpioPinSel->gpioPinSel = config.orx2_enable_gpio_sel.gpioPinSel;
	}

	return (uint32_t)retVal;
}
#if 0
uint32_t FR9009_setOrxLoCfg(fr9009Device_t *device,
			    const fr9009OrxLoCfg_t *orxLoCfg)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	fRHALErr_t halError = FRHAL_OK;
	uint8_t enableRelock = 0;
	uint8_t gpio3StreamMaskValue = 0;
	uint8_t gpio3StreamMaskReadValue = 0;
	uint8_t gpioSel = 0;
	uint8_t gpioSelReadVal = 0;
	uint8_t byteOffset = 0;
	uint32_t radioStatus = 0;

	uint32_t usedGpioPins = 0;
	uint32_t freeGpioPins = 0;

#if FR9009_VERBOSE
	halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK,
				 "FR9009_setOrxLoCfg()\n");
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* Perform input paramter NULL and range checks */
	if (orxLoCfg == NULL) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
						  FR_ERR_SETORXLOCFG_NULL_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	if (orxLoCfg->gpioSelect == FR_GPIO_INVALID) {
		gpio3StreamMaskValue = 1;
		gpioSel = 0;
	} else if ((orxLoCfg->gpioSelect >= FR_GPIO_00)
		   && (orxLoCfg->gpioSelect <= FR_GPIO_15)) {
		gpio3StreamMaskValue = 0;
		gpioSel = (uint8_t)orxLoCfg->gpioSelect;
	} else {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
						  FR_ERR_SETORXLOCFG_INVALIDPARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* If this feature already enabled for any GPIO pin, free the pin */
	halError = brSpiReadField(device->devHalInfo, FR9009_ADDR_STREAM_CONTROL,
				   &gpio3StreamMaskReadValue, 0x80, 7);
	retVal = talApiErrHandler(device,FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = brSpiReadField(device->devHalInfo,
				   FR9009_ADDR_STREAM_GPIO_TRIGGER_PIN_SELECT_BYTE1, &gpioSelReadVal, 0xF0, 4);
	retVal = talApiErrHandler(device,FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	if (gpio3StreamMaskReadValue == 1) {
		/* FR_GPIO_INVALID */
		/* No GPIO pins to free */
	} else {
		freeGpioPins = (uint32_t)(1 << gpioSelReadVal);
	}

	/* Verify GPIO is not already used */
	if ((orxLoCfg->gpioSelect >= FR_GPIO_00)
	    && (orxLoCfg->gpioSelect <= FR_GPIO_15)) {
		usedGpioPins = (uint32_t)(1 << gpioSel);

		/* Check if pin selected is already being used by another feature */
		if ((device->devStateInfo.usedGpiopins & (usedGpioPins & ~freeGpioPins)) > 0) {
			return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
							  FR_ERR_SETORXLOCFG_GPIOUSED, retVal, TALACT_ERR_CHECK_PARAM);
		}
	}

	/* Verify ARM in radio Off state */
	retVal = (talRecoveryActions_t)FR9009_getRadioState(device, &radioStatus);
	IF_ERR_RETURN_U32(retVal);

	if (((radioStatus & 0x07) != FR9009_ARM_RADIO_STATUS_IDLE) &&
	    ((radioStatus & 0x07) != FR9009_ARM_RADIO_STATUS_READY)) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
						  FR_ERR_SETORXLOCFG_INVALID_ARMSTATE, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Disable/enable Auto Relocking AuxPLL in ARM */
	/* Invert value before writing to ARM memory */
	enableRelock = (orxLoCfg->disableAuxPllRelocking == 0) ? 1 : 0;
	byteOffset = 0x1C;
	retVal = (talRecoveryActions_t)FR9009_writeArmConfig(device,
			FR9009_ARM_OBJECTID_SYTEM_INFO,
			byteOffset, &enableRelock, 1);
	IF_ERR_RETURN_U32(retVal);

	/* Set Stream GPIO3 pin select and enable the GPIO pin to be able to create
	 * an interrupt to trigger the stream */
	halError = brSpiWriteField(device->devHalInfo, FR9009_ADDR_STREAM_CONTROL,
				    gpio3StreamMaskValue, 0x80, 7);
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = brSpiWriteField(device->devHalInfo,
				    FR9009_ADDR_STREAM_GPIO_TRIGGER_PIN_SELECT_BYTE1, gpioSel, 0xF0, 4);
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	/* Update used GPIO pins in device data structure */
	device->devStateInfo.usedGpiopins &= ~freeGpioPins;
	device->devStateInfo.usedGpiopins |= usedGpioPins;

	return (uint32_t)retVal;
}

uint32_t FR9009_getOrxLoCfg(fr9009Device_t *device, fr9009OrxLoCfg_t *orxLoCfg)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	fRHALErr_t halError = FRHAL_OK;
	uint32_t radioStatus = 0;
	uint16_t byteOffset = 0;
	uint8_t enableRelock = 0;
	uint8_t gpio3StreamMaskValue = 0;
	uint8_t gpioSel = 0;

#if FR9009_VERBOSE
	halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK,
				 "FR9009_getOrxLoCfg()\n");
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* Perform input paramter NULL and range checks */
	if (orxLoCfg == NULL) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
						  FR_ERR_GETORXLOCFG_NULL_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Verify ARM in radio Off state */
	retVal = (talRecoveryActions_t)FR9009_getRadioState(device, &radioStatus);
	IF_ERR_RETURN_U32(retVal);

	if (((radioStatus & 0x07) != FR9009_ARM_RADIO_STATUS_IDLE) &&
	    ((radioStatus & 0x07) != FR9009_ARM_RADIO_STATUS_READY)) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
						  FR_ERR_GETORXLOCFG_INVALID_ARMSTATE, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* Read back if ARM is auto relocking AuxPll */
	byteOffset = 0x1C;
	retVal = (talRecoveryActions_t)FR9009_readArmConfig(device,
			FR9009_ARM_OBJECTID_SYTEM_INFO, byteOffset, &enableRelock, 1);
	IF_ERR_RETURN_U32(retVal);

	/* Invert relock value */
	orxLoCfg->disableAuxPllRelocking = (enableRelock == 1) ? 0 : 1;

	/* Read back stream GPIO3 pin setup */
	halError = brSpiReadField(device->devHalInfo, FR9009_ADDR_STREAM_CONTROL,
				   &gpio3StreamMaskValue, 0x80, 7);
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	halError = brSpiReadField(device->devHalInfo,
				   FR9009_ADDR_STREAM_GPIO_TRIGGER_PIN_SELECT_BYTE1, &gpioSel, 0xF0, 4);
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal,
				  TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);

	if (gpio3StreamMaskValue == 1) {
		orxLoCfg->gpioSelect = FR_GPIO_INVALID;
	} else {
		orxLoCfg->gpioSelect = (fr9009GpioPinSel_t)gpioSel;
	}

	return (uint32_t)retVal;
}
#endif

uint32_t FR9009_radioOn(fr9009Device_t *device)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;

	//select radio param to enable
	static const uint8_t rx1_rf_en = 1;     //0bit
	static const uint8_t rx2_rf_en = 1;     //1bit
	static const uint8_t tx1_rf_en = 1;     //2bit
	static const uint8_t tx2_rf_en = 1;     //3bit
	static const uint8_t orx1_rf_en = 1;    //4bit
	static const uint8_t orx2_rf_en = 1;    //5bit

	uint32_t enable_sel = 0;

	enable_sel = ((rx1_rf_en & 0x1) |
		       ((rx2_rf_en & 0x1) << 1) |
		       ((tx1_rf_en & 0x1) << 2) |
		       ((tx2_rf_en & 0x1) << 3) |
		      ((orx1_rf_en & 0x1) << 4) |
		      ((orx2_rf_en & 0x1) << 5));
	//retVal = FR9009_armSpiCmd_writeField(device, FR9009_ADDR_TDD_RF_CONFIG0, 0x0201330F, 0x0231773F, 0);
	retVal = FR9009_armSpiCmd_writeField(device, FR9009_ADDR_TDD_RF_CONFIG0, enable_sel, 0x3F, 0);
	IF_ERR_RETURN_U32(retVal);
	FR9009_armSpiCmd_TddManualsel(device, 1);
	device->devStateInfo.devState = (fr9009States_t)(device->devStateInfo.devState | FR_STATE_RADIOON);
	return (uint32_t)retVal;
}

uint32_t FR9009_radioOff(fr9009Device_t *device)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;

	retVal = FR9009_armSpiCmd_writeField(device, FR9009_ADDR_TDD_RF_CONFIG0, 0x00000000, 0x3F, 0);
	IF_ERR_RETURN_U32(retVal); 
	device->devStateInfo.devState = (fr9009States_t)(device->devStateInfo.devState & ~FR_STATE_RADIOON); 
	return (uint32_t)retVal;
}

uint32_t FR9009_getRadioState(fr9009Device_t *device, uint32_t *radioStatus)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	fRHALErr_t halError = FRHAL_OK;
	uint32_t status = 0;

#if FR9009_VERBOSE
	halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK, "FR9009_getRadioState()\n");
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal, TALACT_WARN_RESET_LOG);
#endif
	/* execute only if radioStatus pointer is valid */
	if (radioStatus != NULL) {
		retVal = FR9009_armSpiCmd_readWords(device, FR9009_ADDR_TDD_RF_CONFIG0, &status, 1);
		IF_ERR_RETURN_U32(retVal);

		*radioStatus = (uint32_t)status&0x00000FFF;
	} else {
		/* invalid radioStatus pointer */
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
						  FR_ERR_GETRADIOSTATE_NULL_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	return (uint32_t)retVal;
}

uint32_t FR9009_TxBandSet(fr9009Device_t *device, fr9009TxChannels_t txChannel)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint8_t setval = 0;
	uint32_t DataPathRate_Khz = device->devStateInfo.txInputRate_kHz;
	if(device->devStateInfo.txIfConvMode[0] == FR_IFCONV_INT2){
		DataPathRate_Khz *= 2;
	}
	if(device->devStateInfo.txIfConvMode[0] == FR_IFCONV_DEC2){
		DataPathRate_Khz /= 2;
	}
	if((DataPathRate_Khz >= (device->devStateInfo.clocks.deviceClock_kHz * 4)) || (DataPathRate_Khz > 250)){
		setval = 0;	//491.52
	}else if((DataPathRate_Khz >= (device->devStateInfo.clocks.deviceClock_kHz * 2)) || (DataPathRate_Khz > 125)){
		setval = 1;	//245.76
	}else{
		setval = 2;
	}
	retVal = FR9009_armSpiCmd_tx_bw_set(device,txChannel, setval);    //set txchannel bw  1:窄带
	IF_ERR_RETURN_U32(retVal);
	return retVal;
}

uint32_t FR9009_RXBandSet(fr9009Device_t *device)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint8_t setval = 0;
	uint32_t DataPathRate_Khz = device->devStateInfo.rxOutputRate_kHz;
	if(device->devStateInfo.rxIfConvMode[0] == FR_IFCONV_INT2){
		DataPathRate_Khz /= 2;
	}
	if(device->devStateInfo.rxIfConvMode[0] == FR_IFCONV_DEC2){
		DataPathRate_Khz *= 2;
	}
	if((DataPathRate_Khz >= (device->devStateInfo.clocks.deviceClock_kHz * 4)) || (DataPathRate_Khz > 250)){
		setval = 2;	//491.52
	}else if((DataPathRate_Khz >= (device->devStateInfo.clocks.deviceClock_kHz * 2)) || (DataPathRate_Khz > 125)){
		setval = 0;	//245.76
	}else{
		setval = 1;
	}
	retVal = FR9009_armSpiCmd_rx_bw_set(device, CHANNEL_1_2, setval);   //1:窄带
	IF_ERR_RETURN_U32(retVal);
	return retVal;
}

uint32_t FR9009_ORXBandSet(fr9009Device_t *device)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint8_t setval = 0;
	uint32_t DataPathRate_Khz = device->devStateInfo.orxOutputRate_kHz;
	if(device->devStateInfo.orxIfConvMode[0] == FR_IFCONV_INT2){
		DataPathRate_Khz /= 2;
	}
	if(device->devStateInfo.orxIfConvMode[0] == FR_IFCONV_DEC2){
		DataPathRate_Khz *= 2;
	}
	if((DataPathRate_Khz  >= (device->devStateInfo.clocks.deviceClock_kHz * 4)) || (DataPathRate_Khz > 250)){
		setval = 0;	//491.52
	}else if((DataPathRate_Khz >= (device->devStateInfo.clocks.deviceClock_kHz * 2)) || (DataPathRate_Khz > 125)){
		setval = 1;	//245.76
	}else{
		setval = 2;
	}
	retVal = FR9009_armSpiCmd_orx_bw_set(device, CHANNEL_1_2, setval);
	IF_ERR_RETURN_U32(retVal);
	return retVal;
}

uint32_t FR9009_setRxTxEnable(fr9009Device_t *device,
			      fr9009RxORxChannels_t rxOrxChannel, fr9009TxChannels_t txChannel)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	fRHALErr_t halError = FRHAL_OK;

#if FR9009_VERBOSE
	halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK,
				 "FR9009_setRxTxEnable()\n");
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	/* Only allow valid Rx / ORx cases, return error else */
	switch (rxOrxChannel) {
	case FR_RXOFF_EN:  /* Fall through to next case */
	case FR_RX1_EN:    /* Fall through to next case */
	case FR_RX2_EN:    /* Fall through to next case */
	case FR_RX1RX2_EN: /* Fall through to next case */
	case FR_ORX1_EN:   /* Fall through to next case */
	case FR_ORX2_EN:   /* Fall through to next case */
	case FR_ORX1ORX2_EN:
	case FR_RX1_ORX2_EN:
	case FR_RX2_ORX1_EN:
		//	if(rxOrxChannel & FR_RX1RX2_EN){
		//		retVal = FR9009_RXBandSet(device);
		//		IF_ERR_RETURN_U32(retVal);
		//	}			
		//	if(rxOrxChannel & FR_ORX1ORX2_EN){
		//		retVal = FR9009_ORXBandSet(device);
		//		IF_ERR_RETURN_U32(retVal);
		//	}
		retVal = FR9009_armSpiCmd_orxrx_ch_en(device, rxOrxChannel);       //ORX通道使能
		IF_ERR_RETURN_U32(retVal);
		break;
	default:
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
						  FR_ERR_BBIC_INV_CHN, retVal, TALACT_ERR_CHECK_PARAM);
	}

	switch (txChannel) {
	case FR_TXOFF: /* Fall through to next case */
	case FR_TX1:   /* Fall through to next case */
	case FR_TX2:   /* Fall through to next case */
	case FR_TX1TX2:
		//	chEnable = (uint8_t)((txChannel) | rxOrxChannel);
		//	if(txChannel & FR_TX1TX2){
		//		retVal = FR9009_TxBandSet(device, txChannel);
		//		IF_ERR_RETURN_U32(retVal);
		//	}	
		retVal = FR9009_armSpiCmd_tx_ch_en(device, txChannel);      //TX通道使能
		IF_ERR_RETURN_U32(retVal);
		break;
	default:
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
						  FR_ERR_BBIC_INV_CHN, retVal, TALACT_ERR_CHECK_PARAM);
	}
	return (uint32_t)retVal;
}

uint32_t FR9009_getRxTxEnable(fr9009Device_t *device,
			      fr9009RxORxChannels_t *rxOrxChannel, fr9009TxChannels_t *txChannel)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	fRHALErr_t halError = FRHAL_OK;
	uint32_t chEnable = 0;

#if FR9009_VERBOSE
	halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK,
				 "FR9009_getRxTxEnable()\n");
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	if ((txChannel == NULL) ||
	    (rxOrxChannel == NULL)) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
						  FR_ERR_GETRXTXENABLE_NULLPARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}

	retVal = FR9009_armSpiCmd_readWords(device, FR9009_ADDR_TDD_RF_CONFIG0, &chEnable, 1);
	IF_ERR_RETURN_U32(retVal);

	*txChannel = (fr9009TxChannels_t)((chEnable>>2) & 0x3);
	*rxOrxChannel = (fr9009RxORxChannels_t)((chEnable & 0x3) | ((chEnable>>2) & 0xC));

	return (uint32_t)retVal;
}

uint32_t FR9009_setTxToOrxMapping(fr9009Device_t *device, uint8_t txCalEnable,
				  fr9009TxToOrxMapping_t oRx1Map, fr9009TxToOrxMapping_t oRx2Map)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	talRecoveryActions_t retValWarn = TALACT_NO_ACTION;
	fRHALErr_t halError = FRHAL_OK;

#if FR9009_VERBOSE
	halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK,
				 "FR9009_setTxToOrxMapping()\n");
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	retValWarn = retVal;

	/* cmdByte assignment from oRx1Map */
	switch (oRx1Map) {
	case FR_MAP_NONE:
		break;
	case FR_MAP_TX1_ORX:
		break;
	case FR_MAP_TX2_ORX:
		break;
	default:
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
						  FR_ERR_SETTXTOORXMAP_INV_ORX1_MAP, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* cmdByte assignment from oRx2Map */
	switch (oRx2Map) {
	case FR_MAP_NONE:
		break;
	case FR_MAP_TX1_ORX:
		break;
	case FR_MAP_TX2_ORX:
		break;
	default:
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
						  FR_ERR_SETTXTOORXMAP_INV_ORX2_MAP, retVal, TALACT_ERR_CHECK_PARAM);
	}

	/* adding the txCalEnable bit */

	/* write new setting to the command register */
	if(oRx1Map){
		retVal = FR9009_armSpiCmd_setup_tx_loopback(device, oRx1Map, CHANNEL_1, 1, TX_LOOP2ORX_MODE);//外部回环。loopback2 orx
		IF_ERR_RETURN_U32(retVal);
		/* adding the txCalEnable bit */
		if (txCalEnable > 0) {
			retVal = FR9009_armSpiCmd_tx_lo_leak_cali(device, oRx1Map);
			IF_ERR_RETURN_U32(retVal);
		}
	}else if(oRx2Map){
		retVal = FR9009_armSpiCmd_setup_tx_loopback(device, oRx2Map, CHANNEL_2, 1, TX_LOOP2ORX_MODE);//外部回环。loopback2 orx
		IF_ERR_RETURN_U32(retVal);
		/* adding the txCalEnable bit */
		if (txCalEnable > 0) {
			retVal = FR9009_armSpiCmd_tx_lo_leak_cali(device, oRx2Map);
			IF_ERR_RETURN_U32(retVal);
		}
	}

	/* If higher priority retVal has no error, allow possible lower priority warning to be returned */
	if (retVal == TALACT_NO_ACTION) {
		retVal = retValWarn;
	}

	return (uint32_t)retVal;
}

uint32_t  FR9009_setRfPllFrequency(fr9009Device_t *device, fr9009RfPllName_t pllName, uint64_t rfPllLoFrequency_Hz) 
{
    talRecoveryActions_t retVal = TALACT_NO_ACTION;
    fRHALErr_t halError = FRHAL_OK;
#if FR9009_VERBOSE
    halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK, "FR9009_setRfPllFrequency()\n");
    retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal, TALACT_WARN_RESET_LOG);
#endif
	    /* selecting the PLL of interest */
    switch (pllName)
    {
        case FR_RF_PLL:
        case FR_ORF_PLL:
            retVal = FR9009_armSpiCmd_pll_freq_set(device, pllName, rfPllLoFrequency_Hz);
        	IF_ERR_RETURN_U32(retVal);
			if((device->devStateInfo.devState & FR_STATE_CALS_RUN) && (device->devStateInfo.predict_cali != 0)){
				retVal = FR9009_armSpiCmd_pre_cali_load(device, pllName, rfPllLoFrequency_Hz);
        		IF_ERR_RETURN_U32(retVal);
			}
            break;
        default:
            return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
                    FR_ERR_SETRFPLL_INV_PLLNAME, retVal, TALACT_ERR_CHECK_PARAM);
    }
    return (uint32_t)retVal;
}

uint32_t  FR9009_getRfPllFrequency (fr9009Device_t *device, fr9009RfPllName_t pllName, uint64_t *rfPllLoFrequency_Hz) 
{
    talRecoveryActions_t retVal = TALACT_NO_ACTION;
    fRHALErr_t halError = FRHAL_OK;
#if FR9009_VERBOSE
    halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK, "FR9009_getRfPllFrequency()\n");
    retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal, TALACT_WARN_RESET_LOG);
#endif
	    /* selecting the PLL of interest */
    switch (pllName)
    {
        case FR_RF_PLL:
        case FR_ORF_PLL:
            retVal = FR9009_armSpiCmd_pll_freq_get(device, pllName, rfPllLoFrequency_Hz);
        	IF_ERR_RETURN_U32(retVal);
            break;
        default:
            return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
                    FR_ERR_SETRFPLL_INV_PLLNAME, retVal, TALACT_ERR_CHECK_PARAM);
    }

    return (uint32_t)retVal;
}


uint32_t  FR9009_getPllsLockStatus (fr9009Device_t *device, uint8_t *pllLockStatus) 
{
    talRecoveryActions_t retVal = TALACT_NO_ACTION;
    fRHALErr_t halError = FRHAL_OK;
    uint32_t readData = 0;
	uint32_t rbuf[5];

#if FR9009_VERBOSE
    halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK, "FR9009_getPllsLockStatus()\n");
    retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal, TALACT_WARN_RESET_LOG);
#endif

    /* null pointer check */
    if (pllLockStatus == NULL)
    {
        return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
                FR_ERR_CHECKPLLLOCK_NULL_PARM, retVal, TALACT_ERR_CHECK_PARAM);
    }
    else
    {
        /* clear the pointer contents */
        *pllLockStatus = 0;
		retVal = FR9009_armSpiCmd_readWords(device, FR9009_ADDR_APB_CLK_RST_BB_PLL, rbuf, 5);
        IF_ERR_RETURN_U32(retVal);
		//*pllLockStatus = ((rbuf[0]>>28)&0x1)|((rbuf[1]>>8)&0x2)|((rbuf[3]>>7)&0x4);
		readData = (rbuf[0]>>28)&0x1;
        *pllLockStatus = readData;
		readData = (rbuf[1]>>9)&0x1;
        *pllLockStatus = *pllLockStatus | (readData << 1);
		readData = (rbuf[3]>>9)&0x1;
        *pllLockStatus = *pllLockStatus | (readData << 2);
    }

    return (uint32_t)retVal;
}
#if 0
uint32_t  FR9009_setRfPllLoopFilter (fr9009Device_t *device, uint16_t loopBandwidth_kHz, uint8_t stability) 
{
    talRecoveryActions_t retVal = TALACT_NO_ACTION;
    talRecoveryActions_t retValWarn = TALACT_NO_ACTION;

#if FR9009_VERBOSE
    fRHALErr_t halError = FRHAL_OK;
    halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK, "FR9009_setRfPllLoopFilter()\n");
    retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal, TALACT_WARN_RESET_LOG);
#endif

    retValWarn = retVal;

    retVal = (talRecoveryActions_t)FR9009_setPllLoopFilter(device, FR_RF_PLL, loopBandwidth_kHz, stability);
    IF_ERR_RETURN_U32(retVal);

    /* If higher priority retVal has no error, allow possible lower priority warning to be returned */
    if (retVal == TALACT_NO_ACTION)
    {
        retVal = retValWarn;
    }

    return (uint32_t)retVal;
}


uint32_t  FR9009_getRfPllLoopFilter (fr9009Device_t *device, uint16_t *loopBandwidth_kHz, uint8_t *stability) 
{
    talRecoveryActions_t retVal = TALACT_NO_ACTION;
    talRecoveryActions_t retValWarn = TALACT_NO_ACTION;


#if FR9009_VERBOSE
    fRHALErr_t halError = FRHAL_OK;
    halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK, "FR9009_getRfPllLoopFilter()\n");
    retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal, TALACT_WARN_RESET_LOG);
#endif

    retValWarn = retVal;

    retVal = (talRecoveryActions_t)FR9009_getPllLoopFilter(device, FR_RF_PLL, loopBandwidth_kHz, stability);
    IF_ERR_RETURN_U32(retVal);

    if (retVal == TALACT_NO_ACTION)
    {
        retVal = retValWarn;
    }

    return (uint32_t)retVal;
}


uint32_t  FR9009_setPllLoopFilter (fr9009Device_t *device, fr9009RfPllName_t pllName, uint16_t loopBandwidth_kHz, uint8_t stability) 
{
    talRecoveryActions_t retVal = TALACT_NO_ACTION;
    talRecoveryActions_t retValWarn = TALACT_NO_ACTION;
	uint16_t band = 0;
#if FR9009_VERBOSE
    fRHALErr_t halError = FRHAL_OK;
	halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK, "FR9009_setPllLoopFilter()\n");
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal, TALACT_WARN_RESET_LOG);
#endif
    /* Range check for loopBandwidth - Should be between 50kHz and 500000kHz */
    if ((loopBandwidth_kHz < 50) ||
        (loopBandwidth_kHz > 500000))
    {
        return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
                FR_ERR_SETRFPLL_LOOPFILTER_INV_LOOPBANDWIDTH, retVal, TALACT_ERR_CHECK_PARAM);
    }
		    /* Range check for stability - Should be between 3 and 15 */
    if ((stability < 3) ||
        (stability > 15))
    {
        return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
                FR_ERR_SETRFPLL_LOOPFILTER_INV_STABILITY, retVal, TALACT_ERR_CHECK_PARAM);
    }
	if(loopBandwidth_kHz>125000)
		band = 1;
	
	    /* selecting the PLL of interest */
    switch (pllName)
    {
        case FR_RF_PLL:
			//////////////////////////////////////////////////////////////TODO
            break;
        case FR_ORF_PLL:
			//////////////////////////////////////////////////////////////TODO
            break;
        default:
            return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
                    FR_ERR_SETRFPLL_LOOPFILTER_INV_PLLSEL, retVal, TALACT_ERR_CHECK_PARAM);
    }
    /* If higher priority retVal has no error, allow possible lower priority warning to be returned */
    if (retVal == TALACT_NO_ACTION)
    {
        retVal = retValWarn;
    }

    return (uint32_t)retVal;
}


uint32_t  FR9009_getPllLoopFilter (fr9009Device_t *device, fr9009RfPllName_t pllName, uint16_t *loopBandwidth_kHz, uint8_t *stability) 
{
	return 123;
}
#endif

uint32_t  FR9009_setOrxLoSource(fr9009Device_t *device, fr9009LoSource_t orxLoSource) 
{
	
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	fRHALErr_t halError = FRHAL_OK;

#if FR9009_VERBOSE
	halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK,
				 "FR9009_setOrxLoSource()\n");
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal,
				  TALACT_WARN_RESET_LOG);
#endif

	if ((orxLoSource != FR_LO_RF_PLL) && (orxLoSource != FR_LO_AUX_PLL)){
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
						  FR_ERR_SETORXLOSRC_INVALIDPARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}
	retVal = FR9009_armSpiCmd_orx_Lo_set(device, FR_ORX1ORX2, orxLoSource == FR_LO_RF_PLL ? FR_LO_RF_PLL : FR_LO_AUX_PLL);
	IF_ERR_RETURN_U32(retVal);
	return (uint32_t)retVal;

}


uint32_t  FR9009_getOrxLoSource(fr9009Device_t *device, fr9009LoSource_t *orx1LoSource, fr9009LoSource_t *orx2LoSource) 
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	fRHALErr_t halError = FRHAL_OK;
	fr9009LoSource_t losel = FR_LO_RF_PLL;
#if FR9009_VERBOSE
	halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK, "FR9009_getOrxLoSource()\n");
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal, TALACT_WARN_RESET_LOG);
#endif

	/* Check for NULL */
	if ((orx1LoSource == NULL) || (orx2LoSource == NULL)) {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
						  FR_ERR_GETORXLOSRC_NULLPARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}
	retVal = FR9009_armSpiCmd_orx_Lo_get(device, FR_ORX1, &losel);
	IF_ERR_RETURN_U32(retVal);
	*orx1LoSource = losel == FR_LO_RF_PLL? FR_LO_RF_PLL : FR_LO_AUX_PLL;
	retVal = FR9009_armSpiCmd_orx_Lo_get(device, FR_ORX2, &losel);
	IF_ERR_RETURN_U32(retVal);
	*orx2LoSource = losel == FR_LO_RF_PLL? FR_LO_RF_PLL : FR_LO_AUX_PLL;
	return (uint32_t)retVal;
}

static inline uint8_t __bit2num(uint32_t pin)
{
	uint8_t pin_num = 0xff;
	// if(pin == 0){	
	// 	return pin_num;
	// }
	// for(pin_num = 0; pin_num < 32; pin_num++){
	// 	if(pin & (1<<pin_num)){
	// 		break;
	// 	}
	// }
	pin_num = pin;
	return pin_num;
}

uint32_t  FR9009_setFhmConfig (fr9009Device_t *device, fr9009FhmMode_t *fhmConfig) 
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	fRHALErr_t halError = FRHAL_OK;
	uint8_t ch_type = 0;
	uint8_t freq_len = 0;
	uint8_t fhmFreqNums_rf = 0;
	uint8_t fhmFreqNums_orf = 0;
	fr9009FhmTriggerMode_t trigger_mode_rf = 0;
	fr9009FhmTriggerMode_t trigger_mode_orf = 0;
	uint8_t trigger_pin_num_rf = 0;
	uint8_t trigger_pin_num_orf = 0;
	int32_t *fhmFrequencyList_KHz_rf = NULL;
	int32_t *fhmFrequencyList_KHz_orf = NULL;

#if FR9009_VERBOSE
	halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK, "FR9009_setFhmConfig()\n");
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal, TALACT_WARN_RESET_LOG);
#endif
	if(fhmConfig->fhmRf.caliFreqNums && fhmConfig->predict_cali){
		retVal = FR9009_armSpiCmd_precali_freqs_set(device, 0x2, (int32_t *)fhmConfig->fhmRf.calibrationFrequencyList_KHz, fhmConfig->fhmRf.caliFreqNums);
		IF_ERR_RETURN_U32(retVal);
	}
	if(fhmConfig->fhmORf.caliFreqNums  && fhmConfig->predict_cali){
		retVal = FR9009_armSpiCmd_precali_freqs_set(device, 0x4, (int32_t *)fhmConfig->fhmORf.calibrationFrequencyList_KHz, fhmConfig->fhmORf.caliFreqNums);
		IF_ERR_RETURN_U32(retVal);
	}
	if(fhmConfig->fhmEnable == 0){
		return (uint32_t)retVal;		
	}
	if(fhmConfig->fhmEnable){
		// FR9009_armSpiCmd_fasthop_trigger_set(device, 1 << FR_RF_PLL, fhmConfig->fhmRf.fhmTriggerMode, )
		if((fhmConfig->fhmType == FR_FHM_MANUAL_NCO_MODE) || (fhmConfig->fhmType == FR_FHM_AUTO_NCO_MODE)){
			if(fhmConfig->rx_nco.fhmFreqNums){
				ch_type = 0x1;
				freq_len = fhmConfig->rx_nco.fhmFreqNums;
				fhmFreqNums_rf = fhmConfig->rx_nco.fhmFreqNums;
				fhmFrequencyList_KHz_rf = fhmConfig->rx_nco.fhmFrequencyList_KHz;
				trigger_mode_rf = fhmConfig->rx_nco.fhmTriggerMode;
				trigger_pin_num_rf = __bit2num(fhmConfig->rx_nco.fhmGpioPin);
			}		
			if(fhmConfig->tx_nco.fhmFreqNums){
				ch_type |= 0x4;
				freq_len = fhmConfig->tx_nco.fhmFreqNums;
				fhmFreqNums_orf = fhmConfig->tx_nco.fhmFreqNums;
				fhmFrequencyList_KHz_orf = fhmConfig->tx_nco.fhmFrequencyList_KHz;
				trigger_mode_orf = fhmConfig->tx_nco.fhmTriggerMode;
				trigger_pin_num_orf = __bit2num(fhmConfig->tx_nco.fhmGpioPin);
			}
		}else{
			ch_type = 0x0;
			if((fhmConfig->fhmType & FR_FHM_AUTO_ORF_MODE) || (fhmConfig->fhmType & FR_FHM_MANUAL_ORF_MODE)){
				freq_len = fhmConfig->fhmORf.fhmFreqNums;
				fhmFreqNums_orf = fhmConfig->fhmORf.fhmFreqNums;
				fhmFrequencyList_KHz_orf = (int32_t *)fhmConfig->fhmORf.fhmFrequencyList_KHz;
				trigger_mode_orf = fhmConfig->fhmORf.fhmTriggerMode;
				trigger_pin_num_orf = __bit2num(fhmConfig->fhmORf.fhmGpioPin);
			}
			if((fhmConfig->fhmType & FR_FHM_AUTO_RF_MODE) || (fhmConfig->fhmType & FR_FHM_MANUAL_RF_MODE)){			
				freq_len = fhmConfig->fhmRf.fhmFreqNums;
				fhmFreqNums_rf = fhmConfig->fhmRf.fhmFreqNums;
				fhmFrequencyList_KHz_rf = (int32_t *)fhmConfig->fhmRf.fhmFrequencyList_KHz;
				trigger_mode_rf = fhmConfig->fhmRf.fhmTriggerMode;
				trigger_pin_num_rf = __bit2num(fhmConfig->fhmRf.fhmGpioPin);
			}
			if((fhmConfig->fhm_rx_ch == 0xff && (device->devStateInfo.initializedChannels & 0x3)) || (fhmConfig->fhm_rx_ch != 0xff && (fhmConfig->fhm_rx_ch & 0x3))){
				ch_type |= CHANNEL_TYPE_RX;
			}
			if((fhmConfig->fhm_orx_ch == 0xff && (device->devStateInfo.initializedChannels & 0xC)) || (fhmConfig->fhm_orx_ch != 0xff && (fhmConfig->fhm_orx_ch & 0x3))){
				ch_type |= CHANNEL_TYPE_ORX;
			}
			if((fhmConfig->fhm_tx_ch == 0xff && (device->devStateInfo.initializedChannels & 0x3)) || (fhmConfig->fhm_tx_ch != 0xff && (fhmConfig->fhm_tx_ch & 0x3))){
				ch_type |= CHANNEL_TYPE_TX;
			}
		}
		if(trigger_pin_num_rf == 0xFF || trigger_pin_num_orf == 0xFF){
			return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_SETFHMMODE_NULL_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
		}
		retVal = FR9009_armSpiCmd_fasthop_init(device, fhmConfig->fhmType, ch_type, ch_type == 0 ? 0 : 0x3, fhmConfig->fhmSwitchFreq_Hz, freq_len, 0,0);
		IF_ERR_RETURN_U32(retVal);
		if(fhmFreqNums_rf && fhmFrequencyList_KHz_rf){
			FR9009_armSpiCmd_fasthop_trigger_set(device, 1 << FR_RF_PLL, trigger_mode_rf, trigger_pin_num_rf);
			retVal = FR9009_armSpiCmd_fasthop_freqs_set(device, 0x2, fhmFrequencyList_KHz_rf, fhmFreqNums_rf);
			IF_ERR_RETURN_U32(retVal);
		}
		if(fhmFreqNums_orf && fhmFrequencyList_KHz_orf){
			FR9009_armSpiCmd_fasthop_trigger_set(device, 1 << FR_ORF_PLL, trigger_mode_orf, trigger_pin_num_orf);
			retVal = FR9009_armSpiCmd_fasthop_freqs_set(device, 0x4, fhmFrequencyList_KHz_orf, fhmFreqNums_orf);
			IF_ERR_RETURN_U32(retVal);
		}
		if(fhmConfig->fhm_rx_ch == 0xff){
			fhmConfig->fhm_rx_ch = device->devStateInfo.initializedChannels & 0x3;
		}		
		if(fhmConfig->fhm_orx_ch == 0xff){
			fhmConfig->fhm_orx_ch = (device->devStateInfo.initializedChannels >> 2) & 0x3;
		} 
		if(fhmConfig->fhm_tx_ch == 0xff){
			fhmConfig->fhm_tx_ch = (device->devStateInfo.initializedChannels >> 4) & 0x3;
		}
		retVal = FR9009_armSpiCmd_fasthop_ch_set(device, (CHANNEL_t)fhmConfig->fhm_rx_ch, (CHANNEL_t)fhmConfig->fhm_orx_ch, (CHANNEL_t)fhmConfig->fhm_tx_ch);
		IF_ERR_RETURN_U32(retVal);
	}

	return (uint32_t)retVal;
}

uint32_t  FR9009_FhmStart(fr9009Device_t *device, uint8_t start_en)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	fRHALErr_t halError = FRHAL_OK;
#if FR9009_VERBOSE
	halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK, "FR9009_setFhmConfig()\n");
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal, TALACT_WARN_RESET_LOG);
#endif
	if(device->devStateInfo.initializedChannels & 0x3){//rx ch Enable，fhm used orx cfir
		FR9009_armSpiCmd_writeField(device, APB_TDD_BASEADDR + 0x8, 0, 3ul << 18, 0); //跳频模式使用rxmap
	}
	retVal = FR9009_armSpiCmd_fasthop_start(device, start_en);
	return (uint32_t)retVal;
}

uint32_t  FR9009_getFhmRfPllFrequency (fr9009Device_t *device, uint64_t *fhmRfPllFrequency_Hz) 
{	
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	fRHALErr_t halError = FRHAL_OK;
#if FR9009_VERBOSE
	halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK, "FR9009_setFhmConfig()\n");
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal, TALACT_WARN_RESET_LOG);
#endif
	retVal = FR9009_armSpiCmd_fasthop_current_freq_get(device, fhmRfPllFrequency_Hz);
	IF_ERR_RETURN_U32(retVal);
	return (uint32_t)retVal;
}
#if 0
uint32_t  FR9009_getFhmConfig (fr9009Device_t *device, fr9009FhmConfig_t *fhmConfig) 
{
	return 123;
}


uint32_t  FR9009_setFhmMode (fr9009Device_t *device, fr9009FhmMode_t *fhmMode) 
{
	return 123;
}


uint32_t  FR9009_getFhmMode (fr9009Device_t *device, fr9009FhmMode_t *fhmMode) 
{
	return 123;
}
#endif

uint32_t  FR9009_setFhmHop (fr9009Device_t *device, uint64_t nextRfPllFrequency_Hz) 
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	fRHALErr_t halError = FRHAL_OK;
#if FR9009_VERBOSE
	halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK, "FR9009_setFhmConfig()\n");
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal, TALACT_WARN_RESET_LOG);
#endif
	retVal = FR9009_armSpiCmd_fasthop_next_freq_set(device, 0x2, nextRfPllFrequency_Hz);// rf pll next freq set
	IF_ERR_RETURN_U32(retVal);
	return (uint32_t)retVal;
}

uint32_t FR9009_setFhmNextFreq(fr9009Device_t *device, fr9009RfPllName_t pllName, uint64_t nextRfPllFrequency_Hz)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	fRHALErr_t halError = FRHAL_OK;
	uint32_t freq_hz_div4 = nextRfPllFrequency_Hz >> 2;
	uint32_t addr = pllName == FR_RF_PLL ? 0x44000060 : 0x44000064;
	uint32_t direct_set = 0;//0x80000000;
	if((pllName != FR_RF_PLL) && (pllName != FR_ORF_PLL)){
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_INIT_NULLPARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}
	retVal = FR9009_armSpiCmd_writeWords(device, addr, &freq_hz_div4, 1);
	IF_ERR_RETURN_U32(retVal);
	return (uint32_t)retVal;	
}
uint32_t FR9009_setExtLoCfg(fr9009Device_t *device, uint8_t enableExtLo, fr9009ExtLoDiv_t extLoDivide) 
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	fRHALErr_t halError = FRHAL_OK;
	uint32_t data = enableExtLo ? ((enableExtLo & 0x10) ? 0x0002D000 : 0x0002C800) : 0x00000800;
#if FR9009_VERBOSE
	halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK, "FR9009_setFhmConfig()\n");
	retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal, TALACT_WARN_RESET_LOG);
#endif
	retVal = FR9009_armSpiCmd_writeField(device, FR9009_ADDR_ARMSPI(SPI_RFPLL_ID, 0x1C), data, 0x0002F800, 0);
	IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_armSpiCmd_writeField(device, FR9009_ADDR_ARMSPI(SPI_FAFC_RF_ID, 0x80), extLoDivide, 0x00000070, 4);
	IF_ERR_RETURN_U32(retVal);
	return (uint32_t)retVal;
}

#if 0
uint32_t  FR9009_getFhmStatus (fr9009Device_t *device, fr9009FhmStatus_t *fhmStatus) 
{
	return 123;
}
uint32_t  FR9009_setExtLoOutCfg (fr9009Device_t *device, uint8_t enableExtLoOutput, fr9009ExtLoDiv_t extLoOutDivide) 
{
	return 123;
}


uint32_t  FR9009_getExtLoOutCfg (fr9009Device_t *device, uint8_t *enableExtLoOutput, fr9009ExtLoDiv_t *extLoOutDivide) 
{
	return 123;
}

#endif

