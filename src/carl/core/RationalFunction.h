/** 
 * @file:   RationalFunction.h
 * @author: Sebastian Junges
 */

#pragma once
#include "../numbers/typetraits.h"
#include "MultivariateGCD.h"
#include "FactorizedPolynomial.h"

namespace carl
{
	
template<typename Pol, bool AutoSimplify=false>
class RationalFunction
{
	Pol mNominator;
	Pol mDenominator;
	bool mIsSimplified;
	
	
public:
    typedef Pol PolyType;
    
	typedef typename Pol::CoeffType CoeffType;
	RationalFunction()
	: mNominator(0), mDenominator(1), mIsSimplified(true)
	{
		
	}
	
	explicit RationalFunction(int v)
	: mNominator(v), mDenominator(1), mIsSimplified(true)
	{
		
	}
	
	explicit RationalFunction(const CoeffType& c)
	: mNominator(c), mDenominator(1), mIsSimplified(true)
	{
		
	}
	
    template<typename P = Pol, DisableIf<needs_cache<P>> = dummy>
	explicit RationalFunction(Variable::Arg v)
	: mNominator(v), mDenominator(1), mIsSimplified(true)
	{
	}
	
	explicit RationalFunction(const Pol& p)
	: mNominator(p), mDenominator(1), mIsSimplified(true)
	{
		
	}
	
	RationalFunction(const Pol& nom, const Pol& denom)
	: mNominator(nom), mDenominator(denom), mIsSimplified(false)
	{
        if(AutoSimplify) eliminateCommonFactor();
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
	
	/**
	 * Checks if this rational function has been simplified since it's last modification.
	 * Note that if AutoSimplify is true, this should always return true.
     * @return If this is simplified.
     */
	bool isSimplified() const {
		return mIsSimplified;
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
	
	bool isConstant() const 
	{
		return mNominator.isConstant() && mDenominator.isConstant();
	}
	
	CoeffType constantPart() const 
	{
		return mNominator.constantPart() / mDenominator.constantPart();
	}
	
	/**
	 * Collect all occurring variables
     * @return All occcurring variables
     */
	std::set<Variable> gatherVariables() const {
		std::set<Variable> vars;
		gatherVariables(vars);
		return vars;
	}
	
	/**
	 * Add all occurring variables to the set vars
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
    template<typename P = Pol, DisableIf<needs_cache<P>> = dummy>
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
    template<typename P = Pol, DisableIf<needs_cache<P>> = dummy>
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
    template<typename P = Pol, DisableIf<needs_cache<P>> = dummy>
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
    template<typename P = Pol, DisableIf<needs_cache<P>> = dummy>
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
		if (mIsSimplified) return;
        mIsSimplified = true;
		if(mNominator.isZero())
		{
			mDenominator = Pol(CoeffType(1));
			return;
		}
		
		if(mNominator == mDenominator)
		{
			mNominator = Pol(CoeffType(1));
			mDenominator = Pol(CoeffType(1));
			return;
		}
		
        CoeffType cpFactorNom = mNominator.coprimeFactor();
        CoeffType cpFactorDen = mDenominator.coprimeFactor();
        mNominator *= cpFactorNom;
        mDenominator *= cpFactorDen;
        CoeffType cpFactor = cpFactorDen/cpFactorNom;
		if(!mDenominator.isConstant())
		{
			Pol gcd = carl::gcd(mNominator, mDenominator);
			assert(mNominator.quotient(gcd) * gcd == mNominator);
			mNominator = mNominator.quotient(gcd);
			assert(mDenominator.quotient(gcd) * gcd == mDenominator);
			mDenominator = mDenominator.quotient(gcd);
            CoeffType cpFactorNom = mNominator.coprimeFactor();
            CoeffType cpFactorDen = mDenominator.coprimeFactor();
            mNominator *= cpFactorNom;
            mDenominator *= cpFactorDen;
            cpFactor *= cpFactorDen/cpFactorNom;
		}
        mNominator *= carl::getNum( cpFactor );
        mDenominator *= carl::getDenom( cpFactor );
	}
	
};

	template<typename Pol, bool AS>
	bool operator==(const RationalFunction<Pol, AS>& lhs, const RationalFunction<Pol, AS>& rhs);
	template<typename Pol, bool AS>
	bool operator<(const RationalFunction<Pol, AS>& lhs, const RationalFunction<Pol, AS>& rhs);
	template<typename Pol, bool AS>
	bool operator!=(const RationalFunction<Pol, AS>& lhs, const RationalFunction<Pol, AS>& rhs);

	template<typename Pol, bool AS>
	RationalFunction<Pol, AS> operator+(const RationalFunction<Pol, AS>& lhs, const RationalFunction<Pol, AS>& rhs);
	template<typename Pol, bool AS>
	RationalFunction<Pol, AS> operator+(const RationalFunction<Pol, AS>& lhs, const Pol& rhs);
	template<typename Pol, bool AS, DisableIf<needs_cache<Pol>> = dummy>
	RationalFunction<Pol, AS> operator+(const RationalFunction<Pol, AS>& lhs, const Term<typename Pol::CoeffType>& rhs);
	template<typename Pol, bool AS, DisableIf<needs_cache<Pol>> = dummy>
	RationalFunction<Pol, AS> operator+(const RationalFunction<Pol, AS>& lhs, Variable::Arg rhs);
	template<typename Pol, bool AS>
	RationalFunction<Pol, AS> operator+(const RationalFunction<Pol, AS>& lhs, const typename Pol::CoeffType& rhs);
	
	template<typename Pol, bool AS>
	RationalFunction<Pol, AS> operator-(const RationalFunction<Pol, AS>& lhs, const RationalFunction<Pol, AS>& rhs);
	template<typename Pol, bool AS>
	RationalFunction<Pol, AS> operator-(const RationalFunction<Pol, AS>& lhs, const Pol& rhs);
	template<typename Pol, bool AS, DisableIf<needs_cache<Pol>> = dummy>
	RationalFunction<Pol, AS> operator-(const RationalFunction<Pol, AS>& lhs, const Term<typename Pol::CoeffType>& rhs);
	template<typename Pol, bool AS, DisableIf<needs_cache<Pol>> = dummy>
	RationalFunction<Pol, AS> operator-(const RationalFunction<Pol, AS>& lhs, Variable::Arg rhs);
	template<typename Pol, bool AS>
	RationalFunction<Pol, AS> operator-(const RationalFunction<Pol, AS>& lhs, const typename Pol::CoeffType& rhs);
	
	template<typename Pol, bool AS>
	RationalFunction<Pol, AS> operator*(const RationalFunction<Pol, AS>& lhs, const RationalFunction<Pol, AS>& rhs);
	template<typename Pol, bool AS>
	RationalFunction<Pol, AS> operator*(const RationalFunction<Pol, AS>& lhs, const Pol& rhs);
	template<typename Pol, bool AS, DisableIf<needs_cache<Pol>> = dummy>
	RationalFunction<Pol, AS> operator*(const RationalFunction<Pol, AS>& lhs, const Term<typename Pol::CoeffType>& rhs);
	template<typename Pol, bool AS, DisableIf<needs_cache<Pol>> = dummy>
	RationalFunction<Pol, AS> operator*(const RationalFunction<Pol, AS>& lhs, Variable::Arg rhs);
	template<typename Pol, bool AS>
	RationalFunction<Pol, AS> operator*(const RationalFunction<Pol, AS>& lhs, const typename Pol::CoeffType& rhs);
	
	template<typename Pol, bool AS>
	RationalFunction<Pol, AS> operator/(const RationalFunction<Pol, AS>& lhs, const RationalFunction<Pol, AS>& rhs);
	template<typename Pol, bool AS>
	RationalFunction<Pol, AS> operator/(const RationalFunction<Pol, AS>& lhs, const Pol& rhs);
	template<typename Pol, bool AS, DisableIf<needs_cache<Pol>> = dummy>
	RationalFunction<Pol, AS> operator/(const RationalFunction<Pol, AS>& lhs, const Term<typename Pol::CoeffType>& rhs);
	template<typename Pol, bool AS, DisableIf<needs_cache<Pol>> = dummy>
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

namespace std
{
	template<typename Pol, bool AS>
	struct hash<carl::RationalFunction<Pol, AS>> 
	{
		std::size_t operator()(const carl::RationalFunction<Pol, AS>& r) const {
			std::hash<Pol> h;
			
			return (h(r.nominator()) << 8) ^ (h(r.denominator()) >> 8);
		}
	};
}

#include "RationalFunction.tpp"
