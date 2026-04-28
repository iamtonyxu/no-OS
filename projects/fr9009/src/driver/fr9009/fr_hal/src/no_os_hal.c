/***************************************************************************//**
 *   @file   no_os_hal.c
 *   @brief  No-OS Hardware Abstraction Layer.
 *   @author 
********************************************************************************

*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdio.h>
#include "fr_hal.h"

#ifndef HAL_writeToLogFile
#define  HAL_writeToLogFile    UART_Printf
#define UCERR	HAL_writeToLogFile
#define UCWARN	HAL_writeToLogFile
#define UCLOG	HAL_writeToLogFile
#endif

#define HAL_Gpio_Write(a, b)		gpio_direction_output(a, b)
/******************************************************************************/
/************************** Functions Implementation **************************/
/******************************************************************************/


fRHALErr_t FRHAL_setTimeout(void *devHalInfo, uint32_t halTimeout_ms)
{
	if (devHalInfo == NULL)
		return (FRHAL_GEN_SW);
    halTimeout_ms = halTimeout_ms;
	return FRHAL_OK;
}


fRHALErr_t FRHAL_openHw(void *devHalInfo, uint32_t halTimeout_ms)
{
    struct fr_hal *dev_hal_data = (struct fr_hal *)devHalInfo;
	int32_t status = 0;
	struct spi_init_param spi_param;
    halTimeout_ms = halTimeout_ms;

    if(dev_hal_data->devIndex == FRHAL_CHIP0)
    {
    	status = gpio_get((struct gpio_desc **)&dev_hal_data->gpio_resetb, FR9009_RESETB);
    	spi_param.id = 0;
    	spi_param.mode = SPI_MODE_0;
    	spi_param.chip_select = FR9009_CS;
    #ifndef ALTERA_PLATFORM
    	spi_param.flags = SPI_CS_DECODE;
    #endif
    	status |= spi_init((struct spi_desc **)&dev_hal_data->spi_desc, &spi_param);
    	status |= gpio_get(&dev_hal_data->gpio_sysref_req, ADRV_SYSREF_REQ);
    }

	return FRHAL_OK;
}


fRHALErr_t FRHAL_closeHw(void *devHalInfo)
{
	struct fr_hal *dev_hal_data = (struct fr_hal *)devHalInfo;
	int32_t status = 0;
	if (devHalInfo == NULL)
		return (FRHAL_GEN_SW);
    if(dev_hal_data->devIndex == FRHAL_CHIP0)
    {
		status = gpio_remove(dev_hal_data->gpio_resetb);
		status |= spi_remove(dev_hal_data->spi_desc);
    }

	return FRHAL_OK;
}

fRHALErr_t FRHAL_resetHw(void *devHalInfo)
{

	struct fr_hal *devHalData = (struct fr_hal *)devHalInfo;
    
	if (devHalInfo == NULL)
		return (FRHAL_GEN_SW);

    HAL_Gpio_Write(devHalData->gpio_resetb, 1);
    FRHAL_wait_us(devHalInfo, 100);
    HAL_Gpio_Write(devHalData->gpio_resetb, 0);
    FRHAL_wait_us(devHalInfo, 100);
    HAL_Gpio_Write(devHalData->gpio_resetb, 1);
    FRHAL_wait_us(devHalInfo, 100);
        
	return FRHAL_OK;
}

fRHALErr_t FRHAL_sysrefReq(void *devHalInfo, SysrefReqMode_t mode)
{
	struct fr_hal *devHalData = (struct fr_hal *)devHalInfo;
	if (devHalInfo == NULL)
		return (FRHAL_GEN_SW);

//    return FRHAL_OK;
////aura support contine and pulse, will drive by gpio
	if(mode == __SYSREF_CONT_LOCAL_ON){
		gpio_direction_output(devHalData->gpio_sysref_req, 1);
	}else if(mode == __SYSREF_CONT_LOCAL_OFF){
		gpio_direction_output(devHalData->gpio_sysref_req, 0);
	}else if(mode == __SYSREF_CONT_GLOBAL_ON){
        /*不需要多板同步的情况，global sysref 与 local sysref配置一致*/
		gpio_direction_output(devHalData->gpio_sysref_req, 1);
	}else if(mode == __SYSREF_CONT_GLOBAL_OFF){
        /*不需要多板同步的情况，global sysref 与 local sysref配置一致*/
        /*注意：当使用global配置时， global off 要包含 local off的配置，即当前的sysref一定要关闭*/
		gpio_direction_output(devHalData->gpio_sysref_req, 0);
    }else if(mode == __SYSREF_GLOBAL_PULSE){
        /*不需要多板同步的情况，global sysref 与 local sysref配置一致*/
		gpio_direction_output(devHalData->gpio_sysref_req, 1);
		FRHAL_wait_us(devHalInfo, 1000);
		gpio_direction_output(devHalData->gpio_sysref_req, 0);
    }else if(mode == __SYSREF_LOCAL_PULSE){
		gpio_direction_output(devHalData->gpio_sysref_req, 1);
		FRHAL_wait_us(devHalInfo, 1000);
		gpio_direction_output(devHalData->gpio_sysref_req, 0);
	} else
		return FRHAL_ERR;

	return FRHAL_OK;
}
#define RW_ADDR_OFFSET	1
fRHALErr_t FRHAL_spiWriteWord(void *devHalInfo,
				uint32_t addr, uint32_t data)
{
    struct fr_hal *devHalData = (struct fr_hal *)devHalInfo;
    uint8_t buf[4+8];
    int32_t status;
	if((addr&0x3) != 0)
	{
		return FRHAL_GEN_SW;
	}

    buf[0] = (addr >> (24+RW_ADDR_OFFSET)) & 0x7F; //write command
    buf[1] = (addr >> (16+RW_ADDR_OFFSET)) & 0xFF;
    buf[2] = (addr >> ( 8+RW_ADDR_OFFSET)) & 0xFF;
	buf[3] = (addr >> ( 0+RW_ADDR_OFFSET)) & 0xFE; //must be alighed to 32-bit word boundary
	
	buf[4] = (data >> 24) & 0xFF;
    buf[5] = (data >> 16) & 0xFF;
    buf[6] = (data >>  8) & 0xFF;
	buf[7] = (data      ) & 0xFF; //must be alighed to 32-bit word boundary

    status = spi_write_and_read(devHalData->spi_desc, buf, 12);//Write 4-byte register address,Write 4 byte of register data    
    if (status != SUCCESS)
        return FRHAL_SPI_FAIL;
    else
        return FRHAL_OK;
}				

fRHALErr_t FRHAL_spiBlockWriteWords(void *devHalInfo, uint32_t addr, uint32_t *data, uint32_t count)
{
    fRHALErr_t errVal;
#define WBUF_MAX_LEN	(XSPIPS_FIFO_DEPTH/4 - 2)    //根据真是SPI fifo大小 而定，注意这里是word的长度
	uint8_t buf[4 * WBUF_MAX_LEN + 8] = {0};
    uint32_t i;
    uint32_t cur_index = 0;
    uint32_t remainsz = count;
    uint32_t w_sz = 0;
    struct fr_hal *devHalData = (struct fr_hal *)devHalInfo;
    if (devHalInfo == NULL)
        return (FRHAL_GEN_SW);
    while(remainsz > 0)
    {
    	w_sz = (remainsz > WBUF_MAX_LEN) ? WBUF_MAX_LEN : remainsz;
    	cur_index = count - remainsz;//下边0对应地址
        buf[0] = (addr >> (24 + RW_ADDR_OFFSET)) & 0x7F; //write command
        buf[1] = (addr >> (16 + RW_ADDR_OFFSET)) & 0xFF;
        buf[2] = (addr >> ( 8 + RW_ADDR_OFFSET)) & 0xFF;
        buf[3] = (addr >> ( 0 + RW_ADDR_OFFSET)) & 0xFE; //must be alighed to 32-bit word boundary
    	for(i = 0; i < w_sz; i++){
            buf[4 * i + 4] = (data[cur_index + i] >> 24) & 0xFF;
            buf[4 * i + 5] = (data[cur_index + i] >> 16) & 0xFF;
            buf[4 * i + 6] = (data[cur_index + i] >>  8) & 0xFF;
            buf[4 * i + 7] = (data[cur_index + i]      ) & 0xFF;
    	}
        errVal = spi_write_and_read(devHalData->spi_desc, buf, w_sz*4 + 8);//Write 4-byte register address,Write 4 byte of register data   
        if (errVal != SUCCESS)
            return FRHAL_SPI_FAIL;

 	   addr += w_sz*4;
 	   remainsz -= w_sz;
    }

    return FRHAL_OK;
}

fRHALErr_t FRHAL_spiWriteWords(void *devHalInfo,
				 uint32_t *addr, uint32_t *data, uint32_t count)
{
    fRHALErr_t errVal;
    uint32_t i;

	for (i = 0; i < count; i++) {
		errVal = FRHAL_spiWriteWord(devHalInfo, addr[i], data[i]);
		if (errVal)
		return errVal;
	}

    return FRHAL_OK;
}


fRHALErr_t FRHAL_spiReadWord(void *devHalInfo, uint32_t addr, uint32_t *readdata)
{
    struct fr_hal *devHalData = (struct fr_hal *)devHalInfo;
    uint8_t wrbuf[4+8];
	uint8_t *rdbuf = &wrbuf[4];
    int32_t status;

    *readdata = 0;
	if((addr&0x3) != 0){
		return FRHAL_GEN_SW;
	}
    wrbuf[0] = 0x80| ((addr >> (24+RW_ADDR_OFFSET)) & 0x7F); //read command
    wrbuf[1] = (addr >> (16+RW_ADDR_OFFSET)) & 0xFF;
    wrbuf[2] = (addr >> ( 8+RW_ADDR_OFFSET)) & 0xFF;
	wrbuf[3] = (addr >> ( 0+RW_ADDR_OFFSET)) & 0xFE; //must be alighed to 32-bit word boundary
	//Initialization of [4~11] is unnecessary
    status = spi_write_and_read(devHalData->spi_desc, wrbuf, 12);
	*readdata = (rdbuf[4+0]<<24)|(rdbuf[4+1]<<16)|(rdbuf[4+2]<<8)|rdbuf[4+3];

    if (status != SUCCESS)
        return FRHAL_SPI_FAIL;
    else
        return FRHAL_OK;
}
		


fRHALErr_t FRHAL_spiReadWords(void *devHalInfo, uint32_t *addr, uint32_t *readdata, uint32_t count)
{
    fRHALErr_t errVal;
    uint32_t i;
    if (devHalInfo == NULL)
        return (FRHAL_GEN_SW);
    for (i = 0; i < count; i++) {
        errVal = FRHAL_spiReadWord(devHalInfo, addr[i], &readdata[i]);
        if (errVal)
            return errVal;
    }

    return FRHAL_OK;
}
fRHALErr_t FRHAL_spiBlockReadWords(void *devHalInfo,
				uint32_t addr, uint32_t *readdata, uint32_t count)
{
    fRHALErr_t errVal;
    uint32_t i;
    if (devHalInfo == NULL)
        return (FRHAL_GEN_SW);
    for (i = 0; i < count; i++) {
        errVal = FRHAL_spiReadWord(devHalInfo, addr, &readdata[i]);
        if (errVal)
            return errVal;
		addr+=4;
    }

    return FRHAL_OK;
}
				


fRHALErr_t FRHAL_spiWriteField(void *devHalInfo,
				 uint32_t addr, uint32_t fieldVal, uint32_t mask, uint8_t startBit)
{
    fRHALErr_t errVal;
    uint32_t readVal;
    if (devHalInfo == NULL)
        return (FRHAL_GEN_SW);
    errVal = FRHAL_spiReadWord(devHalInfo, addr, &readVal);
    if (errVal != FRHAL_OK)
        return errVal;

    readVal = (readVal & ~mask) | ((fieldVal << startBit) & mask);

    return FRHAL_spiWriteWord(devHalInfo, addr, readVal);
}
					 


fRHALErr_t FRHAL_spiReadField(void *devHalInfo, uint32_t addr, uint32_t *fieldVal, uint32_t mask, uint8_t startBit)
{
    fRHALErr_t errVal;
    uint32_t readVal;
    if (devHalInfo == NULL)
        return (FRHAL_GEN_SW);
    errVal = FRHAL_spiReadWord(devHalInfo, addr, &readVal);
    if (errVal != FRHAL_OK)
        return errVal;
    *fieldVal = ((readVal & mask) >> startBit);

    return FRHAL_OK;
}
				

fRHALErr_t  FRHAL_wait_us(void *devHalInfo, uint32_t time_us)
{
    if (devHalInfo == NULL)
        return (FRHAL_GEN_SW);
    delay_us(time_us);

    return FRHAL_OK;
}

fRHALErr_t FRHAL_writeToLog(void *devHalInfo, brLogLevel_t logLevel, uint32_t errorCode, const char *comment)
{
    struct fr_hal *dev_hal_data = (struct fr_hal *)devHalInfo;
    fRHALErr_t retVal = FRHAL_OK;

    if (devHalInfo == NULL)
        return (FRHAL_GEN_SW);

    if(devHalInfo == NULL)
    {
        return (FRHAL_GEN_SW);
    }

    if((dev_hal_data->log_level & FRHAL_LOG_ERR) && (logLevel == FRHAL_LOG_ERR))
    {
        UCERR("ERROR: %d: %s", (int)errorCode, comment);
    }
    else if((dev_hal_data->log_level & FRHAL_LOG_WARN) && (logLevel == FRHAL_LOG_WARN))
    {
        UCWARN("WARNING: %d: %s", (int)errorCode, comment);
    }
    // else if ((dev_hal_data->log_level & FRHAL_LOG_WARN) )
    //     UCLOG("SPI: %d: %s", (int)errorCode, comment);

    return retVal;
}

fRHALErr_t FRHAL_setLogLevel(void *devHalInfo, uint16_t logLevel)
{
    struct fr_hal *dev_hal_data = (struct fr_hal *)devHalInfo;

    if (devHalInfo == NULL)
        return (FRHAL_GEN_SW);

    dev_hal_data->log_level = logLevel;

    return FRHAL_OK;
}






