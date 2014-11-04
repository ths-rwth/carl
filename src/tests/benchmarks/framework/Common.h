/**
 * @file Common.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

namespace carl {

template<typename Coeff>
using CMP = carl::MultivariatePolynomial<Coeff>;
typedef GiNaC::ex GMP;
typedef polynomial::polynomial_ref ZMP;


template<typename Coeff>
using CUMP = carl::UnivariatePolynomial<carl::MultivariatePolynomial<Coeff>>;

template<typename Coeff>
using CUP = carl::UnivariatePolynomial<Coeff>;

typedef carl::Variable CVAR;
typedef GiNaC::symbol GVAR;
typedef polynomial::var ZVAR;

}