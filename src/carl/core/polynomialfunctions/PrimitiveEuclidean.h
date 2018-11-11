#pragma once

#include "../UnivariatePolynomial.h"

namespace carl {

/**
 * Computes the GCD of two univariate polynomial with coefficients
 * from a unique factorization domain using the primitive euclidean algorithm.
 * @see @cite GCL92, page 57, Algorithm 2.3 
 */
template<typename Coeff>
UnivariatePolynomial<Coeff> primitive_euclidean(const UnivariatePolynomial<Coeff>& a, const UnivariatePolynomial<Coeff>& b) {
	UnivariatePolynomial<Coeff> c = a.normalized().primitivePart();
	UnivariatePolynomial<Coeff> d = b.normalized().primitivePart();
	
	while (!d.isZero()) {
		UnivariatePolynomial<Coeff> r = c.prem(d);
		c = d;
		d = r.normalized().primitivePart();
	}
	return gcd(a.content(), b.content()) * c;
}

}