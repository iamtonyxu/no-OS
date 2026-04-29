#ifndef ADRV9025_TXQEC_HW_H_
#define ADRV9025_TXQEC_HW_H_

#include <stdint.h>
#include "adrv9025_txqec_reg_addr_macros.h"

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


#define TXQEC_GRP_MAX_VAL               (511)      /*!< Group delay corr - max value for signed 10 bit:    0 to 511 (  0x0 - 0x1FF) 10 bits */
#define TXQEC_GRP_MIN_VAL               (-512)     /*!< Group delay corr - min value for signed 10 bit: -512 to  -1 (0x200 - 0x3FF)         */
#define TXQEC_GRP_SIGN_CHECK            (0x200)    /*!< Group delay corr - sign check mask */
#define TXQEC_GRP_SIGN_EXTENSION        (0xFC00)   /*!< Group delay corr - sign extension mask (turn 10 bit signed into 16 bit signed) */


/*! TXQEC correction parameter types */
typedef enum {
    PARAM_PHASE = 0,
    PARAM_GAIN,
    PARAM_GROUP_DELAY,
    PARAM_GROUP_DELAY_2ND,
    PARAM_DIGITAL_PHASE,
    PARAM_NUM_TYPES
} txqec_params_t;

/*! TXQEC correction parameter masks */
#define MASK_PARAM_PHASE            (1 << PARAM_PHASE)
#define MASK_PARAM_GAIN             (1 << PARAM_GAIN)
#define MASK_PARAM_GROUP_DELAY      (1 << PARAM_GROUP_DELAY)
#define MASK_PARAM_GROUP_DELAY_2ND  (1 << PARAM_GROUP_DELAY_2ND)
#define MASK_PARAM_DIGITAL_PHASE    (1 << PARAM_DIGITAL_PHASE)

/* outputs of txqec algorithm to be written to compensation hardware */
typedef struct {
    int16_t phase;     /*!< phase or "lo delay" (delay between LO for I and Q) */
    int16_t gain;      /*!< gain of Q channel */
    int16_t gd[2];     /*!< group delay values */
} txqec_outputs_t;

void TALISE_set_phase_gain_gd(void *devHalInfo, uint32_t ch, txqec_params_t param, int16_t value);
void TALISE_get_phase_gain_gd(void *devHalInfo, uint32_t ch, txqec_outputs_t *out, uint32_t param_mask);


#endif // ADRV9025_TXQEC_HW_H_
