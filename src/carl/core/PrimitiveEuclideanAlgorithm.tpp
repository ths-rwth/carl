/** 
 * @file   PrimitiveEuclideanAlgorithm.tpp
 * @author Sebastian Junges
 *
 */

#pragma once

#include "MultivariateGCD.h"
#include "UnivariatePolynomial.h"



namespace carl
{
	template<typename Coeff>
	UnivariatePolynomial<Coeff> PrimitiveEuclidean::operator ()(const UnivariatePolynomial<Coeff>& a, const UnivariatePolynomial<Coeff>& b) const
	{
		assert(!a.isZero());
		assert(!b.isZero());
		assert(a.isNormal());
		assert(b.isNormal());
		UnivariatePolynomial<Coeff> c = a.primitivePart();
		UnivariatePolynomial<Coeff> d = b.primitivePart();
		
		/// @todo Using pointers may speed up the code here.
		while(!d.isZero())
		{
			UnivariatePolynomial<Coeff> r = c.prem(d);
			c = d;
			d = r.normalized().primitivePart();
		}
		Coeff gamma = gcd(a.content(), b.content());
		return gamma * c;
	}
}