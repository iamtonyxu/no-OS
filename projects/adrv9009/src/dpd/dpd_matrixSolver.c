#include "dpd_matrixSolver.h"
#include <string.h>

#if 1
static dpd_AdpFeatureRow_t feature[DPD_MAX_NUM_FEATURES];
#endif

#if 0
static double c[(DPD_MAX_NUM_FEATURES+1) * (DPD_MAX_NUM_FEATURES+1)];

static void readDecompBuf(uint16_t features, const double complex *pCholDecompBuf)
{
    for(uint16_t i = 0u; i < features + 1; i++)
    {
        for(uint16_t j = i; j < features + 1; j++)
        {
            c[i * (features + 1) + j] = creal(pCholDecompBuf[i * (features + 1) + j]);
            if(i != j)
            {
                c[j * (features + 1) + i] = cimag(pCholDecompBuf[i * (features + 1) + j]);
            }
        }
    }

#ifdef MATLAB_BUILD
    MEX_LOG("\npCholDecompBuf:\n");
    for(uint16_t i = 0u; i < features + 1; i++)
    {
        for(uint16_t j = 0u; j < features + 1; j++)
        {
            MEX_LOG("%5.3f+%5.3fI\t", 
                    creal(pCholDecompBuf[i*(features+1)+j]), 
                    cimag(pCholDecompBuf[i*(features+1)+j]));
        }
        printf("\n");
    }
    MEX_LOG("\nMatrix-C:\n");
    for(uint16_t i = 0u; i < features + 1; i++)
    {
        for(uint16_t j = 0u; j < features + 1; j++)
        {
            MEX_LOG("%5.3f\t", c[i*(features+1)+j]);
        }
        printf("\n");
    }
#endif
    return;
}
#endif

dpd_ErrCode_e dpd_CholeskyDecomp(uint8_t features,
                                double rawRegValue,
                                uint8_t linearTermFeatureIndex,
                                const double c[],
                                double l[],
                                double complex a[])
{
    dpd_ErrCode_e                   err = DPD_ERR_CODE_NO_ERROR;
    int                             i, j, k;
    double complex                  sum        = DBL_CPX_ZERO;
    double                          regVal     = rawRegValue;

    /* correlation matrix has the size of features x features,
     * xy cross-corr stored at end of correlation matrix
     * the output matrix of XCORR is (features + 1) x (features + 1)*/
    int cDimension = features + 1;

    /* the decomposition matrix has the same size as correlation matrix */
    double (*pl)[features] = (double (*)[features])l;

    /*
     * Ax = b
     *
     * With Cholesky decomposition A matrix can be represented as L^H*L (L^H is the lower
     * triangular matrix and is equal to the hermitian transpose of upper triangular
     * matrix L.
     *
     * L can solved by the following equations
     *
     * If i == j,
     * L[j][j] = sqrt(A[j][j] - sum(L[j][k] * conj(L[j][k]))),
     * where k = 1 to j - 1
     *
     * If i != j
     * L[i][j] = 1 / L[j][j] * (A[i][j] - sum(L[i][k] * conj(L[j][k])))
     * for i > j
     *
     * A = L^H*L and L^H*Lx = b. Solve L^H*y = b for y by forward substitution and solve Lx = y
     * for x by back substitution.
     *
     */
    for (i = 0; (i < features) && (err == DPD_ERR_CODE_NO_ERROR); i++)
    {
        for (j = 0; (j <= i) && (err == DPD_ERR_CODE_NO_ERROR); j++)
        {
            sum = (double)c[j * cDimension + i];
            if (i == j)
            {
#if 0 // Do NOT skip linear term for regValue
                if (i != linearTermFeatureIndex)
#endif
                {
                    sum += (double)regVal;  /* add regularization term to all features  (except linear term) */
                }

                for (k = 0; k < j; k++)
                {
                    sum -= pl[k][i] * pl[k][j] + pl[i][k] * pl[j][k];
                }

                if (creal(sum) < 0.0)
                {
                    err = DPD_CHOL_SOLVER_ERROR;
                    MEX_LOG("DPD_CHOL_SOLVER_ERROR: creal(sum) = %5.3f\n", creal(sum));
                }
                else
                {
                    pl[i][i] = sqrt(creal(sum));
                }
            }
            else
            {
                sum += (double)c[i * cDimension + j] * I;
                for (k = 0; k < j; k++)
                {
                    sum -= pl[k][i] * pl[k][j] + pl[i][k] * pl[j][k]
                           + (pl[i][k] * pl[k][j] - pl[k][i] * pl[j][k]) * I;
                }

                double temp = 1.0 / pl[j][j];
                pl[j][i] = temp * creal(sum);
                pl[i][j] = temp * cimag(sum);
            }
        }
    }

    if (err == DPD_ERR_CODE_NO_ERROR)
    {
        /* Now use forward substituion to solve L^H*y = b for y, storing y in a */
        for (i = 0; i < features; i++)
        {
            sum = (double)c[i * cDimension + features] + (double)c[features * cDimension + i] * I;

            for (k = i - 1; k >= 0; k--)
            {
                sum -= pl[k][i] * creal(a[k]) + pl[i][k] * cimag(a[k])
                       + (-pl[i][k] * creal(a[k]) + pl[k][i] * cimag(a[k])) * I;
            }

            a[i] = sum / pl[i][i];
        }

        /* And solve Lx = y for x by back substitution, storing x in feature[].a */
        for (i = features - 1; i >= 0; i--)
        {
            sum = a[i];

            for (k = i + 1; k < features; k++)
            {
                sum -= pl[i][k] * creal(a[k]) - pl[k][i] * cimag(a[k])
                       + (pl[k][i] * creal(a[k]) + pl[i][k] * cimag(a[k])) * I;
            }

            a[i] = sum / pl[i][i];
        }
    }

    return err;
}

void dpd_updateFeatureCoeffs(dpd_TrackData_t *pData, double complex *a)
{
    uint8_t             features   = pData->pDpdModelDesc->features;
    double              mu = pData->pTrackCfg->mu / 100.0;
#if 0
    dpd_AdpFeatureRow_t *feature   = pData->pDpdModelDesc->feature;
#endif
    double complex      *bias = pData->biasOfCoeffs;

    /* copy to feature structure */
    for (uint8_t i = 0u; i < features; i++)
    {
        if (pData->direct)
        {
#if 0
            feature[i].a -= mu * a[i] - bias[i];
            bias[i] = pData->biasBeta * a[i] + pData->biasAlpha * bias[i];
#else
            feature[i].a -= mu * a[i];
#endif
        }
        else
        {
            feature[i].a = a[i];
        }

        pData->pDpdModelDesc->feature[i].a = feature[i].a;
    }

    return;
}