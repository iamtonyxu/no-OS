/*! **************************************************************************** 
 *
 * @file:    txqec.h
 *
 * @brief:   TXQEC algorithm only local type definitions
 *
 * @details: 
 *
 * @version: $Revision:
 *
 * @date:    $Date:
 *
 *******************************************************************************
 */
#ifndef __TXQEC_HEADER__
#define __TXQEC_HEADER__

#include <stdint.h>
#include "txqec_hw.h"

#define TXQEC_CURSOR                    (1)       /*!< Offset from pathdelay fractional value   */
#define TXQEC_ATTEN_POINTS              (9)       /*!< Number of attenuation points in init cal */

#define TXQEC_PRECISION_EXPONENT        (1.0e12f) /*!< exponent multipler used in Init and Tracking cals */

#define TXQEC_PRECISION_REF_IQ_RATE     (491520000UL) /*!< Reference ORX I/Q for the default precision values */

#define TXQEC_GAIN_SCALE                (15400.0f) /*!< Gain  scale (theoretical = 2^14 = 16384) */
#define TXQEC_PHASE_SCALE               (20372.0f) /*!< Phase scale (theoretical) */

#define TXQEC_GAIN_MAX_VAL              ((1<<16) - 1) /*!< Gain corr max value: 0xFFFF = 65535 */
#define TXQEC_GAIN_MIN_VAL              (0)           /*!< Gain corr min value */

#define TXQEC_GRP_MAX_VAL               (511)      /*!< Group delay corr - max value for signed 10 bit:    0 to 511 (  0x0 - 0x1FF) 10 bits */
#define TXQEC_GRP_MIN_VAL               (-512)     /*!< Group delay corr - min value for signed 10 bit: -512 to  -1 (0x200 - 0x3FF)         */
#define TXQEC_GRP_SIGN_CHECK            (0x200)    /*!< Group delay corr - sign check mask */
#define TXQEC_GRP_SIGN_EXTENSION        (0xFC00)   /*!< Group delay corr - sign extension mask (turn 10 bit signed into 16 bit signed) */

#define TXQEC_PHASE_MAX_VAL             (1023)     /*!< Phase correction - max value for 10 bit number : 0x3FF = 1023 */
#define TXQEC_PHASE_MIN_VAL             (0)        /*!< Phase correction - min value: 0  */

#define BITP_UPPER_NIB                  (4)
#define BITM_LOWER_NIB                  (0xF)

/*----------------------------------------------------------------------------
 * TX-QEC general data structures
 *----------------------------------------------------------------------------*/

/* floating point precision (double or float) */
#if (TXQEC_DOUBLE_MODE == 1)
typedef double txqec_float_t;
#else
typedef float txqec_float_t;
#endif

/*! Init Status structure */
typedef struct {
    /* standard header */
    uint32_t error_code;         /*!< error code from cal */
    uint32_t percent_complete;   /*!< percent completed. Range 0 to 100 */
    uint32_t performance_metric; /*!< total number of HD2 power measurements made for GD estimations */
    uint32_t iter_count;         /*!< increments each time the cal runs                  */
    uint32_t update_count;       /*!< increments each time the cal successfully finishes */

    /* TXQEC Init specific status - add more as needed */
    uint16_t batch_cnt;          /*!< number of batches needed to reach precision (total accumulation over attens) */
    uint32_t gd_image_pwr;       /*!< GD image power metric */

} txqec_init_status_t;

/*! Tracking Status structure */
typedef struct {
    /* standard header */
    uint32_t error_code;         /*!< (current_cal << 16) | error code from cal */
    uint32_t percent_complete;   /*!< percent of required data collected for the current cal. Range 0 to 100 */
    uint32_t performance_metric; /*!< metric of how well the tracking cal is performing */
    uint32_t iter_count;         /*!< running counter that increments each time the cal runs to completion */
    uint32_t update_count;       /*!< running counter that increments each time the cal updates the correction/actuator hardware */

    /* TXQEC Tracking specific status - add more as needed */
    uint16_t current_cal;        /*!< Current sub-cal: 0 - 3 (see TXQEC_TRACK_CURRENT_CAL_*) */
    uint16_t cur_batch_cnt;      /*!< current batch counter */
    uint16_t last_batch_cnt;     /*!< last batch counter: number of batches needed to reach precision */

} txqec_track_status_t;

typedef struct {
    txqec_init_status_t ch[2];
} txqec_top_init_status_t;

typedef struct {
    txqec_track_status_t ch[2];
} txqec_top_track_status_t;

typedef struct {
    txqec_float_t uu;  /*!< Corr (Tx_I * Tx_I) = Auto (Tx_I) */
    txqec_float_t vv;  /*!< Corr (Tx_Q * Tx_Q) = Auto (Tx_Q) */
    txqec_float_t uv;  /*!< Corr (Tx_I * Tx_Q)               */
    txqec_float_t zu;  /*!< Corr (Rx_I * Tx_I)               */
    txqec_float_t yu;  /*!< Corr (Rx_Q * Tx_I)               */
    txqec_float_t zv;  /*!< Corr (Rx_I * Tx_Q)               */
    txqec_float_t yv;  /*!< Corr (Rx_Q * Tx_Q)               */
} corr_data_t;

typedef struct {
    txqec_float_t u;   /*!< DC (Tx_I)   */
    txqec_float_t v;   /*!< DC (Tx_Q)   */
    txqec_float_t z;   /*!< DC (Rx_I)   */
    txqec_float_t y;   /*!< DC (Rx_Q)   */
} dc_data_t;

typedef struct {
    corr_data_t    c_sum;    /* Sum of correlations */

    dc_data_t      dc_sum;   /* Sum of DC values    */

    float          num_samples;  /*!< total number of samples correlated (an accumulated value) */
    int32_t        atten;        /*!< attenuation value associated with these statistics */

    corr_data_t    c_no_dc;   /* correlation with DC removed: c_sum - "dc_sum".  This is NOT accumulated. */

} txqec_stats_t;

typedef struct {
    txqec_float_t a; /*!< 2 x 2 channel estimate matrix */
    txqec_float_t b;
    txqec_float_t c;
    txqec_float_t d;
    txqec_float_t precision;  /*!< how accurately we know this matrix. (1/variance) */
} txqec_channel_matrix_t;

/* correction adjustments to txqec knobs */
typedef struct {
    float   phase;     /*!< phase or "lo delay" (delay between LO for I and Q) */
    float   gain;      /*!< gain of Q channel                                  */
} txqec_phs_gain_t;

#define MAX_NUM_TX_PFIR_COEFF   (128) /* maximum possible number of coefficients - TX */

typedef enum {
    HAL_PFIR_TX_1,
    HAL_PFIR_TX_2,
    HAL_PFIR_RX_A,  
    HAL_PFIR_RX_B,  
    HAL_PFIR_RX_C,
    HAL_NUM_PFIRS,
} HAL_pfirSel_t;

/*----------------------------------------------------------------------------
 * TX-QEC Table structures
 *----------------------------------------------------------------------------*/
#define TXQEC_INIT_TBL_LEN (TXQEC_ATTEN_POINTS - 2)  /*!< Length of TxQEC Init table */

/*!< Table of actual measured/estimated correction values for TX attenuations
 *   in txqec_init_config_t - atten_hpsel[TXQEC_ATTEN_POINTS]
 */
typedef struct
{
    int16_t  phase_adjust[TXQEC_INIT_TBL_LEN]; /*!< phase acorrections */
    int16_t  gain_adjust[TXQEC_INIT_TBL_LEN];  /*!< gain  corrections */
    int16_t  gd_adjust[2];                     /*!< group delay corrections - 1st and 2nd order */
} txqec_init_table_t;

#define TXQEC_TBL_LEN (64)  /*!< Length of TxQEC table */

/*!< Table of corrections for all TX attenutor values (interpolated values) */
typedef struct {
    int16_t  phase_adjust[TXQEC_TBL_LEN]; /*!< phase adjustment table */
    int16_t  gain_adjust[TXQEC_TBL_LEN];  /*!< gain  adjustment table */
    int16_t  gd_adjust[2]; /*!< group delay - 1st and 2nd order - applies to all attenuations  */
    uint8_t  flag[2];      /*!< TXQEC sets at beginning of a run, cleared whenever updates happen during interrupts */
} txqec_table_t;

/*----------------------------------------------------------------------------
 * TX-QEC INIT data structures
 *----------------------------------------------------------------------------*/
typedef struct {
    uint16_t enable_gd;           /*!< enable group delay estimation */
    uint8_t  atten_point_start;   /*!< not currently used */
    uint8_t  atten_point_end;
    uint8_t  atten_hpsel[TXQEC_ATTEN_POINTS];
    uint8_t  gd_qbin_percent;    /*!< How close is the group delay estimation tone to the TX BW */

    uint16_t precision;          /*!< channel estimation threshold for normal LO (<= 3 GHz) (* TXQEC_PRECISION_EXPONENT) */
    uint8_t  iter_cnt;           /*!< 2^iter_cnt samples are taken with each TX-QEC correlator run */
    uint16_t tap_dis;
    uint16_t precision_hilo;     /*!< channel estimation threshold for high LO (> 3 GHz) (* TXQEC_PRECISION_EXPONENT) */
    uint8_t  dig_atten[TXQEC_ATTEN_POINTS]; /*!< digital backoff to apply */
    uint8_t  init_tbl_indx[TXQEC_ATTEN_POINTS]; /* index into txqec_init_tables[] */

} txqec_init_config_t;

typedef struct {
    void *devHalInfo;                   /*!< pointer to device HAL info */
    uint32_t channel;                   /*!< channel (0 or 1)                */
    float lo_ghz;                       /*!< local oscillator frequency (GHz) */
    float    precision;                 /*!< working channel estimation threshold (* TXQEC_PRECISION_EXPONENT) */
    txqec_stats_t *stats;               /*!< statistics from correlator read */
    const pathdelay_out_t *pathdelay;   /*!< ptr to pathdelay output data, one for each channel */
    const txqec_init_config_t *config;  /*!< ptr to init config data          */
    txqec_init_table_t *init_table;     /*!< ptr to txqec_init_table[channel] */

    int16_t txfir_coef[MAX_NUM_TX_PFIR_COEFF]; /*!< storage for non-cal channel's PFIR coeffs */
    HAL_pfirSel_t pfir;                 /*!< PFIR type */
    uint16_t num_pfir_coefs;            /*!< number of PFIR coefficients to save and restore */

    /* I/Q raw data */
#if 0 
    complex int16_t *tx;              /*!< Tx I/Q data for the channel    */
    complex int16_t *rx;              /*!< Rx I/Q data for the channel    */
#else
    int16_t *tx;              /*!< Tx I/Q data for the channel    */
    int16_t *rx;              /*!< Rx I/Q data for the channel    */

#endif

    /* status */
    txqec_init_status_t *status;        /*!< pointer to status structure */

    int32_t phase_adjust;               /*!< phase correction adjustment */
    int32_t gain_adjust;                /*!< gain correction adjustment */
    int16_t gd_adjust[2];               /*!< group delay correction */
    int32_t digital_phase_adjust;       /*!< not used */

    txqec_channel_matrix_t loopback_estimate; /*!< low freq channel estimate */
    txqec_phs_gain_t       adj;               /*!< phase and gain adjustment values */
    uint32_t               num_samples;       /*!< number of samples in corr capture */
    int32_t                gd_freq;           /*!< group delay test tone (Hz) */
    int32_t                gd_freq_image;     /*!< group delay test tone's image frequency (Hz) */
    uint32_t               gd_freq_pwr;       /*!< group delay test tone power estimate */

} txqec_init_data_t;


/*----------------------------------------------------------------------------
 * TX-QEC TRACK data structures
 *----------------------------------------------------------------------------*/

/* configuration for TX-QEC tracking */
typedef struct {
    uint16_t  precision;         /*!< channel estimation threshold (* TXQEC_PRECISION_EXPONENT) */
    uint16_t  tap_dis;           /*!< correlators enabled/disabled */
    uint32_t  sleep_time;        /*!< How long to sleep when no Tx signal is detected */
    float     min_tx_power;      /*!< minimum power needed to run TXQEC tracking */

    uint8_t   param_adj_scale;   /*!< phase/gain adjustment scaling */
                                 /* upper nibble contains numerator; lower the denominator */
    uint8_t   padding;

    uint8_t   adj_round;         /*!< boolean - when set, round the phase and gain adjustments */

} txqec_track_config_t;

typedef struct {
    txqec_stats_t stats;        /*!< statistics */
    txqec_stats_t stats_cached; /*!< cached statistics - from previous attenuation */
} txqec_track_statics_t;

/* flags for handshaking with test bench for TX-QEC test mode */
typedef struct {
    volatile uint32_t req;
    volatile uint32_t ack;
} txqec_track_test_flags_t;

/*!< TXQEC tracking data */
typedef struct {

    /* inputs */
    uint16_t ch;                         /*!< channel (0 or 1)               */
    uint8_t  iter_cnt;                   /*!< correlator setup for tracking  */
    float    precision;                  /*!< working channel estimation threshold (* TXQEC_PRECISION_EXPONENT) */
    const txqec_track_config_t *config;  /*!< ptr to tracking config data    */
    txqec_table_t *table;                /*!< ptr to global txqec_tables[ch] */
    txqec_track_statics_t *statics;      /*!< persistent data, one for each channel */
    const pathdelay_out_t *pathdelay;    /*!< ptr to pathdelay output data, one for each channel */

    /* status */
    txqec_track_status_t *status;        /*!< pointer to tracking status structure */

    /* outputs */
    uint32_t update_out;              /*!< bit 0 for channel 1, bit 1 for channel 2 */
    txqec_outputs_t out;              /*!< output phase and gain correction values  */

    txqec_outputs_t        orig_out;          /*!< original output phase and gain correction values  */
    txqec_channel_matrix_t loopback_estimate; /*!< low freq channel estimate */
    txqec_phs_gain_t       adj;               /*!< phase and gain adjustment values */
    uint32_t               num_samples;       /*!< number of samples in corr capture */

} txqec_track_data_t;

/*----------------------------------------------------------------------------
 * PROTOTYPES
 *----------------------------------------------------------------------------*/

/* General */
void txqec_compute_loopback_channel_matrix(const txqec_stats_t *s, txqec_channel_matrix_t *matrix);
void txqec_set_calibrated_scaling(uint32_t ch, float cal_phase_scale);
void txqec_compute_adjustment(uint32_t ch,
                              const txqec_channel_matrix_t *channel_matrix,
                              txqec_phs_gain_t *adj,
                              float lo_ghz);

float txqec_compute_phase_delta(const txqec_stats_t *s, const txqec_channel_matrix_t *lf_chan);

void txqec_array_interp(int16_t *p, int32_t low_indx, int32_t high_indx);

void txqec_reset_stats(txqec_stats_t *stats);

void txqec_accum_stats(txqec_stats_t *stats, const txqec_corr__outputs_t *out, uint32_t num_samples);

/* TRACK */
void txqec_track_main(txqec_track_data_t *data);
void txqec_track_abort(txqec_track_data_t *data);
uint32_t txqec_track_datapath_test(txqec_track_data_t *data, txqec_track_test_flags_t *test_flags);

#endif /* __TXQEC_HEADER__ */
