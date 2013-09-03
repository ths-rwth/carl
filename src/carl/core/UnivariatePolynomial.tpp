/** 
 * @file:   UnivariatePolynomial.tpp
 * @author: Sebastian Junges
 *
 * @since August 26, 2013
 */

#pragma once

#include "UnivariatePolynomial.h"

namespace carl
{
template<typename Coeff>
UnivariatePolynomial<Coeff>::UnivariatePolynomial(Variable::Arg mainVar, std::initializer_list<Coeff> coefficients)
: mMainVar(mainVar), mCoefficients(coefficients)
{
	
}

template<typename C>
std::ostream& operator<<(std::ostream& os, const UnivariatePolynomial<C>& rhs)
{
	for(size_t i = 0; i < rhs.mCoefficients.size()-1; i++ )
	{
		os << "(" << rhs.mCoefficients[rhs.mCoefficients.size()-i-1] << ")*" << rhs.mMainVar << "^" << rhs.mCoefficients.size()-i-1 << " + ";
	}
	os << rhs.mCoefficients[0];
	return os;
}
}
