/*! **************************************************************************** 
 *
 * @file:    CholeskyMatrix_real.h
 *
 * @brief:   Top-level module for the RxQEC linear solver
 *
 * @details: This file instantiates a separate instance of the TxQEC tracking
 *           algorithm for each receiver channel.
 *
 * @date:    $Date:
 *
 *******************************************************************************
 */
#ifndef __CHOLESKY_MATRIX_REAL_HEADER__
#define __CHOLESKY_MATRIX_REAL_HEADER__

#include <stdint.h>
#include <complex.h>
#include <math.h>
#include "math_utils.h"
#include "dpd_utils.h"

void CholeskyMatrix_real(double *_l, const double *b, uint32_t nSize, double *z);
void CholeskyMatrix_mldivide(double *_l, const double *_b, double *_z, uint32_t nSize, uint32_t mSize);
void CholeskyMatrix_mrdivide(const double *_a, double *_b, double *_z, uint32_t nSize, uint32_t mSize);

void CholeskyMatrix_solve(const double *_l, const double *_b, double *_z, uint32_t nSize, uint32_t mSize);
void CholeskyMatrix_Decompose(double *_l, uint32_t nSize);
void CholeskyMatrix_mrsolve(const double *_a, double *_b, double *_z, uint32_t nSize, uint32_t mSize);
void CholeskyMatrix_mlsolve(const double *_l, const double *_b, double *_z, uint32_t nSize, uint32_t mSize);

#endif /* __CHOLESKY_MATRIX_REAL_HEADER__ */