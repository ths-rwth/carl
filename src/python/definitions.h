#pragma once
#include "carl/core/Variable.h"
#include "carl/core/Monomial.h"
#include "carl/core/MultivariatePolynomial.h"
#include "carl/core/RationalFunction.h"
#include "carl/formula/Formula.h"

#include <cln/cln.h>
typedef cln::cl_RA Rational;
//typedef double Rational;

typedef carl::MultivariatePolynomial<Rational> Polynomial;
typedef carl::FactorizedPolynomial<Polynomial> FactorizedPolynomial;
typedef carl::RationalFunction<Polynomial> RationalFunction;
typedef carl::RationalFunction<FactorizedPolynomial> FactorizedRationalFunction;
typedef carl::PolynomialFactorizationPair<Polynomial> FactorizationPair;
typedef carl::Formula<Polynomial> Formula;
