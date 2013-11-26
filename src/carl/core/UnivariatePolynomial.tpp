/** 
 * @file:   UnivariatePolynomial.tpp
 * @author: Sebastian Junges
 *
 * @since August 26, 2013
 */

#pragma once
#include "UnivariatePolynomial.h"
#include <algorithm>
#include "../util/SFINAE.h"
#include "logging.h"

namespace carl
{

template<typename Coeff>
UnivariatePolynomial<Coeff>::UnivariatePolynomial(Variable::Arg mainVar)
: mMainVar(mainVar)
{
	
}
template<typename Coeff>
UnivariatePolynomial<Coeff>::UnivariatePolynomial(Variable::Arg mainVar, const Coeff& c, exponent e) :
mMainVar(mainVar),
mCoefficients(e+1,(Coeff)c-c) // We would like to use 0 here, but Coeff(0) is not always constructable (some methods need more parameter)
{
	if(c != 0)
	{
		mCoefficients[e] = c;
	}
	else
	{
		mCoefficients.clear();
	}
}

template<typename Coeff>
UnivariatePolynomial<Coeff>::UnivariatePolynomial(Variable::Arg mainVar, std::initializer_list<Coeff> coefficients)
: mMainVar(mainVar), mCoefficients(coefficients)
{
	
}

template<typename Coeff>
UnivariatePolynomial<Coeff>::UnivariatePolynomial(Variable::Arg mainVar, const std::vector<Coeff>& coefficients)
: mMainVar(mainVar), mCoefficients(coefficients)
{
	
}

template<typename Coeff>
UnivariatePolynomial<Coeff>::UnivariatePolynomial(Variable::Arg mainVar, std::vector<Coeff>&& coefficients)
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
std::map<UnivariatePolynomial<Coeff>, unsigned> UnivariatePolynomial<Coeff>::factorization() const
{
    std::map<UnivariatePolynomial, unsigned> result;
    if( mCoefficients.size() == 1 )
    {
        result.insert(std::pair<UnivariatePolynomial, unsigned>(*this, 1));
    }
    else if( mCoefficients.size() > 1 )
    {
        UnivariatePolynomial<Coeff> remainingPoly;
        // Exclude the factors  (x-r)^i  with  r rational.
        remainingPoly = excludeLinearFactors( *remainingPoly, result );
        // Calculate the square free factorization.
        std::map<unsigned, UnivariatePolynomial<Coeff>> sff = remainingPoly->squareFreeFactorization();
        for(auto expFactorPair = sff.begin(); expFactorPair != sff.end(); ++expFactorPair)
        {
            auto retVal = result.insert(std::pair<UnivariatePolynomial, unsigned>(expFactorPair->second, expFactorPair->first));
            if(!retVal->second)
            {
                retVal->first->second += expFactorPair->first;
            }
        }
    }
    return result;
}

template<typename Coeff>
UnivariatePolynomial<Coeff> UnivariatePolynomial<Coeff>::excludeLinearFactors(const UnivariatePolynomial<Coeff>& _poly, std::map<UnivariatePolynomial<Coeff>, unsigned>& _linearFactors)
{
    UnivariatePolynomial<Coeff> result;
    // Exclude the factor x^i.
    auto cf = _poly.coefficients().begin();
    if(*cf == 0) // Polynomial is of the form a_n * x^n + ... + a_k * x^k (a>k, k>0)
    {
        unsigned k = 0;
        while(*cf == 0)
        {
            assert(cf != _poly.coefficients().end());
            ++cf;
            ++k;
        }
        // Take x^k as a factor
        _linearFactors.insert(std::pair<unsigned, UnivariatePolynomial<Coeff>>(k, _poly.mainVar()));
        // Construct the remainder  a_n * x^{n-k} + ... + a_{k-1} * x + a_k
        std::vector<Coeff> cfs;
        cfs.reserve(_poly.coefficients().size()-k);
        cfs = std::vector<Coeff>(cf, _poly.coefficients().end());
        result = UnivariatePolynomial<Coeff>(_poly.mainVar(), std::move(cfs));
    }
    else
    {
        result = _poly;
    }
    // Exclude the factor (x-r)^i  with  r rational and r!=0.
    std::set<Coeff> possibleRationalRoots;
    do
    {
        if(result->degree() <= 1)
        {
            auto retVal = _linearFactors.insert(std::pair<UnivariatePolynomial, unsigned>(result, 1));
            if(!retVal->second)
            {
                ++retVal->first->second;
            }
        }
        int lcAsInt = toInt(getNum(result.lcoeff()));
        std::vector<unsigned> lcFactorization;
        if( lcAsInt <= INT_MAX )
        {
            lcFactorization = calculateFactorization((unsigned)std::abs(lcAsInt));
        }
        assert(result.coefficients().size() > 1);
        assert(result.coefficients().front() != 0);
        int tcAsInt = toInt(getNum(result.coefficients().front()));
        std::vector<unsigned> tcFactorization;
        if( tcAsInt <= INT_MAX )
        {
            tcFactorization = calculateFactorization((unsigned)std::abs(tcAsInt));
        }
    }
    while( !possibleRationalRoots.empty() );
    return result;
}

template<typename Coeff>
std::map<unsigned, UnivariatePolynomial<Coeff>> UnivariatePolynomial<Coeff>::squareFreeFactorization() const
{
    std::map<unsigned,UnivariatePolynomial> result;
	if(characteristic<Coeff>::value != 0 && degree() >= characteristic<Coeff>::value)
    {
        result.insert(std::pair<unsigned, UnivariatePolynomial<Coeff>>(1, *this));
    }
    else
    {
        UnivariatePolynomial<Coeff> b = this->derivative();
        UnivariatePolynomial<Coeff> c = gcd(*this, b);
        if(c.isZero())
        {
            result.insert(std::pair<unsigned, UnivariatePolynomial<Coeff>>(1, *this));
        }
        else
        {
            UnivariatePolynomial<Coeff> w = (*this).divide(c);
            UnivariatePolynomial<Coeff> y = b.divide(c);
            UnivariatePolynomial<Coeff> z = y-w.derivative();
            unsigned i = 1;
            while(!z.isZero())
            {
                UnivariatePolynomial<Coeff> g = gcd(w, z);
                assert(result.find(i) == result.end());
                result.insert(std::pair<unsigned, UnivariatePolynomial<Coeff>>(i, g));
                ++i;
                w = w.divide(g);
                y = z.divide(g);
                z = y - w.derivative();
            }
            result.insert(std::pair<unsigned, UnivariatePolynomial<Coeff>>(i, w));
        }
    }
    return result;
}

template<typename Coeff>
Coeff UnivariatePolynomial<Coeff>::evaluate(const Coeff& value) const 
{
	Coeff result(0);
	Coeff var = 1;
	for(const Coeff& coeff : mCoefficients)
	{
		result += (coeff * var);
		var *= value;
	}
    return result;
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
		lastNonZero = (unsigned)(std::find(result.mCoefficients.rbegin(), result.mCoefficients.rend(), 0) - result.mCoefficients.rend()) + 1;
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
	result.mCoefficients.resize(lastNonZero, Coeff(0));
	
	if(result.degree() < divisor.degree())
	{
		return result;
	}
	else 
	{	
		return result.reduce(divisor);
	}
}

/**
 * See Algorithm 2.2 in GZL92.
 * @param a
 * @param b
 * @param s
 * @param t
 * @return 
 */
template<typename Coeff>
UnivariatePolynomial<Coeff> UnivariatePolynomial<Coeff>::extended_gcd(const UnivariatePolynomial& a, const UnivariatePolynomial& b, UnivariatePolynomial& s, UnivariatePolynomial& t)
{
	assert(a.mMainVar == b.mMainVar);
	assert(a.mMainVar == s.mMainVar);
	assert(a.mMainVar == t.mMainVar);
	
	const Variable& x = a.mMainVar;
	//TODO Normalize!
	UnivariatePolynomial<Coeff> c = a;
	//TODO Normalize!
	UnivariatePolynomial<Coeff> d = b;
	UnivariatePolynomial<Coeff> c2(x);
	UnivariatePolynomial<Coeff> c1 = c2 + (Coeff)1;
	UnivariatePolynomial<Coeff> d1(x);
	UnivariatePolynomial<Coeff> d2 = d1 + (Coeff)1;
	while(!d.isZero())
	{
		DivisionResult<UnivariatePolynomial<Coeff>> divres = c.divide(d);
		UnivariatePolynomial r1 = c1 - divres.quotient*d1;
		UnivariatePolynomial r2 = c2 - divres.quotient*d2;
		c = d;
		c1 = d1;
		c2 = d2;
		d = divres.remainder;
		d1 = r1;
		d2 = r2;
	}
	s = c1;
	t = c2;
	return c;
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
        Coeff absOfCoeff = abs( *it );
		if( absOfCoeff > maxCoeff ) 
		{
			maxCoeff = absOfCoeff;
		}
	}
	
	return 1 + maxCoeff/abs( lcoeff() );
}


template<typename Coeff>
template<typename Integer>
UnivariatePolynomial<Integer> UnivariatePolynomial<Coeff>::coprimeCoefficients() const
{
	static_assert(is_number<Coeff>::value, "We can only make integer coefficients if we have a number type before.");
	typename std::vector<Coeff>::const_iterator it = mCoefficients.begin();
	typename IntegralT<Coeff>::type num = getNum(*it);
	typename IntegralT<Coeff>::type den = getDenom(*it);
	for(++it; it != mCoefficients.end(); ++it)
	{
		num = gcd(num, getNum(*it));
		den = lcm(den, getDenom(*it));
	}
	Coeff factor = den/num;
	// Notice that even if factor is 1, we create a new polynomial
	UnivariatePolynomial<Integer> result;
	result.mCoefficients.reserve(mCoefficients.size());
	for(const Coeff& coeff : mCoefficients)
	{
		result.mCoefficients.push_back(coeff * factor);
	}
	return result;
}	

template<typename Coeff>
DivisionResult<UnivariatePolynomial<Coeff>> UnivariatePolynomial<Coeff>::divide(const UnivariatePolynomial<Coeff>& divisor) const
{
	
	assert(!divisor.isZero());
	DivisionResult<UnivariatePolynomial<Coeff>> result(UnivariatePolynomial<Coeff>(mMainVar), *this);
	result.quotient.mCoefficients.resize(1 + degree()-divisor.degree(),(Coeff)0);
	
	do
	{
		Coeff factor = result.remainder.lcoeff()/divisor.lcoeff();
		unsigned degdiff = result.remainder.degree() - divisor.degree();
		result.remainder -= UnivariatePolynomial<Coeff>(mMainVar, factor, degdiff) * divisor;
		result.quotient.mCoefficients[degdiff] += factor;
	}
	while(divisor.degree() <= result.remainder.degree());
	return result;
	
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
template<typename C, EnableIf<is_instantiation_of<GFNumber, C>>>
UnivariatePolynomial<typename IntegralT<Coeff>::type> UnivariatePolynomial<Coeff>::toIntegerDomain() const
{
	UnivariatePolynomial<typename IntegralT<Coeff>::type> res(mMainVar);
	res.mCoefficients.reserve(mCoefficients.size());
	for(const Coeff& c : mCoefficients)
	{
		assert(isInteger(c));
		res.mCoefficients.push_back(c.representingInteger());
	}
	res.stripLeadingZeroes();
}

template<typename Coeff>
template<typename C, DisableIf<is_instantiation_of<GFNumber, C>>>
UnivariatePolynomial<typename IntegralT<Coeff>::type> UnivariatePolynomial<Coeff>::toIntegerDomain() const
{
	UnivariatePolynomial<typename IntegralT<Coeff>::type> res(mMainVar);
	res.mCoefficients.reserve(mCoefficients.size());
	for(const Coeff& c : mCoefficients)
	{
		assert(isInteger(c));
		res.mCoefficients.push_back((typename IntegralT<Coeff>::type)c);
	}
	res.stripLeadingZeroes();
}

template<typename Coeff>
//template<typename T = Coeff, EnableIf<!std::is_same<IntegralT<Coeff>, bool>::value>>
UnivariatePolynomial<GFNumber<typename IntegralT<Coeff>::type>> UnivariatePolynomial<Coeff>::toFiniteDomain(const GaloisField<typename IntegralT<Coeff>::type>* galoisField) const
{
	UnivariatePolynomial<GFNumber<typename IntegralT<Coeff>::type>> res(mMainVar);
	res.mCoefficients.reserve(mCoefficients.size());
	for(const Coeff& c : mCoefficients)
	{
		assert(isInteger(c));
		res.mCoefficients.push_back(GFNumber<typename IntegralT<Coeff>::type>(c,galoisField));
	}
	res.stripLeadingZeroes();
	return res;
	
}

template<typename Coeff>
UnivariatePolynomial<Coeff> UnivariatePolynomial<Coeff>::operator -() const
{
	UnivariatePolynomial result(mMainVar);
	result.mCoefficients.resize(mCoefficients.size(), (Coeff)0);
	std::transform(mCoefficients.begin(), mCoefficients.end(), result.mCoefficients.begin(), 
				 [](const Coeff& c) -> Coeff {return -c;});
	return result;		 
}

template<typename Coeff>
UnivariatePolynomial<Coeff>& UnivariatePolynomial<Coeff>::operator+=(const Coeff& rhs)
{
	if(rhs == 0) return *this;
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
		for(unsigned i = 0; i < mCoefficients.size(); ++i)
		{
			mCoefficients[i] += rhs.mCoefficients[i];
		}
		mCoefficients.insert(mCoefficients.end(), rhs.mCoefficients.end() - (rhs.degree() - degree()), rhs.mCoefficients.end());
	}
	else
	{
		for(unsigned i = 0; i < rhs.mCoefficients.size(); ++i)
		{
			mCoefficients[i] += rhs.mCoefficients[i]; 
		}
	}
	stripLeadingZeroes();
	return *this;
}

template<typename C>
UnivariatePolynomial<C> operator+(const UnivariatePolynomial<C>& lhs, const UnivariatePolynomial<C>& rhs)
{
	UnivariatePolynomial<C> res(lhs);
	res += rhs;
	return res;
}

template<typename C>
UnivariatePolynomial<C> operator+(const UnivariatePolynomial<C>& lhs, const C& rhs)
{
	UnivariatePolynomial<C> res(lhs);
	res += rhs;
	return res;
}

template<typename C>
UnivariatePolynomial<C> operator+(const C& lhs, const UnivariatePolynomial<C>& rhs)
{
	return rhs + lhs;
}
	

template<typename Coeff>
UnivariatePolynomial<Coeff>& UnivariatePolynomial<Coeff>::operator-=(const Coeff& rhs)
{
	LOG_INEFFICIENT();
	return *this += -rhs;
}

template<typename Coeff>
UnivariatePolynomial<Coeff>& UnivariatePolynomial<Coeff>::operator-=(const UnivariatePolynomial& rhs)
{
	LOG_INEFFICIENT();
	return *this += -rhs;
}


template<typename C>
UnivariatePolynomial<C> operator-(const UnivariatePolynomial<C>& lhs, const UnivariatePolynomial<C>& rhs)
{
	UnivariatePolynomial<C> res(lhs);
	res -= rhs;
	return res;
}

template<typename C>
UnivariatePolynomial<C> operator-(const UnivariatePolynomial<C>& lhs, const C& rhs)
{
	UnivariatePolynomial<C> res(lhs);
	res -= rhs;
	return res;
}

template<typename C>
UnivariatePolynomial<C> operator-(const C& lhs, const UnivariatePolynomial<C>& rhs)
{
	return rhs - lhs;
}

template<typename Coeff>
UnivariatePolynomial<Coeff>& UnivariatePolynomial<Coeff>::operator*=(const Coeff& rhs)
{
	if(rhs == (Coeff)0)
	{
		mCoefficients.clear();
		return *this;
	}
	for(Coeff& c : mCoefficients)
	{
		c *= rhs;
	}
	return *this;		
}


template<typename Coeff>
template<typename I, DisableIf<std::is_same<Coeff, I>>...>
UnivariatePolynomial<Coeff>& UnivariatePolynomial<Coeff>::operator*=(const typename IntegralT<Coeff>::type& rhs)
{
	static_assert(std::is_same<Coeff, I>::value, "Do not provide template parameters");
	if(rhs == (I)0)
	{
		mCoefficients.clear();
		return *this;
	}
	for(Coeff& c : mCoefficients)
	{
		c *= rhs;
	}
	return *this;		
}

template<typename Coeff>
UnivariatePolynomial<Coeff>& UnivariatePolynomial<Coeff>::operator*=(const UnivariatePolynomial& rhs)
{
	assert(mMainVar == rhs.mMainVar);
	if(rhs.isZero())
	{
		mCoefficients.clear();
		return *this;
	}
	
	std::vector<Coeff> newCoeffs; 
	newCoeffs.reserve(mCoefficients.size() + rhs.mCoefficients.size());
	for(unsigned e = 0; e < mCoefficients.size() + rhs.mCoefficients.size(); ++e)
	{
		newCoeffs.push_back((Coeff)0);
		for(unsigned i = 0; i < mCoefficients.size() && i <= e; ++i)
		{
			if(e - i < rhs.mCoefficients.size())
			{
				newCoeffs.back() += mCoefficients[i] * rhs.mCoefficients[e-i];
			}
		}
	}
	mCoefficients.swap(newCoeffs);
	return *this;
}


template<typename C>
UnivariatePolynomial<C> operator*(const UnivariatePolynomial<C>& lhs, const UnivariatePolynomial<C>& rhs)
{
	UnivariatePolynomial<C> res(lhs);
	res *= rhs;
	return res;
}

template<typename C>
UnivariatePolynomial<C> operator*(const UnivariatePolynomial<C>& lhs, const C& rhs)
{
	UnivariatePolynomial<C> res(lhs);
	res *= rhs;
	return res;
}

template<typename C>
UnivariatePolynomial<C> operator*(const C& lhs, const UnivariatePolynomial<C>& rhs)
{
	return rhs * lhs;
}

template<typename C>
UnivariatePolynomial<C> operator*(const UnivariatePolynomial<C>& lhs, const typename IntegralT<C>::type& rhs)
{
	UnivariatePolynomial<C> res(lhs);
	res *= rhs;
	return res;
}

template<typename C>
UnivariatePolynomial<C> operator*(const typename IntegralT<C>::type& lhs, const UnivariatePolynomial<C>& rhs)
{
	return rhs * lhs;
}


template<typename Coeff>
UnivariatePolynomial<Coeff>& UnivariatePolynomial<Coeff>::operator/=(const Coeff& rhs)
{
	if(!is_field<Coeff>::value)
	{
		LOGMSG_WARN("carl.core", "Division by coefficients is only defined for field-coefficients");
	}
	assert(rhs != (Coeff)0);
	for(Coeff& c : mCoefficients)
	{
		c /= rhs;
	}
	return *this;		
}


template<typename C>
bool operator==(const UnivariatePolynomial<C>& lhs, const UnivariatePolynomial<C>& rhs)
{
	if(lhs.mMainVar == rhs.mMainVar)
	{
		return lhs.mCoefficients == rhs.mCoefficients;
	}
	else
	{
		// in different variables, polynomials can still be equal if constant.
		if(lhs.isZero() && rhs.isZero()) return true;
		if(lhs.isConstant() && rhs.isConstant() && lhs.lcoeff() == rhs.lcoeff()) return true;
		return false;
	}
}

template<typename C>
bool operator!=(const UnivariatePolynomial<C>& lhs, const UnivariatePolynomial<C>& rhs)
{
	return !(lhs == rhs);
}

template<typename C>
std::ostream& operator<<(std::ostream& os, const UnivariatePolynomial<C>& rhs)
{
	if(rhs.isZero()) return os << "0";
	for(size_t i = 0; i < rhs.mCoefficients.size()-1; ++i )
	{
		const C& c = rhs.mCoefficients[rhs.mCoefficients.size()-i-1];
		if(c != 0)
		{
			os << "(" << c << ")*" << rhs.mMainVar << "^" << rhs.mCoefficients.size()-i-1 << " + ";
		}
	}
	os << rhs.mCoefficients[0];
	return os;
}
}
