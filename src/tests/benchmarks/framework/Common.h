/**
 * @file Common.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

namespace carl {

template<typename Coeff>
using CMP = carl::MultivariatePolynomial<Coeff>;
#ifdef COMPARE_WITH_GINAC
typedef GiNaC::ex GMP;
#endif
#ifdef COMPARE_WITH_Z3
typedef polynomial::polynomial_ref ZMP;
#endif


template<typename Coeff>
using CUMP = carl::UnivariatePolynomial<carl::MultivariatePolynomial<Coeff>>;

template<typename Coeff>
using CUP = carl::UnivariatePolynomial<Coeff>;

typedef carl::Variable CVAR;
#ifdef COMPARE_WITH_GINAC
typedef GiNaC::symbol GVAR;
#endif
#ifdef COMPARE_WITH_Z3
typedef polynomial::var ZVAR;
#endif

}