/** 
 * @file:   RationalFunction.tpp
 * @author: Sebastian Junges
 *
 * @since March 16, 2014
 */

#include "RationalFunction.h"


#pragma once

namespace carl
{
	template<typename Pol>
	bool operator==(const RationalFunction<Pol>& lhs, const RationalFunction<Pol>& rhs)
	{
		return lhs.nominator() == rhs.nominator() && lhs.denominator() == rhs.denominator();
	}
	
	template<typename Pol>
	bool operator!=(const RationalFunction<Pol>& lhs, const RationalFunction<Pol>& rhs)
	{
		return !(lhs == rhs);
	}
	
	
	template<typename Pol>
	RationalFunction<Pol>& RationalFunction<Pol>::operator+=(const RationalFunction<Pol>& rhs)
	{
		Pol leastCommonMultiple = carl::lcm(this->mDenominator, rhs.mDenominator);
		mNominator = this->mNominator * quotient(leastCommonMultiple,this->mDenominator) + rhs.mNominator * quotient(leastCommonMultiple,rhs.mDenominator);
		mDenominator = leastCommonMultiple;
		return *this;
	}

	template<typename Pol>
	RationalFunction<Pol>& RationalFunction<Pol>::operator+=(const Pol& rhs)
	{
		mNominator += rhs * mDenominator;
		return *this;
	}	
	
	//template<typename Pol>
	//RationalFunction<Pol>& RationalFunction<Pol>::operator+=(const Te)
	
	template<typename Pol>
	RationalFunction<Pol>& RationalFunction<Pol>::operator-=(const RationalFunction<Pol>& rhs)
	{
		Pol leastCommonMultiple = carl::lcm(this->mDenominator, rhs.mDenominator);
		mNominator = this->mNominator * quotient(leastCommonMultiple,this->mDenominator) - rhs.mNominator * quotient(leastCommonMultiple,rhs.mDenominator);
		mDenominator = leastCommonMultiple;
		return *this;
	}
	
	template<typename Pol>
	RationalFunction<Pol>& RationalFunction<Pol>::operator-=(Variable::Arg rhs)
	{
		mNominator -= rhs * mDenominator;
		return *this;
	}
	
	template<typename Pol>
	RationalFunction<Pol>& RationalFunction<Pol>::operator*=(const RationalFunction<Pol>& rhs)
	{
		mNominator *= rhs.mNominator;
		mDenominator *= rhs.mDenominator;
		return *this;
	}
	
	template<typename Pol>
	RationalFunction<Pol>& RationalFunction<Pol>::operator*=(Variable::Arg rhs)
	{
		mNominator *= rhs;
		return *this;
	}
	
	
	template<typename Pol>
	RationalFunction<Pol>& RationalFunction<Pol>::operator/=(const RationalFunction<Pol>& rhs)
	{
		mNominator *= rhs.mDenominator;
		mDenominator *= rhs.mNominator;
		return *this;
	}
	
	template<typename Pol>
	RationalFunction<Pol>& RationalFunction<Pol>::operator/=(const Pol& rhs)
	{
		mDenominator *= rhs;
		return *this;
	}
	
	template<typename Pol>
	RationalFunction<Pol>& RationalFunction<Pol>::operator/=(unsigned long rhs)
	{
		mDenominator *= rhs;
		return *this;
	}
	
	template<typename Pol>
	RationalFunction<Pol> operator+(const RationalFunction<Pol>& lhs, const RationalFunction<Pol>& rhs)
	{
		RationalFunction<Pol> res(lhs);
		return res += rhs;
	}
	template<typename Pol>
	RationalFunction<Pol> operator-(const RationalFunction<Pol>& lhs, const RationalFunction<Pol>& rhs)
	{
		RationalFunction<Pol> res(lhs);
		return res -= rhs;
	}
	
	template<typename Pol>
	RationalFunction<Pol> operator-(const RationalFunction<Pol>& lhs, Variable::Arg rhs)
	{
		RationalFunction<Pol> res(lhs);
		return res -= rhs;
	}
	
	template<typename Pol>
	RationalFunction<Pol> operator*(const RationalFunction<Pol>& lhs, const RationalFunction<Pol>& rhs)
	{
		RationalFunction<Pol> res(lhs);
		return res *= rhs;
	}
	template<typename Pol>
	RationalFunction<Pol> operator*(const RationalFunction<Pol>& lhs, Variable::Arg rhs)
	{
		RationalFunction<Pol> res(lhs);
		return res *= rhs;
	}
	
	template<typename Pol>
	RationalFunction<Pol> operator/(const RationalFunction<Pol>& lhs, const RationalFunction<Pol>& rhs)
	{
		RationalFunction<Pol> res(lhs);
		return res /= rhs;
	}
	template<typename Pol>
	RationalFunction<Pol> operator/(const RationalFunction<Pol>& lhs, unsigned long rhs)
	{
		RationalFunction<Pol> res(lhs);
		return res /= rhs;
	}
	
	template<typename Pol>
	RationalFunction<Pol> operator-(const RationalFunction<Pol>& rhs)
	{
		return RationalFunction<Pol>(-rhs.nominator(), rhs.denominator());
	}
	
	template<typename Pol>
	std::ostream& operator<<(std::ostream& os, const RationalFunction<Pol>& rhs)
	{
		return os << rhs.nominator() << "/" << rhs.denominator();
	}
}