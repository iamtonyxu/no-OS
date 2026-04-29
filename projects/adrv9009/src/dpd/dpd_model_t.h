#ifndef _DPD_MODEL_T_H_
#define _DPD_MODEL_T_H_
#include <stdint.h>
#include <complex.h>

/**
 * \brief Maxinum number of feature filter coefficients
 */
#define DPD_MAX_NUM_COEFFICIENTS    (64U)

/**
 * \brief Maxinum number of features supported in a DPD model
 */
#define DPD_MAX_NUM_FEATURES        (255u)

/**
 * \brief Number of DPD GMP power models
 */
#define DPD_GMP_POWER_MODELS        (3u)

/**
 * \brief DPD types
 */
typedef enum
{
    DPD_TYPE_DPD = 0u,        /*!< Regular DPD */
    DPD_TYPE_EDPD,            /*!< Enhanced DPD */
    DPD_TYPE_CTC,             /*!< Charge trap correction */
} dpd_Type_e;

/**
 * \brief DPD model polynomial types
 */
typedef enum
{
    DPD_ACT_POLY_GMP0,        /*!< GMP polynomial */
    DPD_ACT_POLY_GMP1,        /*!< GMP polynomial for startup model */
    DPD_ACT_POLY_DDR7,        /*!< DDR7 polynomial */
    DPD_ACT_POLY_DDR8,        /*!< DDR8 polynomial */
    DPD_ACT_POLY_DDR9,        /*!< DDR9 polynomial */
} dpd_ActPolyType_e;

/**
 * \brief Data structure to hold parameters of DPD partial structure
 */
typedef struct adi_app_CalDpdPartial
{
    dpd_Type_e           partial;         /*!< DPD, EDPD, or CTC (note: no DPD+EDPD combo) */
    uint8_t              updateOrder;     /*!< Update order (note: same order for (E)DPD High/Low power) */
    uint8_t              modelIndex;      /*!< 0/1/2: (E)DPD M/C/R models; 3 to 7: CTC models */
} dpd_Partial_t;

/**
 * \brief Enumerated list of options of GMP and DDR mode
 */
typedef enum
{
    DPD_ACT_GMP_DDR_MODE0,                   /*!< GMP0-50 */
    DPD_ACT_GMP_DDR_MODE1,                   /*!< GMP0-32 DDR7-6 DDR8-6 DDR9-6 */
    DPD_ACT_GMP_DDR_MODE2,                   /*!< GMP0-38 DDR7-6 DDR9-6 */
    DPD_ACT_GMP_DDR_MODE3,                   /*!< GMP0-38 DDR7-4 DDR8-4 DDR9-4 */
    DPD_ACT_GMP_DDR_MODE4,                   /*!< GMP0-44 DDR7-6 */
    DPD_ACT_GMP_DDR_MODE5,                   /*!< GMP0-44 DDR9-6 */
    DPD_ACT_GMP_DDR_MODE6,                   /*!< GMP0-44 DDR7-3 DDR9-3 */
    DPD_ACT_GMP_DDR_MODE7,                   /*!< GMP0-32 GMP1-9 DDR7-3 DDR8-3 DDR9-3 */
} dpd_ActuatorGmpDdrMode_e;

/**
 * \brief DPD LUT depth
 */
typedef enum
{
    DPD_ACT_DEPTH_16 = 0u,    /*!< Current depth of LUT 0: 16 deep */
    DPD_ACT_DEPTH_32 = 1u,    /*!< Current depth of LUT 1: 32 deep */
    DPD_ACT_DEPTH_64 = 2u,    /*!< Current depth of LUT 2: 64 deep */
    DPD_ACT_DEPTH_128 = 3u,    /*!< Current depth of LUT 3: 128 deep */
    DPD_ACT_DEPTH_256 = 4u,    /*!< Current depth of LUT 4: 256 deep */
} dpd_ActDepth_e;

/**
 * \brief Data structure to hold parameters of a row of DPD model features
 */
typedef struct
{
    int8_t                      i;         /*!< i - amplitude term */
    int8_t                      j;         /*!< j - signal term */
    uint8_t                     k;         /*!< k - power term */
    dpd_ActPolyType_e           poly;      /*!< polynomial type */
    double complex              a;         /*!< complex coefficient of a row of features */
} dpd_AdpFeatureRow_t;

/**
 * \brief Data structure to hold parameters of DPD model descriptor
 *
 * It is the interfacing structure for the user to describe the DPD model and retrieve the high/low power coefficients.
 */
typedef struct dpd_ModelDesc
{
    uint8_t                         features;                          /*!< number of features defined in a model */
    dpd_Partial_t                   dpdPartial;                        /*!< DPD partial */
    uint8_t                         mode;                              /*!< GMP+DDR mode: dpd_ActuatorGmpDdrMode_e */
    dpd_ActDepth_e                  actDepth;                          /*!< depth of LUT 0: 64 deep 1: 32 deep 2: 16 deep */
    int8_t                          lutIDelay;                         /*!< LUT i delay of the LUT representing i = j = 0 in the model */
    int8_t                          lutJDelay;                         /*!< LUT j delay of the LUT representing i = j = 0 in the model */
    dpd_AdpFeatureRow_t             feature[DPD_MAX_NUM_FEATURES];     /*!< GMP+DDR mixed model description */
} dpd_ModelDesc_t;

/**
 * \brief Data structure to hold model configuration parameters
 */
typedef struct
{
    uint8_t  txCh;                                   /*!< Tx channel */
    uint8_t  gmpDdrMode;                             /*!< GMP+DDR mode, see enum defined above */
    uint8_t  ddrOutShift;                            /*!< gain (shift left bit positions, default = 9) */
    uint8_t  gmpOutShift;                            /*!< gain (shift left bit positions, default = 9) */
    uint8_t  magGain;                                /*!< Format: XX.XX_XXXX  Default = 0x40. */
    uint8_t  modelIndex;                             /*!< model index 0-1 for 32 deep LUT, 0-3 for 16 Deep LUT */
    uint8_t  bankIndex;                              /*!< bank index 0-1 for 32 deep LUT, 0-3 for 16 Deep LUT */
    uint8_t  actDepth;                               /*!< depth of LUT 0: 64 deep 1: 32 deep 2: 16 deep */
    uint8_t  lutLinearTerm;                          /*!< LUT linear term */
    uint8_t  lutInterpMagRound;                      /*!< LUT interpolator magnitude rounding */
    uint8_t  lutInterpBypass;                        /*!< LUT interpolator bypass */
    uint64_t lutEn;                                  /*!< LUT ram that are active in a model */
    uint8_t  ddr_j_prog_delay_from_ddr_i;            /*!< Programmable delay before I terms of DDR after ddr_i_delay_from_gmp_i - Legal Value 0 to 4 */
    uint8_t  ddr_i_prog_delay_from_gmp_i;            /*!< Programmable delay before I terms of DDR after gmp_i_prog_delay - Legal Value 0 to 11 */
    uint8_t  gmp_1_j_prog_delay_from_gmp_j;          /*!< Programmable delay before i terms of GMP 1 after GMPI I delay - Legal Value 0 to 11 */
    uint8_t  gmp_1_i_prog_delay_from_gmp_i;          /*!< Programmable delay before i terms of GMP 1 after GMPI I delay - Legal Value 0 to 13 */
    uint8_t  gmp_j_prog_delay;                       /*!< Programmable delay before J terms of GMP - Legal Value 0 to 4 */
    uint8_t  gmp_i_prog_delay;                       /*!< Programmable delay before I terms of GMP - Legal Value 0 to 4 */
} dpd_ActModelCfg_t;

/**
 * \brief Data structure to hold parameters of DPD model
 *
 * It is the "live" structure containing the "live" DPD coefficients updated during DPD adaptation and "live" configuration
 * changed by the tracking config to setup the HW.
 */
typedef struct
{
    dpd_ModelDesc_t         modelDesc;                  /*!< model description */
    dpd_ActModelCfg_t       modelConfig;                /*!< Actuator model configuration */
} dpd_Model_t;

#endif