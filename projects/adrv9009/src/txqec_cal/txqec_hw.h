#ifndef __TXQEC_HW_H__
#define __TXQEC_HW_H__

#include <stdint.h>
#include <stdbool.h>
#include "spi_addr_def.h"
#include "spi.h"

#define TXQEC_HW_TAPS            (3)  /*!< number of taps available in hardware */

#define INITIAL_PHASE_VALUE    (512)  /*!< Initial phase correction value; 512 is center */
#define INITIAL_GAIN_VALUE  (0x4000)  /*!< Initial gain correction value; 0100_0000_0000_0000 represent 1.0 in 2.14 format */
#define INITIAL_GD_VALUE         (0)  /*!< Initial group delay correction value (GD1 and GD2) */
#define INITIAL_DIG_PHASE_VALUE  (0)  /*!< Initial digital phase correction value */

#define TXQEC_GRP_MAX_VAL        (511)      /*!< Group delay corr - max value for signed 10 bit:    0 to 511 (  0x0 - 0x1FF) 10 bits */
#define TXQEC_GRP_MIN_VAL        (-512)     /*!< Group delay corr - min value for signed 10 bit: -512 to  -1 (0x200 - 0x3FF)         */
#define TXQEC_GRP_SIGN_CHECK     (0x200)    /*!< Group delay corr - sign check mask */
#define TXQEC_GRP_SIGN_EXTENSION (0xFC00)   /*!< Group delay corr - sign extension mask (turn 10 bit signed into 16 bit signed) */

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


/*! TXQEC correlator status */
typedef enum {
    TXQEC_CORR_STATUS_DONE,
    TXQEC_CORR_STATUS_ABORT,
    TXQEC_CORR_STATUS_ERROR
} txqec_corr__status_t;

/*! TXQEC correlator tap values from the hardware block */
typedef struct {
    int64_t r_uu[3]; /*!< Corr (Tx_I * Tx_I) = Auto (Tx_I) */
    int64_t r_uv[3]; /*!< Corr (Tx_I * Tx_Q)               */
    int64_t r_vu[3]; /*!< Corr (Tx_Q * Tx_I)               */
    int64_t r_vv[3]; /*!< Corr (Tx_Q * Tx_Q) = Auto (Tx_Q) */
    int64_t r_zu[3]; /*!< Corr (Rx_I * Tx_I)               */
    int64_t r_yu[3]; /*!< Corr (Rx_Q * Tx_I)               */
    int64_t r_zv[3]; /*!< Corr (Rx_I * Tx_Q)               */
    int64_t r_yv[3]; /*!< Corr (Rx_Q * Tx_Q)               */
    int64_t dc_u;    /*!< DC (Tx_I)         */
    int64_t dc_v;    /*!< DC (Tx_Q)         */
    int64_t dc_z;    /*!< DC (Rx_I)         */
    int64_t dc_y;    /*!< DC (Rx_Q)         */
} txqec_corr__outputs_t;

/* outputs of txqec algorithm to be written to compensation hardware */
typedef struct {
    int16_t phase;     /*!< phase or "lo delay" (delay between LO for I and Q) */
    int16_t gain;      /*!< gain of Q channel */
    int16_t gd[2];     /*!< group delay values */
} txqec_outputs_t;

typedef struct {
    uint8_t fifo_del;
    uint8_t interp_index;
} pathdelay_out_t;

void txqec_init(void *devHalInfo, uint32_t chan);
void txqec_hw_init_all_correction_values(void *devHalInfo, uint32_t chan);
void txqec_set_phase_gain_gd(void *devHalInfo, uint32_t chan, txqec_params_t param, int16_t value);
void txqec_get_phase_gain_gd(void *devHalInfo, uint32_t chan, txqec_outputs_t *out, uint32_t param_mask);


#endif // __TXQEC_HW_H__
