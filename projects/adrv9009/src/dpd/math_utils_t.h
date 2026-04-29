/*! **************************************************************************** 
 *
 * @file:    math_utils_t.h
 *
 * @brief:   exported Math utility types and non-macro defines
 *
 * @details: This file contains all the exported Math utility types and defines
 *
 * @date:    $Date:
 *
 *******************************************************************************
 */
#ifndef __MATH_UTILS_T_HEADER__
#define __MATH_UTILS_T_HEADER__

#define  DEF_PI     (3.1415926535897932f)

#define  TWO_PI     (2.0f * DEF_PI)

#define  ONE_MILLION_INV  (1.0f / 1000000.0f)

#define  ONE_DIV_SQRT2    (0.7071067812f)

#define  DBL_CPX_ZERO    (double complex)(0.0 + 0.0 * I)
#define  DBL_CPX_ONE     (double complex)(1.0 + 0.0 * I)

typedef struct
{
    int16_t i;
    int16_t q;
} IQInt16_t;

typedef struct
{
    int32_t i;
    int32_t q;
} IQInt32_t;

typedef struct
{
    int64_t i;
    int64_t q;
} IQInt64_t;

typedef struct
{
    float i;
    float q;
} IQFloat_t;

typedef struct
{
    double real;
    double imag;
} dbl_cpx;

typedef struct
{
    float real;
    float imag;
} flt_cpx;

typedef struct
{
    int64_t real;
    int64_t imag;
} int64_cpx;

typedef struct
{
    int32_t real;
    int32_t imag;
} int32_cpx;

typedef struct
{
    int16_t real;
    int16_t imag;
} int16_cpx;

typedef struct
{
    int8_t real;
    int8_t imag;
} int8_cpx;

typedef struct
{
    float   p[3];
} CoeffInfo_t;

typedef struct
{
    float   slope;
    float   b;
} LineInfo_t;

#endif /* __MATH_UTILS_T_HEADER__ */
