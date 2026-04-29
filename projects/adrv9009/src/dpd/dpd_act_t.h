#ifndef _DPD_ACT_T_H_
#define _DPD_ACT_T_H_

/**
 * \brief Timeout for LUT copy done
 */

#define LUT_COPY_TIME_OUT_MS    (10)               /* 10ms */

/**
 * \brief Define maximum number of LUTs
 */
#define DPD_ACT_NUM_LUTS        (50u)

/**
 * \brief Define maximum number of banks
 */
#define DPD_ACT_NUM_BANKS       (4u)

/**
 * \brief Define maximum number of Lut depth
 */
#define DPD_ACT_DEPTH_MAX       (256u)


/**
 * \brief Enumerated list of options of DPD model type
 */
typedef enum
{
    DPD_ACT_MODEL_M          = 0u,                 /*!< M table for SUMO/CMT */
    DPD_ACT_MODEL_C          = 1u,                 /*!< C table for CMT */
    DPD_ACT_MODEL_R          = 2u,                 /*!< R table for system recovery */
    DPD_ACT_MODEL_CTC        = 3u,                 /*!< CTC table */
    DPD_ACT_MODEL_INVALID,                         /*!< invalid DPD model */
} dpd_ActModel_e;

// TODO: Only one Model is supported currently
#define DPD_ACT_MODEL_MAX    (DPD_ACT_MODEL_M + 1)

/**
 * \brief Enumerated list of MOD_CFGx bitfields
 */
typedef enum
{
    DPD_ACT_MOD_CFG0 = 0u,                 /*!< MOD_CFG0  */
    DPD_ACT_MOD_CFG1 = 1u,                 /*!< MOD_CFG1  */
    DPD_ACT_MOD_CFG2 = 2u,                 /*!< MOD_CFG2  */
    DPD_ACT_MOD_CFG3 = 3u,                 /*!< MOD_CFG3  */
} dpd_ActModCfg_e;

/**
 * \brief Enumerated list of options of DPD ACT reset mode
 */
typedef enum
{
    DPD_ACT_COEF_NORMAL,                           /*!< Normal actuator Luts update using entry values calculated from the coefficients */
    DPD_ACT_COEF_UNITY,                            /*!< Reset the actuator linear term Lut to unity and rest of the Luts to zero */
} dpd_ActResetMode_e;

#endif