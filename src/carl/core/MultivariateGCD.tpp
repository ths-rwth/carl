/** 
 * @file:   MultivariateGCD.tpp
 * @author: Sebastian Junges
 *
 * @since January 19, 2014
 */

#pragma once

#include "MultivariateGCD.h"

#include "UnivariatePolynomial.h"

#include "MultivariatePolynomial.h"

namespace carl
{

template<typename GCDCalculation, typename C, typename O, typename P>
MultivariatePolynomial<C,O,P> MultivariateGCD<GCDCalculation, C, O, P>::calculate() 
{
	// We start with some trivial cases.
	if(mp1 == 1 || mp2 == 1) return Polynomial(1);
	if(is_field<C>::value && mp1.isConstant() && mp2.isConstant()) 
	{
		return Polynomial(1);
	}
	//if(mp1.nrTerms() == 1 && mp2.nrTerms() == 1)
	//{
		//return Polynomial(Term<C>::gcd(*mp1.lterm(), *mp2.lterm()));
	//}

	// And we do some simplifications for the input.
	// In order to do so, we gather some information about the polynomials, as we most certainly need them later on.

	// We check for mutual trivial factorizations.

	// And we check for linearly appearing variables. Notice that ay + b is irreducible and thus,
	// gcd(p, ay + b) is either ay + b or 1.

	Variable x = getMainVar(mp1, mp2);
	if(x == Variable::NO_VARIABLE)
	{
		return Polynomial(1);
	}
	UnivReprPol A = mp1.toUnivariatePolynomial(x);
	UnivReprPol B = mp2.toUnivariatePolynomial(x);
	UnivReprPol GCD = (*static_cast<GCDCalculation*>(this))(A.normalized(),B.normalized());
	
	return Polynomial(GCD);
	//return Result()
//		return Result(GCD, A/GCD, B/GCD);

}	


template<typename C, typename O, typename P>
MultivariatePolynomial<C,O,P> gcd(const MultivariatePolynomial<C,O,P>& a, const MultivariatePolynomial<C,O,P>& b)
{
	MultivariateGCD<PrimitiveEuclidean, C, O, P> gcd_calc(a,b);
	return gcd_calc.calculate();
}

}