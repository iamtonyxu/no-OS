/*! **************************************************************************** 
 *
 * @file:    math_utils.h
 *
 * @brief:   exported Math utility functions and macros
 *
 * @details: This file contains all the exported Math utility prototypes 
 *           referenced globally.
 *
 * @date:    $Date:
 *
 *******************************************************************************
 */
#ifndef __MATH_UTILS_HEADER__
#define __MATH_UTILS_HEADER__

#include <stdint.h>
#include "math_utils_t.h"

#define  MIN(a,b)   ( ((a) < (b)) ? (a) : (b) )
#define  MAX(a,b)   ( ((a) > (b)) ? (a) : (b) )

#define  ABS(a)     ( ((a) >= (0)) ? (a) : (-(a)) )

#define  POW2(x)    ((x) * (x))

#define  DEF_ROUND_INT32(a)  ( (int32_t) (  ((float)(a) >= 0.0f) ? ((float)(a) + 0.5f) : ((float)(a) - 0.5f) ) )

#define  POWER_OF_TWO(x) ( (((x) & ((x) - 1u)) == 0u) && ((x) != 0u) )

#define  POWER_OF_TWO_OR_ZERO(x) ( (((x) & ((x) - 1u)) == 0u) || ((x) == 0u) )

#define  FLT_CPX_ZERO (flt_cpx){.real = 0.0f, .imag = 0.0f}

#define  FLT_CPX_ONE (flt_cpx){.real = 1.0f, .imag = 0.0f}

uint32_t utils_floorDivU32(uint32_t num, uint32_t den);
uint32_t utils_ceilDivU32(uint32_t num, uint32_t den);
uint32_t utils_floorLog2U32(uint32_t x);
uint32_t utils_ceilLog2U32(uint32_t x);
uint32_t utils_GCD(uint32_t a, uint32_t b);
uint32_t utils_powCpx(int16_cpx x);
uint64_t utils_powCpx32(int32_cpx x);
float    utils_powCpxFlt(flt_cpx x);
float    utils_powFloatIQ(IQFloat_t *p);
uint32_t utils_isqrt(uint32_t num);
flt_cpx  utils_cpxMultiply(const flt_cpx x, const flt_cpx y);
uint32_t utils_maxU32(const uint32_t *pArray, uint32_t arrayLen, uint32_t *pIndex);
float    utils_maxFloat(const float *pArray, uint32_t arrayLen, uint32_t *pIndex);

void     utils_LinearFit(const uint16_t x[],
                         const float    y[],
                         const uint16_t numXY,
                         LineInfo_t *pLineInfo);

void     utils_Polyfit(const int16_t  x[],
                       const float    y[],
                       const uint16_t numXY,
                       CoeffInfo_t *pCoeffInfo);

float    utils_PolyEval(const int16_t x,
                        const CoeffInfo_t *pCoeffInfo);

#endif /* __MATH_UTILS_HEADER__ */ 
