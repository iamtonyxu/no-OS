#ifndef __SPI_ADDR_DEF_H__
#define __SPI_ADDR_DEF_H__

#define _ADRV9009_ 1

// TXQEC Address Definitions
#ifdef _ADRV9009_
#define SPI_IDX_TX1_ABBF_C1_QEC_TRIM_MSBS		    		0x0F4A
#define SPI_IDX_TX1_ABBF_C1_QEC_TRIM_LSBS 		    		0x0F4B
#define SPI_IDX_TX1_ABBF_C2_QEC_TRIM_MSBS 		    		0x0F4C
#define SPI_IDX_TX1_ABBF_C2_QEC_TRIM_LSBS 		    		0x0F4D
#define SPI_IDX_TX2_ABBF_C1_QEC_TRIM_MSBS 		    		0x0F4E
#define SPI_IDX_TX2_ABBF_C1_QEC_TRIM_LSBS 		    		0x0F4F
#define SPI_IDX_TX2_ABBF_C2_QEC_TRIM_MSBS 		    		0x0F50
#define SPI_IDX_TX2_ABBF_C2_QEC_TRIM_LSBS 		    		0x0F51

#define SPI_IDX_TX_TXQEC_FILTER_CFG 						0x0FE0
#define SPI_IDX_TX1_TXQEC_FILTER_COEFF_Q_MSB_0 				0x0FE1
#define SPI_IDX_TX1_TXQEC_FILTER_COEFF_Q_LSB_0 				0x0FE2
#define SPI_IDX_TX1_TXQEC_FILTER_COEFF_Q_MSB_1 				0x0FE3
#define SPI_IDX_TX1_TXQEC_FILTER_COEFF_Q_LSB_1 				0x0FE4
#define SPI_IDX_TX1_TXQEC_FILTER_COEFF_Q_MSB_2 				0x0FE5
#define SPI_IDX_TX1_TXQEC_FILTER_COEFF_Q_LSB_2 				0x0FE6
#define SPI_IDX_TX1_TXQEC_FILTER_COEFF_Q_MSB_3 				0x0FE7
#define SPI_IDX_TX1_TXQEC_FILTER_COEFF_Q_LSB_3 				0x0FE8
#define SPI_IDX_TX1_TXQEC_FILTER_COEFF_Q_MSB_4 				0x0FE9
#define SPI_IDX_TX1_TXQEC_FILTER_COEFF_Q_LSB_4 				0x0FEA

#define SPI_IDX_TX1_TXQEC_FILTER_COEFF_I_MSB_0 				0x0FEB
#define SPI_IDX_TX1_TXQEC_FILTER_COEFF_I_LSB_0 				0x0FEC
#define SPI_IDX_TX1_TXQEC_FILTER_COEFF_I_MSB_1 				0x0FED
#define SPI_IDX_TX1_TXQEC_FILTER_COEFF_I_LSB_1 				0x0FEE
#define SPI_IDX_TX1_TXQEC_FILTER_COEFF_I_MSB_2 				0x0FEF
#define SPI_IDX_TX1_TXQEC_FILTER_COEFF_I_LSB_2 				0x0FF0
#define SPI_IDX_TX1_TXQEC_FILTER_COEFF_I_MSB_3 				0x0FF1
#define SPI_IDX_TX1_TXQEC_FILTER_COEFF_I_LSB_3 				0x0FF2
#define SPI_IDX_TX1_TXQEC_FILTER_COEFF_I_MSB_4 				0x0FF3
#define SPI_IDX_TX1_TXQEC_FILTER_COEFF_I_LSB_4 				0x0FF4

#define SPI_IDX_TX2_TXQEC_FILTER_COEFF_Q_MSB_0              0x1001
#define SPI_IDX_TX2_TXQEC_FILTER_COEFF_Q_LSB_0              0x1002
#define SPI_IDX_TX2_TXQEC_FILTER_COEFF_Q_MSB_1              0x1002
#define SPI_IDX_TX2_TXQEC_FILTER_COEFF_Q_LSB_1              0x1004
#define SPI_IDX_TX2_TXQEC_FILTER_COEFF_Q_MSB_2              0x1005
#define SPI_IDX_TX2_TXQEC_FILTER_COEFF_Q_LSB_2              0x1006
#define SPI_IDX_TX2_TXQEC_FILTER_COEFF_Q_MSB_3              0x1007
#define SPI_IDX_TX2_TXQEC_FILTER_COEFF_Q_LSB_3              0x1008
#define SPI_IDX_TX2_TXQEC_FILTER_COEFF_Q_MSB_4              0x1009
#define SPI_IDX_TX2_TXQEC_FILTER_COEFF_Q_LSB_4              0x100A
                
#define SPI_IDX_TX2_TXQEC_FILTER_COEFF_I_MSB_0              0x100B
#define SPI_IDX_TX2_TXQEC_FILTER_COEFF_I_LSB_0              0x100C
#define SPI_IDX_TX2_TXQEC_FILTER_COEFF_I_MSB_1              0x100D
#define SPI_IDX_TX2_TXQEC_FILTER_COEFF_I_LSB_1              0x100E
#define SPI_IDX_TX2_TXQEC_FILTER_COEFF_I_MSB_2              0x100F
#define SPI_IDX_TX2_TXQEC_FILTER_COEFF_I_LSB_2              0x1010
#define SPI_IDX_TX2_TXQEC_FILTER_COEFF_I_MSB_3              0x1011
#define SPI_IDX_TX2_TXQEC_FILTER_COEFF_I_LSB_3              0x1012
#define SPI_IDX_TX2_TXQEC_FILTER_COEFF_I_MSB_4              0x1013
#define SPI_IDX_TX2_TXQEC_FILTER_COEFF_I_LSB_4              0x1014

#define SPI_IDX_TX1_TXQEC_FILTER_OFFSET_I_MSB               0x0FF5
#define SPI_IDX_TX1_TXQEC_FILTER_OFFSET_I_LSB               0x0FF6
#define SPI_IDX_TX1_TXQEC_FILTER_OFFSET_Q_MSB               0x0FF7
#define SPI_IDX_TX1_TXQEC_FILTER_OFFSET_Q_LSB               0x0FF8
                
#define SPI_IDX_TX2_TXQEC_FILTER_OFFSET_I_MSB               0x1015
#define SPI_IDX_TX2_TXQEC_FILTER_OFFSET_I_LSB               0x1016
#define SPI_IDX_TX2_TXQEC_FILTER_OFFSET_Q_MSB               0x1017
#define SPI_IDX_TX2_TXQEC_FILTER_OFFSET_Q_LSB               0x1018

#define SPI_IDX_TX1_LO_DELAY_CORR_DAC_1                     0x0CC0
#define SPI_IDX_TX1_LO_DELAY_CORR_DAC_0                     0x0CC1
#define SPI_IDX_TX2_LO_DELAY_CORR_DAC_1                     0x0CC2
#define SPI_IDX_TX2_LO_DELAY_CORR_DAC_0                     0x0CC3

#define SPI_IDX_LO_DELAY_CORR_CONTROL_1                     (0x0C86U) 
#define SPI_IDX_LO_DELAY_CORR_CONTROL_2                     (0x0C87U)
#define BITM_SPI_LO_DELAY_CORR_CONTROL_1_TX1_LO_DELAY_CORR_PD (0x04U)
#define BITM_SPI_LO_DELAY_CORR_CONTROL_1_TX2_LO_DELAY_CORR_PD (0x08U)

#define SPI_IDX_CLOCK_CONTROL_1              (0x0141U)                /*  Clock_control_2 */
//TXQEC clock divide ratio from div4or5 clock /1, /2, /4, /8 */
#define BITM_SPI_CLOCK_CONTROL_1_TXQEC_CLOCK_ENABLE (0x20U)    /*  Reserved for future use.

/* ============================================================================================================================
        Tx_LO_Delay_Correction_Registers Register BitMasks, Positions & Enumerations 
   ============================================================================================================================ */
/* -------------------------------------------------------------------------------------------------------------------------
          SPI_TX1_LO_DELAY_CORR_DAC_1          Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_SPI_TX1_LO_DELAY_CORR_DAC_1_TX1_LO_DELAY_CORR_DAC ( 0U)            /*  Setting this bit powers down the Tx1 LO delay correction DAC. Note that this does not disable the LO path, it only disables the I/Q LO delay tuning.
 Tx1 Lo Delay Corr Dac */
#define BITM_SPI_TX1_LO_DELAY_CORR_DAC_1_TX1_LO_DELAY_CORR_DAC (0xFFU)    /*  Setting this bit powers down the Tx1 LO delay correction DAC. Note that this does not disable the LO path, it only disables the I/Q LO delay tuning.
 Tx1 Lo Delay Corr Dac */

/* -------------------------------------------------------------------------------------------------------------------------
          SPI_TX1_LO_DELAY_CORR_DAC_0          Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_SPI_TX1_LO_DELAY_CORR_DAC_0_TX1_LO_DELAY_LATCH_EN ( 7U)            /*  Toggling this bit latches the Tx1 LO delay correction DAC word.
 Tx1 Lo  Delay Latch En */
#define BITP_SPI_TX1_LO_DELAY_CORR_DAC_0_TX1_LO_DELAY_CORR_DAC_SPARE ( 2U)            /*  Setting this bit powers down the Tx1 LO delay correction DAC. Note that this does not disable the LO path, it only disables the I/Q LO delay tuning.
 spare */
#define BITP_SPI_TX1_LO_DELAY_CORR_DAC_0_TX1_LO_DELAY_CORR_DAC ( 0U)            /*  Setting this bit powers down the Tx1 LO delay correction DAC. Note that this does not disable the LO path, it only disables the I/Q LO delay tuning.
 Tx1 Lo Delay Corr Dac */
#define BITM_SPI_TX1_LO_DELAY_CORR_DAC_0_TX1_LO_DELAY_LATCH_EN (0x80U)    /*  Toggling this bit latches the Tx1 LO delay correction DAC word.
 Tx1 Lo  Delay Latch En */
#define BITM_SPI_TX1_LO_DELAY_CORR_DAC_0_TX1_LO_DELAY_CORR_DAC_SPARE (0x7CU)    /*  Setting this bit powers down the Tx1 LO delay correction DAC. Note that this does not disable the LO path, it only disables the I/Q LO delay tuning.
 spare */
#define BITM_SPI_TX1_LO_DELAY_CORR_DAC_0_TX1_LO_DELAY_CORR_DAC (0x03U)    /*  Setting this bit powers down the Tx1 LO delay correction DAC. Note that this does not disable the LO path, it only disables the I/Q LO delay tuning.
 Tx1 Lo Delay Corr Dac */

/* -------------------------------------------------------------------------------------------------------------------------
          SPI_TX2_LO_DELAY_CORR_DAC_1          Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_SPI_TX2_LO_DELAY_CORR_DAC_1_TX2_LO_DELAY_CORR_DAC ( 0U)            /*  Setting this bit powers down the Tx2 LO delay correction DAC. Note that this does not disable the LO path, it only disables the I/Q LO delay tuning.
 Tx2 Lo Delay Corr Dac */
#define BITM_SPI_TX2_LO_DELAY_CORR_DAC_1_TX2_LO_DELAY_CORR_DAC (0xFFU)    /*  Setting this bit powers down the Tx2 LO delay correction DAC. Note that this does not disable the LO path, it only disables the I/Q LO delay tuning.
 Tx2 Lo Delay Corr Dac */

/* -------------------------------------------------------------------------------------------------------------------------
          SPI_TX2_LO_DELAY_CORR_DAC_0          Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_SPI_TX2_LO_DELAY_CORR_DAC_0_TX2_LO_DELAY_LATCH_EN ( 7U)            /*  Toggling this bit latches the Tx2 LO delay correction DAC word.
 latch en */
#define BITP_SPI_TX2_LO_DELAY_CORR_DAC_0_TX2_LO_DELAY_CORR_DAC_SPARE ( 2U)            /*  Setting this bit powers down the Tx2 LO delay correction DAC. Note that this does not disable the LO path, it only disables the I/Q LO delay tuning.
 spare */
#define BITP_SPI_TX2_LO_DELAY_CORR_DAC_0_TX2_LO_DELAY_CORR_DAC ( 0U)            /*  Setting this bit powers down the Tx2 LO delay correction DAC. Note that this does not disable the LO path, it only disables the I/Q LO delay tuning.
 Tx2 Lo Delay Corr Dac */
#define BITM_SPI_TX2_LO_DELAY_CORR_DAC_0_TX2_LO_DELAY_LATCH_EN (0x80U)    /*  Toggling this bit latches the Tx2 LO delay correction DAC word.
 latch en */
#define BITM_SPI_TX2_LO_DELAY_CORR_DAC_0_TX2_LO_DELAY_CORR_DAC_SPARE (0x7CU)    /*  Setting this bit powers down the Tx2 LO delay correction DAC. Note that this does not disable the LO path, it only disables the I/Q LO delay tuning.
 spare */
#define BITM_SPI_TX2_LO_DELAY_CORR_DAC_0_TX2_LO_DELAY_CORR_DAC (0x03U)    /*  Setting this bit powers down the Tx2 LO delay correction DAC. Note that this does not disable the LO path, it only disables the I/Q LO delay tuning.
 Tx2 Lo Delay Corr Dac */

#define BITM_SPI_TXQEC_FILTER_CFG_TXQEC_FILTER_CH1_COEFF_UPDATE (0x04U)    /*  Update all coefficients at once (self clear) */
#define BITM_SPI_TXQEC_FILTER_CFG_TXQEC_FILTER_CH2_COEFF_UPDATE (0x08U)    /*  Update all coefficients at once (self clear) */
#else

#endif // _ADRV9009_


#endif // __SPI_ADDR_DEF_H__
