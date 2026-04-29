#include "dpd_adp.h"

void dpd_GainPhaseAdjust(dpd_TrackData_t *pData,
                        double complex  *pTx,
                        double complex  *pORx,
                        int32_t size)
{
#define USE_APPROACH_1    (0)

/*
 *   By using this approach 1, the mean square error of the gain-phase adjustment
 *   estimation is obviously higher than the alternative approach. This can be verified
 *   by running the following MATLAB script :
 *   verifyGainPhaseAdjust.m
 */
    double         gain;
    double complex adjust;
    int32_t        index;
#if (USE_APPROACH_1)
    int            ixCount         = 0;
    double         totalPhaseDelta = 0;
    double         avgPhaseDelta;
#else
    double complex xcorrVal = 0.00001 + 0 * I;
    double complex acorrVal = 0 + 0 * I;
#endif

#if (USE_APPROACH_1)
    if (size > 0)
    {
        gain = sqrt(dpd_AdpVariance(pTx, size) / dpd_AdpVariance(pORx, size));
    }
    else
#endif
    {
        /* added if statement for fixing Coverity error */
        gain = 1;
    }

    for (index = 0; index < size; index++)
    {
#if (USE_APPROACH_1)
        pORx[index] = pORx[index] * gain;
        if (cabs(pORx[index]) > 0)
        {
            totalPhaseDelta += carg(pTx[index] / pORx[index]);
            ixCount++;
        }
#else
        xcorrVal += pORx[index] * conj(pTx[index]);
        acorrVal += pTx[index] * conj(pTx[index]);
#endif
    }

#if (USE_APPROACH_1)
    avgPhaseDelta = totalPhaseDelta / ixCount;
    adjust        = cexp(0 + avgPhaseDelta * I);
#else
    adjust = acorrVal / xcorrVal;
#endif

    for (index = 0; index < size; index++)
    {
        pORx[index] = pORx[index] * adjust;
    }

    if (pData->direct)
    {
        pData->statics.invDirectLoopGain = gain * adjust;
    }
    else
    {
        pData->statics.invIndirectLoopGain = gain * adjust;
    }
}

dpd_ErrCode_e dpd_SampleProc(dpd_TrackData_t *pData,
                            double complex  *pTx,
                            double complex  *pORx,
                            uint16_t captureIndex)
{
    dpd_ErrCode_e       err = DPD_ERR_CODE_NO_ERROR;
    dpd_PathDlyStatus_t *pDpd_PathDlyStatus = &(pData->pathDlyStatus);
    int32_t             indexTx, indexORx;
    dpd_Filter_t        fracDelayFilter;
    dpd_Filter_t        txFilter;
    double              *pFracFilterCoef;
    double              *pTxFilterCoef;

    double complex      inFracDelayFilterBuffer[DPD_FILTER_FRAC_LEN];
    double complex      outFracDelayFilterBuffer[DPD_FILTER_BUF_LEN];

    double complex      inTxFilterBuffer[DPD_FILTER_TX_LEN];
    double complex      outTxFilterBuffer[DPD_FILTER_BUF_LEN];

    double complex      fracDelayedORx, filteredTx;
    double complex      txSample, orxSample;
    uint8_t             txFilterEn = 0;    /* set to 0 for now and to be removed */
    int32_t             txFilGroupDelay;
    int32_t             orxFilGroupDelay;
    int32_t             maxNumPrimeSamples;
    int32_t             adjustedIndex;
    uint16_t            filterSel        = pData->pTrackCfg->filterSel;
    int32_t             size             = pData->capCfg.capDepth;
    uint32_t            sampleCount      = pData->alignedSampleCount;
    uint32_t            alignedCntPerCap = 0ul;

    txFilGroupDelay    = (txFilterEn) ? DPD_FILTER_TX_GROUP_DELAY : 0;
    orxFilGroupDelay   = DPD_FILTER_FRAC_INT_GROUP_DELAY;
    maxNumPrimeSamples = (txFilGroupDelay > orxFilGroupDelay) ?
                         DPD_FILTER_TX_LEN - 1 : DPD_FILTER_FRAC_LEN - 1;

    pFracFilterCoef = &dpd_FilterFracCoefs[DPD_FILTER_FRAC_DELAY_NUM - 1 - pDpd_PathDlyStatus->fracDelay][0];
    pTxFilterCoef   = &dpd_FilterTxCoefs[filterSel][0];

    dpd_FirResetBuf(&fracDelayFilter,
                    pFracFilterCoef,
                    inFracDelayFilterBuffer,
                    outFracDelayFilterBuffer,
                    DPD_FILTER_FRAC_LEN,
                    DPD_FILTER_BUF_LEN);

    dpd_FirResetBuf(&txFilter,
                    pTxFilterCoef,
                    inTxFilterBuffer,
                    outTxFilterBuffer,
                    DPD_FILTER_TX_LEN,
                    DPD_FILTER_BUF_LEN);

    for (int32_t index = 0; (index + ABS(pDpd_PathDlyStatus->intDelay)) < size; index++)
    {
        indexORx  = (pDpd_PathDlyStatus->intDelay < 0) ? index - pDpd_PathDlyStatus->intDelay : index;
        indexTx   = (pDpd_PathDlyStatus->intDelay > 0) ? index + pDpd_PathDlyStatus->intDelay : index;
        orxSample = pORx[indexORx];
        txSample  = pTx[indexTx];

        fracDelayedORx = dpd_Fir(&fracDelayFilter, orxSample, (uint32_t)index, DPD_ENABLE);
        filteredTx     = dpd_Fir(&txFilter, txSample, (uint32_t)index, txFilterEn);

        if (index >= maxNumPrimeSamples)
        {
            if (txFilGroupDelay > orxFilGroupDelay)
            {
                adjustedIndex = index - txFilGroupDelay + orxFilGroupDelay;
                txSample      = filteredTx;
                orxSample     = outFracDelayFilterBuffer[adjustedIndex % DPD_FILTER_BUF_LEN];
            }
            else
            {
                adjustedIndex = index - orxFilGroupDelay + txFilGroupDelay;
                orxSample     = fracDelayedORx;
                txSample      = outTxFilterBuffer[adjustedIndex % DPD_FILTER_BUF_LEN];
            }

            pData->pTx[pData->alignedSampleCount]    = txSample;
            pData->pORx[pData->alignedSampleCount++] = orxSample;
            alignedCntPerCap++;
        }
    }

    pData->firstSampleIndex[captureIndex] = sampleCount;
    pData->lastSampleIndex[captureIndex] = pData->alignedSampleCount;

#if 1
    MEX_LOG("firstSampleIndex[%d] = %d\n", captureIndex, pData->firstSampleIndex[captureIndex]);
    MEX_LOG("lastSampleIndex[%d] = %d\n", captureIndex, pData->lastSampleIndex[captureIndex]);
#endif

    /* update total aligned sample count */
    alignedCntPerCap -= DPD_DELAY_BUF_SIZE;
    alignedCntPerCap -= (captureIndex == 0) ? pData->pTrackCfg->numFilterCoefficients : 0;
    uint32_t numBlocks = alignedCntPerCap / pData->pTrackCfg->numFilterOutput;

    pData->alignedSampleCount  = sampleCount + (numBlocks * pData->pTrackCfg->numFilterOutput + DPD_DELAY_BUF_SIZE);
    pData->alignedSampleCount += (captureIndex == 0) ? pData->pTrackCfg->numFilterCoefficients : 0;

    /* Perform Gain and Phase Adjustment between  Tx and ORx samples */
    dpd_GainPhaseAdjust(pData,
                        &(pData->pTx[sampleCount]),
                        &(pData->pORx[sampleCount]),
                        (int32_t)(pData->alignedSampleCount - sampleCount));

    for (uint32_t index = sampleCount; index < pData->alignedSampleCount; index++)
    {
        txSample  = pData->pTx[index];
        orxSample = pData->pORx[index];

        if (pData->direct)
        {
            pData->pTx[index]  = txSample;
            pData->pORx[index] = orxSample - txSample;
        }
        else
        {
            pData->pTx[index]  = orxSample;
            pData->pORx[index] = txSample;
        }
    }

    return (err);
}

double dpd_Variance(const double complex *x, int32_t size)
{
    double complex sumMean = 0;
    double         sumVar  = 0;
    double complex mean;
    double         variance;
    int32_t        i;
    double         d;
    int32_t        n = size;

    for (i = 0; i < size; i++)
    {
        sumMean += x[i];
    }

    mean = sumMean / n;

    for (i = 0; i < size; i++)
    {
        d       = cabs(x[i] - mean);
        sumVar += d * d;
    }

    if (n > 1)
    {
        n--;
    }
    variance = sumVar / n;

    return (variance);
}