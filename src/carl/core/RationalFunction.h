/** 
 * @file:   RationalFunction.h
 * @author: Sebastian Junges
 */

#pragma once
#include "MultivariateGCD.h"

namespace carl
{
	
template<typename Pol>
class RationalFunction
{
	Pol mNominator;
	Pol mDenominator;
	
	
public:
	typedef typename Pol::CoeffType CoeffType;
	RationalFunction()
	: mNominator(0), mDenominator(1)
	{
		
	}
	
	explicit RationalFunction(int v)
	: mNominator(v), mDenominator(1)
	{
		
	}
	
	explicit RationalFunction(const CoeffType& c)
	: mNominator(c), mDenominator(1)
	{
		
	}
	
	explicit RationalFunction(Variable::Arg v)
	: mNominator(v), mDenominator(1)
	{
		
	}
	
	explicit RationalFunction(const Pol& p)
	: mNominator(p), mDenominator(1)
	{
		
	}
	
	RationalFunction(const Pol& nom, const Pol& denom)
	: mNominator(nom), mDenominator(denom)
	{
		assert(!denom.isZero());
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
		if(mDenominator.isConstant())
		{
			mNominator /= mDenominator.constantPart();
			mDenominator = Pol(1);
		}
		else
		{
			Pol gcd = carl::gcd(mNominator, mDenominator);
			mNominator = mNominator.quotient(gcd);
			mDenominator = mDenominator.quotient(gcd);
		}
	}
	
	bool isZero() const
	{
		assert(!mDenominator.isZero());
		return mNominator.isZero();
	}
	
	void gatherVariables(std::set<Variable>& vars) const
	{
		mNominator.gatherVariables(vars);
		mDenominator.gatherVariables(vars);
	}
	
	
	RationalFunction& operator+=(const RationalFunction& rhs);
	RationalFunction& operator+=(const Pol& rhs);
	RationalFunction& operator+=(const Term<CoeffType>& rhs);
	
	RationalFunction& operator-=(const RationalFunction& rhs);
	RationalFunction& operator-=(Variable::Arg rhs);
	
	RationalFunction& operator*=(const RationalFunction& rhs);
	RationalFunction& operator*=(Variable::Arg rhs);
	
	RationalFunction& operator/=(const RationalFunction& rhs);
	RationalFunction& operator/=(const Pol& rhs);
	RationalFunction& operator/=(unsigned long);
	
};

	template<typename Pol>
	bool operator==(const RationalFunction<Pol>& lhs, const RationalFunction<Pol>& rhs);
	template<typename Pol>
	bool operator!=(const RationalFunction<Pol>& lhs, const RationalFunction<Pol>& rhs);

	template<typename Pol>
	RationalFunction<Pol> operator+(const RationalFunction<Pol>& lhs, const RationalFunction<Pol>& rhs);
	template<typename Pol>
	RationalFunction<Pol> operator-(const RationalFunction<Pol>& lhs, const RationalFunction<Pol>& rhs);
	template<typename Pol>
	RationalFunction<Pol> operator-(const RationalFunction<Pol>& lhs, Variable::Arg rhs);
	template<typename Pol>
	RationalFunction<Pol> operator*(const RationalFunction<Pol>& lhs, const RationalFunction<Pol>& rhs);
	template<typename Pol>
	RationalFunction<Pol> operator*(const RationalFunction<Pol>& lhs, Variable::Arg rhs);
	template<typename Pol>
	RationalFunction<Pol> operator/(const RationalFunction<Pol>& lhs, const RationalFunction<Pol>& rhs);
	template<typename Pol>
	RationalFunction<Pol> operator/(const RationalFunction<Pol>& lhs, unsigned long);
	template<typename Pol>
	RationalFunction<Pol> operator-(const RationalFunction<Pol>& lhs);
	
	template<typename Pol>
	std::ostream& operator<<(std::ostream& os, const RationalFunction<Pol>& rhs);
}

#include "RationalFunction.tpp"
