/*! **************************************************************************** 
 *
 * @file:    txqec_init.c
 *
 * @brief:   Performs TXQEC initial calibration
 *
 * @details: 
 *
 * @version: $Revision:
 *
 * @date:    $Date:
 *
 *******************************************************************************
*/

#include "txqec.h"
#include "txqec_hw.h"
#include "error_code_t.h"


#define NCO_LOW_FREQ_QBIN        (4) /*!< NCO frequency used for channel estimate; qbin 4 is same as bin = 1 */

#define PARAM_ADJ_SCALE       (0.5f) /*!< phase/gain adjustment scaler (same as tracking default) */

#define NUM_EST_LOOPS            (5) /*!< Number of channel estimate loops to perform per attenuation */

#define NUM_PHASE_CAL_LOOPS      (4) /* txqec_calc_phase_scale() */

#define PHS_SCALE_FACTOR_LOW_LIMIT   (0.3f) /*!< phase calibration limits */
#define PHS_SCALE_FACTOR_HIGH_LIMIT  (3.0f)

#define PHASE_CAL_OFFSET           (128)  /* phase = +/- 1/8th of range */

#define PERCENT_PER_ATTEN_LOOP     (100 / (TXQEC_ATTEN_POINTS - 2)) /* assumes default config */

/*-------------------------------
 * Group Delay #defines structs
 *-------------------------------*/
#define MAX_NUM_GD_EST_PASSES_PER_ATTEN  (3)  /*!< upper limit of GD est passes per attenuation */

#define NUM_GD_EST_PASSES_ATTEN_1  (2)
#define NUM_GD_EST_PASSES_ATTEN_2  (3)  /* total of 5 estimation passes over two attenuations */

#if ((NUM_GD_EST_PASSES_ATTEN_1 > MAX_NUM_GD_EST_PASSES_PER_ATTEN) || \
     (NUM_GD_EST_PASSES_ATTEN_2 > MAX_NUM_GD_EST_PASSES_PER_ATTEN))
#error "NUM_GD_EST_PASSES_ATTEN_x must be <= MAX_NUM_GD_EST_PASSES_PER_ATTEN"
#endif

#define GD_HD2_GAIN              (4)
#define NUM_GD_MEAS_HIST         (8)

/*-----------------------------
 * Static Function Prototypes
 *-----------------------------*/

static ErrorCode_e txqec_init_find_channel_estimate(int32_t qbin,
                                                 txqec_init_data_t *data);

//static ErrorCode_e txqec_calc_phase_scale(txqec_init_data_t *data);

/*-----------------------------
 * External Function Prototypes
 *-----------------------------*/


/**
 ***************************************************************************
 * Function: txqec_init_find_channel_estimate
 *
 * @brief    Find the low frequency or frequency independent channel estimate.
 *
 * @details  Channel estimate stored in: data->loopback_estimate
 *           Adjustments needed to phase and gain stored in data->adj
 *
 * Parameters:
 * @param [in]      qbin - frequency quarter bin
 * @param [in,out]  data - pointer of type txqec_init_data_t
 *
 * @return      error status - TXQEC_INIT_ERR_NO_ERROR
 *                             TXQEC_INIT_ERR_CORR_TIMEOUT_ABORT
 *
 ***************************************************************************
*/
static ErrorCode_e txqec_init_find_channel_estimate(int32_t qbin,
                                                    txqec_init_data_t *data) {

    ErrorCode_e err_code = TXQEC_INIT_ERR_NO_ERROR;
    txqec_corr__outputs_t out;

    txqec_tone_download(data->channel, qbin);

    while(1)
    {
        // run txqec capture
        err_code = txqec_data_capture(data);
        if(err_code != TXQEC_INIT_ERR_NO_ERROR) {
            err_code = TXQEC_INIT_ERR_CAP_FAIL;
            break; /* error or timeout */
        }

        // compute the correlation results
        err_code = txqec_corr_compute(data);
        if(err_code != TXQEC_INIT_ERR_NO_ERROR) {
            err_code = TXQEC_INIT_ERR_CORR_FAIL;
            break; /* error or timeout */
        }

        data->status->batch_cnt++;

        /* read and accumulate data from correlator */
        txqec_accum_stats(data->stats, &out, data->num_samples);

        /* estimate gain and phase parameters */
        txqec_compute_loopback_channel_matrix(data->stats, &data->loopback_estimate);

        /* Check if the precision in our calculation is 'good' enough. */
        if (data->loopback_estimate.precision >= data->precision) {
            /* If here, we have collected enough statistics for a high-precision estimate and can proceed. */
            /* Compute the quadrature error correction (adjustment) from the measured 2x2 linear loopback channel */
            txqec_compute_adjustment(data->channel, &data->loopback_estimate, &data->adj, data->lo_ghz);
            break;
        }
    } /* end while(1) */

    txqec_reset_stats(data->stats); /* changes made to knobs, correlations now invalid. */

    return err_code;
}

#if 0
/**
 ***************************************************************************
 * Function: txqec_calc_phase_scale
 *
 * @brief    Determine the scale factor needed for the phase value
 *           to produce a one-to-one relationship between this and
 *           the correction value.
 *
 * @details  Assumes that the phase correction value has been
 *           set such that the adjustment value is small.
 *           The gain scaler doesn't need to be calculated because it is
 *           simply a digital multiplier; it doesn't vary from part-to-part.
 *           Measure two sets of offsets from the original settings.
 *           A measurement is the absolute value of the actual variation
 *           versus the ideal variation.
 *           scale factor = Ideal variation / Maximum absolute variation
 *
 *           pass scale factor to txqec_set_calibrated_scaling()
 *
 * Parameters: 
 * @param [in,out]  data - pointer of type txqec_init_data_t
 *
 * @return      error status
 *
 ***************************************************************************
*/
static uint32_t txqec_calc_phase_scale(txqec_init_data_t *data) {

    uint32_t err_code = TXQEC_INIT_ERR_NO_ERROR;
    int16_t  set_delta_value = PHASE_CAL_OFFSET;
    int16_t  orig_set_value = data->phase_adjust;
    int16_t  set_value = orig_set_value;
    float    adj_value;
    float    adj_value_max = 0.0;
    int32_t  qbin = NCO_LOW_FREQ_QBIN; /* must set to LOW_FREQ */

    /* check that delta isn't too big */
    if (set_delta_value > DEF_ABS(set_value)) {
        set_delta_value = DEF_ABS(set_value);
    }
    set_value -= set_delta_value;

    for (uint32_t j = 0; j < NUM_PHASE_CAL_LOOPS; j++) {

        if ((j & 1) == 0) {
            set_value += 2 * set_delta_value;
        } else {
            set_value -= 2 * set_delta_value;
        }
        txqec_set_phase_gain_gd(data->channel, PARAM_PHASE, set_value);

        err_code = txqec_init_find_channel_estimate(qbin, data);
        if (err_code != TXQEC_INIT_ERR_NO_ERROR) {
            break; /* scale_factor check will prevent saving a bad value */
        }
        adj_value = DEF_ABS(data->adj.phase);
        if (adj_value > adj_value_max) {
            adj_value_max = adj_value;
        }
        /* no need to restart the NCO (saves time) */
        qbin = QBIN_NO_CHANGE;
    }

    /* restore */
    txqec_set_phase_gain_gd(data->channel, PARAM_PHASE, orig_set_value);

    if (err_code == TXQEC_INIT_ERR_NO_ERROR) {
        float scale_factor;

        /* Only scale by the largest variation. */
        scale_factor = (float)set_delta_value / adj_value_max;

        /* If scale_factor too small or large, set to limit */
        if (scale_factor < PHS_SCALE_FACTOR_LOW_LIMIT) {
            scale_factor = PHS_SCALE_FACTOR_LOW_LIMIT;
        } else if (scale_factor > PHS_SCALE_FACTOR_HIGH_LIMIT) {
            scale_factor = PHS_SCALE_FACTOR_HIGH_LIMIT;
        }
        txqec_set_calibrated_scaling(data->channel, scale_factor);
    }

    return err_code;

} /* txqec_calc_phase_scale */

#endif

/**
 *****************************************************************************
 * Function: txqec_run_init_cal
 *
 * @brief       Perform TXQEC init calibration for data->channel
 *
 * @details     
* - Preconditions: 
    1. Run txqec init/track cal in FW
    2. Then disable txqec track cal
* 
* - Procedures:
*   1. read calibrated txqec outputs via SPI
*       - phase, gain, group delays
*   2. set uninitialized value of phase and gain
*   3. clear statistics
*   4. loop over attenIndexes (but don't change attenIndex at this early stage)
*       - set analog and digital attenuation for this attenIndex
*           - run txqec_data_capture
*               - Send tx tone and capture orx data
*               - align tx and orx
*               - perform correlation of tx and orx
*               - return correlation results
*           - estimate gain and phase parameters
*           - save phase and gain in initTable
*           - if attenIndex == 0
*               - estimate phase and gain scale factor
*   5. apply phase and gain adjustments
 *
 * Parameters:
 * @param [in]  data - txqec_init_data_t
 *
 * @return      error status
 *
 * Reference to other related functions
 * @sa          txqec_track_main
 *
 * Notes: 
 *****************************************************************************
*/
ErrorCode_e txqec_run_init_cal(txqec_init_data_t *data) 
{
    int32_t  qbin, i, j;
    uint32_t atten_index;
    uint32_t local_channel = data->channel;
    txqec_outputs_t out;
    ErrorCode_e rtnVal = TXQEC_INIT_ERR_NO_ERROR;

    /* run txqec FW init/track cal and disable the cal */

    /* read calibrated txqec outputs in HW */
    txqec_get_phase_gain_gd(data->devHalInfo, 
                            local_channel, 
                            &out,
                            (MASK_PARAM_PHASE | MASK_PARAM_GAIN | MASK_PARAM_GROUP_DELAY | MASK_PARAM_GROUP_DELAY_2ND));

    data->init_table->gain_adjust[0] = out.gain; /* save gain in init table */
    data->init_table->phase_adjust[0] = out.phase; /* save phase in init table */
    data->init_table->gd_adjust[0] = out.gd[0];
    data->init_table->gd_adjust[1] = out.gd[1];

    /* Set same values as in txqec_top_init_init() */
    data->phase_adjust = INITIAL_PHASE_VALUE;
    data->gain_adjust = INITIAL_GAIN_VALUE;
    data->digital_phase_adjust = INITIAL_DIG_PHASE_VALUE;

    txqec_set_phase_gain_gd(data->devHalInfo, local_channel, PARAM_PHASE, data->phase_adjust);
    txqec_set_phase_gain_gd(data->devHalInfo, local_channel, PARAM_GAIN, data->gain_adjust);
    txqec_set_phase_gain_gd(data->devHalInfo, local_channel, PARAM_DIGITAL_PHASE, data->digital_phase_adjust);

    /* enable_gd is false as group delay estimation will be skipped */
    if (data->config->enable_gd) {
        data->gd_adjust[0] = INITIAL_GD_VALUE;
        data->gd_adjust[1] = INITIAL_GD_VALUE;

        txqec_set_phase_gain_gd(data->devHalInfo, local_channel, PARAM_GROUP_DELAY, data->gd_adjust[0]);
        txqec_set_phase_gain_gd(data->devHalInfo, local_channel, PARAM_GROUP_DELAY_2ND, data->gd_adjust[0]);
    }

    /* clear statistics */
    txqec_reset_stats(data->stats);

    /* Note: Only run channel estimate with current attenIndex */

    /* Calibrate the phase correction scaling factor when atten_index == 1 */
    #if 0
        rtnVal = txqec_calc_phase_scale(data);
    #endif
    
    for (j = 0; (j < NUM_EST_LOOPS) && (rtnVal == TXQEC_INIT_ERR_NO_ERROR); j++) {

        qbin = -qbin; /* toggle sign of qbin */

        /* the estimate will be made at this qbin frequency */
        rtnVal = txqec_init_find_channel_estimate(qbin, data);
        if (rtnVal != TXQEC_INIT_ERR_NO_ERROR) {
            break;
        }

        /* Only adjust by a fraction of the correction to avoid overshooting. */
        data->adj.gain  *= PARAM_ADJ_SCALE;
        data->adj.phase *= PARAM_ADJ_SCALE;

        if (data->adj.phase > 0.0) { /* round the phase adjustment */
            data->adj.phase += 0.5f;
        } else if (data->adj.phase < 0.0) {
            data->adj.phase -= 0.5f;
        }

        data->gain_adjust  += (int32_t)data->adj.gain;
        data->phase_adjust -= (int32_t)data->adj.phase;

        txqec_set_phase_gain_gd(data->devHalInfo, local_channel, PARAM_PHASE, (int16_t)data->phase_adjust);
        txqec_set_phase_gain_gd(data->devHalInfo, local_channel, PARAM_GAIN,  (int16_t)data->gain_adjust);

    } /* for (j = 0; j < NUM_EST_LOOPS; j++) */

    data->init_table->phase_adjust[1] = data->phase_adjust; /* record settings in table */
    data->init_table->gain_adjust[1] = data->gain_adjust;

    // TODO: Group Delay estimation when atten_index == 1

    // update status
    data->status->iter_count++;
    if(data->status->error_code == TXQEC_INIT_ERR_NO_ERROR) {
        data->status->percent_complete = 100;
        data->status->update_count++;
    }

    return rtnVal;
} /* txqec_run_init_cal */

/*@}*/
