#pragma once

#include "GCD_univariate.h"

#include "../logging.h"
#include "../UnivariatePolynomial.h"

namespace carl {

template<typename Coeff>
std::map<uint, UnivariatePolynomial<Coeff>> squareFreeFactorization(const UnivariatePolynomial<Coeff>& p) {
	CARL_LOG_TRACE("carl.core.upoly", "UnivSSF: " << p);
	std::map<uint,UnivariatePolynomial<Coeff>> result;
CLANG_WARNING_DISABLE("-Wtautological-compare")
	assert(!p.isZero()); // TODO what if zero?
	// degree() >= characteristic<Coeff>::value throws a warning in clang...
	if(characteristic<Coeff>::value != 0 && p.degree() >= characteristic<Coeff>::value)
CLANG_WARNING_RESET
	{
		CARL_LOG_TRACE("carl.core.upoly", "UnivSSF: degree greater than characteristic!");
		result.emplace(1, p);
		CARL_LOG_TRACE("carl.core.upoly", "UnivSSF: add the factor (" << p << ")^1");
	}
	else
	{
		assert(!p.isConstant()); // Othewise, the derivative is zero and the next assertion is thrown.
		UnivariatePolynomial<Coeff> b = p.derivative();
		CARL_LOG_TRACE("carl.core.upoly", "UnivSSF: b = " << b);
		UnivariatePolynomial<Coeff> s(p.mainVar());
		UnivariatePolynomial<Coeff> t(p.mainVar());
		assert(!b.isZero());
		UnivariatePolynomial<Coeff> c = carl::extended_gcd(p, b, s, t); // TODO: use gcd instead
		typename IntegralType<Coeff>::type numOfCpf = getNum(c.coprimeFactor());
		if(numOfCpf != 1) // TODO: is this maybe only necessary because the extended_gcd returns a polynomial with non-integer coefficients but it shouldn't?
		{
			c *= Coeff(numOfCpf);
		}
		CARL_LOG_TRACE("carl.core.upoly", "UnivSSF: c = " << c);
		if(c.isZero())
		{
			result.emplace(1, p);
			CARL_LOG_TRACE("carl.core.upoly", "UnivSSF: add the factor (" << p << ")^1");
		}
		else
		{
			UnivariatePolynomial<Coeff> w = p.divideBy(c).quotient;
			CARL_LOG_TRACE("carl.core.upoly", "UnivSSF: w = " << w);
			UnivariatePolynomial<Coeff> y = b.divideBy(c).quotient;
			CARL_LOG_TRACE("carl.core.upoly", "UnivSSF: y = " << y);
			UnivariatePolynomial<Coeff> z = y-w.derivative();
			CARL_LOG_TRACE("carl.core.upoly", "UnivSSF: z = " << z);
			uint i = 1;
			while(!z.isZero())
			{
				CARL_LOG_TRACE("carl.core.upoly", "UnivSSF: next iteration");
				UnivariatePolynomial<Coeff> g = carl::extended_gcd(w, z, s, t); // TODO: use gcd instead
				numOfCpf = getNum(g.coprimeFactor());
				if(numOfCpf != 1) // TODO: is this maybe only necessary because the extended_gcd returns a polynomial with non-integer coefficients but it shouldn't?
				{
					g *= Coeff(numOfCpf);
				}
				CARL_LOG_TRACE("carl.core.upoly", "UnivSSF: g = " << g);
				assert(result.find(i) == result.end());
				result.emplace(i, g);
				CARL_LOG_TRACE("carl.core.upoly", "UnivSSF: add the factor (" << g << ")^" << i);
				++i;
				w = w.divideBy(g).quotient;
				CARL_LOG_TRACE("carl.core.upoly", "UnivSSF: w = " << w);
				y = z.divideBy(g).quotient;
				CARL_LOG_TRACE("carl.core.upoly", "UnivSSF: y = " << y);
				z = y - w.derivative();
				CARL_LOG_TRACE("carl.core.upoly", "UnivSSF: z = " << z);
			}
			result.emplace(i, w);
			CARL_LOG_TRACE("carl.core.upoly", "UnivSSF: add the factor (" << w << ")^" << i);
		}
	}
	return result;
}

template<typename Coeff>
FactorMap<Coeff> factorization(const UnivariatePolynomial<Coeff>& p) {
    CARL_LOG_TRACE("carl.core.upoly", "UnivFactor: " << p);
	FactorMap<Coeff> result;
	if(p.isConstant()) // Constant.
	{
		CARL_LOG_TRACE("carl.core.upoly", "UnivFactor: add the factor (" << p << ")^" << 1 );
		result.emplace(p, 1);
		return result;
	}
	// Make the polynomial's coefficients coprime (integral and with gcd 1).
	UnivariatePolynomial<Coeff> remainingPoly(p.mainVar());
	Coeff factor = p.coprimeFactor();
	if(factor == 1)
	{
		remainingPoly = p;
	}
	else
	{
		// Store the rational factor and make the polynomial's coefficients coprime.
		CARL_LOG_TRACE("carl.core", "UnivFactor: add the factor (" << UnivariatePolynomial<Coeff>(p.mainVar(), constant_one<Coeff>::get() / factor) << ")^" << 1 );
		result.emplace(UnivariatePolynomial<Coeff>(p.mainVar(), constant_one<Coeff>::get() / factor), 1);
        std::vector<Coeff> remaining;
        remaining.reserve(p.coefficients().size());
		for(const Coeff& coeff : p.coefficients())
		{
			remaining.push_back(coeff * factor);
		}
        remainingPoly = UnivariatePolynomial<Coeff>(p.mainVar(), std::move(remaining));
	}
	assert(p.coefficients().size() > 1);
	// Exclude the factors  (x-r)^i  with  r rational.
	remainingPoly = UnivariatePolynomial<Coeff>::excludeLinearFactors(remainingPoly, result, static_cast<carl::sint>(INT_MAX));
	assert(!remainingPoly.isConstant() || remainingPoly.lcoeff() == (Coeff)1);
	if(!remainingPoly.isConstant())
	{
		CARL_LOG_TRACE("carl.core.upoly", "UnivFactor: Calculating square-free factorization of " << remainingPoly);
		// Calculate the square free factorization.
		auto sff = carl::squareFreeFactorization(remainingPoly);
//		factor = (Coeff) 1;
		for(auto expFactorPair = sff.begin(); expFactorPair != sff.end(); ++expFactorPair)
		{
//			Coeff cpf = expFactorPair->second.coprimeFactor();
//			if(cpf != (Coeff) 1)
//			{
//				factor *= pow(expFactorPair->second.coprimeFactor(), expFactorPair->first);
//				expFactorPair->second /= cpf;
//			}
			if(!expFactorPair->second.isConstant() || !carl::isOne(expFactorPair->second.lcoeff()))
			{
				auto retVal = result.emplace(expFactorPair->second, expFactorPair->first);
				CARL_LOG_TRACE("carl.core.upoly", "UnivFactor: add the factor (" << expFactorPair->second << ")^" << expFactorPair->first );
				if(!retVal.second)
				{
					retVal.first->second += expFactorPair->first;
				}
			}
		}
//		if(factor != (Coeff) 1)
//		{
//			CARL_LOG_TRACE("carl.core.upoly", "UnivFactor: add the factor (" << UnivariatePolynomial<Coeff>(mainVar(), {factor}) << ")^" << 1 );
//			// Add the constant factor to the factors.
//			if( result.begin()->first.isConstant() )
//			{
//				factor *= result.begin()->first.lcoeff();
//				result.erase( result.begin() );
//			}
//			result.insert(result.begin(), std::pair<UnivariatePolynomial<Coeff>, unsigned>(UnivariatePolynomial<Coeff>(mainVar(), {factor}), 1));
//		}
	}
	return result;
}

}