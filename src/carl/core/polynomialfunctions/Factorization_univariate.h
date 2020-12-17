#pragma once

#include "Derivative.h"
#include "Division.h"
#include "GCD_univariate.h"

#include "../logging.h"
#include "../UnivariatePolynomial.h"

namespace carl {

// TODO replace with cocoa implementation

template<typename Coeff>
std::map<uint, UnivariatePolynomial<Coeff>> squareFreeFactorization(const UnivariatePolynomial<Coeff>& p) {
	CARL_LOG_TRACE("carl.core.upoly", "UnivSSF: " << p);
	std::map<uint,UnivariatePolynomial<Coeff>> result;
CLANG_WARNING_DISABLE("-Wtautological-compare")
	assert(!isZero(p)); // TODO what if zero?
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
		assert(!is_constant(p)); // Othewise, the derivative is zero and the next assertion is thrown.
		UnivariatePolynomial<Coeff> b = derivative(p);
		CARL_LOG_TRACE("carl.core.upoly", "UnivSSF: b = " << b);
		UnivariatePolynomial<Coeff> s(p.mainVar());
		UnivariatePolynomial<Coeff> t(p.mainVar());
		assert(!isZero(b));
		UnivariatePolynomial<Coeff> c = carl::extended_gcd(p, b, s, t); // TODO: use gcd instead
		typename IntegralType<Coeff>::type numOfCpf = getNum(c.coprimeFactor());
		if(numOfCpf != 1) // TODO: is this maybe only necessary because the extended_gcd returns a polynomial with non-integer coefficients but it shouldn't?
		{
			c *= Coeff(numOfCpf);
		}
		CARL_LOG_TRACE("carl.core.upoly", "UnivSSF: c = " << c);
		if(isZero(c))
		{
			result.emplace(1, p);
			CARL_LOG_TRACE("carl.core.upoly", "UnivSSF: add the factor (" << p << ")^1");
		}
		else
		{
			UnivariatePolynomial<Coeff> w = carl::divide(p, c).quotient;
			CARL_LOG_TRACE("carl.core.upoly", "UnivSSF: w = " << w);
			UnivariatePolynomial<Coeff> y = carl::divide(b, c).quotient;
			CARL_LOG_TRACE("carl.core.upoly", "UnivSSF: y = " << y);
			UnivariatePolynomial<Coeff> z = y - derivative(w);
			CARL_LOG_TRACE("carl.core.upoly", "UnivSSF: z = " << z);
			uint i = 1;
			while(!isZero(z))
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
				w = carl::divide(w, g).quotient;
				CARL_LOG_TRACE("carl.core.upoly", "UnivSSF: w = " << w);
				y = carl::divide(z, g).quotient;
				CARL_LOG_TRACE("carl.core.upoly", "UnivSSF: y = " << y);
				z = y - derivative(w);
				CARL_LOG_TRACE("carl.core.upoly", "UnivSSF: z = " << z);
			}
			result.emplace(i, w);
			CARL_LOG_TRACE("carl.core.upoly", "UnivSSF: add the factor (" << w << ")^" << i);
		}
	}
	return result;
}

namespace detail {

template<typename Coeff, typename Integer>
UnivariatePolynomial<Coeff> exclude_linear_factors(const UnivariatePolynomial<Coeff>& poly, FactorMap<Coeff>& linearFactors, const Integer& maxInt) {
	CARL_LOG_TRACE("carl.core.upoly", "UnivELF: " << poly );
	UnivariatePolynomial<Coeff> result(poly.mainVar());
	// Exclude the factor x^i from result.
	auto cf = poly.coefficients().begin();
	if(*cf == Coeff(0)) // result is of the form a_n * x^n + ... + a_k * x^k (a>k, k>0)
	{
		uint k = 0;
		while(*cf == Coeff(0))
		{
			assert(cf != poly.coefficients().end());
			++cf;
			++k;
		}
		// Take x^k as a factor.
		auto retVal = linearFactors.emplace(UnivariatePolynomial<Coeff>(poly.mainVar(), {Coeff(0), Coeff(1)}), k);
		CARL_LOG_TRACE("carl.core.upoly", "UnivELF: add the factor (" << retVal.first->first << ")^" << k );
		if(!retVal.second)
		{
			retVal.first->second += k;
		}
		// Construct the remainder:  result := a_n * x^{n-k} + ... + a_{k-1} * x + a_k
		std::vector<Coeff> cfs;
		cfs.reserve(poly.coefficients().size()-k);
		cfs = std::vector<Coeff>(cf, poly.coefficients().end());
		result = UnivariatePolynomial<Coeff>(poly.mainVar(), std::move(cfs));
		CARL_LOG_TRACE("carl.core.upoly", "UnivELF: remainder is  " << result );
	}
	else
	{
		result = poly;
	}
	// Check whether the polynomial is already a linear factor.
	if(!result.isLinearInMainVar())
	{
		// Exclude the factor (x-r)^i, with r rational and r!=0, from result.
		assert(result.coefficients().size() > 1);
		typename IntegralType<Coeff>::type lc = carl::abs(getNum(result.lcoeff()));
		typename IntegralType<Coeff>::type tc = carl::abs(getNum(result.coefficients().front()));
		typename IntegralType<Coeff>::type mi = carl::fromInt<typename IntegralType<Coeff>::type>(maxInt);
		if( maxInt != 0 && (tc > mi || lc > mi) )
		{
			return result;
		}
		Integer lcAsInt = toInt<Integer>(lc);
		Integer tcAsInt = toInt<Integer>(tc);
		Integer halfOfLcAsInt = lcAsInt == 1 ? 1 : lcAsInt/2;
		Integer halfOfTcAsInt = tcAsInt == 1 ? 1 : tcAsInt/2;
		std::vector<std::pair<Integer, Integer>> shiftedTcs;
		bool positive = true;
		bool tcFactorsFound = false;
		std::vector<Integer> tcFactors(1, 1); // TODO: store the divisors of some numbers during compilation
		auto tcFactor = tcFactors.begin();
		bool lcFactorsFound = false;
		std::vector<Integer> lcFactors(1, 1); // TODO: store the divisors of some numbers during compilation
		auto lcFactor = lcFactors.begin();
		while(true)
		{
			CARL_LOG_TRACE("carl.core.upoly", "UnivELF: try rational  " << (positive ? "-" : "") << *tcFactor << "/" << *lcFactor);
			// Check whether the numerator of the rational to consider divides the trailing coefficient of all
			// zero-preserving shifts {result(x+x_0) | for some found x_0 with result(x_0)!=0 and x_0 integer}
			auto shiftedTc = shiftedTcs.begin();
			for(; shiftedTc != shiftedTcs.end(); ++shiftedTc)
			{
				// we need to be careful with overflows in the following lines
				if(maxInt/(*lcFactor) >= shiftedTc->first)
				{
					Integer divisor = (*lcFactor) * shiftedTc->first;
					if( divisor != *tcFactor )
					{
						if( !(divisor < 0 && *tcFactor < 0 && maxInt + divisor >= -(*tcFactor)) && !(divisor > 0 && *tcFactor > 0 && maxInt - divisor >= *tcFactor ) )
						{
							if( divisor > *tcFactor )
							{
								divisor = divisor - *tcFactor;
							}
							else
							{
								divisor = *tcFactor - divisor;
							}
							if(carl::mod(shiftedTc->second, divisor) != 0)
							{
								break;
							}
						}
					}	
				}
			}
			if(shiftedTc == shiftedTcs.end())
			{
				Coeff posRatZero = carl::fromInt<typename IntegralType<Coeff>::type>(*tcFactor) / carl::fromInt<typename IntegralType<Coeff>::type>(*lcFactor);
				if (!positive) posRatZero = -posRatZero;
				CARL_LOG_TRACE("carl.core.upoly", "UnivELF: consider possible non zero rational factor  " << posRatZero);
				Coeff image = result.syntheticDivision(posRatZero);
				if (carl::isZero(image)) {
					// Remove all linear factor with the found zero from result.
					UnivariatePolynomial<Coeff> linearFactor(result.mainVar(), {-posRatZero, Coeff(1)});
					while (carl::isZero(image)) {
						auto retVal = linearFactors.emplace(linearFactor, 1);
						CARL_LOG_TRACE("carl.core.upoly", "UnivELF: add the factor (" << linearFactor << ")^" << 1 );
						if(!retVal.second)
						{
							++retVal.first->second;
						}
						// Check whether result is a linear factor now.
						if(result.isLinearInMainVar() <= 1)
						{
							goto LinearFactorRemains;
						}
						image = result.syntheticDivision(posRatZero);
					}
				}
				else if(isInteger(posRatZero))
				{
					// Add a zero-preserving shift.
					assert(isInteger(image));
					typename IntegralType<Coeff>::type imageInt = carl::abs(getNum(image));
					using IntNumberType = typename IntegralType<typename UnderlyingNumberType<Coeff>::type>::type;
					if( imageInt <= carl::fromInt<IntNumberType>(maxInt) )
					{
						CARL_LOG_TRACE("carl.core.upoly", "UnivELF: new shift with " << getNum(posRatZero) << " to " << carl::abs(getNum(image)));
						shiftedTcs.push_back(std::pair<Integer, Integer>(toInt<Integer>(getNum(posRatZero)), toInt<Integer>(carl::abs(getNum(image)))));
					}
				}
			}
			// Find the next numerator-denominator combination.
			if(shiftedTc == shiftedTcs.end() && positive)
			{
				positive = false;
			}
			else
			{
				positive = true;
				if(lcFactorsFound)
				{
					++lcFactor;
				}
				else
				{
					lcFactors.push_back(lcFactors.back());
					while(lcFactors.back() <= halfOfLcAsInt)
					{
						++lcFactors.back();
						if(carl::mod(lcAsInt, lcFactors.back()) == 0)
						{
							break;
						}
					}
					if(lcFactors.back() > halfOfLcAsInt)
					{
						lcFactors.pop_back();
						lcFactorsFound = true;
						lcFactor = lcFactors.end();
					}
					else
					{
						lcFactor = --(lcFactors.end());
					}
				}
				if(lcFactor == lcFactors.end())
				{
					if(tcFactorsFound)
					{
						++tcFactor;
					}
					else
					{
						tcFactors.push_back(tcFactors.back());
						while(tcFactors.back() <= halfOfTcAsInt)
						{
							++(tcFactors.back());
							if(carl::mod(tcAsInt, tcFactors.back()) == 0)
							{
								break;
							}
						}
						if(tcFactors.back() > halfOfTcAsInt)
						{
							tcFactors.pop_back();
							tcFactor = tcFactors.end();
						}
						else
						{
							tcFactor = --(tcFactors.end());
						}
					}
					if(tcFactor == tcFactors.end())
					{
						Coeff factor = result.coprimeFactor();
						if (!carl::isOne(factor)) {
							result *= factor;
							CARL_LOG_TRACE("carl.core.upoly", "UnivFactor: add the factor (" << UnivariatePolynomial<Coeff>(result.mainVar(), std::initializer_list<Coeff>({(Coeff)1/factor})) << ")^" << 1 );
							// Add the constant factor to the factors.
							if( is_constant(linearFactors.begin()->first) )
							{
								factor = Coeff(1) / factor;
								factor *= linearFactors.begin()->first.lcoeff();
								linearFactors.erase(linearFactors.begin());
							}
							linearFactors.insert(linearFactors.begin(), std::pair<UnivariatePolynomial<Coeff>, uint>(UnivariatePolynomial<Coeff>(result.mainVar(), std::initializer_list<Coeff>({factor})), 1));
						}
						return result;
					}
					lcFactor = lcFactors.begin();
				}
			}
		}
		assert(false);
	}
LinearFactorRemains:
	Coeff factor = result.lcoeff();
	if (!carl::isOne(factor)) {
		result /= factor;
		CARL_LOG_TRACE("carl.core.upoly", "UnivFactor: add the factor (" << UnivariatePolynomial<Coeff>(result.mainVar(), factor) << ")^" << 1 );
		// Add the constant factor to the factors.
		if( !linearFactors.empty() && is_constant(linearFactors.begin()->first) )
		{
			if( carl::isZero(linearFactors.begin()->first) )
				factor = Coeff( 0 );
			else
				factor *= linearFactors.begin()->first.lcoeff();
			linearFactors.erase(linearFactors.begin());
		}
		linearFactors.insert(linearFactors.begin(), std::pair<UnivariatePolynomial<Coeff>, uint>(UnivariatePolynomial<Coeff>(result.mainVar(), factor), 1));
	}
	auto retVal = linearFactors.emplace(result, 1);
	CARL_LOG_TRACE("carl.core.upoly", "UnivELF: add the factor (" << result << ")^" << 1 );
	if(!retVal.second)
	{
		++retVal.first->second;
	}
	return UnivariatePolynomial<Coeff>(result.mainVar(), Coeff(1));
}

}

template<typename Coeff>
FactorMap<Coeff> factorization(const UnivariatePolynomial<Coeff>& p) {
    CARL_LOG_TRACE("carl.core.upoly", "UnivFactor: " << p);
	FactorMap<Coeff> result;
	if(is_constant(p)) // Constant.
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
	remainingPoly = detail::exclude_linear_factors(remainingPoly, result, static_cast<carl::sint>(INT_MAX));
	assert(!is_constant(remainingPoly) || remainingPoly.lcoeff() == (Coeff)1);
	if(!is_constant(remainingPoly))
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
			if(!is_constant(expFactorPair->second) || !carl::isOne(expFactorPair->second.lcoeff()))
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