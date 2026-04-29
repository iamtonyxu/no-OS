/*! ****************************************************************************
*
* @file:    complex_wrapper_t.h
*
* @brief:   Wrapper around <complex.h> to support both standard and Microsoft
*           versions of <complex.h>
*
* @details:
*
* @date:    $Date:
*
*******************************************************************************
*/

#ifndef __COMPLEX_WRAPPER_T_H__
#define __COMPLEX_WRAPPER_T_H__

#include <stdint.h>
#include <complex.h>

#ifdef _MSC_VER

/* 
 * The Microsoft compiler does not support the full
 * <complex.h> standard
 */
typedef _Fcomplex ComplexFloat_t;
typedef _Dcomplex ComplexDouble_t;

#define ComplexFloat(a,b)    (_FCbuild(a, b))
#define caddf(a,b)           (_FCbuild(crealf(a) + crealf(b), cimagf(a) + cimagf(b)))
#define csubf(a,b)           (_FCbuild(crealf(a) - crealf(b), cimagf(a) - cimagf(b)))
#define cmulf(a,b)           (_FCmulcc((a),(b)))
#define cdivf(a,b)           (_FCmulcr(_FCmulcc(a, conjf(b)), 1.0f / normf(b)))

#define ComplexDouble(a,b)   (_Cbuild(a, b))
#define cadd(a,b)            (_Cbuild(creal(a) + creal(b), cimag(a) + cimag(b)))
#define csub(a,b)            (_Cbuild(creal(a) - creal(b), cimag(a) - cimag(b)))
#define cmul(a,b)            (_Cmulcc((a),(b)))
#define cdiv(a,b)            (_Cmulcr(_Cmulcc(a, conj(b)), 1.0 / norm(b)))

#define craddf(a,b)          (_FCbuild(crealf(a) + b, cimagf(a)))
#define crsubf(a,b)          (_FCbuild(crealf(a) - b, cimagf(a)))
#define crmulf(a,b)          (_FCmulcr((a),(b)))
#define crdivf(a,b)          (_FCmulcr((a), 1.0f/(b)))

#define cradd(a,b)           (_Cbuild(creal(a) + b, cimag(a)))
#define crsub(a,b)           (_Cbuild(creal(a) - b, cimag(a)))
#define crmul(a,b)           (_Cmulcr((a),(b)))
#define crdiv(a,b)           (_Cmulcr((a), 1.0/(b)))

#define rcaddf(a,b)          (craddf((b),(a)))
#define rcsubf(a,b)          (_FCbuild((a) - crealf((b)), -cimagf((b))))
#define rcmulf(a,b)          (crmulf((b),(a)))
#define rcdivf(a,b)          (crmulf(rcmulf((a), conjf(b)), 1.0f / normf(b)))

#define rcadd(a,b)           (cradd((b),(a)))
#define rcsub(a,b)           (_Cbuild((a) - creal((b)), -cimag((b))))
#define rcmul(a,b)           (crmul((b),(a)))
#define rcdiv(a,b)           (crmul(rcmul((a), conj(b)), 1.0 / norm(b)))
    
#else

/* assume standard compliance */

typedef float complex ComplexFloat_t;
typedef double complex ComplexDouble_t;

#define ComplexFloat(a,b)   ((ComplexFloat_t)(a) + I*(ComplexFloat_t)(b))
#define caddf(a,b)          ( (a) + (b) )
#define csubf(a,b)          ( (a) - (b) )
#define cmulf(a,b)          ( (a) * (b) )
#define cdivf(a,b)          ( (a) / (b) ) 

#define cadd(a,b)           ( (a) + (b) )
#define csub(a,b)           ( (a) - (b) )
#define cmul(a,b)           ( (a) * (b) )
#define cdiv(a,b)           ( (a) / (b) )

#define craddf(a,b)         ( (a) + (b) )
#define crsubf(a,b)         ( (a) - (b) )
#define crmulf(a,b)         ( (a) * (b) )
#define crdivf(a,b)         ( (a) / (b) )

#define ComplexDouble(a,b)  ((ComplexDouble_t)(a) + I*(ComplexDouble_t)(b))
#define cradd(a,b)          ( (a) + (b) )
#define crsub(a,b)          ( (a) - (b) )
#define crmul(a,b)          ( (a) * (b) )
#define crdiv(a,b)          ( (a) / (b) )

#define rcaddf(a,b)         ( (a) + (b) )
#define rcsubf(a,b)         ( (a) - (b) )
#define rcmulf(a,b)         ( (a) * (b) )
#define rcdivf(a,b)         ( (a) / (b) )

#define rcadd(a,b)          ( (a) + (b) )
#define rcsub(a,b)          ( (a) - (b) )
#define rcmul(a,b)          ( (a) * (b) )
#define rcdiv(a,b)          ( (a) / (b) )

#define normf(a)            (crealf(a)*crealf(a) + cimagf(a)*cimagf(a))
#define norm(a)             (creal(a)*creal(a) + cimag(a)*cimag(a))
    
#endif

/*
 * Now with ComplexFloat_t and ComplexDouble_t, we can use the
 * standard library functions (e.g. cexp, cabs, creal, cimag, conj, etc.)
 */

#endif /* __COMPLEX_WRAPPER_T_H__ */
