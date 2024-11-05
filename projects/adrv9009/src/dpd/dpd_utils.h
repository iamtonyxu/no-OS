#ifndef _DPD_UTILS_H_
#define _DPD_UTILS_H_

#ifndef EN_MEX_LOG
#define   EN_MEX_LOG    (0)
#endif

#if (EN_MEX_LOG)
     #include <mex.h>
     #define MEX_LOG(...)    mexPrintf(__VA_ARGS__)
#else
     #define MEX_LOG(...)
#endif

#ifndef EN_MEX_LOG_DETAILED
#define   EN_MEX_LOG_DETAILED    (0)
#endif

#if (EN_MEX_LOG_DETAILED)
     #include <mex.h>
     #define MEX_LOG_DETAILED(...)    mexPrintf(__VA_ARGS__)
#else
     #define MEX_LOG_DETAILED(...)
#endif

/* CAUTION: Do NOT change any of these #defines */
#define  DPD_NULL        (0u)

#define  DPD_FALSE       (0ul)
#define  DPD_TRUE        (1ul)

#define  DPD_DISABLE     (0u)
#define  DPD_ENABLE      (1u)

#define  DPD_POWERUP     (0u)
#define  DPD_POWERDOWN   (1u)

/**
 * \brief Get block feature index
 *
 * Feature mask requires 255 bits and is divided into serveral 64-bit
 * feature mask blocks. This function converts a given feature mask index
 * into a 64-bit block index and a feature mask index within that
 * 64-bit block.
 *
 * \param[in]  fIndex      The feature index
 * \param[out] pBlkIdx     The feature mask block index
 * \param[out] pBlkFeatIdx The feature index within a featrue mask block
 *
 * \retval NA
 */
void dpd_UtilGetBlockFeatureMaskIndex(uint8_t fIndex,
                                      uint8_t *pBlkIdx,
                                      uint8_t *pBlkFeatIdx);

/*!
 *******************************************************************************
 * \brief calculate legendre result
 *
 * \note calculate legendre result based on the given degree of polunomial
 *
 * \param [in]  x - input value
 * \param [in]  n - degree of polynomial
 *
 * @return      err
 *******************************************************************************
 */
double dpd_UtilShiftedLegendre(double x, uint32_t n);

#endif
