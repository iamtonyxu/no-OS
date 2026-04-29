/**
 * \file fr9009_jesd204_types.h
 * \brief Contains FR9009 API JESD data types
 *
 * Copyright 2022 BriRadio.
 * Released under the FR9009 API license, for more information see the "LICENSE.txt" file in this zip file.
 */

#ifndef FR9009_JESD204_TYPES_H_
#define FR9009_JESD204_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  \brief Enum to select desired framer
 */
typedef enum {
	FR_FRAMER_NONE = 0,       /*!< Framer not selection */
	FR_FRAMER_A = 1,       /*!< Framer A selection */
	FR_FRAMER_B,           /*!< Framer B selection */
	FR_FRAMER_A_AND_B      /*!< Used for cases where Rx1 uses one framer, Rx2 uses the second framer */
} fr9009FramerSel_t;
/**
 *  \brief Enum to select desired framer map
 */
typedef enum {
	FR_FRAMER_MAP_RX = 0x1,       /*!< Framer RX_MAP selection */
	FR_FRAMER_MAP_ORX = 0x2,           /*!< Framer ORX_MAP selection */
	FR_FRAMER_MAP_RX_AND_ORX  = 0x3,      /*!< Used for cases where Rx uses RX_MAP, ORx uses the ORX_MAP */
} fr9009FramerMapSel_t;

/**
 *  \brief Enum to select desired deframer
 */
typedef enum {
	FR_DEFRAMER_A = 0,    /*!< Deframer A selection */
	// FR_DEFRAMER_B,        /*!< Deframer B selection */
	// FR_DEFRAMER_A_AND_B   /*!< Used for cases where Tx1 uses one deframer, Tx2 uses the second deframer */
} fr9009DeframerSel_t;

/**
 * \brief Enum of possible Framer Test Data sources
 */
typedef enum {
	FR_TEST_DATA_DISABLE =0,
	FR_FTD_PRBS7,           		/*!< Framer test data PRBS7 data source */
	FR_FTD_PRBS9,              	/*!< Framer test data PRBS9 data source */
	FR_FTD_PRBS11,                 /*!< Framer test data PRBS11 data source */
	FR_FTD_PRBS15,                 /*!< Framer test data PRBS15 data source */
	FR_FTD_PRBS20,                 /*!< Framer test data PRBS20 data source */
	FR_FTD_PRBS23,                  /*!< Framer test data PRBS23 data source */
	FR_FTD_PRBS31,                  /*!< Framer test data PRBS31 data source */
	FR_FTD_ADC_DATA,           /*!< Framer test data ADC data source */
	FR_FTD_CHECKERBOARD,             /*!< Framer test data checkerboard data source*/

} fr9009FramerDataSource_t;

/**
 *  \brief Enum of Framer test data injection points
 */
typedef enum {
	FR_FTD_FRAMERINPUT = 0,        /*!< Framer test data injection point at framer input */
	FR_FTD_SERIALIZER,             /*!< Framer test data injection point at serializer input */
	FR_FTD_POST_LANEMAP            /*!< Framer test data injection point after lane mapping */

} fr9009FramerInjectPoint_t;

/**
 * \brief Enum for JESD204B PRBS generated types
 */
typedef enum {
	FR_PRBS_DISABLE = 0,       /*!< Deframer PRBS pattern disable */
	FR_PRBS7,                  /*!< Deframer PRBS7 pattern select */
	FR_PRBS9,                  /*!< Deframer PRBS9 pattern select */
	FR_PRBS11,                 /*!< Deframer PRBS11 pattern select */
	FR_PRBS15,                 /*!< Deframer PRBS15 pattern select */
	FR_PRBS20,                 /*!< Deframer PRBS20 pattern select */
	FR_PRBS23,                 /*!< Deframer PRBS23 pattern select */
	FR_PRBS31                  /*!< Deframer PRBS31 pattern select */

} fr9009DeframerPrbsOrder_t;

/**
 * \brief Enum for JESD204B deserializer / deframer PRBS selection
 */
typedef enum {
	FR_PRBSCHECK_LANEDATA = 0, /*!< Check PRBS at deserializer lane output (does not require JESD204b link) */
	FR_PRBSCHECK_SAMPLEDATA    /*!< Check PRBS at output of deframer (JESD204b deframed sample) */

} fr9009DefPrbsCheckLoc_t;


typedef enum{
	FR_DEFRAMER_L0_OUT0 = 0x0,               /*!< L0 0, Deframer out 0 */
	FR_DEFRAMER_L0_OUT1 = 0x1,               /*!< L0 1, Deframer out 1 */
	FR_DEFRAMER_L1_OUT0 = 0x2,               /*!< L1 0, Deframer out 2 */
	FR_DEFRAMER_L1_OUT1 = 0x3,               /*!< L1 1, Deframer out 3 */
	FR_DEFRAMER_L2_OUT0 = 0x4,               /*!< L2 0, Deframer out 4 */
	FR_DEFRAMER_L2_OUT1 = 0x5,               /*!< L2 1, Deframer out 5 */
	FR_DEFRAMER_L3_OUT0 = 0x6,               /*!< L3 0, Deframer out 6 */
	FR_DEFRAMER_L3_OUT1 = 0x7,               /*!< L3 1, Deframer out 7 */
	FR_DEFRAMER_OUT_ONE = 0x8,               /*!< 4 lane out 8个16bit */
}fr9009DeframeXbarSelect_t;
#define FR_DEFRAMER_LANE_OUT_DIFF	0x2		/*相邻两条lane的out差值，计算偏差使用*/

typedef enum{
	FR_ADC_RX1_I_IND0 = 0x0 ,                    /*!<  Rx1 I data(n-0) */
	FR_ADC_RX1_Q_IND0 = 0x1 ,                    /*!<  Rx1 Q data(n-0) */
	FR_ADC_RX2_I_IND0 = 0x2 ,                    /*!<  Rx2 I data(n-0) */
	FR_ADC_RX2_Q_IND0 = 0x3 ,                    /*!<  Rx2 Q data(n-0) */
	FR_ADC_RX_SAMEDATA_OFFSET = 0x4 ,                    /*!< FR_ADC_RX1_I_IND0 与FR_ADC_RX1_I_IND1 的偏移量，其他相同  */
}fr9009FrameXbarSelect_t;

typedef struct {
	fr9009DeframeXbarSelect_t	dacChanI[4];    /*!< Sample Crossbar select for I channel data*/
	fr9009DeframeXbarSelect_t	dacChanQ[4];    /*!< Sample Crossbar select for Q channel data*/
}fr9009DeframeXbar_t;

typedef struct
{
	fr9009FrameXbarSelect_t lane[4][4];			/* lane[0] lane0上发送的数据优先发送[0],然后发送[1]，……，根据F配置发送长度，至少是[0-1]*/
}fr9009FrameXbar_t;

/**
 *  \brief Data structure to hold FR9009 JESD204b DeFramer ILAS configuration settings
 */
typedef struct {
	uint8_t DID;                          /*!< JESD204B Configuration Device ID for ILAS check */
	uint8_t BID;                          /*!< JESD204B Configuration Bank ID for ILAS check */
	uint8_t LID0;                         /*!< JESD204B Configuration starting Lane ID for ILAS check */
	uint8_t L;                            /*!< JESD204B Configuration L = lanes per data converter for ILAS check */
	uint8_t SCR;                          /*!< JESD204B Configuration scramble setting for ILAS check */
	uint8_t F;                            /*!< JESD204B Configuration F = octets per frame for ILAS check */
	uint8_t K;                            /*!< JESD204B Configuration K = frames per multiframe for ILAS check */
	uint8_t M;                            /*!< JESD204B Configuration M = number of data converters for ILAS check */
	uint8_t N;                            /*!< JESD204B Configuration N = data converter sample resolution for ILAS check */
	uint8_t CS;                           /*!< JESD204B Configuration CS = number of control bits transferred per sample per frame for ILAS check */
	uint8_t NP;                           /*!< JESD204B Configuration NP = JESD204B word size based on the highest resolution of the data converter for ILAS check */
	uint8_t S;                            /*!< JESD204B Configuration S = number of samples/data converter/frame for ILAS check */
	uint8_t CF;                           /*!< JESD204B Configuration CF = '0' = control bits appended to each sample, '1' = appended to end of frame for ILAS check */
	uint8_t HD;                           /*!< JESD204B Configuration HD = high density bit - samples are contained within lane (0) or divided over more than one lane (1) for ILAS check */
	uint8_t FCHK0;                        /*!< JESD204B Configuration checksum for ILAS check lane0 */
	uint8_t FCHK1;                        /*!< JESD204B Configuration checksum for ILAS check lane1 */
	uint8_t FCHK2;                        /*!< JESD204B Configuration checksum for ILAS check lane2 */
	uint8_t FCHK3;                        /*!< JESD204B Configuration checksum for ILAS check lane3 */
} fr9009Jesd204bLane0Config_t;

/**
 *  \brief Data structure to hold FR9009 JESD204b Framer configuration settings
 */
typedef struct {
	uint8_t bankId;                     /*!< JESD204B Configuration Bank ID extension to Device ID. Range is 0..15 */
	uint8_t deviceId;                   /*!< JESD204B Configuration Device ID link identification number. Range is 0..255 */
	uint8_t lane0Id;                    /*!< JESD204B Configuration starting Lane ID. If more than one lane is used, each lane will increment from the Lane0 ID. Range is 0..31 */
	uint8_t M;                          /*!< Number of ADCs (0, 2, or 4) where 2 ADCs are required per receive chain (I and Q) */
	uint8_t K;                          /*!< Number of frames in a multiframe. Default = 32, F*K must be modulo 4. Where, F=2*M/numberOfLanes */
	uint8_t F;                          /*!< Number of bytes(octets) per frame (Valid 1, 2, 4, 8) */
	uint8_t Np;                         /*!< converter sample resolution (12, 16, 24) */
	uint8_t scramble;                   /*!< Scrambling off if framerScramble = 0, if framerScramble > 0 scrambling is enabled */
	uint8_t externalSysref;             /*!< External SYSREF select. 0 = use internal SYSREF(not currently valid), 1 = use external SYSREF */	//todo clk_reset 0x80 inter_refclk_sel[24]
	uint8_t serializerLanesEnabled;     /*!< Serializer lane select bit field. Where, [0] = Lane0 enabled, [1] = Lane1 enabled, etc */
	uint8_t serializerLaneCrossbar;     /*!< Lane crossbar to map framer lane outputs to physical lanes */
	uint8_t lmfcOffset;                 /*!< LMFC offset value for deterministic latency setting.  Range is 0..31 */
	uint8_t syncbInSelect;              /*!< Selects SYNCb input source. Where, 0 = use SYNCBIN0 for this framer, 1 = use SYNCBIN1 for this framer */
	uint8_t syncbInLvdsMode;            /*!< 1 - enable LVDS input pad with 100ohm internal termination, 0 - enable CMOS input pad */	//todo delate
	uint8_t syncbInLvdsPnInvert;        /*!< 0 - syncb LVDS PN not inverted, 1 - syncb LVDS PN inverted */	// 0x10 syncb_in_level[1]
	uint8_t enableManualLaneXbar;       /*!< 0 - Automatic Lane crossbar mapping, 1 - Manual Lane crossbar mapping (use serializerLaneCrossbar value with no checking) */
	uint8_t MFdisableCharReplacement;   /*!< 0 - replacement the same char with the K code, 1 - 多帧边界不进行K码替换  */
	uint8_t sysrefDisable;       		/*!< 1 - frame A /B ignore external sysref, 0 - used external sysref */
	uint8_t sysrefOneShot;     			/*!< 0 - normal connect with sysref, 1 - jesd connect only use whitched first sysref */	
} fr9009Jesd204bFramerConfig_t;

/**
* \brief Data structure to hold the settings for the deframer configuration
*/
typedef struct {
	uint8_t bankId;                     /*!< Extension to Device ID. Range is 0..15 */
	uint8_t deviceId;                   /*!< Link identification number. Range is 0..255 */
	uint8_t lane0Id;                    /*!< Lane0 ID. Range is 0..31 */
	uint8_t M;                          /*!< Number of DACs (0, 2, or 4) - 2 DACs per transmit chain (I and Q) */
	uint8_t K;                          /*!< Number of frames in a multiframe. Default = 32, F*K = modulo 4. Where, F=2*M/numberOfLanes */
	uint8_t scramble;                   /*!< Scrambling off if scramble = 0, if framerScramble > 0 scrambling is enabled */
	uint8_t externalSysref;             /*!< External SYSREF select. 0 = use internal SYSREF, 1 = external SYSREF */	//与rx 共用
	uint8_t deserializerLanesEnabled;   /*!< Deserializer lane select bit field. Where, [0] = Lane0 enabled, [1] = Lane1 enabled, etc */
	uint8_t deserializerLaneCrossbar;   /*!< Lane crossbar to map deframer lane outputs to physical lanes */
	uint8_t lmfcOffset;                 /*!< LMFC offset value to adjust deterministic latency. Range is 0..31 */
	uint8_t syncbOutSelect;             /*!< Selects deframer SYNCBOUT pin (0 = SYNCBOUT0, 1 = SYNCBOUT1) */
	uint8_t Np;                         /*!< converter sample resolution (12, 16) */
	uint8_t syncbOutLvdsPnInvert;       /*!< 0 - syncb LVDS PN not inverted, 1 - syncb LVDS PN inverted */	//0x18 syncb_out_level[14]
	uint8_t enableManualLaneXbar;       /*!< 0 - Automatic Lane crossbar mapping, 1 - Manual Lane crossbar mapping (use deserializerLaneCrossbar value with no checking) */
	// uint8_t MFdisableCharReplacement;   /*!< 0 - replacement the same char with the K code, 1 - 多帧边界不进行K码替换  */
	uint8_t sysrefDisable;       		/*!< 1 - frame A /B ignore external sysref, 0 - used external sysref */
	uint8_t sysrefOneShot;     			/*!< 0 - normal connect with sysref, 1 - jesd connect only use whitched first sysref */	
} fr9009Jesd204bDeframerConfig_t;

/**
 *  \brief Data structure to hold FR9009 JESD204b Framer and Deframer configuration information
 */
typedef struct {
	fr9009Jesd204bFramerConfig_t	framerA;       /*!< Framer A configuration data structure */
	fr9009Jesd204bFramerConfig_t	framerB;       /*!< Framer B configuration data structure */
	fr9009Jesd204bDeframerConfig_t	deframerA;   /*!< Deframer A configuration data structure */
	uint8_t serAmplitude;                       /*!< Serializer amplitude setting. Default = 15. Range is 0..15 */
	uint8_t serPreEmphasis;                     /*!< Serializer pre-emphasis setting. Default = 1 Range is 0..4 */
	uint8_t serInvertLanePolarity;              /*!< Serializer Lane PN inversion select. Default = 0. Where, bit[0] = 1 will invert lane [0], bit[1] = 1 will invert lane 1, etc. */	//todo 贾老师
	uint8_t desInvertLanePolarity;              /*!< Deserializer Lane PN inversion select.  bit[0] = 1 Invert PN of Lane 0, bit[1] = Invert PN of Lane 1, etc */
	uint8_t desEqSetting;                       /*!< Deserializer Equalizer setting. Applied to all deserializer lanes. Range is 0..2 (default 2, 0 = max boost) */
	uint8_t sysrefLvdsMode;                     /*!< 1 - enable LVDS Input pad with 100ohm internal termination, 0 - enable CMOS input pad *///todo delate
	uint8_t sysrefLvdsPnInvert;                 /*!< 0 - sysref LVDS PN is not inverted, 1 - sysref LVDS PN is inverted *///todo delate
} fr9009JesdSettings_t;


#ifdef __cplusplus
}
#endif

#endif /* FR9009_JESD204_TYPES_H_ */
