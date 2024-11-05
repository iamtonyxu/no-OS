/*! **************************************************************************** 
 *
 * @file:    math_utils.c
 *
 * @brief:   Math utility functions
 *
 * @details: This file contains the math utility functions. 
 *
 *
 * @date:    $Date:
 *
 *******************************************************************************
 */

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "math_utils.h"

#ifdef __ICCARM__
/* Error[Pm152]: array indexing shall only be applied to object defined as an 
 *               array type (MISRA C 2004 rule 17.4)
 */
#pragma diag_suppress=Pm152
#endif

/**
 *  @addtogroup UTILS
 *  @addtogroup utilities
 *  @{
 */

#define NEWTON_ITER(n, i)  (((n) + (i)/(n)) >> 1)

/**
 ******************************************************************************* 
 * Function: utils_floorLog2U32
 *
 * @brief       compute floor(log2(x)), where 'x' is uint32_t.
 *
 * @details   
 * 
 * Parameters: 
 * @param [in/out]  x  -  Value to be converted
 *
 * @return          floor_log2_u32(x)
 *
 * Reference to other related functions
 * @sa 
 *
 * Notes: 
 * 
 *******************************************************************************
*/
#ifdef __ICCARM__

#include <intrinsics.h>

uint32_t utils_floorLog2U32(uint32_t x)
{
    /* __CLZ returns the number of leading (MSB) zeros */
    return (31U - __CLZ(x));
}

#elif defined  __GNUC__

uint32_t utils_floorLog2U32(uint32_t x)
{
    /* __CLZ returns the number of leading (MSB) zeros */
    return (31U - __builtin_clz(x));
}

#else /* other compiler */

uint32_t utils_floorLog2U32(uint32_t x)
{
    return (uint32_t)floor(log2((double)x));
}

#endif


/**
 ******************************************************************************* 
 * Function: utils_ceilLog2U32
 *
 * @brief       Calculate log base 2.
 *
 * @details   
 * 
 * Parameters: 
 * @param [in/out]  x  -  Value to be converted
 *
 * @return          log2(x)
 *
 * Reference to other related functions
 * @sa 
 *
 * Notes: 
 * 
 *******************************************************************************
*/
uint32_t utils_ceilLog2U32(uint32_t x)
{
    uint32_t mask;
    uint32_t y;

    y = utils_floorLog2U32(x);
    mask = (1ul << y) - 1u;
    if (mask & x)
    {
        y++;
    }

    return y;
}

/*!
 *******************************************************************************
 * Function: utils_powCpx
 *
 * @brief       calculate complex power in 16-bit integer
 *
 * @details     
 *
 * Parameters:
 * @param [in]  x - complex input
 *
 * @return      result
 *
 * Reference to other related functions
 * @sa
 *
 *******************************************************************************
*/
uint32_t utils_powCpx(int16_cpx x)
{
    int32_t result = (int32_t)x.real * (int32_t)x.real + (int32_t)x.imag * (int32_t)x.imag;

    return (uint32_t)result;
}

/*!
 *******************************************************************************
 * Function: utils_powCpx32
 *
 * @brief       calculate complex power in 32-bit integer
 *
 * @details     
 *
 * Parameters:
 * @param [in]  x - complex input
 *
 * @return      result
 *
 * Reference to other related functions
 * @sa
 *
 *******************************************************************************
*/
uint64_t utils_powCpx32(int32_cpx x)
{
    int64_t result = (int64_t)x.real * (int64_t)x.real + (int64_t)x.imag * (int64_t)x.imag;

    return (uint64_t)result;
}

/*!
 *******************************************************************************
 * Function: utils_powCpxFlt
 *
 * @brief       calculate complex power in floating-point
 *
 * @details     
 *
 * Parameters:
 * @param [in]  x - complex input
 *
 * @return      result
 *
 * Reference to other related functions
 * @sa
 *
 *******************************************************************************
*/
float utils_powCpxFlt(flt_cpx x)
{
    return ((x.real * x.real) + (x.imag * x.imag));
}


/**
 *******************************************************************************
 * Function: utils_powFloatIQ
 *
 * @brief    calculate IQFloat_t i^2 + q^2
 *
 * Parameters:
 * @param [in,out]  p - IQFloat_t pointer
 *
 * @return          p->i^2 + p->q^2
 *
 *******************************************************************************
*/
float utils_powFloatIQ(IQFloat_t *p)
{
    return ((p->i * p->i) + (p->q * p->q));
}

/*!
 *******************************************************************************
 * Function: utils_isqrt
 *
 * @brief       take square root of a number
 *
 * @details     
 *
 * Parameters:
 * @param [in]  num - number
 *
 * @return      n1
 *
 * Reference to other related functions
 * @sa
 *
 *******************************************************************************
*/
uint32_t utils_isqrt(uint32_t num)
{  
    uint32_t n  = 1u;  
    uint32_t n1 = NEWTON_ITER(n, num);  

    while (abs((int32_t)n1 - (int32_t)n) > 1)
    {  
        n  = n1;  
        n1 = NEWTON_ITER(n, num);  
    }  
    while(n1 * n1 > num)
    {
        n1--;
    }
    
    return n1;  
}

/*!
 *******************************************************************************
 * Function: utils_cpxMultiply
 *
 * @brief    Floating-point complex multiply
 *
 * @details  
 *
 * Parameters:
 * @param [in]  x - input
 * @param [in]  y - input
 *
 * @return      result
 *
 *******************************************************************************
*/
flt_cpx utils_cpxMultiply(const flt_cpx x, const flt_cpx y)
{
    flt_cpx result;

    result.real = x.real * y.real - x.imag * y.imag;
    result.imag = x.real * y.imag + x.imag * y.real;

    return result;
}

/*!
 *******************************************************************************
 * Function: utils_maxU32
 *
 * @brief    Find max value in a uint32_t array
 *
 * @details  Optionally return the index of the max value
 *
 * Parameters:
 * @param [in]  pArray - array of uint32_t
 * @param [in]  arrayLen - length of pArray
 * @param [out] pIndex - index of max value.  NULL can be passed if index not needed
 *
 * @return      maximum value found in pArray
 *
 *******************************************************************************
*/
uint32_t utils_maxU32(const uint32_t *pArray, uint32_t arrayLen, uint32_t *pIndex)
{
    uint32_t i;
    uint32_t maxVal = 0u;
    uint32_t maxIdx = 0u;
    
    for (i = 0u; i < arrayLen; i++)
    {
        if (pArray[i] > maxVal)
        {
            maxVal = pArray[i];
            maxIdx = i;
        }
    }

    if (pIndex != NULL)
    {
        *pIndex = maxIdx;
    }

    return maxVal;
}

/*!
 *******************************************************************************
 * Function: utils_maxFloat
 *
 * @brief    Find max value in a float array
 *
 * @details  Optionally return the index of the max value
 *
 * Parameters:
 * @param [in]  pArray - array of float
 * @param [in]  arrayLen - length of pArray
 * @param [out] pIndex - index of max value.  NULL can be passed if index not needed
 *
 * @return      maximum value found in pArray
 *
 *******************************************************************************
*/
float utils_maxFloat(const float *pArray, uint32_t arrayLen, uint32_t *pIndex)
{
    uint32_t i;
    float maxVal = -INFINITY;
    uint32_t maxIdx = 0u;
    
    for (i = 0u; i < arrayLen; i++)
    {
        if (pArray[i] > maxVal)
        {
            maxVal = pArray[i];
            maxIdx = i;
        }
    }

    if (pIndex != NULL)
    {
        *pIndex = maxIdx;
    }

    return maxVal;
}

/*!
 *******************************************************************************
 * Function: utils_floorDivU32
 *
 * @brief    computes floor(num/den) for integers 'num' and 'den'
 *
 * @details  
 *
 * Parameters:
 * @param [in]  num - numerator
 * @param [in]  den - denominatory
 *
 * @return      floor(num/den)
 *
 *******************************************************************************
*/
uint32_t utils_floorDivU32(uint32_t num, uint32_t den)
{
    return num / den;
}

/*!
 *******************************************************************************
 * Function: utils_ceilDivU32
 *
 * @brief    computes ceil(num/den) for integers 'num' and 'den'
 *
 * @details  
 *
 * Parameters:
 * @param [in]  num - numerator
 * @param [in]  den - denominatory
 *
 * @return      ceil(num/den)
 *
 *******************************************************************************
*/
uint32_t utils_ceilDivU32(uint32_t num, uint32_t den)
{
    uint32_t r = num / den;
    return (r*den == num) ? r : (r + 1u);
}

/*!
 *******************************************************************************
 * Function: utils_GCD
 *
 * @brief    computes the greatest common divisor (GCD) of integers
 *           'a' and 'b' using the Euclidean algorithm
 *
 * @details  
 *
 * Parameters:
 * @param [in]  a - integer input
 * @param [in]  b - integer input
 *
 * @return      gcd(a, b)
 *
 *******************************************************************************
*/
uint32_t utils_GCD(uint32_t a, uint32_t b)
{
    while (b > 0u)
    {
        uint32_t t = b;
        b = a % b;
        a = t;
    }
    return a;
}

/*!
 *******************************************************************************
 * Function:    utils_LinearFit
 *
 * @brief       Perform a linear regression fit
 *
 * @details     
 *
 * NOTE: Considered adding logic to calculate the residues to store in the
 *       performance metric.  However, the logic to perform that requires
 *       a lot more math.
 *
 * Parameters:
 * @param [in]  x         - x array
 * @param [in]  y         - y array
 * @param [in]  numXY     - number of x,y points to fit
 * @param [out] pLineInfo - pointer to line information output structure
 *
 * @return     None
 *
 * Reference to other related functions
 * @sa  
 *
 *******************************************************************************
*/
void utils_LinearFit(const uint16_t x[],
                     const float    y[],
                     const uint16_t numXY,
                     LineInfo_t *pLineInfo)
{
    float  sumX    = 0.0f;
    float  sumXSqr = 0.0f;
    float  sumY    = 0.0f;
    float  sumYX   = 0.0f;
    float  bmDenom, mNumer, bNumer;
    uint16_t i;

    for (i = 0u; i < numXY; i++)
    {
        sumX    += (float)x[i];               /* sum of x   */
        sumXSqr += (float)x[i] * (float)x[i]; /* sum of x^2 */
        sumY    += y[i];                      /* sum of y   */
        sumYX   += y[i] * (float)x[i];        /* sum of x*y */
    }

    /* y = mx + b, if y = 0, x(0) = -b/m, N = number of points
     *
     * m = (N       * sumXY) - (sumX * sumY)  / (N * sumXsqr) - (sumX * sumX)
     * b = (sumXsqr * sumY)  - (sumX * sumXY) / (N * sumXsqr) - (sumX * sumX)
     *
     * notice that these have the same denominator
     * m = mNumer / bmDenom
     * b = bNumer / bmDenom
     * x(0) = -b/m = -bNumer / mNumer
     */

    bmDenom = ((float)numXY * sumXSqr) - (sumX * sumX);

    mNumer = ((float)numXY * sumYX) - (sumX * sumY);

    pLineInfo->slope = mNumer / bmDenom;

    bNumer = (sumX * sumYX) - (sumXSqr * sumY);

    /* where b = -bNumer / bSlopeDenom */
    pLineInfo->b = -bNumer / bmDenom;
}

/*!
 *******************************************************************************
 * Function: utils_Polyfit
 *
 * @brief    Perform a polynomial (2nd order) curve fit of input data
 *
 * @details  This code was verified by comparing various data sets with results
 *           from Matlab's Curve Fitting Tool.
 *
 * Parameters:
 * @param [in]  x          - array containing x input
 * @param [in]  y          - array containing y input
 * @param [in]  numXY      - number of x,y pairs
 * @param [out] pCoeffInfo - pointer to output polynomial coefficients
 *
 * @return      None
 *
 *******************************************************************************
*/
void utils_Polyfit(const int16_t  x[],
                   const float    y[],
                   const uint16_t numXY,
                   CoeffInfo_t *pCoeffInfo)
{
    int8_t i, j, k;
    int8_t n = 2; /* order */
    const int16_t N = (int16_t)numXY;
 
    float X[5];    /* 2*n + 1 Array that will store the values of sigma(xi),sigma(xi^2),sigma(xi^3)....sigma(xi^2n) */
    float B[3][4]; /* [n+1][n+2] B is the Normal matrix(augmented) that will store the equations */
    float Y[3];    /* [n + 1] Array to store the values of sigma(yi),sigma(xi*yi),sigma(xi^2*yi)...sigma(xi^n*yi) */
    float a[3];    /* 'a' is for value of the final coefficients */

    /* consecutive positions of the array will store
     * N,sigma(xi),sigma(xi^2),sigma(xi^3)....sigma(xi^2n) */
    for (i = 0; i < (2 * n) + 1; i++)
    {
        X[i] = 0.0f;
        
        for (j = 0; j < N; j++)
        {
            X[i] += powf((float)x[j], (float)i);
        }
    }

    /* Build the Normal matrix by storing the corresponding coefficients
     * at the right positions except the last column of the matrix */
    for (i = 0; i <= n; i++)
    {
        for (j = 0; j <= n; j++)
        {
            B[i][j] = X[i + j];
        }
    }

    /* consecutive positions will store sigma(yi),sigma(xi*yi),
     * sigma(xi^2*yi)...sigma(xi^n*yi) */
    for (i = 0; i < n + 1; i++)
    {    
        Y[i] = 0.0f;
        for (j = 0; j < N; j++)
        {
            Y[i] += powf((float)x[j], (float)i) * y[j];
        }
    }
    /* load the values of Y as the last column of B(Normal Matrix but augmented) */
    for (i = 0; i <= n; i++)
    {
        B[i][n + 1] = Y[i];
    }

    n++;

    /* n is made n+1 because the Gaussian Elimination part below was for n 
     * equations, but here n is the degree of polynomial and for n degree
     * we get n+1 equations
     *
     * The Normal(Augmented Matrix) is as follows:  B[i..n-1][0 ..n]
     *
     * From now Gaussian Elimination starts (can be ignored) to solve
     * the set of linear equations (pivot the matrix)
     */
    for (i = 0; i < n; i++)
    {
        for (k = i + 1; k < n; k++)
        {
            if (B[i][i] < B[k][i])
            {
                for (j = 0; j <= n; j++)
                {
                    float tmp = B[i][j];
                    B[i][j] = B[k][j];
                    B[k][j] = tmp;
                }
            }
        }
    }
    for (i = 0; i < n - 1; i++) /* loop to perform the gauss elimination */
    {
        for (k = i + 1; k < n; k++)
        {
            float t = B[k][i] / B[i][i];

            /* make the elements below the pivot elements equal to zero
             * or eliminate the variables */
            for (j = 0; j <= n; j++)
            {
                B[k][j] -= (t * B[i][j]);
            }
        }
    }

    for (i = n - 1; i >= 0; i--)  /* back-substitution */
    {
        /* make the variable to be calculated equal to
         * the rhs of the last equation */
        a[i] = B[i][n];

        for (j = 0; j < n; j++)
        {
            /* then subtract all the lhs values except the coefficient of
             * the variable whose value is being calculated */
            if (j != i)
            {
                a[i] -= (B[i][j] * a[j]);
            }
        }
        /* now finally divide the rhs by the coefficient
         * of the variable to be calculated */
        a[i] /= B[i][i];
    }

    /* copy computed coefficients into the output structure */
    for (i = 0; i < n; i++)
    {
        pCoeffInfo->p[i] = a[i];
    }

} /* utils_Polyfit */

/*!
 *******************************************************************************
 * Function: utils_PolyEval
 *
 * @brief    Evaluate a polynomial and return the result
 *
 * @details  
 *
 * Parameters:
 * @param [in]  x          - input
 * @param [in]  pCoeffInfo - pointer to polynomial coefficients
 *
 * @return      result
 *
 *******************************************************************************
*/
float utils_PolyEval(const int16_t x, const CoeffInfo_t *pCoeffInfo)
{
    float result;
    float xf = (float)x;

    result  = xf * (pCoeffInfo->p[1] + (pCoeffInfo->p[2] * xf));
    result += pCoeffInfo->p[0];

    return result;
}

/*@}*/

