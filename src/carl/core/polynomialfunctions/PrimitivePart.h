#pragma once

#include "Content.h"

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
	if (p.isZero()) {
		// By definition
		return p;
	}
	if (p.isNormal()) {
		return p / carl::content(p);
	} else {
		return p / Coeff(-carl::content(p));
	}
}

}
