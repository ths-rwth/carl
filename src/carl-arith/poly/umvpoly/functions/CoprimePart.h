#pragma once

#include "../CoCoAAdaptor.h"
#include <carl-logging/carl-logging.h>

namespace carl {

template<typename C, typename O, typename P>
class MultivariatePolynomial;

/**
* Calculates the coprime part of p and q.
*/
template<typename C, typename O, typename P>
MultivariatePolynomial<C,O,P> coprimePart(const MultivariatePolynomial<C,O,P>& p, const MultivariatePolynomial<C,O,P>& q) {
	if (p.is_constant() || q.is_constant()) {
		return p;
	}

	auto s = overloaded {
	#if defined USE_COCOA
		[](const MultivariatePolynomial<mpq_class,O,P>& p, const MultivariatePolynomial<mpq_class,O,P>& q){ CoCoAAdaptor<MultivariatePolynomial<mpq_class,O,P>> c({p, q}); return c.makeCoprimeWith(p, q); },
		[](const MultivariatePolynomial<mpz_class,O,P>& p, const MultivariatePolynomial<mpz_class,O,P>& q){ CoCoAAdaptor<MultivariatePolynomial<mpz_class,O,P>> c({p, q}); return c.makeCoprimeWith(p, q); }
	#else
		[](const MultivariatePolynomial<mpq_class,O,P>& p, const MultivariatePolynomial<mpq_class,O,P>&){ return p; },
		[](const MultivariatePolynomial<mpz_class,O,P>& p, const MultivariatePolynomial<mpz_class,O,P>&){ return p; }
	#endif
	#if defined USE_GINAC
		,
		[](const MultivariatePolynomial<cln::cl_RA,O,P>& p, const MultivariatePolynomial<cln::cl_RA,O,P>&){ return p; },
		[](const MultivariatePolynomial<cln::cl_I,O,P>& p, const MultivariatePolynomial<cln::cl_I,O,P>&){ return p; }
	#endif
	};
	return s(p, q);
}

}
