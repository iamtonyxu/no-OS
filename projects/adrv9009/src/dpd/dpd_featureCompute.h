#ifndef _DPD_FEATURECOMPUTE_H_
#define _DPD_FEATURECOMPUTE_H_
#include <stdint.h>
#include <complex.h>
#include <math.h>
#include "math_utils.h"
#include "dpd_t_p.h"
#include "dpd_model_t.h"
#include "dpd_utils.h"

uint32_t dpd_FeatCompute(const dpd_TrackData_t *pData,
                    uint32_t xIdxStart,
                    uint32_t xIdxEnd,
                    uint8_t discardSamplesEn,
                    double complex *pFfInputBuf);

void dpd_QUMatrixCompute(uint16_t featureSampleComputed,
                        uint16_t features,
                        const double complex *pFfInputBuf,
                        double *pCholeskyDecompBuf);

void dpd_QUMatrixDamping(double *pCholeskyDecompBuf, double damping);

#endif