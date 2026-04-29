#include "dpd_top.h"
#include "dpd_adp.h"
#include "dpd_featureCompute.h"
#include "dpd_matrixSolver.h"
#include "dpd_delayEstimate.h"

static int dpdIteration;

static double complex *pFfInputBuf = NULL;
static double *pCholeskyDecompBuf = NULL;
static double *l = NULL;

static double complex a[DPD_MAX_NUM_FEATURES];
static uint32_t bankPingBuf[DPD_ACT_DEPTH_MAX * DPD_ACT_NUM_LUTS];
static uint32_t bankPongBuf[DPD_ACT_DEPTH_MAX * DPD_ACT_NUM_LUTS];

static dpd_TrackCfg_t defaultDpdTrackCfg =
{
    .numFilterCoefficients  = 64u,
    .numFilterOutput        = 64u,
    {
        .pdEstIterStep      = 1u,
        .pdEstPerIter       = 0u,
        .xCorrLagRange      = 128u,
        .intDelayRangeLoop  = 3u,
    },
    .filterSel              = 0u,
    .txPwrSelect            = 0u,
    .delta                  = 0u,
    .decayP                 = 0u,
    .mThreshold             = 0u,
    .indirectErrSamples     = 256u,
    .satThresh              = 40000u,
    .outlierThresh          = 32768u,
    .satXCntLimit           = 1u,
    .satYCntLimit           = 3u,
    .errCntLimit            = 2u,
    .txMinAvgSignalLevel    = 512u,
    .orxMinAvgSignalLevel   = 512u,
    .direct                 = 1u,
    .indirectRegValue       = 1e-2,
    .directRegValue         = 1e-2,
    .mu                     = 50u,
    .magGain                = 0x40u,
    .absScale               = 4u,
    .absOffset              = 0.5f,
#if 0
    .linearTerm             = 5u,
#else
    .linearTerm             = 0u,
#endif
};

#if 0
static dpd_ModelDesc_t defaultDpdModelDesc =
{
    .features           =       101,
    {
        .partial        =       DPD_TYPE_DPD,
        .updateOrder    =       0,
        .modelIndex     =       0,
    },
    .mode               =       DPD_ACT_GMP_DDR_MODE0,
    .actDepth           =       DPD_ACT_DEPTH_16,
    .lutIDelay          =       5,
    .lutJDelay          =       5,
    {
        { .i = 5u, .j = 4u, .k = 0u, .poly = DPD_ACT_POLY_GMP0, 1.0 + 0.0 * I, },
        { .i = 5u, .j = 4u, .k = 1u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 5u, .j = 4u, .k = 2u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 5u, .j = 4u, .k = 3u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 5u, .j = 4u, .k = 4u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 5u, .j = 4u, .k = 5u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 5u, .j = 4u, .k = 6u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 5u, .j = 4u, .k = 7u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 5u, .j = 4u, .k = 8u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 5u, .j = 5u, .k = 0u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 5u, .j = 5u, .k = 1u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 5u, .j = 5u, .k = 2u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 5u, .j = 5u, .k = 3u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 5u, .j = 5u, .k = 4u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 5u, .j = 5u, .k = 5u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 5u, .j = 5u, .k = 6u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 5u, .j = 5u, .k = 7u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 5u, .j = 5u, .k = 8u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 5u, .j = 5u, .k = 9u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 5u, .j = 5u, .k = 10u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 5u, .j = 5u, .k = 11u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 5u, .j = 5u, .k = 12u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 6u, .j = 5u, .k = 1u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 6u, .j = 5u, .k = 3u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 6u, .j = 5u, .k = 5u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 6u, .j = 5u, .k = 10u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 7u, .j = 5u, .k = 4u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 7u, .j = 5u, .k = 6u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 8u, .j = 5u, .k = 3u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 8u, .j = 5u, .k = 11u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 9u, .j = 5u, .k = 1u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 9u, .j = 5u, .k = 2u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 9u, .j = 5u, .k = 7u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 10u,.j = 5u, .k = 2u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 10u,.j = 5u, .k = 3u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 10u,.j = 5u, .k = 4u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 10u,.j = 5u, .k = 5u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 6u, .j = 6u, .k = 5u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 6u, .j = 6u, .k = 7u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 7u, .j = 6u, .k = 0u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 7u, .j = 6u, .k = 2u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 7u, .j = 6u, .k = 6u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 7u, .j = 6u, .k = 7u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 7u, .j = 6u, .k = 11u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 8u, .j = 6u, .k = 1u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 8u, .j = 6u, .k = 5u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 8u, .j = 6u, .k = 8u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 6u, .j = 7u, .k = 5u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 7u, .j = 7u, .k = 2u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 7u, .j = 7u, .k = 4u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 7u, .j = 7u, .k = 8u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 8u, .j = 7u, .k = 0u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 8u, .j = 7u, .k = 2u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 8u, .j = 7u, .k = 3u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 8u, .j = 7u, .k = 5u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 8u, .j = 7u, .k = 6u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 8u, .j = 7u, .k = 7u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 8u, .j = 7u, .k = 8u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 9u, .j = 7u, .k = 1u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 9u, .j = 7u, .k = 4u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 9u, .j = 7u, .k = 5u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 9u, .j = 7u, .k = 7u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 9u, .j = 7u, .k = 8u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 9u, .j = 7u, .k = 9u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 9u, .j = 7u, .k = 11u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 6u, .j = 8u, .k = 1u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 8u, .j = 8u, .k = 2u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 8u, .j = 8u, .k = 6u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 8u, .j = 8u, .k = 9u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 8u, .j = 8u, .k = 11u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 9u, .j = 8u, .k = 0u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 9u, .j = 8u, .k = 2u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 9u, .j = 8u, .k = 3u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 9u, .j = 8u, .k = 6u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 9u, .j = 8u, .k = 9u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 9u, .j = 8u, .k = 10u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 10u,.j = 8u, .k = 3u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 10u,.j = 8u, .k = 5u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 10u,.j = 8u, .k = 7u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 10u,.j = 8u, .k = 9u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 6u, .j = 9u, .k = 3u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 6u, .j = 9u, .k = 10u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 6u, .j = 9u, .k = 11u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 9u, .j = 9u, .k = 1u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 9u, .j = 9u, .k = 2u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 9u, .j = 9u, .k = 3u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 9u, .j = 9u, .k = 4u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 9u, .j = 9u, .k = 11u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 10u,.j = 9u, .k = 0u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 10u,.j = 9u, .k = 2u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 10u,.j = 9u, .k = 6u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 10u,.j = 9u, .k = 10u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 11u,.j = 9u, .k = 4u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 11u,.j = 9u, .k = 6u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 11u,.j = 9u, .k = 7u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 11u,.j = 9u, .k =  11u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 10u,.j = 10u, .k =  5u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 10u,.j = 10u, .k =  6u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 10u,.j = 10u, .k =  9u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 11u,.j = 10u, .k =  0u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 11u,.j = 10u, .k =  9u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
    },
};
#else
static dpd_ModelDesc_t defaultDpdModelDesc =
{
    .features           =       95,
    {
        .partial        =       DPD_TYPE_DPD,
        .updateOrder    =       0,
        .modelIndex     =       0,
    },
    .mode               =       DPD_ACT_GMP_DDR_MODE0,
    .actDepth           =       DPD_ACT_DEPTH_16,
    .lutIDelay          =       5,
    .lutJDelay          =       5,
    {
        { .i = 0u, .j = 0u, .k = 0u, .poly = DPD_ACT_POLY_GMP0, 1.0 + 0.0 * I, },
        { .i = 0u, .j = 0u, .k = 1u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 0u, .j = 0u, .k = 2u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 0u, .j = 0u, .k = 3u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 0u, .j = 0u, .k = 4u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 1u, .j = 0u, .k = 1u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 1u, .j = 0u, .k = 2u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 1u, .j = 0u, .k = 3u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 1u, .j = 0u, .k = 5u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 1u, .j = 0u, .k = 6u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 1u, .j = 0u, .k = 7u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 1u, .j = 0u, .k = 8u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 2u, .j = 0u, .k = 1u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 2u, .j = 0u, .k = 3u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 2u, .j = 0u, .k = 4u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 2u, .j = 0u, .k = 5u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 2u, .j = 0u, .k = 6u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 2u, .j = 0u, .k = 7u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 2u, .j = 0u, .k = 8u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 3u, .j = 0u, .k = 1u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 3u, .j = 0u, .k = 2u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 3u, .j = 0u, .k = 3u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 3u, .j = 0u, .k = 4u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 3u, .j = 0u, .k = 6u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 3u, .j = 0u, .k = 7u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 0u, .j = 1u, .k = 2u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 0u, .j = 1u, .k = 4u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 0u, .j = 1u, .k = 5u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 0u, .j = 1u, .k = 6u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 0u, .j = 1u, .k = 8u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 1u, .j = 1u, .k = 0u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 1u, .j = 1u, .k = 1u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 1u, .j = 1u, .k = 2u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 1u, .j = 1u, .k = 3u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 1u, .j = 1u, .k = 4u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 1u, .j = 1u, .k = 5u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 1u, .j = 1u, .k = 6u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 2u, .j = 1u, .k = 1u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 2u, .j = 1u, .k = 2u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 2u, .j = 1u, .k = 4u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 2u, .j = 1u, .k = 5u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 2u, .j = 1u, .k = 6u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 2u, .j = 1u, .k = 8u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 3u, .j = 1u, .k = 1u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 3u, .j = 1u, .k = 2u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 3u, .j = 1u, .k = 3u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 3u, .j = 1u, .k = 4u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 3u, .j = 1u, .k = 5u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 3u, .j = 1u, .k = 6u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 3u, .j = 1u, .k = 7u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 3u, .j = 1u, .k = 8u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 0u, .j = 2u, .k = 1u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 0u, .j = 2u, .k = 2u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 0u, .j = 2u, .k = 3u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 0u, .j = 2u, .k = 4u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 0u, .j = 2u, .k = 6u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 0u, .j = 2u, .k = 7u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 1u, .j = 2u, .k = 1u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 1u, .j = 2u, .k = 2u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 1u, .j = 2u, .k = 3u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 1u, .j = 2u, .k = 5u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 1u, .j = 2u, .k = 7u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 1u, .j = 2u, .k = 8u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 2u, .j = 2u, .k = 0u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 2u, .j = 2u, .k = 1u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 2u, .j = 2u, .k = 2u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 2u, .j = 2u, .k = 3u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 2u, .j = 2u, .k = 4u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 3u, .j = 2u, .k = 4u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 3u, .j = 2u, .k = 5u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 3u, .j = 2u, .k = 6u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 3u, .j = 2u, .k = 7u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 3u, .j = 2u, .k = 8u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 0u, .j = 3u, .k = 3u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 0u, .j = 3u, .k = 5u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 0u, .j = 3u, .k = 6u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 0u, .j = 3u, .k = 7u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 0u, .j = 3u, .k = 8u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 1u, .j = 3u, .k = 1u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 1u, .j = 3u, .k = 3u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 1u, .j = 3u, .k = 5u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 1u, .j = 3u, .k = 6u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 1u, .j = 3u, .k = 7u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 1u, .j = 3u, .k = 8u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 2u, .j = 3u, .k = 1u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 2u, .j = 3u, .k = 3u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 2u, .j = 3u, .k = 4u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 2u, .j = 3u, .k = 5u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 2u, .j = 3u, .k = 7u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 2u, .j = 3u, .k = 8u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 3u, .j = 3u, .k = 0u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 3u, .j = 3u, .k = 1u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 3u, .j = 3u, .k = 2u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 3u, .j = 3u, .k = 3u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 3u, .j = 3u, .k = 4u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 0u, .j = 0u, .k = 0u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 0u, .j = 0u, .k = 0u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 0u, .j = 0u, .k = 0u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 0u, .j = 0u, .k = 0u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 0u, .j = 0u, .k = 0u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 0u, .j = 0u, .k = 0u, .poly = DPD_ACT_POLY_GMP0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
        { .i = 0u,.j = 0u, .k =  0u, .poly =                 0, 0.0 + 0.0 * I, },
    },
};
#endif

static dpd_TrackCfg_t dpdTrackCfg = {0};
static dpd_ModelDesc_t dpdModelDesc = {0};
static dpd_ActModelCfg_t  dpdActConfig = {0};
static dpd_LutFeature_t dpdLut = { 0 };
static dpd_ModelStateInfo_t defaultDpdModelStateInfo = { 0 };

static dpd_ErrCode_e dpd_InitModelStateInfo(dpd_TrackData_t *pData);

static dpd_ErrCode_e dpd_FindLutId_GMPMode(int8_t *pId,
                                            int8_t i,
                                            int8_t j,
                                            uint8_t poly);

dpd_ErrCode_e dpd_Init(dpd_TrackData_t *pData)
{
    dpd_ErrCode_e err = DPD_ERR_CODE_NO_ERROR;
    uint8_t modelIndex = 0u; // Only one model is supported currently

    memset(pData, 0, sizeof(dpd_TrackData_t));
    pData->pTrackCfg = &defaultDpdTrackCfg;
    pData->pDpdModelDesc = &defaultDpdModelDesc;

    pData->alignedSampleCount = 0u;
    pData->pathDelayMode = DPD_STARTUP_MODE;

    pData->biasAlpha = 1.0;
    pData->biasBeta = 0;

    pData->pActModelCfg = &dpdActConfig;
    pData->pLut = &dpdLut;

    err = dpd_ModelConfig(pData, modelIndex);
    if(err == DPD_ERR_CODE_NO_ERROR)
    {
        dpd_InitModelStateInfo(pData);
    }

    return err;
}

dpd_ErrCode_e dpd_Finish(void)
{
    dpd_ErrCode_e err = DPD_ERR_CODE_NO_ERROR;

#ifdef MATLAB_BUILD
    /* free memory */
    free(pCholeskyDecompBuf);
    free(pFfInputBuf);
    free(l);
#endif

    return err;
}

/** Limitation of dpd coefficient estimation including:
 * 1. No feature filter applied before feature computation
 * 2. Need memory usage optimization
 *      -   pData->pTx, size = DPD_MAX_NUM_CAP_SAMPLES * DPD_MAX_NUM_CAP_BATCHES * sizeof(double complex)
 *      -   pData->pORx, size = DPD_MAX_NUM_CAP_SAMPLES * DPD_MAX_NUM_CAP_BATCHES * sizeof(double complex)
 *      -   pFfInputBuf, size = DPD_MAX_NUM_FF_TAPS *(DPD_MAX_NUM_FEATURES + 1) * sizeof(double complex)
 *      -   pCholeskyDecompBuf, size = (DPD_MAX_NUM_FEATURES + 1) * (DPD_MAX_NUM_FEATURES + 1) * sizeof(double)
 * 3. No model check (stability check) after coeff estimation
 * 4. Only one DPD model is supported.
*/
dpd_ErrCode_e dpd_CoeffEstimate(dpd_TrackData_t *pData,
                                double complex *pTx,
                                double complex *pORx,
                                uint16_t capSize,
                                uint8_t  capBatch)
{
    dpd_ErrCode_e err = DPD_ERR_CODE_NO_ERROR;

    pData->capCfg.capBatch = capBatch;
    pData->capCfg.capDepth = capSize;

    uint32_t featureSampleComputed = 0u;
    uint8_t discardSamplesEn = DPD_TRUE;
    uint8_t linearTermFeatureIndex = pData->pLut->linearTermFeatureIndex;
    uint8_t features = pData->pDpdModelDesc->features;
    uint32_t xIdxStart = 0u;
    uint32_t xIdxEnd = 0u;
    uint8_t captureIndex = 0u;

    pData->procState = PROCSTATE_NULL;
    pData->alignedSampleCount = 0u;
    pData->featureSampleCount = 0u;

    /* apply memory for dpd model training */
    pFfInputBuf = (double complex *)malloc(sizeof(double complex)*(DPD_MAX_NUM_FF_TAPS * (DPD_MAX_NUM_FEATURES + 1)));
    pCholeskyDecompBuf = (double *)malloc(sizeof(double) * ((DPD_MAX_NUM_FEATURES + 1) * (DPD_MAX_NUM_FEATURES + 1)));
    l = (double *)malloc(sizeof(double) * (DPD_MAX_NUM_FEATURES + 1) * (DPD_MAX_NUM_FEATURES + 1));

    if((!pFfInputBuf) ||(!pCholeskyDecompBuf) || (!l))
    {
#ifdef MATLAB_BUILD
        MEX_LOG("Failed to apply memory for feature compute.\n");
#endif
        err = DPD_MEMORY_APPLY_ERROR;
    }
    else
    {
        memset(pFfInputBuf, 0, sizeof(double complex)*(DPD_MAX_NUM_FF_TAPS * (DPD_MAX_NUM_FEATURES + 1)));
        memset(pCholeskyDecompBuf, 0, sizeof(double) * ((DPD_MAX_NUM_FEATURES + 1) * (DPD_MAX_NUM_FEATURES + 1)));
        memset(l, 0, sizeof(double) * (DPD_MAX_NUM_FEATURES + 1) * (DPD_MAX_NUM_FEATURES + 1));

        pData->pCholeskyDecompBuf = pCholeskyDecompBuf;
        pData->pFfInputBuf = pFfInputBuf;
        pData->l = l;
    }

#if 0
    FILE *fptr;
    fptr = fopen("c:\\tmp\\choleskydecompbuf.txt","a");
    if(fptr == NULL)
    {
        printf("Error!");
        MEX_LOG("Error: failed to create txt file to save data!\n");
    }
    else
    {
        fprintf(fptr, "\nIteration : %d\n", dpdIteration++);
        for(int i = 0; i < (pData->pDpdModelDesc->features + 1) * (pData->pDpdModelDesc->features + 1); i++)
        {
            fprintf(fptr, "%8.3f\n", pCholeskyDecompBuf[i]);
        }
    }
    fclose(fptr);
#endif

    MEX_LOG("pTrackCfg->direct=%d, pData->iterCount=%d\n", pData->pTrackCfg->direct, pData->iterCount);
    if((pData->pTrackCfg->direct == 1) && (pData->iterCount > DPD_MAX_INDIRECT_COUNT - 1))
    {
        pData->direct = 1;
    }
    else
    {
        pData->direct = 0;
    }

    pData->regValue = pData->direct ? pData->pTrackCfg->directRegValue : pData->pTrackCfg->indirectRegValue;

    /* Sanity check on data pointer */
    if((pTx == NULL) || (pORx == NULL))
    {
        err = DPD_SAMPLE_POINTER_NULL_ERROR;
    }
    pData->pTx = pTx;
    pData->pORx = pORx;

    if(err == DPD_ERR_CODE_NO_ERROR)
    {
        /* Perform delay estimation */
        /* Only the first capBatch samples are used to estimate path delay */
        MEX_LOG("PROCSTATE_DELAYEST\n");
        pData->procState = PROCSTATE_DELAYEST;
        err = dpd_DelayEstimate(pData, pTx, pORx);
    }

    for(captureIndex = 0u; (captureIndex < capBatch) && (err == DPD_ERR_CODE_NO_ERROR); captureIndex++)
    {
        uint16_t offset = (uint16_t)(captureIndex * capSize);
        /* Perform sample alignment */
        MEX_LOG("PROCSTATE_ALIGNMENT\n");
        pData->procState = PROCSTATE_ALIGNMENT;
        err = dpd_SampleProc(pData, &(pData->pTx[offset]), &(pData->pORx[offset]), captureIndex);
    }

    /* Perform feature computation */
    MEX_LOG("PROCSTATE_FEATCOMP\n");
    pData->procState = PROCSTATE_FEATCOMP;
    captureIndex = 0u;
    pData->featureSampleCount = 0u;
    for(xIdxStart = 0u; (xIdxStart < pData->alignedSampleCount) && (err == DPD_ERR_CODE_NO_ERROR); xIdxStart += DPD_MAX_NUM_FF_TAPS)
    {
        featureSampleComputed = 0u;
        discardSamplesEn = 0u;
        memset(pFfInputBuf, 0, sizeof(pFfInputBuf));

        xIdxEnd = xIdxStart + DPD_MAX_NUM_FF_TAPS;
#if 1
        if(xIdxEnd > pData->alignedSampleCount)
        {
            xIdxEnd = pData->alignedSampleCount - 1;
        }
#else
        if(xIdxEnd > pData->alignedSampleCount - 31)
        {
            xIdxEnd = pData->alignedSampleCount - 1 - 31;
        }
#endif
        //TODO: this condition cannot be triggered since xIdxStart += 64
        if(xIdxStart == pData->firstSampleIndex[captureIndex])
        {
            captureIndex++;
            captureIndex %= DPD_MAX_NUM_CAP_BATCHES;
            discardSamplesEn = 1u;
        }

        featureSampleComputed = dpd_FeatCompute(pData,
                                                xIdxStart,
                                                xIdxEnd,
                                                discardSamplesEn,
                                                pFfInputBuf);
        pData->featureSampleCount += featureSampleComputed;

        /* Perform QUMatrix computation */
        dpd_QUMatrixCompute(featureSampleComputed,
                            features,
                            pFfInputBuf,
                            pCholeskyDecompBuf);

#if 1
        /* TODO: Run test to verify QUMatrix damping */
        if((captureIndex < capBatch-1) &&
            (pData->featureSampleCount > pData->lastSampleIndex[captureIndex] - DPD_MAX_NUM_FF_TAPS))
        {
            MEX_LOG("damping...featureSampleCount=%d, captureIndex=%d\n", pData->featureSampleCount, captureIndex);
            /* QUMatrix damping */
            double damping = (1 - 1.0/4096);
            dpd_QUMatrixDamping(pCholeskyDecompBuf, damping);
        }
#endif
    }

    if(err == DPD_ERR_CODE_NO_ERROR)
    {
        /* Perform cholesky decompostion */
        MEX_LOG("PROCSTATE_MATRIXSOLVER\n");
        pData->procState = PROCSTATE_MATRIXSOLVER;

        err = dpd_CholeskyDecomp(features,
                                pData->regValue,
                                pData->pTrackCfg->linearTerm,
                                pCholeskyDecompBuf,
                                l,
                                a);
    }

    if(err == DPD_ERR_CODE_NO_ERROR)
    {
        dpd_updateFeatureCoeffs(pData, a);
    }

    if(err == DPD_ERR_CODE_NO_ERROR)
    {
        /* Perform Model Check */
        MEX_LOG("PROCSTATE_MODELCHECK\n");
        pData->procState = PROCSTATE_MODELCHECK;
        // TODO, dpd_ModelCheck(dpd_TrackData_t *pData)
        pData->iterCount++;
    }

#ifndef MATLAB_BUILD
    /* free memory */
    free(pCholeskyDecompBuf);
    free(pFfInputBuf);
    free(l);
#endif

    return err;
}

dpd_ErrCode_e dpd_TrackConfig(dpd_TrackData_t *pData, dpd_TrackCfg_t *pNewTrackCfg)
{
    dpd_ErrCode_e err = DPD_ERR_CODE_NO_ERROR;
    dpd_TrackCfg_t *pTrackCfg = pData->pTrackCfg;

    // TODO: Sanity check on new DPD track config

    memcpy(pTrackCfg, pNewTrackCfg, sizeof(dpd_TrackCfg_t));

    return err;
}

dpd_ErrCode_e dpd_ModelConfig(dpd_TrackData_t *pData, uint8_t modelIndex)
{
    dpd_ErrCode_e err = DPD_ERR_CODE_NO_ERROR;
    dpd_ModelDesc_t *pModelDesc = pData->pDpdModelDesc;

    if(modelIndex > DPD_ACT_MODEL_MAX-1)
    {
#ifdef MATLAB_BUILD
        MEX_LOG("modelIndex = %d\n", modelIndex);
#endif
        err = DPD_MODEL_INVALID_ERROR;
    }
    if(err == DPD_ERR_CODE_NO_ERROR)
    {
        /* TODO: update model configuration */
    }

    if(err == DPD_ERR_CODE_NO_ERROR)
    {
        dpd_AdpFeatureRow_t *feature = pModelDesc->feature;
        uint64_t(*featureMask)[DPD_NUM_FEAT_MASK_BLKS] = pData->pLut->featMask;
        int8_t lut = -1;
        bool linearTermFound = false;
        /* TODO: update model configuration */
        pData->pActModelCfg->txCh              = 0u;                                /*!< Tx channel is fixed as 0u */
        pData->pActModelCfg->gmpDdrMode        = pModelDesc->mode;                  /*!< GMP+DDR mode */
        pData->pActModelCfg->ddrOutShift       = 9u;                                /*!< fixed at 9 or could be set by tracking config */
        pData->pActModelCfg->gmpOutShift       = 9u;                                /*!< fixed at 9 or could be set by tracking config */
        pData->pActModelCfg->magGain           = pData->pTrackCfg->magGain;         /*!< magnitude square gain assignment */
        pData->pActModelCfg->modelIndex        = pModelDesc->dpdPartial.modelIndex; /*!< model index 0-1 for 32 deep LUT, 0-3 for 16 Deep LUT */
        pData->pActModelCfg->actDepth          = pModelDesc->actDepth;              /*!< depth of LUT 0: 64 deep 1: 32 deep 2: 16 deep */
        pData->pActModelCfg->lutInterpMagRound = 0u;                                /*!< fixed at 0, no further rounding required */
        pData->pActModelCfg->lutInterpBypass   = 0u;                                /*!< fixed at 0, enable interpolator */
        /*
        * From the DPD model descriptor, find number of LUTs and LUT IDs
        * and store feature IDs in each LUT for LUT programming and Tx
        * sample EVM calculation.
        */
        memset((void *)pData->pLut, 0, sizeof(dpd_LutFeature_t));
        memset(pData->pLut->idx, -1, DPD_ACT_NUM_LUTS);
        pData->pLut->linearTermFeatureIndex = 0xffu;
        pData->pLut->linearTermLutIndex     = 0xffu;
        for (uint8_t fIdx = 0u; (fIdx < pModelDesc->features) && (err == DPD_ERR_CODE_NO_ERROR); fIdx++)
        {
            uint8_t bIndex, fIndex;
            int8_t  lutI     = feature[fIdx].i;
            int8_t  lutJ     = feature[fIdx].j;
            dpd_UtilGetBlockFeatureMaskIndex(fIdx, &bIndex, &fIndex);
            /* ONLY DPD_ACT_POLY_GMP0/1 is supported currently */
            err = dpd_FindLutId_GMPMode(&lut, lutI, lutJ, feature[fIdx].poly);
#ifdef MATLAB_BUILD
        MEX_LOG("lutI=%d, lutJ=%d, poly=%d, lut = %d\n",
                lutI, lutJ, feature[fIdx].poly, lut);
#endif
            if (err == DPD_ERR_CODE_NO_ERROR)
            {
                if (pData->pLut->lutIdFound & (1ull << lut))
                {
                    if ((feature[fIdx].i != feature[pData->pLut->featIdx[lut]].i)
                        || (feature[fIdx].j != feature[pData->pLut->featIdx[lut]].j)
                        || (feature[fIdx].poly != feature[pData->pLut->featIdx[lut]].poly))
                    {
#ifdef MATLAB_BUILD
        MEX_LOG("pData->pLut->featIdx[%d] = %d\n", lut, pData->pLut->featIdx[lut]);
        MEX_LOG("fIndex=%d, i=%d, j=%d, poly=%d\n", fIdx, feature[fIdx].i, feature[fIdx].j, feature[fIdx].poly);
#endif
                        err = DPD_MODEL_DUPLICATE_LUT_ERROR;
                    }
                }
                else
                {
                    /* keep track of found LUT ID and store it */
                    pData->pLut->lutIdFound           |= 1ull << lut;
                    pData->pLut->idx[pData->pLut->cnt] = lut;
                    /* store one of feature IDs of a LUT */
                    pData->pLut->featIdx[lut] = fIdx;
                    /* count number of LUTs in DPD model */
                    pData->pLut->cnt++;
                }
            }
            if (err == DPD_ERR_CODE_NO_ERROR)
            {
                /* find feature index of the linear term */
                if (!linearTermFound)
                {
                    if ((feature[fIdx].k == 0u)
                        && (feature[fIdx].i == feature[fIdx].j))
                    {
                        /* In case that i = j = linearTerm LUT does not exist in the model,
                            * assign i = j = 0 as the linear term LUT.
                            */
                        if ((feature[fIdx].i == 0)
                            || (feature[fIdx].i == pData->pTrackCfg->linearTerm))
                        {
                            linearTermFound                      = (feature[fIdx].i == 0) ? false : true;
                            pData->pActModelCfg->lutLinearTerm   = (feature[fIdx].i == 0) ? 0u : (uint8_t)(pData->pTrackCfg->linearTerm + pModelDesc->lutJDelay);
                            pData->pLut->linearTermFeatureIndex  = fIdx;
                            pData->pLut->linearTermLutIndex      = (uint8_t)lut;
                        }
                    }
                }
                /* keep track of features of a LUT */
                featureMask[lut][bIndex] |= 1ull << fIndex;

            }
        }
    }
    return err;
}

static dpd_ErrCode_e dpd_InitModelStateInfo(dpd_TrackData_t *pData)
{
    dpd_ErrCode_e err = DPD_ERR_CODE_NO_ERROR;

    pData->pModelStateInfo = &defaultDpdModelStateInfo;
    pData->pModelStateInfo->modelSel = DPD_ACT_MODEL_M;
    pData->pModelStateInfo->vBankPingBuf[DPD_ACT_MODEL_M] = bankPingBuf;
    pData->pModelStateInfo->vBankPongBuf[DPD_ACT_MODEL_M] = bankPongBuf;

    return err;
}

static dpd_ErrCode_e dpd_FindLutId_GMPMode(int8_t *pId,
                                            int8_t i,
                                            int8_t j,
                                            uint8_t poly)
{
    dpd_ErrCode_e err = DPD_ERR_CODE_NO_ERROR;

    if((i > DPD_SIGNAL_I_RANGE) || (j > DPD_SIGNAL_J_RANGE))
    {
        err = DPD_MODEL_INVALID_ERROR;
    }

    if(err == DPD_ERR_CODE_NO_ERROR)
    {
        switch(poly)
        {
            case(DPD_ACT_POLY_GMP0):
            case(DPD_ACT_POLY_GMP1):
                /* Derive GMP1 LUT ID using signal I and J */
                *pId = (int8_t)(j * (int8_t)DPD_SIGNAL_I_RANGE + i);
                break;
            default:
                *pId = -1;
                err = DPD_MODEL_INVALID_ERROR;
                break;
        }
    }

    return err;
}
