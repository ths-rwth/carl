/** 
 * @file:   UnivariatePolynomial.tpp
 * @author: Sebastian Junges
 *
 * @since August 26, 2013
 */

#pragma once

#include "UnivariatePolynomial.h"
#include "logging.h"
#include <algorithm>

namespace carl
{

template<typename Coeff>
UnivariatePolynomial<Coeff>::UnivariatePolynomial(Variable::Arg mainVar)
: mMainVar(mainVar)
{
	
}


template<typename Coeff>
UnivariatePolynomial<Coeff>::UnivariatePolynomial(Variable::Arg mainVar, std::initializer_list<Coeff> coefficients)
: mMainVar(mainVar), mCoefficients(coefficients)
{
	
}

template<typename Coeff>
UnivariatePolynomial<Coeff>::UnivariatePolynomial(Variable::Arg mainVar, const std::map<unsigned, Coeff>& coefficients)
: mMainVar(mainVar)
{
	mCoefficients.reserve(coefficients.rbegin()->first);
	for( const std::pair<unsigned, Coeff>& expAndCoeff : coefficients)
	{
		if(expAndCoeff.first != mCoefficients.size() + 1)
		{
			mCoefficients.resize(expAndCoeff.first, (Coeff)0);
		}
		mCoefficients.push_back(expAndCoeff.second);
	}
}


template<typename Coeff>
UnivariatePolynomial<Coeff> UnivariatePolynomial<Coeff>::derivative(unsigned nth ) const
{
	UnivariatePolynomial<Coeff> result(mMainVar);
	result.mCoefficients.reserve(mCoefficients.size()-nth);
	// nth == 1 is most common case and can be implemented more efficient.
	if(nth == 1)
	{
		typename std::vector<Coeff>::const_iterator it = mCoefficients.begin();
		unsigned i = 0;
		for(it += nth; it != mCoefficients.end(); ++it)
		{
			++i;
			result.mCoefficients.push_back(i * *it);
		}
		return result;
	}
	else
	{
		// here we handle nth > 1.
		unsigned c = 1;
		for(unsigned k = 2; k <= nth; ++k)
		{
			c *= k;
		}
		typename std::vector<Coeff>::const_iterator it = mCoefficients.begin();
		unsigned i = nth;
		for(it += nth; it != mCoefficients.end(); ++it)
		{
			result.mCoefficients.push_back(c * *it);
			++i;
			c /= (i - nth);
			c *= i;
		}
		return result;
	}
}


template<typename Coeff>
UnivariatePolynomial<Coeff> UnivariatePolynomial<Coeff>::reduce(const UnivariatePolynomial& divisor) const
{
	assert(degree() >= divisor.degree());
	assert(!divisor.isZero());
	//std::cout << *this << " / " << divisor << std::endl;
	unsigned degdiff = degree() - divisor.degree();
	Coeff factor = lcoeff()/divisor.lcoeff();
	UnivariatePolynomial<Coeff> result(mMainVar);
	result.mCoefficients.reserve(mCoefficients.size()-1);
	unsigned lastNonZero = 0;
	if(degdiff > 0)
	{
		result.mCoefficients.assign(mCoefficients.begin(), mCoefficients.begin() + degdiff);
		lastNonZero = (unsigned)(std::find(result.mCoefficients.rbegin(), result.mCoefficients.rend(), (Coeff)0) - result.mCoefficients.rend()) + 1;
	}
	
	// By construction, the leading coefficient will be zero.
	for(unsigned i=0; i < mCoefficients.size() - degdiff -1; ++i)
	{
		result.mCoefficients.push_back(mCoefficients[i + degdiff] - factor * divisor.mCoefficients[i]);
		if(result.mCoefficients.back() != 0) 
		{
			lastNonZero = i+degdiff+1;
		}
	}
	// strip zeros from the end as we might have pushed zeros.
	result.mCoefficients.resize(lastNonZero, (Coeff)0);
	
	if(result.degree() < divisor.degree())
	{
		return result;
	}
	else 
	{	
		return result.reduce(divisor);
	}
}

template<typename Coeff>
UnivariatePolynomial<Coeff> UnivariatePolynomial<Coeff>::gcd(const UnivariatePolynomial& a, const UnivariatePolynomial& b)
{
	// We want degree(b) <= degree(a).
	if(a.degree() < b.degree()) return gcd_recursive(b,a);
	else return gcd_recursive(a,b);
}


template<typename Coeff>
UnivariatePolynomial<Coeff> UnivariatePolynomial<Coeff>::gcd_recursive(const UnivariatePolynomial& a, const UnivariatePolynomial& b)
{
	if(b.isZero()) return a;
	else return gcd_recursive(b, a.reduce(b));
}

template<typename Coeff>
Coeff UnivariatePolynomial<Coeff>::cauchyBound() const
{
	// We could also use SFINAE, but this gives clearer error messages.
	// Just in case, if we want to use SFINAE, the right statement would be
	// template<typename t = Coefficient, typename std::enable_if<is_field<t>::value, int>::type = 0>
	static_assert(is_field<Coeff>::value, "Cauchy bounds are only defined for field-coefficients");
	Coeff maxCoeff = mCoefficients.front() > 0 ? mCoefficients.front() : -mCoefficients.front();
	for(typename std::vector<Coeff>::const_iterator it = ++mCoefficients.begin(); it != --mCoefficients.end(); ++it)
	{
		if(*it > maxCoeff ) 
		{
			maxCoeff = *it;
		}
		else if( -(*it) > maxCoeff )
		{
			maxCoeff = -*it;
		}
	}
	
	return 1 + maxCoeff/lcoeff();
}


template<typename Coeff>
Coeff UnivariatePolynomial<Coeff>::modifiedCauchyBound() const
{
	// We could also use SFINAE, but this gives clearer error messages.
	// Just in case, if we want to use SFINAE, the right statement would be
	// template<typename t = Coefficient, typename std::enable_if<is_field<t>::value, int>::type = 0>
	static_assert(is_field<Coeff>::value, "Modified Cauchy bounds are only defined for field-coefficients");
	LOG_NOTIMPLEMENTED();
}

template<typename Coeff>
UnivariatePolynomial<Coeff>& UnivariatePolynomial<Coeff>::operator+=(const Coeff& rhs)
{
	if(rhs == (Coeff)0) return *this;
	if(mCoefficients.empty())
	{
		// Adding non-zero rhs to zero.
		mCoefficients.resize(1, rhs);
	}
	else
	{
		mCoefficients.front() += rhs;
		if(mCoefficients.size() == 1 && mCoefficients.front() == (Coeff)0) 
		{
			// Result is zero.
			mCoefficients.clear();
		}
	}
	return *this;
}

template<typename Coeff>
UnivariatePolynomial<Coeff>& UnivariatePolynomial<Coeff>::operator+=(const UnivariatePolynomial& rhs)
{
	assert(mMainVar == rhs.mMainVar);
	if(degree() < rhs.degree())
	{
		for(unsigned i = 0; i < degree(); ++i)
		{
			mCoefficients[i] += rhs.mCoefficients[i];
		}
		mCoefficients.insert(mCoefficients.end(), rhs.mCoefficients.end() - (rhs.degree() - degree()), rhs.mCoefficients.end());
	}
	else
	{
		for(unsigned i = 0; i < rhs.degree(); ++i)
		{
			mCoefficients[i] += rhs.mCoefficients[i];
		}
	}
	return *this;
}


template<typename Coeff>
UnivariatePolynomial<Coeff>& UnivariatePolynomial<Coeff>::operator*=(const Coeff& rhs)
{
	
}

template<typename Coeff>
UnivariatePolynomial<Coeff>& UnivariatePolynomial<Coeff>::operator*=(const UnivariatePolynomial& rhs)
{
	assert(mMainVar == rhs.mMainVar);
}


template<typename Coeff>
UnivariatePolynomial<Coeff>& UnivariatePolynomial<Coeff>::operator/=(const Coeff& rhs)
{
	static_assert(is_field<Coeff>::value, "Division by coefficients is only defined for field-coefficients");
}

template<typename C>
std::ostream& operator<<(std::ostream& os, const UnivariatePolynomial<C>& rhs)
{
	if(rhs.isZero()) return os << "0";
	for(size_t i = 0; i < rhs.mCoefficients.size()-1; ++i )
	{
		os << "(" << rhs.mCoefficients[rhs.mCoefficients.size()-i-1] << ")*" << rhs.mMainVar << "^" << rhs.mCoefficients.size()-i-1 << " + ";
	}
	os << rhs.mCoefficients[0];
	return os;
}
}
