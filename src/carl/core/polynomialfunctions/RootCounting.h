#pragma once

#include "SturmSequence.h"
#include "../Sign.h"
#include "../UnivariatePolynomial.h"

#include <cassert>

namespace carl {

/**
 * Calculate the number of real roots of a polynomial within a given interval based on a sturm sequence of this polynomial.
 * @param seq Sturm sequence.
 * @param interval Interval.
 * @return Number of real roots in the interval.
 */
template<typename Coefficient>
int count_real_roots(const std::vector<UnivariatePolynomial<Coefficient>>& seq, const Interval<Coefficient>& i) {
	int l = static_cast<int>(carl::signVariations(seq.begin(), seq.end(), 
		[&i](const auto& p){ return p.sgn(i.lower()); }
	));
	int r = static_cast<int>(carl::signVariations(seq.begin(), seq.end(),
		[&i](const auto& p){ return p.sgn(i.upper()); }
	));
	return l - r;
}

/**
 * Count the number of real roots of p within the given interval using Sturm sequences.
 * @param interval Count roots within this interval.
 * @return Number of real roots within the interval.
 */
template<typename Coefficient>
int count_real_roots(const UnivariatePolynomial<Coefficient>& p, const Interval<Coefficient>& i) {
	assert(!isZero(p));
	assert(!p.isRoot(i.lower()));
	assert(!p.isRoot(i.upper()));
	return count_real_roots(sturm_sequence(p), i);
}

}
