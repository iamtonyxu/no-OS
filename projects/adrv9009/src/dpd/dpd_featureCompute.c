#include "dpd_featureCompute.h"

static double complex   xPower[DPD_SAMPLE_DELAY_I_BUF_SIZE][DPD_MAX_K_POW];
static double complex   xVal[DPD_SAMPLE_DELAY_J_BUF_SIZE];
static double complex   xConj[DPD_SAMPLE_DELAY_J_BUF_SIZE];
static double complex   xSquared[DPD_SAMPLE_DELAY_J_BUF_SIZE];
static double           xAbsSquared[DPD_SAMPLE_DELAY_J_BUF_SIZE];


/*
* \brief       Convert a double input into the fixed-point format
*/
static int32_t fToFixedPt(double f, uint8_t n);

uint32_t dpd_FeatCompute(const dpd_TrackData_t *pData,
                    uint32_t xIdxStart,
                    uint32_t xIdxEnd,
                    uint8_t discardSamplesEn,
                    double complex *pFfInputBuf)
{
    uint32_t                    xIdx, fIdx;
    double complex              feature = DBL_CPX_ZERO;
    uint32_t                    p;
    uint32_t                    sampleCount = 0u;
    double complex              xNormalized, dSigNormalized;
    uint8_t                     i, j, k;
    dpd_ActPolyType_e           poly;
    dpd_ModelDesc_t             *pDpdModelDesc = pData->pDpdModelDesc;
    uint32_t                    offset     = 0ul;
    uint32_t                    featureSampleComputed = 0ul;

    for (xIdx = xIdxStart; xIdx < xIdxEnd; xIdx++)
    {
        sampleCount++;
        xNormalized = pData->pTx[xIdx];

        /* Fill circular buffers */
        for (p = 0; p < DPD_MAX_K_POW; p++)
        {
            xPower[xIdx % DPD_SAMPLE_DELAY_I_BUF_SIZE][p] = dpd_UtilShiftedLegendre(cabs(xNormalized), p);
        }

        xVal[xIdx % DPD_SAMPLE_DELAY_J_BUF_SIZE]        = xNormalized;
        xConj[xIdx % DPD_SAMPLE_DELAY_J_BUF_SIZE]       = conj(xNormalized);
        xSquared[xIdx % DPD_SAMPLE_DELAY_J_BUF_SIZE]    = xNormalized * xNormalized;
        xAbsSquared[xIdx % DPD_SAMPLE_DELAY_J_BUF_SIZE] = cabs(xNormalized * xNormalized);
#if 0
        if (discardSamplesEn && ((sampleCount < DPD_SAMPLE_DELAY_I_BUF_SIZE) ||
                                 (sampleCount < DPD_SAMPLE_DELAY_J_BUF_SIZE)))
#else
        if (discardSamplesEn && (sampleCount < DPD_SAMPLE_DELAY_I_BUF_SIZE + 16)) // DEBUG ONLY, best performance with 32 ENDSKIP
#endif
        {
            continue;

            /*
             * we need to discard the initial set of samples as the circular buffers are
             * not fully populated yet and delayed samples are not available.
             *
             */
        }

        offset         = featureSampleComputed * ((uint32_t)pDpdModelDesc->features + 1u);
        dSigNormalized = pData->pORx[xIdx - pData->pTrackCfg->linearTerm] / (double)(DPD_SAMPLE_NORMALIZATION);
        //MEX_LOG("pData->pTrackCfg->linearTerm = %d\n", pData->pTrackCfg->linearTerm);

        for (fIdx = 0; fIdx < pDpdModelDesc->features; fIdx++)
        {
            i    = (uint8_t)(pDpdModelDesc->feature[fIdx].i);
            j    = (uint8_t)(pDpdModelDesc->feature[fIdx].j);
            k    = pDpdModelDesc->feature[fIdx].k;
            poly = pDpdModelDesc->feature[fIdx].poly;

            uint32_t iModPwr    = (xIdx - i) % DPD_SAMPLE_DELAY_I_BUF_SIZE;
            uint32_t jMod       = (xIdx - j) % DPD_SAMPLE_DELAY_J_BUF_SIZE;
            uint32_t iModDdrPwr = xIdx % DPD_SAMPLE_DELAY_I_BUF_SIZE;
            uint32_t iModDdr    = xIdx % DPD_SAMPLE_DELAY_J_BUF_SIZE;
            uint32_t jModDdr    = (xIdx - j) % DPD_SAMPLE_DELAY_J_BUF_SIZE;

            switch (poly)
            {
                case DPD_ACT_POLY_GMP0:
                case DPD_ACT_POLY_GMP1:
                {
                    feature = xPower[iModPwr][k] * xVal[jMod];
                    break;
                }

                case DPD_ACT_POLY_DDR7:
                {
                    feature = xPower[iModDdrPwr][k] * xSquared[iModDdr] * xConj[jModDdr];
                    break;
                }

                case DPD_ACT_POLY_DDR8:
                {
                    feature = xPower[iModDdrPwr][k] * xVal[iModDdr] * xAbsSquared[jModDdr];
                    break;
                }

                case DPD_ACT_POLY_DDR9:
                {
                    feature = xPower[iModDdrPwr][k] * xConj[iModDdr] * xSquared[jModDdr];
                    break;
                }
            }
            /* Normalize the feature (not needed and can be removed) */
            /* feature = feature / DPD_FEATURE_NORMALIZATION; */
            pFfInputBuf[offset + fIdx] = feature;
        }
        pFfInputBuf[offset + fIdx] = dSigNormalized;
        featureSampleComputed += 1;

#if 0
        FILE *fptr;
        fptr = fopen("c:\\tmp\\featureMatrix.txt","a");
        if(fptr == NULL)
        {
            printf("Error!");
            MEX_LOG("Error: failed to create txt file to save featureMatrix!\n");
        }
        else
        {
            fprintf(fptr, "featureSampleComputed : %d\n", featureSampleComputed);
            for (fIdx = 0; fIdx < pDpdModelDesc->features; fIdx++)
            {
                fprintf(fptr, "%8.3f + i%8.3f\n", creal(pFfInputBuf[offset+fIdx]), cimag(pFfInputBuf[offset+fIdx]));
            }
        }
        fclose(fptr);
#endif
    }

    return featureSampleComputed;
}

static int32_t fToFixedPt(double f, uint8_t n)
{
    /* assume f is in range [-1.0, +1.0) */
    return ((int32_t)lround(scalbn(f, n)));
}

#if 0
void dpd_QUMatrixCompute(uint16_t featureSampleComputed,
                        uint16_t features,
                        const double complex *pFfInputBuf,
                        double complex *pCholeskyDecompBuf)
{
    uint16_t    xIdx, fRow, fCol;
    uint16_t    offset1 = 0u; // offset of pFfInputBuf
    uint16_t    offset2 = 0u; // offset of pCholeskyDecompBuf

#if 0   //#ifdef MATLAB_BUILD
    //check input parameters
    MEX_LOG("featureSampleComputed=%d, features=%d\n", featureSampleComputed,features);
    // check pFfInputBuf
    for(uint32_t i = 0u; i < (featureSampleComputed) * (features + 1); i++)
    {
        MEX_LOG("pFfInputBuf[%d] = %5.3f + %5.3fI\n", i, creal(pFfInputBuf[i]), cimag(pFfInputBuf[i]));
    }
#endif

    //Note: Q is a hermitian Matrix and the calculation below could be optimized.
    for(xIdx = 0u; xIdx < featureSampleComputed; xIdx++)
    {
        offset1 = xIdx * (features + 1u);
        for(fRow = 0u; fRow < features; fRow++)
        {
            offset2 = fRow * (features + 1u);
            for(fCol = 0u; fCol < features; fCol++)
            {
                pCholeskyDecompBuf[offset2 + fCol] += conj(pFfInputBuf[offset1 + fRow]) * pFfInputBuf[offset1 + fCol];
            }
            pCholeskyDecompBuf[offset2 + fCol] += conj(pFfInputBuf[offset1 + fRow]) * pFfInputBuf[offset1 + fCol];
        }
    }

    return;
}
#endif

void dpd_QUMatrixCompute(uint16_t featureSampleComputed,
                        uint16_t features,
                        const double complex *pFfInputBuf,
                        double *pCholeskyDecompBuf)
{
    uint16_t    xIdx, fRow, fCol;
    uint16_t    offset1; // offset of pFfInputBuf
    uint16_t    offset2, offset3; // offset of pCholeskyDecompBuf
    double complex tmp = DBL_CPX_ZERO;

    //Note: Q is a hermitian Matrix and the calculation below could be optimized.
    for(xIdx = 0u; xIdx < featureSampleComputed; xIdx++)
    {
        offset1 = xIdx * (features + 1u);
        for(fRow = 0u; fRow < features; fRow++)
        {
            offset2 = fRow * (features + 1u);
            for(fCol = fRow; fCol < features; fCol++)
            {
                offset3 = fCol * (features + 1u);
                tmp = conj(pFfInputBuf[offset1 + fRow]) * pFfInputBuf[offset1 + fCol];
                pCholeskyDecompBuf[offset3 + fRow] += cimag(tmp);
                pCholeskyDecompBuf[offset2 + fCol] += creal(tmp);
            }
                offset3 = fCol * (features + 1u);
                tmp = conj(pFfInputBuf[offset1 + fRow]) * pFfInputBuf[offset1 + fCol];
                pCholeskyDecompBuf[offset3 + fRow] += cimag(tmp);
                pCholeskyDecompBuf[offset2 + fCol] += creal(tmp);
        }
    }
    return;
}

void dpd_QUMatrixDamping(double *pCholeskyDecompBuf, double damping)
{
    for(int i = 0; i < DPD_MAX_NUM_FEATURES + 1; i++)
    {
        for(int j = 0; j < DPD_MAX_NUM_FEATURES + 1; j++)
        {
            int index = i*(DPD_MAX_NUM_FEATURES+1)+j;
            pCholeskyDecompBuf[index] *= damping;
        }
    }
}
