/*This code decomposes an input coefficient matrix using Cholesky decomposition method,
and then solves the input system of linear equations.
Input arguments:
1) Correlation (coefficient) matrix _l of size (nSize by nSize), containing auto- and cross-correlation terms
between the input signal and its image at a single observation frequency.

2) (nSize by 1) Cross-correlation vector b, containing the cross-correlation terms 
between the loopback signal and the input signal/ the image of the input signal at a single observation frequency (known measurements).

Output arguments:
(nSize by 1) vector z, containing the transfer function of the channels (S, and I) at a single observation frequency.  
*/

#include "CholeskyMatrix_real.h"

#pragma diag_suppress = Pm128,Pm136,Pm152,Pm154

#define MIN_TOL  (0.0f)
#define l(i,j)   ( _l[(i*nSize)+(j)] )
#define b(i,j)   ( _b[(i*mSize)+(j)] )
#define B(i)     ( _b[(i*mSize)] )
#define z(i)     ( _z[(i*mSize)] )

/**
 ******************************************************************************* 
 * Function: CholeskyMatrix_real
 *
 * @brief    Solve a set of real linear equation with a symmetric matrix
 *
 * @details 
 *          Az = b
 *          A = LL*
 *          A matrix will be overwritten by corresponding L
 *
 * Parameters: 
 * @param      [in] _l      - pointer to matrix A
 * @param      [in] b       - pointer to column b 
 * @param      [in] nSize   - length of column and matrix size
 * @param      [out] z      - pointer to souput column
 *
 * @return     None
 *
 *******************************************************************************
*/ 
void CholeskyMatrix_real(double *_l, const double *b, uint32_t nSize, double *z)
{
#ifdef MATLAB_BUILD
    MEX_LOG("L Matrix before decomposition:\n");
    for(int i = 0; i < nSize; i++)
    {
        for(int j = 0; j < nSize; j++)
        {
            MEX_LOG("%5.3f\t", _l[i * nSize + j]);
        }
        MEX_LOG("\n");
    }
#endif

    /* Perform Cholesky Decomposition*/
    CholeskyMatrix_Decompose(_l, nSize);

#ifdef MATLAB_BUILD
    MEX_LOG("L Matrix after decomposition:\n");
    for(int i = 0; i < nSize; i++)
    {
        for(int j = 0; j < nSize; j++)
        {
            MEX_LOG("%5.3f\t", _l[i * nSize + j]);
        }
        MEX_LOG("\n");
    }
    MEX_LOG("B vector:\n");
    for(int i = 0; i < nSize; i++)
    {
        MEX_LOG("%5.3f\t", b[i]);
    }
    MEX_LOG("\n");
#endif

    /* find the solution */
    CholeskyMatrix_solve(_l, b, z, nSize, 1u);

}

/**
 ******************************************************************************* 
 * Function: CholeskyMatrix_solve
 *
 * @brief    Solve a set of real linear equation  with cholesky factor
 *
 * @details 
 * 
 *          l -> lower triangular matrix square matrix of size nSize * nSize
 *          b -> First element in the column of matrix with size nSize * mSize
 *          z -> First element in the column of matrix with size nSize * mSize
 *
 *          Solves the Az = b with A = LL*
 *          where z is the column of the output matrix
 *          and b is also a column of the input matrix
 *
 * Parameters: 
 * @param      [in] _l      - pointer to matrix A
 * @param      [in] b       - pointer to column b 
 * @param      [out] z      - pointer to souput column
 * @param      [in] nSize   - size of square L matrix 
 * @param      [in] mSize   - No of cols in the matrix corresbonfing to B and Z 
 *
 * @return     None
 *
 *******************************************************************************
*/ 
void CholeskyMatrix_solve(const double *_l, const double *_b, double *_z, uint32_t nSize, uint32_t mSize)
{
    
    /*Solving the linear system: Ax=b -> LL*x=b, defining L*x as a new vector y,
    First, solve Ly=b (forward substituion)
    Second, solve Lx=y (backward substituion)  */
    
    /* Use forward substituion to solve L*y = b */
    for (uint32_t i = 0; i < nSize; i++) 
    {
        if (l(i, i) > MIN_TOL)
        {
            double s = B(i);
            for (uint32_t k = 0; k < i; k++)
            {
                s -= (l(i, k) * z(k));
            }
            double temp = 1.0 / l(i, i);
            z(i) = (double)(temp * s);
        }
        else
        {
            z(i) = 0.0f;
        }
    }
    
    /* Use backward substituion to solve Lx = y */
    uint32_t cnt = 0u;
    for (int32_t i = (int32_t)(nSize - 1u); i >= 0; i--) 
    {
        cnt += 1;
        if (cnt > nSize) 
        {
            break;
        }
        double s = z(i);
        for (uint32_t k = (uint32_t)(nSize - 1u); k > (uint32_t)i; k--) 
        {
            s -= (l(k, i)*z(k));
        }
        if (l(i, i) > MIN_TOL)
        {
            double temp = 1.0 / l(i, i);
            z(i) = (double)(temp * s);
        }
        else
        {
            z(i) = 0.0f;
        }
    }
}

/**
 ******************************************************************************* 
 * Function: CholeskyMatrix_Decompose
 *
 * @brief    decomposes the symmetric matrix into lower triangular cholesky factor
 *
 * @details 
 * 
 *
 * Parameters: 
 * @param      [in/out] _l      - pointer to matrix A
 * @param      [in] nSize       - size of square L matrix 
 *
 * @return     None
 *
 *******************************************************************************
*/ 
void CholeskyMatrix_Decompose(double *_l, uint32_t nSize)
{
    
    /* Perform Cholesky Decomposition*/
    
    
    /*Step 1: setting the upper elements of the input coefficient matrix to zero*/
    for (uint32_t i = 0; i < nSize; i++)
    {
        for (uint32_t j = i + 1; j < nSize; j++)
        {
            l(i, j) = 0.0f;
        }
    }
    
    
    /*Calculating the entries of the lower triangular matrix (Cholesky–Banachiewicz algorithm)*/
    for (uint32_t i = 0; i < nSize; i++)
    {       
        for (uint32_t j = 0; j < i; j++)
        {
            if (l(j, j) > MIN_TOL)
            {
                double s = 0.0f;
                s = l(i, j);
                for (uint32_t k = 0; k < j; k++)
                {
                    s -= (l(i, k)*l(j, k));
                }
                double temp = 1.0 / l(j, j);
                l(i, j) = (double)(temp * s);
            }
        }
        double s = 0.0f;
        s = l(i, i);
        for (uint32_t k = 0; k < i; k++) /* for (uint32_t k = 0; k < j; k++) */
        {
            s -= powf(l(i, k), 2);
        }
        l(i, i) = sqrtf(s);
    }
    
}

/**
 ******************************************************************************* 
 * Function: CholeskyMatrix_mlsolve
 *
 * @brief    gives the slution to inv(A)*B
 *
 * @details 
 *
 *          mldivide(A, B) = INV(A)*B, solution to A*C = B, C = A\B
 *          For every column of C (c) we can have corresponding column in B (b)
 *          Hence in loop we need to solve the Ac = b
 *          Given A is symmetric we have it's cholesky factor as 
 *          A = LL'
 *          So instead of saving A we will only store L and operate on that
 *
 *          _l corresponds to input matrix A
 *          the matrix A will be overwritten by corresponding L matrix
 *
 *          A -> square matrix of size nSize * nSize
 *          B -> matrix of size nSize * mSize
 *          C -> matrix of size nSize * mSize
 *
 * Parameters: 
 * @param      [in] _l      - pointer to matrix A
 * @param      [in] _b      - pointer to matrix B 
 * @param      [out] _z     - pointer to ouput matrix Z
 * @param      [in] nSize   - size of square A matrix and rows of B
 * @param      [in] mSize   - number of columns in B and Z
 *
 * @return     None
 *
 *******************************************************************************
*/ 
void CholeskyMatrix_mlsolve(const double *_l, const double *_b, double *_z, uint32_t nSize, uint32_t mSize)
{

    for (uint32_t i = 0u; i < mSize; i++)
    {
        /* find the solution */
        CholeskyMatrix_solve(_l, &(_b[i]), &(_z[i]), nSize, mSize);
    }

}

/**
 ******************************************************************************* 
 * Function: CholeskyMatrix_mldivide
 *
 * @brief    Combined with CholeskyMatrix_Decompose, gives the solution to inv(A)*B
 *
 * @details 
 *
 *          mldivide(A, B) = INV(A)*B, solution to A*C = B, C = A\B
 *          For every column of C (c) we can have corresponding column in B (b)
 *          Hence in loop we need to solve the Ac = b
 *          Given A is symmetric we have it's cholesky factor as 
 *          A = LL'
 *          So instead of saving A we will only store L and operate on that
 *
 *          _l corresponds to cholesky factor of input matrix A
 *
 *          A -> square matrix of size nSize * nSize
 *          B -> matrix of size nSize * mSize
 *          C -> matrix of size nSize * mSize
 *
 * Parameters: 
 * @param      [in] _l      - pointer to matrix A
 * @param      [in] _b      - pointer to matrix B 
 * @param      [out] _z     - pointer to ouput matrix Z
 * @param      [in] nSize   - size of square A matrix and rows of B
 * @param      [in] mSize   - number of columns in B and Z
 *
 * @return     None
 *
 *******************************************************************************
*/ 
void CholeskyMatrix_mldivide(double *_l, const double *_b, double *_z, uint32_t nSize, uint32_t mSize)
{
    /* Perform Cholesky Decomposition */
    CholeskyMatrix_Decompose(_l, nSize);

    /* Perform Cholesky forward solution */
    CholeskyMatrix_mlsolve(_l, _b, _z, nSize, mSize);

}

/**
 ******************************************************************************* 
 * Function: CholeskyMatrix_mrsolve
 *
 * @brief    Combined with CholeskyMatrix_Decompose, gives the solution to A*INV(B)
 *
 * @details 
 *
 *          mrdivide(A, B) = A*INV(B) = Z
 *
 *          The final proof shows that the mrdivide solution can be obtained as
 *          doing solving B*y = A', implies cosider the column of A as a row
 *          and then final result column is row in the output z = y'
 *          Hence, solving the linear equation B*y = a where row of A will be treated as column
 *          and the resulting column will be stored in row of matrix Z
 *
 *          A -> mSize*nSize
 *          B -> nSize*nSize
 *          Z -> mSize*nSize
 *          B is assumed to be symmetric and cholesky factorization used
 *
 * Parameters: 
 * @param      [in] _l      - pointer to matrix A
 * @param      [in] _b      - pointer to matrix B 
 * @param      [out] _z     - pointer to ouput matrix Z
 * @param      [in] nSize   - size of square A matrix and rows of B
 * @param      [in] mSize   - number of columns in B and Z
 *
 * @return     None
 *
 *******************************************************************************
*/ 
void CholeskyMatrix_mrsolve(const double *_a, double *_b, double *_z, uint32_t nSize, uint32_t mSize)
{
    for (uint32_t i = 0u; i < mSize; i++)
    {
        /* find the solution */
        CholeskyMatrix_solve(_b, &(_a[i*nSize]), &(_z[i*nSize]), nSize, 1u);
    }

}

/**
 ******************************************************************************* 
 * Function: CholeskyMatrix_mrdivide
 *
 * @brief    gives the slution to A*INV(B)
 *
 * @details 
 *
 *          mrdivide(A, B) = A*INV(B) = Z
 *              solution to Z*B = A, Z = A/B
 *              computed as A/B = (B'\A')' i.e A*inv(B) = (inv(B') * A')*
 *             Proof:
 *                   A*INV(B) = ((A*inv(B))')' double transpose
 *                            = (inv(B)' * A')'
 *                            = (inv(B') * A')'
 *                            = (inv(B) * A')'
 *
 *          The final proof shows that the mrdivide solution can be obtained as
 *          doing solving B*y = A', implies cosider the column of A as a row
 *          and then final result column is row in the output z = y'
 *          Hence, solving the linear equation B*y = a where row of A will be treated as column
 *          and the resulting column will be stored in row of matrix Z
 *
 *          A -> mSize*nSize
 *          B -> nSize*nSize
 *          Z -> mSize*nSize
 *          B is assumed to be symmetric and cholesky factorization used
 *
 * Parameters: 
 * @param      [in] _a      - pointer to matrix A
 * @param      [in] _b      - pointer to matrix B 
 * @param      [out] _z     - pointer to ouput matrix Z
 * @param      [in] nSize   - size of square B matrix and columns of A
 * @param      [in] mSize   - number of rows in A and Z
 *
 * @return     None
 *
 *******************************************************************************
*/ 
void CholeskyMatrix_mrdivide(const double *_a, double *_b, double *_z, uint32_t nSize, uint32_t mSize)
{

    /* Perform Cholesky Decomposition */
    CholeskyMatrix_Decompose(_b, nSize);

    /* Perform Cholesky backward solution */
    CholeskyMatrix_mrsolve(_a, _b, _z, nSize, mSize);
}
