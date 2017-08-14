/** 
 * @file:   RationalFunction.tpp
 * @author: Sebastian Junges
 * @author: Florian Corzilius
 *
 * @since March 16, 2014
 */

#include "RationalFunction.h"


#pragma once

namespace carl
{   
	
	template<typename Pol, bool AS>
	RationalFunction<Pol, AS> RationalFunction<Pol, AS>::derivative( const Variable& x, unsigned nth) const
	{
		assert(nth == 1);
		if( isConstant() ) {
			return RationalFunction<Pol, AS>(0);
		}
		
		// TODO use factorization whenever possible.
		// TODO specialize if it is just a polynomial.
		CARL_LOG_INEFFICIENT();
		// (u/v)' = (u'v - uv')/(v^2)
		const auto& u = nominatorAsPolynomial();
		const auto& v = denominatorAsPolynomial();
		return RationalFunction<Pol, AS>(u.derivative(x) * v - u * v.derivative(x), v.pow(2));
	}
	
	
    template<typename Pol, bool AS>
	void RationalFunction<Pol, AS>::eliminateCommonFactor( bool _justNormalize )
    {
        if(mIsSimplified) return;
        if(nominatorAsPolynomial().isZero())
        {
            delete mPolynomialQuotient;
            mPolynomialQuotient = nullptr;
            mNumberQuotient = std::move(CoeffType(0));
            mIsSimplified = true;
            return;
        }
        if(nominatorAsPolynomial() == denominatorAsPolynomial())
        {
            delete mPolynomialQuotient;
            mPolynomialQuotient = nullptr;
            mNumberQuotient = std::move(CoeffType(1));
            mIsSimplified = true;
            return;
        }
        CoeffType cpFactorNom( std::move( nominatorAsPolynomial().coprimeFactor() ) );
        CoeffType cpFactorDen( std::move( denominatorAsPolynomial().coprimeFactor() ) );
        mPolynomialQuotient->first *= cpFactorNom;
        mPolynomialQuotient->second *= cpFactorDen;
        CoeffType cpFactor( std::move( cpFactorDen/cpFactorNom ) );
        if(!_justNormalize && !denominatorAsPolynomial().isConstant())
        {
            carl::gcd(nominatorAsPolynomial(), denominatorAsPolynomial());
            auto ret = carl::lazyDiv( nominatorAsPolynomial(), denominatorAsPolynomial() );
            mPolynomialQuotient->first = std::move( ret.first );
            mPolynomialQuotient->second = std::move( ret.second );
            CoeffType cpFactorNom( nominatorAsPolynomial().coprimeFactor() );
            CoeffType cpFactorDen( denominatorAsPolynomial().coprimeFactor() );
            mPolynomialQuotient->first *= cpFactorNom;
            mPolynomialQuotient->second *= cpFactorDen;
            cpFactor *= cpFactorDen/cpFactorNom;
            mIsSimplified = true;
        }
        mPolynomialQuotient->first *= carl::getNum( cpFactor );
        mPolynomialQuotient->second *= carl::getDenom( cpFactor );
        if( nominatorAsPolynomial().isConstant() && denominatorAsPolynomial().isConstant() )
        {
            mNumberQuotient = std::move( constantPart() );
            delete mPolynomialQuotient;
            mPolynomialQuotient = nullptr;
            mIsSimplified = true;
        }
    }
    
    template<typename Pol, bool AS>
    template<bool byInverse>
    RationalFunction<Pol, AS>& RationalFunction<Pol, AS>::add(const RationalFunction<Pol, AS>& rhs)
    {
        if( this->isConstant() && rhs.isConstant() )
        {
            if(byInverse)
                this->mNumberQuotient -= rhs.mNumberQuotient;
            else
                this->mNumberQuotient += rhs.mNumberQuotient;
            return *this;
        }
        else if( this->isConstant() )
        {
            CoeffType c = this->mNumberQuotient;
            if(byInverse)
                *this = -rhs;
            else
                *this = rhs;
            return *this += c;
        }
        else if( rhs.isConstant() )
        {
            if(byInverse)
                return *this -= rhs.mNumberQuotient;
            else
                return *this += rhs.mNumberQuotient;
        }
        mIsSimplified = false;
		if(denominatorAsPolynomial().isConstant() && rhs.denominatorAsPolynomial().isConstant())
		{
			mPolynomialQuotient->first *= rhs.denominatorAsPolynomial().constantPart();
            if(byInverse)
                mPolynomialQuotient->first -= rhs.nominatorAsPolynomial() * denominatorAsPolynomial().constantPart();
            else
                mPolynomialQuotient->first += rhs.nominatorAsPolynomial() * denominatorAsPolynomial().constantPart();
            mPolynomialQuotient->second *= rhs.denominatorAsPolynomial().constantPart();
		}
		else
		{
			if(denominatorAsPolynomial().isConstant())
			{
				// TODO use more efficient elimination
				mPolynomialQuotient->first *= rhs.denominatorAsPolynomial();
                if(byInverse)
                    mPolynomialQuotient->first -= rhs.nominatorAsPolynomial() * denominatorAsPolynomial().constantPart();
                else
                    mPolynomialQuotient->first += rhs.nominatorAsPolynomial() * denominatorAsPolynomial().constantPart();
				// TODO use info that it is faster
				mPolynomialQuotient->second *= rhs.denominatorAsPolynomial();
			}
			else if(rhs.denominatorAsPolynomial().isConstant())
			{
				mPolynomialQuotient->first *= rhs.denominatorAsPolynomial().constantPart();
                if(byInverse)
                    mPolynomialQuotient->first -= rhs.nominatorAsPolynomial() * denominatorAsPolynomial();
                else
                    mPolynomialQuotient->first += rhs.nominatorAsPolynomial() * denominatorAsPolynomial();
				mPolynomialQuotient->second *= rhs.denominatorAsPolynomial().constantPart();
			}
			else
			{
				Pol leastCommonMultiple( std::move( carl::lcm(this->denominatorAsPolynomial(), rhs.denominatorAsPolynomial() ) ) );
                if(byInverse)
                {
                    mPolynomialQuotient->first = std::move( this->nominatorAsPolynomial() * quotient(leastCommonMultiple,this->denominatorAsPolynomial()) 
                                                - rhs.nominatorAsPolynomial() * quotient(leastCommonMultiple,rhs.denominatorAsPolynomial()) );
                }
                else
                {
                    mPolynomialQuotient->first = std::move( this->nominatorAsPolynomial() * quotient(leastCommonMultiple,this->denominatorAsPolynomial()) 
                                                + rhs.nominatorAsPolynomial() * quotient(leastCommonMultiple,rhs.denominatorAsPolynomial()) );
                }
				mPolynomialQuotient->second = std::move( leastCommonMultiple );
			}
		}
        eliminateCommonFactor( !AS );
		return *this;
    }
    
    template<typename Pol, bool AS>
    template<bool byInverse>
    RationalFunction<Pol, AS>& RationalFunction<Pol, AS>::add(const Pol& rhs)
    {
        if( this->isConstant() )
        {
            CoeffType c = this->mNumberQuotient;
            Pol resultNum( std::move( byInverse ? (rhs * CoeffType(getDenom(c)) - CoeffType(getNum(c))) : (rhs * CoeffType(getDenom(c)) + CoeffType(getNum(c))) ) );
            *this = std::move( RationalFunction<Pol, AS>( std::move(resultNum), std::move(Pol(CoeffType(getDenom(c)))) ) );
            return *this;
        }
		mIsSimplified = false;
        if(byInverse)
            mPolynomialQuotient->first -= std::move(rhs * denominatorAsPolynomial());
        else
            mPolynomialQuotient->first += std::move(rhs * denominatorAsPolynomial());
		eliminateCommonFactor( !AS );
		return *this;
    }
    
    template<typename Pol, bool AS>
    template<bool byInverse, typename P, DisableIf<needs_cache<P>>>
    RationalFunction<Pol, AS>& RationalFunction<Pol, AS>::add(Variable::Arg rhs)
    {
        if( this->isConstant() )
        {
            CoeffType c(this->mNumberQuotient);
            Pol resultNum( rhs );
            resultNum *= CoeffType(getDenom(c));
            if(byInverse)
                resultNum -= CoeffType(getNum(c));
            else
                resultNum += CoeffType(getNum(c));
            *this = std::move( RationalFunction<Pol, AS>( std::move(resultNum), std::move(Pol(CoeffType(getDenom(c)))) ) );
            return *this;
        }
		mIsSimplified = false;
        if(byInverse)
            mPolynomialQuotient->first -= std::move(rhs * denominatorAsPolynomial());
        else
            mPolynomialQuotient->first += std::move(rhs * denominatorAsPolynomial());
		eliminateCommonFactor( !AS );
		return *this;
    }
    
    template<typename Pol, bool AS>
    template<bool byInverse>
    RationalFunction<Pol, AS>& RationalFunction<Pol, AS>::add(const typename Pol::CoeffType& rhs)
    {
        if( this->isConstant() )
        {
            if(byInverse)
                this->mNumberQuotient -= rhs;
            else
                this->mNumberQuotient += rhs;
            return *this;
        }
		mIsSimplified = false;
        if(byInverse)
            mPolynomialQuotient->first -= std::move(rhs * denominatorAsPolynomial());
        else
            mPolynomialQuotient->first += std::move(rhs * denominatorAsPolynomial());
		eliminateCommonFactor( !AS );
		return *this;
    }
	
	template<typename Pol, bool AS>
	RationalFunction<Pol, AS>& RationalFunction<Pol, AS>::operator*=(const RationalFunction<Pol, AS>& rhs)
	{
        if( this->isConstant() && rhs.isConstant() )
        {
            this->mNumberQuotient *= rhs.mNumberQuotient;
            return *this;
        }
        else if( this->isConstant() )
        {
            CoeffType c(this->mNumberQuotient);
            *this = rhs;
            return *this *= c;
        }
        else if( rhs.isConstant() )
        {
            return *this *= rhs.mNumberQuotient;
        }
		mIsSimplified = false;
		mPolynomialQuotient->first *= rhs.nominatorAsPolynomial();
		mPolynomialQuotient->second *= rhs.denominatorAsPolynomial();
		eliminateCommonFactor( !AS );
		return *this;
	}
	
	template<typename Pol, bool AS>
	RationalFunction<Pol, AS>& RationalFunction<Pol, AS>::operator*=(const Pol& rhs)
	{
        if( this->isConstant() )
        {
            CoeffType c = this->mNumberQuotient;
            Pol resultNum( rhs );
            resultNum *= CoeffType(getNum(c));
            *this = std::move( RationalFunction<Pol, AS>( std::move(resultNum), std::move(Pol(CoeffType(getDenom(c)))) ) );
            return *this;
        }
		mIsSimplified = false;
		mPolynomialQuotient->first *= rhs;
		eliminateCommonFactor( !AS );
		return *this;
	}	
	
	template<typename Pol, bool AS>
    template<typename P, DisableIf<needs_cache<P>>>
	RationalFunction<Pol, AS>& RationalFunction<Pol, AS>::operator*=(Variable::Arg rhs)
	{
        if( this->isConstant() )
        {
            CoeffType c(this->mNumberQuotient);
            Pol resultNum( rhs );
            resultNum *= CoeffType(getNum(c));
            *this = std::move( RationalFunction<Pol, AS>( std::move(resultNum), std::move(Pol(CoeffType(getDenom(c)))) ) );
            return *this;
        }
		mIsSimplified = false;
		mPolynomialQuotient->first *= rhs;
		eliminateCommonFactor( !AS );
		return *this;
	}
	
	template<typename Pol, bool AS>
	RationalFunction<Pol, AS>& RationalFunction<Pol, AS>::operator*=(const typename Pol::CoeffType& rhs)
	{
	    // TODO handle rhs == 0
        if( this->isConstant() )
        {
            this->mNumberQuotient *= rhs;
            return *this;
        }
        mIsSimplified = false;
		mPolynomialQuotient->first *= rhs;
		eliminateCommonFactor( !AS );
		return *this;
	}

	template<typename Pol, bool AS>
	RationalFunction<Pol, AS>& RationalFunction<Pol,AS>::operator*=(long rhs)
	{
	    // TODO handle rhs == 0
	    if( this->isConstant() )
	    {
	        this->mNumberQuotient *= rhs;
	        return *this;
	    }
	    mIsSimplified = false;
	    mPolynomialQuotient->first *= rhs;
	    eliminateCommonFactor( !AS );
	    return *this;
	}
	
	template<typename Pol, bool AS>
	RationalFunction<Pol, AS>& RationalFunction<Pol, AS>::operator/=(const RationalFunction<Pol, AS>& rhs)
	{
        if( this->isConstant() && rhs.isConstant() )
        {
            this->mNumberQuotient /= rhs.mNumberQuotient;
            return *this;
        }
        else if( this->isConstant() )
        {
            CoeffType c(this->mNumberQuotient);
            *this = rhs.inverse();
            return *this *= c;
        }
        else if( rhs.isConstant() )
        {
            return *this /= rhs.mNumberQuotient;
        }
		mIsSimplified = false;
		if(rhs.denominatorAsPolynomial().isOne())
		{
			return *this /= rhs.nominatorAsPolynomial();
		}
		mPolynomialQuotient->first *= rhs.denominatorAsPolynomial();
		mPolynomialQuotient->second *= rhs.nominatorAsPolynomial();
		eliminateCommonFactor( !AS );
		return *this;
	}
	
	template<typename Pol, bool AS>
	RationalFunction<Pol, AS>& RationalFunction<Pol, AS>::operator/=(const Pol& rhs)
	{
        if( this->isConstant() )
        {
            CoeffType c(this->mNumberQuotient);
            Pol resultNum( rhs );
            resultNum *= CoeffType(getDenom(c));
            *this = std::move( RationalFunction<Pol, AS>( std::move(Pol(CoeffType(getNum(c)))), std::move(resultNum) ) );
            return *this;
        }
        mIsSimplified = false;
		if(rhs.isConstant()) 
		{ 
			mPolynomialQuotient->first /= rhs.constantPart();
		}
		else
		{
			mPolynomialQuotient->second *= rhs;
			eliminateCommonFactor( !AS );
		}
		return *this;
	}
	
	template<typename Pol, bool AS>
    template<typename P, DisableIf<needs_cache<P>>>
	RationalFunction<Pol, AS>& RationalFunction<Pol, AS>::operator/=(Variable::Arg rhs)
	{
        if( this->isConstant() )
        {
            CoeffType c(this->mNumberQuotient);
            Pol resultNum( rhs );
            resultNum *= CoeffType(getDenom(c));
            *this = std::move( RationalFunction<Pol, AS>( std::move(Pol(CoeffType(getNum(c)))), std::move(resultNum) ) );
            return *this;
        }
		mIsSimplified = false;
		mPolynomialQuotient->second *= rhs;
		eliminateCommonFactor( !AS );
		return *this;
	}
	
	template<typename Pol, bool AS>
	RationalFunction<Pol, AS>& RationalFunction<Pol, AS>::operator/=(unsigned long rhs)
	{
        if( this->isConstant() )
        {
            this->mNumberQuotient /= CoeffType(rhs);
            return *this;
        }
		mIsSimplified = false;
		mPolynomialQuotient->first /= rhs;
		return *this;
	}

	template<typename Pol, bool AS>
	RationalFunction<Pol, AS>& RationalFunction<Pol, AS>::operator/=(const typename Pol::CoeffType& rhs)
	{
        if( this->isConstant() )
        {
            this->mNumberQuotient /= rhs;
            return *this;
        }
		mIsSimplified = false;
		mPolynomialQuotient->first /= rhs;
		return *this;
	}

	template<typename Pol, bool AS>
	bool operator==(const RationalFunction<Pol, AS>& lhs, const RationalFunction<Pol, AS>& rhs)
	{
        if( lhs.isConstant() )
        {
            if( rhs.isConstant() )
                return lhs.mNumberQuotient == rhs.mNumberQuotient;
            else
                return false;
        }
        if( rhs.isConstant() )
            return false;
		return lhs.nominatorAsPolynomial() == rhs.nominatorAsPolynomial() && lhs.denominatorAsPolynomial() == rhs.denominatorAsPolynomial();
	}
	
	template<typename Pol, bool AS>
	bool operator<(const RationalFunction<Pol, AS>& lhs, const RationalFunction<Pol, AS>& rhs)
	{
        if( lhs.isConstant() )
        {
            if( rhs.isConstant() )
                return lhs.mNumberQuotient < rhs.mNumberQuotient;
            else
                return true;
        }
        if( rhs.isConstant() )
            return false;
		return lhs.nominatorAsPolynomial()*rhs.denominatorAsPolynomial() < rhs.nominatorAsPolynomial()*lhs.denominatorAsPolynomial();
	}

    template<typename Pol, bool AS>
    std::string RationalFunction<Pol, AS>::toString(bool infix, bool friendlyNames) const {
        if (denominator().isOne()) {
            return nominatorAsPolynomial().toString(infix, friendlyNames);
        }

        if (infix) {
            return "(" + nominatorAsPolynomial().toString(infix, friendlyNames) + ")/(" + denominatorAsPolynomial().toString(infix, friendlyNames) + ")";
        } else {
            return "(/ " + nominatorAsPolynomial().toString(infix, friendlyNames) + " " + denominatorAsPolynomial().toString(infix, friendlyNames) + ")";
        }
    }

	template<typename Pol, bool AS>
	std::ostream& operator<<(std::ostream& os, const RationalFunction<Pol, AS>& rhs)
	{
        if( rhs.isConstant() )
            return os << rhs.mNumberQuotient;
		return os << "(" << rhs.nominatorAsPolynomial() << ")/(" << rhs.denominatorAsPolynomial() << ")";
	}
}
