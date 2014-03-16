/** 
 * @file:   RationalFunction.h
 * @author: Sebastian Junges
 */

#pragma once

namespace carl
{
	
template<typename Pol>
class RationalFunction
{
	Pol mNominator;
	Pol mDenominator;
	
public:
	explicit RationalFunction(const Pol& p)
	: mNominator(p), mDenominator(1)
	{
		
	}
	
	RationalFunction(const Pol& nom, const Pol& denom)
	: mNominator(nom), mDenominator(denom)
	{
		
	}
	
	const Pol& nominator() const
	{
		return mNominator;
	}
	
	const Pol& denominator() const
	{
		return mDenominator;
	}
	
	void simplify() 
	{
		*this =/ carl::gcd(mNominator, mDenominator);
	}
	
	
	RationalFunction& operator+=(const RationalFunction& rhs);
	
	RationalFunction& operator-=(const RationalFunction& rhs);
	
	RationalFunction& operator*=(const RationalFunction& rhs);
	
	RationalFunction& operator/=(const RationalFunction& rhs);
	
};

	template<typename Pol>
	RationalFunction<Pol> operator+(const RationalFunction<Pol>& lhs, const RationalFunction<Pol>& rhs);
	RationalFunction<Pol> operator-(const RationalFunction<Pol>& lhs, const RationalFunction<Pol>& rhs);
	RationalFunction<Pol> operator*(const RationalFunction<Pol>& lhs, const RationalFunction<Pol>& rhs);
	RationalFunction<Pol> operator/(const RationalFunction<Pol>& lhs, const RationalFunction<Pol>& rhs);
	RationalFunction<Pol> operator-(const RationalFunction<Pol>& lhs);
	
	std::ostream& operator<<(std::ostream& os, const RationalFunction& rhs);
}

#include "RationalFunction.tpp"
