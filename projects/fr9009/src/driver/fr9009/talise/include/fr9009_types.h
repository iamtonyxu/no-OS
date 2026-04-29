/*!
 * \file fr9009_types.h
 * \brief Contains FR9009 API configuration and run-time type definitions
 *
 * Copyright 2022 BriRadio.
 * Released under the FR9009 API license, for more information see the "LICENSE.txt" file in this zip file.
 */

#ifndef FR9009_TYPES_H_
#define FR9009_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include "fr9009_rx_types.h"
#include "fr9009_jesd204_types.h"
#include "fr9009_tx_types.h"
#include "fr9009_radioctrl_types.h"

#ifndef DIV_U64
#define DIV_U64(a, b)	((a)/(b))
#endif

/**
 * \brief Enum of ORx PLL selections
 */
typedef enum {
	FR_LO_RF_PLL  = 0,           /*!< Selects RF PLL for LO */
	FR_LO_AUX_PLL,      /*!< Selects AUX PLL for LO */	
} fr9009LoSource_t;

typedef enum {
	MCS_TYPE_BBPLL  = 0x01,           /*!< MCS BB dig clk */
	MCS_TYPE_RFPLL	= 0x02,      /*!< MCS RF PLL for LO */	
	MCS_TYPE_ORFPLL	= 0x04,      /*!< MCS ORF PLL for LO */	
	MCS_TYPE_NCO	= 0x08,      /*!< MCS LIF NCO for offset Lo */	
} fr9009MCSType_t;
/**
 * \brief Enum of ADC Mode
 */
typedef enum {
	FR_ADC_IQ4ITLV = 0,       /*!< IQ Mode. I and Q branch use 4 sub-ADCs separately */
	FR_ADC_IQ2ITLV,           /*!< IQ Mode. I and Q branch use 2 sub-ADCs separately */
	FR_ADC_IQ,           	   /*!< IQ Mode. I and Q branch use 1 sub-ADC separately */
	FR_ADC_REAL4ITLV,     	   /*!< REAL(I) Mode. Use 4 sub-ADCs. Unsupported  */	
	FR_ADC_REAL2ITLV,         /*!< REAL(I) Mode. Use 2 sub-ADCs. Unsupported */	
	FR_ADC_REAL,              /*!< REAL(I) Mode. Use 1 sub-ADC. Unsupported */	
} fr9009AdcMode_t;

/**
 *  \brief Enum to set the desired FIR filter type for related functions
 */
typedef enum {
	FR_TX1_FIR = 1,                        /*!< Tx1 FIR filter select */
	FR_TX2_FIR = 2,                        /*!< Tx2 FIR filter select */
	FR_TX1TX2_FIR = 3,                     /*!< Tx1 + Tx2 FIR filter select */
	FR_RX1_FIR = 4,                        /*!< Rx1 FIR filter select */
	FR_RX2_FIR = 8,                        /*!< Rx2 FIR filter select */
	FR_RX1RX2_FIR = 12,                    /*!< Rx1 + Rx2 FIR filter select */
//	FR_RX1RX2_LOOPBACK_FIR = 140,          /*!< Rx1 + Rx2 loopback FIR filter select */
	FR_OBSRX1_FIR = 16,                    /*!< ORx1 FIR filter select */
	FR_OBSRX2_FIR = 32,                    /*!< ORx2 FIR filter select */
	FR_OBSRX1RX2_FIR = 48,                 /*!< ORx1 + ORx2 FIR filter select */
//	FR_OBSRX1RX2_LOOPBACK_FIR = 176,       /*!< ORx1 + ORx2 loopback FIR filter select */
//	FR_LOOPBACK_FIR = 128                  /*!< Loopback FIR filter select */
} fr9009firName_t;

/**
 *  \brief Enum to set the desired Complex FIR filter type for related functions
 */
typedef enum {
	FR_TX1_CPLX_FIR = 1,                        /*!< Tx1 FIR filter select */
	FR_TX2_CPLX_FIR = 2,                        /*!< Tx2 FIR filter select */
	FR_TX1TX2_CPLX_FIR = 3,                     /*!< Tx1 + Tx2 FIR filter select */
	FR_RX1_CPLX_FIR = 4,                        /*!< Rx1 FIR filter select */
	FR_RX2_CPLX_FIR = 8,                        /*!< Rx2 FIR filter select */
	FR_RX1RX2_CPLX_FIR = 12,                    /*!< Rx1 + Rx2 FIR filter select */
	FR_OBSRX1_CPLX_FIR = 16,                    /*!< ORx1 FIR filter select */
	FR_OBSRX2_CPLX_FIR = 32,                    /*!< ORx2 FIR filter select */
	FR_OBSRX1RX2_CPLX_FIR = 48,                 /*!< ORx1 + ORx2 FIR filter select */	
} fr9009cplxFirName_t;

/**
 *  \brief Enum to select the proper gain for the FIR
 */
typedef enum {
	FR_FIR_GAIN_NEG12_DB = -12, /*!< FR9009 FIR gain -12 */
	FR_FIR_GAIN_NEG6_DB = -6,   /*!< FR9009 FIR gain -6 */
	FR_FIR_GAIN_0_DB = 0,       /*!< FR9009 FIR gain 0 */
	FR_FIR_GAIN_POS6_DB = 6     /*!< FR9009 FIR gain 6 */
} fr9009FirGain_t;

/**
 *  \brief Data structure to hold FR9009 States
 */
typedef enum {
	FR_STATE_POWERONRESET = 0x00,
	FR_STATE_INITIALIZED  = 0x01,
	FR_STATE_STREAMLOADED = 0x02,
	FR_STATE_ARMLOADED    = 0x04,
	FR_STATE_CALS_RUN     = 0x08,
	FR_STATE_RADIOON      = 0x10
} fr9009States_t;

/**
 *  \brief Enum of possible DDC Modes
 */
typedef enum {
	FR_IFCONV_BYPASS = 0,      	/*!< No Half Band Enabled */
	FR_IFCONV_SHIFTONLY, 			/*!< FE NCO works only */
	FR_IFCONV_FILTERONLY,      	/*!< Half Band Filters only */
	FR_IFCONV_INT2,            	/*!< rx/orx Not Support. Half Band Interpolation by 2 */
	FR_IFCONV_DEC2,            	/*!< tx Not Support. Half Band Decimate by 2 */
	FR_IFCONV_BYPASS_REALIF,   	/*!< Not Support. No Half Band Enabled */
	FR_IFCONV_FILTERONLY_REALIF,  	/*!< Not Support. Half Band Filters only */
	FR_IFCONV_INT2_REALIF,     	/*!< Not Support. Half Band Interpolation by 2 */
	FR_IFCONV_DEC2_REALIF     	 	/*!< Not Support. Half Band Decimate by 2 */
} fr9009IfConvMode_t;

/**
 * \brief Enumerated list of CMOS pads drive strength options
 */
typedef enum {
	/* Values encoded as {non_gpio_drv , spi_cmos_drv_select[3:0]} */
	FR_CMOSPAD_DRV_1X  = 0x00,       /*!<  2.5pF load @ 65MHz */
	FR_CMOSPAD_DRV_2X  = 0x10,       /*!<    5pF load @ 65MHz */
	FR_CMOSPAD_DRV_3X  = 0x03,       /*!<  7.5pF load @ 65MHz */
	FR_CMOSPAD_DRV_4X  = 0x11,       /*!<   10pF load @ 65MHz */
	FR_CMOSPAD_DRV_5X  = 0x0F,       /*!< 12.5pF load @ 65MHz */
	FR_CMOSPAD_DRV_6X  = 0x13,       /*!<   15pF load @ 65MHz */
	FR_CMOSPAD_DRV_8X  = 0x17,       /*!<   20pF load @ 65MHz */
	FR_CMOSPAD_DRV_10X = 0x1F        /*!<   25pF load @ 65MHz */
} fr9009CmosPadDrvStr_t;

/**
 * \brief Enumerated list of options to disable Tx data when PLL unlocks
 */
typedef enum {
	FR_TXDIS_TX_NOT_DISABLED       = 0x0,      /*!< Tx data is not disabled when PLL unlocks */
	FR_TXDIS_TX_ZERO_DATA          = 0x1,      /*!< Tx data is zeroed immediately when PLL unlocks */
	FR_TXDIS_TX_RAMP_DOWN_TO_ZERO  = 0x2       /*!< Tx data is ramped down to zero when PLL unlocks */
} fr9009TxDataIfPllUnlock_t;

/**
 * \brief Enumerated list of RFPLL phase synchronization modes
 *
 * RFPLL Phase sync requires extra time to sync each time the RFPLL frequency
 * is changed. If RFPLL phase sync is not required, it may be desired to
 * disable the feature to allow the RFPLL to lock faster.
 *
 * Depending on the desired accuracy of the RFPLL phase sync, several options
 * are provided.
 */
typedef enum {
	FR_RFPLLMCS_NOSYNC = 0,                /*!< Disable RFPLL phase synchronization */
	FR_RFPLLMCS_INIT_AND_SYNC = 1,         /*!< Enable RFPLL phase sync init only */
	FR_RFPLLMCS_INIT_AND_1TRACK = 2,       /*!< Enable RFPLL phase sync init and track once  */	//todo
	FR_RFPLLMCS_INIT_AND_CONTTRACK = 3     /*!< Enable RFPLL phase sync init and track continuously */		//todo
} fr9009RfPllMcs_t;

/**
 *  \brief Data structure to hold FR9009 FIR filter settings
 */
typedef struct {
	uint8_t symmetric;		/*!< 1: Coefficients are symmetric; 0: non-symmetric */
	uint8_t even; 			/*!< 1: Total number of coefficients is even; 0: odd */
	fr9009FirGain_t gain_dB;/*!< Filter gain in dB */
	uint8_t numFirCoefs;    /*!< Number of coefficients contained in *coefs */
	int16_t *coefs;         /*!< A pointer to an array of filter coefficients */
} fr9009Fir_t;

/**
 *  \brief Data structure to hold FR9009 Complex FIR filter settings
 */
typedef struct {
	uint8_t numFirCoefs;    /*!< Number of coefficients contained in *coefs */
	int16_t *coefs_II;      /*!< A pointer to an array of filter coefficients */
	int16_t *coefs_QQ;      /*!< A pointer to an array of filter coefficients */
	int16_t *coefs_IQ;      /*!< A pointer to an array of filter coefficients */
	int16_t *coefs_QI;      /*!< A pointer to an array of filter coefficients */
} fr9009CplxFir_t;

/**
 * \brief Parameters for NCO Mixer
 */
typedef struct fr9009NcoMixerConfig
{
    int32_t  freq_Hz;      /*!< NCO output frequency. NCO is bypass when freq_Hz == 0 */
    uint32_t phase;        /*!< Phase offset. Range 0~0x3FFFFF; Unit: 2*pi/2^23 */
} fr9009NcoConfig_t;

/**
 *  \brief Data structure to hold FR9009 Rx NCO shifter parameters for both BandA and BandB
 *  configurations.  Settings all four element values to zero for a band will disable the NCO
 *  for that shift.
 */
typedef struct {
	fr9009NcoConfig_t bandAFe;		/*!< NCO configure of BandA in the Front-End*/
	fr9009NcoConfig_t bandABe;		/*!< NCO configure of BandA in the Back-End*/
	fr9009NcoConfig_t bandBFe;		/*!< NCO configure of BandB in the Front-End*/
	fr9009NcoConfig_t bandBBe;		/*!< NCO configure of BandB in the Back-End*/
	uint32_t bandSel; 					/*!< Bit Mask used for BandA and BandB enable*/
} fr9009IfConvCfg_t;

/**
 *  \brief Data structure to hold settings for the current Rx specific use case profile
 */
typedef struct {
	fr9009Fir_t rxFir;			        /*!< Rx FIR filter structure */
	uint32_t rxOutputRate_kHz;          /*!< Rx Output data rate in kHz */	
	fr9009IfConvMode_t rxIfConvMode[2];    /*!< Rx IF convert mode */
	fr9009IfConvCfg_t rxIfConvCfg[2]; 		/*!< Rx IF convert parameters */
	fr9009NcoConfig_t rxMixer; 			/*!< Rx Mixer */
	uint8_t rfDirectModeEn[2]; 			/*!< Rx RF direct mode set*/
	fr9009AdcMode_t  rxAdcMode; 		
	fr9009CplxFir_t rxFlatnessFir[2];		/*!< RX Flatness compensation FIR*/
} fr9009RxProfile_t;

/**
 *  \brief Data structure to hold settings for the current Tx specific use case profile
 */
typedef struct {
	fr9009Fir_t txFir;                  /*!< Tx FIR filter structure */
	uint32_t txInputRate_kHz;           /*!< Tx input data rate in kHz */
	fr9009IfConvMode_t txIfConvMode[2];    /*!< Tx IF convert mode */
	fr9009IfConvCfg_t txIfConvCfg[2]; 		/*!< Tx IF convert parameters */
	fr9009NcoConfig_t txMixer; 			/*!< Tx Mixer */	
	uint8_t rfDirectModeEn[2]; 			/*!< Tx RF direct mode set*/
	fr9009CplxFir_t txFlatnessFir[2];		/*!< TX Flatness compensation FIR*/
} fr9009TxProfile_t;

/**
 *  \brief Data structure to hold ORx gain control settings for initialization and during use
 */
typedef struct {
	uint16_t ORx1Atten_mdB;                    /*!< Initial and current ORx1 Attenuation */
	uint16_t ORx2Atten_mdB;                    /*!< Initial and current ORx2 Attenuation */
} fr9009ORxGainControl_t;

/**
 *  \brief Data structure to hold Rx gain control settings for initialization and during use
 */
typedef struct {
	fr9009GainMode_t gainMode;  /*!< Current Rx gain control mode setting */
	uint8_t rx1GainIndex;       /*!< Rx1 Gain Index, can be used in different ways for manual and AGC gain control */
	uint8_t rx2GainIndex;       /*!< Rx2 Gain Index, can be used in different ways for manual and AGC gain control */
	uint8_t rx1MaxGainIndex;    /*!< Max gain index for the currently loaded Rx1 Gain table */
	uint8_t rx1MinGainIndex;    /*!< Min gain index for the currently loaded Rx1 Gain table */
	uint8_t rx2MaxGainIndex;    /*!< Max gain index for the currently loaded Rx2 Gain table */
	uint8_t rx2MinGainIndex;    /*!< Min gain index for the currently loaded Rx2 Gain table */
	uint8_t FeAttCaliEn;    	/*!< Fe attention calibrate enable, use dig gain to fix*/
	fr9009GpioPinSel_t rx1GainDecPin;	/*rx1 gain decrease pin set*/
	fr9009GpioPinSel_t rx1GainIncPin;	/*rx1 gain increase pin set*/
	fr9009GpioPinSel_t rx2GainDecPin;	/*rx2 gain decrease pin set*/
	fr9009GpioPinSel_t rx2GainIncPin;	/*rx2 gain increase pin set*/
} fr9009RxGainControl_t;

/**
 *  \brief Data structure to hold Tx data path settings
 */
typedef struct {
	fr9009TxProfile_t			txProfile;              /*!< Tx datapath profile, 3dB corner frequencies, and digital filter enables */
	fr9009DeframerSel_t			deframerSel;          	/*!< FR9009 JESD204b deframer select (Deframer A or B, or both) */
	fr9009TxChannels_t			txChannels;            	/*!< The desired Tx channels to enable during initialization */
	fr9009TxAttenStepSize_t 	txAttenStepSize;  		/*!< Tx Attenuation step size */
	uint16_t 					tx1Atten_mdB;           /*!< Initial and current Tx1 Attenuation */
	uint16_t 					tx2Atten_mdB;           /*!< Initial and current Tx2 Attenuation */
	fr9009LoSource_t  			txLoSource[2];   		/*!< The ORx mixers can use the RF_PLL or Aux_PLL */
} fr9009TxSettings_t;

/**
 *  \brief Data structure to hold Rx data path settings
 */
typedef struct {
	fr9009RxProfile_t	  rxProfile;        /*!< Rx datapath profile, 3dB corner frequencies, and digital filter enables */
	fr9009FramerSel_t	  framerSel;        /*!< Rx JESD204b framer configuration enum */
	fr9009RxGainControl_t rxGainCtrl;   	/*!< Rx Gain control settings structure */
	fr9009RxChannels_t    rxChannels;      	/*!< The desired Rx Channels to enable during initialization */
	fr9009LoSource_t	  rxLoSource[2];   	/*!< The ORx mixers can use the RF_PLL or Aux_PLL */
} fr9009RxSettings_t;

/**
 *  \brief Data structure to hold ORx data path settings
 */
typedef struct {
	fr9009RxProfile_t		orxProfile;               /*!< ORx datapath profile, 3dB corner frequencies, and digital filter enables. */
	fr9009ORxGainControl_t	orxGainCtrl;         	  /*!< ObsRx gain control settings structure */
	fr9009FramerSel_t		framerSel;                /*!< ObsRx JESD204b framer configuration structure */
	fr9009ObsRxChannels_t	obsRxChannelsEnable;  	  /*!< The desired ObsRx Channel to enable during initialization */
	fr9009LoSource_t		obsRxLoSource[2];      	  /*!< The ORx mixers can use the RF_PLL or Aux_PLL */
} fr9009ObsRxSettings_t;
typedef struct{
	uint8_t en;
	uint8_t gpio_num;
} fr9009PLLStatusGpio_t;

/**
 *  \brief Data structure to hold digital clock settings
 */
typedef struct {
	uint32_t deviceClock_kHz;       /*!< CLKPLL and device reference clock frequency in kHz */
	uint32_t sysrefDiv;       		/*!< internal sysref clock divider( from device reference clock)*/
	uint32_t clkPllVcoFreq_kHz;     /*!< CLKPLL VCO frequency in kHz */	
	uint32_t clkPllHsDiv;     		/*!< CLKPLL high speed clock divider */	
	uint8_t rfPllUseExternalLo;     /*!< 1= Use external LO input for RF PLL, 0 = use internal LO generation for RF PLL */	//todo 孙诗岩
	fr9009RfPllMcs_t rfPllPhaseSyncMode;   /*!< Set RF PLL phase synchronization mode. Adds extra time to lock RF PLL when PLL frequency changed. See enum for options */	//todo delay^^^^
	fr9009PLLStatusGpio_t bbpllStatusGpio; /* BB PLL status select gpio*/
	fr9009PLLStatusGpio_t rfpllStatusGpio; /* rf PLL status select gpio*/
	fr9009PLLStatusGpio_t orfpllStatusGpio; /* orf PLL status select gpio*/
	uint64_t rfpllFreq_hz;				/* RF CLKPLL LO frequency in Hz*/
	uint64_t orfpllFreq_hz;			/* ORF CLKPLL LO frequency in Hz*/
} fr9009DigClocks_t;

/**
 * \brief Data structure used by the API run time state structure to hold the calculated
 *        digital clocks for the selected init profiles
 */
typedef struct {
	uint32_t deviceClock_kHz;       /*!< CLKPLL and device reference clock frequency in kHz */
	uint32_t clkPllVcoFreq_kHz;     /*!< CLKPLL VCO frequency in kHz */
	uint32_t clkPllHsDiv;     		/*!< CLKPLL high speed clock divider */
	uint32_t hsDigClkDiv2_Hz;       /*!< 1/2 of ADC/DAC Clock. Calculated in FR9009_initialize() digital clock used throughout API functions */
	uint32_t hsDigClkDiv4or5_Hz;    /*!< DP Clock. Calculated in FR9009_initialize() digital clock used throughout API functions */
	uint8_t rfPllUseExternalLo;     /*!< Stored version of the init struct setting (Ext LO input) to keep in the device data structure, 1= Use external LO input for RF PLL, 0 = use internal LO generation for RF PLL */
} fr9009Clocks_t;

/**
 * \brief Data structure to hold SPI settings for all system device types
 */
typedef struct {
	uint8_t MSBFirst;                           /*!< 1 = MSBFirst*/
	uint8_t enSpiStreaming;                     /*!< Not Recommended - most registers in FR9009 API are not consecutive */
	uint8_t autoIncAddrUp;                      /*!< For SPI Streaming, set address increment direction. 1= next addr = addr+1, 0:addr = addr-1 */
	uint8_t fourWireMode;                       /*!< 1: Use 4-wire SPI, 0: 3-wire SPI (SDIO pin is bidirectional). NOTE: ADI's FPGA platform always uses 4-wire mode */
	fr9009CmosPadDrvStr_t cmosPadDrvStrength;   /*!< Drive strength of CMOS pads when used as outputs (SDIO, SDO, GP_INTERRUPT, GPIO 1, GPIO 0) */	//TODO ???
} fr9009SpiSettings_t;

/**
 * \brief Data structure to hold FR9009 device instance initialization settings
 */
typedef struct {
	fr9009SpiSettings_t   spiSettings;     /*!< SPI settings data structure */
	fr9009RxSettings_t    rx;              /*!< Rx settings data structure */
	fr9009TxSettings_t    tx;              /*!< Tx settings data structure */
	fr9009ObsRxSettings_t obsRx;           /*!< ObsRx settings data structure */
	fr9009DigClocks_t     clocks;          /*!< Holds settings for CLKPLL and reference clock */
	fr9009JesdSettings_t  jesd204Settings; /*!< Holds the JESD204B data link settings */
	fr9009FhmMode_t fhm;
} fr9009Init_t;

/**
 * \brief Data structure to hold FR9009 error callback function
 */
typedef struct {
	uint32_t errSrc;
	const char* (*callbackFunction)(uint32_t errSrc, uint32_t errCode);
} brErrorFunction_t;

#define FR9009_ERRORFUNCTIONTABLEMAX 2

/**
 * \brief Function jump table to hold a registry of error message functions for
 *        add on features.
 */
typedef struct {
	brErrorFunction_t talErrorFunctionTable[FR9009_ERRORFUNCTIONTABLEMAX];
} brErrFunctionTable_t;

/**
 * \brief Structure to hold FHM mode frequency range
 */
typedef struct {
	uint32_t fhmMinFreq_MHz; /*!< Lower limit for FHM hop frequency in MHz */
	uint32_t fhmMaxFreq_MHz; /*!< Upper limit for FHM hop frequency in MHz */
} brFrequencyHoppingRange_t;

/**
 * \brief Data structure to hold a FR9009 device instance status information
 */
typedef struct {
	fr9009States_t	devState;          			/*!< Current device state of the part, i.e., radio on, radio off, arm loaded, etc., defined by deviceState enum */
	uint8_t initializedChannels;                /*!< Holds Rx/ORx/Tx channels that were initialized and calibrated for the current device */
	uint8_t profilesValid;            			/*!< Current device profilesValid bit field for use notification, i.e., Tx = 0x01, Rx = 0x02, Orx = 0x04 */
	uint32_t txProfileMtxIdx; 					/*!< The Index of Tx profile mapping into txProfileMatrix*/
	uint32_t rxProfileMtxIdx; 					/*!< The Index of Rx profile mapping into txProfileMatrix*/
	uint32_t orxProfileMtxIdx; 					/*!< The Index of ORx profile mapping into txProfileMatrix*/
	uint32_t errSource;               			/*!< Current source of error returned */
	uint32_t errCode;                 			/*!< Current error code returned */
	fr9009Clocks_t	clocks;            			/*!< Currently calculated FR9009 digital clocks for the selected init profiles */
	fr9009RxGainControl_t rxGainCtrl;			/*!< Current device gain mode,which can be MGC, AGCFAST, and index */
	fr9009TxAttenStepSize_t txAttenStepSize; 	/*!< Current tx Atten step size */
	uint32_t usedGpiopins;                      /*!< Each bit position 'N' indicates whether the GPIO 'N' is assigned to some function (if 1) or not (if 0) */
	uint16_t usedGpio3p3pins;                   /*!< Each bit position 'N' indicates whether the GPIO3.3 'N' is assigned to some function (if 1) or not (if 0) */
	uint8_t rxFramerNp;                         /*!< Rx Framer Np - converter sample resolution (12, 16, 24) */
	uint8_t orxFramerNp;                        /*!< ORx Framer Np - converter sample resolution (12, 16, 24) */
	uint32_t rxOutputRate_kHz;                  /*!< Rx Output data rate in kHz */
	uint32_t orxOutputRate_kHz;                  /*!< Rx Output data rate in kHz */
	uint32_t txInputRate_kHz;                   /*!< Tx input data rate in kHz */
	fr9009IfConvMode_t rxIfConvMode[2];            /*!< DDC Mode saved from initialization  */
	fr9009IfConvMode_t orxIfConvMode[2];            /*!< DDC Mode saved from initialization  */
	fr9009IfConvMode_t txIfConvMode[2];            /*!< DDC Mode saved from initialization  */
	uint8_t rxDualBandEnabled;                  /*!< DualBand Mode saved from initialization for determining dualband status */
	uint8_t rxTotalM;                           /*!< Value of M saved from initialization  */
	uint8_t rxframerSel;                    	/*!< Rx JESD204b framer configuration */
	uint8_t txdeframerSel;                    	/*!< FR9009 JESD204b deframer config for the Tx data path */
	uint8_t orxframerSel;                   	/*!< ORx JESD204b framer configuration */
	uint32_t txFirInterpolation; 				/* Tx digital Channel filter interpolation (1,2,4,....) */
	uint32_t rxFirDecimation; 					/* Rx digital Channel filter interpolation (1,2,4,....) */
	uint32_t orxFirDecimation; 					/* ORx digital Channel filter interpolation (1,2,4,....) */
	uint32_t swTest;                            /*!< Software testmode signal */
	uint32_t tracking_en;                            /*!< Software tracking enabled */
	uint8_t deviceSiRev;                        /*!< FR9009 silicon rev read during FR9009_initialize */
	uint8_t predict_cali;                        /*!< FR9009 predict calibration */
	uint8_t fhm_enable;                        /*!< fast hop enable */
	fr9009FhmType_t fhmtype;					/* !< fast hop type*/
	brErrFunctionTable_t talErrFunctionTable;  /*!< FR9009  callback function table */
	brFrequencyHoppingRange_t talFhmFreqRange; /*!< FR9009 FHM frequency range */
	fr9009FhmTriggerMode_t talFhmTriggerMode;   /*!< FR9009 frequency hop trigger mode */
	uint64_t talFhmInitHopFreq_Hz;              /*!< Initial FR9009 hop frequency */
	uint8_t talFhmMcsSync;                      /*!< Flag to indicate if MCS synchronization is enabled on frequency hopping */
} fr9009Info_t;
/**
 * \brief Data structure to hold FR9009 device instance settings
 */
typedef struct {
	void *devHalInfo;               /*!< ADI_HAL Hardware layer settings pointer specific to this FR9009 instance */
	fr9009Info_t devStateInfo;      /*!< FR9009 infomration container */
} fr9009Device_t;

#ifdef __cplusplus
}
#endif

#endif
