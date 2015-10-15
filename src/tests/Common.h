#pragma once

#include "carl/core/MultivariatePolynomial.h"
#include "carl/core/PrimitiveEuclideanAlgorithm.h"
#include "carl/core/RealAlgebraicNumber.h"


#ifdef COMPARE_WITH_CLN
#include <cln/cln.h>
#endif
#include <gmpxx.h>

#ifdef USE_CLN_NUMBERS
#include <cln/cln.h>
typedef cln::cl_RA Rational;
typedef cln::cl_I Integer;
#else
#include <gmpxx.h>
typedef mpq_class Rational;
typedef mpz_class Integer;
#endif

extern template class carl::GaloisField<Integer>;
extern template class carl::GFNumber<Integer>;
extern template class carl::Term<Rational>;
extern template class carl::MultivariatePolynomial<Rational>;
extern template class carl::MultivariateGCD<carl::PrimitiveEuclidean, Rational>;
