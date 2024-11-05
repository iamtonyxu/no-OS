#include "dpd_t_p.h"
#include "dpd_delay_filter_p.h"
#include "dpd_filter.h"
#include "dpd_delayEstimate.h"
#include "dpd_err_codes_t.h"

double complex dpd_XcorrOneLag(const double complex *pX,
                                  const double complex *pY,
                                  int32_t xSize,
                                  int32_t ySize,
                                  int32_t lag)
{
    double complex sum = DBL_CPX_ZERO;
    double complex product;
    int32_t        index;
    double complex x, y;

    MEX_LOG_DETAILED("xSize:%d ySize:%d lag:%d\n", xSize, ySize, lag);

    if ((lag >= 0) && (lag < xSize))
    {
        for (index = lag; index < xSize; index++)
        {
            if (index - lag < ySize)
            {
                x       = pX[index];
                y       = conj(pY[index - lag]);
                product = x * y;
                sum     = sum + product;
            }
        }
    }
    else if ((lag < 0) && (-lag < ySize))
    {
        for (index = -lag; index < ySize; index++)
        {
            if (index + lag < xSize)
            {
                x       = pX[index + lag];
                y       = conj(pY[index]);
                product = x * y;
                sum     = sum + product;
            }
        }
    }
    else
    {
        /* Do nothing */
    }
    return (sum);
}


int32_t dpd_Xcorr(const double complex *pTx,
                    const double complex *pORx,
                    double *pResult,
                    int32_t xSize,
                    int32_t ySize,
                    int32_t lagIndex,
                    int32_t lagRange)
{
/*
 *  xSize = number of Tx samples per capture (e.g., 4096).
 *  ySize = number of Orx samples per capture/or interpolated (e.g., 4096 or 4097).
 *
 *  lagIndex = correlation peak index from previous correlation
 *             result (if it is run first time, lagRange = 0 and lagIndex is discarded)
 *
 *  lagRange = +/- range from correlation peak index (if it is run first time,
 *             lagRange = 0. Full search from -(ySize -1) to xSize - 1)
 */

    int32_t        minLag, maxLag, lag;
    uint8_t        firstTime;
    double complex xCorrelation;
    double         correlationMag;
    double         maxCorrelation = 0.0;
    int32_t        optLag         = 0u;

    firstTime = (lagRange == 0) ? 1u:0u;

    if (firstTime)
    {
        minLag = -(ySize - 1);
        maxLag = xSize - 1;
    }
    else
    {
        minLag = (lagIndex - lagRange < -(ySize - 1))? -(ySize - 1) : lagIndex - lagRange;
        maxLag = (lagIndex + lagRange > xSize - 1)? xSize - 1 : lagIndex + lagRange;
    }

    MEX_LOG("minLag:%d maxLag:%d\n", minLag, maxLag);

    for (lag = minLag; lag <= maxLag; lag++)
    {
        xCorrelation   = dpd_XcorrOneLag(pTx, pORx, xSize, ySize, lag);
        correlationMag = cabs(xCorrelation);

        MEX_LOG_DETAILED("correlationMag:%f\n", correlationMag);

        if (maxCorrelation < correlationMag)
        {
            maxCorrelation = correlationMag;
            *pResult       = maxCorrelation;
            optLag         = lag;
        }

        MEX_LOG_DETAILED("\n\n");
        MEX_LOG_DETAILED("lag:%d correlationMag:%f maxCorrelation:%f optLag:%d\n",
                         lag, correlationMag, maxCorrelation, optLag);
    }

    return (optLag);
}

void dpd_FilterXCorr(const double complex *pTx,
                        const double complex *pORx,
                        int32_t xSize,
                        int32_t ySize,
                        int32_t optIntDelay,
                        int32_t intDelayRange,
                        int32_t fracDelay,
                        double complex *result)
{
    dpd_Filter_t fracDelayFilter;
    double          *pFracFilterCoef;

    double complex  inFracDelayFilterBuffer[DPD_FILTER_FRAC_LEN];
    double complex  outFracDelayFilterBuffer[DPD_FILTER_FRAC_LEN];

    double complex  fracDelayedORx;
    double complex  orxSample;

    int32_t         totalDelay;
    uint32_t        totalCount = 0;
    int32_t         range;


    pFracFilterCoef = &dpd_FilterFracCoefs[fracDelay][0];

    dpd_FirResetBuf(&fracDelayFilter,
                       pFracFilterCoef,
                       inFracDelayFilterBuffer,
                       outFracDelayFilterBuffer,
                       DPD_FILTER_FRAC_LEN,
                       DPD_FILTER_FRAC_LEN);

    for (range = -1 * intDelayRange; range < (intDelayRange + 1); range++)
    {
        result[range + intDelayRange] = DBL_CPX_ZERO;
    }


    for (int32_t index = 0; index < (ySize + DPD_FILTER_FRAC_INT_GROUP_DELAY); index++)
    {
        orxSample      = (index < ySize) ? pORx[index] : DBL_CPX_ZERO;
        uint8_t ENABLE = 1u;
        fracDelayedORx = dpd_Fir(&fracDelayFilter, orxSample, (uint32_t)index, ENABLE);

        {
            for (range = -1 * intDelayRange; range < (intDelayRange + 1); range++)
            {
                totalDelay = optIntDelay + range - DPD_FILTER_FRAC_INT_GROUP_DELAY;

                if ((index + totalDelay >= 0) && (index + totalDelay < xSize))
                {
                    result[range + intDelayRange] += pTx[index + totalDelay] * conj(fracDelayedORx);
                    totalCount++;
                }
            }
            MEX_LOG_DETAILED("totalCount:%d indx:%d \n", totalCount
                             , index);
        }
    }
    return;
}

dpd_ErrCode_e dpd_DelayEstimate(dpd_TrackData_t *pData,
                               const double complex *pTx,
                               const double complex *pORx)
{
    /* TODO: to add errCode for this function */
    dpd_ErrCode_e               err                 = DPD_ERR_CODE_NO_ERROR;
    dpd_PathDlyStatus_t         *pPathDlyStatus     = &(pData->pathDlyStatus);

    int32_t                         size        = pData->capCfg.capDepth;
    int32_t                         optIntDelay = 0;
    double                          Result;
    double                          yn1, y0, y1;
    uint32_t                        estIterStep   = (pData->pTrackCfg)->pdEstCtrl.pdEstIterStep;
    uint8_t                         xCorrLagRange = (pData->pTrackCfg)->pdEstCtrl.xCorrLagRange;
    int32_t                         intDelay      = pPathDlyStatus->intDelay;
    double complex                  filterXcorrResult[3];
    int32_t                         fracDelaySweepVar;
    double                          maxCorrVale = 0;

    if ((pPathDlyStatus->iterNum) % estIterStep == 0)
    {
        if (pData->pathDelayMode == DPD_STARTUP_MODE)
        {
            optIntDelay = dpd_Xcorr(pTx, pORx, &Result, size, size, 0, 0);
        }
        else
        {
            optIntDelay = dpd_Xcorr(pTx, pORx, &Result, size, size, intDelay, xCorrLagRange);
        }

        for (fracDelaySweepVar = 0; fracDelaySweepVar < DPD_FILTER_FRAC_DELAY_NUM; fracDelaySweepVar++)
        {
            dpd_FilterXCorr(pTx,
                            pORx,
                            size,
                            size,
                            optIntDelay,
                            1,
                            DPD_FILTER_FRAC_DELAY_NUM - 1 - fracDelaySweepVar,
                            filterXcorrResult);

            yn1 = cabs(filterXcorrResult[0]);
            y0  = cabs(filterXcorrResult[1]);
            y1  = cabs(filterXcorrResult[2]);

            if (maxCorrVale < yn1)
            {
                maxCorrVale                   = yn1;
                pPathDlyStatus->fracDelay = fracDelaySweepVar;
                pPathDlyStatus->intDelay  = optIntDelay - 1;
            }

            if (maxCorrVale < y0)
            {
                maxCorrVale                   = y0;
                pPathDlyStatus->fracDelay = fracDelaySweepVar;
                pPathDlyStatus->intDelay  = optIntDelay;
            }

            if (maxCorrVale < y1)
            {
                maxCorrVale                   = y1;
                pPathDlyStatus->fracDelay = fracDelaySweepVar;
                pPathDlyStatus->intDelay  = optIntDelay + 1;
            }
        }
    }
    pPathDlyStatus->iterNum++;
    return (err);
}

dpd_ErrCode_e dpd_SearchRefSignal(double complex *pTx,
                                double complex *pORx,
                                double complex *pRef,
                                uint16_t xSize,
                                uint16_t ySize)
{
    double result;
    dpd_ErrCode_e err = DPD_ERR_CODE_NO_ERROR;
    int32_t lagIndex = 0;
    int32_t lagRange = xSize;
    int32_t optIntDelay = dpd_Xcorr(pTx, pORx, &result, xSize, ySize, lagIndex, lagRange);

    MEX_LOG("optIntDelay:%d\n", optIntDelay);

    if(optIntDelay < 0)
    {
        err = DPD_SEARCH_REF_ERROR;
    }
    else
    {
        memcpy(pRef, &(pTx[optIntDelay]), sizeof(double complex)*ySize);
    }
    return err;
}