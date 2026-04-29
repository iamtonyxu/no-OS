#ifndef _DPD_T_P_H_
#define _DPD_T_P_H_

#include <stdint.h>
#include <complex.h>
#include <stdbool.h>
#include "math_utils.h"
#include "dpd_model_t.h"
#include "dpd_act_t.h"

#define DPD_SAMPLE_NORMALIZATION       (1)
#define DPD_FEATURE_NORMALIZATION      (1.0)
#define DPD_SIGNAL_I_RANGE             (8u) // TODO: refering to HW Actuator Design
#define DPD_SIGNAL_J_RANGE             (8u) // TODO: refering to HW Actuator Design
#define DPD_SAMPLE_DELAY_I_BUF_SIZE    (16u)
#define DPD_SAMPLE_DELAY_J_BUF_SIZE    (14u)
#define DPD_MAX_K_POW                  (15u)
#define DPD_MAX_NUM_CAP_BATCHES        (8u)
#define DPD_MAX_NUM_CAP_SAMPLES        (4096u)
#define DPD_LUT_SCALE                  (8192u)
#define DPD_CHOL_SOLVE_BUF_OFFSET      (65536u)
#define DPD_DELAY_BUF_SIZE             ((DPD_SAMPLE_DELAY_I_BUF_SIZE > DPD_SAMPLE_DELAY_J_BUF_SIZE) ? DPD_SAMPLE_DELAY_I_BUF_SIZE - 1u : DPD_SAMPLE_DELAY_J_BUF_SIZE - 1u)
#define DPD_MAX_NUM_FEATURES           (95u)
#define DPD_MAX_NUM_FF_TAPS            (64u)
#define DPD_MAX_INDIRECT_COUNT         (3u)

/**
 * \brief Define maxinum number of feature mask blocks to fit 255 features
 *        (Each block is 64 bits)
 * Note: It's hardware related and could be optimzed later.
 */
#define DPD_NUM_FEAT_MASK_BLKS         (4u)

/**
 * \brief Enumerated list of options of DPD processing state
 */
typedef enum
{
    PROCSTATE_DELAYEST,             /*!< Perform pathdelay estimation */
    PROCSTATE_ALIGNMENT,            /*!< Sample alignment including gain and phase adjustment */
    PROCSTATE_FEATCOMP,             /*!< Peform feature computation */
    PROCSTATE_MATRIXSOLVER,         /*!< Perform cholesky decomposition */
    PROCSTATE_MODELCHECK,           /*!< Perform stability check */
    PROCSTATE_NULL,                 /*!< DPD process is finished or aborted */
} dpd_ProcState_e;

/**
 * \brief Enumerated list of options of DPD path delay mode
 */
typedef enum
{
    DPD_STARTUP_MODE  = 0u,
    DPD_TRACKING_MODE = 1u,
} dpd_PathDlyMode_e;

/**
 * \brief Data structure to hold parameters of DPD path delay estimate control
 *
 * This structure is only a part of complete DPD path delay data.
 */
typedef struct
{
    uint8_t pdEstIterStep;     /*!< run delay estimation on every iteration or every few iterations */
    uint8_t pdEstPerIter;      /*!< run delay estimation on which set(s) of captured data for each iteration */
    uint8_t xCorrLagRange;     /*!< shorter x-corr & peak search range in pathdelay tracking mode */
    uint8_t intDelayRangeLoop; /*!< search range for integer delay within the loop */
} dpd_PathDlyEstCtrl_t;

typedef struct
{
    int32_t  intDelay;   /*!< Integer Delay */
    int32_t  fracDelay;  /*!< Fractional Delay */
    uint32_t iterNum;    /*!< Iteration Number */
} dpd_PathDlyStatus_t;

/**
 * \brief Enumerated list of options of DPD power selection
 */
typedef enum
{
    DPD_TXPWRSEL_MEAN,                                /*!< Select mean of 4 10ms averaged powers for SUMO/CMT operation */
    DPD_TXPWRSEL_MEDIAN,                              /*!< Select median of 4 10ms averaged powers for SUMO/CMT operation */
    DPD_TXPWRSEL_MAX                                  /*!< Select max of 4 10ms averaged powers for SUMO/CMT operation */
} dpd_TxPwrSelect_e;

/**
 * \brief Data structure to hold parameters of DPD statics data
 */
typedef struct
{
    double complex cyx;                            /*!< raw (unnormalized) x-y correlation, used for current estimate of loopback gain and phase */
    double complex cxx;                            /*!< raw (unnormalized) x-x correlation, used for current estimate of loopback gain and phase */
    double complex cyu;                            /*!< raw (unnormalized) u-y correlation, used for current estimate of loopback gain and phase */
    int64_t        cuu;                            /*!< raw (unnormalized) u-u correlation, used for current estimate of loopback gain and phase */
    double complex invDirectLoopGain;              /*!< current inverse loop gain measured between u and y */
    double complex invIndirectLoopGain;            /*!< current inverse loop gain measured between x and y */
    uint8_t        doTableUpdate;                  /*!< do table update */
} dpd_Statics_t;

/**
 * \brief Data structure to hold parameters of DPD LUT features
 *
 * This structure is only a part of complete DPD model config data
 */
typedef struct
{
    uint8_t  cnt;                                                /*!< number of LUTS used in the DPD model */
    int8_t   idx[DPD_ACT_NUM_LUTS];                              /*!< LUT index of a LUT */
    uint64_t featMask[DPD_ACT_NUM_LUTS][DPD_NUM_FEAT_MASK_BLKS]; /*!< feature index mask of a LUT */
    uint8_t  featIdx[DPD_ACT_NUM_LUTS];                          /*!< one of the feature index in a LUT */
    uint64_t lutIdFound;                                         /*!< lutIdFound mask */
    uint8_t  linearTermFeatureIndex;                             /*!< linear term feature index */
    uint8_t  linearTermLutIndex;                                 /*!< linear term LUT index */
} dpd_LutFeature_t;

/**
 * \brief Data structure to hold  LUT update and model switching related information
 */
typedef struct
{
    uint32_t *vBankPingBuf[DPD_ACT_MODEL_MAX];        /*!< Ping vBank in memory */
    uint32_t *vBankPongBuf[DPD_ACT_MODEL_MAX];        /*!< Pong vBank in memory */
    uint8_t  bankIndex[DPD_ACT_MODEL_MAX];            /*!< Bank Index of each model */
    uint8_t  pingPongReadyTogFlag[DPD_ACT_MODEL_MAX]; /*!< Toggling Flag for Ping/Pong  */
    uint8_t  pingBufReady[DPD_ACT_MODEL_MAX];         /*!< Ping Buffer Ready Indicator  */
    uint8_t  pongBufReady[DPD_ACT_MODEL_MAX];         /*!< Pong Buffer ready Indicator  */
    uint8_t  ModCfgNum[DPD_ACT_MODEL_MAX];            /*!< Model Config Number  */
    uint8_t  ModCfgSwapBank;                          /*!< Model Config Swap Bank */
    uint8_t  bankIndexSwap;                           /*!< Bank Index of Swap Bank in FDD  */
    uint8_t  modelSel;                                /*!< Current active model  */
} dpd_ModelStateInfo_t;

/**
 * \brief Data structure to hold parameters of DPD tracing configuration
 *
 * This structure is only a part of complete DPD tracking strucutre.
 */
typedef struct
{
    uint8_t                    numFilterCoefficients;                   /*!< ** number of filter coefficients */
    uint8_t                    numFilterOutput;                         /*!< ** number of filter output */
    dpd_PathDlyEstCtrl_t       pdEstCtrl;                               /*!< GENERIC: pathdelay estimation control */
    uint16_t                   filterSel;                               /*!< GENERIC: filter select to match OBW of tx and orx samples, 0: 0.5Fs, 1: 0.8Fs */
    dpd_TxPwrSelect_e          txPwrSelect;                             /*!< Dynamics: 0: mean, 1: median, 2: max; make selection from 4 averaged power readings */
    int8_t                     delta;                                   /*!< Dynamics: SUMO/CMT M table power delta */
    int8_t                     decayP;                                  /*!< Dynamics: SUMO/CMT M table power decay */
    uint32_t                   mThreshold;                              /*!< Dynamics: SUMO/CMT M table threshold */
    uint16_t                   indirectErrSamples;                      /*!< Robustness: number of samples used to calculate indirect error (default: 256) */
    uint16_t                   satThresh;                               /*!< Robustness: sample level to be considered an ORX saturation or TX saturation */
    uint16_t                   outlierThresh;                           /*!< Robustness: threshold for sample in AM-AM plot outside of 1:1 line to be thrown out. */
    uint32_t                   satXCntLimit;                            /*!< Robustness: Tx saturation count limit */
    uint32_t                   satYCntLimit;                            /*!< Robustness: Orx saturation count limit */
    uint32_t                   errCntLimit;                             /*!< Robustness: AM-AM outlier error count limit */
    uint16_t                   txMinAvgSignalLevel;                     /*!< Robustness: Tx minimum DPD average signal power for tracking to occur (default 512 = -36dBFs) */
    uint16_t                   orxMinAvgSignalLevel;                    /*!< Robustness: Orx minimum DPD average signal power for tracking to occur (default 512 = -36dBFs) */
    uint8_t                    direct;                                  /*!< Learning mode: indirect or direct learning */
    double                     indirectRegValue;                        /*!< Learning mode: Tikhonov regularization value added to diagonal of correlation matrix for indriect learning */
    double                     directRegValue;                          /*!< Learning mode: Tikhonov regularization value added to diagonal of correlation matrix for direct learning */
    uint8_t                    mu;                                      /*!< Learning mode: direct learning step size from 0 to 100, will be divided by 100 */
    uint8_t                    magGain;                                 /*!< DPD LUT: Format: XX.XX_XXXX  Default = 0x40 */
    uint16_t                   absScale;                                /*!< CTC LUT: scale factor to keep most significant bits */
    float                      absOffset;                               /*!< CTC LUT: offset to prevent negative interpolation output */
    uint8_t                    linearTerm;
    //adi_svc_DfeAdaptationCfg_t adapt;                                   /*!< Adapt engine: adaptation engine configuration */
    //int16_t                    filterCoef[DPD_MAX_NUM_COEFFICIENTS];    /*!< Adapt engine: feature filter coefficient buffer (symmetric upto 32 complex pairs) */
    //adi_svc_DfePwrMtrDpdCfg_t  dpdPwrMeasCtrlCfg;                       /*!< GENERIC: dpd pwr measure ctrl structure */
} dpd_TrackCfg_t;

typedef struct
{
    uint16_t                    capDepth;                   /*!< Capture: samples per capture */
    uint8_t                     capBatch;                   /*!< Capture: number of capture times */
} dpd_CaptureCfg_t;

typedef struct
{
    dpd_TrackCfg_t          *pTrackCfg;                                     /*!< Dpd Track configuration */
    dpd_ModelDesc_t         *pDpdModelDesc;                                 /*!< Dpd Model Description */
    dpd_ActModelCfg_t       *pActModelCfg;                                  /*!<** Actuator config */
    bool                    actLutSatFlag;                                  /*!< GENERIC: Actuator LUT saturation flag */
    dpd_LutFeature_t        *pLut;                                          /*!<** DPD Lut config */
    dpd_ModelStateInfo_t    *pModelStateInfo;                               /*!< GENERIC: pointer to model state information */
    dpd_CaptureCfg_t        capCfg;                                         /*!< Capture size and numbers */
    dpd_PathDlyStatus_t     pathDlyStatus;                                  /*!<** path delay estimation status */
    dpd_PathDlyMode_e       pathDelayMode;                                  /*!< GENERIC: 0: startup mode, 1: tracking mode */    
    double complex          *pTx;                                           /*!< GENERIC: pointer to Tx adaptation (or input sample) buffer base address */
    double complex          *pORx;                                          /*!< GENERIC: pointer to ORx adaptation (or desired sample) buffer base address */
    uint32_t                firstSampleIndex[DPD_MAX_NUM_CAP_BATCHES];      /*!< GENERIC: first sample index of each capture */
    uint32_t                lastSampleIndex[DPD_MAX_NUM_CAP_BATCHES];      /*!< GENERIC: last sample index of each capture */
    uint32_t                alignedSampleCount;
    uint32_t                featureSampleCount;
    dpd_ProcState_e         procState;
    dpd_Statics_t           statics;
    double complex          biasOfCoeffs[DPD_MAX_NUM_FEATURES];
    double                  biasAlpha;
    double                  biasBeta;
    uint8_t                 iterCount;
    uint8_t                 direct;
    double                  regValue;

    /* Debug ONLY */
    double complex          *pFfInputBuf;
    double                  *pCholeskyDecompBuf;
    double                  *l;
} dpd_TrackData_t;

#endif
