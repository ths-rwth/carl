#pragma once

#include "../Sign.h"
#include "../UnivariatePolynomial.h"
#include "../../interval/Interval.h"

namespace carl {

/**
 * Counts the sign variations (i.e. an upper bound for the number of real roots) via Descarte's rule of signs.
 * This is an upper bound for countRealRoots().
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
	p.shift(interval.lower());
	p.scale(interval.diameter());
	p.reverse();
	p.shift(1);
	p.stripLeadingZeroes();
	assert(p.isConsistent());
	auto res = carl::sign_variations(p.coefficients().begin(), p.coefficients().end(), [](const auto& c){ return carl::sgn(c); });
	CARL_LOG_TRACE("carl.core", polynomial << " has " << res << " sign variations within " << interval);
	return res;
}

}