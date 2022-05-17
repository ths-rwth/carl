#pragma once

#include "Content.h"
#include "Degree.h"
#include "Division.h"

#include "../MultivariatePolynomial.h"
#include "../UnivariatePolynomial.h"

namespace  carl {

/**
 * The primitive part of p is the normal part of p divided by the content of p.
 * The primitive part of zero is zero.
 * @see @cite GCL92, page 53, definition 2.18
 * @return The primitive part of the polynomial.
 */
template<typename Coeff>
UnivariatePolynomial<Coeff> primitive_part(const UnivariatePolynomial<Coeff>& p) {
	if (carl::is_zero(p)) {
		// By definition
		return p;
	}
	if (p.isNormal()) {
		return p / carl::content(p);
	} else {
		return p / Coeff(-carl::content(p));
	}
}

/**
 * Returns this/divisor where divisor is the numeric content of this polynomial.
 * @return 
 */
template<typename Coeff>
UnivariatePolynomial<Coeff> pseudo_primitive_part(const UnivariatePolynomial<Coeff>& p) {
	auto c = p.numericContent();
	if (is_zero(c) || is_one(c)) return p;
	return divide(p, c).quotient;
}

}
