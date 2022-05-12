#pragma once

#include "../UnivariatePolynomial.h"
#include <carl/core/Variable.h>

namespace carl {

/**
 * Implements a generator for Chebyshev polynomials.
 */
template<typename Number>
struct Chebyshev {
	Variable mVar;
	explicit Chebyshev(Variable v): mVar(v) {}

	UnivariatePolynomial<Number> operator()(std::size_t n) const {
		UnivariatePolynomial<Number> t0(mVar, Number(1), 0);
		if (n == 0) return t0;
		UnivariatePolynomial<Number> t1(mVar, Number(1), 1);
		if (n == 1) return t1;
		UnivariatePolynomial<Number> twox(mVar, Number(2), 1);
		while (n >= 2) {
			UnivariatePolynomial<Number> tn = twox * t1 - t0;
			if (n == 2) return tn;
			t0 = t1;
			t1 = tn;
			--n;
		}
		assert(false);
		return t0;
	}
};

}
