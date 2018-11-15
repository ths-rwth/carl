#pragma once

#include "../MultivariatePolynomial.h"
#include "../UnivariatePolynomial.h"

namespace  carl {

template<typename T>
T gcd(const T& lhs, const T& rhs);

/**
 * The content of a polynomial is the gcd of the coefficients of the normal part of a polynomial.
 * The content of zero is zero.
 * @see @cite GCL92, page 53, definition 2.18
 * @return The content of the polynomial.
 */
template<typename Coeff>
Coeff content(const UnivariatePolynomial<Coeff>& p) {
	if (isZero(p)) {
		// By definition
		return Coeff(0);
	}
	assert(p.isNormal());
	if constexpr (is_field<Coeff>::value) {
		// For fields, content(p) = 1
		return Coeff(1);
	}
	auto it = p.coefficients().rbegin();
	Coeff gcd = *it;
	for (++it; it != p.coefficients().rend(); ++it) {
		if (isOne(gcd)) break;
		if (isZero(*it)) continue;
		gcd = carl::gcd(gcd, *it);
	}
	return gcd;
}

}
