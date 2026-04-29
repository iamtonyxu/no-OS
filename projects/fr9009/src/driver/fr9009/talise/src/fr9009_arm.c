/**
 * \file fr9009_arm.c
 * \brief Contains functions to support interfacing with the FR9009 internal
 *          ARM processor
 *
 * Copyright 2022 BriRadio.
 * Released under the FR9009 API license, for more information see the "LICENSE.txt" file in this zip file.
 */

#include "fr9009_arm.h"
#include "fr9009_radioctrl.h"
#include "fr9009_reg_addr_macros.h"
#include "fr9009_arm_macros.h"
#include "fr9009_hal.h"
#include "fr9009_user.h"
#include "fr9009_error.h"
#include "fr9009_cals.h"
#include "fr9009.h"

uint32_t FR9009_initArmclk(fr9009Device_t *device, fr9009Init_t *init)
{
#define ARM_CLK_MAX_FREQ_KHZ_MULT(n)    (200000 * n)
    talRecoveryActions_t retVal = TALACT_NO_ACTION;
    fRHALErr_t halError = FRHAL_OK;
    uint32_t regdat = 0;
    uint32_t armClkRate_kHz = 0;
    // uint32_t hsDigClkDiv4or5_Hz = 0;
	uint8_t refarm_div = 0; /** 0:div1  1:div2  2:div4  3:div6  4;div8  5:div10 6,7:div12*/
	uint8_t armClk_div =  2;/** 0:div4  1:div6  2:div8  3:div10  4;div12  5:div14 6；div16 7:div18*/
#if FR9009_VERBOSE
    halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK, "FR9009_initArm()\n");
    retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal, TALACT_WARN_RESET_LOG);
#endif
	if (init->clocks.deviceClock_kHz > 10000 && init->clocks.deviceClock_kHz <= 1000000) {
		regdat = init->clocks.deviceClock_kHz*1000;
		retVal = brSpiBlockWriteWords(device->devHalInfo, FR9009_ADDR_REFCLK_FREQ,  &regdat, 1);
		IF_ERR_RETURN_U32(retVal);
	} else {
		return (uint32_t)talApiErrHandler(device,FR_ERRHDL_INVALID_PARAM, FR_ERR_INITARM_INV_ARMCLK_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}
	/* dividing from Hz to kHz */
	armClkRate_kHz =  init->clocks.deviceClock_kHz;
	/* the SPI read register and write register clocks must be equal or less than 250MHz */
	/* the ARM clock should not exceed 250 Mhz */
	if (armClkRate_kHz <= ARM_CLK_MAX_FREQ_KHZ_MULT(1)) {
		refarm_div = 0;
	} else if (armClkRate_kHz <= ARM_CLK_MAX_FREQ_KHZ_MULT(2)) {
		refarm_div = 0x01;
	} else if (armClkRate_kHz <= ARM_CLK_MAX_FREQ_KHZ_MULT(4)) {
		refarm_div = 0x02;
	} else if (armClkRate_kHz <= ARM_CLK_MAX_FREQ_KHZ_MULT(5)) {
		refarm_div = 0x04;
	} else {
		return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_INITARM_INV_ARMCLK_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
	}
    regdat = refarm_div | (armClk_div << 4) | (0 << 8);//8 bit选择refclk 分频器输出
    retVal = brSpiBlockWriteWords(device->devHalInfo, FR9009_ADDR_ARMCLK_DIV,  &regdat, 1);
    IF_ERR_RETURN_U32(retVal);
    regdat = 0;
    retVal = brSpiBlockWriteWords(device->devHalInfo, FR9009_ADDR_ARMCLK_DIV_ACTION,  &regdat, 1);
    IF_ERR_RETURN_U32(retVal);
    regdat = 1 | (0 << 1) | (0 << 2);//1 bit选择refclk 分频器生效
    retVal = brSpiBlockWriteWords(device->devHalInfo, FR9009_ADDR_ARMCLK_DIV_ACTION,  &regdat, 1);
    IF_ERR_RETURN_U32(retVal);
    regdat = 0 | (0 << 1) | (0 << 2);//1 bit选择refclk 分频器生效
    retVal = brSpiBlockWriteWords(device->devHalInfo, FR9009_ADDR_ARMCLK_DIV_ACTION,  &regdat, 1);
    IF_ERR_RETURN_U32(retVal);	
	return (uint32_t)retVal;
}

uint32_t FR9009_writeArmProfile(fr9009Device_t *device, fr9009Init_t *init)
{
    talRecoveryActions_t retVal = TALACT_NO_ACTION;

    return (uint32_t)retVal;
}

uint32_t FR9009_loadArmFromBinary(fr9009Device_t *device, uint32_t *binary, uint32_t count)
{
    talRecoveryActions_t retVal = TALACT_NO_ACTION;
    fRHALErr_t halError = FRHAL_OK;
    static const uint32_t MAX_BIN_CNT = 0x35000;

#if FR9009_VERBOSE
    halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK, "FR9009_loadArmFromBinary()\n");
    retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal, TALACT_WARN_RESET_LOG);
#endif

    if (binary == NULL)
    {
        return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
                FR_ERR_LOADBIN_NULL_PARAM, retVal, TALACT_ERR_CHECK_PARAM);
    }

    if (count > MAX_BIN_CNT)
    {
        return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM,
                FR_ERR_LOADBIN_INVALID_BYTECOUNT, retVal, TALACT_ERR_CHECK_PARAM);
    }
    else
    {
        /* writing binary data to ARM memory */
        retVal = (talRecoveryActions_t)FR9009_writeArmMem(device, FR9009_ADDR_ARM_START_PROG_ADDR | FR9009_ADDR_EXTDATA_FLAG, &binary[0], count);
        IF_ERR_RETURN_U32(retVal);
    }

    /* verifying ARM checksum */
//    retVal = (talRecoveryActions_t)FR9009_verifyArmChecksum(device);
//    IF_ERR_RETURN_U32(retVal);
    device->devStateInfo.devState = (fr9009States_t)(device->devStateInfo.devState | FR_STATE_ARMLOADED);
    return (uint32_t)retVal;
}

uint32_t FR9009_readArmMem(fr9009Device_t *device, uint32_t address, uint32_t *returnDataWord, uint32_t WordsToRead, uint8_t autoIncrement)
{
    talRecoveryActions_t retVal = TALACT_NO_ACTION;
    fRHALErr_t halError = FRHAL_OK;
#if FR9009_VERBOSE
    halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK, "FR9009_readArmMem()\n");
    retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal, TALACT_WARN_RESET_LOG);
#endif

    /* check that start and stop address are in valid range */
    if (!((address & 0x7FFFFFFF) >= FR9009_ADDR_ARM_START_PROG_ADDR && (address & 0x7FFFFFFF) <= FR9009_ADDR_ARM_END_PROG_ADDR ) &&
        !((address & 0x7FFFFFFF) >= FR9009_ADDR_ARM_START_DATA_ADDR && (address & 0x7FFFFFFF) <= FR9009_ADDR_ARM_END_DATA_ADDR))
    {
        return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_READARMMEM_INV_ADDR_PARM, retVal, TALACT_ERR_CHECK_PARAM);
    }

    if (!(((address & 0x7FFFFFFF) + WordsToRead*4 - 1) >= FR9009_ADDR_ARM_START_PROG_ADDR && ((address & 0x7FFFFFFF) + WordsToRead*4 - 1) <= FR9009_ADDR_ARM_END_PROG_ADDR ) &&
        !(((address & 0x7FFFFFFF) + WordsToRead*4 - 1) >= FR9009_ADDR_ARM_START_DATA_ADDR && ((address & 0x7FFFFFFF) + WordsToRead*4 - 1) <= FR9009_ADDR_ARM_END_DATA_ADDR))
    {
        return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_READARMMEM_INV_ADDR_PARM, retVal, TALACT_ERR_CHECK_PARAM);
    }
    halError = brSpiBlockReadWords(device->devHalInfo, address, returnDataWord, WordsToRead);
    retVal = talApiErrHandler(device,FR_ERRHDL_HAL_SPI, halError, retVal, TALACT_ERR_RESET_SPI);
    IF_ERR_RETURN_U32(retVal);

    return (uint32_t)retVal;
}

uint32_t FR9009_writeArmMem(fr9009Device_t *device, uint32_t address, uint32_t *dataWord, uint32_t WordCount)
{
    talRecoveryActions_t retVal = TALACT_NO_ACTION;
    fRHALErr_t halError = FRHAL_OK;
#if FR9009_VERBOSE
    brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK, "FR9009_writeArmMem()\n");
    retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal, TALACT_WARN_RESET_LOG);
#endif

    if (!((address & 0x7FFFFFFF) >= FR9009_ADDR_ARM_START_PROG_ADDR && (address & 0x7FFFFFFF) <= FR9009_ADDR_ARM_END_PROG_ADDR) &&
        !((address & 0x7FFFFFFF) >= FR9009_ADDR_ARM_START_DATA_ADDR && (address & 0x7FFFFFFF) <= FR9009_ADDR_ARM_END_DATA_ADDR))
    {
        return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_WRITEARMMEM_INV_ADDR_PARM, retVal, TALACT_ERR_CHECK_PARAM);
    }

    if (!(((address & 0x7FFFFFFF) + WordCount*4-1) >= FR9009_ADDR_ARM_START_PROG_ADDR && ((address & 0x7FFFFFFF) + WordCount*4-1) <= FR9009_ADDR_ARM_END_PROG_ADDR ) &&
        !(((address & 0x7FFFFFFF) + WordCount*4-1) >= FR9009_ADDR_ARM_START_DATA_ADDR && ((address & 0x7FFFFFFF) + WordCount*4-1) <= FR9009_ADDR_ARM_END_DATA_ADDR))
    {
        return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_WRITEARMMEM_INV_ADDR_PARM, retVal, TALACT_ERR_CHECK_PARAM);
    }

    /* starting write at zero address offset */
	halError = brSpiBlockWriteWords(device->devHalInfo, address, dataWord, WordCount);
	retVal = talApiErrHandler(device,FR_ERRHDL_HAL_SPI, halError, retVal, TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);
    return (uint32_t)retVal;
}

uint32_t FR9009_writeArmConfig(fr9009Device_t *device, uint8_t objectId, uint16_t offset, uint8_t *data, uint8_t byteCount)
{
    talRecoveryActions_t retVal = TALACT_NO_ACTION;
    return (uint32_t)retVal;
}

uint32_t FR9009_readArmConfig(fr9009Device_t *device, uint8_t objectId, uint16_t offset, uint8_t *data, uint8_t byteCount)
{
    talRecoveryActions_t retVal = TALACT_NO_ACTION;

    return (uint32_t)retVal;
}

uint32_t FR9009_readArmCmdStatus(fr9009Device_t *device, uint32_t *errorWord, uint32_t *statusWord)
{
    talRecoveryActions_t retVal = TALACT_NO_ACTION;

    return (uint32_t)retVal;
}

uint32_t FR9009_readArmCmdStatusWord(fr9009Device_t *device, uint32_t opCode, uint32_t *cmdStatword)
{
    talRecoveryActions_t retVal = TALACT_NO_ACTION;
    fRHALErr_t halError = FRHAL_OK;

    UNUSED_VARIABLE(opCode);

#if FR9009_VERBOSE
    brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK, "FR9009_readArmCmdStatusWord()\n");
    retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal, TALACT_WARN_RESET_LOG);
#endif

    /* reading the command status register for given opcode */
    halError = brSpiReadWord(device->devHalInfo, FR9009_ADDR_WAKE_MCU, cmdStatword);
    retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal, TALACT_ERR_RESET_SPI);
    IF_ERR_RETURN_U32(retVal);
    if((*cmdStatword & FR9009_ADDR_WAKE_MCU_MASK) == 0)
    {
        *cmdStatword = TALAPI_ARMSPI_READY;
    }
    else
    {
        *cmdStatword = TALAPI_ARMSPI_BUSY;
    }
    
    return (uint32_t)retVal;
}

uint32_t FR9009_ReadWait(fr9009Device_t *device, uint32_t readaddr, uint32_t waitval, uint32_t *rd_word, uint32_t timeout_us, uint32_t waitInterval_us)
{
    talRecoveryActions_t retVal = TALACT_NO_ACTION;
    fRHALErr_t halError = FRHAL_OK;
    uint32_t eventCheck = 0;
    uint32_t numEventChecks = 0;

#if FR9009_VERBOSE
    halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK, "FR9009_ReadWait()\n");
    retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal, TALACT_WARN_RESET_LOG);
#endif

    waitInterval_us = (waitInterval_us > timeout_us) ? timeout_us : waitInterval_us;
    numEventChecks = (waitInterval_us == 0) ? 1 : (timeout_us / waitInterval_us);

    /* timeout event check loop */
    for (eventCheck = 0; eventCheck <= numEventChecks; eventCheck++){
        /* read status of readaddr */
        retVal = (talRecoveryActions_t)FR9009_armSpiCmd_readWords(device, readaddr, rd_word, 1);
        IF_ERR_RETURN_U32(retVal);

        if ((*rd_word != waitval) && (eventCheck < numEventChecks)){
            halError = FRHAL_wait_us(device->devHalInfo, waitInterval_us);
            retVal = talApiErrHandler(device, FR_ERRHDL_HAL_WAIT, halError, retVal, TALACT_ERR_CHECK_TIMER);
        }else{
            break;
        }
    }

    if(*rd_word != waitval){
        return (uint32_t)talApiErrHandler(device, FR_ERRHDL_API_FAIL, FR_ERR_WAITARMCMDSTATUS_TIMEOUT, retVal, TALACT_ERR_RESET_ARM);
    }
    return (uint32_t)retVal;
}

uint32_t FR9009_waitArmCmdStatus(fr9009Device_t *device, uint32_t opCode, uint32_t *cmdStatword, uint32_t timeout_us, uint32_t waitInterval_us)
{
    talRecoveryActions_t retVal = TALACT_NO_ACTION;
    fRHALErr_t halError = FRHAL_OK;
    uint32_t eventCheck = 0;
    uint32_t numEventChecks = 0;

    UNUSED_VARIABLE(opCode);

#if FR9009_VERBOSE
    halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK, "FR9009_waitArmCmdStatus()\n");
    retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal, TALACT_WARN_RESET_LOG);
#endif


    waitInterval_us = (waitInterval_us > timeout_us) ? timeout_us : waitInterval_us;
    numEventChecks = (waitInterval_us == 0) ? 1 : (timeout_us / waitInterval_us);

    /* timeout event check loop */
    for (eventCheck = 0; eventCheck <= numEventChecks; eventCheck++)
    {
        /* read status of opcode */
        retVal = (talRecoveryActions_t)FR9009_readArmCmdStatusWord(device, opCode, cmdStatword);
        IF_ERR_RETURN_U32(retVal);


        /* if pending bit is set for opcode of interest and the number of events have not expired, perform wait */
        if (((talApiArmSpiSta)*cmdStatword == TALAPI_ARMSPI_BUSY) &&
              (eventCheck < numEventChecks))
        {
            halError = FRHAL_wait_us(device->devHalInfo, waitInterval_us);
            retVal = talApiErrHandler(device, FR_ERRHDL_HAL_WAIT, halError, retVal, TALACT_ERR_CHECK_TIMER);
        }
        else
        {
            break;
        }
    }

    /* if ARM Command did not complete within the timeout period */
    if ((talApiArmSpiSta)*cmdStatword  == TALAPI_ARMSPI_BUSY) 
    {
        return (uint32_t)talApiErrHandler(device, FR_ERRHDL_API_FAIL, FR_ERR_WAITARMCMDSTATUS_TIMEOUT, retVal, TALACT_ERR_RESET_ARM);
    }
    return (uint32_t)retVal;
}

uint32_t FR9009_sendArmCommand(fr9009Device_t *device, uint32_t opCode, const uint32_t *extendedData, uint32_t extendedDataNumWords)
{
    talRecoveryActions_t retVal = TALACT_NO_ACTION;
    fRHALErr_t halError = FRHAL_OK;

    uint32_t armCommandBuf[FR9009_CMDBUF_WORDSIZE];
    uint32_t armCommandBusy = 1;

    uint32_t timeout_us = SENDARMCMD_TIMEOUT_US;
    uint32_t waitInterval_us = SENDARMCMD_INTERVAL_US;
    uint32_t eventCheck = 0;
    uint32_t numEventChecks = 0;
    uint32_t i = 0;

#if FR9009_VERBOSE
    halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK, "FR9009_sendArmCommand()\n");
    retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal, TALACT_WARN_RESET_LOG);
#endif

    armCommandBuf[1] = opCode;
    /* valid range of opCode and numWords*/
    if ((extendedDataNumWords + 2) > FR9009_CMDBUF_WORDSIZE || opCode == 0 || (extendedDataNumWords > 0 && extendedData == NULL)) 
    {
        return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_ARMCMD_INV_NUMBYTES_PARM, retVal, TALACT_ERR_CHECK_PARAM);
    }

    for(i = 0; i < extendedDataNumWords; i++)
    {
        armCommandBuf[i+2] = extendedData[i];
    }

    /* setting a 2 second timeout for mailbox busy bit to be clear (can't send an arm mailbox command until mailbox is ready) */
    waitInterval_us = (waitInterval_us > timeout_us) ? timeout_us : waitInterval_us;
    numEventChecks = (waitInterval_us == 0) ? 1 : (timeout_us / waitInterval_us);

    /* timeout event loop to permit non-blocking of thread */
    for (eventCheck = 0; eventCheck <= numEventChecks; eventCheck++)
    {
        halError = brSpiReadWord(device->devHalInfo, FR9009_ADDR_WAKE_MCU, &armCommandBusy);
        retVal = talApiErrHandler(device,FR_ERRHDL_HAL_SPI, halError, retVal, TALACT_ERR_RESET_SPI);
        IF_ERR_RETURN_U32(retVal);

        if (armCommandBusy != 0)
        {
            halError = FRHAL_wait_us(device->devHalInfo, waitInterval_us);
            retVal = talApiErrHandler(device, FR_ERRHDL_HAL_WAIT, halError, retVal, TALACT_ERR_CHECK_TIMER);
        }
        else
        {
            break;
        }
    }

    /* if busy bit remains set after timeout event loop function is exited, otherwise command is sent after extended bytes */
    if (armCommandBusy != 0)
    {
        return (uint32_t)talApiErrHandler(device, FR_ERRHDL_API_FAIL, FR_ERR_TIMEDOUT_ARMMAILBOXBUSY, retVal, TALACT_ERR_RESET_ARM);
    }
    else
    {
        halError = brSpiBlockWriteWords(device->devHalInfo, FR9009_ADDR_CMDBUF_BASE, armCommandBuf, extendedDataNumWords + 2);
        retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal, TALACT_ERR_RESET_SPI);
        IF_ERR_RETURN_U32(retVal);
		
        halError = brSpiWriteWord(device->devHalInfo, FR9009_ADDR_WAKE_MCU, 1);
        retVal = talApiErrHandler(device, FR_ERRHDL_HAL_SPI, halError, retVal, TALACT_ERR_RESET_SPI);
        IF_ERR_RETURN_U32(retVal);
    }

    return (uint32_t)retVal;
}

uint32_t FR9009_getArmVersion_v2(fr9009Device_t *device, fr9009ArmVersionInfo_t *talArmVersionInfo)
{
    talRecoveryActions_t retVal = TALACT_NO_ACTION;
    uint32_t fullVersion = 0;

#if FR9009_VERBOSE
    fRHALErr_t halError = FRHAL_OK;
    halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK, "FR9009_getArmVersion()\n");
    retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal, TALACT_WARN_RESET_LOG);
#endif

    if (talArmVersionInfo == NULL)
    {
        return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_GETARMVER_V2_NULL_PARM, retVal, TALACT_ERR_CHECK_PARAM);
    }
    retVal = (talRecoveryActions_t)brSpiBlockReadWords(device->devHalInfo, FR9009_ADDR_ARM_GLOBLE_CONFIG_DATA, &fullVersion, 1);
    IF_ERR_RETURN_U32(retVal);
    talArmVersionInfo->rcVer = (uint8_t)(fullVersion & 0xff);
    talArmVersionInfo->minorVer = (uint8_t)((fullVersion >> 8) & 0xff);
    talArmVersionInfo->majorVer = (uint8_t)((fullVersion >> 16) & 0xff);

    if(talArmVersionInfo->rcVer & 0x01)
    {
        talArmVersionInfo->buildType = FR_ARM_BUILD_DEBUG;
    }
    else if(talArmVersionInfo->rcVer & 0x04)
    {
        talArmVersionInfo->buildType = FR_ARM_BUILD_TEST_OBJECT;
    }
    else
    {
        talArmVersionInfo->buildType = FR_ARM_BUILD_RELEASE;
    }
    retVal = (talRecoveryActions_t)brSpiBlockReadWords(device->devHalInfo, FR9009_ADDR_ARM_GLOBLE_CONFIG_DATA + 0xC, &talArmVersionInfo->build_time_str, 32/4);
    IF_ERR_RETURN_U32(retVal);
    retVal = (talRecoveryActions_t)brSpiBlockReadWords(device->devHalInfo, FR9009_ADDR_ARM_GLOBLE_CONFIG_DATA + 0x2C, &talArmVersionInfo->customer_info_str, 32/4);
    IF_ERR_RETURN_U32(retVal);

    return (uint32_t)retVal;
}

uint32_t FR9009_getArmVersion(fr9009Device_t *device, uint8_t *majorVer, uint8_t *minorVer, uint8_t *rcVer)
{
    talRecoveryActions_t retVal = TALACT_NO_ACTION;
    uint32_t ver[4] = {0};
    uint32_t fullVersion = 0;

#if FR9009_VERBOSE
    fRHALErr_t halError = FRHAL_OK;
    halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK, "FR9009_getArmVersion()\n");
    retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal, TALACT_WARN_RESET_LOG);
#endif

    if ((majorVer == NULL) ||
        (minorVer == NULL) ||
        (rcVer == NULL))
    {
        return (uint32_t)talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_GETARMVER_NULL_PARM, retVal, TALACT_ERR_CHECK_PARAM);
    }

    retVal = (talRecoveryActions_t)brSpiReadWord(device, FR9009_ADDR_ARM_VERSION, &ver[0]);
    IF_ERR_RETURN_U32(retVal);

    fullVersion = (((uint32_t)ver[0]) | (((uint32_t)ver[1]) << 8) | (((uint32_t)ver[2]) << 16) | (((uint32_t)ver[3]) << 24));
    *rcVer = (uint8_t)(fullVersion % 100);
    *minorVer = (uint8_t)((fullVersion / 100) % 100);
    *majorVer = (uint8_t)(fullVersion / 10000);

    return (uint32_t)retVal;
}

// uint32_t FR9009_ArmWriteField(fr9009Device_t *device, uint32_t addr, uint32_t fieldVal, uint32_t mask, uint32_t startBit)
// {
//     talRecoveryActions_t retVal = TALACT_NO_ACTION;
//     return (uint32_t)retVal;
// }

// uint32_t FR9009_verifyArmChecksum_2(fr9009Device_t *device, uint32_t *data, uint32_t len)
// {
//     fRHALErr_t halError = FRHAL_OK;
//     talRecoveryActions_t retVal = TALACT_NO_ACTION;
//     int i = 0,j = 0;
// #define RD_DATA_MAX_LEN 128
//     uint32_t rdata[RD_DATA_MAX_LEN];
//     uint32_t rlen = RD_DATA_MAX_LEN;
//     uint32_t remain_len = len;
//     uint32_t raddr = 0;
// #if FR9009_VERBOSE
//     halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK, "FR9009_verifyArmChecksum()\n");
//     retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal, TALACT_WARN_RESET_LOG);
// #endif
//     for(i = 0; i< 1.0*len/RD_DATA_MAX_LEN; i++){
//         /* reading four (4) bytes at ARM checksum memory location */
//         rlen = remain_len < RD_DATA_MAX_LEN?remain_len : RD_DATA_MAX_LEN;
//         raddr = FR9009_ARMSPI_FLAG + 0 +i*RD_DATA_MAX_LEN*4;
//         retVal = (talRecoveryActions_t)FR9009_readArmMem(device, raddr, rdata, rlen, 0);
//         IF_ERR_RETURN_U32(retVal);
//         for(j = 0; j < rlen; j++){
//           if(rdata[j] != data[i*RD_DATA_MAX_LEN+j]){
//             raddr = raddr+2;
//             return FAILURE;
//           }
//         }
//         remain_len -= rlen;
//         if(remain_len == 0){
//             break;
//         }
//     }
//     return (uint32_t)retVal;
// }

uint32_t FR9009_verifyArmChecksum(fr9009Device_t *device, uint32_t checksum)
{
    fRHALErr_t halError = FRHAL_OK;
    talRecoveryActions_t retVal = TALACT_NO_ACTION;
    uint32_t rchecksum = 0;
#if FR9009_VERBOSE
    halError = brWriteToLog(device->devHalInfo, FRHAL_LOG_MSG, FR_ERR_OK, "FR9009_verifyArmChecksum()\n");
    retVal = talApiErrHandler(device, FR_ERRHDL_HAL_LOG, halError, retVal, TALACT_WARN_RESET_LOG);
#endif
	/* reading four (4) bytes at ARM checksum memory location */
	retVal = (talRecoveryActions_t)FR9009_ReadWait(device, FR9009_ADDR_ARM_GLOBLE_CONFIG_DATA + type_OffSet(Globle_conf_t, checksum), 
                                    checksum, &rchecksum, 10000000, WAITINITCALS_INTERVAL_US);
	IF_ERR_RETURN_U32(retVal);
	if(rchecksum != checksum){
		retVal = FR_ERR_VERIFYBIN_CHECKSUM_ERR;
	}
    return (uint32_t)retVal;
}

const char* talGetArmErrorMessage(uint32_t errSrc, uint32_t errCode)
{
    

    return "Wrong error handler - not a FR9009 ARM error\n";
}

talRecoveryActions_t talArmCmdErrorHandler(fr9009Device_t *device, fr9009ErrHdls_t errHdl,
        uint32_t detErr, talRecoveryActions_t retVal, talRecoveryActions_t recAction)
{
    
    return recAction;
}
