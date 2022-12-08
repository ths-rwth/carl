#pragma once

#include "../MultivariatePolynomial.h"

namespace carl {

/**
 * @return An approximation of the bitsize of this monomial.
 */
inline std::size_t bitsize(const Monomial& m) {
	return 0;
}

/**
 * @return An approximation of the bitsize of this term.
 */
template<typename Coeff>
std::size_t bitsize(const Term<Coeff>& t) {
	if (!t.monomial()) return bitsize(t.coeff());
	return bitsize(*t.monomial()) + bitsize(t.coeff());
}

/**
 * @return An approximation of the bitsize of this polynomial.
 */
template<typename Coeff, typename Ordering, typename Policies>
std::size_t bitsize(const MultivariatePolynomial<Coeff,Ordering,Policies>& p) {
	return std::accumulate(p.begin(), p.end(), static_cast<std::size_t>(0),
		[](std::size_t cur, const auto& t){ return cur + bitsize(t); }
	);
}

}