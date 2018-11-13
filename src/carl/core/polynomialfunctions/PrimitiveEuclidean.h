#pragma once

#include "Content.h"
#include "PrimitivePart.h"

#include "../UnivariatePolynomial.h"

namespace carl {

/**
 * Computes the GCD of two univariate polynomial with coefficients
 * from a unique factorization domain using the primitive euclidean algorithm.
 * @see @cite GCL92, page 57, Algorithm 2.3 
 */
template<typename Coeff>
UnivariatePolynomial<Coeff> primitive_euclidean(const UnivariatePolynomial<Coeff>& a, const UnivariatePolynomial<Coeff>& b) {
	UnivariatePolynomial<Coeff> c = carl::primitive_part(a.normalized());
	UnivariatePolynomial<Coeff> d = carl::primitive_part(b.normalized());
	
	while (!d.isZero()) {
		UnivariatePolynomial<Coeff> r = c.prem(d);
		c = d;
		d = carl::primitive_part(r.normalized());
	}
	return carl::gcd(carl::content(a), carl::content(b)) * c;
}

}
