/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file FLINTconvert.h
 *
 * This file defines functions for conversion to FLINT (www.flintlib.org)
 * and back.
 *
 * @author Martin Lee
 *
 **/
/*****************************************************************************/

#ifndef FLINT_CONVERT_H
#define FLINT_CONVERT_H

// #include "config.h"
#include "canonicalform.h"
#include "fac_util.h"

#ifdef HAVE_FLINT
#ifdef __cplusplus
extern "C"
{
#endif
#ifndef __GMP_BITS_PER_MP_LIMB
#define __GMP_BITS_PER_MP_LIMB GMP_LIMB_BITS
#endif
#include <flint/fmpz.h>
#include <flint/fmpq.h>
#include <flint/fmpz_poly.h>
#include <flint/fmpz_mod_poly.h>
#include <flint/fmpq_poly.h>
#include <flint/nmod_poly.h>
#include <flint/nmod_mat.h>
#include <flint/fmpz_mat.h>
#if ( __FLINT_RELEASE >= 20400)
#include <flint/fq.h>
#include <flint/fq_poly.h>
#include <flint/fq_nmod.h>
#include <flint/fq_nmod_poly.h>
#include <flint/fq_nmod_mat.h>
#if ( __FLINT_RELEASE >= 20503)
#include <flint/fmpq_mpoly.h>
#endif
#if ( __FLINT_RELEASE >= 20700)
#include <flint/fq_nmod_mpoly_factor.h>
#include <flint/fmpz_mod.h>
#endif
#if ( __FLINT_RELEASE >= 30000)
#include <flint/nmod.h>
#include <flint/nmod_mpoly.h>
#include <flint/fmpz_vec.h>
#endif
#endif

EXTERN_VAR flint_rand_t FLINTrandom;

#ifdef __cplusplus
}
#endif

#include "factory/cf_gmp.h"

/// conversion of a factory integer to fmpz_t
void
convertCF2Fmpz (fmpz_t result,         ///< [out] an fmpz_t
                const CanonicalForm& f ///< [in] a CanonicalForm wrapping an
                                       ///< integer
               );

/// conversion of a factory integer to fmpz_t(init.)
void
convertCF2initFmpz (fmpz_t result,     ///< [in,out] an fmpz_t
                const CanonicalForm& f ///< [in] a CanonicalForm wrapping an
                                       ///< integer
               );

/// conversion of a factory univariate polynomial over Z to a fmpz_poly_t
void
convertFacCF2Fmpz_poly_t (fmpz_poly_t result,    ///< [in,out] an fmpz_poly_t
                          const CanonicalForm& f ///< [in] univariate poly over
                                                 ///< Z
                         );

/// conversion of a FLINT integer to CanonicalForm
CanonicalForm
convertFmpz2CF (const fmpz_t coefficient ///< [in] a FLINT integer
               );

/// conversion of a FLINT poly over Z to CanonicalForm
CanonicalForm
convertFmpz_poly_t2FacCF (const fmpz_poly_t poly, ///< [in] an fmpz_poly_t
                          const Variable& x ///< [in] variable the result should
                                            ///< have
                         );

/// conversion of a factory univariate polynomials over Z/p (for word size p)
/// to nmod_poly_t
void
convertFacCF2nmod_poly_t (nmod_poly_t result,    ///< [in, out] a nmod_poly_t
                          const CanonicalForm& f ///< [in] univariate poly over
                                                 ///< Z/p
                         );

/// conversion of a FLINT poly over Z/p to CanonicalForm
CanonicalForm
convertnmod_poly_t2FacCF (const nmod_poly_t poly, ///< [in] a nmod_poly_t
                          const Variable& x ///< [in] variable the result should
                                            ///< have
                         );

/// conversion of a factory rationals to fmpq_t
void
convertCF2Fmpq (fmpq_t result,         ///< [in,out] an fmpq_t
                const CanonicalForm& f ///< [in] a CanonicalForm wrapping a
                                       ///< rational
               );

/// conversion of a FLINT rational to CanonicalForm
CanonicalForm convertFmpq2CF (const fmpq_t q);

/// conversion of a factory univariate polynomials over Q to fmpq_poly_t
void
convertFacCF2Fmpq_poly_t (fmpq_poly_t result,    ///< [in,out] an fmpq_poly_t
                          const CanonicalForm& f ///< [in] univariate poly over
                                                 ///< Q
                         );

/// conversion of a FLINT poly over Q to CanonicalForm
CanonicalForm
convertFmpq_poly_t2FacCF (const fmpq_poly_t p,    ///< [in] an fmpq_poly_t
                          const Variable& x ///< [in] variable the result should
                                            ///< have
                         );

/// conversion of a FLINT factorization over Z/p (for word size p) to a
/// CFFList
CFFList
convertFLINTnmod_poly_factor2FacCFFList (
                   const nmod_poly_factor_t fac, ///< [in] a nmod_poly_factor_t
                   const mp_limb_t leadingCoeff, ///< [in] leading coefficient
                   const Variable& x       ///< [in] variable the result should
                                           ///< have
                                        );

/// conversion of a FLINT factorization over Z to a CFFList
CFFList
convertFLINTfmpz_poly_factor2FacCFFList (
                   const fmpz_poly_factor_t fac, ///< [in] a fmpz_poly_factor_t
                   const Variable& x       ///< [in] variable the result should
                                           ///< have
                                        );


#if ( __FLINT_RELEASE >= 20700)
/// conversion of a FLINT factorization over Zp(a) to a CFFList
CFFList
convertFLINTFq_nmod_mpoly_factor2FacCFFList (
                   fq_nmod_mpoly_factor_t fac,    ///< [in] a fq_nmod_mpoly_factor_t
                   const fq_nmod_mpoly_ctx_t& ctx,///< [in] context
                   const int N,                   ///< [in] #vars
                   const fq_nmod_ctx_t& fq_ctx,   ///< [in] fq context
                   const Variable alpha           ///< [in] alpha
                                        );
#endif

/// conversion of a factory univariate poly over Z to a FLINT poly over
/// Z/p (for non word size p)
void
convertFacCF2Fmpz_mod_poly_t (
                          fmpz_mod_poly_t result, ///< [in,out] fmpz_mod_poly_t
                          const CanonicalForm& f, ///< [in] univariate poly over
                                                  ///< Z
                          const fmpz_t p          ///< [in] some integer p
                             );

/// conversion of a FLINT poly over Z/p (for non word size p) to a CanonicalForm
/// over Z
CanonicalForm
convertFmpz_mod_poly_t2FacCF (
                          const fmpz_mod_poly_t poly, ///< [in] fmpz_mod_poly_t
                          const Variable& x,    ///< [in] variable the result
                                                ///< should have
                          const modpk& b        ///< [in] coeff bound to map
                                                ///< coeffs in (-p/2,p/2)
                             );

#if __FLINT_RELEASE >= 20400
/// conversion of a FLINT element of F_q to a CanonicalForm with alg. variable
/// alpha
CanonicalForm
convertFq_nmod_t2FacCF (const fq_nmod_t poly, ///< [in] fq_nmod_t
                        const Variable& alpha, ///< [in] algebraic variable
                        const fq_nmod_ctx_t ctx ///<[in] context
                       );

/// conversion of a FLINT element of F_q with non-word size p to a CanonicalForm
/// with alg. variable alpha
CanonicalForm
convertFq_t2FacCF (const fq_t poly,      ///< [in] fq_t
                   const Variable& alpha ///< [in] algebraic variable
                  );

/// conversion of a factory element of F_q to a FLINT fq_nmod_t, does not do any
/// memory allocation for poly
void
convertFacCF2Fq_nmod_t (fq_nmod_t result,       ///< [in,out] fq_nmod_t
                        const CanonicalForm& f, ///< [in] element of Fq
                        const fq_nmod_ctx_t ctx ///< [in] Fq context
                       );

#if ( __FLINT_RELEASE >= 20700)
/// conversion of a factory polynomial over of F_q to a FLINT fq_nmod_mpoly_t, does not do any
/// memory allocation for poly
void
convertFacCF2Fq_nmod_mpoly_t (fq_nmod_mpoly_t result, ///< [in,out] fq_nmod_mpoly__t
                        const CanonicalForm& f,       ///< [in] poly over Fq
                        const fq_nmod_mpoly_ctx_t ctx,///< [in] context
                        const int N,                  ///< [in] #vars
                        const fq_nmod_ctx_t fq_ctx    ///< [in] fq context
                       );
#endif

/// conversion of a factory element of F_q (for non-word size p) to a FLINT fq_t
void
convertFacCF2Fq_t (fq_t result,            ///< [in,out] fq_t
                   const CanonicalForm& f, ///< [in] element of Fq
                   const fq_ctx_t ctx      ///< [in] Fq context
                  );

/// conversion of a factory univariate poly over F_q (for non-word size p) to a
/// FLINT fq_poly_t
void
convertFacCF2Fq_poly_t (fq_poly_t result,      ///< [in,out] fq_poly_t
                        const CanonicalForm& f,///< [in] univariate poly over Fq
                        const fq_ctx_t ctx     ///< [in] Fq context
                       );

/// conversion of a factory univariate poly over F_q to a FLINT fq_nmod_poly_t
void
convertFacCF2Fq_nmod_poly_t (fq_nmod_poly_t result, ///< [in,out] fq_nmod_poly_t
                             const CanonicalForm& f,///< [in] univariate poly
                                                    ///< over Fq
                             const fq_nmod_ctx_t ctx///< [in] Fq context
                            );

/// conversion of a FLINT poly over Fq (for non-word size p) to a CanonicalForm
/// with alg. variable alpha and polynomial variable x
CanonicalForm
convertFq_poly_t2FacCF (const fq_poly_t p,     ///< [in] fq_poly_t
                        const Variable& x,     ///< [in] polynomial variable
                        const Variable& alpha, ///< [in] algebraic variable
                        const fq_ctx_t ctx     ///< [in] Fq context
                       );

/// conversion of a FLINT poly over Fq to a CanonicalForm with alg. variable
/// alpha and polynomial variable x
CanonicalForm
convertFq_nmod_poly_t2FacCF (const fq_nmod_poly_t p, ///< [in] fq_nmod_poly_t
                             const Variable& x,      ///< [in] polynomial var.
                             const Variable& alpha,  ///< [in] algebraic var.
                             const fq_nmod_ctx_t ctx ///< [in] Fq context
                            );
#endif

/// conversion of a factory matrix over Z to a fmpz_mat_t
void convertFacCFMatrix2Fmpz_mat_t (fmpz_mat_t M,      ///<[in,out] fmpz_mat_t
                                    const CFMatrix &m  ///<[in] matrix over Z
                                   );

/// conversion of a FLINT matrix over Z to a factory matrix
CFMatrix* convertFmpz_mat_t2FacCFMatrix(const fmpz_mat_t m ///<[in] fmpz_mat_t
                                       );

/// conversion of a factory matrix over Z/p to a nmod_mat_t
void convertFacCFMatrix2nmod_mat_t (nmod_mat_t M,     ///<[in,out] nmod_mat_t
                                    const CFMatrix &m ///<[in] matrix over Z/p
                                   );

/// conversion of a FLINT matrix over Z/p to a factory matrix
CFMatrix* convertNmod_mat_t2FacCFMatrix(const nmod_mat_t m ///<[in] nmod_mat_t
                                       );

#if __FLINT_RELEASE >= 20400
/// conversion of a FLINT matrix over F_q to a factory matrix
CFMatrix*
convertFq_nmod_mat_t2FacCFMatrix(const fq_nmod_mat_t m,       ///< [in] fq_nmod_mat_t
                                 const fq_nmod_ctx_t& fq_con, ///< [in] Fq context
                                 const Variable& alpha ///< [in] algebraic variable
                                );

/// conversion of a factory matrix over F_q to a fq_nmod_mat_t
void
convertFacCFMatrix2Fq_nmod_mat_t (fq_nmod_mat_t M,            ///< [in, out] fq_nmod_mat_t
                                  const fq_nmod_ctx_t fq_con, ///< [in] Fq context
                                  const CFMatrix &m           ///< [in] matrix over Fq
                                 );

/// conversion of a FLINT factorization over Fq (for word size p) to a
/// CFFList
CFFList
convertFLINTFq_nmod_poly_factor2FacCFFList (const fq_nmod_poly_factor_t fac, ///< [in] fq_nmod_poly_factor_t
                                          const Variable& x,     ///< [in] polynomial variable
                                          const Variable& alpha, ///< [in] algebraic variable
                                          const fq_nmod_ctx_t fq_con ///< [in] Fq context
                                           );
#endif


#if __FLINT_RELEASE >= 20503
CanonicalForm mulFlintMP_Zp(const CanonicalForm& F,int lF, const CanonicalForm& Gi, int lG, int m);
CanonicalForm mulFlintMP_QQ(const CanonicalForm& F,int lF, const CanonicalForm& Gi, int lG, int m);
CanonicalForm gcdFlintMP_Zp(const CanonicalForm& F, const CanonicalForm& G);
CanonicalForm gcdFlintMP_QQ(const CanonicalForm& F, const CanonicalForm& G);
void convFactoryPFlintMP ( const CanonicalForm & f, nmod_mpoly_t res, nmod_mpoly_ctx_t ctx, int N );
void convFactoryPFlintMP ( const CanonicalForm & f, fmpq_mpoly_t res, fmpq_mpoly_ctx_t ctx, int N );
void convFactoryPFlintMP ( const CanonicalForm & f, fmpz_mpoly_t res, fmpz_mpoly_ctx_t ctx, int N );
CanonicalForm convFlintMPFactoryP(nmod_mpoly_t f, nmod_mpoly_ctx_t ctx, int N);
CanonicalForm convFlintMPFactoryP(fmpq_mpoly_t f, fmpq_mpoly_ctx_t ctx, int N);
CanonicalForm convFlintMPFactoryP(fmpz_mpoly_t f, fmpz_mpoly_ctx_t ctx, int N);
#endif // FLINT 2.5.3
#if __FLINT_RELEASE >= 20700
CanonicalForm
convertFq_nmod_mpoly_t2FacCF (const fq_nmod_mpoly_t poly,    ///< [in]
                              const fq_nmod_mpoly_ctx_t& ctx,///< [in] context
                              const int N,                   ///< [in] #vars
                              const fq_nmod_ctx_t& fq_ctx,   ///< [in] fq context
                              const Variable alpha           ///< [in] alpha
                         );
#endif // FLINT2.7.0
#endif // FLINT
#endif

// emacs edit mode for this file is -*- C++ -*-
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: convert data between Singular and Flint
*/



#include "misc/auxiliary.h"
#include "flintconv.h"

#ifdef HAVE_FLINT
#if __FLINT_RELEASE >= 20500

#include "coeffs/coeffs.h"
#include "coeffs/longrat.h"
#include "coeffs/rintegers.h"
#include "polys/monomials/p_polys.h"

#include "polys/sbuckets.h"
//#include "polys/clapconv.h"

#include "simpleideals.h"


int convFlintISingI (fmpz_t f)
{
  //return fmpz_get_si(f);
  return (int)*f;
}

void convSingIFlintI(fmpz_t f, int p)
{
  fmpz_init(f);
  *f=p;
  //fmpz_set_si(f,p);
  return;
}

void convFlintNSingN (mpz_t z, fmpz_t f)
{
  mpz_init(z);
  fmpz_get_mpz(z,f);
}

number convFlintNSingN (fmpz_t f)
{
#if __FLINT_RELEASE > 20502
  number z;
  if(COEFF_IS_MPZ(*f))
    nlMPZ(COEFF_TO_PTR(*f),z,NULL);
  else
  {
    mpz_t a;
    mpz_init(a);
    fmpz_get_mpz(a,f);
    nlMPZ(a,z,NULL);
    mpz_clear(a);
  }
  return z;
#else
  WerrorS("not implemented");
  return NULL;
#endif
}

number convFlintNSingN (fmpq_t f, const coeffs cf)
{
#if __FLINT_RELEASE > 20502
  if (getCoeffType(cf)==n_Q) /* QQ, bigint */
  {
    return convFlintNSingN_QQ(f,cf);
  }
  else
  {
    number z;
    mpz_t a,b;
    mpz_init(a);
    mpz_init(b);
    fmpq_get_mpz_frac(a,b,f);
    if (mpz_cmp_si(b,1L)!=0)
    {
      number na=n_InitMPZ(a,cf);
      number nb=n_InitMPZ(b,cf);
      z=n_Div(na,nb,cf);
      n_Delete(&nb,cf);
      n_Delete(&na,cf);
      n_Normalize(z,cf);
    }
    else
    {
      z=n_InitMPZ(a,cf);
    }
    mpz_clear(a);
    mpz_clear(b);
    n_Test(z,cf);
    return z;
  }
#else
  WerrorS("not implemented");
  return NULL;
#endif
}

number convFlintNSingN (fmpz_t f, const coeffs cf)
{
#if __FLINT_RELEASE > 20502
  number z;
  if(COEFF_IS_MPZ(*f))
    z=n_InitMPZ(COEFF_TO_PTR(*f),cf);
  else if (cf->rep==n_rep_gmp)
  {
    z=nrzInit(1,NULL); // alloc and initialization
    fmpz_get_mpz((mpz_ptr)z,f);
  }
  else
  {
    if (fmpz_fits_si(f))
    {
      long i=fmpz_get_si(f);
      z=n_Init(i,cf);
    }
    else
    {
      mpz_t a;
      mpz_init(a);
      fmpz_get_mpz(a,f);
      z=n_InitMPZ(a,cf);
      mpz_clear(a);
    }
  }
  return z;
#else
  WerrorS("not implemented");
  return NULL;
#endif
}

number convFlintNSingN_QQ (fmpq_t f, const coeffs cf)
{
#if __FLINT_RELEASE > 20502
  if (fmpz_is_one(fmpq_denref(f)))
  {
    if (fmpz_fits_si(fmpq_numref(f)))
    {
      long i=fmpz_get_si(fmpq_numref(f));
      return n_Init(i,cf);
    }
  }
  number z=ALLOC_RNUMBER();
  #if defined(LDEBUG)
  z->debug=123456;
  #endif
  mpz_init(z->z);
  if (fmpz_is_one(fmpq_denref(f)))
  {
    z->s=3;
    fmpz_get_mpz(z->z,fmpq_numref(f));
  }
  else
  {
    z->s=0;
    mpz_init(z->n);
    fmpq_get_mpz_frac(z->z,z->n,f);
  }
  return z;
#else
  WerrorS("not implemented");
  return NULL;
#endif
}

void convSingNFlintN(fmpz_t f, mpz_t n)
{
  fmpz_init(f);
  fmpz_set_mpz(f,n);
}

void convSingNFlintN(fmpz_t f, number n)
{
  fmpz_init(f);
  fmpz_set_mpz(f,(mpz_ptr)n);
}

void convSingNFlintN(fmpq_t f, number n, const coeffs cf)
{
  if (LIKELY(getCoeffType(cf)==n_Q)) /* QQ, bigint */
  {
    fmpq_init(f);
    if (SR_HDL(n)&SR_INT)
      fmpq_set_si(f,SR_TO_INT(n),1);
    else if (n->s<3)
    {
      fmpz_set_mpz(fmpq_numref(f), n->z);
      fmpz_set_mpz(fmpq_denref(f), n->n);
    }
    else
    {
      fmpz_set_mpz(fmpq_numref(f), n->z);
      fmpz_set_si(fmpq_denref(f), 1);
    }
  }
  else
  {
    coeffs QQ=nInitChar(n_Q,NULL);
    nMapFunc nMap=n_SetMap(cf,QQ);
    if (nMap!=NULL)
    {
      number nn=nMap(n,cf,QQ);
      convSingNFlintN(f,nn,QQ);
    }
    nKillChar(QQ);
  }
}

void convSingNFlintN_QQ(fmpq_t f, number n)
{
  fmpq_init(f);
  if (SR_HDL(n)&SR_INT)
    fmpq_set_si(f,SR_TO_INT(n),1);
  else if (n->s<3)
  {
    fmpz_set_mpz(fmpq_numref(f), n->z);
    fmpz_set_mpz(fmpq_denref(f), n->n);
  }
  else
  {
    mpz_t one;
    mpz_init_set_si(one,1);
    fmpz_set_mpz(fmpq_numref(f), n->z);
    fmpz_set_mpz(fmpq_denref(f), one);
    mpz_clear(one);
  }
}

void convSingNFlintNN(fmpq_t re, fmpq_t im, number n, const coeffs cf)
{
  number n_2=n_RePart(n,cf);
  convSingNFlintN(re,n_2,cf);
  n_Delete(&n_2,cf);
  n_2=n_ImPart(n,cf);
  convSingNFlintN(im,n_2,cf);
  n_Delete(&n_2,cf);
}

void convSingPFlintP(fmpq_poly_t res, poly p, const ring r)
{
  if (p==NULL)
  {
    fmpq_poly_init(res);
    return;
  }
  int d=p_GetExp(p,1,r);
  fmpq_poly_init2(res,d+1);
  _fmpq_poly_set_length (res, d + 1);
  while(p!=NULL)
  {
    number n=pGetCoeff(p);
    fmpq_t c;
    convSingNFlintN(c,n,r->cf);
    fmpq_poly_set_coeff_fmpq(res,p_GetExp(p,1,r),c);
    fmpq_clear(c);
    pIter(p);
  }
}

void convSingImPFlintP(fmpq_poly_t res, poly p, const ring r)
{
  int d=p_GetExp(p,1,r);
  fmpq_poly_init2(res,d+1);
  _fmpq_poly_set_length (res, d + 1);
  while(p!=NULL)
  {
    number n=n_ImPart(pGetCoeff(p),r->cf);
    fmpq_t c;
    convSingNFlintN(c,n,r->cf);
    fmpq_poly_set_coeff_fmpq(res,p_GetExp(p,1,r),c);
    fmpq_clear(c);
    n_Delete(&n,r->cf);
    pIter(p);
  }
}

poly convFlintPSingP(fmpq_poly_t f, const ring r)
{
  if (fmpq_poly_is_zero(f)) return NULL;
  int d=fmpq_poly_length(f);
  poly p=NULL;
  fmpq_t c;
  fmpq_init(c);
  for(int i=0; i<=d; i++)
  {
    fmpq_poly_get_coeff_fmpq(c,f,i);
    number n=convFlintNSingN(c,r->cf);
    if(!n_IsZero(n,r->cf))
    {
      poly pp=p_Init(r);
      pSetCoeff0(pp,n);
      p_SetExp(pp,1,i,r);
      p_Setm(pp,r);
      p=p_Add_q(p,pp,r);
    }
  }
  fmpq_clear(c);
  p_Test(p,r);
  return p;
}

void convSingPFlintnmod_poly_t(nmod_poly_t result, const poly p, const ring r)
{
  // assume univariate, r->cf=Z/p
  nmod_poly_init2 (result,rChar(r),p_Deg(p,r));
  poly h=p;
  while(h!=NULL)
  {
    if (h==NULL)
      nmod_poly_set_coeff_ui(result,0,0);
    else
      nmod_poly_set_coeff_ui(result,p_GetExp(h,1,r),n_Int(pGetCoeff(h),r->cf)+rChar(r));
    pIter(h);
  }
}

void convSingMFlintFq_nmod_mat(matrix m, fq_nmod_mat_t M, const fq_nmod_ctx_t fq_con, const ring r)
{
  fq_nmod_mat_init (M, (long)MATROWS(m), (long) MATCOLS(m), fq_con);
  int i,j;
  for(i=MATROWS(m);i>0;i--)
  {
    for(j=MATCOLS(m);j>0;j--)
    {
      convSingPFlintnmod_poly_t (M->rows[i-1]+j-1, MATELEM(m,i,j),r);
    }
  }
}

poly convFlintFq_nmodSingP(const fq_nmod_t Fp, const fq_nmod_ctx_t ctx, const ring r)
{
  poly p=NULL;
  poly h;
  for (int i= 0; i < nmod_poly_length (Fp); i++)
  {
    ulong coeff= nmod_poly_get_coeff_ui (Fp, i);
    if (coeff != 0)
    h=p_NSet(n_Init(coeff,r->cf),r);
    if (h!=NULL)
    {
      p_SetExp(h,1,i,r);
      p_Setm(h,r);
      p=p_Add_q(p,h,r);
    }
  }
  return p;
}

matrix convFlintFq_nmod_matSingM(fq_nmod_mat_t m, const fq_nmod_ctx_t fq_con, const ring r)
{
  matrix M=mpNew(fq_nmod_mat_nrows (m, fq_con),fq_nmod_mat_ncols (m, fq_con));
   int i,j;
  for(i=MATROWS(M);i>0;i--)
  {
    for(j=MATCOLS(M);j>0;j--)
    {
      MATELEM(M,i,j)=convFlintFq_nmodSingP(fq_nmod_mat_entry (m, i-1, j-1),
                                          fq_con, r);
    }
  }
  return M;
}

void convSingMFlintNmod_mat(matrix m, nmod_mat_t M, const ring r)
{
  nmod_mat_init (M, (long)MATROWS(m), (long) MATCOLS(m), rChar(r));
  int i,j;
  for(i=MATROWS(m);i>0;i--)
  {
    for(j=MATCOLS(m);j>0;j--)
    {
      poly h=MATELEM(m,i,j);
      if (h!=NULL)
        nmod_mat_entry(M,i-1,j-1)=(long)pGetCoeff(h);
    }
  }
}

matrix convFlintNmod_matSingM(nmod_mat_t m, const ring r)
{
  matrix M=mpNew(nmod_mat_nrows (m),nmod_mat_ncols (m));
   int i,j;
  for(i=MATROWS(M);i>0;i--)
  {
    for(j=MATCOLS(M);j>0;j--)
    {
      MATELEM(M,i,j)=p_ISet(nmod_mat_entry (m, i-1, j-1),r);
    }
  }
  return M;
}

matrix singflint_rref(matrix m, const ring R)
{
  int r=m->rows();
  int c=m->cols();
  int i,j;
  matrix M=NULL;
  if (rField_is_Q(R))
  {
    fmpq_mat_t FLINTM;
    fmpq_mat_init(FLINTM,r,c);
    M=mpNew(r,c);
    for(i=r;i>0;i--)
    {
      for(j=c;j>0;j--)
      {
        poly h=MATELEM(m,i,j);
        if (h!=NULL)
        {
          if (p_Totaldegree(h,R)==0)
            convSingNFlintN(fmpq_mat_entry(FLINTM,i-1,j-1),pGetCoeff(h),R->cf);
          else
          {
            WerrorS("matrix for rref is not constant");
            return M;
          }
        }
      }
    }
    fmpq_mat_rref(FLINTM,FLINTM);
    for(i=r;i>0;i--)
    {
      for(j=c;j>0;j--)
      {
        number n=convFlintNSingN(fmpq_mat_entry(FLINTM,i-1,j-1),R->cf);
        MATELEM(M,i,j)=p_NSet(n,R);
      }
    }
    fmpq_mat_clear(FLINTM);
  }
  else if (rField_is_Zp(R))
  {
    nmod_mat_t FLINTM;
    // convert matrix
    convSingMFlintNmod_mat(m,FLINTM,R);
    // rank
    long rk= nmod_mat_rref (FLINTM);
    M=convFlintNmod_matSingM(FLINTM,R);
    // clean up
    nmod_mat_clear(FLINTM);
  }
  else
  {
    WerrorS("not implemented for these coefficients");
  }
  return M;
}

ideal singflint_rref(ideal  m, const ring R) /*assume smatrix m*/
{
  int r=m->rank;
  int c=m->ncols;
  int i,j;
  ideal M=idInit(c,r);
  if (rField_is_Q(R))
  {
    fmpq_mat_t FLINTM;
    fmpq_mat_init(FLINTM,r,c);
    for(j=c-1;j>=0;j--)
    {
      poly h=m->m[j];
      while(h!=NULL)
      {
        i=p_GetComp(h,R);
        if (p_Totaldegree(h,R)==0)
          convSingNFlintN(fmpq_mat_entry(FLINTM,i-1,j),p_GetCoeff(h,R),R->cf);
        else
        {
          WerrorS("smatrix for rref is not constant");
          return M;
        }
        pIter(h);
      }
    }
    fmpq_mat_rref(FLINTM,FLINTM);
    for(i=r;i>0;i--)
    {
      for(j=c-1;j>=0;j--)
      {
        number n=convFlintNSingN(fmpq_mat_entry(FLINTM,i-1,j),R->cf);
        if(!n_IsZero(n,R->cf))
        {
          poly p=p_NSet(n,R);
          p_SetComp(p,i,R);
          M->m[j]=p_Add_q(M->m[j],p,R);
        }
      }
    }
    fmpq_mat_clear(FLINTM);
  }
  else if (rField_is_Zp(R))
  {
    nmod_mat_t FLINTM;
    nmod_mat_init(FLINTM,r,c,rChar(R));
    for(j=c-1;j>=0;j--)
    {
      poly h=m->m[j];
      while(h!=NULL)
      {
        i=p_GetComp(h,R);
        if (p_Totaldegree(h,R)==0)
          nmod_mat_entry(FLINTM,i-1,j)=(long)p_GetCoeff(h,R);
        else
        {
          WerrorS("smatrix for rref is not constant");
          return M;
        }
        pIter(h);
      }
    }
    nmod_mat_rref(FLINTM);
    for(i=r;i>0;i--)
    {
      for(j=c-1;j>=0;j--)
      {
        number n=n_Init(nmod_mat_entry(FLINTM,i-1,j),R->cf);
        if(!n_IsZero(n,R->cf))
        {
          poly p=p_NSet(n,R);
          p_SetComp(p,i,R);
          M->m[j]=p_Add_q(M->m[j],p,R);
        }
      }
    }
    nmod_mat_clear(FLINTM);
  }
  else
  {
    WerrorS("not implemented for these coefficients");
  }
  return M;
}

matrix singflint_kernel(matrix  m, const ring R)
{
  matrix M=NULL;
  if (rField_is_Zp(R))
  {
    nmod_mat_t FLINTM;
    nmod_mat_t FLINTX;
    nmod_mat_init (FLINTX, (long)MATROWS(m), (long) MATCOLS(m), rChar(R));
    // convert matrix
    convSingMFlintNmod_mat(m,FLINTM,R);
    // rank
    long rk= nmod_mat_nullspace(FLINTX,FLINTM);
    nmod_mat_clear(FLINTM);
    M=convFlintNmod_matSingM(FLINTX,R);
    // clean up
    nmod_mat_clear(FLINTX);
  }
  else
  {
    WerrorS("not implemented for these coefficients");
  }
  return M;
}

ideal singflint_kernel(ideal  m, const ring R) /*assume smatrix m*/
{
  int r=m->rank;
  int c=m->ncols;
  int i,j;
  ideal M=idInit(c,r);
  if (rField_is_Zp(R))
  {
    nmod_mat_t FLINTM;
    nmod_mat_t FLINTX;
    nmod_mat_init(FLINTM,r,c,rChar(R));
    nmod_mat_init(FLINTX,r,c,rChar(R));
    for(j=c-1;j>=0;j--)
    {
      poly h=m->m[j];
      while(h!=NULL)
      {
        i=p_GetComp(h,R);
        if (p_Totaldegree(h,R)==0)
          nmod_mat_entry(FLINTM,i-1,j)=(long)p_GetCoeff(h,R);
        else
        {
          WerrorS("smatrix for rref is not constant");
          return M;
        }
        pIter(h);
      }
    }
    nmod_mat_nullspace(FLINTX,FLINTM);
    nmod_mat_clear(FLINTM);
    for(i=r;i>0;i--)
    {
      for(j=c-1;j>=0;j--)
      {
        number n=n_Init(nmod_mat_entry(FLINTX,i-1,j),R->cf);
        if(!n_IsZero(n,R->cf))
        {
          poly p=p_NSet(n,R);
          p_SetComp(p,i,R);
          M->m[j]=p_Add_q(M->m[j],p,R);
        }
      }
    }
    nmod_mat_clear(FLINTX);
  }
  else
  {
    WerrorS("not implemented for these coefficients");
  }
  return M;
}

bigintmat* singflint_LLL(bigintmat*  m, bigintmat* T)
{
  int r=m->rows();
  int c=m->cols();
  bigintmat* res=new bigintmat(r,c,m->basecoeffs());
  fmpz_mat_t M, Transf;
  fmpz_mat_init(M, r, c);
  if(T != NULL)
  {
    fmpz_mat_init(Transf, T->rows(), T->rows());
  }
  fmpz_t dummy;
  mpz_t n;
  int i,j;
  for(i=r;i>0;i--)
  {
    for(j=c;j>0;j--)
    {
      n_MPZ(n, BIMATELEM(*m, i, j),m->basecoeffs());
      convSingNFlintN(dummy,n);
      mpz_clear(n);
      fmpz_set(fmpz_mat_entry(M, i-1, j-1), dummy);
      fmpz_clear(dummy);
    }
  }
  if(T != NULL)
  {
    for(i=T->rows();i>0;i--)
    {
      for(j=T->rows();j>0;j--)
      {
        n_MPZ(n, BIMATELEM(*T, i, j),T->basecoeffs());
        convSingNFlintN(dummy,n);
        mpz_clear(n);
        fmpz_set(fmpz_mat_entry(Transf, i-1, j-1), dummy);
        fmpz_clear(dummy);
      }
    }
  }
  fmpz_lll_t fl;
  fmpz_lll_context_init_default(fl);
  if(T != NULL)
    fmpz_lll(M, Transf, fl);
  else
    fmpz_lll(M, NULL, fl);
  for(i=r;i>0;i--)
  {
    for(j=c;j>0;j--)
    {
      convFlintNSingN(n, fmpz_mat_entry(M, i-1, j-1));
      n_Delete(&(BIMATELEM(*res,i,j)),res->basecoeffs());
      BIMATELEM(*res,i,j)=n_InitMPZ(n,res->basecoeffs());
      mpz_clear(n);
    }
  }
  if(T != NULL)
  {
    for(i=T->rows();i>0;i--)
    {
      for(j=T->cols();j>0;j--)
      {
        convFlintNSingN(n, fmpz_mat_entry(Transf, i-1, j-1));
        n_Delete(&(BIMATELEM(*T,i,j)),T->basecoeffs());
        BIMATELEM(*T,i,j)=n_InitMPZ(n,T->basecoeffs());
        mpz_clear(n);
      }
    }
  }
  return res;
}

intvec* singflint_LLL(intvec*  m, intvec* T)
{
  int r=m->rows();
  int c=m->cols();
  intvec* res = new intvec(r,c,(int)0);
  fmpz_mat_t M,Transf;
  fmpz_mat_init(M, r, c);
  if(T != NULL)
    fmpz_mat_init(Transf, r, r);
  fmpz_t dummy;
  int i,j;
  for(i=r;i>0;i--)
  {
    for(j=c;j>0;j--)
    {
      convSingIFlintI(dummy,IMATELEM(*m,i,j));
      fmpz_set(fmpz_mat_entry(M, i-1, j-1), dummy);
      fmpz_clear(dummy);
    }
  }
  if(T != NULL)
  {
    for(i=T->rows();i>0;i--)
    {
      for(j=T->rows();j>0;j--)
      {
        convSingIFlintI(dummy,IMATELEM(*T,i,j));
        fmpz_set(fmpz_mat_entry(Transf, i-1, j-1), dummy);
        fmpz_clear(dummy);
      }
    }
  }
  fmpz_lll_t fl;
  fmpz_lll_context_init_default(fl);
  if(T != NULL)
    fmpz_lll(M, Transf, fl);
  else
    fmpz_lll(M, NULL, fl);
  for(i=r;i>0;i--)
  {
    for(j=c;j>0;j--)
    {
      IMATELEM(*res,i,j)=convFlintISingI(fmpz_mat_entry(M, i-1, j-1));
    }
  }
  if(T != NULL)
  {
    for(i=Transf->r;i>0;i--)
    {
      for(j=Transf->r;j>0;j--)
      {
        IMATELEM(*T,i,j)=convFlintISingI(fmpz_mat_entry(Transf, i-1, j-1));
      }
    }
  }
  return res;
}
#endif
#endif