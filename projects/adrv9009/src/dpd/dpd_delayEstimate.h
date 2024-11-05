#ifndef _DPD_DELAYESTIMATE_H_
#define _DPD_DELAYESTIMATE_H_

#include <stdint.h>
#include <complex.h>
#include <string.h>
#include "dpd_utils.h"
#include "math_utils.h"
#include "dpd_t_p.h"
#include "dpd_err_codes_t.h"

double complex dpd_XcorrOneLag(const double complex *pX, 
                                const double complex *pY, 
                                int32_t xSize, 
                                int32_t ySize,
                                int32_t lag);

int32_t dpd_Xcorr(const double complex *pTx,
                    const double complex *pORx,
                    double *pResult,
                    int32_t xSize,
                    int32_t ySize,
                    int32_t lagIndex,
                    int32_t lagRange);

void dpd_FilterXCorr(const double complex *pTx,
                        const double complex *pORx,
                        int32_t xSize,
                        int32_t ySize,
                        int32_t optIntDelay,
                        int32_t intDelayRange,
                        int32_t fracDelay,
                        double complex *result);

dpd_ErrCode_e dpd_DelayEstimate(dpd_TrackData_t *pData,
                               const double complex *pTx,
                               const double complex *pORx);

dpd_ErrCode_e dpd_SearchRefSignal(double complex *pTx,
                                double complex *pORx,
                                double complex *pRef,
                                uint16_t xSize,
                                uint16_t ySize);

#endif