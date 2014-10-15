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
	template<typename Pol, bool AS>
	bool operator==(const RationalFunction<Pol, AS>& lhs, const RationalFunction<Pol, AS>& rhs)
	{
		return lhs.nominator() == rhs.nominator() && lhs.denominator() == rhs.denominator();
	}
	
	template<typename Pol, bool AS>
	bool operator<(const RationalFunction<Pol, AS>& lhs, const RationalFunction<Pol, AS>& rhs)
	{
		return lhs.nominator() < rhs.nominator() && (lhs.nominator() == rhs.nominator() && lhs.denominator() < rhs.denominator());
	}
	
	template<typename Pol, bool AS>
	bool operator!=(const RationalFunction<Pol, AS>& lhs, const RationalFunction<Pol, AS>& rhs)
	{
		return !(lhs == rhs);
	}
	
	
	template<typename Pol, bool AS>
	RationalFunction<Pol, AS>& RationalFunction<Pol, AS>::operator+=(const RationalFunction<Pol, AS>& rhs)
	{
		mIsSimplified = false;
		Pol leastCommonMultiple = carl::lcm(this->mDenominator, rhs.mDenominator);
		mNominator = this->mNominator * quotient(leastCommonMultiple,this->mDenominator) + rhs.mNominator * quotient(leastCommonMultiple,rhs.mDenominator);
		mDenominator = leastCommonMultiple;
		if(AS) eliminateCommonFactor();
		return *this;
	}
	
	template<typename Pol, bool AS>
	RationalFunction<Pol, AS>& RationalFunction<Pol, AS>::operator+=(const Pol& rhs)
	{
		mIsSimplified = false;
		mNominator += rhs * mDenominator;
		if(AS) eliminateCommonFactor();
		return *this;
	}	

	template<typename Pol, bool AS>
    template<typename P, DisableIf<needs_cache<P>>>
	RationalFunction<Pol, AS>& RationalFunction<Pol, AS>::operator+=(Variable::Arg rhs)
	{
		mIsSimplified = false;
		mNominator += rhs * mDenominator;
		if(AS) eliminateCommonFactor();
		return *this;
	}
	
	template<typename Pol, bool AS>
	RationalFunction<Pol, AS>& RationalFunction<Pol, AS>::operator+=(const typename Pol::CoeffType& rhs)
	{
		mIsSimplified = false;
		mNominator += rhs * mDenominator;
		if(AS) eliminateCommonFactor();
		return *this;
	}
	
	template<typename Pol, bool AS>
	RationalFunction<Pol, AS>& RationalFunction<Pol, AS>::operator-=(const RationalFunction<Pol, AS>& rhs)
	{
		mIsSimplified = false;
		Pol leastCommonMultiple = carl::lcm(this->mDenominator, rhs.mDenominator);
		mNominator = this->mNominator * quotient(leastCommonMultiple,this->mDenominator) - rhs.mNominator * quotient(leastCommonMultiple,rhs.mDenominator);
		mDenominator = leastCommonMultiple;
		if(AS) eliminateCommonFactor();
		return *this;
	}
	
	template<typename Pol, bool AS>
	RationalFunction<Pol, AS>& RationalFunction<Pol, AS>::operator-=(const Pol& rhs)
	{
		mIsSimplified = false;
		mNominator = this->mNominator + rhs.mNominator * this->mDenominator;
		if(AS) eliminateCommonFactor();
		return *this;
	}	

	template<typename Pol, bool AS>
    template<typename P, DisableIf<needs_cache<P>>>
	RationalFunction<Pol, AS>& RationalFunction<Pol, AS>::operator-=(Variable::Arg rhs)
	{
		mIsSimplified = false;
		mNominator -= rhs * mDenominator;
		if(AS) eliminateCommonFactor();
		return *this;
	}
	
	template<typename Pol, bool AS>
	RationalFunction<Pol, AS>& RationalFunction<Pol, AS>::operator-=(const typename Pol::CoeffType& rhs)
	{
		mIsSimplified = false;
		mNominator -= rhs * mDenominator;
		if(AS) eliminateCommonFactor();
		return *this;
	}
	
	
	template<typename Pol, bool AS>
	RationalFunction<Pol, AS>& RationalFunction<Pol, AS>::operator*=(const RationalFunction<Pol, AS>& rhs)
	{
		mIsSimplified = false;
		mNominator *= rhs.mNominator;
		mDenominator *= rhs.mDenominator;
		if(AS) eliminateCommonFactor();
		return *this;
	}
	
	template<typename Pol, bool AS>
	RationalFunction<Pol, AS>& RationalFunction<Pol, AS>::operator*=(const Pol& rhs)
	{
		mIsSimplified = false;
		mNominator *= rhs;
		if(AS) eliminateCommonFactor();
		return *this;
	}	
	
	template<typename Pol, bool AS>
    template<typename P, DisableIf<needs_cache<P>>>
	RationalFunction<Pol, AS>& RationalFunction<Pol, AS>::operator*=(Variable::Arg rhs)
	{
		mIsSimplified = false;
		mNominator *= rhs;
		if(AS) eliminateCommonFactor();
		return *this;
	}
	
	template<typename Pol, bool AS>
	RationalFunction<Pol, AS>& RationalFunction<Pol, AS>::operator*=(const typename Pol::CoeffType& rhs)
	{
		mNominator *= rhs;
		if(AS) eliminateCommonFactor();
		return *this;
	}
	
	
	template<typename Pol, bool AS>
	RationalFunction<Pol, AS>& RationalFunction<Pol, AS>::operator/=(const RationalFunction<Pol, AS>& rhs)
	{
		if(mDenominator.isOne()) 
		{
			return *this /= rhs.mNominator;
		}
		mIsSimplified = false;
		mNominator *= rhs.mDenominator;
		mDenominator *= rhs.mNominator;
		if(AS) eliminateCommonFactor();
		return *this;
	}
	
	template<typename Pol, bool AS>
	RationalFunction<Pol, AS>& RationalFunction<Pol, AS>::operator/=(const Pol& rhs)
	{
		if(rhs.isConstant()) 
		{ 
			mNominator /= rhs.constantPart();
		}
		else
		{
			mDenominator *= rhs;
			mIsSimplified = false;
			if(AS) eliminateCommonFactor();
		}
		return *this;
	}
	
	
	template<typename Pol, bool AS>
    template<typename P, DisableIf<needs_cache<P>>>
	RationalFunction<Pol, AS>& RationalFunction<Pol, AS>::operator/=(Variable::Arg rhs)
	{
		mIsSimplified = false;
		mDenominator *= rhs;
		if(AS) eliminateCommonFactor();
		return *this;
	}
	
	template<typename Pol, bool AS>
	RationalFunction<Pol, AS>& RationalFunction<Pol, AS>::operator/=(unsigned long rhs)
	{
		mIsSimplified = false;
		mNominator /= rhs;
		return *this;
	}
	
	template<typename Pol, bool AS>
	RationalFunction<Pol, AS> operator+(const RationalFunction<Pol, AS>& lhs, const RationalFunction<Pol, AS>& rhs)
	{
		RationalFunction<Pol, AS> res(lhs);
		return res += rhs;
	}
	
	
	template<typename Pol, bool AS>
	RationalFunction<Pol, AS> operator+(const RationalFunction<Pol, AS>& lhs, const Pol& rhs)
	{
		RationalFunction<Pol, AS> res(lhs);
		return res += rhs;
	}
	
	
	template<typename Pol, bool AS, DisableIf<needs_cache<Pol>>>
	RationalFunction<Pol, AS> operator+(const RationalFunction<Pol, AS>& lhs, const Term<typename Pol::CoeffType>& rhs)
	{
		RationalFunction<Pol, AS> res(lhs);
		return res += rhs;
	}
	
	template<typename Pol, bool AS, DisableIf<needs_cache<Pol>>>
	RationalFunction<Pol, AS> operator+(const RationalFunction<Pol, AS>& lhs, Variable::Arg rhs)
	{
		RationalFunction<Pol, AS> res(lhs);
		return res += rhs;
	}
	
	template<typename Pol, bool AS>
	RationalFunction<Pol, AS> operator+(const RationalFunction<Pol, AS>& lhs, const typename Pol::CoeffType& rhs)
	{
		RationalFunction<Pol, AS> res(lhs);
		return res += rhs;
	}
	
	template<typename Pol, bool AS>
	RationalFunction<Pol, AS> operator-(const RationalFunction<Pol, AS>& lhs, const RationalFunction<Pol, AS>& rhs)
	{
		RationalFunction<Pol, AS> res(lhs);
		return res -= rhs;
	}
	
	
	template<typename Pol, bool AS>
	RationalFunction<Pol, AS> operator-(const RationalFunction<Pol, AS>& lhs, const Pol& rhs)
	{
		RationalFunction<Pol, AS> res(lhs);
		return res -= rhs;
	}
	
	
	template<typename Pol, bool AS, DisableIf<needs_cache<Pol>>>
	RationalFunction<Pol, AS> operator-(const RationalFunction<Pol, AS>& lhs, const Term<typename Pol::CoeffType>& rhs)
	{
		RationalFunction<Pol, AS> res(lhs);
		return res -= rhs;
	}
	
	template<typename Pol, bool AS, DisableIf<needs_cache<Pol>>>
	RationalFunction<Pol, AS> operator-(const RationalFunction<Pol, AS>& lhs, Variable::Arg rhs)
	{
		RationalFunction<Pol, AS> res(lhs);
		return res -= rhs;
	}
	
	template<typename Pol, bool AS>
	RationalFunction<Pol, AS> operator-(const RationalFunction<Pol, AS>& lhs, const typename Pol::CoeffType& rhs)
	{
		RationalFunction<Pol, AS> res(lhs);
		return res -= rhs;
	}
	
	template<typename Pol, bool AS>
	RationalFunction<Pol, AS> operator*(const RationalFunction<Pol, AS>& lhs, const RationalFunction<Pol, AS>& rhs)
	{
		RationalFunction<Pol, AS> res(lhs);
		return res *= rhs;
	}
	
	
	template<typename Pol, bool AS>
	RationalFunction<Pol, AS> operator*(const RationalFunction<Pol, AS>& lhs, const Pol& rhs)
	{
		RationalFunction<Pol, AS> res(lhs);
		return res *= rhs;
	}
	
	
	template<typename Pol, bool AS, DisableIf<needs_cache<Pol>>>
	RationalFunction<Pol, AS> operator*(const RationalFunction<Pol, AS>& lhs, const Term<typename Pol::CoeffType>& rhs)
	{
		RationalFunction<Pol, AS> res(lhs);
		return res *= rhs;
	}
	
	template<typename Pol, bool AS, DisableIf<needs_cache<Pol>>>
	RationalFunction<Pol, AS> operator*(const RationalFunction<Pol, AS>& lhs, Variable::Arg rhs)
	{
		RationalFunction<Pol, AS> res(lhs);
		return res *= rhs;
	}
	
	template<typename Pol, bool AS>
	RationalFunction<Pol, AS> operator*(const RationalFunction<Pol, AS>& lhs, const typename Pol::CoeffType& rhs)
	{
		RationalFunction<Pol, AS> res(lhs);
		return res *= rhs;
	}
	
	template<typename Pol, bool AS>
	RationalFunction<Pol, AS> operator/(const RationalFunction<Pol, AS>& lhs, const RationalFunction<Pol, AS>& rhs)
	{
		RationalFunction<Pol, AS> res(lhs);
		return res /= rhs;
	}
	
	
	template<typename Pol, bool AS>
	RationalFunction<Pol, AS> operator/(const RationalFunction<Pol, AS>& lhs, const Pol& rhs)
	{
		RationalFunction<Pol, AS> res(lhs);
		return res /= rhs;
	}
	
	
	template<typename Pol, bool AS, DisableIf<needs_cache<Pol>>>
	RationalFunction<Pol, AS> operator/(const RationalFunction<Pol, AS>& lhs, const Term<typename Pol::CoeffType>& rhs)
	{
		RationalFunction<Pol, AS> res(lhs);
		return res /= rhs;
	}
	
	template<typename Pol, bool AS, DisableIf<needs_cache<Pol>>>
	RationalFunction<Pol, AS> operator/(const RationalFunction<Pol, AS>& lhs, Variable::Arg rhs)
	{
		RationalFunction<Pol, AS> res(lhs);
		return res /= rhs;
	}
	
	template<typename Pol, bool AS>
	RationalFunction<Pol, AS> operator/(const RationalFunction<Pol, AS>& lhs, const typename Pol::CoeffType& rhs)
	{
		RationalFunction<Pol, AS> res(lhs);
		return res /= rhs;
	}
	
	template<typename Pol, bool AS>
	RationalFunction<Pol, AS> operator/(const RationalFunction<Pol, AS>& lhs, unsigned long rhs)
	{
		RationalFunction<Pol, AS> res(lhs);
		return res /= rhs;
	}
	
	template<typename Pol, bool AS>
	RationalFunction<Pol, AS> operator-(const RationalFunction<Pol, AS>& rhs)
	{
		return RationalFunction<Pol, AS>(-rhs.nominator(), rhs.denominator());
	}
	
	template<typename Pol, bool AS>
	std::ostream& operator<<(std::ostream& os, const RationalFunction<Pol, AS>& rhs)
	{
		return os << "(" << rhs.nominator() << ")/(" << rhs.denominator() << ")";
	}
}
