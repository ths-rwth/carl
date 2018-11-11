#pragma once

#include "GCD.h"

namespace carl {

template<typename C, typename O, typename P>
MultivariatePolynomial<C,O,P> lcm(const MultivariatePolynomial<C,O,P>& a, const MultivariatePolynomial<C,O,P>& b)
{
	assert(!a.isZero());
	assert(!b.isZero());
	if (a == b) return a;
	return quotient((a*b), gcd(a,b));
}

}