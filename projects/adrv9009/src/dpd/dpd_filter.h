#ifndef _DPD_FILTER_H_
#define _DPD_FILTER_H_

#include <stdint.h>
#include <complex.h>
#include <string.h>
#include "dpd_utils.h"
#include "math_utils.h"

typedef struct
{
    uint32_t       inputBufPtr;          /* Filtering input buffer pointer */
    uint32_t       filterLen;            /* FIR filter length */
    uint32_t       outBufLen;            /* Output buffer length */
    double complex *pInputBuf;           /* Filtering input buffer */
    double complex *pOutputBuf;          /* Filtering output buffer */
    double         *pCoefBuf;            /* filter coefficient buffer */
} dpd_Filter_t;

void dpd_FirResetBuf(dpd_Filter_t *pFilter,
                    double *pFilterCoef,
                    double complex *pInBuffer,
                    double complex *pOutBuffer,
                    uint32_t filterLen,
                    uint32_t outBufLen);

double complex dpd_Fir(dpd_Filter_t *pFilter,
                        double complex input,
                        uint32_t sampleIndex,
                        uint8_t en);

#endif