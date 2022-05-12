#pragma once

#include "Derivative.h"
#include "Remainder.h"

#include "../UnivariatePolynomial.h"

namespace carl {


/**
 * Computes the sturm sequence of two polynomials.
 * Compared to the regular sturm sequence, we use the second polynomial as p_1.
 */
template<typename Coeff>
std::vector<UnivariatePolynomial<Coeff>> sturm_sequence(const UnivariatePolynomial<Coeff>& p, const UnivariatePolynomial<Coeff>& q) {
	std::vector<UnivariatePolynomial<Coeff>> seq({ p, q });
	if (isZero(p) || isZero(q)) return seq;

	for (std::size_t k = 2; ; ++k) {
		auto tmp = - remainder(seq[k-2], seq[k-1]);
		if (isZero(tmp)) return seq;
		seq.emplace_back(std::move(tmp));
	}
}

/**
 * Computes the sturm sequence of a polynomial as defined at @cite GCL92, page 333, example 22.
 * The sturm sequence of p is defined as:
 * - p_0 = p
 * - p_1 = p'
 * - p_k = -rem(p_{k-2}, p_{k-1})
 */
template<typename Coeff>
std::vector<UnivariatePolynomial<Coeff>> sturm_sequence(const UnivariatePolynomial<Coeff>& p) {
	return sturm_sequence(p, derivative(p));
}

}