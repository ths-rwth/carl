#pragma once

#include <carl/core/Sign.h>
#include "../UnivariatePolynomial.h"
#include <carl/interval/Interval.h>

namespace carl {

namespace detail_sign_variations {

/**
 * Reverses the order of the coefficients of this polynomial.
 * This method is meant to be called by signVariations only.
 * @complexity O(n)
 */
template<typename Coefficient>
UnivariatePolynomial<Coefficient> reverse(UnivariatePolynomial<Coefficient>&& p) {
	std::vector<Coefficient> coeffs(std::move(p.coefficients()));
	std::reverse(coeffs.begin(), coeffs.end());
	return UnivariatePolynomial<Coefficient>(p.mainVar(), std::move(coeffs));
}

/**
 * Scale the variable, i.e. apply \f$ x \rightarrow factor * x \f$
 * This method is meant to be called by signVariations only.
 * @param factor Factor to scale x.
 * @complexity O(n)
 */
template<typename Coefficient>
UnivariatePolynomial<Coefficient> scale(UnivariatePolynomial<Coefficient>&& p, const Coefficient& factor) {
	std::vector<Coefficient> coeffs(std::move(p.coefficients()));
	Coefficient f = factor;
	for (auto& c: coeffs) {
		c *= f;
		f *= factor;
	}
	return UnivariatePolynomial<Coefficient>(p.mainVar(), std::move(coeffs));
}

/**
 * Shift the variable by a, i.e. apply \f$ x \rightarrow x + a \f$
 * This method is meant to be called by signVariations only.
 * @param a Offset to shift x.
 * @complexity O(n^2)
 */
template<typename Coefficient>
UnivariatePolynomial<Coefficient> shift(const UnivariatePolynomial<Coefficient>& p, const Coefficient& a) {
	std::vector<Coefficient> next;
	next.reserve(p.coefficients().size());
	next.push_back(p.coefficients().back());

	for (std::size_t i = 0; i < p.coefficients().size()-1; i++) {
		next.push_back(next.back());
		for (std::size_t j = i; j > 0; j--) {
			next[j] = a * next[j] + next[j-1];
		}
		next[0] = a * next[0] + p.coefficients()[p.coefficients().size()-2-i];
	}
	return UnivariatePolynomial<Coefficient>(p.mainVar(), std::move(next));
}

}

/**
 * Counts the sign variations (i.e. an upper bound for the number of real roots) via Descarte's rule of signs.
 * This is an upper bound for countRealRoots().
 * @param polynomial A polynomial.
 * @param interval Count roots within this interval.
 * @return Upper bound for number of real roots within the interval.
 */
template<typename Coefficient>
uint sign_variations(const UnivariatePolynomial<Coefficient>& polynomial, const Interval<Coefficient>& interval) {
	if (interval.isEmpty()) return 0;
	if (interval.isPointInterval()) {
		std::vector<Coefficient> vals;
		Coefficient factor = carl::constant_one<Coefficient>::get();
		for (const auto& c: polynomial.coefficients()) {
			vals.push_back(c * factor);
			factor *= interval.lower();
		}
		auto res = carl::sign_variations(vals.begin(), vals.end(), [](const auto& c){ return carl::sgn(c); });
		CARL_LOG_TRACE("carl.core", polynomial << " has " << res << " sign variations at " << interval.lower());
		return res;
	}
	UnivariatePolynomial<Coefficient> p(polynomial);
	p = detail_sign_variations::shift(p, interval.lower());
	p = detail_sign_variations::scale(std::move(p), interval.diameter());
	p = detail_sign_variations::reverse(std::move(p));
	p = detail_sign_variations::shift(p, carl::constant_one<Coefficient>::get());
	p.stripLeadingZeroes();
	assert(p.isConsistent());
	auto res = carl::sign_variations(p.coefficients().begin(), p.coefficients().end(), [](const auto& c){ return carl::sgn(c); });
	CARL_LOG_TRACE("carl.core", polynomial << " has " << res << " sign variations within " << interval);
	return res;
}

}