/**
 * \file fr9009_radioctrl_types.h
 * \brief Contains FR9009 API Radio Control data types
 *
 * Copyright 2022 BriRadio.
 * Released under the FR9009 API license, for more information see the "LICENSE.txt" file in this zip file.
 */

#ifndef FR9009_RADIOCTRL_TYPES_H_
#define FR9009_RADIOCTRL_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct{
	uint32_t HW_REVISION;			   // 0x00
	uint32_t test_mux;				   // 0x04
	uint32_t gpioSrcCtrl_reg;		   // 0x08
	uint32_t usr_set_gpio_out;		   // 0x0C
	uint32_t usr_set_gpio_oe;		   // 0x10
	uint32_t usr_get_gpio_in;		   // 0x14
	uint32_t arm_gpio_out;			   // 0x18
	uint32_t arm_gpio_oe;			   // 0x1C
	uint32_t arm_gpio_in;			   // 0x20
	uint32_t reserve;				   // 0x24
	uint32_t prg_event_gpio_oe;		   // 0x28
	uint32_t prg_event_gpio_in;		   // 0x2C
	uint32_t gpio_irq_mask;			   // 0x30
	uint32_t gpio_irq;				   // 0x34
	uint32_t gpio_event_edge_type;	   // 0x38
	uint32_t gpio_event_polar;		   // 0x3C
	uint32_t gpio3v3SrcCtrl_reg;	   // 0x40
	uint32_t arm_gpio_3v3_out;		   // 0x44
	uint32_t arm_gpio_3v3_oe;		   // 0x48
	uint32_t arm_gpio_3v3_in;		   // 0x4C
	uint32_t gpio_3v3_irq_mask;		   // 0x50
	uint32_t gpio_3v3_irq;			   // 0x54
	uint32_t gpio_3v3_event_edge_type; // 0x58
	uint32_t gpio_3v3_event_polar;	   // 0x5C
	uint32_t monitor_mask;			   // 0x60
	uint32_t monitor_index;			   // 0x64
	uint32_t spi_sdio_bdir_off;		   // 0x68
	uint32_t orx_tx_sel_pin_sel;	   // 0x6C
	uint32_t rx_mgc_sel;			   // 0x70
	uint32_t orx_mgc_sel;			   // 0x74
	uint32_t tx_attn_sel;			   // 0x78
	uint32_t tx_attn_state_pin_sel;	   // 0x7C
	uint32_t pwm1_period_adj;		   // 0x80
	uint32_t pwm1_enable_adj;		   // 0x84
	uint32_t pwm2_period_adj;		   // 0x88
	uint32_t pwm2_enable_adj;		   // 0x8C
	uint32_t rx_mgc_read_back;		   // 0x90
	uint32_t orx_enable_gpio_sel;	   // 0x94
} FR9009_GPIO_TypeDef;
/**
 *  \brief Enum to select desired low voltage GPIO pin used by the API
 */
typedef enum {
	FR_GPIO_00 = 0,
	FR_GPIO_01,
	FR_GPIO_02,
	FR_GPIO_03,
	FR_GPIO_04,
	FR_GPIO_05,
	FR_GPIO_06,
	FR_GPIO_07,
	FR_GPIO_08,
	FR_GPIO_09,
	FR_GPIO_10,
	FR_GPIO_11,
	FR_GPIO_12,
	FR_GPIO_13,
	FR_GPIO_14,
	FR_GPIO_15,
	FR_GPIO_16,
	FR_GPIO_17,
	FR_GPIO_18,
	FR_GPIO_INVALID
} fr9009GpioPinSel_t;

/**
 *  \brief Enum to select desired External LO output frequency = RFPLL VCO / 2 / ExtLoDiv
 *  Where ExtLoDiv is a power of 2 from (1, 2, 4, 8, 16, 32, 64, 128)
 */
typedef enum {
	FR_EXTLO_RFPLLVCO_DIV2 =  0, 
	FR_EXTLO_RFPLLVCO_DIV4 =  1, 
	FR_EXTLO_RFPLLVCO_DIV8 =  2, 
	FR_EXTLO_RFPLLVCO_DIV16 = 3, 
	FR_EXTLO_RFPLLVCO_DIV32 = 4, 
	FR_EXTLO_RFPLLVCO_DIV64 = 5, 
	FR_EXTLO_RFPLLVCO_DIV128 = 6, 
	FR_EXTLO_RFPLLVCO_DIV256 = 7, 
} fr9009ExtLoDiv_t;
/**
 * \brief Data structure to hold ARM GPIO pin assignments, polarity, and pin enable for the FR9009 ARM
 */
typedef struct {
	fr9009GpioPinSel_t gpioPinSel; /*!< Select desired GPIO pin to input into FR9009 (valid 0-15) */
	uint8_t polarity;           /*!< Signal polarity (0 = Normal polarity, 1=FR9009 will invert the signal before using) */
	uint8_t enable;             /*!< 1 = Enable FR9009 ARM use of GPIO signal, 0 = FR9009 ARM uses ARM command to set this signal value */
} fr9009ArmGpioPinSettings_t;

/**
 * \brief set gpio pin select, added on 2023.03.15 for 3109ES2, gpio_reg format 5 bit 
 * gpio reg format:
 * --------------------------------------------------------------------------------------
 * |	PIN_SEL_VAL (size: 4 bit, range: 0~15)	|	ENABLE (size: 1 bit, range: 0~1)	|
 * --------------------------------------------------------------------------------------
 */
typedef struct {
	fr9009GpioPinSel_t gpioPinSel; /*!< Select desired GPIO pin to input into FR9009 (valid 0-15) */
	uint8_t enable;             /*!< 1 = Enable FR9009 ARM use of GPIO signal, 0 = FR9009 ARM uses ARM command to set this signal value */
} fr9009GpioShortPinSel_t;


/**
 * \brief Data structure to hold ARM GPIO pin assignments for each ARM input/output pin.
 */
typedef struct {
	/* FR9009 ARM input GPIO pins -- Only valid if orxPinMode = 1 */
	fr9009ArmGpioPinSettings_t orx1TxSel0Pin;      /*!< Select desired GPIO pin to input into FR9009(valid 0-15), polarity, enable */
	fr9009ArmGpioPinSettings_t orx1TxSel1Pin;      /*!< Select desired GPIO pin to input into FR9009(valid 0-15), polarity, enable */
	fr9009ArmGpioPinSettings_t orx2TxSel0Pin;      /*!< Select desired GPIO pin to input into FR9009(valid 0-15), polarity, enable */
	fr9009ArmGpioPinSettings_t orx2TxSel1Pin;      /*!< Select desired GPIO pin to input into FR9009(valid 0-15), polarity, enable */
	fr9009ArmGpioPinSettings_t enTxTrackingCals;   /*!< Select desired GPIO pin to input into FR9009(valid 0-15), polarity, enable */
} fr9009ArmGpioConfig_t;

/**
 * \brief Data structure to hold FR9009 GPIO pin assignments for each input/output pin.
 * updated on 2023.03.15 for FR9009ES2
 */
typedef struct {
	/* FR9009 ARM input GPIO pins -- Only valid if orxPinMode = 1 */
	fr9009GpioShortPinSel_t p0;      /*!< Select desired GPIO pin to input into FR9009(valid 0-15), polarity, enable */
	fr9009GpioShortPinSel_t p1;      /*!< Select desired GPIO pin to input into FR9009(valid 0-15), polarity, enable */
	fr9009GpioShortPinSel_t p2;      /*!< Select desired GPIO pin to input into FR9009(valid 0-15), polarity, enable */
	fr9009GpioShortPinSel_t p3;      /*!< Select desired GPIO pin to input into FR9009(valid 0-15), polarity, enable */
} fr9009GpioShortConfig_t;

/**
 * \brief Data structure to hold FR9009 GPIO pin assignments for each input/output pin.
 * updated on 2023.03.15 for FR9009ES2
 */
typedef struct {
	/* FR9009 ARM input GPIO pins -- Only valid if orxPinMode = 1 */
	fr9009GpioShortPinSel_t orx1_enable_gpio_sel;      /*!< Select desired GPIO pin to input into FR9009(valid 0-15), polarity, enable */
	uint8_t orx1_enble_sel;      /*!< 0/1 */
	fr9009GpioShortPinSel_t orx2_enable_gpio_sel;      /*!< Select desired GPIO pin to input into FR9009(valid 0-15), polarity, enable */
	uint8_t orx2_enble_sel;      /*!< 0/1 */
} fr9009GpioConfigOrxGpio_t;


/**
 * \brief Enumerated list of Radio Control Config2 ORx1/ORx2 GPIO pin pair settings
 */
typedef enum {
	FR_ORX1ORX2_PAIR_01_SEL = 0x00,   /*!< Radio Control Config 2 ORx1/ORx2 GPIO '0,1' pin pair select */
	FR_ORX1ORX2_PAIR_45_SEL,          /*!< Radio Control Config 2 ORx1/ORx2 GPIO '4,5' pin pair select */
	FR_ORX1ORX2_PAIR_89_SEL,          /*!< Radio Control Config 2 ORx1/ORx2 GPIO '8,9' pin pair select */
	FR_ORX1ORX2_PAIR_NONE_SEL         /*!< Radio Control Config 2 ORx1/ORx2 GPIO 'none' pin pair select */

} fr9009RadioCtlCfg2_t;

/**
 * \brief Enumerated list of Radio Control Config1 register bits
 */
typedef enum {
	FR_TXRX_PIN_MODE = 0x01,       /*!< Radio Control Config 1 bit '0' mask */
	FR_ORX_PIN_MODE = 0x02,        /*!< Radio Control Config 1 bit '1' mask */
	FR_ORX_USES_RX_PINS = 0x04,    /*!< Radio Control Config 1 bit '2' mask */
	FR_ORX_SEL = 0x10,             /*!< Radio Control Config 1 bit '4' mask */
	FR_ORX_SINGLE_CHANNEL = 0x20,  /*!< Radio Control Config 1 bit '5' mask */
	FR_ORX_ENAB_SEL_PIN = 0x40     /*!< Radio Control Config 1 bit '6' mask */
} fr9009RadioCtlCfg1_t;

/**
 *  \brief Enum of possible Rx and ORx stream processor enables to be used with
 *         the FR9009_setRxTxEnable() function
 */
typedef enum {
	FR_RXOFF_EN    = 0x00,  /*!< All Rx/ORx channels off */
	FR_RX1_EN      = 0x01,  /*!< Rx1 channel enabled */
	FR_RX2_EN      = 0x02,  /*!< Rx2 channel enabled */
	FR_RX1RX2_EN   = 0x03,  /*!< Rx1 + Rx2 channels enabled */
	FR_ORX1_EN     = 0x04,  /*!< ORx1 channel enabled */
	FR_ORX2_EN     = 0x08,  /*!< ORx2 channel enabled */
	FR_ORX1ORX2_EN = 0x0C,	 /*!< ORx1 and ORx2 channels enabled - only allowed if ADC stitching is not enabled */
	FR_RX1_ORX2_EN = 0x09,  /*!< RX1 and ORx2 channel enabled */
	FR_RX2_ORX1_EN = 0x06,  /*!< RX2 and ORx1 channel enabled */

} fr9009RxORxChannels_t;

/**
 * \brief Enumerated list of Tx to ORx mapping selections
 */
typedef enum {
	FR_MAP_NONE = 0,           /*!< No Tx to ORx mapping select */
	FR_MAP_TX1_ORX = 0x01,     /*!< Tx1 to ORx mapping select */
	FR_MAP_TX2_ORX = 0x02      /*!< Tx2 to ORx mapping select */

} fr9009TxToOrxMapping_t;
	
/**
 *  \brief Enum of PLL selections
 */
typedef enum {
	FR_CLK_PLL = 0,                /*!< Selects CLK PLL for Rx and Tx */
	FR_RF_PLL,                     /*!< Selects RF PLL for Rx and Tx */
	FR_ORF_PLL                     /*!< Selects AUX PLL for Rx and tx*/

} fr9009RfPllName_t;
	
/**
 * \brief Structure to setup/enable ORx LO Selection feature
 */
typedef struct {
	uint8_t disableAuxPllRelocking; /*!< Disables the ARM from automatically relocking the Aux PLL.
                                         Set to 1 when using AuxLO as ORx LO source, 0 = default when RFPLL used as ORx LO source */
	fr9009GpioPinSel_t gpioSelect;  /*!< FR_GPIO_INVALID = disable pin mode, GPIO0-15 valid */
} fr9009OrxLoCfg_t;

/**
 *  \brief Enum of FHM TYPE Selections
 */
typedef enum {
	FR_FHM_AUTO_RF_MODE 			= 0x0001,  /*!< Selects auto RF to frequency hopping*/
	FR_FHM_AUTO_ORF_MODE 			= 0x0002,  /*!< Selects auto ORF to frequency hopping*/
	FR_FHM_AUTO_RF_ORF_MODE 		= 0x0003,  /*!< Selects auto ORF RF to frequency hopping*/
	FR_FHM_AUTO_NCO_MODE			= 0x0004,  /*!< Selects auto NCO to frequency hopping*/
	FR_FHM_MANUAL_RF_MODE 			= 0x0010,  /*!< Selects manual RF to frequency hopping*/
	FR_FHM_MANUAL_ORF_MODE 			= 0x0020,  /*!< Selects manual AUX RF to frequency hopping*/
	FR_FHM_MANUAL_RF_ORF_MODE 		= 0x0030,  /*!< Selects manual AUXRF / RF pingpang to frequency hopping*/
	FHM_MANUAL_SINGLE_RF_ORF_MODE 	= 0x0031,  /*!< Selects manual single AUX/ RF to frequency hopping*/
	FR_FHM_MANUAL_NCO_MODE			= 0x0040,  /*!< Selects manual NCO to frequency hopping*/
} fr9009FhmType_t;
/**
 *  \brief Enum of FHM Trigger Mode Selections
 */
typedef enum {
	FR_FHM_INVALID_TRIGGER_MODE = 0,
	FR_FHM_GPIO_MODE,       /*!< Selects FHM trigger mode as GPIO. A low to high pulse triggers frequency hop */
	FR_FHM_NON_GPIO_MODE,       /*!< Selects FHM trigger mode as non-GPIO. An ARM command triggers frequency hop */
} fr9009FhmTriggerMode_t;

/**
 * \brief Structure to setup fr9009 LO frequency hopping config
 */
typedef struct {
	fr9009GpioPinSel_t fhmGpioPin; /*!< Maps the FR9009 ARM GPIO pin(FR_GPIO_0 - FR_GPIO_18) for frequency hopping. 
										A low to high pulse on this pin triggers freq hopping Setting 
										fhmGpioPin = FR_GPIO_INVALID will unassign ARM GPIO pin mapped to 
										Rf Pll Frequency hopping if auto fhm mode, aux rf gpio is invalid*/
	fr9009FhmTriggerMode_t fhmTriggerMode;  /*!< FR_FHM_GPIO_MODE - Frequency Hop triggered via GPIO low to high pulse
                                                 FR_FHM_NON_GPIO_MODE - Frequency Hop triggered via ARM command*/
	uint32_t fhmFreqNums;					/*!< number of frequency to hop*/
	int32_t *fhmFrequencyList_KHz;       /*!< Sets frequency hopping  frequency */
	uint32_t caliFreqNums;					/*!< number of frequency to calibration, it is invalid if nco fast hop mode*/
	uint32_t *calibrationFrequencyList_KHz;       /*!< Sets frequency to calibration, it is invalid if nco fast hop mode */
} fr9009FhmLoConfig_t;

/**
 * \brief Structure to setup fr9009 NCO frequency hopping config
 */
typedef struct {
	// uint8_t channel_type;					/*!< nco control mode, 0x1:RX, 0x2: orx, 0x4: tx*/
	uint8_t ch;       					/*!< channel of nco fast hop, 0x1:CHANNEL_1 0x2:CHANNEL_2 0x3:CHANNEL_1_2 */
	fr9009GpioPinSel_t fhmGpioPin; /*!< Maps the FR9009 ARM GPIO pin(FR_GPIO_0 - FR_GPIO_18) for frequency hopping. 
										A low to high pulse on this pin triggers freq hopping Setting 
										fhmGpioPin = FR_GPIO_INVALID will unassign ARM GPIO pin mapped to 
										Rf Pll Frequency hopping if auto fhm mode, aux rf gpio is invalid*/
	fr9009FhmTriggerMode_t fhmTriggerMode;  /*!< FR_FHM_GPIO_MODE - Frequency Hop triggered via GPIO low to high pulse
                                                 FR_FHM_NON_GPIO_MODE - Frequency Hop triggered via ARM command*/
	uint32_t fhmFreqNums;					/*!< number of frequency to hop*/
	int32_t *fhmFrequencyList_KHz;       /*!< Sets frequency hopping  frequency */
} fr9009FhmNcoConfig_t;

/**
 * \brief Structure to setup fr9009 frequency hopping mode settings
 */
typedef struct {
	uint8_t fhmEnable;                      /*!< 0 - Disables Frequency Hopping, 1 - Enables Frequency Hopping */
	uint32_t fhmSwitchFreq_Hz;              /*!< frequency switch frequency in AUTO Mode, this param is invalid if use manual mode */
	uint8_t predict_cali;                        /*!< FR9009 predict calibration enable*/
	fr9009FhmLoConfig_t fhmRf;				/*!< RF fhm config*/
	fr9009FhmLoConfig_t fhmORf;				/*!< ORF fhm config*/
	fr9009FhmNcoConfig_t rx_nco;				/*!< rx nco fhm config*/
	fr9009FhmNcoConfig_t tx_nco;				/*!< tx nco fhm config*/
	fr9009FhmType_t fhmType;				/*!< set frequency hopping type*/
	uint8_t fhm_rx_ch;				/*!< set hopping rx ch 0xff表示程序根据开启通道进行配置, 0x1 : CHANNEL_1 0x2 CHANNEL_2 0x3 CHANNEL_1_2*/
	uint8_t fhm_orx_ch;				/*!< set hopping orx ch 0xff表示程序根据开启通道进行配置, 0x1 : CHANNEL_1 0x2 CHANNEL_2 0x3 CHANNEL_1_2*/
	uint8_t fhm_tx_ch;				/*!< set hopping tx ch 0xff表示程序根据开启通道进行配置, 0x1 : CHANNEL_1 0x2 CHANNEL_2 0x3 CHANNEL_1_2*/
} fr9009FhmMode_t;

/**
 * \brief Structure to read fr9009 frequency hopping mode status
 */
typedef struct {
	uint16_t currentFhmCmdErrorStatus;     /*!< Current FHM Enter Command Error Status. This is same as the mailbox command error status for FHM */
	uint16_t currentFhmHopErrorStatus;     /*!< Currently active FHM errors during frequency hopping */
	uint32_t numFhmHops;                   /*!< Total no. of Hops since entering FHM */
	uint32_t numFhmNoErrorEvents;          /*!< Total no. of NO FHM error events */
	uint64_t lastFhmNoErrorFreq_Hz;        /*!< Last frequency for which NO Error was encountered */
	uint32_t numFhmHopsOutsideScanRange;   /*!< Total no. of Hops outside FHM scan range */
	uint64_t lastFreqOutsideScanRange_Hz;  /*!< Last frequency which was outside FHM scan range*/
	uint32_t numInvalidFhmHopFrequencies;  /*!< Invalid Hop Freq */
	uint64_t lastInvalidHopFreq_Hz;        /*!< Last Invalid Hop Freq */
	uint32_t compPllError;                 /*!< PLL LO Computation Error */
	uint64_t compPllErrorFreq_Hz;          /*!< PLL LO Computation Error frequency */
	uint32_t rfPllLockFailed;              /*!< RF PLL Lock failed */
	uint64_t rfPllLockFailedFreq_Hz;       /*!< RF PLL Lock failed frequency*/
} fr9009FhmStatus_t;

#ifdef __cplusplus
}
#endif

#endif /* FR9009_RADIOCTRL_TYPES_H_ */
