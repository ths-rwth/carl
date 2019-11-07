#pragma once

#include "to_univariate_polynomial.h"

#include "../MultivariatePolynomial.h"
#include "../UnivariatePolynomial.h"

namespace carl {

/**
 * Switches the main variable using a purely syntactical restructuring.
 * The resulting polynomial will be algebraicly identical, but have the given variable as its main variable.
 * @param newVar New main variable.
 * @return Restructured polynomial.
 */
template<typename Coeff>
UnivariatePolynomial<MultivariatePolynomial<typename UnderlyingNumberType<Coeff>::type>> switch_main_variable(const UnivariatePolynomial<Coeff>& p, Variable newVar) {
	assert(p.isConsistent());
	using MP = MultivariatePolynomial<typename UnderlyingNumberType<Coeff>::type>;
	return to_univariate_polynomial(MP(p), newVar);
}

/**
 * Replaces the main variable in p.
 * @param newVar New main variable.
 * @return New polynomial.
 */
template<typename Coeff>
UnivariatePolynomial<Coeff> replace_main_variable(const UnivariatePolynomial<Coeff>& p, Variable newVar) {
	if constexpr (carl::is_number<Coeff>::value) {
		return UnivariatePolynomial<Coeff>(newVar, p.coefficients());
	} else {
		using MP = MultivariatePolynomial<typename UnderlyingNumberType<Coeff>::type>;
		return to_univariate_polynomial(MP(p).substitute(p.mainVar(), MP(newVar)), newVar);
	}
}

}