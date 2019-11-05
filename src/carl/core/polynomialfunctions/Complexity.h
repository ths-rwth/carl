#pragma once

#include "../MultivariatePolynomial.h"
#include "../UnivariatePolynomial.h"

namespace carl {

/**
 * @return An approximation of the complexity of this monomial.
 */
inline std::size_t complexity(const Monomial& m) {
	return m.tdeg();
}

/**
 * @return An approximation of the complexity of this term.
 */
template<typename Coeff>
std::size_t complexity(const Term<Coeff>& t) {
	if (!t.monomial()) return 1;
	return complexity(*t.monomial()) + 1;
}

/**
 * @return An approximation of the complexity of this polynomial.
 */
template<typename Coeff, typename Ordering, typename Policies>
std::size_t complexity(const MultivariatePolynomial<Coeff,Ordering,Policies>& p) {
	return std::accumulate(p.begin(), p.end(), static_cast<std::size_t>(0),
		[](std::size_t cur, const auto& t){ return cur + complexity(t); }
	);
}

/**
* @return An approximation of the complexity of this polynomial.
*/
template<typename Coeff>
std::size_t complexity(const UnivariatePolynomial<Coeff>& p) {
	std::size_t result = 0;
	for (std::size_t deg = 0; deg <= p.degree(); ++deg) {
		if (p.coefficients()[deg].isZero()) continue;
		result += complexity(p.coefficients()[deg]) + deg;
	}
	return result;
}

}
