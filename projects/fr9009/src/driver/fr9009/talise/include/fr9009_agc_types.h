/**
 * \file fr9009_agc_types.h
 * \brief Contains FR9009 API AGC data types
 *
 * Copyright 2022 BriRadio.
 * Released under the FR9009 API license, for more information see the "LICENSE.txt" file in this zip file.
 */

#ifndef FR9009_AGC_TYPES_H_
#define FR9009_AGC_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  \brief Data structure to hold AGC peak settings
 *  The evaluation software GUI for the product can be used to generate a structure with suggested settings.
 */
typedef struct {
	uint32_t agcUnderRangeLowInterval_ns;        /*!< Update interval for AGC loop mode in nanoseconds */
	uint8_t agcUnderRangeMidInterval;           /*!< 2nd update interval for multiple time constant AGC mode. Calculated as (agcUnderRangeMidInterval+1)*agcUnderRangeLowInterval_ns. Valid range is 0 to 63 */
	uint8_t agcUnderRangeHighInterval;          /*!< 3rd update interval for multiple time constant AGC mode. Calculated as (agcUnderRangeHighInterval+1)*2nd update interval. Valid range is 0 to 63 */
	uint8_t apdHighThresh;                      /*!< NO USED AGC APD high threshold. Valid range is 7 to 49 */
	uint8_t apdLowGainModeHighThresh;           /*!<  NO USED AGC APD high threshold in low gain mode. Valid range is 7 to 49. Recommended to be 3dB above apdHighThresh */
	uint8_t apdLowThresh;                       /*!<  NO USED AGC APD low threshold. Valid range is 7 to 49. Recommended to be 3dB below apdHighThresh */
	uint8_t apdLowGainModeLowThresh;            /*!<  NO USED AGC APD low threshold in low gain mode. Valid range is 7 to 49. Recommended to be 3dB above apdLowThresh */
	uint8_t apdUpperThreshPeakExceededCnt;      /*!< AGC APD peak detect upper threshold count. Valid range is 0 to 255 */
	uint8_t apdLowerThreshPeakExceededCnt;      /*!< AGC APD peak detect lower threshold count. Valid range is 0 to 255 */
	uint8_t apdGainStepAttack;                  /*!< AGC APD peak detect attack gain step. Valid range is 0 to 31 */
	uint8_t apdGainStepRecovery;                /*!< MUST 0 AGC APD gain index step size for recovery. Valid range is 0 to 31 */
	uint8_t enableHb2Overload;                  /*!< Enable or disables the HB2 overload detector. */
	uint8_t hb2OverloadDurationCnt;             /*!< Sets the window of clock cycles (at the HB2 output rate) to meet the overload count. (0 = 2 cycles, 1 = 4 cycles, 2 = 8 cycles, 3 = 12 cycles, 4 = 16 cycles, 5 = 24 cycles, 6 = 32 cycles) */
	uint8_t hb2OverloadThreshCnt;               /*!< Sets the number of actual overloads required to trigger the overload signal. Valid range from 1 to 15 */
	uint16_t hb2HighThresh;                      /*!< AGC HB2 output high threshold. Valid range from  0 to 2^13 8192* squrt(2) * 10^(-6/20) = -6 dBfs*/
	uint16_t hb2UnderRangeLowThresh;             /*!< AGC HB2 output low threshold. Valid range from  0 to 2^13 */
	uint16_t hb2UnderRangeMidThresh;             /*!< AGC HB2 output low threshold for 2nd interval for multiple time constant AGC mode. Valid range from  0 to 2^13 */
	uint16_t hb2UnderRangeHighThresh;            /*!< AGC HB2 output low threshold for 3rd interval for multiple time constant AGC mode. Valid range from  0 to 2^13 */
	uint8_t hb2UpperThreshPeakExceededCnt;      /*!< AGC HB2 output upper threshold count. Valid range from  0 to 255 */
	uint8_t hb2LowerThreshPeakExceededCnt;      /*!< AGC HB2 output lower threshold count. Valid range from  0 to 255 */
	uint8_t hb2GainStepHighRecovery;            /*!< AGC HB2 gain index step size. Valid range from  0 to 31, step（default 0.5db） power should < (High-UnderRangeHigh)/2*/
	uint8_t hb2GainStepLowRecovery;             /*!< AGC HB2 gain index step size, when the HB2 Low Overrange interval 2 triggers. Valid range from  0 to 31*/
	uint8_t hb2GainStepMidRecovery;             /*!< AGC HB2 gain index step size, when the HB2 Low Overrange interval 3 triggers. Valid range from  0 to 31, step（default 0.5db） power should < (High-hb2UnderRangeMidThresh) - 3*/
	uint8_t hb2GainStepAttack;                  /*!< AGC HB2 output attack gain step. Valid range from  0 to 31 ,step（default 0.5db） power should < (High-UnderRangeHigh) - 1*/
	uint8_t hb2OverloadPowerMode;               /*!< When this bit is set, √(I² + Q²) compare with thresh, else max(abs(I), abs(Q)), default 1*/
	uint8_t hb2OvrgSel;                         /*!< NO USED */
	uint8_t hb2ThreshConfig;                    /*!< NO USED   Initialized to 0x03 */
	uint8_t hb2UnderRangeLowThreshExceededCnt;  /*!< AGC HB2 low overrange interval 0 threshold count. Optional. Valid range from 1 to 255. Passing 0 will result in the reset value of 3. */
	uint8_t hb2UnderRangeMidThreshExceededCnt;  /*!< AGC HB2 mid overrange interval 1 threshold count. Optional. Valid range from 1 to 255. Passing 0 will result in the reset value of 3. */

} fr9009AgcPeak_t;

/**
 * \brief Data structure to hold AGC power settings
 *  The evaluation software GUI for the product can be used to generate a structure with suggested settings.
 */
typedef struct {
	uint8_t powerEnableMeasurement;                 /*!< Enable the Rx power measurement block. (0/1) */
	uint8_t powerUseRfirOut;                        /*!< Use output of Rx PFIR for power measurement. (0/1) */
	uint8_t powerUseBBDC2;                          /*!< Use output of DC offset block for power measurement. (0/1) */
	uint8_t underRangeHighPowerThresh;              /*!< AGC power measurement detect lower 0 threshold. Valid Range from 1 to 80. 1 == -1dBFS*/
	uint8_t underRangeLowPowerThresh;               /*!< AGC power measurement detect lower 1 threshold. Valid offset from 1 to 80. 1 == -1dBFS */
	uint8_t underRangeHighPowerGainStepRecovery;    /*!< AGC power measurement detect lower 0 recovery gain step. Valid range from  0 to 31 */
	uint8_t underRangeLowPowerGainStepRecovery;     /*!< AGC power measurement detect lower 1 recovery gain step. Valid range from  0 to 31 */
	uint8_t powerMeasurementDuration;               /*!< Average power measurement duration = 2^powerMeasurementDuration datapath clk(1/(4*ref @ bb = ref * 80)). Valid range from 2 to 20 */
	uint16_t rx1TddPowerMeasDuration;                /*!< Measurement duration to detect power for specific slice of the gain update counter. duration = 2^powerMeasurementDuration samples. Valid range from 0 to 21 */
	uint16_t rx1TddPowerMeasDelay;                   /*!< Measurement delay to detect power for specific slice of the gain update counter. */
	uint16_t rx2TddPowerMeasDuration;                /*!< Measurement duration to detect power for specific slice of the gain update counter. */
	uint16_t rx2TddPowerMeasDelay;                   /*!< Measurement delay to detect power for specific slice of the gain update counter. */
	uint8_t upper0PowerThresh;                      /*!< AGC upper 0 (overRangeHighPowerThreshold) threshold for power measurement. Valid Range from 1 to 80. 1 == -1dBFS*/
	uint8_t upper1PowerThresh;                      /*!< AGC upper 1 (overRangeLowPowerThreshold)  threshold for power measurement. Valid Range from 1 to 80. 1 == -1dBFS*/
	uint8_t powerLogShift;                          /*!< no used when set agc power */
	uint8_t overRangeLowPowerGainStepAttack;        /*!< AGC inner upper threshold exceeded attack gain step. Optional. Valid range from  1 to 31. Passing 0 will result in the reset value of 4. */
	uint8_t overRangeHighPowerGainStepAttack;       /*!< AGC outer high power threshold exceeded attack gain step. Optional. Valid range from  1 to 31. Passing 0 will result in the reset value of 4. */

} fr9009AgcPower_t;

/**
 * \brief Data structure to hold all AGC configuration settings for initialization
 *  The evaluation software GUI for the product can be used to generate a structure with suggested settings.
 */
typedef struct {
	uint8_t agcPeakWaitTime;                    /*!< AGC peak wait time. Valid range is from 0 to 31 */
	uint8_t agcRx1MaxGainIndex;                 /*!< AGC Rx1 max gain index. Valid range is from 0 to 255 */
	uint8_t agcRx1MinGainIndex;                 /*!< AGC Rx1 min gain index. Valid range is from 0 to 255 */
	uint8_t agcRx2MaxGainIndex;                 /*!< AGC Rx2 max gain index. Valid range is from 0 to 255 */
	uint8_t agcRx2MinGainIndex;                 /*!< AGC Rx2 min gain index. Valid range is from 0 to 255 */
	uint32_t agcGainUpdateCounter_us;            /*!< AGC gain update time in micro seconds */
	uint8_t agcRx1AttackDelay;                  /*!< On entering Rx, the Rx1 AGC is kept inactive for a period = agcRx1AttackDelay*1us */
	uint8_t agcRx2AttackDelay;                  /*!< On entering Rx, the Rx2 AGC is kept inactive for a period = agcRx2AttackDelay*1us */
	uint8_t agcSlowLoopSettlingDelay;           /*!< On any gain change, the AGC waits for the time (range 0 to 127) specified in AGC clock cycles to allow gain transients to flow through the Rx path before starting any measurements. */
	uint8_t agcLowThreshPreventGain;            /*!< Prevent gain index from incrementing if peak thresholds are being exceeded */
	uint8_t agcChangeGainIfThreshHigh;          /*!< Enable immediate gain change if high threshold counter is exceeded. Bit 0 enables ULB high threshold, Bit 1 enables HB2 high threshold */
	uint8_t agcPeakThreshGainControlMode;       /*!< Enable gain change based only on the signal peak threshold over-ranges. Power based AGC changes are disabled in this mode. */
	uint8_t agcResetOnRxon;                     /*!< Reset the AGC slow loop state machine to max gain when the Rx Enable is taken low */
	uint8_t agcEnableSyncPulseForGainCounter;   /*!< Enable the AGC gain update counter to be sync'ed to a time-slot boundary. */
	uint8_t agcEnableFastRecoveryLoop;          /*!< Enable multiple time constants in AGC loop for fast attack and fast recovery. */

	fr9009AgcPower_t agcPower;
	fr9009AgcPeak_t agcPeak;

} fr9009AgcCfg_t;

/**
 * \brief Data structure to hold all AGC Dualband configuration.
 * Used along with fr9009RxChannels_t structure to setup AGC for Rx1 and/or Rx2
 */
typedef struct {
	uint8_t agcDualBandEnable;                  /*!< Enable AGC operation for dualband receiver  */
	uint8_t agcRxDualbandExtTableUpperIndex;    /*!< Rx1/2 AGC dual band operation - Indicates the gain table index below which the AGC prioritizes
                                                     decreasing gain through external LNA control over the Front-end gain  */
	uint8_t agcRxDualbandExtTableLowerIndex;    /*!< Rx1/2 AGC dual band operation - Indicates the gain table index above which the AGC prioritizes
                                                     increasing gain through external LNA control over the Front-end gain  */
	uint8_t agcDualbandPwrMargin;               /*!< Margin for comparing total power against power of individual bands. If
                                                     Total power > Upper Band Power + Lower Band Power +margin, the signal contains other components
                                                     than the two bands, and AGC should behave like a single band system. Margin is in 0.5db steps. */
	uint8_t agcDualbandLnaStep;                 /*!< Margin to compare Upper band power versus Lower band power
                                                     (for Upper Band Power > Lower Band Power + margin, and Lower Band Power > Upper Band Power + margin checks).
                                                     The margin compares the powers of the bands to change the LNA of one band so that powers of the bands match.
                                                     Value is in 0.5db resolution  */
	uint8_t agcDualbandHighLnaThreshold;        /*!< High threshold for Upper band or Lower band power above which the LNA index is decreased.
                                                     Value is in 0.5db */
	uint8_t agcDualbandLowLnaThreshold;         /*!< Low threshold for Upper band or Lower band power below which the LNA index is increased.
                                                     Value is in 0.5db */
	uint8_t dualBandGpioEnable;                 /*!< Enable the 3.3V GPIO's that would be used to drive the external LNA's.
                                                     Rx1 : GPIO3.3[1:0] controls Lower band LNA, GPIO3.3[3:2] controls Upper band LNA
                                                     Rx2 : GPIO3.3[5:4] controls Lower band LNA, GPIO3.3[7:6] controls Upper band LNA
                                                     If disabled, user needs to use FR9009_getDualBandLnaControls() to read back the LNA control
                                                     value through the SPI. */
	uint8_t decPowerDdcMeasurementDuration;     /*!< Power measurement duration for measuring the power of the individual bands. This variable has
                                                     a range of 0 to 31. The sampling period is calculated as = 8 x 2^decPowerDdcMeasurementDuration. */
} fr9009AgcDualBandCfg_t;

/**
 * \brief Data structure to hold AGC Dualband LNA controls for the external LNA's.
 * Used along with fr9009RxChannels_t structure to read back LNA controls for Rx1 or Rx2
 */
typedef struct {
	uint8_t rxLowerBandLnaControl;         /* The control value for the Rx1/2 Lower band LNA (Values 0-3) */
	uint8_t rxUpperBandLnaControl;         /* The control value for the Rx1/2 Upper band LNA (Values 0-3) */
} fr9009DualBandLnaControls_t;

#ifdef __cplusplus
}
#endif

#endif /* FR9009_AGC_TYPES_H_ */
