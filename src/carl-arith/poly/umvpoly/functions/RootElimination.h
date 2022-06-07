#pragma once

#include "Degree.h"
#include "Evaluation.h"
#include "../UnivariatePolynomial.h"

namespace carl {

/**
 * Reduces the given polynomial such that zero is not a root anymore.
 * Is functionally equivalent to eliminate_root(0), but faster.
 */
template<typename Coeff>
void eliminate_zero_root(UnivariatePolynomial<Coeff>& p) {
	std::size_t i = 0;
	while ((i < p.coefficients().size()) && (is_zero(p.coefficients()[i]))) i++;
	if (i == 0) return;
	// Now shift by i elements, drop lower i coefficients (they are zero anyway)
	for (std::size_t j = 0; j < p.coefficients().size()-i; j++) {
		p.coefficients()[j] = p.coefficients()[j+i];
	}
	p.coefficients().resize(p.coefficients().size()-i);
	assert(p.is_consistent());
}

/**
 * Reduces the polynomial such that the given root is not a root anymore.
 * The reduction is achieved by removing the linear factor (mainVar - root) from the polynomial, possibly multiple times.
 *
 * This method assumes that the given root is an actual real root of this polynomial.
 * If this is not the case, i.e. <code>evaluate(root) != 0</code>, the polynomial will contain meaningless garbage.
 * @param p The polynomial.
 * @param root Root to be eliminated.
 */
template<typename Coeff>
void eliminate_root(UnivariatePolynomial<Coeff>& p, const Coeff& root) {
	assert(is_root_of(p, root));
	if (carl::is_zero(p)) return;
	if (is_zero(root)) {
		eliminate_zero_root(p);
		return;
	}
	do {
		std::vector<Coeff> tmp(p.coefficients().size()-1);
		for (std::size_t i = p.coefficients().size()-1; i > 0; i--) {
			tmp[i-1] = p.coefficients()[i];
			p.coefficients()[i-1] += p.coefficients()[i] * root;
		}
		p.coefficients() = tmp;
	} while ((is_zero(evaluate(p, root))) && (p.coefficients().size() > 0));
}

}
