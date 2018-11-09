#pragma once

#include "../logging.h"
#include "../../converter/CoCoAAdaptor.h"

namespace carl {

template<typename C, typename O, typename P>
class MultivariatePolynomial;

template<typename C, typename O, typename P>
MultivariatePolynomial<C,O,P> coprimePart(const MultivariatePolynomial<C,O,P>& p, const MultivariatePolynomial<C,O,P>& q) {
	if (p.isConstant() || q.isConstant()) {
		return p;
	}

	auto s = overloaded {
	#if defined USE_COCOA
		[](const MultivariatePolynomial<mpq_class,O,P>& p, const MultivariatePolynomial<mpq_class,O,P>& q){ CoCoAAdaptor<MultivariatePolynomial<mpq_class,O,P>> c({p, q}); return c.makeCoprimeWith(p, q); },
		[](const MultivariatePolynomial<mpz_class,O,P>& p, const MultivariatePolynomial<mpz_class,O,P>& q){ CoCoAAdaptor<MultivariatePolynomial<mpz_class,O,P>> c({p, q}); return c.makeCoprimeWith(p, q); }
	#else
		[](const MultivariatePolynomial<mpq_class,O,P>& p, const MultivariatePolynomial<mpq_class,O,P>& q){ return p; },
		[](const MultivariatePolynomial<mpz_class,O,P>& p, const MultivariatePolynomial<mpz_class,O,P>& q){ return p; }
	#endif
	#if defined USE_GINAC
		,
		[](const MultivariatePolynomial<cln::cl_RA,O,P>& p, const MultivariatePolynomial<cln::cl_RA,O,P>& q){ return p; },
		[](const MultivariatePolynomial<cln::cl_I,O,P>& p, const MultivariatePolynomial<cln::cl_I,O,P>& q){ return p; }
	#endif
	};
	return s(p, q);
}

}
