#ifndef _DPD_ADP_H_
#define _DPD_ADP_H_

#include <stdint.h>
#include <complex.h>
#include <string.h>
#include "dpd_utils.h"
#include "dpd_t_p.h"
#include "dpd_filter.h"
#include "dpd_delay_filter_p.h"
#include "dpd_err_codes_t.h"
#include "dpd_utils.h"
#include "math_utils.h"

void dpd_GainPhaseAdjust(dpd_TrackData_t *pData,
                        double complex  *pTx,
                        double complex  *pORx,
                        int32_t size);

dpd_ErrCode_e dpd_SampleProc(dpd_TrackData_t *pData,
                            double complex  *pTx,
                            double complex  *pORx,
                            uint16_t captureIndex);

double dpd_Variance(const double complex *x, int32_t size);

dpd_ErrCode_e dpd_CoeffEstimate(dpd_TrackData_t *pData,
                                double complex *pTx,
                                double complex *pORx,
                                uint16_t capSize,
                                uint8_t  capBatch);
#endif