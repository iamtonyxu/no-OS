#ifndef _DPD_MATRIXSOLVER_H_
#define _DPD_MATRIXSOLVER_H_
#include <stdint.h>
#include <complex.h>
#include <math.h>
#include "math_utils.h"
#include "dpd_t_p.h"
#include "dpd_model_t.h"
#include "dpd_utils.h"
#include "dpd_err_codes_t.h"


/*!
 *******************************************************************************
 * @brief       We decompose the positive symmetric correlation matrix C => L^H*L
 *              using the cholesky decomposition.
 *
 * @details     none
 *
 * Parameters:
 * @param [in]     pData - pointer to DPD data structure
 * @param [in]     c     - pointer to the output array of XCORR, lower half contains the real part
 *                         upper half contains the imag part
 * @param [in]     l     - pointer to the output array of the output of cholesky, lower half contains
 *                         the real part and upper half contains the image part
 *
 * @return      err
 *
 * Reference to other related functions
 * @sa
 *
 *******************************************************************************
 */
dpd_ErrCode_e dpd_CholeskyDecomp(uint8_t features,
                                double rawRegValue,
                                uint8_t linearTermFeatureIndex,
                                const double c[],
                                double l[],
                                double complex a[]);



void dpd_updateFeatureCoeffs(dpd_TrackData_t *pData, double complex *a);
#endif