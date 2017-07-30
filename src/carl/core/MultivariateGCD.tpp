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
#include "VariablesInformation.h"

#include "../numbers/FunctionSelector.h"

namespace carl
{

template<typename GCDCalculation, typename C, typename O, typename P>
MultivariatePolynomial<C,O,P> MultivariateGCD<GCDCalculation, C, O, P>::calculate() 
{
	assert(!mp1.isZero());
	assert(!mp2.isZero());
	// We start with some trivial cases.
	if(mp1.isOne() || mp2.isOne())
    {
        return Polynomial(1);
    }
	if(is_field<C>::value && mp1.isConstant())
	{
		return Polynomial(carl::gcd( mp1.constantPart(), carl::constant_one<C>().get()/mp2.coprimeFactor() ));
	}
    if(is_field<C>::value && mp2.isConstant())
    {
		return Polynomial(carl::gcd( mp2.constantPart(), carl::constant_one<C>().get()/mp1.coprimeFactor() ));
    }
	if(mp1.nrTerms() == 1 && mp2.nrTerms() == 1)
	{
		return Polynomial(Term<C>::gcd(mp1.lterm(), mp2.lterm()));
	}

	// And we do some simplifications for the input.
	// In order to do so, we gather some information about the polynomials, as we most certainly need them later on.
	
	// We check for mutual trivial factorizations.

	// And we check for linearly appearing variables. Notice that ay + b is irreducible and thus,
	// gcd(p, ay + b) is either ay + b or 1.
	using TypeSelector = carl::function_selector::NaryTypeSelector;
#if defined USE_GINAC
using types = carl::function_selector::wrap_types<mpz_class,mpq_class,cln::cl_I,cln::cl_RA>;
#else
using types = carl::function_selector::wrap_types<mpz_class,mpq_class>;
#endif
auto s = carl::createFunctionSelector<TypeSelector, types>(
#if defined USE_COCOA
	[](const auto& n1, const auto& n2){ CoCoAAdaptor<Polynomial> c({n1, n2}); return c.gcd(n1,n2); },
	[](const auto& n1, const auto& n2){ CoCoAAdaptor<Polynomial> c({n1, n2}); return c.gcd(n1,n2); }
#else
	[this](const auto& n1, const auto& n2){ return this->customCalculation(n1,n2); },
	[this](const auto& n1, const auto& n2){ return this->customCalculation(n1,n2); }
#endif
#if defined USE_GINAC
	,
	[](const auto& n1, const auto& n2){ return ginacGcd<Polynomial>( n1, n2 ); },
	[](const auto& n1, const auto& n2){ return ginacGcd<Polynomial>( n1, n2 );}
#endif
);
	return s(mp1, mp2);
}

template<typename GCDCalculation, typename C, typename O, typename P>
MultivariatePolynomial<C,O,P> MultivariateGCD<GCDCalculation, C, O, P>::customCalculation(const Polynomial& a, const Polynomial& b) {
	Variable x = getMainVar(a, b);
	if(x == Variable::NO_VARIABLE)
	{
		return Polynomial(1);
	}
	UnivReprPol A = a.toUnivariatePolynomial(x);
	UnivReprPol B = b.toUnivariatePolynomial(x);
	UnivReprPol GCD = (*static_cast<GCDCalculation*>(this))(A.normalized(),B.normalized());
    Polynomial result = Polynomial(GCD);
    // TODO: prevent the following case in the given algorithm GCDCalculation
    if( carl::isNegative( result.lcoeff() ) && !(carl::isNegative( a.lcoeff() ) && carl::isNegative(  b.lcoeff() )) )
    {
        result = -result;
    }
	return result;
}

template<typename C, typename O, typename P>
Term<C> gcd(const MultivariatePolynomial<C,O,P>& a, const Term<C>& b)
{
	static_assert(is_field<C>::value, "Only implemented for field coefficients");
	assert(!a.isZero());
	assert(!b.isZero());
	if(b.isConstant()) return Term<C>(C(1));
	return Term<C>(C(1), gcd(a,*(b.monomial())));
}


template<typename C, typename O, typename P>
Monomial::Arg gcd(const MultivariatePolynomial<C,O,P>& a, const Monomial::Arg& b)
{
    if( !b )
        return nullptr;
	assert(!a.isZero());
	VariablesInformation<false, MultivariatePolynomial<C,O,P>> varinfo = a.getVarInfo();
	std::vector<std::pair<Variable, exponent>> vepairs;
	for(const auto& ve : *b)
	{
		if(varinfo.getVarInfo(ve.first)->occurence() == a.nrTerms())
		{
			vepairs.push_back(ve.first, std::min(varinfo.getVarInfo(ve.first)->minDegree(), ve.second));
		}
	}
	return createMonomial( std::move(vepairs) );
}

template<typename C, typename O, typename P>
MultivariatePolynomial<C,O,P> gcd(const MultivariatePolynomial<C,O,P>& a, const MultivariatePolynomial<C,O,P>& b)
{
	MultivariateGCD<PrimitiveEuclidean, C, O, P> gcd_calc(a,b);
    #ifdef USE_GINAC
    assert( gcd_calc.checkCorrectnessWithGinac() );
    #endif 
	return gcd_calc.calculate();
}

template<typename C, typename O, typename P>
MultivariatePolynomial<C,O,P> lcm(const MultivariatePolynomial<C,O,P>& a, const MultivariatePolynomial<C,O,P>& b)
{
	assert(!a.isZero());
	assert(!b.isZero());
	if(a == b) return a;
	//if(is_field<C>::value && a.isConstant() && b.isConstant()) return MultivariatePolynomial<C,O,P>(1);
	//if(is_field<C>::value && a.isConstant()) return b;
	//if(is_field<C>::value && b.isConstant()) return a;
	
	
	return quotient((a*b),gcd(a,b));
}

}
