/** 
 * @file   PrimitiveEuclideanAlgorithm.tpp
 * @author Sebastian Junges
 *
 */

#pragma once

#include "PrimitiveEuclideanAlgorithm.h"
#include "UnivariatePolynomial.h"
namespace carl
{
	template<typename Coeff>
	UnivariatePolynomial<Coeff> PrimitiveEuclidean::operator ()(const UnivariatePolynomial<Coeff>& a, const UnivariatePolynomial<Coeff>& b) const
	{
		assert(a.isNormal());
		assert(b.isNormal());
		UnivariatePolynomial<Coeff> c = a.primitivePart();
		UnivariatePolynomial<Coeff> d = b.primitivePart();
		
		std::cout << "c = " << c << std::endl;
		std::cout << "d = " << d << std::endl;
		/// @todo Using pointers may speed up the code here.
		while(!d.isZero())
		{
			UnivariatePolynomial<Coeff> r = c.prem(d);
			c = d;
			std::cout << "r = " << r << std::endl;
			d = r.normalized().primitivePart();
			std::cout << "c = " << c << std::endl;
			std::cout << "d = " << d << std::endl;
		
		}
		Coeff gamma = gcd(a.content(), b.content());
		return gamma * c;
	}
}