#include "headless.h"


/**
 * @description: 获取pll状态
 * @return {*}
 * @param {uint8_t} chip 目标芯片index
 * @param {fr9009RfPllName_t} pllName 目标pll或者Lo名称，参考结构体
 * @param {uint8_t} *pllStatus ：返回状态 0：not lock 1：locked
 */
int32_t FR_GetPLLStatus(uint8_t chip, fr9009RfPllName_t pllName, uint8_t *pllStatus)
{
	int ret = SUCCESS;
	uint8_t chip_nums = DEVICE_NUMS;
	fr9009Device_t  *device = 0;
	uint8_t pllLockStatus = 0;
	uint8_t pll_lock_mask = 1 << pllName;
    if(chip >= DEVICE_NUMS){
        UART_Printf("chip %d should < %d\r\n", chip, DEVICE_NUMS);
        goto error_1;
    }
    device = &brDev[chip];
    ret = FR9009_getPllsLockStatus(device, &pllLockStatus);
    if((pllLockStatus & pll_lock_mask) < pll_lock_mask){
        UART_Printf("chip %d lo 0x%x not lock 0x%08X\r\n", chip, pllName, pllLockStatus);
        *pllStatus = 0;
        goto error_1;
    }else{
        *pllStatus = 1;
    }
error_1:
    return ret;
}
/**
 * @description: 设置本振频率
 * @return {*}
 * @param {fr9009RfPllName_t} pllName 选择本振名称，参考结构体
 * @param {uint64_t} rfPllLoFrequency_Hz 设置的本振频率，单位Hz
 */
int32_t FR_LoFreqSet(fr9009RfPllName_t pllName, uint64_t rfPllLoFrequency_Hz)
{
	int ret = SUCCESS;
	uint8_t chip = 0;
	uint8_t chip_nums = DEVICE_NUMS;
	fr9009Device_t  *device = &brDev[0];
	uint8_t pllLockStatus = 0;
	uint8_t pll_lock_mask = 1 << pllName;
	for(chip = 0; chip < chip_nums; chip ++){
		device = &brDev[chip];
        ret = FR9009_armSpiCmd_pll_freq_set(device, pllName, rfPllLoFrequency_Hz);
        if(ret != SUCCESS){
            UART_Printf("set chip %d lo 0x%x freq err\r\n", chip, pllName);
            goto error_1;

        }
    }
	for(chip = 0; chip < chip_nums; chip ++){
		device = &brDev[chip];
	    ret = FR9009_getPllsLockStatus(device, &pllLockStatus);
        if((pllLockStatus & pll_lock_mask) < pll_lock_mask){
            UART_Printf("chip %d lo 0x%x not lock 0x%08X\r\n", chip, pllName, pllLockStatus);
            goto error_1;
        }
    }
error_1:
    return ret;
}
/**
 * @description: 多片同步操作
 * @return {*}
 */
int32_t FR_MCS(void)
{
	int ret = SUCCESS;
	uint8_t chip = 0;
	uint8_t chip_nums = DEVICE_NUMS;
	fr9009Device_t  *device = &brDev[0];
	uint32_t sync_type[3]={MCS_TYPE_RFPLL, MCS_TYPE_ORFPLL, MCS_TYPE_NCO};
    /*******************************************************/
    /**** 注意这里要先停掉sysref ***/
    /*******************************************************/
    /*** < User stop SYSREF Here > ***/
    FRHAL_sysrefReq(device->devHalInfo, __SYSREF_CONT_GLOBAL_OFF);
    /*******************************************************/
    /**** Perform MultiChip Sync (MCS) on FR9009 Device ***/
    /*******************************************************/
    for(chip = 0; chip < chip_nums; chip ++){
        device = &brDev[chip];
        ret = FR9009_enableMultichipSync(device, 1,  sync_type[0]);
        if (ret != TALACT_NO_ACTION) {
            /*** < User: decide what to do based on FR9009 recovery action returned > ***/
            UART_Printf("error: FR9009_enableMultichipSync\r\n");
            goto error_1;
        }
    }
    
    /*** < User Sends SYSREF Here > ***/
    FRHAL_sysrefReq(device->devHalInfo, __SYSREF_CONT_GLOBAL_ON);
    FRHAL_wait_us(device->devHalInfo, 5000);
error_1:
    return ret;
}
/**
 * @description: 初始化校准
 * @return {*}
 */
int32_t FR_InitCali(void)
{
	int ret = SUCCESS;
	int func_ret = SUCCESS;
	uint8_t chip = 0;
	uint8_t chip_nums = DEVICE_NUMS;
	uint8_t errorFlag = 0;
	fr9009Device_t  *device = &brDev[0];
    CHANNEL_t tx_ch = 0;
    CHANNEL_t rx_ch = 0;
    CHANNEL_t orx_ch = 0;
    CHANNEL_t tx1_lp2orx_ch = 0;
    CHANNEL_t tx2_lp2orx_ch = 0;	
	uint32_t initCalMask = (FR_DC_OFFSET | FR_PATH_DELAY | FR_TX_LO_LEAKAGE_INTERNAL | FR_TX_QEC_INIT | FR_RX_LO_DELAY | FR_RX_QEC_INIT| FR_ORX_LO_DELAY| FR_ORX_QEC_INIT);
	for(chip = 0; chip < chip_nums; chip ++){
		device = &brDev[chip];
        tx_ch = (device->devStateInfo.initializedChannels >> 4) & 0x3;
        rx_ch = device->devStateInfo.initializedChannels & 0x3;
        orx_ch = (device->devStateInfo.initializedChannels >> 2) & 0x3;
        /****************************************************/
        /**** Run FR9009 ARM Initialization Calibrations ***/
        /****************************************************/
        /**
         * 如果使用外环tx校准，可以两个tx通道分别校准一次，即一次调用FR9009_runInitCals时，配置initCalMask只选择tx校准项，
         * 使用外环校准FR_TX_INIT_EXTERNAL必须选择上
        */
        func_ret = FR9009_runInitCals(device, tx_ch, rx_ch, orx_ch, tx1_lp2orx_ch, tx2_lp2orx_ch, initCalMask);
        if(func_ret != TALACT_NO_ACTION){
            UART_Printf("error: FR9009_runInitCals() failed\r\n");
            ret |= 1<< chip;
            continue;;
        }
        func_ret = FR9009_waitInitCals(device, 20000, &errorFlag);
        if(errorFlag || func_ret){
            UART_Printf("error: Calibrations not completed\r\n");
            ret |= 1<< chip;
            continue;;
        }else{
            /*< user code - Calibrations completed successfully > */
            UART_Printf("fr9009: Calibrations completed successfully\r\n");
        }
    }
    return ret;
}
/**
 * @description: 获取芯片结温
 * @return {*}
 * @param {uint8_t} chip 目标芯片index
 * @param {uint16_t} *ptemp 返回芯片温度，单位摄氏度
 */
int32_t FR_getTemparature(uint8_t chip, uint16_t *ptemp)
{
	int ret = SUCCESS;
	fr9009Device_t  *device = NULL;
    if(chip >= DEVICE_NUMS){
        UART_Printf("chip %d should < %d\r\n", chip, DEVICE_NUMS);
        goto error_1;
    }
    device = &brDev[chip];
    ret = FR9009_getTemparature(device, ptemp);
    if(ret != SUCCESS){
        UART_Printf("error: chip %d get Temparature err\r\n", chip);
        goto error_1;
    }
error_1:
    return ret;
}
/**
 * @description: 配置数字通道滤波器
 * @return {*}
 * @param {fr9009firName_t} filterToProgram 要配置的通道选择，详见结构体
 * @param {fr9009Fir_t} *firFilter 滤波器参数，详见结构体
 */
int32_t FR_DigFirSet(fr9009firName_t filterToProgram, fr9009Fir_t *firFilter)
{
	int ret = SUCCESS;
	int func_ret = SUCCESS;
	uint8_t chip = 0;
	uint8_t chip_nums = DEVICE_NUMS;
	fr9009Device_t  *device = &brDev[0];
	for(chip = 0; chip < chip_nums; chip ++){
		device = &brDev[chip];
        func_ret = FR9009_programFir(device, filterToProgram, firFilter);
        if(func_ret != SUCCESS){
            ret |= 1<< chip;
            UART_Printf("error: chip %d set 0x%x dig fir err\r\n", chip, filterToProgram);
            continue;
        }
    }
    return ret;
}
/**
 * @description: 配置rx的gain index
 * @return {*}
 * @param {fr9009RxChannels_t} rxChannel 要配置的目标通道
 * @param {uint8_t} gainIndex 注意每个通道的gain index的范围可能不同，在fr9009_config.c中由客户决定
 */
int32_t FR_RxManualGainSet(fr9009RxChannels_t rxChannel, uint8_t gainIndex)
{
	int ret = SUCCESS;
	int func_ret = SUCCESS;
	uint8_t chip = 0;
	uint8_t chip_nums = DEVICE_NUMS;
	fr9009Device_t  *device = &brDev[0];
	for(chip = 0; chip < chip_nums; chip ++){
		device = &brDev[chip];
        func_ret = FR9009_setRxManualGain(device, rxChannel, gainIndex);
        if(func_ret != SUCCESS){
            ret |= 1<< chip;
            UART_Printf("error: chip %d set ch 0x%x gain fir err\r\n", chip, rxChannel);
            continue;
        }
    }
    return ret;
}

/**
 * @description: 配置rx的衰减
 * @return {*}
 * @param {fr9009RxChannels_t} rxChannel 要配置的目标通道可以是FR_RX1， FR_RX2， FR_RX1FR_RX2
 * @param {uint8_t} AttIndex 配置衰减值index（默认取值范围0-63，对应0-31db），AttIndex * att_step =  衰减值， 默认att_step = 0.5db 在fr9009_config.c中由客户决定
 */
int32_t FR_RxManualAttSet(fr9009RxChannels_t rxChannel, uint8_t AttIndex)
{
	int ret = SUCCESS;
	int func_ret = SUCCESS;
	uint8_t chip = 0;
	uint8_t chip_nums = DEVICE_NUMS;
	fr9009Device_t  *device = &brDev[0];
    uint8_t gainIndex = 0;
    int i = 0;
	fr9009Init_t *pInitPara = &talInit; 
    fr9009RxChannels_t rxChannelset = FR_RX1;
    for(i = 0; i < 2; i++){
        if((i == 0) && (rxChannel & FR_RX1)){
            gainIndex = pInitPara->rx.rxGainCtrl.rx1MaxGainIndex - AttIndex;
            rxChannelset = FR_RX1;
        }else if((i == 1) && (rxChannel & FR_RX2)){
            gainIndex = pInitPara->rx.rxGainCtrl.rx2MaxGainIndex - AttIndex;
            rxChannelset = FR_RX2;
        }else{
            continue;
        }
        for(chip = 0; chip < chip_nums; chip ++){
            device = &brDev[chip];
            func_ret = FR9009_setRxManualGain(device, rxChannelset, gainIndex);
            if(func_ret != SUCCESS){
                ret |= 1<< chip;
                UART_Printf("error: chip %d set ch 0x%x gain err\r\n", chip, rxChannelset);
                continue;
            }
        }
    }
    return ret;
}

int32_t FR_RxGainGet(uint8_t chip_index, int16_t *pch1_mdb, int16_t *pch2_mdb)
{
    int i= 0;
    int32_t func_ret = SUCCESS;
    uint8_t chip =0;
    uint8_t chip_nums = chip_index == 0xFF ? DEVICE_NUMS : 1;
    fr9009Init_t *pInitPara = &talInit;
    fr9009Device_t *device =NULL;
    uint32_t regdat = 0;
    uint8_t gain_table_index_step1 = 64 / (pInitPara->rx.rxGainCtrl.rx1MaxGainIndex - pInitPara->rx.rxGainCtrl.rx1MinGainIndex + 1);
    uint8_t gain_table_index_step2 = 64 / (pInitPara->rx.rxGainCtrl.rx2MaxGainIndex - pInitPara->rx.rxGainCtrl.rx2MinGainIndex + 1);
    for(chip = 0; chip < chip_nums; chip ++){
        if(chip_nums == 1 && DEVICE_NUMS != 1){
            chip = chip_index;
        }
        device = &brDev[chip];
        func_ret = FR9009_armSpiCmd_readWords(device, FR9009_ADDR_RX1_RX2_GAIN_TAB_IDX, &regdat, 1);
        if(func_ret != SUCCESS){
            UART_Printf("fr9009 %d get rx gain index fail\r\n", chip);
            break;
        }else{
            if(pch1_mdb){
                pch1_mdb[chip] = (pInitPara->rx.rxGainCtrl.rx1MaxGainIndex -((regdat >> 0) & 0xFF)) * gain_table_index_step1 * 500;//*1000/2
            }
            if(pch2_mdb){
                pch2_mdb[chip] = (pInitPara->rx.rxGainCtrl.rx2MaxGainIndex -((regdat >> 0) & 0xFF)) * gain_table_index_step2 * 500;
            }
        }
    }
}
/**
 * @description: 配置tx衰减
 * @return {*}
 * @param {fr9009TxChannels_t} txChannel 通道选择
 * @param {uint16_t} txAttenuation_mdB  衰减值(eg：1500：衰减1.5 dbm)
 */
int32_t FR_TxAttenuationSet(fr9009TxChannels_t txChannel, uint16_t txAttenuation_mdB)
{
	int ret = SUCCESS;
	int func_ret = SUCCESS;
	uint8_t chip = 0;
	uint8_t chip_nums = DEVICE_NUMS;
	fr9009Device_t  *device = &brDev[0];
	for(chip = 0; chip < chip_nums; chip ++){
		device = &brDev[chip];
        func_ret = FR9009_setTxAttenuation(device, txChannel, txAttenuation_mdB);
        if(func_ret != SUCCESS){
            ret |= 1<< chip;
            UART_Printf("error: chip %d set ch 0x%x Attenuation fir err\r\n", chip, txChannel);
            continue;
        }
    }
    return ret;
}


/**
 * @description: 设置本振频率
 * @return {*}
 * @param {fr9009RfPllName_t} pllName 选择本振名称，参考结构体
 * @param {uint64_t} rfPllLoFrequency_Hz 设置的本振频率，单位Hz
 */
int32_t FR_LoFreqSetWithMcs(fr9009RfPllName_t pllName, uint64_t rfPllLoFrequency_Hz)
{
	int ret = SUCCESS;
	uint8_t set_step =0xff;
	uint8_t chip = 0;
	uint8_t chip_nums = DEVICE_NUMS;
	fr9009Device_t  *device = &brDev[0];
	uint32_t pllLockStatus = 0;
	uint8_t pll_lock_mask = (1 << pllName) << 29;
	uint32_t mcsStatus= 0;
	uint32_t reg = 0;
	int timeout_ms = 1000;
    /*******************************************************/
    /**** 注意这里要先停掉sysref ***/
    /*******************************************************/
    /*** < User stop SYSREF Here > ***/
	FRHAL_sysrefReq(device->devHalInfo, __SYSREF_CONT_GLOBAL_OFF);
	for(chip = 0; chip < chip_nums; chip ++){
		device = &brDev[chip];
        ret = FR9009_armSpiCmd_pll_freq_set(device, pllName, rfPllLoFrequency_Hz);
        if(ret != SUCCESS){
            UART_Printf("set chip %d lo 0x%x freq err\r\n", chip, pllName);
            goto error_1;

        }
    }
	FRHAL_wait_us(device->devHalInfo, 500);
	for(chip = 0; chip < chip_nums; chip ++){
		device = &brDev[chip];
		ret = FRHAL_spiReadWord(device->devHalInfo, HOST_SYSCTRL_BASEADDR + 0xC, &pllLockStatus);
//	    ret = FR9009_getPllsLockStatus(device, &pllLockStatus);
        if((pllLockStatus & pll_lock_mask) < pll_lock_mask){
            UART_Printf("chip %d lo 0x%x not lock 0x%08X\r\n", chip, pllName, pllLockStatus);
            goto error_1;
        }
    }
    /*******************************************************/
    /**** Perform MultiChip Sync (MCS) on FR9009 Device ***/
    /*******************************************************/
    for(chip = 0; chip < chip_nums; chip ++){
        device = &brDev[chip];
        ret = FR9009_armSpiCmd_sync_en(device, 1,  MCS_TYPE_RFPLL);
        if (ret != TALACT_NO_ACTION) {
            /*** < User: decide what to do based on FR9009 recovery action returned > ***/
            UART_Printf("error: FR9009_enableMultichipSync\r\n");
            goto error_1;
        }
    } 
    for(chip = 0; chip < chip_nums; chip ++){//确保上一条指令执行完成
        device = &brDev[chip];
        FR9009_ReadWait(device, FR9009_ADDR_WAKE_MCU, 0, &reg, 1000, 20);
    }
	/*** < User Sends SYSREF Here > ***/
	FRHAL_sysrefReq(device->devHalInfo, __SYSREF_CONT_GLOBAL_ON);
    for(chip = 0; chip < chip_nums; chip ++){
        device = &brDev[chip];   
        timeout_ms = 1000;
		do{
			ret = FR9009_armSpiCmd_sync_state_get(device, &mcsStatus);
			if(ret != TALACT_NO_ACTION){
				UART_Printf("error: FR9009_%d_getMultiChipSyncStatus() failed\r\n", chip);
				goto error_1;
			}
			if((mcsStatus & 0x2) != 0x2){
				FRHAL_wait_us(device->devHalInfo, 1000);
				timeout_ms -= 1;
			}else{
				break;
			}
		}while (timeout_ms > 0);
		if(timeout_ms <= 0){
			UART_Printf("error: FR9009_%d Sync status err:0x%x\r\n", chip, mcsStatus);
			goto error_1;
		}else{
//			UART_Printf("FR9009_%d Sync status 0x%x\r\n", chip, mcsStatus);
		}
	}
	UART_Printf("set freq mcs end\r\n");
error_1:
    return ret;
}

void fr9009_setTxFreqTo1842()
{
    fr9009Device_t *device=&brDev[0];//set rx if convert
    int32_t nco_freg = 95000; // KHz
    int64_t fword =nco_freg*4294967296 /491520;
    FR9009_armSpiCmd_writeField(device, APB_TX_IFCONV_BASEADDR + 0x14, fword, 0xffffffff, 0);
    delay_us(100000);
    FR9009_armSpiCmd_writeField(device, APB_TX_IFCONV_BASEADDR + 0x8, 0x1610, 0xffff, 0);
    delay_us(100000);
    FR9009_armSpiCmd_writeField(device, APB_TX_PATH_TOP_BASEADDR + 0x1C,0x0, 1<<2,0);

}

void fr9009_setTxFreqTo2140()
{
    fr9009Device_t *device =&brDev[0];
    //set tx if convert
    int32_t nco_freg = 150000; // KHz
    int64_t fword =nco_freg * 4294967296 /491520;
    FR9009_armSpiCmd_writeField(device, APB_TX_IFCONV_BASEADDR + 0x8, 0x1610, 0xffff, 0);
    delay_us(100000);
    FR9009_armSpiCmd_writeField(device, APB_TX_IFCONV_BASEADDR + 0x14,fword, 0xffffffff, 0);
    delay_us(100000);
    FR9009_armSpiCmd_writeField(device, APB_TX_PATH_TOP_BASEADDR + 0x1C, 0x0, 1<<2,0);

}

typedef enum{
	FIR_SET_200 = 0,
	FIR_SET_100 = 1,
	FIR_SET_50  = 2,
	FIR_SET_20  = 3,
}FIR_BW_t;

///fir set
int32_t FR_RXRcFirSet(FIR_BW_t bw)
{
    // static uint8_t first_in_flag = 0;
    // if(first_in_flag != 0){
    //     return -1;
    // }
	int ret = SUCCESS;
	int func_ret = SUCCESS;
	uint8_t chip = 0;
	uint8_t chip_nums = DEVICE_NUMS;
	fr9009Device_t  *device = &brDev[0];
    fr9009Fir_t firFilter={
            	.symmetric = 1,
				.even = 1,
                .gain_dB = FR_FIR_GAIN_0_DB,                 /* filter gain */
        };

    const int16_t cfirCoefs_20MHz[64] = { //get_chan_filter(20/250, 30/250, 0.1, 50)
        51,    0,   -4,  -10,  -18,  -27,  -36,  -43,  -48,  -49,  -45,  -36,  -21,   -3,   18,   41,
        62,   80,   91,   94,   87,   70,   43,    7,  -34,  -77, -117, -151, -172, -178, -166, -134,
        -85,  -20,   55,  134,  208,  270,  312,  326,  307,  253,  165,   48,  -91, -241, -387, -514,
    -606, -649, -630, -539, -371, -127,  187,  561,  977, 1416, 1854, 2267, 2632, 2927, 3134, 3241
    };
    const int16_t cfirCoefs_50MHz[62] = { //get_chan_filter(50/250, 60/250, 0.1, 50)
        -55,   -1,   15,   36,   51,   50,   31,   -1,   -34,  -51,  -43,  -10,   35,   70,   78,   48,
        -8,  -69, -104,  -92,  -34,   49,  119,  141,  97,    0, -110, -180, -171,  -78,   65,  194,
        245,  183,   24, -167, -301, -306, -162,   80,  315,  429,  348,   85, -259, -529, -580, -352,
        91,  571,  858,  772,  272, -496, -1223, -1534,  -1129,  97, 1980, 4114, 5967, 7042
    };
	const int16_t cfirCoefs_100MHz[62] = {
	    -63, -54, 12, 27, 19, -42, -26, 20, 54, -5, -57, -33, 54, 61, -24, -88,
	    -16, 89, 69, -66, -112, 13, 135, 57, -121, -130, 67, 182, 22, -194, -128, 149,
	    224, -48, -278, -95, 263, 248, -164, -369, -12, 413, 236, -345, -457, 151, 612, 157,
	    -633, -534, 463, 905, -63, -1170, -591, 1203, 1533, -807, -2950, -621, 6388, 12760
	};
    const int16_t cfirCoefs_200MHz[50] = { //get_chan_filter(20/250, 30/250, 0.1, 50)
        63, 35, -33, 30, -23, 10, 9, -31, 48, -54, 45, -21, -16, 57, -89, 103, -88, 45, 20, -93,
        153, -182, 165, -97, -10, 135, -246, 309, -297, 201, -31, -180, 381, -516, 536, -411, 146, 218, -606, 919,
        -1055, 930, -495, -245, 1228, -2342, 3441, -4369, 4989, 27561
    };
    if(bw == FIR_SET_20){
        firFilter.coefs = cfirCoefs_20MHz;
        firFilter.numFirCoefs = 64;
    }else if(bw == FIR_SET_50){
        firFilter.coefs = cfirCoefs_50MHz;
        firFilter.numFirCoefs = 62;
	}else if(bw == FIR_SET_100){
		firFilter.coefs = (int16_t *)cfirCoefs_100MHz;
		firFilter.numFirCoefs = 62;
    }else{
        firFilter.coefs = cfirCoefs_200MHz;
        firFilter.numFirCoefs = 50;
        firFilter.even = 0;
    }
	for(chip = 0; chip < chip_nums; chip ++){
		device = &brDev[chip];
        func_ret = FR9009_programFir(device, FR_RX1RX2_FIR, &firFilter);
        if(func_ret != SUCCESS){
            ret |= 1<< chip;
            UART_Printf("error: chip %d set 0x%x dig fir err\r\n", chip, FR_RX1RX2_FIR);
            continue;
        }
    }
    // first_in_flag = 1;
    return ret;
}