/*! **************************************************************************** 
 *
 * @file:    txqec.c
 *
 * @brief:   txqec algorithmic helper functions (non-hardware related)
 *
 * @details: 
 *
 * @version: $Revision:
 *
 * @date:    $Date:
 *
 *******************************************************************************
 */

#include <math.h>
#include <string.h>
#include <stdint.h>
#include "txqec.h"
#include "txqec_hw.h"

static float txqec_adj_phase_scale_factor[2];

static const txqec_float_t filter_lower[] = {0.0000, 1.0000, 0.0000}; /* No filtering for lower sampling frequencies */
static const txqec_float_t filter_245[]   = {0.2368, 0.5263, 0.2368}; /* -25dB rejection of high frequency noise     */


/**
 ***************************************************************************
 * Function: txqec_reset_stats
 *
 * @brief    Reset/clear given stat structure
 *
 * @details     
 *
 * Parameters: 
 * @param [in,out]  stats - pointer txqec_stats_t structure
 *
 * @return      None
 *
 ***************************************************************************
*/
void txqec_reset_stats(txqec_stats_t *stats) {
    memset((void*)stats, 0, sizeof(txqec_stats_t));
}

/**
 ***************************************************************************
 * Function: txqec_accum_stats
 *
 * @brief    Filter the correlator output and accumulate in txqec_stats_t *stats
 *           Also, calculate and save the current sum of correlated values minus
 *           the sum of dc and store in stats->c_no_dc.
 *
 * Parameters: 
 * @param [out]  stats - txqec_stats_t pointer
 * @param [in]   out   - txqec_corr__outputs_t pointer to the raw correlator output
 * @param [in]   num_samples   - number of samples correlated to produce "out"
 *
 * @return      None
 *
 ***************************************************************************
*/
void txqec_accum_stats(txqec_stats_t *stats, const txqec_corr__outputs_t *out, uint32_t num_samples)
{
    uint32_t i;
    const txqec_float_t * filter;
    filter = filter_lower; /* default filter */

    for (i = 0; i < 3; i++) {
        stats->c_sum.zu += filter[i] * (txqec_float_t)out->r_zu[i];
        stats->c_sum.yu += filter[i] * (txqec_float_t)out->r_yu[i];
        stats->c_sum.zv += filter[i] * (txqec_float_t)out->r_zv[i];
        stats->c_sum.yv += filter[i] * (txqec_float_t)out->r_yv[i];
    }
    for (i = 0; i < 2; i++) {
        stats->c_sum.uu += filter[1-i] * (txqec_float_t)out->r_uu[i];
        stats->c_sum.vv += filter[1-i] * (txqec_float_t)out->r_vv[i];
        stats->c_sum.uv += filter[1-i] * (txqec_float_t)out->r_uv[i];
    }
    for (i = 1; i < 2; i++) {
        stats->c_sum.uu += filter[1-i] * (txqec_float_t)out->r_uu[i];
        stats->c_sum.vv += filter[1-i] * (txqec_float_t)out->r_vv[i];
        stats->c_sum.uv += filter[1-i] * (txqec_float_t)out->r_vu[i];
    }

    stats->dc_sum.u += (txqec_float_t)out->dc_u;
    stats->dc_sum.v += (txqec_float_t)out->dc_v;
    stats->dc_sum.z += (txqec_float_t)out->dc_z;
    stats->dc_sum.y += (txqec_float_t)out->dc_y;

    stats->num_samples += (txqec_float_t)num_samples; /* must use floating point to avoid overflow */

    /* Remove the accumulated DC from the accumulated correlations and store in c_no_dc */
    stats->c_no_dc.uu = stats->c_sum.uu - (stats->dc_sum.u * stats->dc_sum.u) / stats->num_samples;
    stats->c_no_dc.vv = stats->c_sum.vv - (stats->dc_sum.v * stats->dc_sum.v) / stats->num_samples;
    stats->c_no_dc.uv = stats->c_sum.uv - (stats->dc_sum.u * stats->dc_sum.v) / stats->num_samples;
    stats->c_no_dc.zu = stats->c_sum.zu - (stats->dc_sum.z * stats->dc_sum.u) / stats->num_samples;
    stats->c_no_dc.zv = stats->c_sum.zv - (stats->dc_sum.z * stats->dc_sum.v) / stats->num_samples;
    stats->c_no_dc.yu = stats->c_sum.yu - (stats->dc_sum.y * stats->dc_sum.u) / stats->num_samples;
    stats->c_no_dc.yv = stats->c_sum.yv - (stats->dc_sum.y * stats->dc_sum.v) / stats->num_samples;

} /* txqec_accum_stats */

/**
***************************************************************************
 * Function: txqec_compute_loopback_channel_matrix
 *
 * @brief    Given the TXQEC channel statistics, estimate the loopback channel
 *
 * @details  Uses the TX-QEC correlator results to estimate the 2x2 linear channel
 *           between TX I/Q and LBRX I/Q (or ORX).
 *
 *------------------------------------------------------------------------------
 * Solve for the quadrature-imbalances (a 2x2 crossover channel)
 * TX INPUT  (in-phase)  u ----a---[+]---->  z  (in-phase)       OUTPUT (LOOPBACK RX)
 *                           \     ^ 
 *                            b   /
 *                             \ /
 *                             / \
 *                            c   \
 *                           /     v
 *         (quad-phase)  v ----d-->[+]---->  y  (quad-phase)
 *
 * Matrix formulation of covariances,
 *    | Cuu  Cuv | * | a  b |  =  | Czu  Cyu |
 *    | Cuv  Cvv |   | c  d |     | Czv  Cyv |
 * Solving,
 *    | a  b |  =  | Cuu  Cuv |^-1  *  | Czu  Cyu |  =  | Cvv -Cuv |     *  | Czu  Cyu |
 *    | c  d |     | Cuv  Cvv |        | Czv  Cyv |     |-Cuv  Cuu |/det    | Czv  Cyv |
 *
 *------------------------------------------------------------------------------
 * Parameters: 
 * @param [in]  s      - txqec statistics structure pointer
 * @param [out] matrix - txqec_channel_matrix_t pointer
 *
 * @return      None
 *
 **************************************************************************
*/
void txqec_compute_loopback_channel_matrix(const txqec_stats_t *s,
                                           txqec_channel_matrix_t *matrix) {

    txqec_float_t det;
    const corr_data_t *c = &s->c_no_dc; /* point to the corr results with DC removed */
    
    det = (c->uu * c->vv) - (c->uv * c->uv);

    if ((det * det) >= (txqec_float_t)1e6) { /* check if not nearly singular (signal too small or u nearly identical to v) */
        matrix->a = (( c->zu * c->vv) - (c->zv * c->uv)) / det;
        matrix->b = (( c->yu * c->vv) - (c->yv * c->uv)) / det;
        matrix->c = ((-c->zu * c->uv) + (c->zv * c->uu)) / det;
        matrix->d = ((-c->yu * c->uv) + (c->yv * c->uu)) / det;

        /* This is proportional to the total input power Cuu + Cvv and the
         * attenuation of the transformation (1/(e^2+f^2) = a^2+b^2)
         */
        matrix->precision  = (c->uu + c->vv);
        matrix->precision *= ((matrix->a * matrix->a) + (matrix->b * matrix->b));
    } else {
        memset((void*)matrix, 0, sizeof(txqec_channel_matrix_t));
    }

    /* matrix contains:
        | a  b |
        | c  d |    */

} /* txqec_compute_loopback_channel_matrix */

/**
 ***************************************************************************
 * Function: txqec_set_calibrated_scaling
 *
 * @brief    Store the phase correction scale factor for the given channel
 *
 * Parameters: 
 * @param [in]  ch              - {0 | 1}
 * @param [in]  cal_phase_scale - calibrated phase scale value
 *
 * @return      None
 *
 ***************************************************************************
*/
void txqec_set_calibrated_scaling(uint32_t ch, float cal_phase_scale) {

    txqec_adj_phase_scale_factor[ch] = TXQEC_PHASE_SCALE * cal_phase_scale;
}

void txqec_get_calibrated_scaling(uint32_t ch, float *cal_phase_scale) {

    if (cal_phase_scale != NULL) {
        *cal_phase_scale = txqec_adj_phase_scale_factor[ch] / TXQEC_PHASE_SCALE;
    }
} /* txqec_get_calibrated_scaling */

/**
 ***************************************************************************
 * Function: txqec_compute_adjustment
 *
 * @brief    Using the channel estimate, compute the gain and phase correction adjustments
 * @details  
 *
 * Computes gain, phase Quadrature Error Correction values based on the 2x2 linear TX->LBRX loopback channel
 * Assumes group delay is already corrected or has little effect (i.e., signal is near baseband DC)
 *
 *----------------------------------------------------------------------------------------------------
 * The 2x2 matrix loopback channel contains: (1) Overall gain between inputs (u,v) and outputs (z,y)
 *         ^ (a,b,c,d)                       (2) Gain imbalance (gain difference between u->z and v->y)
 *                                           (3) Phase imbalance (from TX I&Q mixers not having exactly 90 deg. relative phase)
 *                                           (4) LO-offset (a rotation, due to different TX and RX mixer phase)
 *
 * We only are correcting (2) and (3) which can be represented with the following channel,
 * TX INPUT  (in-phase)  u -------------->  z  (in-phase)       OUTPUT (LOOPBACK RX)
 *                                 |
 *                                 p
 *                                 |
 *         (quad-phase)  v ----g--[+]---->  y  (quad-phase)
 *
 * The a,b,c,d matrix can be transformed into this (p, g) form by a scale/rotation matrix (e ,f)
 * i.e., multiplication by a complex constant  e + j f, corresponding to transformations (1) and (4).
 *    | a  b |  *  | e  -f |   =   |  1    0  |
 *    | c  d |     | f   e |       |  p    g  |
 *
 * Solving for the new variables, (don't actually need to calculate these)
 *   p = (a*c + b*d)/(a^2 + b^2)           e = a / (a^2 + b^2)
 *   g = (a*d - b*c)/(a^2 + b^2)           f = b / (a^2 + b^2)
 *
 * Calculation of Phase correction:
 *
 * Phs Corr = 512 (1 + deltaT / Tau-FS)
 * deltaT = (Phi-rad / 2PI) * (1 / F-lo)
 * Tau-FS = (3/3) * 4 picosec
 *
 * Phs Corr = 512 (1 + Phi-rad * 1/2PI * 1/F-lo * 1/Tau-FS)
 *          = 512 (1 + Phi-rad * 39.8 / F-lo-ghz)
 *
 * Talise theoretical:
 * slope = delta-Corr / delta-Phi-rad = 512 * 39.8 / F-lo =  20372 / F-lo-ghz
 *
 * Mykonos:
 * slope = delta-Corr / delta-Phi-rad = 512 * 29.8 / F-lo =  15283 / F-lo-ghz
 *------------------------------------------------------------------------------
 * Parameters: 
 * @param [in]  channel        - {0 | 1}
 * @param [in]  channel_matrix - txqec_channel_matrix_t structure pointer (channel estimate)
 * @param [out] adj            - txqec_phs_gain_t structure pointer
 *
 * @return      None
 *
 **************************************************************************
*/
void txqec_compute_adjustment(uint32_t ch,
                              const txqec_channel_matrix_t *channel_matrix,
                              txqec_phs_gain_t *adj,
                              float lo_ghz) {

    txqec_float_t a, b, c, d;
    txqec_float_t gain_correction, phase_correction;
    //float lo_ghz = (float)HAL_GetRfLoFreq() / 1.0e9f;

    a = channel_matrix->a;
    b = channel_matrix->b;
    c = channel_matrix->c;
    d = channel_matrix->d;

    /* To correct for the imbalance, we compute the inverse gain (1/g) and the correcting phase (-p), */
    gain_correction = sqrt((a*a + b*b) / (c*c + d*d)) - 1.0; /* subtract 1 because gain knob is relative to 1 */

    phase_correction =  -(a*c + b*d) / (a*d - b*c); /* radians; atan() small angle approximation */

    /* Scale to code words */
    adj->gain  = (float)gain_correction * TXQEC_GAIN_SCALE;

    adj->phase = ((float)phase_correction * txqec_adj_phase_scale_factor[ch]) / lo_ghz;

} /* txqec_compute_adjustment */

/**
 ***************************************************************************
 * Function: txqec_compute_phase_delta
 *
 * @brief    Using the channel estimate, compute the phase delta correction
 *
 * @details  Computes the phase delay between I and Q baseband Tx paths
 * given a nonzero frequency CW and a known low-freq 2x2 linear channel
 * (used to de-rotate channel due to residual loopback delay).
 * Assume phase and gain quadrature imbalance have already been corrected
 * and signal is a high freq CW.
 *
 * Parameters: 
 * @param [in]      s           txqec_stats_t structure pointer
 * @param [in]      lf_chan     low frequency channel estimate structure ptr
 *
 * @return      phase delta
 *
 **************************************************************************
*/
float txqec_compute_phase_delta(const txqec_stats_t *s,
                                const txqec_channel_matrix_t *lf_chan) {

    float phase_delta;
    txqec_float_t Czu, Cyu, Czv, Cyv; /* de-rotated correlations */
    txqec_float_t det;
    const corr_data_t *c = &s->c_no_dc; /* point to the corr results with DC removed */

    det = (lf_chan->a * lf_chan->d) - (lf_chan->b * lf_chan->c);

    /* Derotate the cross correlation matrix
     *   | Czu  Cyu |   |  d  -b |
     *   | Czv  Cyv |   | -c   a |  / det
     */
    Czu = (( lf_chan->d * c->zu) - (lf_chan->c * c->yu)) / det;
    Cyu = ((-lf_chan->b * c->zu) + (lf_chan->a * c->yu)) / det;
    Czv = (( lf_chan->d * c->zv) - (lf_chan->c * c->yv)) / det;
    Cyv = ((-lf_chan->b * c->zv) + (lf_chan->a * c->yv)) / det;

    /* calculate group delay (assumes phase and gain already corrected), and signal is high frequency CW */
    phase_delta  = ( Czv * Cyv) + (Czu * Cyu);
    phase_delta /= (-Czu * Cyv) + (Czv * Cyu);

    phase_delta *= (16536.0f * 8.0f); /* 8.0 is a scale factor */

    return (phase_delta);

} /* txqec_compute_phase_delta */


/**
 ***************************************************************************
 * Function: txqec_array_interp
 *
 * @brief    Perform a linear interpolation between two points in an array
 *
 * @details  Fill in p[low_indx + 1 .. high_indx - 1] with points based on
 *           p[low_indx .. high_indx]
 *
 * Parameters: 
 * @param [in,out]  p         - pointer to array
 * @param [in]      low_indx  - low index of the array
 * @param [in]      high_indx - high index of the array
 *
 * @return      None
 *
 **************************************************************************
*/
void txqec_array_interp(int16_t *p, int32_t low_indx, int32_t high_indx) {

    int32_t param_low  = (int32_t)p[low_indx];
    int32_t param_high = (int32_t)p[high_indx];

    for (int32_t i = low_indx + 1; i < high_indx; i++) {
        int32_t interp = param_high - param_low;
        interp *= (int32_t) (i - low_indx);
        interp /= (int32_t) (high_indx - low_indx);
        interp += param_low;
        p[i] = (int16_t) interp; /* record interpolated value to array */
    }
} /* txqec_array_interp */


/*@}*/
