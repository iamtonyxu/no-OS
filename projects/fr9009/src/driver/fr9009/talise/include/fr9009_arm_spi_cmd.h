
#ifndef FR9009_ARM_SPI_CMD_H_
#define FR9009_ARM_SPI_CMD_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "fr9009_reg_addr_macros.h"
#include "fr9009_types.h"


#define RX_PATH             1
#define RIPPLE_WRITE_LEN    32

typedef enum {
	TALAPI_ARMSPI_BUSY		= 0x00D0C0DEUL,
	TALAPI_ARMSPI_READY		= 0xC0DE00D0,
} talApiArmSpiSta;

enum LOG_CONF_MODE{
	LOG_INIT = 0,
	LOG_OPT_SET,
	LOG_GET_FROMRAM,
};

enum CALI_part_t{
	FR_CALI_RX_DC_OFFSET            = 0x00000001,   /*!< DC offset calibration */
	FR_CALI_ORX_DC_OFFSET           = 0x00000002,   /*!< DC offset calibration */
	FR_CALI_PATH_DELAY              = 0x00000004,   /*!< Path delay equalization calibration */
	FR_CALI_TX_LO_CALI				= 0x00000008,   /*!< Internal Tx LO leakage calibration */
	FR_CALI_TX_QEC_INIT             = 0x00000010,   /*!< Tx quadrature error correction calibration */
	FR_CALI_RX_LO_DELAY             = 0x00000020,   /*!< Rx LO delay path calibration */
	FR_CALI_RX_QEC_INIT             = 0x00000040,   /*!< Rx quadrature error correction calibration */
	FR_CALI_ORX_LO_DELAY            = 0x00000080,   /*!< ORx LO delay path calibration */
	FR_CALI_ORX_QEC_INIT            = 0x00000100,   /*!< ORx quadrature error correction calibration */
	FR_CALI_TX_FLATTEN_INIT         = 0x00000200,   /*!< TX flatten calibration */
	FR_CALI_RX_FLATTEN_INIT         = 0x00000400,   /*!< RX flatten calibration */
	FR_CALI_ORX_FLATTEN_INIT        = 0x00000800,   /*!< ORX flatten calibration */
	FR_CALI_NCO_CALS                = 0x00400000,   /*!< FHM (Fast Frequency Hopping Mode) NCO Calibrations */
	FR_CALI_FHM_CALS                = 0x00800000,   /*!< FHM (Fast Frequency Hopping Mode) Calibrations */
	FR_CALI_PRECALI_START           = 0x20000000,   /*!< PREDICT Calibrations Start*/
	FR_CALI_SWEEP	                = 0x40000000,   /*!< after Calibrations do sweep check*/
	FR_CALI_TX_INIT_EXTERNAL		= 0x80000000,   /*!< TX calibration external loopback*/
};

enum TX_LOOPMODE_t{
	TX_LOOP2RX_MODE		= 0x1,
	TX_LOOP2ORX_MODE,
	TX_LOOP2ORX2_MODE,
};

enum SYNC_MODE_t{
	SYNC_FUNC_MODE_PLL_EN = 0,
	SYNC_FUNC_MODE_RESYNC,
	SYNC_FUNC_MODE_TYPE,
	SYNC_FUNC_MODE_STATE,
	SYNC_FUNC_MODE_NCO_SYNC,
	SYNC_FUNC_MODE_DIG_EN,
	SYNC_FUNC_MODE_TIMEP_TRACK_SET,
	SYNC_FUNC_MODE_FLAG_OFF_SET,
};

enum FASTHOP_FUNC_MODE_t{
	FASTHOP_FUNC_MODE_INIT = 0,
	FASTHOP_FUNC_MODE_SET_FREQ,
	FASTHOP_FUNC_MODE_ONOFF,
	FASTHOP_FUNC_MODE_PARAM_SET,
	FASTHOP_FUNC_TRIGGER_MODE_SET,
	FASTHOP_FUNC_MODE_NEXT_FREQ_SET,
	FASTHOP_FUNC_MODE_CURRENT_FREQ_GET,
	FASTHOP_FUNC_MODE_FHM_CH_SET,
};

enum DEBUG_FUNC_MODE_t{
	DEBUG_TX_PLAY_OFDM = 0,
	DEBUG_RX_PLAY_WAVE,
    DEBUG_RX_POWER_GET,
};

enum TX_CTRL_CONFIG_t{
	TX_ATT_SET_GET = 0x00,
	TX_POWER_CTRL_SET,
	TX_POWER_CTRL_ERR_GET,
	TX_POWER_CTRL_ERR_RESET,
};

enum CALI_MODE_t{
	TRACK_MASK_SET_MODE = 0,
	RX_DC_CALI_SET_MODE,
	CALI_POWER_SET_MODE,
	PRECALI_FREQS_SET_MODE,
	PRECALI_REG_LOAD_MODE,
	CALI_TRIM_SET_MODE,
	CALI_TRIM_IRR_SCALAR_SET,
	CALI_TRIM_IRR_SCALAR_ESTI,
};
enum LO_MODE_t{
    CLK_PLL_SET = 0x00,
    RF_PLL_SET	= 0x01, 
    ORF_PLL_SET = 0x02,
    PLL_ONOFF_SET = 0x04,
    PLL_TEMP_EN_SET = 0x05,
    PLL_STATUS_GPIO_SET = 0x06,
    PLL_OPT_SET = 0x07,
    CLK_PLL_GET = 0x08,
    RF_PLL_GET	= 0x09, 
    ORF_PLL_GET = 0x0A,
    VCO_CFG_SET = 0x0B,
	PLL_MERGE_SET = 0x0C,
};
typedef enum{
	VCO_T_OFF = 0x0,
	RFVCO_IN = 0x1,
	ORFVCO_IN = 0x2,
	RFVCO_OUT = 0x4,
	ORFVCO_OUT = 0x8,
}VCOConfig_t;

typedef enum {	
	TALAPI_ARMSPI_CMD_BLKCPY							= 0x0001,
	TALAPI_ARMSPI_CMD_SPI_BLK_WRITE                     = 0x0002,
	TALAPI_ARMSPI_CMD_SPI_BLK_READ                      = 0x0003,
	TALAPI_ARMSPI_CMD_CLEAR_TX_QFIR                 	= 0x0004,
	TALAPI_ARMSPI_CMD_CLEAR_TX_CFIR                 	= 0x0005,
	TALAPI_ARMSPI_CMD_SETUP_BB_RF_PLL               	= 0x0006,
	TALAPI_ARMSPI_CMD_PURESWEEP_LB_RX1              	= 0x0007,
	TALAPI_ARMSPI_CMD_SETONEFREQ_LB_RX1             	= 0x0008,
	TALAPI_ARMSPI_CMD_CALIBRATIONFUNC_LB_RX1        	= 0x0009,
	TALAPI_ARMSPI_CMD_PURESWEEP_LB_ORX1             	= 0x000A,
	TALAPI_ARMSPI_CMD_SETONEFREQ_LB_ORX1            	= 0x000B,
	TALAPI_ARMSPI_CMD_CALIBRATIONFUNC_LB_ORX1       	= 0x000C,
	TALAPI_ARMSPI_CMD_TX_PURESWEEP                  	= 0x000D,
	TALAPI_ARMSPI_CMD_TX_SETONEFREQ                 	= 0x000E,
	TALAPI_ARMSPI_CMD_TX_CALIBRATIONFUNC            	= 0x000F,
	TALAPI_ARMSPI_CMD_SETUP_TX_LOOPBACK             	= 0x0010,
	TALAPI_ARMSPI_CMD_MEM_MASK_WRITE            	    = 0x0011,
	TALAPI_ARMSPI_CMD_SPI_DEV_MASK_WRITE              	= 0x0012,
	TALAPI_ARMSPI_CMD_SETUP_LOOPBACK_LB_RX1         	= 0x0013,
	TALAPI_ARMSPI_CMD_SETUP_LOOPBACK_LB_ORX1        	= 0x0014,
	TALAPI_ARMSPI_CMD_LO_LEAK_CALI                  	= 0x0015,
	TALAPI_ARMSPI_CMD_O_RX_LO_LEAK_CALI_ADDR4       	= 0x0016,
	TALAPI_ARMSPI_CMD_O_RX_LO_LEAK_CALI_ADDR1C      	= 0x0017,
	SPI_CMD_TEST_CONFIG				              	= 0x0018,
	TALAPI_ARMSPI_CMD_AUX_CONFIG                    	= 0x0019,
	TALAPI_ARMSPI_CMD_TRACK_CALI                 		= 0x001A,
	TALAPI_ARMSPI_CMD_MAG_AGC	                    	= 0x001B,
	TALAPI_ARMSPI_CMD_TURNOFF_LOOPBACK_LO_ORFPLL    	= 0x001C,
	TALAPI_ARMSPI_CMD_CONFIG_AGC                   		= 0x001D,
	TALAPI_ARMSPI_CMD_RX_DIG_DC_XAL                   	= 0x0020,
	TALAPI_ARMSPI_CMD_INIT_CAL							= 0x0021,
	TALAPI_ARMSPI_CMD_JESD_CONFIG						= 0x0022,
	TALAPI_ARMSPI_CMD_SETRF_FREQ_NEW					= 0x0023,
	TALAPI_ARMSPI_CMD_SYNC_ADC_Q						= 0x0024,
	TALAPI_ARMSPI_CMD_PLL_FREQ							= 0x0025,
	TALAPI_ARMSPI_CMD_AGC_CALI							= 0x0026,
	TALAPI_ARMSPI_CMD_JESD_STATE_SET					= 0x0027,
	TALAPI_ARMSPI_CMD_SYNC								= 0x0031,
	TALAPI_ARMSPI_CMD_FASTHOP							= 0x0032,
	TALAPI_ARMSPI_CMD_DEBUG								= 0x0033,
    TALAPI_ARMSPI_CMD_CALI_CPLXFIR					    = 0x1001,
	TALAPI_ARMSPI_CMD_PLAYCAP_SET						= 0x1101,
	TALAPI_ARMSPI_CMD_PLAYCAP_MANUAL_START			    = 0x1102,
	TALAPI_ARMSPI_CMD_TDD_MANUAL_MODE_SET				= 0x1200,
	TALAPI_ARMSPI_CMD_ORXRX_CH_EN						= 0x1201,
	TALAPI_ARMSPI_CMD_ORXRX_BW_SET					    = 0x1202,
	TALAPI_ARMSPI_CMD_ORXCH_GAIN_SET_GET				= 0x1203,
	TALAPI_ARMSPI_CMD_TX_CH_EN						    = 0x1204,
	TALAPI_ARMSPI_CMD_TX_CTRL_CONFIG				    = 0x1205,
	TALAPI_ARMSPI_CMD_CHANNEL_CONFIG				    = 0x1206,
	TALAPI_ARMSPI_CMD_TX_NCO_SET						= 0x1207,
	TALAPI_ARMSPI_CMD_TX_NCOPOWER_SET					= 0x1208,
	TALAPI_ARMSPI_CMD_TX_LO_RIPPLE_SET                  = 0x1209,
	TALAPI_ARMSPI_CMD_MEM_WRITE 						= 0x2000,
	TALAPI_ARMSPI_CMD_MEM_READ 							= 0x2001,
	TALAPI_ARMSPI_CMD_FIELD_WRITE 						= 0x2002,
	TALAPI_ARMSPI_CMD_LOG_CONFIG		 				= 0x2003,		
	TALAPI_ARMSPI_CMD_TEST_FUNC						    = 0xFFFF,
} talApiArmSpiCMD;

enum CMD_AGC_CONF_MODE{
	AGC_SET_CONTROL = 0,
	AGC_GET_CONTROL	,
	AGC_SET_PEAK	,
	AGC_GET_PEAK	,
	AGC_SET_POWER	,
	AGC_GET_POWER	,
	AGC_SET_MGC_OR_AGC	,
	AGC_SET_MGC_SET	,
	AGC_SET_GAIN_RANGE_SET	,
};
enum CHANNEL_CONF_MODE_T{
	CHANNEL_CONF_MODE_TX_BW_SET = 0,
	CHANNEL_CONF_MODE_RX_BW_SET,
	CHANNEL_CONF_MODE_ORX_BW_SET,
	CHANNEL_CONF_MODE_DEC_MODE_SET,
	CHANNEL_CONF_MODE_TX_LO_SET,
	CHANNEL_CONF_MODE_RX_LO_SET,
	CHANNEL_CONF_MODE_ORX_LO_SET,
	CHANNEL_CONF_MODE_TX_LO_GET,
	CHANNEL_CONF_MODE_RX_LO_GET,
	CHANNEL_CONF_MODE_ORX_LO_GET,
	CHANNEL_CONF_MODE_LIF_MODE_SET,
	CHANNEL_CONF_MODE_LIF_FREQ_SET,
	CHANNEL_CONF_MODE_RFDIRECT_SET,
	CHANNEL_CONF_MODE_DEFAULT_FLATNESS_SET,
	CHANNEL_CONF_MODE_CFIR_COEFS_SET,
	CHANNEL_CONF_MODE_CFIR_COEFS_IND_SET,
};

enum AUX_MODE_T{
	AUX_MODE_TEMPARATURE_GET = 0,
	AUX_MODE_TEMPARATURE_READ ,
	AUX_MODE_TEMPARATURE_PROTECT_EN,
	AUX_MODE_TEMPARATURE_PROTECT_STATUS,
};
#pragma pack(push)
#pragma pack(1)

typedef struct {
	uint32_t cmdMode;
	uint32_t agcGainUpdateCounter_us;            /*!< AGC gain update time in micro seconds */
	uint8_t agcPeakWaitTime;                    /*!< AGC peak wait time. Valid range is from 0 to 31 */
	uint8_t agcRx1MaxGainIndex;                 /*!< AGC Rx1 max gain index. Valid range is from 0 to 255 */
	uint8_t agcRx1MinGainIndex;                 /*!< AGC Rx1 min gain index. Valid range is from 0 to 255 */
	uint8_t agcRx2MaxGainIndex;                 /*!< AGC Rx2 max gain index. Valid range is from 0 to 255 */
	uint8_t agcRx2MinGainIndex;                 /*!< AGC Rx2 min gain index. Valid range is from 0 to 255 */
	uint8_t agcRx1AttackDelay;                  /*!< On entering Rx, the Rx1 AGC is kept inactive for a period = agcRx1AttackDelay*1us */
	uint8_t agcRx2AttackDelay;                  /*!< On entering Rx, the Rx2 AGC is kept inactive for a period = agcRx2AttackDelay*1us */
	uint8_t agcSlowLoopSettlingDelay;           /*!< On any gain change, the AGC waits for the time (range 0 to 127) specified in AGC clock cycles to allow gain transients to flow through the Rx path before starting any measurements. */
	uint8_t agcLowThreshPreventGain;            /*!< Prevent gain index from incrementing if peak thresholds are being exceeded */
	uint8_t agcChangeGainIfThreshHigh;          /*!< Enable immediate gain change if high threshold counter is exceeded. Bit 0 enables ULB high threshold, Bit 1 enables HB2 high threshold */
	uint8_t agcPeakThreshGainControlMode;       /*!< Enable gain change based only on the signal peak threshold over-ranges. Power based AGC changes are disabled in this mode. */
	uint8_t agcResetOnRxon;                     /*!< Reset the AGC slow loop state machine to max gain when the Rx Enable is taken low */
	uint8_t agcEnableSyncPulseForGainCounter;   /*!< Enable the AGC gain update counter to be sync'ed to a time-slot boundary. */
	uint8_t agcEnableFastRecoveryLoop;          /*!< Enable multiple time constants in AGC loop for fast attack and fast recovery. */
}AgcCtrl_t;

typedef struct {
	uint32_t cmdMode;
	uint16_t rx1TddPowerMeasDuration;                /*!< Measurement duration to detect power for specific slice of the gain update counter. */
	uint16_t rx1TddPowerMeasDelay;                   /*!< Measurement delay to detect power for specific slice of the gain update counter. */
	uint16_t rx2TddPowerMeasDuration;                /*!< Measurement duration to detect power for specific slice of the gain update counter. */
	uint16_t rx2TddPowerMeasDelay;                   /*!< Measurement delay to detect power for specific slice of the gain update counter. */
	uint8_t powerEnableMeasurement;                 /*!< Enable the Rx power measurement block. (0/1) */
	uint8_t powerUseRfirOut;                        /*!< Use output of Rx PFIR for power measurement. (0/1) */
	uint8_t powerUseBBDC2;                          /*!< Use output of DC offset block for power measurement. (0/1) */
	uint8_t underRangeHighPowerThresh;              /*!< AGC power measurement detect lower 0 threshold. Valid Range from 0 to 127. */
	uint8_t underRangeLowPowerThresh;               /*!< AGC power measurement detect lower 1 threshold. Valid offset from 0 to 31 */
	uint8_t underRangeHighPowerGainStepRecovery;    /*!< AGC power measurement detect lower 0 recovery gain step. Valid range from  0 to 31 */
	uint8_t underRangeLowPowerGainStepRecovery;     /*!< AGC power measurement detect lower 1 recovery gain step. Valid range from  0 to 31 */
	uint8_t powerMeasurementDuration;               /*!< Average power measurement duration = 8*2^powerMeasurementDuration. Valid range from 0 to 31 */
	uint8_t upper0PowerThresh;                      /*!< AGC upper 0 (overRangeHighPowerThreshold) threshold for power measurement. Valid Range from 0 to 127.*/
	uint8_t upper1PowerThresh;                      /*!< AGC upper 1 (overRangeLowPowerThreshold)  threshold for power measurement. Valid offset from 0 to 15 */
	uint8_t powerLogShift;                          /*!< Enable Increase in dynamic range of the power measurement from 40dB to ~60dB. Provides higher accuracy.
                                                         *   NOTE: If you input a signal below -60dBFS while enabled, TALISE_getRxDecPower() may return an incorrect reading of 0dBFS.
                                                         *   When disabled, TALISE_getRxDecPower() will bottom out around -40dBFS. */
	uint8_t overRangeLowPowerGainStepAttack;        /*!< AGC inner upper threshold exceeded attack gain step. Optional. Valid range from  1 to 31. Passing 0 will result in the reset value of 4. */
	uint8_t overRangeHighPowerGainStepAttack;       /*!< AGC outer high power threshold exceeded attack gain step. Optional. Valid range from  1 to 31. Passing 0 will result in the reset value of 4. */
}AgcPower_t;

typedef struct {
	uint32_t cmdMode;
	uint32_t agcUnderRangeLowInterval_ns;        /*!< Update interval for AGC loop mode in nanoseconds */
	uint16_t hb2HighThresh;                      /*!< AGC HB2 output high threshold. Valid range from  0 to 255 */
	uint16_t hb2UnderRangeLowThresh;             /*!< AGC HB2 output low threshold. Valid range from  0 to 255 */
	uint16_t hb2UnderRangeMidThresh;             /*!< AGC HB2 output low threshold for 2nd interval for multiple time constant AGC mode. Valid range from  0 to 255 */
	uint16_t hb2UnderRangeHighThresh;            /*!< AGC HB2 output low threshold for 3rd interval for multiple time constant AGC mode. Valid range from  0 to 255 */
	uint8_t agcUnderRangeMidInterval;           /*!< 2nd update interval for multiple time constant AGC mode. Calculated as (agcUnderRangeMidInterval+1)*agcUnderRangeLowInterval_ns. Valid range is 0 to 63 */
	uint8_t agcUnderRangeHighInterval;          /*!< 3rd update interval for multiple time constant AGC mode. Calculated as (agcUnderRangeHighInterval+1)*2nd update interval. Valid range is 0 to 63 */
	uint8_t apdHighThresh;                      /*!< AGC APD high threshold. Valid range is 7 to 49 */
	uint8_t apdLowGainModeHighThresh;           /*!< AGC APD high threshold in low gain mode. Valid range is 7 to 49. Recommended to be 3dB above apdHighThresh */
	uint8_t apdLowThresh;                       /*!< AGC APD low threshold. Valid range is 7 to 49. Recommended to be 3dB below apdHighThresh */
	uint8_t apdLowGainModeLowThresh;            /*!< AGC APD low threshold in low gain mode. Valid range is 7 to 49. Recommended to be 3dB above apdLowThresh */
	uint8_t apdUpperThreshPeakExceededCnt;      /*!< AGC APD peak detect upper threshold count. Valid range is 0 to 255 */
	uint8_t apdLowerThreshPeakExceededCnt;      /*!< AGC APD peak detect lower threshold count. Valid range is 0 to 255 */
	uint8_t apdGainStepAttack;                  /*!< AGC APD peak detect attack gain step. Valid range is 0 to 31 */
	uint8_t apdGainStepRecovery;                /*!< AGC APD gain index step size for recovery. Valid range is 0 to 31 */
	uint8_t enableHb2Overload;                  /*!< Enable or disables the HB2 overload detector. */
	uint8_t hb2OverloadDurationCnt;             /*!< Sets the window of clock cycles (at the HB2 output rate) to meet the overload count. (0 = 2 cycles, 1 = 4 cycles, 2 = 8 cycles, 3 = 12 cycles, 4 = 16 cycles, 5 = 24 cycles, 6 = 32 cycles) */
	uint8_t hb2OverloadThreshCnt;               /*!< Sets the number of actual overloads required to trigger the overload signal. Valid range from 1 to 15 */
	uint8_t hb2UpperThreshPeakExceededCnt;      /*!< AGC HB2 output upper threshold count. Valid range from  0 to 255 */
	uint8_t hb2LowerThreshPeakExceededCnt;      /*!< AGC HB2 output lower threshold count. Valid range from  0 to 255 */
	uint8_t hb2GainStepHighRecovery;            /*!< AGC HB2 gain index step size. Valid range from  0 to 31 */
	uint8_t hb2GainStepLowRecovery;             /*!< AGC HB2 gain index step size, when the HB2 Low Overrange interval 2 triggers. Valid range from  0 to 31 */
	uint8_t hb2GainStepMidRecovery;             /*!< AGC HB2 gain index step size, when the HB2 Low Overrange interval 3 triggers. Valid range from  0 to 31 */
	uint8_t hb2GainStepAttack;                  /*!< AGC HB2 output attack gain step. Valid range from  0 to 31 */
	uint8_t hb2OverloadPowerMode;               /*!< When this bit is set, the dynamic range of the power measurement increases from -40dB to ~-60dB (that is, all signal levels from 0dBFS to -60dBFS are accurately detected */
	uint8_t hb2OvrgSel;                         /*!< To be used in fast recovery mode. Clearing this bit enables the decimated data overload detection functionality */
	uint8_t hb2ThreshConfig;                    /*!< Not User Modifiable   Initialized to 0x03 */
	uint8_t hb2UnderRangeLowThreshExceededCnt;  /*!< AGC HB2 low overrange interval 0 threshold count. Optional. Valid range from 1 to 255. Passing 0 will result in the reset value of 3. */
	uint8_t hb2UnderRangeMidThreshExceededCnt;  /*!< AGC HB2 mid overrange interval 1 threshold count. Optional. Valid range from 1 to 255. Passing 0 will result in the reset value of 3. */

}AgcPeak_t;

#pragma pack(pop)



/**
 * \brief Enumerated list of channel
 */
typedef enum{
	CHANNEL_1	= 0x1,
	CHANNEL_2	= 0x2,
	CHANNEL_1_2	= 0x3,
}CHANNEL_t;

typedef enum{
	CHANNEL_TYPE_RX		= 0x1,
	CHANNEL_TYPE_ORX	= 0x2,
	CHANNEL_TYPE_TX		= 0x4,
}CHANNEL_TYPE_t;

// typedef enum{
// 	FR_LO_RF_PLL  = 0x00,
// 	FR_LO_AUX_PLL = 0x01,
// }fr9009LoSource_t;

typedef enum{
	JESD_TX	= 0x1,
	JESD_RX	= 0x2,
	JESD_TX_RX	= 0x3,
}JESDSerializer_Tpye_t;

enum JESD_SERDES_MODE_t{
	JESD_SERDES_INIT,
	JESD_SERIALIZER_PARAM_SET,
	JESD_DESERIALIZER_PARAM_SET,
	JESD_CLK_DIV_SET,
};
enum INIT_MODE_t{
	INIT_CHANNEL_CALIBRATION = 0,
	INIT_ADDA_CALIBRATION,
	INIT_ARM_CLK_SEL,
	INIT_SUB_ADC_NUM_SET,
	INIT_APB_REG_SET,
};
//uint32_t FR9009_armSpiCmd_blkcpy(fr9009Device_t *device, uint32_t srcAdr, uint32_t dstAdr, uint32_t wSz);
MYDLL_API uint32_t FR9009_armSpiCmd_writeWords(fr9009Device_t *device, uint32_t wrAddr, uint32_t* wrDataBuf, uint32_t wrWordSz);
MYDLL_API uint32_t FR9009_armSpiCmd_readWords(fr9009Device_t *device, uint32_t rdAddr, uint32_t* rdDataBuf, uint32_t rdWordSz);
MYDLL_API uint32_t FR9009_armSpiCmd_writeField(fr9009Device_t *device, uint32_t addr, uint32_t fieldVal,  uint32_t mask, uint32_t startBit);
MYDLL_API uint32_t FR9009_armSpiCmd_readField(fr9009Device_t *device, uint32_t addr, uint32_t *fieldVal,  uint32_t mask, uint32_t startBit);
MYDLL_API uint32_t FR9009_armSpiCmd_log_init_mode(fr9009Device_t *device, uint8_t level, uint32_t mask);
MYDLL_API uint32_t FR9009_armSpiCmd_log_opt_set(fr9009Device_t *device, uint8_t wr2ram_en);
MYDLL_API uint32_t FR9009_armSpiCmd_log_read_from_ram(fr9009Device_t *device, uint32_t* rdDataBuf, uint32_t rdWordSz);
MYDLL_API uint32_t FR9009_armMemoryCmd_blk_write(fr9009Device_t *device, uint32_t startRegAddr, uint32_t *wrDataBuf, uint32_t  wrWordSz);
MYDLL_API uint32_t FR9009_armMemoryCmd_blk_read(fr9009Device_t *device, uint32_t startRegAddr, uint32_t *rdDataBuf, uint32_t rdWordSz);
MYDLL_API uint32_t FR9009_armSpiCmd_SPI_blk_write(fr9009Device_t *device, uint8_t SPI_select, uint16_t regAddr, uint32_t *wrDataBuf, uint32_t  wrWordSz);
MYDLL_API uint32_t FR9009_armSpiCmd_SPI_blk_read(fr9009Device_t *device, uint8_t SPI_select, uint16_t regAddr, uint32_t *rdDataBuf, uint32_t rdWordSz); 
MYDLL_API uint32_t FR9009_armSpiCmd_clear_tx_qfir(fr9009Device_t *device);
MYDLL_API uint32_t FR9009_armSpiCmd_clear_tx_cfir(fr9009Device_t *device);
MYDLL_API uint32_t FR9009_armSpiCmd_setup_bb_rf_pll(fr9009Device_t *device);
MYDLL_API uint32_t FR9009_armSpiCmd_puresweep_lb_rx1(fr9009Device_t *device, CHANNEL_t sweep_ch, uint8_t check_point);
MYDLL_API uint32_t FR9009_armSpiCmd_setonefreq_lb_rx1(fr9009Device_t *device, CHANNEL_t freq_ch, uint8_t freq_index, uint8_t check_point);
MYDLL_API uint32_t FR9009_armSpiCmd_calibrationfunc_lb_rx1(fr9009Device_t *device, CHANNEL_t cali_ch);
MYDLL_API uint32_t FR9009_armSpiCmd_puresweep_lb_orx1(fr9009Device_t *device,  CHANNEL_t sweep_ch, uint8_t check_point);
MYDLL_API uint32_t FR9009_armSpiCmd_setonefreq_lb_orx1(fr9009Device_t *device, CHANNEL_t freq_ch, uint8_t freq_index, uint8_t check_point);
MYDLL_API uint32_t FR9009_armSpiCmd_calibrationfunc_lb_orx1(fr9009Device_t *device, CHANNEL_t cali_ch);
MYDLL_API uint32_t FR9009_armSpiCmd_tx_puresweep(fr9009Device_t *device, CHANNEL_t cali_ch);
MYDLL_API uint32_t FR9009_armSpiCmd_tx_setonefreq(fr9009Device_t *device, CHANNEL_t tx_ch, uint32_t freqpoint);
MYDLL_API uint32_t FR9009_armSpiCmd_tx_calibrationfunc(fr9009Device_t *device,  CHANNEL_t cali_ch);
MYDLL_API uint32_t FR9009_armSpiCmd_setup_tx_loopback(fr9009Device_t *device, CHANNEL_t lb_channel, CHANNEL_t rx_orx_ch, uint8_t lp_type, uint8_t loopmode);
MYDLL_API uint32_t FR9009_armSpiCmd_mem_mask_write(fr9009Device_t *device, uint32_t val, uint32_t dstAdr, uint32_t mask);
MYDLL_API uint32_t FR9009_armSpiCmd_spi_dev_mask_write(fr9009Device_t *device, uint32_t mask, uint32_t spi_sel, uint16_t regAddr, uint32_t val);
MYDLL_API uint32_t FR9009_armSpiCmd_setup_loopback_lb_rx1(fr9009Device_t *device, CHANNEL_t lb_channel);
MYDLL_API uint32_t FR9009_armSpiCmd_setup_loopback_lb_orx1(fr9009Device_t *device, CHANNEL_t lb_channel);
MYDLL_API uint32_t FR9009_armSpiCmd_tx_lo_leak_cali(fr9009Device_t *device, CHANNEL_t set_channel);
MYDLL_API uint32_t FR9009_armSpiCmd_rx_lo_leak_cali_addr4(fr9009Device_t *device, CHANNEL_t set_channel);
MYDLL_API uint32_t FR9009_armSpiCmd_orx_lo_leak_cali_addr1c(fr9009Device_t *device, CHANNEL_t set_channel);
MYDLL_API uint32_t FR9009_armSpiCmd_temparature_get(fr9009Device_t *device, uint8_t temp_sel, int32_t *tempareture_m_0c);
MYDLL_API uint32_t FR9009_armSpiCmd_temparature_read(fr9009Device_t *device, uint8_t temp_sel, int32_t *tempareture_m_0c);
MYDLL_API uint32_t FR9009_armSpiCmd_temp_protect_en(fr9009Device_t *device, uint8_t en);
MYDLL_API uint32_t FR9009_armSpiCmd_temp_protect_status(fr9009Device_t *device, uint8_t *status);
MYDLL_API uint32_t FR9009_armSpiCmd_test_config(fr9009Device_t *device,uint8_t test_key, uint32_t conf);
MYDLL_API uint32_t FR9009_armSpiCmd_track_cali_En(fr9009Device_t *device, uint32_t enableMask);
MYDLL_API uint32_t FR9009_armSpiCmd_rx_dc_mode(fr9009Device_t *device, uint8_t mode);
MYDLL_API uint32_t FR9009_armSpiCmd_cali_power_set(fr9009Device_t *device, uint16_t tone_amp_dig, uint16_t ofdm_power);
MYDLL_API uint32_t FR9009_armSpiCmd_precali_freqs_set(fr9009Device_t *device, uint8_t pllsel, int32_t *freq_khz, uint16_t nums);
MYDLL_API uint32_t FR9009_armSpiCmd_pre_cali_load(fr9009Device_t *device, fr9009RfPllName_t pll_type, uint64_t freq_hz);
MYDLL_API uint32_t FR9009_armSpiCmd_cali_trim_mode_set(fr9009Device_t *device, uint32_t mode);
MYDLL_API uint32_t FR9009_armSpiCmd_cali_irr_scalar_set(fr9009Device_t *device, CHANNEL_TYPE_t ch_type, CHANNEL_t ch, int8_t irr_gain_trim, int8_t irr_theta_trim);
MYDLL_API uint32_t FR9009_armSpiCmd_cali_irr_scalar_auto(fr9009Device_t *device, CHANNEL_TYPE_t ch_type, CHANNEL_t ch);
MYDLL_API uint32_t FR9009_armSpiCmd_agc_mode_set(fr9009Device_t *device, uint8_t mode);
MYDLL_API uint32_t FR9009_armSpiCmd_mgc_agc_mode(fr9009Device_t *device, CHANNEL_t channel, uint8_t index, uint8_t mode);
MYDLL_API uint32_t FR9009_armSpiCmd_turnoff_all(fr9009Device_t *device);
MYDLL_API uint32_t FR9009_armSpiCmd_agc_config_set(fr9009Device_t *device, uint32_t *data, uint16_t datalen);
MYDLL_API uint32_t FR9009_armSpiCmd_agc_config_get(fr9009Device_t *device, uint32_t *data, uint16_t datalen);
MYDLL_API uint32_t FR9009_armSpiCmd_rx_dig_dc_cali(fr9009Device_t *device, CHANNEL_t set_channel);
MYDLL_API uint32_t FR9009_armSpiCmd_Initical_cali(fr9009Device_t *device, CHANNEL_t tx_ch, CHANNEL_t rx_ch, CHANNEL_t orx_ch, CHANNEL_t tx1_lp2orx_ch, CHANNEL_t tx2_lp2orx_ch, uint8_t external_lp, uint8_t tx_lpmode, uint32_t cali_flag);
MYDLL_API uint32_t FR9009_armSpiCmd_Init_ADDAcali(fr9009Device_t *device, CHANNEL_t tx_ch, CHANNEL_t rx_ch, uint8_t *status);
MYDLL_API uint32_t FR9009_armSpiCmd_arm_clkSel(fr9009Device_t *device, uint8_t armClkSel);
MYDLL_API uint32_t FR9009_armSpiCmd_Init_ADNum(fr9009Device_t *device,  CHANNEL_t rx_ch, uint8_t num, uint8_t *cur_num);
MYDLL_API uint32_t FR9009_armSpiCmd_Init_Reg(fr9009Device_t *device,  uint8_t type);
MYDLL_API uint32_t FR9009_armSpiCmd_Jesd_SerdesInit(fr9009Device_t *device, uint8_t jesdtx_rx);
MYDLL_API uint32_t FR9009_armSpiCmd_Jesd_SerializerParam(fr9009Device_t *device, uint8_t serAmplitude, uint8_t serPreEmphasis);
MYDLL_API uint32_t FR9009_armSpiCmd_Jesd_DeserializerParam(fr9009Device_t *device,uint8_t desEqSetting);
MYDLL_API uint32_t FR9009_armSpiCmd_Jesd_DivSet(fr9009Device_t *device,uint8_t div_n);
MYDLL_API uint32_t FR9009_armSpiCmd_Jesd_state_autorelink(fr9009Device_t *device, uint8_t state);
MYDLL_API uint32_t FR9009_armSpiCmd_sync_en(fr9009Device_t *device, uint8_t enable, uint8_t sync_pll_sel);
MYDLL_API uint32_t FR9009_armSpiCmd_resync(fr9009Device_t *device, uint8_t sync_pll_sel);
MYDLL_API uint32_t FR9009_armSpiCmd_sync_type_ch_set(fr9009Device_t *device, uint8_t sync_pll_sel, uint8_t type_tx_or_rx, CHANNEL_t ch, uint8_t reset_flag);
MYDLL_API uint32_t FR9009_armSpiCmd_sync_dig_en(fr9009Device_t *device, uint8_t enable);
MYDLL_API uint32_t FR9009_armSpiCmd_sync_state_get(fr9009Device_t *device, uint32_t *pstate);
MYDLL_API uint32_t FR9009_armSpiCmd_sync_temp_track_en(fr9009Device_t *device, uint8_t enable, int32_t tempFactor);
MYDLL_API uint32_t FR9009_armSpiCmd_sync_offAfterSync_set(fr9009Device_t *device, uint8_t enable);
MYDLL_API uint32_t FR9009_armSpiCmd_sync_nco(fr9009Device_t *device, uint8_t ch_type, uint8_t enable, uint8_t nco_type);
MYDLL_API uint32_t FR9009_armSpiCmd_fasthop_init(fr9009Device_t *device,uint8_t mode, uint8_t type, CHANNEL_t ch, uint32_t switch_freq_hz, uint32_t switch_nums, uint32_t freq_min_khz, uint32_t freq_max_khz);
MYDLL_API uint32_t FR9009_armSpiCmd_fasthop_freqs_set(fr9009Device_t *device, uint8_t pllsel, int32_t *freq_khz, uint16_t nums);
MYDLL_API uint32_t FR9009_armSpiCmd_fasthop_start(fr9009Device_t *device, uint8_t en);
MYDLL_API uint32_t FR9009_armSpiCmd_fasthop_param_set(fr9009Device_t *device, uint32_t *param, uint8_t offset, uint8_t nums);
MYDLL_API uint32_t FR9009_armSpiCmd_fasthop_trigger_set(fr9009Device_t *device, uint8_t pllsel, fr9009FhmTriggerMode_t trigger_mode, uint8_t pin_num);
MYDLL_API uint32_t FR9009_armSpiCmd_fasthop_next_freq_set(fr9009Device_t *device, uint8_t pllsel, uint64_t freq_hz);
MYDLL_API uint32_t FR9009_armSpiCmd_fasthop_current_freq_get(fr9009Device_t *device, uint64_t *freq_hz);
MYDLL_API uint32_t FR9009_armSpiCmd_fasthop_ch_set(fr9009Device_t *device, CHANNEL_t rx_ch, CHANNEL_t orx_ch, CHANNEL_t tx_ch);
MYDLL_API uint32_t FR9009_armSpiCmd_tx_play_interofdm(fr9009Device_t *device, CHANNEL_t tx_ch, uint8_t bandrate, uint8_t power_db, uint8_t enable);
MYDLL_API uint32_t FR9009_armSpiCmd_rx_play_inter(fr9009Device_t *device, CHANNEL_t rx_ch, uint8_t type, uint8_t enable);
MYDLL_API uint32_t FR9009_armSpiCmd_rx_power_get(fr9009Device_t *device, CHANNEL_t rx_ch, uint8_t cap_point, int32_t left_freq_hz, int32_t right_freq_hz, int32_t *pPower_mdbfs);
MYDLL_API uint32_t FR9009_armSpiCmd_setrf_freq(fr9009Device_t *device, uint32_t freq_KHz);
MYDLL_API uint32_t FR9009_armSpiCmd_SyncAdc_Q(fr9009Device_t *device, uint8_t ch1_sync_q, uint8_t ch2_sync_q);
MYDLL_API uint32_t FR9009_armSpiCmd_pll_freq_set(fr9009Device_t *device, fr9009RfPllName_t pll_type, uint64_t freq_Hz);
MYDLL_API uint32_t FR9009_armSpiCmd_pll_OnOff_set(fr9009Device_t *device, fr9009RfPllName_t pll_type, uint8_t on_off);
MYDLL_API uint32_t FR9009_armSpiCmd_pll_tempratureEn_set(fr9009Device_t *device, uint8_t pllsel);
MYDLL_API uint32_t FR9009_armSpiCmd_pll_status_gpio_set(fr9009Device_t *device, uint8_t pllsel, uint8_t bb_gpio_num, uint8_t rf_gpio_num, uint8_t orf_gpio_num);
MYDLL_API uint32_t FR9009_armSpiCmd_pll_Opt_set(fr9009Device_t *device, uint8_t pllsel, uint8_t pllFAFCsel, uint8_t pllDither, uint8_t opt);
MYDLL_API uint32_t FR9009_armSpiCmd_pll_freq_get(fr9009Device_t *device, fr9009RfPllName_t pll_type, uint64_t *freq_Hz);
MYDLL_API uint32_t FR9009_armSpiCmd_VCOCfg_set(fr9009Device_t *device, VCOConfig_t vco_type, uint8_t div_n, uint64_t vco_freq_hz, uint8_t en);
MYDLL_API uint32_t FR9009_armSpiCmd_PllMerge_set(fr9009Device_t *device, uint8_t pllsel, uint8_t en);
MYDLL_API uint32_t FR9009_armSpiCmd_AGC_CALI(fr9009Device_t *device, CHANNEL_t ch);
MYDLL_API uint32_t FR9009_armSpiCmd_cali_cplxfir(fr9009Device_t *device, CHANNEL_t cali_ch, uint8_t tx_rx_orxsel);
MYDLL_API uint32_t FR9009_armSpiCmd_playcap_set(fr9009Device_t *device, uint8_t mode, uint8_t continue_mode, uint8_t start_pluse, uint8_t sel_AB, uint8_t div, CHANNEL_t ch, uint16_t src);
MYDLL_API uint32_t FR9009_armSpiCmd_playcap_manual_start(fr9009Device_t *device, uint8_t start_ab);
MYDLL_API uint32_t FR9009_armSpiCmd_TddManualsel(fr9009Device_t *device, uint8_t mode);
MYDLL_API uint32_t FR9009_armSpiCmd_orxrx_ch_en(fr9009Device_t *device, fr9009RxORxChannels_t ch);
// MYDLL_API uint32_t FR9009_armSpiCmd_orxrx_bw_set(fr9009Device_t *device, uint8_t bw);
MYDLL_API uint32_t FR9009_armSpiCmd_orx_gain_set(fr9009Device_t *device, CHANNEL_t ch, uint32_t att_mdb);
MYDLL_API uint32_t FR9009_armSpiCmd_orx_gain_get(fr9009Device_t *device, CHANNEL_t ch, uint32_t *patt_mdb);
MYDLL_API uint32_t FR9009_armSpiCmd_tx_ch_en(fr9009Device_t *device, uint8_t tx_ch);
MYDLL_API uint32_t FR9009_armSpiCmd_tx_ch_ATT_set(fr9009Device_t *device, uint8_t tx_ch, uint32_t att_mdb);
MYDLL_API uint32_t FR9009_armSpiCmd_tx_ch_ATT_get(fr9009Device_t *device, uint8_t tx_ch, uint32_t *patt_mdb);
MYDLL_API uint32_t FR9009_armSpiCmd_tx_PowerCtrl_set(fr9009Device_t *device, uint8_t tx_ch, uint8_t detect_mode, int32_t power_mdbfs, uint32_t duration_cycle, uint8_t peak_errcnt);
MYDLL_API uint32_t FR9009_armSpiCmd_tx_PowerCtrl_ErrGet(fr9009Device_t *device, uint32_t *pPowerErr);
MYDLL_API uint32_t FR9009_armSpiCmd_tx_PowerCtrl_ErrReset(fr9009Device_t *device, uint8_t tx_ch);
MYDLL_API uint32_t FR9009_armSpiCmd_tx_bw_set(fr9009Device_t *device, CHANNEL_t tx_ch, uint8_t bw);
MYDLL_API uint32_t FR9009_armSpiCmd_rx_bw_set(fr9009Device_t *device, CHANNEL_t ch, uint8_t bw);
MYDLL_API uint32_t FR9009_armSpiCmd_orx_bw_set(fr9009Device_t *device, CHANNEL_t ch, uint8_t bw);
MYDLL_API uint32_t FR9009_armSpiCmd_decmode_set(fr9009Device_t *device, uint8_t Dec5DecimationEn);
MYDLL_API uint32_t FR9009_armSpiCmd_tx_Lo_set(fr9009Device_t *device, CHANNEL_t ch, fr9009LoSource_t losel);
MYDLL_API uint32_t FR9009_armSpiCmd_rx_Lo_set(fr9009Device_t *device, CHANNEL_t ch, fr9009LoSource_t losel);
MYDLL_API uint32_t FR9009_armSpiCmd_orx_Lo_set(fr9009Device_t *device, CHANNEL_t ch, fr9009LoSource_t losel);
MYDLL_API uint32_t FR9009_armSpiCmd_tx_Lo_get(fr9009Device_t *device, CHANNEL_t ch, fr9009LoSource_t *losel);
MYDLL_API uint32_t FR9009_armSpiCmd_rx_Lo_get(fr9009Device_t *device, CHANNEL_t ch, fr9009LoSource_t *losel);
MYDLL_API uint32_t FR9009_armSpiCmd_orx_Lo_get(fr9009Device_t *device, CHANNEL_t ch, fr9009LoSource_t *losel);
MYDLL_API uint32_t FR9009_armSpiCmd_LIF_mode_set(fr9009Device_t *device, uint8_t ch_type, CHANNEL_t ch, uint8_t mode);
MYDLL_API uint32_t FR9009_armSpiCmd_LIF_freq_set(fr9009Device_t *device, uint8_t ch_type, CHANNEL_t ch, int32_t freq_hz);
MYDLL_API uint32_t FR9009_armSpiCmd_RFDirect_mode_set(fr9009Device_t *device, uint8_t ch_type, CHANNEL_t ch, uint8_t mode);
MYDLL_API uint32_t FR9009_armSpiCmd_flatness_default_set(fr9009Device_t *device, uint8_t ch_type, CHANNEL_t ch);
MYDLL_API uint32_t FR9009_armSpiCmd_cfir_coefs_set(fr9009Device_t *device, uint8_t ind, uint8_t symmetric, uint8_t even, uint8_t fir_len, int16_t *coefs);
MYDLL_API uint32_t FR9009_armSpiCmd_cfir_coefs_selInd_set(fr9009Device_t *device, uint8_t ch_type, CHANNEL_t ch, uint8_t select_ind);
MYDLL_API uint32_t FR9009_armSpiCmd_tx_NCO_set(fr9009Device_t *device, uint8_t nco_ch, uint8_t tx_ch, int32_t freq1_khz, int32_t freq2_khz);
MYDLL_API uint32_t FR9009_armSpiCmd_tx_ncopower_set(fr9009Device_t *device, uint8_t tx_ch, int32_t powerda1, int32_t powerda2);
MYDLL_API uint32_t FR9009_armSpiCmd_ch_signal_ripple_param_set(fr9009Device_t *device, uint8_t ch_type, CHANNEL_t ch, int32_t *wrDataBuf, uint32_t wrWordSz);
MYDLL_API uint32_t FR9009_armSpiCmd_test_func(fr9009Device_t *device, int32_t data);
MYDLL_API uint32_t FR9009_armSpiCmd_cap(fr9009Device_t *device, uint8_t start_pluse, uint8_t sel_AB, uint8_t div, CHANNEL_t ch, uint16_t src, uint32_t *buf, uint32_t len);
MYDLL_API uint32_t FR9009_armSpiCmd_play(fr9009Device_t *device, uint8_t start_pluse, uint8_t sel_AB, uint8_t div, CHANNEL_t ch, uint16_t src, uint32_t *buf, uint32_t len);
MYDLL_API uint32_t FR9009_armSpiCmd_capplay_stop(fr9009Device_t *device);


MYDLL_API uint16_t bswap_16bit(uint16_t x);
MYDLL_API uint32_t bswap_32bit(uint32_t x);




#ifdef __cplusplus
}
#endif

#endif


