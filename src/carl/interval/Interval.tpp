/**
 * The implementation for the templated interval class.
 *
 * @file Interval.tpp
 * @autor Stefan Schupp <stefan.schupp@cs.rwth-aachen.de>
 * 
 * @since	2013-12-20
 * @version 2014-01-30
 */
#pragma once
#include "Interval.h"
#include "../numbers/operations.h"
#include <iostream>

namespace carl
{
	
	using namespace boost::numeric;

/*******************************************************************************
 * Constructors & Destructor
 ******************************************************************************/
/*
template<typename Rational, typename Number>
Interval<Number>::Interval(Rational n);

template<typename Rational, typename Number>
Interval<Number>::Interval(Rational lower, Rational upper);

template<typename Rational, typename Number>
Interval<Number>::Interval(Rational lower, BoundType lowerBoundType, Rational upper, BoundType upperBoundType);
*/
/*******************************************************************************
 * Transformations and advanced getters/setters
 ******************************************************************************/

template<typename Number>
Sign Interval<Number>::sgn() const
	{
		assert(this->isConsistent());
		if (this->isUnbounded()) return Sign::ZERO;
		if ((mLowerBoundType == BoundType::STRICT && mContent.lower() >= 0) || (mLowerBoundType == BoundType::WEAK && mContent.lower() > 0)) return Sign::POSITIVE;
		if ((mUpperBoundType == BoundType::STRICT && mContent.upper() <= 0) || (mUpperBoundType == BoundType::WEAK && mContent.upper() < 0)) return Sign::NEGATIVE;
		return Sign::ZERO;
	}

template<typename Number>
Number Interval<Number>::diameter() const
	{
		assert(this->isConsistent());
		return boost::numeric::width(mContent);
	}

template<typename Number>
void Interval<Number>::diameter_assign()
	{
		this->set(BoostInterval(this->diameter()));
	}

template<typename Number>
	Number Interval<Number>::diameterRatio(const Interval<Number>& rhs) const
	{
		assert(rhs.diameter() != Number(0));
		return this->diameter()/rhs.diameter();
	}

template<typename Number>
	void Interval<Number>::diameterRatio_assign(const Interval<Number>& rhs)
	{
		this->set(BoostInterval(this->diameterRatio(rhs)));
	}

template<typename Number>
	Number Interval<Number>::magnitude() const
	{
		assert(this->isConsistent());
		return boost::numeric::norm(mContent);
	}

template<typename Number>
	void Interval<Number>::magnitude_assign()
	{
		this->set(BoostInterval(this->magnitude()));
	}

template<typename Number>
	Number Interval<Number>::center() const
	{
		assert(this->isConsistent());
		return boost::numeric::median(mContent);
	}

template<typename Number>
	void Interval<Number>::center_assign()
	{
		this->set(BoostInterval(this->center()));
	}

template<typename Number>
	Number Interval<Number>::sample() const
	{
		assert(this->isConsistent());
		Number mid = this->center();
		// TODO: check if mid is an integer already.
		Number midf = carl::floor(mid);
		if (this->contains(midf)) return midf;
		Number midc = carl::ceil(mid);
		if (this->contains(midc)) return midc;
		return mid;
	}

template<typename Number>
	void Interval<Number>::sample_assign()
	{
		this->set(BoostInterval(this->sample()));
	}

	template<typename Number>
	bool Interval<Number>::contains(const Number& val) const
	{
		assert(this->isConsistent());
            switch( mLowerBoundType )
            {
            case BoundType::INFTY:
                break;
            case BoundType::STRICT:
                if( mContent.lower() >= val )
                    return false;
                break;
            case BoundType::WEAK:
                if( mContent.lower() > val )
                    return false;
            }
            // Invariant: n is not conflicting with lower bound
            switch( mUpperBoundType )
            {
                case BoundType::INFTY:
                    break;
                case BoundType::STRICT:
                    if( mContent.upper() <= val )
                        return false;
                    break;
                case BoundType::WEAK:
                    if( mContent.upper() < val )
                        return false;
                    break;
            }
            return true;    // for open intervals: (lower() < n && upper() > n) || (n == Number(0) && lower() == cln::cl_RA( 0 ) && upper() == cln::cl_RA( 0 ))	}
	}
	
	template<typename Number>
	bool Interval<Number>::contains(const Interval<Number>& rhs) const
	{
		assert(this->isConsistent());
                // if one bound is totally wrong, we can just return false
                if (
                        (mContent.lower() > rhs.lower() && mLowerBoundType != BoundType::INFTY)
                        || (mContent.upper() < rhs.upper() && mUpperBoundType != BoundType::INFTY)
                    )
                {
                    return false;
                }

                // check the bounds
                bool lowerOk = mContent.lower() < rhs.lower() && rhs.lowerBoundType() != BoundType::INFTY;
                bool upperOk = mContent.upper() > rhs.upper() && rhs.upperBoundType() != BoundType::INFTY;
                
                // if both are ok, return true
		if( lowerOk && upperOk )
		{
                    return true;
		}
                
                // Note that from this point on at least one bound is equal
                // to our bounds but no bound is outside of our bounds
	
                // check the bound types
                bool lowerBoundTypesOk = getWeakestBoundType(mLowerBoundType, rhs.lowerBoundType()) == mLowerBoundType;
                bool upperBoundTypesOk = getWeakestBoundType(mUpperBoundType, rhs.upperBoundType()) == mUpperBoundType;

                // if upper bounds are ok and lower bound types are ok, return true
                if (upperOk && lowerBoundTypesOk)
                {
                    return true;
                }

                // if lower bounds are ok and upper bound types are ok, return true
                if (lowerOk && upperBoundTypesOk)
                {
                    return true;
                }
                
                // if both bound types are ok, return true
                if (lowerBoundTypesOk && upperBoundTypesOk)
                {
                    return true;
                }
            
                // otherwise return false
                return false; // not less and not equal
	}
	
	template<typename Number>
	bool Interval<Number>::meets(const Number& n) const
	{
		assert(this->isConsistent());
		return (mContent.lower() <= n || mLowerBoundType == BoundType::INFTY) && (mContent.upper() >= n || mUpperBoundType == BoundType::INFTY);
	}

	template<typename Number>
	bool Interval<Number>::isSubset(const Interval<Number>& rhs) const
	{
		assert(this->isConsistent());
		return rhs.contains(*this);
	}
	
	template<typename Number>
	bool Interval<Number>::isProperSubset(const Interval<Number>& rhs) const
	{
                return this->isSubset(rhs);
        }
	
template<typename Number>
	void Interval<Number>::bloat_by(const Number& width)
	{
                this->set(boost::numeric::widen(mContent, width));
	}

template<typename Number>
void Interval<Number>::shrink_by(const Number& width)
	{
		this->bloat_by(Number(-width));
	}

template<typename Number>
std::pair<Interval<Number>, Interval<Number>> Interval<Number>::split() const
	{
		std::pair<BoostInterval, BoostInterval> bisection = boost::numeric::bisect(mContent);
                if( this->isEmpty() || this->isPointInterval() )
                {
                    return std::pair<Interval<Number>, Interval<Number> >(Interval<Number>::emptyInterval(), Interval<Number>::emptyInterval());
                }
		return std::pair<Interval<Number>, Interval<Number> >(Interval(bisection.first, mLowerBoundType, BoundType::STRICT), Interval(bisection.second, BoundType::WEAK, mUpperBoundType));
	}

template<typename Number>
std::list<Interval<Number>> Interval<Number>::split(unsigned n) const
	{
		std::list<Interval<Number> > result;
		if(n == 0)
		{
			result.push_back(*this);
			return result;
		}
		Number diameter = this->diameter();
        diameter /= Number(n);
		
        Interval<Number> tmp;
        tmp.set(mContent.lower(), mContent.lower()+diameter);
        tmp.setLowerBoundType(mLowerBoundType);
        tmp.setUpperBoundType(BoundType::STRICT);
        result.push_back(tmp);
		
        for( unsigned i = 1; i < (n-1); ++i )
        {
            tmp.set(diameter*i, diameter*(i+1));
            result.push_back(tmp);
        }
		
        tmp.set(diameter*(n-1),diameter*n);
        tmp.setUpperBoundType(mUpperBoundType);
        result.push_back(tmp);
		return result;
	}
	
template<typename Number>
	std::string Interval<Number>::toString() const
	{
		std::ostringstream oss;
		switch (mLowerBoundType) {
			case BoundType::INFTY:
				oss << std::string("]-INF, ");
				break;
			case BoundType::STRICT:
				oss << std::string("]") << mContent.lower() << ", ";
				break;
			case BoundType::WEAK:
				oss << std::string("[") << mContent.lower() << ", ";
		}
		switch (mUpperBoundType) {
			case BoundType::INFTY:
				oss << std::string("INF[");
				break;
			case BoundType::STRICT:
				oss << mContent.upper() << std::string("[");
				break;
			case BoundType::WEAK:
				oss << mContent.upper() << std::string("]");
		}
		return oss.str();
	}

/*******************************************************************************
 * Arithmetic functions
 ******************************************************************************/

template<typename Number>
Interval<Number> Interval<Number>::add(const Interval<Number>& rhs) const
	{
		assert(this->isConsistent());
		assert(rhs.isConsistent());
		return Interval<Number>( mContent + rhs.content(),
                              getStrictestBoundType( mLowerBoundType, rhs.lowerBoundType() ),
                              getStrictestBoundType( mUpperBoundType, rhs.upperBoundType() ) );
	}

template<typename Number>
void Interval<Number>::add_assign(const Interval<Number>& rhs)
	{
		*this = this->add(rhs);
	}

template<typename Number>
Interval<Number> Interval<Number>::sub(const Interval<Number>& rhs) const
	{
		assert(this->isConsistent());
		assert(rhs.isConsistent());
		return this->add(rhs.inverse());
	}

template<typename Number>
void Interval<Number>::sub_assign(const Interval<Number>& rhs)
	{
		*this = this->sub(rhs);
	}

template<typename Number>
Interval<Number> Interval<Number>::mul(const Interval<Number>& rhs) const
	{
		assert(this->isConsistent());
		assert(rhs.isConsistent());
		BoundType lowerBoundType = BoundType::WEAK;
        BoundType upperBoundType = BoundType::WEAK;
        if( (mLowerBoundType == BoundType::INFTY && (rhs.upper() > Number(0) || rhs.upperBoundType() == BoundType::INFTY))
		   || (mUpperBoundType == BoundType::INFTY && (rhs.lower() < Number(0) || rhs.lowerBoundType() == BoundType::INFTY))
		   || (rhs.lowerBoundType() == BoundType::INFTY && (mContent.upper() > Number(0) || mUpperBoundType == BoundType::INFTY))
		   || (rhs.upperBoundType() == BoundType::INFTY && (mContent.upper() < Number(0) || (mContent.lower() < Number(0) || mLowerBoundType == BoundType::INFTY))) )
        {
            lowerBoundType = BoundType::INFTY;
        }
        if( (mLowerBoundType == BoundType::INFTY && (rhs.upper() < Number(0) || (rhs.lower() < Number(0) || rhs.lowerBoundType() == BoundType::INFTY)))
		   || (mUpperBoundType == BoundType::INFTY && (rhs.lower() > Number(0) || (rhs.upper() > Number(0) || rhs.upperBoundType() == BoundType::INFTY)))
		   || (rhs.lowerBoundType() == BoundType::INFTY && (mContent.upper() < Number(0) || (mContent.lower() < Number(0) || mLowerBoundType == BoundType::INFTY)))
		   || (rhs.upperBoundType() == BoundType::INFTY && (mContent.lower() > Number(0) || (mContent.upper() > Number(0) || mUpperBoundType == BoundType::INFTY))) )
        {
            upperBoundType = BoundType::INFTY;
        }
        return Interval<Number>(BoostInterval( mContent*rhs.content() ), lowerBoundType, upperBoundType );
	}

template<typename Number>
void Interval<Number>::mul_assign(const Interval<Number>& rhs)
	{
		*this = this->mul(rhs);
	}

template<typename Number>
Interval<Number> Interval<Number>::div(const Interval<Number>& rhs) const
	{
		assert(this->isConsistent());
		assert(rhs.isConsistent());
		BoundType lowerBoundType = BoundType::WEAK;
        BoundType upperBoundType = BoundType::WEAK;
        if( (mLowerBoundType == BoundType::INFTY && (rhs.upper() > Number(0) || rhs.upperBoundType() == BoundType::INFTY))
		   || (mUpperBoundType == BoundType::INFTY && (rhs.lower() < Number(0) || rhs.lowerBoundType() == BoundType::INFTY))
		   || (rhs.lowerBoundType() == BoundType::INFTY && ( mContent.upper() > Number(0) || mUpperBoundType == BoundType::INFTY))
		   || (rhs.upperBoundType() == BoundType::INFTY && ( mContent.upper() < Number(0) || ( mContent.lower() < Number(0) || mLowerBoundType == BoundType::INFTY))) )
        {
            lowerBoundType = BoundType::INFTY;
        }
        if( (mLowerBoundType == BoundType::INFTY && (rhs.upper() < Number(0) || (rhs.lower() < Number(0) || rhs.lowerBoundType() == BoundType::INFTY)))
		   || (mUpperBoundType == BoundType::INFTY && (rhs.lower() > Number(0) || (rhs.upper() > Number(0) || rhs.upperBoundType() == BoundType::INFTY)))
		   || (rhs.lowerBoundType() == BoundType::INFTY && ( mContent.upper() < Number(0) || ( mContent.lower() < Number(0) || mLowerBoundType == BoundType::INFTY)))
		   || (rhs.upperBoundType() == BoundType::INFTY && ( mContent.lower() > Number(0) || ( mContent.upper() > Number(0) || mUpperBoundType == BoundType::INFTY))) )
        {
            upperBoundType = BoundType::INFTY;
        }
		///@todo Correctly determine if bounds are strict or weak.
		if (this->isOpenInterval() || rhs.isOpenInterval()) {
			// just a quick heuristic, by no means complete.
            lowerBoundType = BoundType::STRICT;
            upperBoundType = BoundType::STRICT;
		}
        return Interval<Number>(BoostInterval( mContent/rhs.content() ), lowerBoundType, upperBoundType );
	}

template<typename Number>
void Interval<Number>::div_assign(const Interval<Number>& rhs)
	{
		*this = this->div(rhs);
	}

template<typename Number>
bool Interval<Number>::div_ext(const Interval<Number>& rhs, Interval<Number>& a, Interval<Number>& b) const
	{
		// Special case: if both contain 0 we can directly skip and return the unbounded interval.
		if(this->contains(Number(0)) && rhs.contains(Number(0)))
		{
			a = unboundedInterval();
			return false;
		}
		
		Interval<Number> reciprocalA, reciprocalB;
        bool          splitOccured;
		
        if( rhs.lowerBoundType() != BoundType::INFTY && rhs.lower() == Number(0) && rhs.upperBoundType() != BoundType::INFTY && rhs.upper() == Number(0) )    // point interval 0
        {
            splitOccured = false;
            if( this->contains( Number(0) ))
            {
                a = unboundedInterval();
            }
            else
            {
                a = emptyInterval();
            }
            return false;
        }
        else
        {
            if( rhs.isUnbounded() )
            {
                a = unboundedInterval();
                return false;
            }    // rhs.unbounded
            else
            {
                //default case
                splitOccured = rhs.reciprocal( reciprocalA, reciprocalB );
                if( !splitOccured )
                {
                    a = this->mul( reciprocalA );
                    return false;
                }
                else
                {
                    a = this->mul( reciprocalA );
                    b = this->mul( reciprocalB );
					
                    if( a == b )
                    {
                        return false;
                    }
                    else
                    {
                        return true;
                    }
					
                }
            }    // !rhs.unbounded
			
        }    // !pointinterval 0
	}

template<typename Number>
Interval<Number> Interval<Number>::inverse() const
	{
		assert(this->isConsistent());
		return Interval<Number>( mContent.upper()*Number(-1), mUpperBoundType, mContent.lower()*Number(-1), mLowerBoundType );
	}

template<typename Number>
void Interval<Number>::inverse_assign()
	{
		*this = this->inverse();
	}

template<typename Number>
bool Interval<Number>::reciprocal(Interval<Number>& a, Interval<Number>& b) const
	{
		if( this->isUnbounded() )
        {
            a = emptyInterval();
            return false;
        }
        else if( this->contains( Number(0) ) && mContent.lower() != Number(0) && mContent.upper() != Number(0) )
        {
            if( mLowerBoundType == BoundType::INFTY )
            {
                a = Interval<Number>(Number(0), BoundType::INFTY,Number(0), BoundType::WEAK );
                b = Interval<Number>(BoostInterval( Number(1) ) /BoostInterval( mContent.upper() ), BoundType::WEAK, BoundType::INFTY );
            }
            else if( mUpperBoundType == BoundType::INFTY )
            {
                a = Interval<Number>(BoostInterval( Number(1) ) /BoostInterval( mContent.lower() ), BoundType::INFTY, BoundType::WEAK );
                b = Interval<Number>(Number(0), BoundType::WEAK,Number(0), BoundType::INFTY );
            }
            else if( mContent.lower() == Number(0) && mContent.upper() != Number(0) )
            {
                a = Interval<Number>(Number(0), BoundType::INFTY, Number(0), BoundType::INFTY );
                b = Interval<Number>(BoostInterval( Number(1) ) /BoostInterval( mContent.upper() ), BoundType::WEAK, BoundType::INFTY );
            }
            else if( mContent.lower() != Number(0) && mContent.upper() == Number(0) )
            {
                a = Interval<Number>(BoostInterval( Number(1) ) /BoostInterval( mContent.lower() ), BoundType::INFTY, BoundType::WEAK );
                b = unboundedInterval(); // todo: really the whole interval here?
            }
            else if( mContent.lower() == Number(0) && mContent.upper() == Number(0) )
            {
                a = unboundedInterval();
                return false;
            }
            else
            {
                a = Interval<Number>(BoostInterval( Number(1) ) /BoostInterval( mContent.lower() ), BoundType::INFTY, BoundType::WEAK );
                b = Interval<Number>(BoostInterval( Number(1) ) /BoostInterval( mContent.upper() ), BoundType::WEAK, BoundType::INFTY );
            }
            return true;
        }
        else
        {
            if( mLowerBoundType == BoundType::INFTY && mContent.upper() != Number(0) )
            {
                a = Interval<Number>(  Number(1) / mContent.upper() , mUpperBoundType,Number(0),  BoundType::WEAK );
            }
            else if( mLowerBoundType == BoundType::INFTY && mContent.upper() == Number(0) )
            {
                a = Interval<Number>(Number(0), BoundType::INFTY, Number(0), BoundType::WEAK );
            }
            else if( mUpperBoundType == BoundType::INFTY && mContent.lower() != Number(0) )
            {
                a = Interval<Number>(  Number(0) , BoundType::WEAK, Number(1)  /  mContent.lower(), mLowerBoundType );
            }
            else if( mUpperBoundType == BoundType::INFTY && mContent.lower() == Number(0) )
            {
                a = Interval<Number>(Number(0), BoundType::WEAK,Number(0), BoundType::INFTY );
            }
            else if( mContent.lower() != Number(0) && mContent.upper() != Number(0) )
            {
                a = Interval<Number>(BoostInterval( Number(1) ) / mContent, mUpperBoundType, mLowerBoundType );
            }
            else if( mContent.lower() == Number(0) && mContent.upper() != Number(0) )
            {
                a = Interval<Number>(BoostInterval( Number(1) ) /BoostInterval( mContent.upper() ), mUpperBoundType, BoundType::INFTY );
            }
            else if( mContent.lower() != Number(0) && mContent.upper() == Number(0) )
            {
                a = Interval<Number>(BoostInterval( Number(1) ) /BoostInterval( mContent.lower() ), BoundType::INFTY, mLowerBoundType );
            }
			
            return false;
        }
	}

template<typename Number>
Interval<Number> Interval<Number>::power(unsigned exp) const
	{
		assert(this->isConsistent());
		assert(exp <= INT_MAX );
        if( exp % 2 == Number(0) )
        {
            if( mLowerBoundType == BoundType::INFTY && mUpperBoundType == BoundType::INFTY )
            {
                return unboundedInterval();
            }
            else if( mLowerBoundType == BoundType::INFTY )
            {
                if( contains( Number(0) ) )
                {
                    return Interval<Number>( Number(0), BoundType::WEAK, Number(0), BoundType::INFTY );
                }
                else
                {
                    return Interval<Number>( boost::numeric::pow( mContent, (int)exp ), mUpperBoundType, BoundType::INFTY );
                }
            }
            else if( mUpperBoundType == BoundType::INFTY )
            {
                if( contains( Number(0) ) )
                {
                    return Interval<Number>( Number(0), BoundType::WEAK, Number(0), BoundType::INFTY );
                }
                else
                {
                    return Interval<Number>( boost::numeric::pow( mContent, (int)exp ), mLowerBoundType, BoundType::INFTY );
                }
            }
            else
            {
                BoundType rType = mUpperBoundType;
                BoundType lType = mLowerBoundType;
                if( carl::abs( mContent.lower() ) > carl::abs( mContent.upper() ) )
                {
                    rType = mLowerBoundType;
                    lType = mUpperBoundType;
                }
                if( contains( Number(0) ) )
                {
                    return Interval<Number>( boost::numeric::pow( mContent, (int)exp ), BoundType::WEAK, rType );
                }
                else
                {
                    return Interval<Number>( boost::numeric::pow( mContent, (int)exp ), lType, rType );
                }
            }
        }
        else
        {
            return Interval<Number>( boost::numeric::pow( mContent, (int)exp ), mLowerBoundType, mUpperBoundType );
        }

	}

template<typename Number>
void Interval<Number>::power_assign(unsigned exp)
	{
		*this = this->power(exp);
	}

template<typename Number>
Interval<Number> Interval<Number>::sqrt() const
	{
		assert(this->isConsistent());
		return Interval<Number>(boost::numeric::sqrt(mContent), mLowerBoundType, mUpperBoundType);
	}

template<typename Number>
void Interval<Number>::sqrt_assign()
	{
		*this = this->sqrt();
	}

	
template<typename Number>
Interval<Number> Interval<Number>::root(unsigned deg) const
	{
		assert(this->isConsistent());
		return Interval<Number>(boost::numeric::nth_root(mContent, deg), mLowerBoundType, mUpperBoundType);
	}

template<typename Number>
void Interval<Number>::root_assign(unsigned deg)
	{
		*this = this->root(deg);
	}

template<typename Number>
Interval<Number> Interval<Number>::log() const
	{
		assert(this->isConsistent());
		assert( mContent.lower() > Number(0) );
        return Interval<Number>(boost::numeric::log(mContent), mLowerBoundType, mUpperBoundType);
	}

template<typename Number>
void Interval<Number>::log_assign()
	{
		this->set(boost::numeric::log(mContent));
	}

/*******************************************************************************
 * Trigonometric functions
 ******************************************************************************/

template<typename Number>
Interval<Number> Interval<Number>::sin() const
	{
		assert(this->isConsistent());
		return Interval<Number>(boost::numeric::sin(mContent), mLowerBoundType, mUpperBoundType);
	}

template<typename Number>
void Interval<Number>::sin_assign()
	{
		this->set(boost::numeric::sin(mContent));
	}

template<typename Number>
Interval<Number> Interval<Number>::cos() const
	{
		assert(this->isConsistent());
		return Interval<Number>(boost::numeric::cos(mContent), mLowerBoundType, mUpperBoundType);
	}

template<typename Number>
void Interval<Number>::cos_assign()
	{
		this->set(boost::numeric::cos(mContent));
	}

template<typename Number>
Interval<Number> Interval<Number>::tan() const
	{
		assert(this->isConsistent());
		return Interval<Number>(boost::numeric::tan(mContent), mLowerBoundType, mUpperBoundType);
	}

template<typename Number>
void Interval<Number>::tan_assign()
	{
		this->set(boost::numeric::tan(mContent));
	}

template<typename Number>
Interval<Number> Interval<Number>::asin() const
	{
		assert(this->isConsistent());
		return Interval<Number>(boost::numeric::asin(mContent), mLowerBoundType, mUpperBoundType);
	}

template<typename Number>
void Interval<Number>::asin_assign()
	{
		this->set(boost::numeric::asin(mContent));
	}

template<typename Number>
Interval<Number> Interval<Number>::acos() const
	{
		assert(this->isConsistent());
		return Interval<Number>(boost::numeric::acos(mContent), mLowerBoundType, mUpperBoundType);
	}

template<typename Number>
void Interval<Number>::acos_assign()
	{
		this->set(boost::numeric::acos(mContent));
	}

template<typename Number>
Interval<Number> Interval<Number>::atan() const
	{
		assert(this->isConsistent());
		return Interval<Number>(boost::numeric::atan(mContent), mLowerBoundType, mUpperBoundType);
	}

template<typename Number>
void Interval<Number>::atan_assign()
	{
		this->set(boost::numeric::atan(mContent));
	}

template<typename Number>
Interval<Number> Interval<Number>::sinh() const
	{
		assert(this->isConsistent());
		return Interval<Number>(boost::numeric::sinh(mContent), mLowerBoundType, mUpperBoundType);
	}

template<typename Number>
void Interval<Number>::sinh_assign()
	{
		this->set(boost::numeric::sinh(mContent));
	}

template<typename Number>
Interval<Number> Interval<Number>::cosh() const
	{
		assert(this->isConsistent());
		return Interval<Number>(boost::numeric::cosh(mContent), mLowerBoundType, mUpperBoundType);
	}

template<typename Number>
void Interval<Number>::cosh_assign()
	{
		this->set(boost::numeric::cosh(mContent));
	}

template<typename Number>
Interval<Number> Interval<Number>::tanh() const
	{
		assert(this->isConsistent());
		return Interval<Number>(boost::numeric::tanh(mContent), mLowerBoundType, mUpperBoundType);
	}

template<typename Number>
void Interval<Number>::tanh_assign()
	{
		this->set(boost::numeric::tanh(mContent));
	}

template<typename Number>
Interval<Number> Interval<Number>::asinh() const
	{
		assert(this->isConsistent());
		return Interval<Number>(boost::numeric::asinh(mContent), mLowerBoundType, mUpperBoundType);
	}

template<typename Number>
void Interval<Number>::asinh_assign()
	{
		this->set(boost::numeric::asinh(mContent));
	}

template<typename Number>
Interval<Number> Interval<Number>::acosh() const
	{
		assert(this->isConsistent());
		return Interval<Number>(boost::numeric::acosh(mContent), mLowerBoundType, mUpperBoundType);
	}

template<typename Number>
void Interval<Number>::acosh_assign()
	{
		this->set(boost::numeric::acosh(mContent));
	}

template<typename Number>
Interval<Number> Interval<Number>::atanh() const
	{
		assert(this->isConsistent());
		return Interval<Number>(boost::numeric::atanh(mContent), mLowerBoundType, mUpperBoundType);
	}

template<typename Number>
void Interval<Number>::atanh_assign()
	{
		this->set(boost::numeric::atanh(mContent));
	}

/*******************************************************************************
 * Boolean operations
 ******************************************************************************/
	
template<typename Number>
	Interval<Number> Interval<Number>::intersect(const Interval<Number>& rhs) const
	{
		assert(this->isConsistent());
		assert(rhs.isConsistent());
		Number lowerValue;
                Number upperValue;
                BoundType maxLowest;
                BoundType minUppest;
                // determine value first by: LowerValue = max ( lowervalues ) where max considers infty.
                if ( mLowerBoundType != BoundType::INFTY && rhs.lowerBoundType() != BoundType::INFTY )
                {
                    if ( mContent.lower() < rhs.lower() )
                    {
                        lowerValue = rhs.lower();
                        maxLowest = rhs.lowerBoundType();
                    }
                    else if ( rhs.lower() < mContent.lower() )
                    {
                        lowerValue = mContent.lower();
                        maxLowest = mLowerBoundType;
                    }
                    else
                    {
                        lowerValue = mContent.lower();
                        maxLowest = getWeakestBoundType(mLowerBoundType, rhs.lowerBoundType());
                    }
                }
                else if ( mLowerBoundType == BoundType::INFTY && rhs.lowerBoundType() != BoundType::INFTY )
                {
                    lowerValue = rhs.lower();
                    maxLowest = rhs.lowerBoundType();
                }
                else if ( mLowerBoundType != BoundType::INFTY && rhs.lowerBoundType() == BoundType::INFTY )
                {
                    lowerValue = mContent.lower();
                    maxLowest = mLowerBoundType;
                }
                else
                {
                    lowerValue = Number(0);
                    maxLowest = BoundType::INFTY;
                }

                // determine value first by: UpperValue = min ( uppervalues ) where min considers infty.
                if ( mUpperBoundType != BoundType::INFTY && rhs.upperBoundType() != BoundType::INFTY )
                {
                    if ( mContent.upper() > rhs.upper() )
                    {
                        upperValue = rhs.upper();
                        minUppest = rhs.upperBoundType();
                    }
                    else if ( rhs.upper() > mContent.upper() )
                    {
                        upperValue = mContent.upper();
                        minUppest = mUpperBoundType;
                    }
                    else
                    {
                        upperValue = mContent.upper();
                        minUppest = getWeakestBoundType(mUpperBoundType, rhs.upperBoundType());
                    }
                    if( maxLowest == BoundType::INFTY )
                    {
                        lowerValue = upperValue;
                    }
                }
                else if ( mUpperBoundType == BoundType::INFTY && rhs.upperBoundType() != BoundType::INFTY )
                {
                    upperValue = rhs.upper();
                    minUppest = rhs.upperBoundType();
                    if( maxLowest == BoundType::INFTY )
                    {
                        lowerValue = upperValue;
                    }
                }
                else if ( mUpperBoundType != BoundType::INFTY && rhs.upperBoundType() == BoundType::INFTY )
                {
                    upperValue = mContent.upper();
                    minUppest = mUpperBoundType;
                    if( maxLowest == BoundType::INFTY )
                    {
                        lowerValue = upperValue;
                    }
                }
                else
                {
                    upperValue = lowerValue;
                    minUppest = BoundType::INFTY;
                }
                return Interval<Number>(lowerValue, maxLowest, upperValue, minUppest );
	}
	
template<typename Number>
	Interval<Number>& Interval<Number>::intersect_assign(const Interval<Number>& rhs)
	{
		*this = this->intersect(rhs);
	}
	
	template<typename Number>
	bool Interval<Number>::unite(const Interval<Number>& rhs, Interval<Number>& resultA, Interval<Number>& resultB) const
	{
		assert(this->isConsistent());
		assert(rhs.isConsistent());
		if( this->intersect(rhs).isEmpty() )
		{
                    // special case: [a;b] U (b;c]
                    if( (mContent.lower() == rhs.upper() && (rhs.upperBoundType() == BoundType::WEAK || mLowerBoundType == BoundType::WEAK) ) ||
                        (mContent.upper() == rhs.lower() && (mUpperBoundType == BoundType::WEAK || rhs.lowerBoundType() == BoundType::WEAK) ) ) 
                    {
                        Number lower = rhs.lower() <= mContent.lower() ? rhs.lower() : mContent.lower();
                        Number upper = rhs.upper() >= mContent.upper() ? rhs.upper() : mContent.upper();
                        BoundType lowerBoundType = lower == mContent.lower() ? mLowerBoundType : rhs.lowerBoundType();
                        BoundType upperBoundType = upper == mContent.upper() ? mUpperBoundType : rhs.upperBoundType();
                        resultA = Interval<Number>(lower, lowerBoundType, upper, upperBoundType);
                        return false;
                    }
                    else
                    {
                        resultA = *this;
			resultB = rhs;
			return true;
                    }		
		}
		else
		{
			Number lower;
			Number upper;
			BoundType lowerType = getWeakestBoundType(mLowerBoundType, rhs.lowerBoundType());
			BoundType upperType = getWeakestBoundType(mUpperBoundType, rhs.upperBoundType());
			// calculate lowerBound and lowerBoundType
                        if( lowerType != BoundType::INFTY )
                        {
                            if( mContent.lower() < rhs.lower() )
                            {
                                    lower = mContent.lower();
                                    lowerType = mLowerBoundType;
                            }
                            else if ( mContent.lower() == rhs.lower() )
                            {
                                    lower = mContent.lower();
                                    lowerType = getWeakestBoundType(mLowerBoundType, rhs.lowerBoundType());
                            }
                            else
                            {
                                    lower = rhs.lower();
                                    lowerType = rhs.lowerBoundType();
                            }
                        }
                        else
                        {
                            lower = mContent.upper();
                        }
			
			// calculate upperBound and upperBoundType
                        if( upperType != BoundType::INFTY )
                        {
                            if( mContent.upper() > rhs.upper() )
                            {
                                    upper = mContent.upper();
                                    upperType = mUpperBoundType;
                            }
                            else if ( mContent.upper() == rhs.upper() )
                            {
                                    upper = mContent.upper();
                                    upperType = getWeakestBoundType(mUpperBoundType, rhs.upperBoundType());
                            }
                            else
                            {
                                    upper = rhs.upper();
                                    upperType = rhs.upperBoundType();
                            }
                        }
                        else
                        {
                            upper = lower;
                        }
			
			resultA = Interval<Number>(lower, lowerType, upper, upperType);
			return false;
		}
	}
	
	template<typename Number>
	bool Interval<Number>::difference(const Interval<Number>& rhs, Interval<Number>& resultA, Interval<Number>& resultB) const
	{
		assert(this->isConsistent());
		assert(rhs.isConsistent());
                if( rhs.isEmpty() )
                {
                    resultA = *this;
                    return false;
                }
                if ( this->isEmpty() )
                {
                    resultA = rhs;
                    return false;
                }
                // check for subset before contains because we may want to get
                // the difference from ourselves which is empty.
		if( this->isSubset(rhs) )
                {
                    resultA = emptyInterval();
                    return false;
                }
                else if( this->contains(rhs) )
                {
                    if( mContent.lower() != rhs.lower() && mContent.upper() != rhs.upper() )
                    {
			BoundType upperType = getOtherBoundType(rhs.lowerBoundType());
			BoundType lowerType = getOtherBoundType(rhs.upperBoundType());
			resultA = Interval<Number>(mContent.lower(), mLowerBoundType, rhs.lower(), upperType);
			resultB = Interval<Number>(rhs.upper(), lowerType, mContent.upper(), mUpperBoundType);
			return true;
                    }
                    else if( mContent.lower() == rhs.lower() && mContent.upper() != rhs.upper() )
                    {
                        if( mLowerBoundType == rhs.lowerBoundType() )
                        {
                            BoundType lowerType = getOtherBoundType(rhs.upperBoundType());
                            resultA = Interval<Number>(rhs.upper(), lowerType, mContent.upper(), mUpperBoundType );
                            return false;
                        }
                        else
                        {
                            resultA = Interval<Number>( mContent.lower(),mLowerBoundType, mContent.lower(), getOtherBoundType(rhs.lowerBoundType()) );
                            resultB = Interval<Number>(rhs.upper(), getOtherBoundType(rhs.upperBoundType()), mContent.upper(), mUpperBoundType);
                            return true;
                        }
                    }
                    else if( mContent.lower() != rhs.lower() && mContent.upper() == rhs.upper() )
                    {
                        if( mUpperBoundType == rhs.upperBoundType() )
                        {
                            BoundType upperType = getOtherBoundType(rhs.lowerBoundType());
                            BoundType lowerType = mLowerBoundType;
                            resultA = Interval<Number>(mContent.lower(), lowerType, rhs.upper(), upperType);
                            return false;
                        }
                        else
                        {
                            resultA = Interval<Number>(mContent.lower(), mLowerBoundType, rhs.upper(), getOtherBoundType(rhs.upperBoundType()));
                            resultB = Interval<Number>(mContent.upper(), getOtherBoundType(rhs.upperBoundType()), mContent.upper(), mUpperBoundType);
                            return true;
                        }
                    }
                    else if( mContent.lower() == rhs.lower() && mContent.upper() == rhs.upper() )
                    {
                        resultA = emptyInterval();
                        return false;
                    }
                    // Todo return festlegen.
		}
		else
		{
			if( this->contains(rhs.lower()) )
			{
                            BoundType upperType = getOtherBoundType(rhs.lowerBoundType());
                            resultA = Interval<Number>(mContent.lower(), mLowerBoundType, rhs.lower(), upperType);
			}
			else if( this->contains(rhs.upper()) )
			{
                            BoundType lowerType = getOtherBoundType(rhs.upperBoundType());
                            resultA = Interval<Number>(rhs.upper(), lowerType, mContent.upper(), mUpperBoundType);
			}
			else //both are totally distinct
			{
                            resultA = *this;
			}
			return false;
		}
	}
	
	template<typename Number>
	bool Interval<Number>::complement(Interval<Number>& resultA, Interval<Number>& resultB) const
	{
		BoundType upperType;
		BoundType lowerType;
		switch (mLowerBoundType) {
			case BoundType::INFTY:
				if(mUpperBoundType == BoundType::INFTY)
				{
					resultA = emptyInterval();
				}
				else
				{
					lowerType = mUpperBoundType == BoundType::STRICT ? BoundType::WEAK : BoundType::STRICT;
					resultA = Interval<Number>(mContent.upper(), lowerType, mContent.upper(), BoundType::INFTY);
				}
				return false;
				
			default:
				switch (mUpperBoundType) {
					case BoundType::INFTY:
						upperType = mLowerBoundType == BoundType::STRICT ? BoundType::WEAK : BoundType::STRICT;
						resultA = Interval<Number>(mContent.lower(), BoundType::INFTY, mContent.lower(), upperType);
						return false;
						
					default:
						upperType = mLowerBoundType == BoundType::STRICT ? BoundType::WEAK : BoundType::STRICT;
						lowerType = mUpperBoundType == BoundType::STRICT ? BoundType::WEAK : BoundType::STRICT;
						resultA = Interval<Number>(mContent.lower(), BoundType::INFTY, mContent.lower(), upperType);
						resultB = Interval<Number>(mContent.upper(), lowerType, mContent.upper(), BoundType::INFTY);
						return true;
				}
		}
	}
	
	template<typename Number>
	bool Interval<Number>::symmetricDifference(const Interval<Number>& rhs, Interval<Number>& resultA, Interval<Number>& resultB) const
	{
		assert(this->isConsistent());
		assert(rhs.isConsistent());
		Interval<Number> intersection = this->intersect(rhs);
		if( !intersection.isEmpty() )
		{
			Interval<Number> tmp;
			this->unite(rhs, tmp, tmp); //we know this will result in exactly one interval as the intersection is not empty
			return tmp.difference(intersection, resultA, resultB);
 		}
		resultA = *this;
		resultB = rhs;
		return true;
	}
	
/*******************************************************************************
 * Overloaded arithmetics operators
 ******************************************************************************/

template<typename Number>
inline const Interval<Number> operator +(const Interval<Number>& lhs, const Interval<Number>& rhs)
	{
		return lhs.add(rhs);
	}

template<typename Number>
inline const Interval<Number> operator +(const Number& lhs, const Interval<Number>& rhs)
	{
		return Interval<Number>( lhs + rhs.content().lower(), rhs.lowerBoundType(), lhs + rhs.content().upper(), rhs.upperBoundType());
	}

template<typename Number>
inline const Interval<Number> operator +(const Interval<Number>& lhs, const Number& rhs)
	{
		return rhs + lhs;
	}

template<typename Number>
inline const Interval<Number> operator +=(Interval<Number>& lhs, const Interval<Number>& rhs)
	{
		lhs.add_assign(rhs);
                return lhs;
	}

template<typename Number>
inline const Interval<Number> operator +=(Interval<Number>& lhs, const Number& rhs)
	{
		return lhs += Interval<Number>(rhs);
	}

template<typename Number>
inline const Interval<Number> operator -(const Interval<Number>& lhs, const Interval<Number>& rhs)
	{
		return lhs.sub(rhs);
	}

template<typename Number>
inline const Interval<Number> operator -(const Number& lhs, const Interval<Number>& rhs)
	{
		return Interval<Number>(lhs).sub(rhs);
	}

template<typename Number>
inline const Interval<Number> operator -(const Interval<Number>& lhs, const Number& rhs)
	{
		return lhs.sub(Interval<Number>(rhs));
	}

template<typename Number>
inline const Interval<Number> operator *(const Interval<Number>& lhs, const Interval<Number>& rhs)
	{
		return lhs.mul(rhs);
	}

template<typename Number>
inline const Interval<Number> operator *(const Number& lhs, const Interval<Number>& rhs)
	{
		return Interval<Number>(lhs * rhs.content().lower(), rhs.lowerBoundType(), lhs * rhs.content().upper(), rhs.upperBoundType());
	}

template<typename Number>
inline const Interval<Number> operator *(const Interval<Number>& lhs, const Number& rhs)
	{
		return rhs * lhs;
	}

template<typename Number>
inline const Interval<Number> operator *=(Interval<Number>& lhs, const Interval<Number>& rhs)
	{
		lhs.mul_assign(rhs);
                return lhs;
	}

template<typename Number>
inline const Interval<Number> operator *=(Interval<Number>& lhs, const Number& rhs)
	{
		return lhs *= Interval<Number>(rhs);
	}

template<typename Number>
inline const Interval<Number> operator /(const Interval<Number>& lhs, const Interval<Number>& rhs)
	{
		return lhs.div(rhs);
	}

template<typename Number>
inline const Interval<Number> operator /(const Number& lhs, const Interval<Number>& rhs)
	{
		return Interval<Number>(lhs).div_assign(rhs);
	}

template<typename Number>
inline const Interval<Number> operator /(const Interval<Number>& lhs, const Number& rhs)
	{
		return lhs.div(Interval<Number>(rhs));
	}

/*******************************************************************************
 * Comparison operators
 ******************************************************************************/

template<typename Number>
inline bool operator ==(const Interval<Number>& lhs, const Interval<Number>& rhs)
	{
		return lhs.content().lower() == rhs.content().lower() && lhs.content().upper() == rhs.content().upper() && lhs.lowerBoundType() == rhs.lowerBoundType() && lhs.upperBoundType() == rhs.upperBoundType();
	}

template<typename Number>
inline bool operator !=(const Interval<Number>& lhs, const Interval<Number>& rhs)
	{
		return !(lhs == rhs);
	}

template<typename Number>
inline bool operator <=(const Interval<Number>& lhs, const Interval<Number>& rhs)
	{
		if( lhs < rhs )
		{
			return true;
		}
		
		if( lhs.upper() == rhs.lower() )
		{
			switch (lhs.upperBoundType()) {
				case BoundType::STRICT:
				case BoundType::WEAK:
					return rhs.lowerBoundType() != BoundType::INFTY;
				default:
					return false;
			}
		}
		else
		{
			return false;
		}
		
	}

template<typename Number>
inline bool operator >=(const Interval<Number>& lhs, const Interval<Number>& rhs)
	{
		return rhs <= lhs;
	}

template<typename Number>
inline bool operator <(const Interval<Number>& lhs, const Interval<Number>& rhs)
	{
		if( lhs.upper() < rhs.lower() )
		{
			return true;
		}
		
		if( lhs.upper() == rhs.lower() )
		{
			switch (lhs.upperBoundType()) {
				case BoundType::STRICT:
					return rhs.lowerBoundType() != BoundType::INFTY;
				case BoundType::WEAK:
					return rhs.lowerBoundType() == BoundType::STRICT;
				default:
					return false;
			}
		}
		else
		{
			return false;
		}
	}

template<typename Number>
inline bool operator >(const Interval<Number>& lhs, const Interval<Number>& rhs)
	{
		return rhs < lhs;
	}
}