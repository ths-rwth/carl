#pragma once

#include "Evaluation.h"
#include "SturmSequence.h"
#include "../Sign.h"
#include "../UnivariatePolynomial.h"

#include <cassert>

namespace carl {

/**
 * Calculate the number of real roots of a polynomial within a given interval based on a sturm sequence of this polynomial.
 * @param seq Sturm sequence.
 * @param i Interval.
 * @return Number of real roots in the interval.
 */
template<typename Coefficient>
int count_real_roots(const std::vector<UnivariatePolynomial<Coefficient>>& seq, const Interval<Coefficient>& i) {
	int l = static_cast<int>(carl::sign_variations(seq.begin(), seq.end(), 
		[&i](const auto& p){ return carl::sgn(carl::evaluate(p, i.lower())); }
	));
	int r = static_cast<int>(carl::sign_variations(seq.begin(), seq.end(),
		[&i](const auto& p){ return carl::sgn(carl::evaluate(p, i.upper())); }
	));
	return l - r;
}

/**
 * Count the number of real roots of p within the given interval using Sturm sequences.
 * @param p The polynomial.
 * @param i Count roots within this interval.
 * @return Number of real roots within the interval.
 */
template<typename Coefficient>
int count_real_roots(const UnivariatePolynomial<Coefficient>& p, const Interval<Coefficient>& i) {
	assert(!isZero(p));
	assert(!carl::is_root_of(p, i.lower()));
	assert(!carl::is_root_of(p, i.upper()));
	return count_real_roots(sturm_sequence(p), i);
}

}
