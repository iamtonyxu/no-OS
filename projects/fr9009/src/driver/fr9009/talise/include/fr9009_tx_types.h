/**
 * \file fr9009_tx_types.h
 * \brief Contains FR9009 API Tx datapath data types
 *
 * Copyright 2022 BriRadio.
 * Released under the FR9009 API license, for more information see the "LICENSE.txt" file in this zip file.
 */

#ifndef FR9009_TX_TYPES_H_
#define FR9009_TX_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  \brief Enum of possible Tx channel enables
 */
typedef enum
{
    FR_TXOFF = 0,      /*!< No Tx channels are enabled */
    FR_TX1	  =0x01,            /*!< Tx1 channel enabled */
    FR_TX2	  =0x02,            /*!< Tx2 channel enabled */
    FR_TX1TX2=0x03,          /*!< Tx1 + Tx2 channels enabled */
} fr9009TxChannels_t;

/**
 *  \brief Enum of possible Tx DAC FS boost options
 */
typedef enum
{
    FR_DACFS_0DB 	= 0x0,	/*!< No Full Scale Boost */
    FR_DACFS_3DB	= 0x1F  /*!< Full scale boost = 3dB */
} fr9009DacFullScale_t;

/**
 *  \brief Enum to set the Tx Atenuation step size
 */
typedef enum
{
    FR_TXATTEN_0P05_DB = 0,    /*!< Tx attenuation 0.05dB step size */
    FR_TXATTEN_0P1_DB = 1,     /*!< Tx attenuation 0.1dB step size */
    FR_TXATTEN_0P2_DB = 2,     /*!< Tx attenuation 0.2dB step size */
    FR_TXATTEN_0P4_DB = 3      /*!< Tx attenuation 0.4dB step size */
} fr9009TxAttenStepSize_t;

/**
 *  \brief Data structure to hold FR9009 Tx NCO test tone Configuration
 */
typedef struct
{
    uint8_t enable;                 /*!< 0 = Disable Tx NCO, 0x1 = Enable Tx NCO1 0x2 = Enable Tx NCO2 ,0x3 =on both NCO */
    int32_t Tone1Freq_kHz;        /*!< Signed frequency in kHz of the desired Tx1 tone */
    int32_t Tone2Freq_kHz;        /*!< Signed frequency in kHz of the desired Tx2 tone */
	float Tx1Power;
	float Tx2Power;
} fr9009TxNcoTestToneCfg_t;

/**
 *  \brief Enum to set the Tx Atenuation step size
 */
typedef enum
{
    FR_TXNCO_DISABLE = 0,        /*!< TX NCO Disable */
    FR_TXNCO_TX1_ENABLE = 1,     /*!< TX NCO Tx1 Enable */
    FR_TXNCO_TX2_ENABLE = 2,     /*!< TX NCO Tx2 Enable */
    FR_TXNCO_ENABLE_ALL = 3      /*!< TX NCO Tx1 & Tx2 Enable */
} fr9009TxNCOSelect_t;

/**
 *  \brief Data structure to hold FR9009 Tx NCO Configuration (tone configuration, enable/disable and tone phase configuration)
 */
typedef struct
{
    fr9009TxNCOSelect_t enableNCO; /* < 0 = Disable NCO, 1 = Enable Tx1 NCO, 2 = Enable Tx2 NCO, */
    int32_t tx1ToneFreq_kHz; /*!< Signed frequency in kHz of the desired Tx1 tone */
    int32_t tx2ToneFreq_kHz; /*!< Signed frequency in kHz of the desired Tx2 tone */
    int32_t tx1TonePhaseDeg;                         /*!< Tx1 Tone Phase degree */
    int32_t tx2TonePhaseDeg;                         /*!< Tx2 Tone Phase degree */
} fr9009TxNcoShifterCfg_t;


/**
 *  \brief Data structure to hold FR9009 Tx Attenuation Control Pin Configuration
 */
typedef struct
{
        uint8_t stepSize;                   /*!< The step that will increase or decrease the channel attenuation. This parameter sets the
                                                 change in Tx attenuation for each increment or decrement signal received in incr/decr mode.
                                                 Step of 1 changes attenuation by 0.05dB. Valid range is from 0 to 31 */
        fr9009GpioPinSel_t txAttenIncPin;   /*!< GPIO used to increment Tx attenuation
                                                 Tx1 : FR_GPIO_04 or FR_GPIO_12
                                                 Tx2 : FR_GPIO_06 or FR_GPIO_14 */
        fr9009GpioPinSel_t txAttenDecPin;    /*!< GPIO used to decrement Tx attenuation
                                                 Tx1 : FR_GPIO_05 or FR_GPIO_13
                                                 Tx2 : FR_GPIO_07 or FR_GPIO_15 */
        uint8_t enable;                     /*!< Enable (1) or disable (0) the attenuation pin control */
} fr9009TxAttenCtrlPin_t;

/**
 *  \brief Data structure to hold FR9009 Tx PA Protection configuration settings
 */
typedef struct
{
        uint8_t avgDuration;          /*!< Number of Tx samples (at JESD204 IQ sample rate) to average for the power measurement.
                                           samples = 2^(avgDuration + 5), 0 = 32 samples, max:14 = 524288 samples */
        uint8_t txAttenStep;          /*!< if PA protection threshold met, Tx Atten = TxAttenSetting + (txAttenStep * 0.4dB) */
        uint16_t tx1PowerThreshold;   /*!< tx1PowerThreashold = round(4096 * 10^(tx1PowerThreshold_dBFS / 10)) (valid 1-8191) */
        uint16_t tx2PowerThreshold;   /*!< tx2PowerThreashold = round(4096 * 10^(tx2PowerThreshold_dBFS / 10)) (valid 1-8191) */

        uint8_t peakCount;            /*!< 0=Peak Mode is disabled, if the Tx peak power threshold is exceeded more than peakCount times within
                                           one average duration, a PA error is flagged (Si Rev 0xB1: 0-30, Si Rev 0xC0: 0-31) */
        uint8_t tx1PeakThreshold;     /*!< 8-bit threshold for Tx1 peak detect. When instantaneous power exceeds this threshold, a peak is registered (valid 1-255)
                                           tx1PeakThreshold = round(128 * 10^(tx1PeakThreshold_dBFS / 10)) */
        uint8_t tx2PeakThreshold;     /*!< 8-bit threshold for Tx2 peak detect. When instantaneous power exceeds this threshold, a peak is registered (valid 1-255)
                                           tx2PeakThreashold = round(128 * 10^(tx2PeakThreshold_dBFS / 10))*/
} fr9009TxPaProtectCfg_t;

#ifdef __cplusplus
}
#endif

#endif /* FR9009_TX_TYPES_H_ */
