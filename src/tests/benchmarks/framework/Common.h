/**
 * @file Common.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

namespace carl {

template<typename Coeff>
using CMP = carl::MultivariatePolynomial<Coeff>;
#ifdef USE_COCOA
typedef CoCoA::RingElem CoMP;
#endif
#ifdef USE_GINAC
typedef GiNaC::ex GMP;
#endif


template<typename Coeff>
using CUMP = carl::UnivariatePolynomial<carl::MultivariatePolynomial<Coeff>>;

template<typename Coeff>
using CUP = carl::UnivariatePolynomial<Coeff>;

typedef carl::Variable CVAR;
#ifdef USE_COCOA
typedef CoCoA::RingElem CoVAR;
#endif
#ifdef USE_GINAC
typedef GiNaC::symbol GVAR;
#endif

}
