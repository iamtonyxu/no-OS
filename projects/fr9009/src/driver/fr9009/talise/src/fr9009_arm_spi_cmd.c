#include "fr9009_arm.h"
#include "fr9009.h"
#include "fr9009_error.h"
#include "fr9009_user.h"
#include "fr9009_hal.h"
#include "fr9009_arm_spi_cmd.h"

#define MIN(a,b)		(((a) < (b))?(a):(b))

#define IN_RANGE(x, a, b) 		((x) >= (a) && (x) <= (b))
#define DIRECT_ACCESS_MEM(x) 	(IN_RANGE((x), HOST_SYSCTRL_BASEADDR, HOST_SYSCTRL_ENDADDR) || ((x) & FR9009_ADDR_EXTDATA_FLAG) || IN_RANGE((x), FR9009_ADDR_DPRAM_BASEADDR, FR9009_ADDR_DPRAM_ENDADDR))


static int32_t _cmdBufPack(uint32_t *cmdBuf, uint32_t* paramBuf, uint32_t paramNum, uint32_t *extData, uint32_t extDataNum)
{
    int cnt = 0;
	int n = 0;
	if (((paramNum + extDataNum + 2) > FR9009_CMDBUF_WORDSIZE)
		|| (paramBuf == NULL && paramNum > 0) 
		|| (extData == NULL && extDataNum > 0))
    {
        return -1;
    }	

	/* IT BASE + 0x8    CMD PARAM*/
	for(n = 0; n < paramNum; n++)
	{
		cmdBuf[cnt++] = paramBuf[n];
	}

	/* CMD EXT DATA （Behand CMD Param）*/
	for(n = 0; n < extDataNum; n++)
	{
		cmdBuf[cnt++] = extData[n];
	}

	return cnt;	
}


uint32_t FR9009_armSpiCmd_writeWords(fr9009Device_t *device, uint32_t wrAddr, uint32_t* wrDataBuf, uint32_t wrWordSz)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
    fRHALErr_t halError = FRHAL_OK;

	uint32_t opCode = TALAPI_ARMSPI_CMD_MEM_WRITE;
	uint32_t cmdBuf[FR9009_CMDBUF_WORDSIZE] = {0};
	uint32_t cmdBufSz = 0;
	uint32_t cmdStatus;
	uint32_t paramBuf[2] = {0};

	uint32_t curWrAddr = wrAddr;
    uint32_t cnt = 0;
    uint16_t batchSz = 0;

	if(DIRECT_ACCESS_MEM(wrAddr) && DIRECT_ACCESS_MEM(wrAddr+wrWordSz*4))
	{
		halError = brSpiBlockWriteWords(device->devHalInfo, wrAddr, wrDataBuf, wrWordSz);
        retVal = talApiErrHandler(device,FR_ERRHDL_HAL_SPI, halError, retVal, TALACT_ERR_RESET_SPI);
        return (uint32_t) retVal;
	}

    while(cnt < wrWordSz){
        batchSz = MIN(FR9009_CMDBUF_EXTDATA_SIZE, wrWordSz-cnt);
		
		paramBuf[0] = curWrAddr & 0x7FFFFFFF;
		paramBuf[1] = batchSz & 0xFFFF;

		cmdBufSz = _cmdBufPack(cmdBuf, paramBuf, 2, &wrDataBuf[cnt], batchSz);
		if(cmdBufSz < 0){
			return (uint32_t) retVal;
		}
		
		retVal = FR9009_sendArmCommand(device, opCode, cmdBuf, cmdBufSz);
		IF_ERR_RETURN_U32(retVal);		
		
		retVal = FR9009_waitArmCmdStatus(device, 0, &cmdStatus, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
    	IF_ERR_RETURN_U32(retVal);
		
        curWrAddr = curWrAddr + batchSz*4;
        cnt = cnt + batchSz;
    }
	return retVal;	
}

uint32_t FR9009_armSpiCmd_readWords(fr9009Device_t *device, uint32_t rdAddr, uint32_t* rdDataBuf, uint32_t rdWordSz)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
    fRHALErr_t halError = FRHAL_OK;

	uint32_t opCode = TALAPI_ARMSPI_CMD_MEM_READ;
	uint32_t cmdStatus;
	uint32_t paramBuf[2] = {0};	
	
	uint32_t curRdAddr = rdAddr;
    uint32_t cnt = 0;
    uint16_t batchSz = 0;

	if(DIRECT_ACCESS_MEM(rdAddr) && DIRECT_ACCESS_MEM(rdAddr+rdWordSz*4))
	{
		halError = brSpiBlockReadWords(device->devHalInfo, rdAddr, rdDataBuf, rdWordSz);
        retVal = talApiErrHandler(device,FR_ERRHDL_HAL_SPI, halError, retVal, TALACT_ERR_RESET_SPI);
        return (uint32_t) retVal;
	}

    while(cnt < rdWordSz){
        batchSz = MIN(FR9009_CMDBUF_EXTDATA_SIZE, rdWordSz-cnt);
		
		paramBuf[0] = curRdAddr & 0x7FFFFFFF;
		paramBuf[1] = batchSz & 0xFFFF;

		FR9009_sendArmCommand(device, opCode, paramBuf, 2);
		IF_ERR_RETURN_U32(retVal);
		
		retVal = FR9009_waitArmCmdStatus(device, 0, &cmdStatus, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
    	IF_ERR_RETURN_U32(retVal);
		
		halError = brSpiBlockReadWords(device->devHalInfo, FR9009_ADDR_CMDBUF_EXTDATA, &rdDataBuf[cnt], batchSz);
        retVal = talApiErrHandler(device,FR_ERRHDL_HAL_SPI, halError, retVal, TALACT_ERR_RESET_SPI);
        IF_ERR_RETURN_U32(retVal);

        curRdAddr = curRdAddr + batchSz*4;
        cnt = cnt + batchSz;
    }
	return retVal;
}


uint32_t FR9009_armSpiCmd_writeField(fr9009Device_t *device, uint32_t addr, uint32_t fieldVal,  uint32_t mask, uint32_t startBit)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
    fRHALErr_t halError = FRHAL_OK;

	uint32_t opCode = TALAPI_ARMSPI_CMD_FIELD_WRITE;
	uint32_t cmdStatus;
	uint32_t paramBuf[3] = {addr, mask, (fieldVal<<startBit)};

	if(DIRECT_ACCESS_MEM(addr))
	{
		halError = brSpiWriteField(device->devHalInfo, addr, fieldVal, mask, startBit);
        retVal = talApiErrHandler(device,FR_ERRHDL_HAL_SPI, halError, retVal, TALACT_ERR_RESET_SPI);
        return (uint32_t) retVal;
	}

	retVal = FR9009_sendArmCommand(device, opCode, paramBuf, 3);
	IF_ERR_RETURN_U32(retVal);
	
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdStatus, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
	IF_ERR_RETURN_U32(retVal);
	
	return retVal;
}


uint32_t FR9009_armSpiCmd_readField(fr9009Device_t *device, uint32_t addr, uint32_t *fieldVal,  uint32_t mask, uint32_t startBit)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t readVal;

	retVal = FR9009_armSpiCmd_readWords(device, addr, &readVal, 1);
	*fieldVal = ((readVal & mask) >> startBit);
	return retVal;	
}

uint32_t FR9009_armSpiCmd_log_init_mode(fr9009Device_t *device, uint8_t level, uint32_t mask)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[2];
    cmdbuf[0] = level;
    cmdbuf[1] = mask;
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_LOG_CONFIG | (0x0000 << 16), cmdbuf, 2);
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
    IF_ERR_RETURN_U32(retVal);
	return retVal;
}

uint32_t FR9009_armSpiCmd_log_opt_set(fr9009Device_t *device, uint8_t wr2ram_en)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];
    cmdbuf[0] = (wr2ram_en & 0xF);
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_LOG_CONFIG | (LOG_OPT_SET << 16), cmdbuf, 2);
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
    IF_ERR_RETURN_U32(retVal);
	return retVal;
}

uint32_t FR9009_armSpiCmd_log_read_from_ram(fr9009Device_t *device, uint32_t* rdDataBuf, uint32_t rdWordSz)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
    fRHALErr_t halError = FRHAL_OK;

	uint32_t opCode = TALAPI_ARMSPI_CMD_LOG_CONFIG | (LOG_GET_FROMRAM << 16);
	uint32_t cmdStatus;
	uint32_t paramBuf[2] = {0};	
	
    uint32_t cnt = 0;
    uint16_t batchSz = 0;

    while(cnt < rdWordSz){
        batchSz = MIN(FR9009_CMDBUF_EXTDATA_SIZE, rdWordSz-cnt);
		paramBuf[0] = batchSz & 0xFFFF;

		FR9009_sendArmCommand(device, opCode, paramBuf, 2);
		IF_ERR_RETURN_U32(retVal);
		
		retVal = FR9009_waitArmCmdStatus(device, 0, &cmdStatus, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
    	IF_ERR_RETURN_U32(retVal);
		
		halError = brSpiBlockReadWords(device->devHalInfo, FR9009_ADDR_CMDBUF_EXTDATA, &rdDataBuf[cnt], batchSz);
        retVal = talApiErrHandler(device,FR_ERRHDL_HAL_SPI, halError, retVal, TALACT_ERR_RESET_SPI);
        IF_ERR_RETURN_U32(retVal);

        cnt = cnt + batchSz;
    }
	return retVal;
}


uint32_t FR9009_armSpiCmd_clear_tx_qfir(fr9009Device_t *device)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;	

	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_CLEAR_TX_QFIR, NULL, 0);
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US,WAITCOMMONARMCMD_INTERVAL_US);
    IF_ERR_RETURN_U32(retVal);
	return retVal;
}

uint32_t FR9009_armSpiCmd_clear_tx_cfir(fr9009Device_t *device)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;

	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_CLEAR_TX_CFIR, NULL, 0);
	IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
	IF_ERR_RETURN_U32(retVal);
	return retVal;
}

uint32_t FR9009_armSpiCmd_setup_bb_rf_pll(fr9009Device_t *device)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;

	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_SETUP_BB_RF_PLL, NULL, 0);
	IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
	IF_ERR_RETURN_U32(retVal);
	return retVal;
}

uint32_t FR9009_armSpiCmd_puresweep_lb_rx1(fr9009Device_t *device, CHANNEL_t sweep_ch, uint8_t check_point)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];

	cmdbuf[0] = ((sweep_ch - 1) & 0xF) | ((check_point << 4) & 0xF0); //(sweep_ch-1)为了跟arm程序兼容，arm中做了ch+1操作
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_PURESWEEP_LB_RX1, cmdbuf, 1);
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
    IF_ERR_RETURN_U32(retVal);

	return retVal;
}

uint32_t FR9009_armSpiCmd_setonefreq_lb_rx1(fr9009Device_t *device, CHANNEL_t freq_ch, uint8_t freq_index, uint8_t check_point)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];

	cmdbuf[0] = ((freq_ch - 1) & 0xF) | ((check_point << 4) & 0xF0) | ((freq_index << 12) & 0xFF000);
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_SETONEFREQ_LB_RX1, cmdbuf, 1);
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
    IF_ERR_RETURN_U32(retVal);

	return retVal;
}

uint32_t FR9009_armSpiCmd_calibrationfunc_lb_rx1(fr9009Device_t *device, CHANNEL_t cali_ch)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];

	cmdbuf[0] = ((cali_ch - 1) & 0xF);
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_CALIBRATIONFUNC_LB_RX1, cmdbuf, 1);
    IF_ERR_RETURN_U32(retVal);

	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
    IF_ERR_RETURN_U32(retVal);
	
	return retVal;
}

uint32_t FR9009_armSpiCmd_puresweep_lb_orx1(fr9009Device_t *device,  CHANNEL_t sweep_ch, uint8_t check_point)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];

	cmdbuf[0] = ((sweep_ch - 1) & 0xF) | ((check_point << 4) & 0xF0);
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_PURESWEEP_LB_ORX1, cmdbuf, 1);
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
    IF_ERR_RETURN_U32(retVal);

	return retVal;
}

uint32_t FR9009_armSpiCmd_setonefreq_lb_orx1(fr9009Device_t *device, CHANNEL_t freq_ch, uint8_t freq_index, uint8_t check_point)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];

	cmdbuf[0] = ((freq_ch - 1) & 0xF) | ((check_point << 4) & 0xF0) | ((freq_index << 12) & 0xFF000);
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_SETONEFREQ_LB_ORX1, cmdbuf, 1);
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
    IF_ERR_RETURN_U32(retVal);

	return retVal;
}

uint32_t FR9009_armSpiCmd_calibrationfunc_lb_orx1(fr9009Device_t *device, CHANNEL_t cali_ch)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];

	cmdbuf[0] = ((cali_ch - 1) & 0xF);
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_CALIBRATIONFUNC_LB_ORX1, cmdbuf, 1);
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
    IF_ERR_RETURN_U32(retVal);
	
	return retVal;
}

uint32_t FR9009_armSpiCmd_tx_puresweep(fr9009Device_t *device, CHANNEL_t cali_ch)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];

	cmdbuf[0] = ((cali_ch - 1) & 0xF);
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_TX_PURESWEEP, cmdbuf, 1);
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
    IF_ERR_RETURN_U32(retVal);

	return retVal;
}

uint32_t FR9009_armSpiCmd_tx_setonefreq(fr9009Device_t *device, CHANNEL_t tx_ch, uint32_t freqpoint)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];

	cmdbuf[0] = ((tx_ch - 1) & 0xF) | ((freqpoint << 12) & 0xFF000);
	retVal = FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_TX_SETONEFREQ, cmdbuf, 1);
	IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
    IF_ERR_RETURN_U32(retVal);

	return retVal;
}

uint32_t FR9009_armSpiCmd_tx_calibrationfunc(fr9009Device_t *device,  CHANNEL_t cali_ch)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];

	cmdbuf[0] = ((cali_ch - 1) & 0xF);
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_TX_CALIBRATIONFUNC, cmdbuf, 1);
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
    IF_ERR_RETURN_U32(retVal);
	
	return retVal;
}
/**
 * lb_channel: CHANNEL_1 or CHANNEL_2
 * rx_orx_ch: 外部回环时，tx loopback 到rx 或者orx 的channel，只能为CHANNEL_1，CHANNEL_2
 * lp_type：回环模式：0：芯片内部回环 1：芯片外部回环
 * loopmode：参见 enum TX_LOOPMODE_t 1：loopback to rx， 2：loopback to orx, 3：loopback to orx（芯片内部回环有用，内部orx另外一条通路），其他值无效错误
*/
uint32_t FR9009_armSpiCmd_setup_tx_loopback(fr9009Device_t *device, CHANNEL_t lb_channel, CHANNEL_t rx_orx_ch,uint8_t lp_type, uint8_t loopmode)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];
	if(loopmode > 3){
		retVal = TALACT_ERR_CHECK_PARAM;
		return retVal;
	}

	cmdbuf[0] = ((lb_channel - 1) & 0xF) | ((lp_type << 12) & 0xF000) | ((rx_orx_ch & 0x3) << 20) | (((loopmode - 1) << 30) & 0xC0000000);
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_SETUP_TX_LOOPBACK, cmdbuf, 1);
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
    IF_ERR_RETURN_U32(retVal);
	
	return retVal;
}

uint32_t FR9009_armSpiCmd_setup_loopback_lb_rx1(fr9009Device_t *device, CHANNEL_t lb_channel)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];

	cmdbuf[0] = ((lb_channel - 1) & 0xF);
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_SETUP_LOOPBACK_LB_RX1, cmdbuf, 1);
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
    IF_ERR_RETURN_U32(retVal);
	return retVal;
}

uint32_t FR9009_armSpiCmd_setup_loopback_lb_orx1(fr9009Device_t *device, CHANNEL_t lb_channel)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];

    cmdbuf[0] = ((lb_channel-1)&0xF);            //arm中进行了+1操作
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_SETUP_LOOPBACK_LB_ORX1, cmdbuf, 1);
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
    IF_ERR_RETURN_U32(retVal);
	return retVal;
}

uint32_t FR9009_armSpiCmd_tx_lo_leak_cali(fr9009Device_t *device, CHANNEL_t set_channel)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];

	cmdbuf[0] = ((set_channel - 1) & 0xF);
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_LO_LEAK_CALI, cmdbuf, 1);
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
    IF_ERR_RETURN_U32(retVal);
	return retVal;
}

uint32_t FR9009_armSpiCmd_rx_lo_leak_cali_addr4(fr9009Device_t *device, CHANNEL_t set_channel)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];

	cmdbuf[0] = ((set_channel - 1) & 0xF);
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_O_RX_LO_LEAK_CALI_ADDR4, cmdbuf, 1);
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
    IF_ERR_RETURN_U32(retVal);
	return retVal;
}

uint32_t FR9009_armSpiCmd_orx_lo_leak_cali_addr1c(fr9009Device_t *device, CHANNEL_t set_channel)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];

	cmdbuf[0] = ((set_channel - 1) & 0xF);
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_O_RX_LO_LEAK_CALI_ADDR1C, cmdbuf, 1);
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
    IF_ERR_RETURN_U32(retVal);
	return retVal;
}

uint32_t FR9009_armSpiCmd_temparature_get(fr9009Device_t *device, uint8_t temp_sel, int32_t *tempareture_m)
{
    fRHALErr_t halError = FRHAL_OK;
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];
	uint32_t reg = 0;

    cmdbuf[0] = temp_sel;
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_AUX_CONFIG | (AUX_MODE_TEMPARATURE_GET << 16), cmdbuf, 1);
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, 100000000,WAITCOMMONARMCMD_INTERVAL_US);
    IF_ERR_RETURN_U32(retVal);
	halError = brSpiBlockReadWords(device->devHalInfo, FR9009_ADDR_CMDBUF_EXTDATA, &reg, 1);
	retVal = talApiErrHandler(device,FR_ERRHDL_HAL_SPI, halError, retVal, TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);
	*tempareture_m = (int32_t)reg;
	return retVal;
}

uint32_t FR9009_armSpiCmd_temparature_read(fr9009Device_t *device, uint8_t temp_sel, int32_t *tempareture_m)
{
    fRHALErr_t halError = FRHAL_OK;
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];
	uint32_t reg = 0;

	cmdbuf[0] = temp_sel;
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_AUX_CONFIG | (AUX_MODE_TEMPARATURE_READ << 16), cmdbuf, 1);
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, 100000000, WAITCOMMONARMCMD_INTERVAL_US);
    IF_ERR_RETURN_U32(retVal);
	halError = brSpiBlockReadWords(device->devHalInfo, FR9009_ADDR_CMDBUF_EXTDATA, &reg, 1);
	retVal = talApiErrHandler(device,FR_ERRHDL_HAL_SPI, halError, retVal, TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);
	*tempareture_m = (int32_t)reg;
	return retVal;
}

/**芯片结温保护使能**/
/*param:
**fr9009Device_t *device：fr9009Device_t的指针，使用此结构体解析
**uint8_t en: 0-disable, 1-enable
*/
uint32_t FR9009_armSpiCmd_temp_protect_en(fr9009Device_t *device, uint8_t en)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	// uint32_t cmdstatword;
	uint32_t cmdbuf[1];

	cmdbuf[0] = (en != 0);
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_AUX_CONFIG | (AUX_MODE_TEMPARATURE_PROTECT_EN << 16), cmdbuf, 1);
	IF_ERR_RETURN_U32(retVal);

	return retVal;
}

/**芯片结温保护状态读取**/
/*param:
**fr9009Device_t *device：fr9009Device_t的指针，使用此结构体解析
**uint8_t *status: 0-尚未触发温度保护， 非零值-已触发温度保护，且数值表示触发时的芯片结温
*/
uint32_t FR9009_armSpiCmd_temp_protect_status(fr9009Device_t *device, uint8_t *status)
{
    fRHALErr_t halError = FRHAL_OK;
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1] = {0};
	uint32_t reg = 0;

	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_AUX_CONFIG | (AUX_MODE_TEMPARATURE_PROTECT_STATUS << 16), cmdbuf, 1);
	IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, 100000000, WAITCOMMONARMCMD_INTERVAL_US);
    IF_ERR_RETURN_U32(retVal);
	halError = brSpiBlockReadWords(device->devHalInfo, FR9009_ADDR_CMDBUF_EXTDATA, &reg, 1);
	retVal = talApiErrHandler(device,FR_ERRHDL_HAL_SPI, halError, retVal, TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);
	*status = reg & 0xff;

	return retVal;
}
/*param:
*/
uint32_t FR9009_armSpiCmd_test_config(fr9009Device_t *device, uint8_t test_key, uint32_t conf)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];

	cmdbuf[0] = (conf & 0x0FFFFFFF) | (test_key << 28);
	retVal= FR9009_sendArmCommand(device, SPI_CMD_TEST_CONFIG, cmdbuf, 1);
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
    IF_ERR_RETURN_U32(retVal);
	return retVal;
}

/** 配置tracking enable
*param:
**fr9009Device_t *device：fr9009Device_t的指针，使用此结构体解析
**uint32_t enableMask：详见fr9009TrackingCalibrations_t
*/
uint32_t FR9009_armSpiCmd_track_cali_En(fr9009Device_t *device, uint32_t enableMask)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];

    cmdbuf[0] = enableMask;
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_TRACK_CALI | (TRACK_MASK_SET_MODE << 16), cmdbuf, 1);
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
    IF_ERR_RETURN_U32(retVal);
	return retVal;
}
/**
 * @description: 设置rx dc cali 模式
 * @return {*}
 * @param {fr9009Device_t} *device
 * @param {uint8_t} mode： 0： auto 1：manual
 */
uint32_t FR9009_armSpiCmd_rx_dc_mode(fr9009Device_t *device, uint8_t mode)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];

    cmdbuf[0] = mode;
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_TRACK_CALI | (RX_DC_CALI_SET_MODE << 16), cmdbuf, 1);
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
    IF_ERR_RETURN_U32(retVal);
	return retVal;
}
/**
 * @description: 设置tx dc(ofdm) 及 irr cali 校准功率
 * @return {*}
 * @param {fr9009Device_t} *device
 * @param {uint16_t} tone_amp_dig 0xFFFF = 0dbfs， 默认0x4026 （-6dbfs）
 * @param {uint16_t} ofdm_power 取负号例如 5 = -5dbfs，默认10（-10dbfs）
 */
uint32_t FR9009_armSpiCmd_cali_power_set(fr9009Device_t *device, uint16_t tone_amp_dig, uint16_t ofdm_power)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];

    cmdbuf[0] = tone_amp_dig | ((uint32_t)ofdm_power << 16);
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_TRACK_CALI | (CALI_POWER_SET_MODE << 16), cmdbuf, 1);
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
    IF_ERR_RETURN_U32(retVal);
	return retVal;
}

/**
 * @description: 
 * @return {*}
 * @param {fr9009Device_t} *device
 * @param {uint8_t} pllsel:
									PLLSEL_RFPLL=0x2,
									PLLSEL_ORFPLL = 0x4,
									PLLSEL_ALL = 0x6,
 * @param {int32_t} *freq_khz : 预校准频点
 * @param {uint16_t} nums：多少预校准点，最大32
 */
uint32_t FR9009_armSpiCmd_precali_freqs_set(fr9009Device_t *device, uint8_t pllsel, int32_t *freq_khz, uint16_t nums)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[100];
	uint32_t i =0;
	if(nums > 32){
		return FR_ERR_SPI_CMD_ERR_PARAM;
	}
	cmdbuf[0] = (pllsel & 0xF) | (nums << 16);
	for(i= 0; i < nums; i++){
		cmdbuf[i + 1] = freq_khz[i];
	}
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_TRACK_CALI | (PRECALI_FREQS_SET_MODE << 16), cmdbuf, nums + 1);
	IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
	IF_ERR_RETURN_U32(retVal);
	return retVal;
}

uint32_t FR9009_armSpiCmd_pre_cali_load(fr9009Device_t *device, fr9009RfPllName_t pll_type, uint64_t freq_hz)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[3];
	if(pll_type > 2){
		return -1;
	}
	cmdbuf[0] = (freq_hz >> 32) & 0xFFFFFFFF;
	cmdbuf[1] = freq_hz & 0xFFFFFFFF;
	cmdbuf[2] = 1 << pll_type;
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_TRACK_CALI | (PRECALI_REG_LOAD_MODE << 16), cmdbuf, 3);
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
    IF_ERR_RETURN_U32(retVal);
	return retVal;
}
/**
 * @description: 
 * @return {*}
 * @param {fr9009Device_t} *device
 * @param {uint32_t} mode:
 * 		bit[3 : 0 ] rx1_irr_mode	
		bit[7 : 4 ] rx1_lol_mode
		bit[11: 8 ] rx2_irr_mode
		bit[15: 12] rx2_lol_mode
		bit[19: 16] tx1_irr_mode
		bit[23: 20] tx1_lol_mode
		bit[27: 24] tx2_irr_mode
		bit[31: 28] tx2_lol_mode
		value:// 0 disable, 1 using trim scalar value, 2, using default trim table.
 */
uint32_t FR9009_armSpiCmd_cali_trim_mode_set(fr9009Device_t *device, uint32_t mode)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];

    cmdbuf[0] = mode;
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_TRACK_CALI | (CALI_TRIM_SET_MODE << 16), cmdbuf, 1);
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
    IF_ERR_RETURN_U32(retVal);
	return retVal;
}

uint32_t FR9009_armSpiCmd_cali_irr_scalar_set(fr9009Device_t *device, CHANNEL_TYPE_t ch_type, CHANNEL_t ch, int8_t irr_gain_trim, int8_t irr_theta_trim)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];

    cmdbuf[0] = ((ch_type & 0xF) << 28) | ((ch & 0xF) << 24) | ((irr_gain_trim & 0xFF) << 0) | ((irr_theta_trim & 0xFF) << 8) ;
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_TRACK_CALI | (CALI_TRIM_IRR_SCALAR_SET << 16), cmdbuf, 1);
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
    IF_ERR_RETURN_U32(retVal);
	return retVal;
}

uint32_t FR9009_armSpiCmd_cali_irr_scalar_auto(fr9009Device_t *device, CHANNEL_TYPE_t ch_type, CHANNEL_t ch)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];

    cmdbuf[0] = ((ch_type & 0xF) << 28) | ((ch & 0xF) << 24);
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_TRACK_CALI | (CALI_TRIM_IRR_SCALAR_ESTI << 16), cmdbuf, 1);
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
    IF_ERR_RETURN_U32(retVal);
	return retVal;
}

/** 配置 rx agc mode
*param:
**fr9009Device_t *device：fr9009Device_t的指针，使用此结构体解析
**uint8_t mode : 0: MGC 1：AGC 模式
*/
uint32_t FR9009_armSpiCmd_agc_mode_set(fr9009Device_t *device, uint8_t mode)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];

	cmdbuf[0] = (mode & 0xF);
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_CONFIG_AGC | (AGC_SET_MGC_OR_AGC << 16), cmdbuf, 1);
	IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
	IF_ERR_RETURN_U32(retVal);
	return retVal;
}
/** 配置 rx
*param:
**fr9009Device_t *device：fr9009Device_t的指针，使用此结构体解析
**CHANNEL_t ch：0x01—ch1，0x02—ch2 0x3: ch1 & ch2
**uint8_t index：根据配置最大值最小值进行配置
**uint8_t mode : 0: MGC 1：AGC 模式
*/
uint32_t FR9009_armSpiCmd_mgc_agc_mode(fr9009Device_t *device, CHANNEL_t channel, uint8_t index, uint8_t mode)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];

	cmdbuf[0] = (channel & 0xF) | ((index << 4) & 0xFF0) | ((mode << 31) & 0x80000000);
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_CONFIG_AGC | (AGC_SET_MGC_SET << 16), cmdbuf, 1);
	IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
	IF_ERR_RETURN_U32(retVal);
	return retVal;
}
uint32_t FR9009_armSpiCmd_agc_config_set(fr9009Device_t *device, uint32_t *data, uint16_t datalen)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;

	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_CONFIG_AGC | ((data[0] & 0xFF) << 16), &data[1], datalen - 1);
	IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
	IF_ERR_RETURN_U32(retVal);
	return retVal;
}

uint32_t FR9009_armSpiCmd_agc_config_get(fr9009Device_t *device, uint32_t *data, uint16_t datalen)
{
    fRHALErr_t halError = FRHAL_OK;
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;

	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_CONFIG_AGC | ((data[0] & 0xFF) << 16), &data[1], datalen - 1);
	IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
	IF_ERR_RETURN_U32(retVal);
	halError = brSpiBlockReadWords(device->devHalInfo, FR9009_ADDR_CMDBUF_EXTDATA, &data[1], datalen - 1);
	retVal = talApiErrHandler(device,FR_ERRHDL_HAL_SPI, halError, retVal, TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);
	return retVal;
}
/**turn off loopback and radio
 * 
*/
uint32_t FR9009_armSpiCmd_turnoff_all(fr9009Device_t *device)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;

	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_TURNOFF_LOOPBACK_LO_ORFPLL, NULL, 0);
	IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
	IF_ERR_RETURN_U32(retVal);
	return retVal;
}

uint32_t FR9009_armSpiCmd_rx_dig_dc_cali(fr9009Device_t *device, CHANNEL_t set_channel)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];

    cmdbuf[0] = (set_channel & 0x3);
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_RX_DIG_DC_XAL, cmdbuf, 1);
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
    IF_ERR_RETURN_U32(retVal);
	return retVal;
}

/*param:
**fr9009Device_t *device：fr9009Device_t的指针，使用此结构体解析
**CHANNEL_t ch：0x01—ch1，0x02—ch2 0x3: ch1 & ch2，
**tx_ch		:选择tx 校准通道
**rx_ch		:选择rx 校准通道
**orx_ch	:选择orx 校准通道
**tx1_lp2orx_ch	：tx外部回环校准时，orx的通道，0:使用与tx相同的通道进行校准，0x1：txloopback2orx1,0x2：txloopback2orx2， 这里仅外部回环有用,
**tx2_lp2orx_ch	：tx外部回环校准时，orx的通道，0:使用与tx相同的通道进行校准，0x1：txloopback2orx1,0x2：txloopback2orx2， 这里仅外部回环有用,
**uint8_t external_lp : 0: 芯片内部回环 1：芯片外部回环
**uint8_t tx_lpmode : 参见enum TX_LOOPMODE_t，0: 无效值 1：txloop2rx 2: txloop2orx 3:txloop2orx2
**cali_flag: 参见enum fr9009InitCalibrations_t
*/
uint32_t FR9009_armSpiCmd_Initical_cali(fr9009Device_t *device, CHANNEL_t tx_ch, CHANNEL_t rx_ch, CHANNEL_t orx_ch, CHANNEL_t tx1_lp2orx_ch, CHANNEL_t tx2_lp2orx_ch, 
uint8_t external_lp, uint8_t tx_lpmode, uint32_t cali_flag)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	// uint32_t cmdstatword;
	uint32_t cmdbuf[2];

	//bit16 1:可以选择校准选项， 0：全部校准 bit17:1新版本 0：旧版本，tx orx rx通道统一为tx通道
	cmdbuf[0] = ((tx_ch - 1) & 0x3) 
				| (tx_ch << 2) 
				| (rx_ch << 24) 
				| (orx_ch << 26) 
				| (tx1_lp2orx_ch << 20) 
				| (tx2_lp2orx_ch << 22) 
				| ((external_lp << 12) & 0x1000)
				| ((tx_lpmode << 13) & 0xE000);
	cmdbuf[1] = cali_flag;
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_INIT_CAL | (INIT_CHANNEL_CALIBRATION << 16), cmdbuf, 2);
    IF_ERR_RETURN_U32(retVal);
	// retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITARMCMDINITCALI_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
   	// IF_ERR_RETURN_U32(retVal);
	return retVal;
}
/**
 * @description: 
 * @return {*}
 * @param {fr9009Device_t} *device
 * @param {CHANNEL_t} tx_ch ：设置DA校准通道，0：不进行校准 
 * @param {CHANNEL_t} rx_ch ：设置AD校准通道，0：不进行校准 
 */
uint32_t FR9009_armSpiCmd_Init_ADDAcali(fr9009Device_t *device, CHANNEL_t tx_ch, CHANNEL_t rx_ch, uint8_t *status)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
    fRHALErr_t halError = FRHAL_OK;
	uint32_t cmdstatword;
	uint32_t cmdbuf[2];
	cmdbuf[0] = (rx_ch & 0x3) | (tx_ch << 4);
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_INIT_CAL | (INIT_ADDA_CALIBRATION << 16), cmdbuf, 1);
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITARMCMDADCCALI_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
   	IF_ERR_RETURN_U32(retVal);
	if(status){
		halError = brSpiBlockReadWords(device->devHalInfo, FR9009_ADDR_CMDBUF_EXTDATA, &cmdbuf[1], 1);
		retVal = talApiErrHandler(device,FR_ERRHDL_HAL_SPI, halError, retVal, TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
		*status = cmdbuf[1];
	}
	return retVal;
}
/**
 * @description: 设置mcu 运行时钟源
 * @return {*}
 * @param {fr9009Device_t} *device
 * @param {uint8_t} armClkSel ： 0：refclk 1：BB clk
 */
uint32_t FR9009_armSpiCmd_arm_clkSel(fr9009Device_t *device, uint8_t armClkSel)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];
	if(armClkSel > 1){
		return FR_ERRHDL_INVALID_PARAM;
	}
	cmdbuf[0] = armClkSel & 0x1;
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_INIT_CAL | (INIT_ARM_CLK_SEL << 16), cmdbuf, 1);
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
   	IF_ERR_RETURN_U32(retVal);
	return retVal;
}
/**
 * @description: 
 * @return {*}
 * @param {fr9009Device_t} *device
 * @param {CHANNEL_t} rx_ch, 需要开启的rx 或 orx的通道，CHANNE_1， CHANNEL_2 或者CHANNEL_1_2
 * @param {uint8_t} num ，需要开启的ADC数量
 * @param {uint8_t} *cur_num， 获取当前adc开启数量，不需要传递为NULL
 */
uint32_t FR9009_armSpiCmd_Init_ADNum(fr9009Device_t *device,  CHANNEL_t rx_ch, uint8_t num, uint8_t *cur_num)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
    fRHALErr_t halError = FRHAL_OK;
	uint32_t cmdstatword;
	uint32_t cmdbuf[2];
	cmdbuf[0] = (rx_ch & 0x3) | (num << 4);
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_INIT_CAL | (INIT_SUB_ADC_NUM_SET << 16), cmdbuf, 1);
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
   	IF_ERR_RETURN_U32(retVal);
	if(cur_num){
		halError = brSpiBlockReadWords(device->devHalInfo, FR9009_ADDR_CMDBUF_EXTDATA, &cmdbuf[1], 1);
		retVal = talApiErrHandler(device,FR_ERRHDL_HAL_SPI, halError, retVal, TALACT_ERR_RESET_SPI);
		IF_ERR_RETURN_U32(retVal);
		*cur_num = cmdbuf[1];
	}
	return retVal;
}

uint32_t FR9009_armSpiCmd_Init_Reg(fr9009Device_t *device,  uint8_t type)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];
	cmdbuf[0] = type;
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_INIT_CAL | (INIT_APB_REG_SET << 16), cmdbuf, 1);
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
   	IF_ERR_RETURN_U32(retVal);
	return retVal;
}
/**
 * jesdtx_rx: 0x1 只开启jesd tx fifo 0x2：只开启jesd rx fifo 0x3：开启jesd tx rx fifo
*/
uint32_t FR9009_armSpiCmd_Jesd_SerdesInit(fr9009Device_t *device, uint8_t jesdtx_rx)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];

	cmdbuf[0] = jesdtx_rx;
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_JESD_CONFIG | (JESD_SERDES_INIT << 16), cmdbuf, 1);
	IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
	IF_ERR_RETURN_U32(retVal);
	return retVal;
}
/**
 * @description: 
 * @return {*}
 * @param {uint8_t} serAmplitude Serializer amplitude setting. Default = 15. Range is 0..15
 * @param {uint8_t} serPreEmphasis Serializer pre-emphasis setting. Default = 1 Range is 0..4
 * @param {uint8_t} desEqSetting Deserializer Equalizer setting. Applied to all deserializer lanes. Range is 0..7
 */
uint32_t FR9009_armSpiCmd_Jesd_SerializerParam(fr9009Device_t *device, uint8_t serAmplitude, uint8_t serPreEmphasis)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];

	cmdbuf[0] = serAmplitude | (serPreEmphasis << 8);
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_JESD_CONFIG | (JESD_SERIALIZER_PARAM_SET << 16), cmdbuf, 1);
	IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
	IF_ERR_RETURN_U32(retVal);
	return retVal;
}
/**
 * @description: 
 * @return {*}
 * @param {uint8_t} desEqSetting Deserializer Equalizer setting. Applied to all deserializer lanes. Range is 0..7
 */
uint32_t FR9009_armSpiCmd_Jesd_DeserializerParam(fr9009Device_t *device,uint8_t desEqSetting)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];

	cmdbuf[0] = desEqSetting & 0X7;
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_JESD_CONFIG | (JESD_DESERIALIZER_PARAM_SET << 16), cmdbuf, 1);
	IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
	IF_ERR_RETURN_U32(retVal);
	return retVal;
}
/**
 * @description: 
 * @return {*}
 * @param {uint8_t} div_n : 0:DIV_1	1:DIV_2	other: err
 */
uint32_t FR9009_armSpiCmd_Jesd_DivSet(fr9009Device_t *device,uint8_t div_n)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];

	cmdbuf[0] = div_n & 0X1;
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_JESD_CONFIG | (JESD_CLK_DIV_SET << 16), cmdbuf, 1);
	IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
	IF_ERR_RETURN_U32(retVal);
	return retVal;
}
/**
 * state: 0 关闭自动jesd rx reset 0x1 开启自动reset模式1  0x2：开启自动reset模式2
*/
uint32_t FR9009_armSpiCmd_Jesd_state_autorelink(fr9009Device_t *device, uint8_t state)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];

	cmdbuf[0] = state;
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_JESD_STATE_SET, cmdbuf, 1);
	IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
	IF_ERR_RETURN_U32(retVal);
	return retVal;
}
/**
 * @description: 
 * @return {*}
 * @param {fr9009Device_t} *device
 * @param {uint8_t} enable: 0:diable 1:enable
 * @param {uint8_t} sync_pll_sel:    PLLSEL_BBPLL = 0x1,
									PLLSEL_RFPLL=0x2,
									PLLSEL_ORFPLL = 0x4,
									PLLSEL_ALL = 0x7,
 */
uint32_t FR9009_armSpiCmd_sync_en(fr9009Device_t *device, uint8_t enable, uint8_t sync_pll_sel)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];
	cmdbuf[0] = (enable & 0xF) | ((sync_pll_sel & 0xF) << 4);
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_SYNC | (SYNC_FUNC_MODE_PLL_EN << 16), cmdbuf, 1);
	IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
	IF_ERR_RETURN_U32(retVal);
	return retVal;
}
uint32_t FR9009_armSpiCmd_resync(fr9009Device_t *device, uint8_t sync_pll_sel)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];
	cmdbuf[0] = ((sync_pll_sel & 0xF) << 4);
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_SYNC | (SYNC_FUNC_MODE_RESYNC << 16), cmdbuf, 1);
	IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US,WAITCOMMONARMCMD_INTERVAL_US);
	IF_ERR_RETURN_U32(retVal);
	return retVal;
}
/**
 * @description: 
 * @return {*}
 * @param {fr9009Device_t} *device
 * @param {uint8_t} sync_pll_sel:    PLLSEL_BBPLL = 0x1,
									PLLSEL_RFPLL=0x2,
									PLLSEL_ORFPLL = 0x4,
									PLLSEL_ALL = 0x7,
 * @param {uint8_t} type_tx_or_rx: 0x1: RX 		0x4:tx
 * @param {CHANNEL_t} ch: 0x1: ch1 		0x2:ch2
 * @param {uint8_t} reset_flag: 0x0: 保留原来的配置 		0x1：重置原来同步配置，只配置当前配置
 */
uint32_t FR9009_armSpiCmd_sync_type_ch_set(fr9009Device_t *device, uint8_t sync_pll_sel, uint8_t type_tx_or_rx, CHANNEL_t ch, uint8_t reset_flag)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];
	cmdbuf[0] = (type_tx_or_rx & 0xF) | ((sync_pll_sel & 0xF) << 4) | (ch << 8) | (reset_flag << 12);
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_SYNC | (SYNC_FUNC_MODE_TYPE << 16), cmdbuf, 1);
	IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
	IF_ERR_RETURN_U32(retVal);
	return retVal;
}

/**
 * @description: 
 * @return {*}
 * @param {fr9009Device_t} *device
 * @param {uint8_t} enable: 0:diable 1:enable, 0x3: only single device internal sync
 */
uint32_t FR9009_armSpiCmd_sync_dig_en(fr9009Device_t *device, uint8_t enable)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];
	int8_t delay_preset = 1;
	uint8_t is_single_dev = 0;
	CHANNEL_TYPE_t chan_type = 0;
	cmdbuf[0] = (enable & 0x1) | ((is_single_dev & 0x1) << 1) | ((delay_preset & 0xFF) << 4) | ((chan_type & 0xF) << 12);
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_SYNC | (SYNC_FUNC_MODE_DIG_EN << 16), cmdbuf, 1);
	IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
	IF_ERR_RETURN_U32(retVal);
	return retVal;
}
uint32_t FR9009_armSpiCmd_sync_state_get(fr9009Device_t *device, uint32_t *pstate)
{
    fRHALErr_t halError = FRHAL_OK;
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[3];
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_SYNC | (SYNC_FUNC_MODE_STATE << 16), NULL, 0);
	IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
	IF_ERR_RETURN_U32(retVal);
	halError = brSpiBlockReadWords(device->devHalInfo, FR9009_ADDR_CMDBUF_EXTDATA, &cmdbuf[1], 1);
	retVal = talApiErrHandler(device,FR_ERRHDL_HAL_SPI, halError, retVal, TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);
	*pstate = cmdbuf[1];
	return retVal;
}

uint32_t FR9009_armSpiCmd_sync_temp_track_en(fr9009Device_t *device, uint8_t enable, int32_t tempFactor)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];
	cmdbuf[0] = (enable & 0xF) | (tempFactor << 16);
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_SYNC | (SYNC_FUNC_MODE_TIMEP_TRACK_SET << 16), cmdbuf, 1);
	IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
	IF_ERR_RETURN_U32(retVal);
	return retVal;
}
/**
 * @description: 同步完成之后关闭同步控制字
 * @return {*}
 * @param {fr9009Device_t} *device
 * @param {uint8_t} enable: 1-enabel turn off 0-diable turn off
 */
uint32_t FR9009_armSpiCmd_sync_offAfterSync_set(fr9009Device_t *device, uint8_t enable)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];
	cmdbuf[0] = (enable & 0xF);
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_SYNC | (SYNC_FUNC_MODE_FLAG_OFF_SET << 16), cmdbuf, 1);
	IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
	IF_ERR_RETURN_U32(retVal);
	return retVal;
}
/**
 * @description: 
 * @return {*}
 * @param {fr9009Device_t} *device
 * @param {uint8_t} enable	0: disable 1: enable
 * @param {uint8_t} ch_type : 0x1: RX 	0x2:orx		0x4:tx
 * @param {uint8_t} nco_type : 0 ifconv gpio sync 0x8: mix nco hw sync
 */
uint32_t FR9009_armSpiCmd_sync_nco(fr9009Device_t *device, uint8_t ch_type, uint8_t enable, uint8_t nco_type)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];
	cmdbuf[0] = (enable & 0xF) | (nco_type << 8) | (ch_type << 4);
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_SYNC | (SYNC_FUNC_MODE_NCO_SYNC << 16), cmdbuf, 1);
	IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
	IF_ERR_RETURN_U32(retVal);
	return retVal;
}
/**
 * @description: 
 * @return {*}
 * @param {fr9009Device_t} *device
 * @param {uint8_t} mode:
						FASTHOP_MODE_OFF            = 0,
						FASTHOP_MODE_PLL_PINHPONG   = 1,
						FASTHOP_MODE_PLL_ONE ,
						FASTHOP_MODE_NCO,
 * @param {uint8_t} type: 0x1: RX 	0x2:orx		0x4:tx
 * @param {CHANNEL_t} ch
 * @param {uint32_t} switch_freq_hz : 跳频切换频率
 * @param {uint32_t} switch_nums ：跳频数量
 * @param {uint32_t} freq_min_khz
 * @param {uint32_t} freq_max_khz
 */
uint32_t FR9009_armSpiCmd_fasthop_init(fr9009Device_t *device,uint8_t mode, uint8_t type, CHANNEL_t ch, uint32_t switch_freq_hz, uint32_t switch_nums, uint32_t freq_min_khz, uint32_t freq_max_khz)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[5];
	cmdbuf[0] = (mode & 0xFF) | (type << 8) | (ch << 12);
	cmdbuf[1] = switch_freq_hz;
	cmdbuf[2] = switch_nums;
	cmdbuf[3] = freq_min_khz;
	cmdbuf[4] = freq_max_khz;
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_FASTHOP | (FASTHOP_FUNC_MODE_INIT << 16), cmdbuf, 5);
	IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
	IF_ERR_RETURN_U32(retVal);
	return retVal;
}
/**
 * @description: 
 * @return {*}
 * @param {fr9009Device_t} *device
 * @param {uint8_t} pllsel:
									PLLSEL_RFPLL=0x2,
									PLLSEL_ORFPLL = 0x4,
									PLLSEL_ALL = 0x6,
 * @param {int32_t} *freq_khz : 跳频频点
 * @param {uint16_t} nums：多少跳频点
 */
uint32_t FR9009_armSpiCmd_fasthop_freqs_set(fr9009Device_t *device, uint8_t pllsel, int32_t *freq_khz, uint16_t nums)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[100];
	uint32_t i =0;
	if(nums > 99){
		return FR_ERR_SPI_CMD_ERR_PARAM;
	}
	cmdbuf[0] = (pllsel & 0xF) | (nums << 16);
	for(i= 0; i < nums; i++){
		cmdbuf[i + 1] = freq_khz[i];
	}
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_FASTHOP | (FASTHOP_FUNC_MODE_SET_FREQ << 16), cmdbuf, nums + 1);
	IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
	IF_ERR_RETURN_U32(retVal);
	return retVal;
}

uint32_t FR9009_armSpiCmd_fasthop_start(fr9009Device_t *device, uint8_t en)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];
	cmdbuf[0] = (en & 0x1);
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_FASTHOP | (FASTHOP_FUNC_MODE_ONOFF << 16), cmdbuf, 1);
	IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
	IF_ERR_RETURN_U32(retVal);
	return retVal;
}
/**
 * @description: 
 * @return {*}
 * @param {fr9009Device_t} *device
 * @param {uint32_t} *param [fasthop_duration，fasthop_fcwChgOffset，fasthop_pllResetHighOffset，fasthop_pllResetLowOffset，fasthop_pllSwitchOffset]
 * @param {uint8_t} offset 可设置偏移参数
 * @param {uint8_t} nums
 */
uint32_t FR9009_armSpiCmd_fasthop_param_set(fr9009Device_t *device, uint32_t *param, uint8_t offset, uint8_t nums)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[6];
	uint32_t i =0;
	if(nums > 5){
		return FR_ERR_SPI_CMD_ERR_PARAM;
	}
	cmdbuf[0] = (nums & 0xFF) | (offset << 8);
	for(i= 0; i < nums; i++){
		cmdbuf[i + 1] = param[i];
	}
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_FASTHOP | (FASTHOP_FUNC_MODE_PARAM_SET << 16), cmdbuf, nums + 1);
	IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
	IF_ERR_RETURN_U32(retVal);
	return retVal;
}
/**
 * @description: 
 * @return {*}
 * @param {fr9009Device_t} *device
 * @param {uint8_t} pllsel
									PLLSEL_RFPLL=0x2,
									PLLSEL_ORFPLL = 0x4,
									PLLSEL_ALL = 0x6,
 * @param {fr9009FhmTriggerMode_t} trigger_mode
 * @param {uint8_t} pin_num
 */
uint32_t FR9009_armSpiCmd_fasthop_trigger_set(fr9009Device_t *device, uint8_t pllsel, fr9009FhmTriggerMode_t trigger_mode, uint8_t pin_num)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];
	if(pllsel & 0x9){
		return FR_ERR_SPI_CMD_ERR_PARAM;
	}
	cmdbuf[0] = (trigger_mode & 0xFF) | (pin_num << 8) | ((pllsel & 0x6) << 16);
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_FASTHOP | (FASTHOP_FUNC_TRIGGER_MODE_SET << 16), cmdbuf, 1);
	IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
	IF_ERR_RETURN_U32(retVal);
	return retVal;
}
/**
 * @description: 
 * @return {*}
 * @param {fr9009Device_t} *device
 * @param {uint8_t} pllsel:
									PLLSEL_RFPLL=0x2,
									PLLSEL_ORFPLL = 0x4,
									PLLSEL_ALL = 0x6,
 * @param {uint64_t} freq_hz 设置下一个跳频频率，如果值为0，则执行跳频表中的下一个，不为0，则立即改为设置频率
 */
uint32_t FR9009_armSpiCmd_fasthop_next_freq_set(fr9009Device_t *device, uint8_t pllsel, uint64_t freq_hz)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[3];
	cmdbuf[0] = (freq_hz >> 32) & 0xFFFFFFFF;
	cmdbuf[1] = freq_hz & 0xFFFFFFFF;
	cmdbuf[2] = pllsel & 0x6;
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_FASTHOP | (FASTHOP_FUNC_MODE_NEXT_FREQ_SET << 16), cmdbuf, 3);
	IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
	IF_ERR_RETURN_U32(retVal);
	return retVal;
}

uint32_t FR9009_armSpiCmd_fasthop_current_freq_get(fr9009Device_t *device, uint64_t *freq_hz)
{
    fRHALErr_t halError = FRHAL_OK;
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[6];
	if(freq_hz == NULL){
		return FR_ERR_SPI_CMD_ERR_PARAM;		
	}
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_SYNC | (SYNC_FUNC_MODE_STATE << 16), NULL, 0);
	IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
	IF_ERR_RETURN_U32(retVal);
	halError = brSpiBlockReadWords(device->devHalInfo, FR9009_ADDR_CMDBUF_EXTDATA, &cmdbuf[0], 5);
	retVal = talApiErrHandler(device,FR_ERRHDL_HAL_SPI, halError, retVal, TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);
	if(cmdbuf[0] & 1){
		*freq_hz = ((uint64_t)cmdbuf[3] << 32) | cmdbuf[4];
	}else{
		*freq_hz = ((uint64_t)cmdbuf[1] << 32) | cmdbuf[2];
	}
	return retVal;
}
/**
 * @description: 配置需要跳频的通道
 * @return {*}
 * @param {fr9009Device_t} *device
 * @param {CHANNEL_t} rx_ch	，设置rx需要跳频的通道， 0 不进行跳频
 * @param {CHANNEL_t} orx_ch： 同rx
 * @param {CHANNEL_t} tx_ch： 同rx
 */
uint32_t FR9009_armSpiCmd_fasthop_ch_set(fr9009Device_t *device, CHANNEL_t rx_ch, CHANNEL_t orx_ch, CHANNEL_t tx_ch)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];
	cmdbuf[0] = (rx_ch & 0x3) | ((orx_ch & 0x3) << 2) | ((tx_ch & 0x3) << 4);
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_FASTHOP | (FASTHOP_FUNC_MODE_FHM_CH_SET << 16), cmdbuf, 1);
	IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
	IF_ERR_RETURN_U32(retVal);
	return retVal;
}

/**
 * @description: 
 * @return {*}
 * @param {fr9009Device_t} *device
 * @param {CHANNEL_t} tx_ch:设置tx需要跳频的通道
 * @param {uint8_t} bandrate:宽带信号宽度占当前IQrate的比例，eg：80即为百分之80
 * @param {uint8_t} power_db：信号功率，eg：10及-10dbfs
 * @param {uint8_t} enable：0：关闭， 1：使能
 */
uint32_t FR9009_armSpiCmd_tx_play_interofdm(fr9009Device_t *device, CHANNEL_t tx_ch, uint8_t bandrate, uint8_t power_db, uint8_t enable)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];
	cmdbuf[0] = (tx_ch & 0x3) | ((enable & 0x1) << 4) | ((bandrate & 0xFF) << 8) | ((power_db & 0xFF) << 16);
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_DEBUG | (DEBUG_TX_PLAY_OFDM << 16), cmdbuf, 1);
	IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
	IF_ERR_RETURN_U32(retVal);
	return retVal;
}
/**
 * @description: 
 * @return {*}
 * @param {fr9009Device_t} *device
 * @param {CHANNEL_t} rx_ch
 * @param {uint8_t} type：1 递增数据， 2:单次play脉冲
 * @param {uint8_t} enable	:0：关闭， 1：使能play功能
 */
uint32_t FR9009_armSpiCmd_rx_play_inter(fr9009Device_t *device, CHANNEL_t rx_ch, uint8_t type, uint8_t enable)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];
	cmdbuf[0] = (rx_ch & 0x3) | ((enable & 0x1) << 4) | ((type & 0xF) << 8);
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_DEBUG | (DEBUG_RX_PLAY_WAVE << 16), cmdbuf, 1);
	IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
	IF_ERR_RETURN_U32(retVal);
	return retVal;
}
/**
 * @description: 
 * @return {*}
 * @param {fr9009Device_t} *device
 * @param {CHANNEL_t} rx_ch	CHANNEL_1, CHANNEL_2
 * @param {uint8_t} cap_point 具体见CAPTURE_Src_t
	typedef enum{
		RX_TAP_RHB3_CH1_OUT		= 0x10,
		RX_TAP_RHB3_CH2_OUT    = 0x11, 
		RX_TAP_RHB2_OUT        = 0x12, 
		RX_TAP_DEC_CH1_OUT     = 0x13, 
		RX_TAP_DEC_CH2_OUT     = 0x14, 
		RX_TAP_RHB0_OUT        = 0x15, 
		RX_TAP_RCFIR_OUT       = 0x16, 
		RX_TAP_PHSCMPS_OUT     = 0x17, 
		RX_TAP_QFIR_OUT        = 0x18, 
		RX_TAP_QCFIR_OUT       = 0x19, 
		RX_TAP_CPLXFIR_OUT     = 0x1A, 
		RX_TAP_DIG_GAIN_OUT    = 0x1B, 
		RX_TAP_DC_CORR_OUT     = 0x1C, 
		RX_TAP_IFCONV_OUT      = 0x1D, 
		RX_TAP_FPFMT_LSB_OUT   = 0x1E, 
		RX_TAP_FPFMT_MSB_OUT   = 0x1F,
	}CAPTURE_Src_t;
 * @param {int32_t} left_freq_hz
 * @param {int32_t} right_freq_hz
 * @param {int32_t} *pPower_mdbfs
 */
uint32_t FR9009_armSpiCmd_rx_power_get(fr9009Device_t *device, CHANNEL_t rx_ch, uint8_t cap_point, int32_t left_freq_hz, int32_t right_freq_hz, int32_t *pPower_mdbfs)
{
    fRHALErr_t halError = FRHAL_OK;
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint8_t select_AB = 0x1;
	uint32_t cmdbuf[3];
	cmdbuf[0] = (select_AB ) | ((rx_ch & 0x3) << 2) | ((cap_point & 0xFF) << 8);
	cmdbuf[1] = left_freq_hz;
	cmdbuf[2] = right_freq_hz;
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_DEBUG | (DEBUG_RX_POWER_GET << 16), cmdbuf, 3);
	IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
	IF_ERR_RETURN_U32(retVal);
	halError = brSpiBlockReadWords(device->devHalInfo, FR9009_ADDR_CMDBUF_EXTDATA, &cmdbuf[1], 2);
	retVal = talApiErrHandler(device,FR_ERRHDL_HAL_SPI, halError, retVal, TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);
	if(pPower_mdbfs){
		*pPower_mdbfs = cmdbuf[1];
	}
	return retVal;
}


uint32_t FR9009_armSpiCmd_setrf_freq(fr9009Device_t *device, uint32_t freq_KHz)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];

	cmdbuf[0] = freq_KHz;
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_SETRF_FREQ_NEW, cmdbuf, 1);
	IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
	IF_ERR_RETURN_U32(retVal);
	return retVal;
}

/*param:
**fr9009Device_t *device：fr9009Device_t的指针，使用此结构体解析
**ch1_sync_q：0-3 四档位调整
**ch2_sync_q：0-3 四档位调整
**
*/
uint32_t FR9009_armSpiCmd_SyncAdc_Q(fr9009Device_t *device, uint8_t ch1_sync_q, uint8_t ch2_sync_q)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];

    cmdbuf[0] = (ch1_sync_q & 0xf) | ((ch2_sync_q << 4) & 0xF0);
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_SYNC_ADC_Q, cmdbuf, 1);
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
    IF_ERR_RETURN_U32(retVal);
	return retVal;
}

uint32_t FR9009_armSpiCmd_pll_freq_set(fr9009Device_t *device, fr9009RfPllName_t pll_type, uint64_t freq_Hz)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	// uint32_t cmdstatword;
	uint32_t cmdbuf[2];
	if(pll_type > 2){
		return -1;
	}
	cmdbuf[0] = (freq_Hz >> 32) & 0xFFFFFFFF;
	cmdbuf[1] = freq_Hz & 0xFFFFFFFF;
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_PLL_FREQ | (pll_type << 16), cmdbuf, 2);
	IF_ERR_RETURN_U32(retVal);
	// retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
	// IF_ERR_RETURN_U32(retVal);
	return retVal;
}
/**
 * @description: pll 的开关控制
 * @return {*}
 * @param {fr9009Device_t} *device
 * @param {fr9009RfPllName_t} pll_type： 1：RF 2： ORF
 * @param {uint8_t} on_off：1：开启 0：关闭
 */
uint32_t FR9009_armSpiCmd_pll_OnOff_set(fr9009Device_t *device, fr9009RfPllName_t pll_type, uint8_t on_off)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];
	if(pll_type > 2){
		return -1;
	}
	cmdbuf[0] = 1 << pll_type |  (on_off & 0xf) << 4;
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_PLL_FREQ | (0x4 << 16), cmdbuf, 1);
	IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
	IF_ERR_RETURN_U32(retVal);
	return retVal;
}
/**
 * @description: 
 * @return {*}
 * @param {fr9009Device_t} *device
 * @param {uint8_t} pllsel	: [2:0]分别是BBPLL、RFPLL、ORFPLL三个锁相环的温度补偿开关，相应bit 1：开启 0：不开启
 */
uint32_t FR9009_armSpiCmd_pll_tempratureEn_set(fr9009Device_t *device, uint8_t pllsel)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];
	cmdbuf[0] = pllsel;
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_PLL_FREQ | (0x5 << 16), cmdbuf, 1);
	IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
	IF_ERR_RETURN_U32(retVal);
	return retVal;
}

/**
 * @description: 
 * @return {*}
 * @param {fr9009Device_t} *device
 * @param {uint8_t} pllsel	: [2:0]分别是BBPLL、RFPLL、ORFPLL三个锁相环的温度补偿开关，相应bit 1：开启 0：不开启
 * @param {uint8_t} bb_gpio_num	: gpio num 
 */
uint32_t FR9009_armSpiCmd_pll_status_gpio_set(fr9009Device_t *device, uint8_t pllsel, uint8_t bb_gpio_num, uint8_t rf_gpio_num, uint8_t orf_gpio_num)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];
	cmdbuf[0] = pllsel | ((bb_gpio_num & 0x1f) << 4) | ((rf_gpio_num & 0x1f) << 10) | ((orf_gpio_num & 0x1f) << 16);
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_PLL_FREQ | (0x6 << 16), cmdbuf, 1);
	IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
	IF_ERR_RETURN_U32(retVal);
	return retVal;
}

/**
 * @description: 
 * @return {*}
 * @param {fr9009Device_t} *device
 * @param {uint8_t} pllsel	: [2:0]分别是ORFPLL、RFPLL、BBFPLL三个锁相环的设置锁定是加频偏，相应bit 1：开启 0：不开启
 * @param {uint8_t} pllFAFCsel	: [2:0]分别是ORFPLL、RFPLL、BBPLL三个锁相环的锁定方式，相应bit 1：FAFC 0：AFC
 * @param {uint8_t} pllDither	: [2:0]分别是ORFPLL、RFPLL、BBPLL三个锁相环的锁定方式，相应bit 1：Dither off 0：Dither on
 * @param {uint8_t} opt	: 暂时无意义
 */
uint32_t FR9009_armSpiCmd_pll_Opt_set(fr9009Device_t *device, uint8_t pllsel, uint8_t pllFAFCsel, uint8_t pllDither, uint8_t opt)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];
	cmdbuf[0] = pllsel | pllFAFCsel << 4 | pllDither << 8;
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_PLL_FREQ | (0x7 << 16), cmdbuf, 1);
	IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
	IF_ERR_RETURN_U32(retVal);
	return retVal;
}

uint32_t FR9009_armSpiCmd_pll_freq_get(fr9009Device_t *device, fr9009RfPllName_t pll_type, uint64_t *freq_Hz)
{
    fRHALErr_t halError = FRHAL_OK;
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[3];

	if(pll_type > 2 || freq_Hz == NULL){
		return -1;
	}

	cmdbuf[0] = (pll_type & 0x3) | 0x08;
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_PLL_FREQ | (((pll_type & 0x3) | 0x08) << 16), NULL, 0);
	IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
	IF_ERR_RETURN_U32(retVal);
	halError = brSpiBlockReadWords(device->devHalInfo, FR9009_ADDR_CMDBUF_EXTDATA, &cmdbuf[1], 2);
	retVal = talApiErrHandler(device,FR_ERRHDL_HAL_SPI, halError, retVal, TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);
	*freq_Hz = cmdbuf[1];
	*freq_Hz = (*freq_Hz << 32) | cmdbuf[2];
	return retVal;
}
/**
 * @description: vco 输入输出配置
 * @return {*}
 * @param {fr9009Device_t} *device
 * @param {VCOConfig_t} vco_type 参见VCOConfig_t
 * @param {uint8_t} div_n	vco 配置为外部输入时有效， 分频比为 1<<div_n
 * @param {uint64_t} vco_freq_hz vco 配置为外部输入时有效
 * @param {uint8_t} en
 */
uint32_t FR9009_armSpiCmd_VCOCfg_set(fr9009Device_t *device, VCOConfig_t vco_type, uint8_t div_n, uint64_t vco_freq_hz, uint8_t en)
{
    fRHALErr_t halError = FRHAL_OK;
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[3];

	cmdbuf[0] = (vco_freq_hz >> 32) & 0xFFFFFFFF;
	cmdbuf[1] = vco_freq_hz & 0xFFFFFFFF;
	cmdbuf[2] = (vco_type & 0xff) | ((div_n & 0xf) << 8) | ((en & 0x1) << 12);
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_PLL_FREQ | (VCO_CFG_SET << 16), cmdbuf, 3);
	IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
	IF_ERR_RETURN_U32(retVal);
	return retVal;
}
/**
 * @description: 
 * @return {*}
 * @param {fr9009Device_t} *device
 * @param {uint8_t} pllsel 0x2: RFPLL 0x4:ORFPLL 0xC: RF&ORF PLL
 * @param {uint8_t} en 0: off 1: enable
 */
uint32_t FR9009_armSpiCmd_PllMerge_set(fr9009Device_t *device, uint8_t pllsel, uint8_t en)
{
    fRHALErr_t halError = FRHAL_OK;
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];

	cmdbuf[0] = (pllsel & 0xf) | ((en & 0xf) << 4);
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_PLL_FREQ | (PLL_MERGE_SET << 16), cmdbuf, 1);
	IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
	IF_ERR_RETURN_U32(retVal);
	return retVal;
}

/*param:
**fr9009Device_t *device：fr9009Device_t的指针，使用此结构体解析
**CHANNEL_t ch：0x01:ch1，0x02:ch2
*/
uint32_t FR9009_armSpiCmd_AGC_CALI(fr9009Device_t *device, CHANNEL_t ch)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];

	cmdbuf[0] = (ch << 4) & 0x30;
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_AGC_CALI, cmdbuf, 1);
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, 100000000, WAITCOMMONARMCMD_INTERVAL_US);
    IF_ERR_RETURN_U32(retVal);
	return retVal;
}


/********************************FR9009_ES2_API SpiCmd update********************************/
/**实现tx/rx/orx通道的平坦度校准**/
/*param:
**fr9009Device_t *device：fr9009Device_t的指针，使用此结构体解析
**CHANNEL_t cali_ch：0x01—ch1，0x02—ch2
**uint8_t tx_rx_orxsel：0x1，0x2:校准rx orx ， 0x4：校准tx
*/
uint32_t FR9009_armSpiCmd_cali_cplxfir(fr9009Device_t *device, CHANNEL_t cali_ch, uint8_t tx_rx_orxsel)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];

	cmdbuf[0] = (cali_ch & 0x3) | ((tx_rx_orxsel << 8) & 0xF00);
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_CALI_CPLXFIR, cmdbuf, 1);
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
    IF_ERR_RETURN_U32(retVal);
	return retVal;
}

/**实现捕获模式设置**/
/*param:
**fr9009Device_t *device：fr9009Device_t的指针，使用此结构体解析
**uint8_t mode：0-play模式, 1-capture模式
**uint8_t continue_mode：0:单次工作模式, 1:连续工作模式
**uint8_t start_pluse:启动信号选择，0-使用manual_start作为启动信号，1-使用TDD控制信号作为启动信号
**uint8_t sel_AB:0x01—capture A, 0x02—capture B, 0x03—capture A&B
**uint8_t div:0-DIV_1, 1-DIV_2, 2-DIV_4, 3-DIV_8, 4-DIV_16, 5-DIV_32, 6-DIV_64
**CHANNEL_t ch：0x01—ch1, 0x02—ch2
**uint16_t src：捕获信号源选择, 0x0~0x1F, 具体见CAPTURE_Src_t
*/
uint32_t FR9009_armSpiCmd_playcap_set(fr9009Device_t *device, uint8_t mode, uint8_t continue_mode, uint8_t start_pluse, uint8_t sel_AB, uint8_t div, CHANNEL_t ch, uint16_t src)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];

	cmdbuf[0] = (mode & 0xf) 
				| ((continue_mode << 4) & 0xF0) 
				| ((start_pluse << 5) & 0x20) 
				| ((sel_AB << 6) & 0xC0) 
				| ((div << 8) & 0xF00) 
				| ((ch << 12) & 0xF000) 
				| ((src << 16) & 0xFF0000);
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_PLAYCAP_SET, cmdbuf, 1);
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
    IF_ERR_RETURN_U32(retVal);
	return retVal;
}

/**实现捕获模式的选择**/
/*param:
**fr9009Device_t *device：fr9009Device_t的指针，使用此结构体解析
**uint8_t start_ab：0x01—capture A,0x02—capture B,0x03—capture A&B
*/
uint32_t FR9009_armSpiCmd_playcap_manual_start(fr9009Device_t *device, uint8_t start_ab)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];

	cmdbuf[0] = (start_ab << 6) & 0xC0;
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_PLAYCAP_MANUAL_START, cmdbuf, 1);
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
    IF_ERR_RETURN_U32(retVal);
	return retVal;
}
/**实现TDD/manual mode**/
/*param:
**fr9009Device_t *device：fr9009Device_t的指针，使用此结构体解析
**uint8_t mode: 1:TDD 0:manual
*/
uint32_t FR9009_armSpiCmd_TddManualsel(fr9009Device_t *device, uint8_t mode)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];

    cmdbuf[0] = (0x7 << 8) | (0x3 << 4) | (mode & 0xF);
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_TDD_MANUAL_MODE_SET, cmdbuf, 1);
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
    IF_ERR_RETURN_U32(retVal);
	return retVal;
}

/**实现rx/orx通道使能**/
/*param:
**fr9009Device_t *device：fr9009Device_t的指针，使用此结构体解析
**fr9009RxORxChannels_t ch：0x01—rx1,0x02—rx2,0x03—rx1&rx2,0x04—orx1,0x08—orx2,0x0c—orx1&orx2
*/
uint32_t FR9009_armSpiCmd_orxrx_ch_en(fr9009Device_t *device, fr9009RxORxChannels_t ch)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];

    cmdbuf[0] = ch & 0xF;
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_ORXRX_CH_EN, cmdbuf, 1);
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
    IF_ERR_RETURN_U32(retVal);
	return retVal;
}

// /**实现rx/orx通道带宽设置**/
// /*param:
// **fr9009Device_t *device：fr9009Device_t的指针，使用此结构体解析
// **uint8_t bw：0—窄带200Mhz/245.76Msps，1—宽带450Mhz/491.52Msps
// */
// uint32_t FR9009_armSpiCmd_orxrx_bw_set(fr9009Device_t *device, uint8_t bw)
// {
// 	talRecoveryActions_t retVal = TALACT_NO_ACTION;
// 	uint32_t cmdstatword;
// 	uint32_t cmdbuf[1];
// 	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
//     IF_ERR_RETURN_U32(retVal);
//     cmdbuf[0] = ((bw<<4)&0x70);
// 	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_ORXRX_BW_SET), cmdbuf, 1);
//     IF_ERR_RETURN_U32(retVal);
// 	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
//     IF_ERR_RETURN_U32(retVal);
// 	return retVal;
// }

/**实现rx/orx通道增益设置**/
/*param:
**fr9009Device_t *device：fr9009Device_t的指针，使用此结构体解析
**CHANNEL_t ch：0x1: ch1 0x2:ch2	0x3: ch1&ch2
**uint32_t att_mdb : db*1000
*/
uint32_t FR9009_armSpiCmd_orx_gain_set(fr9009Device_t *device, CHANNEL_t ch, uint32_t att_mdb)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[2];

    cmdbuf[0] = ((ch << 2) & 0xF);
	cmdbuf[1] = att_mdb;
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_ORXCH_GAIN_SET_GET, cmdbuf, 2);
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
    IF_ERR_RETURN_U32(retVal);
	return retVal;
}

uint32_t FR9009_armSpiCmd_orx_gain_get(fr9009Device_t *device, CHANNEL_t ch, uint32_t *patt_mdb)
{
    fRHALErr_t halError = FRHAL_OK;
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[2];

    cmdbuf[0] = (ch << 2) & 0xF;
	cmdbuf[1] = 1 << 31;	//read flag
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_ORXCH_GAIN_SET_GET, cmdbuf, 2);
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
    IF_ERR_RETURN_U32(retVal);
	halError = brSpiBlockReadWords(device->devHalInfo, FR9009_ADDR_CMDBUF_EXTDATA, patt_mdb, 1);
	retVal = talApiErrHandler(device,FR_ERRHDL_HAL_SPI, halError, retVal, TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);
	return retVal;
}

/**tx通道使能**/
/*param:
**fr9009Device_t *device：fr9009Device_t的指针，使用此结构体解析
**uint8_t tx_ch：0x00—all disable，0x01—ch1，0x02—ch2，0x03—ch1&ch2
*/
uint32_t FR9009_armSpiCmd_tx_ch_en(fr9009Device_t *device, uint8_t tx_ch)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];

    cmdbuf[0] = tx_ch & 0x3;
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_TX_CH_EN, cmdbuf, 1);
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
    IF_ERR_RETURN_U32(retVal);
	return retVal;
}

/**tx通道ATT设置**/
/*param:
**fr9009Device_t *device：fr9009Device_t的指针，使用此结构体解析
**uint8_t tx_ch：0x00—all disable，0x01—ch1，0x02—ch2，0x03—ch1&ch2
**uint32_t att_mdb : DB = att_mdb / 1000
*/
uint32_t FR9009_armSpiCmd_tx_ch_ATT_set(fr9009Device_t *device, uint8_t tx_ch, uint32_t att_mdb)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[2];

    cmdbuf[0] = tx_ch & 0xF;
	cmdbuf[1] = att_mdb;
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_TX_CTRL_CONFIG | (TX_ATT_SET_GET << 16), cmdbuf, 2);
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
    IF_ERR_RETURN_U32(retVal);
	return retVal;
}

uint32_t FR9009_armSpiCmd_tx_ch_ATT_get(fr9009Device_t *device, uint8_t tx_ch, uint32_t *patt_mdb)
{
    fRHALErr_t halError = FRHAL_OK;
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[2];

    cmdbuf[0] = tx_ch & 0xF;
	cmdbuf[1] = 1 << 31;
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_TX_CTRL_CONFIG | (TX_ATT_SET_GET << 16), cmdbuf, 2);
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
    IF_ERR_RETURN_U32(retVal);
	halError = brSpiBlockReadWords(device->devHalInfo, FR9009_ADDR_CMDBUF_EXTDATA, patt_mdb, 1);
	retVal = talApiErrHandler(device,FR_ERRHDL_HAL_SPI, halError, retVal, TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);
	return retVal;
}
/**
 * @description: 设置tx PA 保护模式与配置
 * @return {*}
 * @param {fr9009Device_t} *device
 * @param {uint8_t} tx_ch： tx通道配置： 0x1 ch1；0x2 ch2；0x3:ch1&2
 * @param {uint8_t} detect_mode： 0x0： off； 0x1：power mode；0x2：peak mode
 * @param {int32_t} power_mdbfs： 设置保护门限功率： eg：6000 表示-6 dbfs
 * @param {uint32_t} duration_cycle：检测周期（单位为datapath clk）设置值建议为2^n，（0x4000）
 * @param {uint8_t} peak_errcnt：设置周期内检测异常peak个数进入保护状态，power mode无效
 */
uint32_t FR9009_armSpiCmd_tx_PowerCtrl_set(fr9009Device_t *device, uint8_t tx_ch, uint8_t detect_mode, int32_t power_mdbfs, uint32_t duration_cycle, uint8_t peak_errcnt)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[3];

    cmdbuf[0] = (tx_ch & 0xF) | (detect_mode << 4) | (peak_errcnt << 8);
	cmdbuf[1] = power_mdbfs;
	cmdbuf[2] = duration_cycle;
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_TX_CTRL_CONFIG | (TX_POWER_CTRL_SET << 16), cmdbuf, 3);
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
    IF_ERR_RETURN_U32(retVal);
	return retVal;
}
/**
 * @description: 
 * @return {*}
 * @param {fr9009Device_t} *device
 * @param {uint32_t} *pPowerErr: 返回power、peak err status：
 * 			bit0：ch1_powerOverRange 
 * 			bit1：ch1_peakOverRange 
 * 			bit2：ch2_powerOverRange
 * 			bit3：ch2_peakOverRange
 */
uint32_t FR9009_armSpiCmd_tx_PowerCtrl_ErrGet(fr9009Device_t *device, uint32_t *pPowerErr)
{
    fRHALErr_t halError = FRHAL_OK;
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;

	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_TX_CTRL_CONFIG | (TX_POWER_CTRL_ERR_GET << 16), NULL, 0);
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
    IF_ERR_RETURN_U32(retVal);

	halError = brSpiBlockReadWords(device->devHalInfo, FR9009_ADDR_CMDBUF_EXTDATA, pPowerErr, 1);
	retVal = talApiErrHandler(device,FR_ERRHDL_HAL_SPI, halError, retVal, TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);
	return retVal;
}

uint32_t FR9009_armSpiCmd_tx_PowerCtrl_ErrReset(fr9009Device_t *device, uint8_t tx_ch)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];

    cmdbuf[0] = tx_ch & 0xF;
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_TX_CTRL_CONFIG | (TX_POWER_CTRL_ERR_RESET << 16), cmdbuf, 1);
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
    IF_ERR_RETURN_U32(retVal);
	return retVal;
}
/**tx带宽选择**/
/*param:
**fr9009Device_t *device：fr9009Device_t的指针，使用此结构体解析
**uint8_t tx_ch：0x00—all disable，0x01—ch1，0x02—ch2，0x03—ch1&ch2
**uint8_t bw：1—窄带200Mhz/245.76Msps，0—宽带450Mhz/491.52Msps
*/
uint32_t FR9009_armSpiCmd_tx_bw_set(fr9009Device_t *device, CHANNEL_t tx_ch, uint8_t bw)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];

    cmdbuf[0] = (tx_ch&0x3) | ((bw<<4)&0x70);
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_CHANNEL_CONFIG | (CHANNEL_CONF_MODE_TX_BW_SET << 16), cmdbuf, 1);
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
    IF_ERR_RETURN_U32(retVal);
	return retVal;
}
/**rx带宽选择**/
/*param:
**fr9009Device_t *device：fr9009Device_t的指针，使用此结构体解析
**uint8_t tx_ch：0x00—all disable，0x01—ch1，0x02—ch2，0x03—ch1&ch2
**uint8_t bw：1—窄带200Mhz/245.76Msps，0—宽带450Mhz/491.52Msps
*/
uint32_t FR9009_armSpiCmd_rx_bw_set(fr9009Device_t *device, CHANNEL_t ch, uint8_t bw)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];

    cmdbuf[0] = (ch&0x3) | ((bw<<4)&0x70);
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_CHANNEL_CONFIG | (CHANNEL_CONF_MODE_RX_BW_SET << 16), cmdbuf, 1);
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
    IF_ERR_RETURN_U32(retVal);
	return retVal;
}

/**orx带宽选择**/
/*param:
**fr9009Device_t *device：fr9009Device_t的指针，使用此结构体解析
**uint8_t tx_ch：0x00—all disable，0x01—ch1，0x02—ch2，0x03—ch1&ch2
**uint8_t bw：1—窄带200Mhz/245.76Msps，0—宽带450Mhz/491.52Msps
*/
uint32_t FR9009_armSpiCmd_orx_bw_set(fr9009Device_t *device, CHANNEL_t ch, uint8_t bw)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];

    cmdbuf[0] = (ch&0x3) | ((bw<<4)&0x70);
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_CHANNEL_CONFIG | (CHANNEL_CONF_MODE_ORX_BW_SET << 16), cmdbuf, 1);
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
    IF_ERR_RETURN_U32(retVal);
	return retVal;
}
/**dec5 mode设置**/
/*param:
**fr9009Device_t *device：fr9009Device_t的指针，使用此结构体解析
**uint8_t Dec5DecimationEn: 0: dec4 1: dec5
*/
uint32_t FR9009_armSpiCmd_decmode_set(fr9009Device_t *device, uint8_t Dec5DecimationEn)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];

    cmdbuf[0] = Dec5DecimationEn&0x1;
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_CHANNEL_CONFIG | (CHANNEL_CONF_MODE_DEC_MODE_SET << 16), cmdbuf, 1);
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
    IF_ERR_RETURN_U32(retVal);
	return retVal;
}

/**lo set**/
/*param:
**fr9009Device_t *device：fr9009Device_t的指针，使用此结构体解析
**CHANNEL_t ch：0x00—all disable，0x01—ch1，0x02—ch2，0x03—ch1&ch2
**fr9009LoSource_t losel : 参见fr9009LoSource_t
*/
uint32_t FR9009_armSpiCmd_tx_Lo_set(fr9009Device_t *device, CHANNEL_t ch, fr9009LoSource_t losel)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];

    cmdbuf[0] = (ch & 0xF) | ((losel == FR_LO_RF_PLL? FR_LO_RF_PLL : FR_LO_AUX_PLL)  << 4);
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_CHANNEL_CONFIG | (CHANNEL_CONF_MODE_TX_LO_SET << 16), cmdbuf, 1);
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
    IF_ERR_RETURN_U32(retVal);
	return retVal;
}

/**lo set**/
/*param:
**fr9009Device_t *device：fr9009Device_t的指针，使用此结构体解析
**CHANNEL_t ch：0x00—all disable，0x01—ch1，0x02—ch2，0x03—ch1&ch2
**fr9009LoSource_t losel : 参见fr9009LoSource_t
*/
uint32_t FR9009_armSpiCmd_rx_Lo_set(fr9009Device_t *device, CHANNEL_t ch, fr9009LoSource_t losel)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];

    cmdbuf[0] = (ch & 0xF) | ((losel == FR_LO_RF_PLL? FR_LO_RF_PLL : FR_LO_AUX_PLL)  << 4);
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_CHANNEL_CONFIG | (CHANNEL_CONF_MODE_RX_LO_SET << 16), cmdbuf, 1);
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
    IF_ERR_RETURN_U32(retVal);
	return retVal;
}

/**lo set**/
/*param:
**fr9009Device_t *device：fr9009Device_t的指针，使用此结构体解析
**CHANNEL_t ch：0x00—all disable，0x01—ch1，0x02—ch2，0x03—ch1&ch2
**fr9009LoSource_t losel : 参见fr9009LoSource_t
*/
uint32_t FR9009_armSpiCmd_orx_Lo_set(fr9009Device_t *device, CHANNEL_t ch, fr9009LoSource_t losel)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];

    cmdbuf[0] = (ch & 0xF) | ((losel == FR_LO_RF_PLL? FR_LO_RF_PLL : FR_LO_AUX_PLL)  << 4);
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_CHANNEL_CONFIG | (CHANNEL_CONF_MODE_ORX_LO_SET << 16), cmdbuf, 1);
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
    IF_ERR_RETURN_U32(retVal);
	return retVal;
}

/**lo get**/
/*param:
**fr9009Device_t *device：fr9009Device_t的指针，使用此结构体解析
**CHANNEL_t ch：0x00—all disable，0x01—ch1，0x02—ch2，0x03—ch1&ch2
**fr9009LoSource_t losel : 指针指向lo选择存放地址，参见fr9009LoSource_t
*/
uint32_t FR9009_armSpiCmd_tx_Lo_get(fr9009Device_t *device, CHANNEL_t ch, fr9009LoSource_t *losel)
{
    fRHALErr_t halError = FRHAL_OK;
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];
	uint32_t reg = 0;

    cmdbuf[0] = (ch & 0xF);
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_CHANNEL_CONFIG | (CHANNEL_CONF_MODE_TX_LO_GET << 16), cmdbuf, 1);
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
    IF_ERR_RETURN_U32(retVal);
	halError = brSpiBlockReadWords(device->devHalInfo, FR9009_ADDR_CMDBUF_EXTDATA, &reg, 1);
	retVal = talApiErrHandler(device,FR_ERRHDL_HAL_SPI, halError, retVal, TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);
	*losel = reg & 0xf;
	return retVal;
}

/**lo get**/
/*param:
**fr9009Device_t *device：fr9009Device_t的指针，使用此结构体解析
**CHANNEL_t ch：0x00—all disable，0x01—ch1，0x02—ch2，0x03—ch1&ch2
**fr9009LoSource_t losel : 指针指向lo选择存放地址，参见fr9009LoSource_t
*/
uint32_t FR9009_armSpiCmd_rx_Lo_get(fr9009Device_t *device, CHANNEL_t ch, fr9009LoSource_t *losel)
{
    fRHALErr_t halError = FRHAL_OK;
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];
	uint32_t reg = 0;

    cmdbuf[0] = (ch & 0xF);
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_CHANNEL_CONFIG | (CHANNEL_CONF_MODE_RX_LO_GET << 16), cmdbuf, 1);
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
    IF_ERR_RETURN_U32(retVal);
	halError = brSpiBlockReadWords(device->devHalInfo, FR9009_ADDR_CMDBUF_EXTDATA, &reg, 1);
	retVal = talApiErrHandler(device,FR_ERRHDL_HAL_SPI, halError, retVal, TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);
	*losel = reg & 0xf;
	return retVal;
}

/**lo get**/
/*param:
**fr9009Device_t *device：fr9009Device_t的指针，使用此结构体解析
**CHANNEL_t ch：0x00—all disable，0x01—ch1，0x02—ch2，0x03—ch1&ch2
**fr9009LoSource_t losel : 指针指向lo选择存放地址，参见fr9009LoSource_t
*/
uint32_t FR9009_armSpiCmd_orx_Lo_get(fr9009Device_t *device, CHANNEL_t ch, fr9009LoSource_t *losel)
{
    fRHALErr_t halError = FRHAL_OK;
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];
	uint32_t reg = 0;

    cmdbuf[0] = (ch & 0xF) | ((losel == FR_LO_RF_PLL? FR_LO_RF_PLL : FR_LO_AUX_PLL)  << 4);
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_CHANNEL_CONFIG | (CHANNEL_CONF_MODE_ORX_LO_GET << 16), cmdbuf, 1);
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
    IF_ERR_RETURN_U32(retVal);
	halError = brSpiBlockReadWords(device->devHalInfo, FR9009_ADDR_CMDBUF_EXTDATA, &reg, 1);
	retVal = talApiErrHandler(device,FR_ERRHDL_HAL_SPI, halError, retVal, TALACT_ERR_RESET_SPI);
	IF_ERR_RETURN_U32(retVal);
	*losel = reg & 0xf;
	return retVal;
}
/**
 * @description: 
 * @return {*}
 * @param {fr9009Device_t} *device
 * @param {uint8_t} ch_type : 0x1: CHANNEL_RX  0x2: CHANNEL_ORX 0x4:CHANNEL_TX
 * @param {CHANNEL_t} ch:
 * @param {uint8_t} mode: 0:LIF off 1：mix 2：ifconv
 */
uint32_t FR9009_armSpiCmd_LIF_mode_set(fr9009Device_t *device, uint8_t ch_type, CHANNEL_t ch, uint8_t mode)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];
    cmdbuf[0] = (ch_type & 0xF) | ((ch & 0xF) << 4) | ((mode & 0x3)  << 8);
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_CHANNEL_CONFIG | (CHANNEL_CONF_MODE_LIF_MODE_SET << 16), cmdbuf, 1);
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
    IF_ERR_RETURN_U32(retVal);
	return retVal;
}

uint32_t FR9009_armSpiCmd_LIF_freq_set(fr9009Device_t *device, uint8_t ch_type, CHANNEL_t ch, int32_t freq_hz)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[2];
    cmdbuf[0] = (ch_type & 0xF) | ((ch & 0xF) << 4);
	cmdbuf[1] = freq_hz;
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_CHANNEL_CONFIG | (CHANNEL_CONF_MODE_LIF_FREQ_SET << 16), cmdbuf, 2);
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
    IF_ERR_RETURN_U32(retVal);
	return retVal;
}
/**
 * @description: 射频直采模式设置
 * @return {*}
 * @param {fr9009Device_t} *device
 * @param {uint8_t} ch_type : 0x1: CHANNEL_RX  0x2: CHANNEL_ORX 0x4:CHANNEL_TX
 * @param {CHANNEL_t} ch:
 * @param {uint8_t} mode: 0:RFDirect off 1：RFDirect mode
 */
uint32_t FR9009_armSpiCmd_RFDirect_mode_set(fr9009Device_t *device, uint8_t ch_type, CHANNEL_t ch, uint8_t mode)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];
    cmdbuf[0] = (ch_type & 0xF) | ((ch & 0xF) << 4) | ((mode & 0x3)  << 8);
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_CHANNEL_CONFIG | (CHANNEL_CONF_MODE_RFDIRECT_SET << 16), cmdbuf, 1);
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
    IF_ERR_RETURN_U32(retVal);
	return retVal;
}
/**
 * @description: 默认平坦度参数配置
 * @return {*}
 * @param {fr9009Device_t} *device
 * @param {uint8_t} ch_type : 0x1: CHANNEL_RX  0x2: CHANNEL_ORX 0x4:CHANNEL_TX
 * @param {CHANNEL_t} ch
 */
uint32_t FR9009_armSpiCmd_flatness_default_set(fr9009Device_t *device, uint8_t ch_type, CHANNEL_t ch)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];
    cmdbuf[0] = (ch_type & 0xF) | ((ch & 0xF) << 4);
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_CHANNEL_CONFIG | (CHANNEL_CONF_MODE_DEFAULT_FLATNESS_SET << 16), cmdbuf, 1);
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
    IF_ERR_RETURN_U32(retVal);
	return retVal;
}
/**
 * @description: 
 * @return {*}
 * @param {fr9009Device_t} *device
 * @param {uint8_t} ind: 可以预存的滤波器index，取值0-3
 * @param {uint8_t} symmetric：滤波器系数是否对称，0： 否 1：是
 * @param {uint8_t} even：奇偶
 * @param {uint8_t} fir_len： 滤波器系数长度
 * @param {int16_t} *coefs：滤波器系数
 */
uint32_t FR9009_armSpiCmd_cfir_coefs_set(fr9009Device_t *device, uint8_t ind, uint8_t symmetric, uint8_t even, uint8_t fir_len, int16_t *coefs)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[33];
	uint8_t len;
	if(fir_len < 1){
		return FR_ERR_SPI_CMD_ERR_PARAM;
	}
    cmdbuf[0] = (ind & 0xF) | ((symmetric ? 1 : 0) << 4) | ((even ? 1 : 0) << 5) |(((uint32_t)fir_len & 0xff) << 8);
	for(len = 0; len < (fir_len + 1) / 2; len++){
		if(2 * len + 1 >= fir_len){
			cmdbuf[len + 1] = (coefs[2 * len] & 0xFFFF);
		}else{
			cmdbuf[len + 1] = (coefs[2 * len] & 0xFFFF) | ((uint32_t)coefs[2 * len + 1] << 16);
		}
	}
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_CHANNEL_CONFIG | (CHANNEL_CONF_MODE_CFIR_COEFS_SET << 16), cmdbuf, len + 1);
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
    IF_ERR_RETURN_U32(retVal);
	return retVal;
}
/**
 * @description: 
 * @return {*}
 * @param {fr9009Device_t} *device
 * @param {uint8_t} ch_type 0x1:CHANNEL_RX, 0x2:CHANNEL_ORX, 0x4:CHANNEL_TX， bit掩码配置，可以多选
 * @param {CHANNEL_t} ch 0x01—ch1，0x02—ch2，0x03—ch1&ch2
 * @param {uint8_t} select_ind
 */
uint32_t FR9009_armSpiCmd_cfir_coefs_selInd_set(fr9009Device_t *device, uint8_t ch_type, CHANNEL_t ch, uint8_t select_ind)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];
	if(select_ind > 4){
		return FR_ERR_SPI_CMD_ERR_PARAM;
	}
    cmdbuf[0] = (ch_type & 0xF) | ((ch & 0xF) << 4)| ((select_ind & 0xF) << 8);
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_CHANNEL_CONFIG | (CHANNEL_CONF_MODE_CFIR_COEFS_IND_SET << 16), cmdbuf, 1);
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
    IF_ERR_RETURN_U32(retVal);
	return retVal;
}
/**设置 tx   NCO**/
/*param:
**fr9009Device_t *device：fr9009Device_t的指针，使用此结构体解析
**uint8_t nco_ch：0x00—nco disable，0x01—nco cha，0x02—nco chb，0x03—nco cha&chb
**uint8_t tx_ch: disable，0x01—配置tx1 ifconv，0x02—配置tx2 ifconv，0x03—配置tx1，2 ifconv
**int32_t freq1_khz：第一个频率值，单位KHz
**int32_t freq2_khz：第二个频率值，单位KHz
*/
uint32_t FR9009_armSpiCmd_tx_NCO_set(fr9009Device_t *device, uint8_t nco_ch, uint8_t tx_ch, int32_t freq1_khz, int32_t freq2_khz)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[3];

    cmdbuf[0] = (nco_ch & 0x3) | ((tx_ch << 4) & 0x30);
	cmdbuf[1] = freq1_khz;
	cmdbuf[2] = freq2_khz;
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_TX_NCO_SET, cmdbuf, 3);
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
    IF_ERR_RETURN_U32(retVal);
	return retVal;
}

/**设置 tx   NCO 功率**/
/*param:
**fr9009Device_t *device：fr9009Device_t的指针，使用此结构体解析
**uint8_t tx_ch：0x00—all disable，0x01—ch1，0x02—ch2，0x03—ch1&ch2
**int32_t powerda1：第一个功率值
**int32_t powerda2：第二个功率值
*/
uint32_t FR9009_armSpiCmd_tx_ncopower_set(fr9009Device_t *device, uint8_t tx_ch, int32_t powerda1, int32_t powerda2)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[3];

    cmdbuf[0] = (tx_ch << 4) & 0x30;
	cmdbuf[1] = powerda1 & 0xFFFF;
	cmdbuf[2] = powerda2 & 0xFFFF;
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_TX_NCOPOWER_SET, cmdbuf, 3);
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
    IF_ERR_RETURN_U32(retVal);
	return retVal;
}

/**设置 tx lo ripple**/
/*param:
**fr9009Device_t *device：fr9009Device_t的指针，使用此结构体解析
**uint8_t ch_type: 0x1:CHANNEL_RX, 0x2:CHANNEL_ORX, 0x4:CHANNEL_TX
**CHANNEL_t ch：0x01—ch1，0x02—ch2，0x03—ch1&ch2
**int32_t *wrDataBuf：指针变量，要写入到指定地址的32bit数据数组首地址
**uint32_t wrWordSz：取值32或16，32（或16）个平整度系数(对应32（或16）个校准频点)乘以10000后的整数值
*/
uint32_t FR9009_armSpiCmd_ch_signal_ripple_param_set(fr9009Device_t *device, uint8_t ch_type, CHANNEL_t ch, int32_t *wrDataBuf, uint32_t wrWordSz)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;

	uint32_t cmdBuf[FR9009_CMDBUF_WORDSIZE] = {0};
	uint32_t cmdBufSz = 0;	
	uint32_t cmdstatword;
	uint32_t paramBuf[1];

	paramBuf[0] = (ch & 0x3) | ((ch_type & 0xF) << 4) | ((wrWordSz & 0xFF) << 8);

	if(wrWordSz > RIPPLE_WRITE_LEN || wrWordSz > FR9009_CMDBUF_EXTDATA_SIZE){
		return talApiErrHandler(device, FR_ERRHDL_INVALID_PARAM, FR_ERR_NUMBER_OF_ERRORS, retVal, TALACT_ERR_CHECK_PARAM);
    }

	cmdBufSz = _cmdBufPack(cmdBuf, paramBuf, 1, (uint32_t*)wrDataBuf, wrWordSz);
	if(cmdBufSz < 0){
		return (uint32_t) retVal;
	}

	retVal = FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_TX_LO_RIPPLE_SET, cmdBuf, cmdBufSz);
	IF_ERR_RETURN_U32(retVal);		
	
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
	IF_ERR_RETURN_U32(retVal);

	return retVal;
}

/**测试函数**/
/*param:
**fr9009Device_t *device：fr9009Device_t的指针，使用此结构体解析
**int32_t freq_khz：频率，单位KHz
*/
uint32_t FR9009_armSpiCmd_test_func(fr9009Device_t *device, int32_t data)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t cmdstatword;
	uint32_t cmdbuf[1];

    cmdbuf[0] = data;
	retVal= FR9009_sendArmCommand(device, TALAPI_ARMSPI_CMD_TEST_FUNC, cmdbuf, 1);
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_waitArmCmdStatus(device, 0, &cmdstatword, WAITCOMMONARMCMD_TIMEOUT_US, WAITCOMMONARMCMD_INTERVAL_US);
    IF_ERR_RETURN_U32(retVal);
	return retVal;
}

int is_lsb(void)
{
	union test
	{
		unsigned char a;
		unsigned int b;
	}data;
	data.b = 0X12345678;
	if (data.a == 0X78)
		return 1;
	else
		return 0;

}
//无符号整型16位
uint16_t bswap_16bit(uint16_t x)
{
	if(is_lsb())
		return x;
	return (((x)& 0x00ff) << 8) | (((x)& 0xff00) >> 8);
}


//无符号整型32位
uint32_t bswap_32bit(uint32_t x)
{
	if(is_lsb())
		return x;
	return (((x) & 0xff000000) >> 24) | (((x)& 0x00ff0000) >> 8) | 
		(((x)& 0x0000ff00) << 8) |  (((x)& 0x000000ff) << 24);
}


uint32_t FR9009_armMemoryCmd_blk_write(fr9009Device_t *device, uint32_t startRegAddr, uint32_t *wrDataBuf, uint32_t  wrWordSz)
{
	return FR9009_armSpiCmd_writeWords(device, startRegAddr, wrDataBuf, wrWordSz);
}

uint32_t FR9009_armMemoryCmd_blk_read(fr9009Device_t *device, uint32_t startRegAddr, uint32_t *rdDataBuf, uint32_t rdWordSz)
{
	return FR9009_armSpiCmd_readWords(device, startRegAddr, rdDataBuf, rdWordSz);
}

uint32_t FR9009_armSpiCmd_SPI_blk_write(fr9009Device_t *device, uint8_t SPI_select, uint16_t regAddr, uint32_t *wrDataBuf, uint32_t  wrWordSz)
{
	return FR9009_armSpiCmd_writeWords(device, FR9009_ADDR_ARMSPI(SPI_select, regAddr), wrDataBuf, wrWordSz);
}

uint32_t FR9009_armSpiCmd_SPI_blk_read(fr9009Device_t *device, uint8_t SPI_select, uint16_t regAddr, uint32_t *rdDataBuf, uint32_t rdWordSz)
{
	return FR9009_armSpiCmd_readWords(device, FR9009_ADDR_ARMSPI(SPI_select, regAddr), rdDataBuf, rdWordSz);
}

uint32_t FR9009_armSpiCmd_mem_mask_write(fr9009Device_t *device, uint32_t val, uint32_t addr, uint32_t mask)
{
	return FR9009_armSpiCmd_writeField(device, addr, val, mask, 0);
}

uint32_t FR9009_armSpiCmd_spi_dev_mask_write(fr9009Device_t *device, uint32_t mask, uint32_t spi_sel, uint16_t regAddr, uint32_t val)
{
	return FR9009_armSpiCmd_writeField(device, FR9009_ADDR_ARMSPI(spi_sel, regAddr), val, mask, 0);
}

/*param:
**fr9009Device_t *device：fr9009Device_t的指针，使用此结构体解析
**uint8_t start_pluse:启动信号选择，0-使用manual_start作为启动信号，1-使用TDD控制信号作为启动信号
**uint8_t sel_AB:0x01—capture A(最大读取长度8192采样点), 0x02—capture B（最大读取长度12288采样点）
**uint8_t div:0-DIV_1, 1-DIV_2, 2-DIV_4, 3-DIV_8, 4-DIV_16, 5-DIV_32, 6-DIV_64, 根据带宽配置
**CHANNEL_t ch：0x01—ch1, 0x02—ch2
**uint16_t src：捕获信号源选择, 0x0~0x1F, 具体见CAPTURE_Src_t
typedef enum{
	TX_TAP_TXIN_OUT    = 0x0,
	TX_TAP_DGAIN_OUT   = 0x1,
	TX_TAP_IFCONV_OUT  = 0x2,
	TX_TAP_CPLXFIR_OUT = 0x3,
	TX_TAP_QCFIR_OUT   = 0x4,
	TX_TAP_QFIR_OUT    = 0x5,
	TX_TAP_PHSCMPS_OUT = 0x6,
	TX_TAP_CFIR_OUT    = 0x7,
	TX_TAP_DCOFFST_OUT = 0x8,
	TX_TAP_THB0_OUT    = 0x9,
	TX_TAP_THB1_CH1_OUT= 0xA,
	TX_TAP_THB1_CH2_OUT= 0xB,
	RX_TAP_RHB3_CH1_OUT		= 0x10,
	RX_TAP_RHB3_CH2_OUT    = 0x11, 
	RX_TAP_RHB2_OUT        = 0x12, 
	RX_TAP_DEC_CH1_OUT     = 0x13, 
	RX_TAP_DEC_CH2_OUT     = 0x14, 
	RX_TAP_RHB0_OUT        = 0x15, 
	RX_TAP_RCFIR_OUT       = 0x16, 
	RX_TAP_PHSCMPS_OUT     = 0x17, 
	RX_TAP_QFIR_OUT        = 0x18, 
	RX_TAP_QCFIR_OUT       = 0x19, 
	RX_TAP_CPLXFIR_OUT     = 0x1A, 
	RX_TAP_DIG_GAIN_OUT    = 0x1B, 
	RX_TAP_DC_CORR_OUT     = 0x1C, 
	RX_TAP_IFCONV_OUT      = 0x1D, 
	RX_TAP_FPFMT_LSB_OUT   = 0x1E, 
	RX_TAP_FPFMT_MSB_OUT   = 0x1F,
}CAPTURE_Src_t;
*/
uint32_t FR9009_armSpiCmd_cap(fr9009Device_t *device, uint8_t start_pluse, uint8_t sel_AB, uint8_t div, CHANNEL_t ch, uint16_t src, uint32_t *buf, uint32_t len)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	retVal = FR9009_armSpiCmd_playcap_set(device, 1, 0, start_pluse, sel_AB, div, ch, src);
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_armSpiCmd_playcap_manual_start(device, sel_AB);
    IF_ERR_RETURN_U32(retVal);
	FRHAL_wait_us(device->devHalInfo, 10000);
	retVal = FR9009_armSpiCmd_readWords(device, sel_AB ==  0x2 ? 0x20010000 : 0x20000000 , buf, len);
    IF_ERR_RETURN_U32(retVal);
	return retVal;
}

/*param:
**fr9009Device_t *device：fr9009Device_t的指针，使用此结构体解析
**uint8_t start_pluse:启动信号选择，0-使用manual_start作为启动信号，1-使用TDD控制信号作为启动信号
**uint8_t sel_AB:0x01—capture A(最大读取长度8192采样点), 0x02—capture B（最大读取长度12288采样点）
**uint8_t div:0-DIV_1, 1-DIV_2, 2-DIV_4, 3-DIV_8, 4-DIV_16, 5-DIV_32, 6-DIV_64, 根据带宽配置
**CHANNEL_t ch：0x01—ch1, 0x02—ch2
**uint16_t src：播放信号源选择, 0x0~0x1F, 具体见PLAY_Src_t
**uint32_t len：播放信号源长度，注意必须为偶数
typedef enum{
	TX_TAP_DGAIN_PLAY_MODE				= 0x00,
	TX_TAP_IFCONV_PLAY_MODE             = 0x01,
	TX_TAP_CPLXFIR_PLAY_MODE            = 0x02,
	TX_TAP_QCFIR_PLAY_MODE              = 0x03,
	TX_TAP_QFIR_PLAY_MODE               = 0x04,
	TX_TAP_PHSCMPS_PLAY_MODE            = 0x05,
	TX_TAP_CFIR_PLAY_MODE               = 0x06,
	TX_TAP_DCOFFST_PLAY_MODE            = 0x07,
	TX_TAP_THB0_PLAY_MODE               = 0x08,
	TX_TAP_INTP_PLAY_MODE               = 0x09,
	TX_TAP_THB2_CH1_PLAY_MODE           = 0x0A,
	TX_TAP_THB2_CH2_PLAY_MODE           = 0x0B,

	RX_TAP_RHB2_CH1_PLAY_MODE			= 0x10, 	
	RX_TAP_RHB2_CH2_PLAY_MODE           = 0x11, 
	RX_TAP_RHB1_CH1_PLAY_MODE           = 0x12, 
	RX_TAP_RHB0_CH1_PLAY_MODE           = 0x13, 
	RX_TAP_RCFIR_CH1_PLAY_MODE          = 0x14, 
	RX_TAP_PHSCMPS_CH1_PLAY_MODE        = 0x15, 
	RX_TAP_QFIR_CH1_PLAY_MODE           = 0x16, 
	RX_TAP_QCFIR_CH1_PLAY_MODE          = 0x17, 
	RX_TAP_CPLXFIR_CH1_PLAY_MODE        = 0x18, 
	RX_TAP_DIG_GAIN_CH1_PLAY_MODE       = 0x19, 
	RX_TAP_DC_CORR_CH1_PLAY_MODE        = 0x1A, 
	RX_TAP_IFCONV_CH1_PLAY_MODE         = 0x1B, 
	RX_TAP_FPFMT_CH1_PLAY_MODE          = 0x1C, 
	RX_TAP_RXOUT_CH1_LSB_PLAY_MODE      = 0x1D, 
	RX_TAP_RXOUT_CH1_MSB_PLAY_MODE      = 0x1E, 
}PLAY_Src_t;
*/	
uint32_t FR9009_armSpiCmd_play(fr9009Device_t *device, uint8_t start_pluse, uint8_t sel_AB, uint8_t div, CHANNEL_t ch, uint16_t src, uint32_t *buf, uint32_t len)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t regval  = (len*4 - 4) << 16;
	retVal = FR9009_armSpiCmd_playcap_set(device, 0, 1, start_pluse, sel_AB, div, ch, src);
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_armSpiCmd_writeWords(device, sel_AB ==  0x2 ? 0x20010000 : 0x20000000 , buf, len);
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_armSpiCmd_writeWords(device, APB_PLAYCAP_BASEADDR + (sel_AB ==  0x2 ? 0x1C : 0x18) , &regval, 1);
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_armSpiCmd_playcap_manual_start(device, sel_AB);
    IF_ERR_RETURN_U32(retVal);
	return retVal;
}
uint32_t FR9009_armSpiCmd_capplay_stop(fr9009Device_t *device)
{
	talRecoveryActions_t retVal = TALACT_NO_ACTION;
	uint32_t regvalbuf[]  = {0x7FFC0000, 0xbFFC0000};
	retVal = FR9009_armSpiCmd_playcap_set(device, 1, 0, 0, 0x1, 1, 0x1, 0);//tx 
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_armSpiCmd_playcap_set(device, 1, 0, 0, 0x2, 1, 0x1, 0x0);//tx
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_armSpiCmd_playcap_set(device, 1, 0, 0, 0x1, 1, 0x1, 0x10);//rx 
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_armSpiCmd_playcap_set(device, 1, 0, 0, 0x2, 1, 0x1, 0x10);//rx
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_armSpiCmd_playcap_set(device, 1, 0, 0, 0x1, 1, 0x2, 0);
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_armSpiCmd_playcap_set(device, 1, 0, 0, 0x2, 1, 0x2, 0);
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_armSpiCmd_playcap_set(device, 1, 0, 0, 0x1, 1, 0x2, 0x10);
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_armSpiCmd_playcap_set(device, 1, 0, 0, 0x2, 1, 0x2, 0x10);
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_armSpiCmd_writeWords(device, APB_PLAYCAP_BASEADDR + 0x18, regvalbuf, 2);
    IF_ERR_RETURN_U32(retVal);
	regvalbuf[0]=0;
	regvalbuf[1]=0;
	retVal = FR9009_armSpiCmd_writeWords(device, APB_TX_PATH_TOP_BASEADDR + 0x38, regvalbuf, 2);
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_armSpiCmd_writeWords(device, APB_RX_PATH_TOP_BASEADDR + 0x24, regvalbuf, 2);
    IF_ERR_RETURN_U32(retVal);
	retVal = FR9009_armSpiCmd_playcap_manual_start(device, 0x3);
    IF_ERR_RETURN_U32(retVal);
	return retVal;
}

