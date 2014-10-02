/** 
 * @file:   RationalFunction.h
 * @author: Sebastian Junges
 */

#pragma once
#include "MultivariateGCD.h"

namespace carl
{
	
template<typename Pol, bool AutoSimplify=false>
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
	
	/**
	 * 
     * @return The nominator
     */
	const Pol& nominator() const
	{
		return mNominator;
	}
	
	/**
	 * 
     * @return  The denominator
     */
	const Pol& denominator() const
	{
		return mDenominator;
	}
	
	void simplify() 
	{
		if(!AutoSimplify)
		{
			eliminateCommonFactor();
		}
		else
		{
			LOGMSG_WARN("carl.core","Calling simplify on rational function with AutoSimplify");
		}
	}
	
	/**
	 * Check whether the rational function is zero
     * @return true if it is
     */
	bool isZero() const
	{
		assert(!mDenominator.isZero());
		return mNominator.isZero();
	}
	
	bool isOne() const 
	{
		assert(!mDenominator.isZero());
		return mNominator == mDenominator;
	}
	/**
	 * Collect all occuring variables
     * @param vars
     */
	void gatherVariables(std::set<Variable>& vars) const
	{
		mNominator.gatherVariables(vars);
		mDenominator.gatherVariables(vars);
	}
	
	/// @name In-place addition operators
	/// @{
	/**
	 * Add something to this rational function and return the changed rational function.
	 * @param rhs Right hand side.
	 * @return Changed rational function.
	 */
	RationalFunction& operator+=(const RationalFunction& rhs);
	RationalFunction& operator+=(const Pol& rhs);
	RationalFunction& operator+=(const Term<CoeffType>& rhs);
	RationalFunction& operator+=(Variable::Arg rhs);
	RationalFunction& operator+=(const CoeffType& rhs);
	/// @}
	
	
	/// @name In-place subtraction operators
	/// @{
	/**
	 * Subtract something from this rational function and return the changed rational function.
	 * @param rhs Right hand side.
	 * @return Changed rational function.
	 */
	RationalFunction& operator-=(const RationalFunction& rhs);
	RationalFunction& operator-=(const Pol& rhs);
	RationalFunction& operator-=(const Term<CoeffType>& rhs);
	RationalFunction& operator-=(Variable::Arg rhs);
	RationalFunction& operator-=(const CoeffType& rhs);
	/// @}
	
	/// @name In-place multiplication operators
	/// @{
	/**
	 * Multiply something with this rational function and return the changed rational function.
	 * @param rhs Right hand side.
	 * @return Changed rational function.
	 */
	RationalFunction& operator*=(const RationalFunction& rhs);
	RationalFunction& operator*=(const Pol& rhs);
	RationalFunction& operator*=(const Term<CoeffType>& rhs);
	RationalFunction& operator*=(Variable::Arg rhs);
	RationalFunction& operator*=(const CoeffType& rhs);
	/// @}
	
	/**
	 * Divide this rational function by something and return the changed rational function.
	 * @param rhs Right hand side.
	 * @return Changed rational function.
	 */
	RationalFunction& operator/=(const RationalFunction& rhs);
	RationalFunction& operator/=(const Pol& rhs);
	RationalFunction& operator/=(const Term<CoeffType>& rhs);
	RationalFunction& operator/=(Variable::Arg rhs);
	RationalFunction& operator/=(const CoeffType& rhs);
	RationalFunction& operator/=(unsigned long rhs);
	/// @}
	
	
protected:
	/**
	 * Helper function for simplify which eliminates the common factor.
	 */
	void eliminateCommonFactor()
	{
		if(mNominator.isZero())
		{
			mDenominator = Pol(1);
			return;
		}
		
		if(mDenominator.isOne())
		{
			return;
		}
		
		if(mNominator == mDenominator)
		{
			mNominator = Pol(1);
			mDenominator = Pol(1);
			return;
		}
		
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
	
};

	template<typename Pol, bool AS>
	bool operator==(const RationalFunction<Pol, AS>& lhs, const RationalFunction<Pol, AS>& rhs);
	template<typename Pol, bool AS>
	bool operator!=(const RationalFunction<Pol, AS>& lhs, const RationalFunction<Pol, AS>& rhs);

	template<typename Pol, bool AS>
	RationalFunction<Pol, AS> operator+(const RationalFunction<Pol, AS>& lhs, const RationalFunction<Pol, AS>& rhs);
	template<typename Pol, bool AS>
	RationalFunction<Pol, AS> operator+(const RationalFunction<Pol, AS>& lhs, const Pol& rhs);
	template<typename Pol, bool AS>
	RationalFunction<Pol, AS> operator+(const RationalFunction<Pol, AS>& lhs, const Term<typename Pol::CoeffType>& rhs);
	template<typename Pol, bool AS>
	RationalFunction<Pol, AS> operator+(const RationalFunction<Pol, AS>& lhs, Variable::Arg rhs);
	template<typename Pol, bool AS>
	RationalFunction<Pol, AS> operator+(const RationalFunction<Pol, AS>& lhs, const typename Pol::CoeffType& rhs);
	
	template<typename Pol, bool AS>
	RationalFunction<Pol, AS> operator-(const RationalFunction<Pol, AS>& lhs, const RationalFunction<Pol, AS>& rhs);
	template<typename Pol, bool AS>
	RationalFunction<Pol, AS> operator-(const RationalFunction<Pol, AS>& lhs, const Pol& rhs);
	template<typename Pol, bool AS>
	RationalFunction<Pol, AS> operator-(const RationalFunction<Pol, AS>& lhs, const Term<typename Pol::CoeffType>& rhs);
	template<typename Pol, bool AS>
	RationalFunction<Pol, AS> operator-(const RationalFunction<Pol, AS>& lhs, Variable::Arg rhs);
	template<typename Pol, bool AS>
	RationalFunction<Pol, AS> operator-(const RationalFunction<Pol, AS>& lhs, const typename Pol::CoeffType& rhs);
	
	template<typename Pol, bool AS>
	RationalFunction<Pol, AS> operator*(const RationalFunction<Pol, AS>& lhs, const RationalFunction<Pol, AS>& rhs);
	template<typename Pol, bool AS>
	RationalFunction<Pol, AS> operator*(const RationalFunction<Pol, AS>& lhs, const Pol& rhs);
	template<typename Pol, bool AS>
	RationalFunction<Pol, AS> operator*(const RationalFunction<Pol, AS>& lhs, const Term<typename Pol::CoeffType>& rhs);
	template<typename Pol, bool AS>
	RationalFunction<Pol, AS> operator*(const RationalFunction<Pol, AS>& lhs, Variable::Arg rhs);
	template<typename Pol, bool AS>
	RationalFunction<Pol, AS> operator*(const RationalFunction<Pol, AS>& lhs, const typename Pol::CoeffType& rhs);
	
	template<typename Pol, bool AS>
	RationalFunction<Pol, AS> operator/(const RationalFunction<Pol, AS>& lhs, const RationalFunction<Pol, AS>& rhs);
	template<typename Pol, bool AS>
	RationalFunction<Pol, AS> operator/(const RationalFunction<Pol, AS>& lhs, const Pol& rhs);
	template<typename Pol, bool AS>
	RationalFunction<Pol, AS> operator/(const RationalFunction<Pol, AS>& lhs, const Term<typename Pol::CoeffType>& rhs);
	template<typename Pol, bool AS>
	RationalFunction<Pol, AS> operator/(const RationalFunction<Pol, AS>& lhs, Variable::Arg rhs);
	template<typename Pol, bool AS>
	RationalFunction<Pol, AS> operator/(const RationalFunction<Pol, AS>& lhs, const typename Pol::CoeffType& rhs);
	
	template<typename Pol, bool AS>
	RationalFunction<Pol, AS> operator/(const RationalFunction<Pol, AS>& lhs, unsigned long);
	template<typename Pol, bool AS>
	RationalFunction<Pol, AS> operator-(const RationalFunction<Pol, AS>& lhs);
	
	template<typename Pol, bool AS>
	std::ostream& operator<<(std::ostream& os, const RationalFunction<Pol, AS>& rhs);
	
}

#include "RationalFunction.tpp"
