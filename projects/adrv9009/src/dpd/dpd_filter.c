#include "dpd_filter.h"

void dpd_FirResetBuf(dpd_Filter_t *pFilter,
                    double *pFilterCoef,
                    double complex *pInBuffer,
                    double complex *pOutBuffer,
                    uint32_t filterLen,
                    uint32_t outBufLen)
{
    pFilter->pCoefBuf    = pFilterCoef;
    pFilter->pInputBuf   = pInBuffer;
    pFilter->pOutputBuf  = pOutBuffer;
    pFilter->inputBufPtr = 0u;
    pFilter->filterLen   = filterLen;
    pFilter->outBufLen   = outBufLen;

    memset((void*)pFilter->pInputBuf, 0, sizeof(double complex) * (uint32_t)filterLen);
    memset((void*)pFilter->pOutputBuf, 0, sizeof(double complex) * (uint32_t)outBufLen);
}

double complex dpd_Fir(dpd_Filter_t *pFilter,
                        double complex input,
                        uint32_t sampleIndex,
                        uint8_t en)
{
    double         *pCoefBuf     = pFilter->pCoefBuf;
    double complex *pInputBuf    = pFilter->pInputBuf;
    double complex *pOutputBuf   = pFilter->pOutputBuf;
    uint32_t       *pInputBufPtr = &pFilter->inputBufPtr;
    uint32_t       filterLen     = pFilter->filterLen;
    uint32_t       outBufLen     = pFilter->outBufLen;


    uint32_t       i, j;
    double complex sum = input;
    double complex coefficient, product;
    if (en)
    {
        sum = DBL_CPX_ZERO;

        /* Update input circular buffer index */
        *pInputBufPtr = (*pInputBufPtr == 0) ? (filterLen - 1) : (*pInputBufPtr - 1);

        /* Place input date value into input buffer */
        pInputBuf[*pInputBufPtr] = input;

        /* Sum over filter numerator coefficients */
        for (i = 0, j = *pInputBufPtr;
             i < filterLen; i++, j++)
        {
            j          %= filterLen;
            coefficient = (double)pCoefBuf[i] + 0.0 * I;
            product     = coefficient * pInputBuf[j];
            sum         = sum + product;
        }
    }

    pOutputBuf[sampleIndex % outBufLen] = sum;
    return (sum);
}