/**
 * The implementation for the templated interval class.
 *
 * @file Interval.tpp
 * @author Stefan Schupp <stefan.schupp@cs.rwth-aachen.de>
 *
 * @since	2013-12-20
 * @version 2014-11-11
 */
#pragma once
#include "Interval.h"

#include "../numbers/numbers.h"

#include <iostream>

namespace carl
{
	namespace bn = boost::numeric;

/*******************************************************************************
 * Transformations and advanced getters/setters
 ******************************************************************************/

template<typename Number>
Sign Interval<Number>::sgn() const
{
    assert(this->isConsistent());
    if (this->isInfinite()) return Sign::ZERO;
    if ((mLowerBoundType == BoundType::STRICT && mContent.lower() >= carl::constant_zero<Number>().get()) || (mLowerBoundType == BoundType::WEAK && mContent.lower() > carl::constant_zero<Number>().get())) return Sign::POSITIVE;
    if ((mUpperBoundType == BoundType::STRICT && mContent.upper() <= carl::constant_zero<Number>().get()) || (mUpperBoundType == BoundType::WEAK && mContent.upper() < carl::constant_zero<Number>().get())) return Sign::NEGATIVE;
    return Sign::ZERO;
}

template<typename Number>
Interval<Number> Interval<Number>::integralPart() const
{
	if(this->isEmpty())
		return *this;

	Number newLowerBound = 0;
	Number newUpperBound = 0;
	BoundType newLowerBoundType = mLowerBoundType;
	BoundType newUpperBoundType = mUpperBoundType;

	switch(mLowerBoundType) {
		case BoundType::WEAK:
			newLowerBound = ceil(mContent.lower());
			newLowerBoundType = BoundType::WEAK;
			break;
		case BoundType::STRICT:
			newLowerBound = ceil(mContent.lower());
			newLowerBoundType = BoundType::WEAK;
			if(newLowerBound == mContent.lower())
				newLowerBound += carl::constant_one<Number>::get();
			break;
		default:
			break;
	}
	switch(mUpperBoundType) {
		case BoundType::WEAK:
			newUpperBound = floor(mContent.upper());
			newUpperBoundType = BoundType::WEAK;
			if(newLowerBoundType == BoundType::INFTY)
				newLowerBound = newUpperBound;
			break;
		case BoundType::STRICT:
			newUpperBound = floor(mContent.upper());
			newUpperBoundType = BoundType::WEAK;
			if(newUpperBound == mContent.upper())
				newUpperBound -= carl::constant_one<Number>::get();
			if(newLowerBoundType == BoundType::INFTY)
				newLowerBound = newUpperBound;
			break;
		default:
			if(newLowerBoundType != BoundType::INFTY)
				newUpperBound = newLowerBound;
			break;
	}
	return Interval<Number>(newLowerBound, newLowerBoundType, newUpperBound, newUpperBoundType);
}

template<typename Number>
        void Interval<Number>::integralPart_assign()
{
    *this = integralPart();
}

template<typename Number>
bool Interval<Number>::containsInteger() const
	{
		assert(this->isConsistent());
		switch (mLowerBoundType) {
			case BoundType::INFTY:
				return true;
			case BoundType::STRICT:
				break;
			case BoundType::WEAK:
				if (carl::isInteger(mContent.lower())) return true;
		}
		switch (mUpperBoundType) {
			case BoundType::INFTY:
				return true;
			case BoundType::STRICT:
				break;
			case BoundType::WEAK:
				if (carl::isInteger(mContent.upper())) return true;
		}
		return carl::floor(mContent.lower()) + carl::constant_one<Number>::get() < mContent.upper();
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
		assert(rhs.diameter() != carl::constant_zero<Number>().get());
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
	void Interval<Number>::center_assign()
	{
		this->set(BoostInterval(this->center()));
	}

template<typename Number>
	Number Interval<Number>::sample( bool _includingBounds ) const
	{
		return carl::sample(*this, _includingBounds);
	}
template<typename Number>
	Number Interval<Number>::sampleSB( bool _includingBounds ) const
	{
		return carl::sample_stern_brocot(*this, _includingBounds);
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
        return true;    // for open intervals: (lower() < n && upper() > n) || (n == carl::constant_zero<Number>().get() && lower() == cln::cl_RA( 0 ) && upper() == cln::cl_RA( 0 ))	}
	}

	template<typename Number>
	bool Interval<Number>::contains(const Interval<Number>& rhs) const
	{
		assert(this->isConsistent());
        if( rhs.isEmpty() )
            return true;
        // if one bound is totally wrong, we can just return false
        if((mContent.lower() > rhs.lower() && mLowerBoundType != BoundType::INFTY) || (mContent.upper() < rhs.upper() && mUpperBoundType != BoundType::INFTY))
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
        return set_subset(rhs, *this);
	}

	template<typename Number>
	bool Interval<Number>::isProperSubset(const Interval<Number>& rhs) const
	{
        return set_proper_subset(rhs, *this);
    }

    template<typename Number>
    void Interval<Number>::bloat_by(const Number& width)
    {
	if(!isInfinite()){
	    BoundType lowerTmp = mLowerBoundType;
	    BoundType upperTmp = mUpperBoundType;
	    this->set(boost::numeric::widen(mContent, width));
	    mLowerBoundType = lowerTmp;
	    mUpperBoundType = upperTmp;
	} else if (mLowerBoundType != BoundType::INFTY) {
	    this->set(boost::numeric::widen(mContent, width));
	    mUpperBoundType = BoundType::INFTY;
	} else if (mUpperBoundType != BoundType::INFTY) {
	    this->set(boost::numeric::widen(mContent, width));
	    mLowerBoundType = BoundType::INFTY;
	}
    }

    template<typename Number>
    void Interval<Number>::shrink_by(const Number& width)
    {
	this->bloat_by(Number(-1)*width);
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
			assert(false);
			result.emplace_back(emptyInterval());
			return result;
		} else if ( n == 1) {
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

        tmp.setLowerBoundType(BoundType::WEAK);
        for( unsigned i = 1; i < (n-1); ++i )
        {
        	tmp += diameter;
            result.push_back(tmp);
        }

        tmp += diameter;
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


/**
* Calculates the distance between two Intervals.
* @param intervalA Interval to which we want to know the distance.
* @return distance to intervalA
*/
template<typename Number>
Number Interval<Number>::distance(const Interval<Number>& intervalA)
{
    if( set_intersect(*this, intervalA) )
        return carl::constant_zero<Number>::get();
    if( intervalA.upperBoundType() == BoundType::INFTY || this->lowerBoundType() == BoundType::INFTY )
    {
        assert( this->upperBoundType() != BoundType::INFTY );
        assert( intervalA.lowerBoundType() != BoundType::INFTY );
        return carl::abs(Number(intervalA.lower() - this->upper()));
    }
    else if( intervalA.lowerBoundType() == BoundType::INFTY || this->upperBoundType() == BoundType::INFTY )
    {
        assert( this->lowerBoundType() != BoundType::INFTY );
        assert( intervalA.upperBoundType() != BoundType::INFTY );
        return carl::abs(Number(intervalA.upper() - this->lower()));
    }
    Number distA = carl::abs(Number(intervalA.upper() - this->lower()));
    Number distB = carl::abs(Number(intervalA.lower() - this->upper()));
    if( distA < distB )
        return distA;
    return distB;
}

template<typename Number>
Interval<Number> Interval<Number>::convexHull(const Interval<Number>& interval) const {
	if(this->isEmpty())
		return interval;

	if(interval.isEmpty())
		return *this;

	BoundType newLowerBound = getStrictestBoundType(this->lowerBoundType(), interval.lowerBoundType());
	BoundType newUpperBound = getStrictestBoundType(this->upperBoundType(), interval.upperBoundType());
	Number newLower = interval.lower() < this->lower() ? interval.lower() : this->lower();
	Number newUpper = interval.upper() > this->upper() ? interval.upper() : this->upper();

    return Interval(newLower, newLowerBound, newUpper, newUpperBound);
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
    if( this->isEmpty() || rhs.isEmpty() ) return emptyInterval();
    BoundType lowerBoundType = BoundType::WEAK;
    BoundType upperBoundType = BoundType::WEAK;
    BoostInterval resultInterval;
    // The following unfortunately copies the content of boost::interval::mul, but we need to get into the case distinction to find the right bound types
    typename BoostIntervalPolicies::rounding rnd;
    const Number& xl = this->lower();
    const Number& xu = this->upper();
    const Number& yl = rhs.lower();
    const Number& yu = rhs.upper();
    const BoundType& xlt = this->lowerBoundType();
    const BoundType& xut = this->upperBoundType();
    const BoundType& ylt = rhs.lowerBoundType();
    const BoundType& yut = rhs.upperBoundType();

    if (xl < carl::constant_zero<Number>().get())
    {
        if (xu > carl::constant_zero<Number>().get())
        {
            if (yl < carl::constant_zero<Number>().get())
            {
                if (yu > carl::constant_zero<Number>().get()) // M * M
                {
                    Number lowerA = rnd.mul_down(xl, yu);
                    Number lowerB = rnd.mul_down(xu, yl);
                    if( lowerA > lowerB )
                    {
                        lowerA = lowerB;
                        lowerBoundType = getStrictestBoundType(xut, ylt);
                    }
                    else
                    {
                        lowerBoundType = getStrictestBoundType(xlt, yut);
                    }
                    Number upperA = rnd.mul_up(xl, yl);
                    Number upperB = rnd.mul_up(xu, yu);
                    if( upperA < upperB )
                    {
                        upperA = upperB;
                        upperBoundType = getStrictestBoundType(xut, yut);
                    }
                    else
                    {
                        upperBoundType = getStrictestBoundType(xlt, ylt);
                    }
                    resultInterval = BoostInterval(lowerA, upperA, true);
                }
                else // M * N
                {
                    lowerBoundType = getStrictestBoundType(xut, ylt);
                    upperBoundType = getStrictestBoundType(xlt, ylt);
                    resultInterval = BoostInterval(rnd.mul_down(xu, yl), rnd.mul_up(xl, yl), true);
                }
            }
            else
            {
                if (yu > carl::constant_zero<Number>().get()) // M * P
                {
                    lowerBoundType = getStrictestBoundType(xlt, yut);
                    upperBoundType = getStrictestBoundType(xut, yut);
                    resultInterval = BoostInterval(rnd.mul_down(xl, yu), rnd.mul_up(xu, yu), true);
                }
                else // M * Z
                {
                    if( ylt != BoundType::INFTY )
                        lowerBoundType = ylt;
                    if( yut != BoundType::INFTY )
                        upperBoundType = yut;
                    resultInterval = BoostInterval(static_cast<Number>(0), static_cast<Number>(0), true);
                }
            }
        }
        else
        {
            if (yl < carl::constant_zero<Number>().get())
            {
                if (yu > carl::constant_zero<Number>().get()) // N * M
                {
                    lowerBoundType = getStrictestBoundType(xlt, yut);
                    upperBoundType = getStrictestBoundType(xlt, ylt);
                    resultInterval = BoostInterval(rnd.mul_down(xl, yu), rnd.mul_up(xl, yl), true);
                }
                else // N * N
                {
                    lowerBoundType = getStrictestBoundType(xut, yut);
                    upperBoundType = getStrictestBoundType(xlt, ylt);
                    resultInterval = BoostInterval(rnd.mul_down(xu, yu), rnd.mul_up(xl, yl), true);
                }
            }
            else
            {
                if (yu > carl::constant_zero<Number>().get()) // N * P
                {
                    lowerBoundType = getStrictestBoundType(xlt, yut);
                    upperBoundType = getStrictestBoundType(xut, ylt);
                    resultInterval = BoostInterval(rnd.mul_down(xl, yu), rnd.mul_up(xu, yl), true);
                }
                else // N * Z
                {
                    lowerBoundType = getStrictestBoundType(xut, yut);
                    upperBoundType = getStrictestBoundType(xut, ylt);
                    resultInterval = BoostInterval(static_cast<Number>(0), static_cast<Number>(0), true);
                }
            }
        }
    }
    else
    {
        if (xu > carl::constant_zero<Number>().get())
        {
            if (yl < carl::constant_zero<Number>().get())
            {
                if (yu > carl::constant_zero<Number>().get()) // P * M
                {
                    lowerBoundType = getStrictestBoundType(xut, ylt);
                    upperBoundType = getStrictestBoundType(xut, yut);
                    resultInterval = BoostInterval(rnd.mul_down(xu, yl), rnd.mul_up(xu, yu), true);
                }
                else // P * N
                {
                    lowerBoundType = getStrictestBoundType(xut, ylt);
                    upperBoundType = getStrictestBoundType(xlt, yut);
                    resultInterval = BoostInterval(rnd.mul_down(xu, yl), rnd.mul_up(xl, yu), true);
                }
            }
            else
            {
                if (yu > carl::constant_zero<Number>().get()) // P * P
                {
                    lowerBoundType = getStrictestBoundType(xlt, ylt);
                    upperBoundType = getStrictestBoundType(xut, yut);
                    resultInterval = BoostInterval(rnd.mul_down(xl, yl), rnd.mul_up(xu, yu), true);
                }
                else // P * Z
                {
                    if( ylt != BoundType::INFTY )
                        lowerBoundType = ylt;
                    if( yut != BoundType::INFTY )
                        upperBoundType = yut;
                    resultInterval = BoostInterval(static_cast<Number>(0), static_cast<Number>(0), true);
                }
            }
        }
        else // Z * ?
        {
            if( xlt != BoundType::INFTY )
                lowerBoundType = xlt;
            if( xut != BoundType::INFTY )
                upperBoundType = xut;
            resultInterval = BoostInterval(static_cast<Number>(0), static_cast<Number>(0), true);
        }
    }
    unsigned zeroBoundInvolved = 2;
    if( (xlt == BoundType::INFTY && (yu > carl::constant_zero<Number>().get() || yut == BoundType::INFTY))
       || (xut == BoundType::INFTY && (yl < carl::constant_zero<Number>().get() || ylt == BoundType::INFTY))
       || (ylt == BoundType::INFTY && (xu > carl::constant_zero<Number>().get() || xut == BoundType::INFTY))
       || (yut == BoundType::INFTY && (xu < carl::constant_zero<Number>().get() || (xl < carl::constant_zero<Number>().get() || xlt == BoundType::INFTY))) )
    {
        lowerBoundType = BoundType::INFTY;
    }
    else if( resultInterval.lower() == carl::constant_zero<Number>().get() )
    {
        if( zeroBoundInvolved == 2 )
            zeroBoundInvolved = (this->contains( carl::constant_zero<Number>().get() ) || rhs.contains( carl::constant_zero<Number>().get() )) ? 1 : 0;
        if( zeroBoundInvolved == 1 )
            lowerBoundType = BoundType::WEAK;
    }
    if( (xlt == BoundType::INFTY && (yu < carl::constant_zero<Number>().get() || (yl < carl::constant_zero<Number>().get() || ylt == BoundType::INFTY)))
       || (xut == BoundType::INFTY && (yl > carl::constant_zero<Number>().get() || (yu > carl::constant_zero<Number>().get() || yut == BoundType::INFTY)))
       || (ylt == BoundType::INFTY && (xu < carl::constant_zero<Number>().get() || (xl < carl::constant_zero<Number>().get() || xlt == BoundType::INFTY)))
       || (yut == BoundType::INFTY && (xl > carl::constant_zero<Number>().get() || (xu > carl::constant_zero<Number>().get() || xut == BoundType::INFTY))) )
    {
        upperBoundType = BoundType::INFTY;
    }
    else if( resultInterval.upper() == carl::constant_zero<Number>().get() )
    {
        if( zeroBoundInvolved == 2 )
            zeroBoundInvolved = (this->contains( carl::constant_zero<Number>().get() ) || rhs.contains( carl::constant_zero<Number>().get() )) ? 1 : 0;
        if( zeroBoundInvolved == 1 )
            upperBoundType = BoundType::WEAK;
    }
    return Interval<Number>(std::move(resultInterval), lowerBoundType, upperBoundType );
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
		assert(!rhs.contains(carl::constant_zero<Number>().get()));
		BoundType lowerBoundType = BoundType::WEAK;
        BoundType upperBoundType = BoundType::WEAK;
		///@todo Correctly determine if bounds are strict or weak.
		if (this->isOpenInterval() || rhs.isOpenInterval()) {
			// just a quick heuristic, by no means complete.
            lowerBoundType = BoundType::STRICT;
            upperBoundType = BoundType::STRICT;
		}
        const Number& xl = mContent.lower();
        const Number& yl = rhs.lower();
        const Number& yu = rhs.upper();
        const BoundType& xlt = mLowerBoundType;
        const BoundType& xut = mUpperBoundType;
        const BoundType& ylt = rhs.lowerBoundType();
        const BoundType& yut = rhs.upperBoundType();
        if( (xlt == BoundType::INFTY && (carl::isPositive(yu) || yut == BoundType::INFTY))
		   || (xut == BoundType::INFTY && (carl::isNegative(yl) || ylt == BoundType::INFTY))
		   || (ylt == BoundType::INFTY && (carl::isPositive(xl) || xut == BoundType::INFTY))
		   || (yut == BoundType::INFTY && (carl::isNegative(xl) || (carl::isNegative(xl) || xlt == BoundType::INFTY))) )
        {
            lowerBoundType = BoundType::INFTY;
        }
        if( (xlt == BoundType::INFTY && (carl::isNegative(yu) || (carl::isNegative(yl) || ylt == BoundType::INFTY)))
		   || (xut == BoundType::INFTY && (carl::isPositive(yl) || (carl::isPositive(yu) || yut == BoundType::INFTY)))
		   || (ylt == BoundType::INFTY && (carl::isNegative(xl) || (carl::isNegative(xl) || xlt == BoundType::INFTY)))
		   || (yut == BoundType::INFTY && (carl::isPositive(xl) || (carl::isPositive(xl) || xut == BoundType::INFTY))) )
        {
            upperBoundType = BoundType::INFTY;
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
            if(this->contains(carl::constant_zero<Number>().get()) && rhs.contains(carl::constant_zero<Number>().get()))
            {
                    a = unboundedInterval();
                    return false;
            }

            Interval<Number> reciprocalA, reciprocalB;
            bool          splitOccured;

            if( rhs.lowerBoundType() != BoundType::INFTY && rhs.lower() == carl::constant_zero<Number>().get() && rhs.upperBoundType() != BoundType::INFTY && rhs.upper() == carl::constant_zero<Number>().get() )    // point interval 0
            {
                splitOccured = false;
                if( this->contains( carl::constant_zero<Number>().get() ))
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
                if( rhs.isInfinite() )
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
        Interval<Number> Interval<Number>::abs() const
        {
            if(this->contains(carl::constant_zero<Number>().get()))
            {
                Number max = mContent.upper() > mContent.lower() ? mContent.upper() : mContent.lower();
                BoundType bt = mContent.upper() > mContent.lower() ? mUpperBoundType : mLowerBoundType;
                return Interval<Number>( carl::constant_zero<Number>().get(), BoundType::WEAK, max, bt );
            }
            else if( mContent.upper() < carl::constant_zero<Number>().get()) // interval is fully negative
            {
                return Interval<Number>(-mContent.upper(), mUpperBoundType, -mContent.lower(), mLowerBoundType);
            }
            // otherwise inteval is already fully positive
            return *this;
        }

        template<typename Number>
        void Interval<Number>::abs_assign()
        {
            *this = this->abs();
        }

template<typename Number>
bool Interval<Number>::reciprocal(Interval<Number>& a, Interval<Number>& b) const {
        if( this->isInfinite() )
        {
            a = emptyInterval();
            return false;
        }
        else if( this->contains( carl::constant_zero<Number>().get() ) && mContent.lower() != carl::constant_zero<Number>().get() && mContent.upper() != carl::constant_zero<Number>().get() )
        {
            if( mLowerBoundType == BoundType::INFTY )
            {
                a = Interval<Number>(carl::constant_zero<Number>().get(), BoundType::INFTY,carl::constant_zero<Number>().get(), BoundType::WEAK );
                b = Interval<Number>(BoostInterval( carl::constant_one<Number>().get() ) /BoostInterval( mContent.upper() ), BoundType::WEAK, BoundType::INFTY );
            }
            else if( mUpperBoundType == BoundType::INFTY )
            {
                a = Interval<Number>(BoostInterval( carl::constant_one<Number>().get() ) /BoostInterval( mContent.lower() ), BoundType::INFTY, BoundType::WEAK );
                b = Interval<Number>(carl::constant_zero<Number>().get(), BoundType::WEAK,carl::constant_zero<Number>().get(), BoundType::INFTY );
            }
            else if( mContent.lower() == carl::constant_zero<Number>().get() && mContent.upper() != carl::constant_zero<Number>().get() )
            {
                a = Interval<Number>(carl::constant_zero<Number>().get(), BoundType::INFTY, carl::constant_zero<Number>().get(), BoundType::INFTY );
                b = Interval<Number>(BoostInterval( carl::constant_one<Number>().get() ) /BoostInterval( mContent.upper() ), BoundType::WEAK, BoundType::INFTY );
            }
            else if( mContent.lower() != carl::constant_zero<Number>().get() && mContent.upper() == carl::constant_zero<Number>().get() )
            {
                a = Interval<Number>(BoostInterval( carl::constant_one<Number>().get() ) /BoostInterval( mContent.lower() ), BoundType::INFTY, BoundType::WEAK );
                b = unboundedInterval(); // todo: really the whole interval here?
            }
            else if( mContent.lower() == carl::constant_zero<Number>().get() && mContent.upper() == carl::constant_zero<Number>().get() )
            {
                a = unboundedInterval();
                return false;
            }
            else
            {
                a = Interval<Number>(BoostInterval( carl::constant_one<Number>().get() ) /BoostInterval( mContent.lower() ), BoundType::INFTY, BoundType::WEAK );
                b = Interval<Number>(BoostInterval( carl::constant_one<Number>().get() ) /BoostInterval( mContent.upper() ), BoundType::WEAK, BoundType::INFTY );
            }
            return true;
        }
        else
        {
            if( mLowerBoundType == BoundType::INFTY && mContent.upper() != carl::constant_zero<Number>().get() )
            {
                a = Interval<Number>(  carl::constant_one<Number>().get() / mContent.upper() , mUpperBoundType,carl::constant_zero<Number>().get(),  BoundType::WEAK );
            }
            else if( mLowerBoundType == BoundType::INFTY && mContent.upper() == carl::constant_zero<Number>().get() )
            {
                a = Interval<Number>(carl::constant_zero<Number>().get(), BoundType::INFTY, carl::constant_zero<Number>().get(), BoundType::WEAK );
            }
            else if( mUpperBoundType == BoundType::INFTY && mContent.lower() != carl::constant_zero<Number>().get() )
            {
                a = Interval<Number>(  carl::constant_zero<Number>().get() , BoundType::WEAK, carl::constant_one<Number>().get()  /  mContent.lower(), mLowerBoundType );
            }
            else if( mUpperBoundType == BoundType::INFTY && mContent.lower() == carl::constant_zero<Number>().get() )
            {
                a = Interval<Number>(carl::constant_zero<Number>().get(), BoundType::WEAK,carl::constant_zero<Number>().get(), BoundType::INFTY );
            }
            else if( mContent.lower() != carl::constant_zero<Number>().get() && mContent.upper() != carl::constant_zero<Number>().get() )
            {
                a = Interval<Number>(BoostInterval( carl::constant_one<Number>().get() ) / mContent, mUpperBoundType, mLowerBoundType );
            }
            else if( mContent.lower() == carl::constant_zero<Number>().get() && mContent.upper() != carl::constant_zero<Number>().get() )
            {
                a = Interval<Number>(BoostInterval( carl::constant_one<Number>().get() ) /BoostInterval( mContent.upper() ), mUpperBoundType, BoundType::INFTY );
            }
            else if( mContent.lower() != carl::constant_zero<Number>().get() && mContent.upper() == carl::constant_zero<Number>().get() )
            {
                a = Interval<Number>(BoostInterval( carl::constant_one<Number>().get() ) /BoostInterval( mContent.lower() ), BoundType::INFTY, mLowerBoundType );
            }

            return false;
        }
	}

template<typename Number>
template<typename Num, EnableIf<std::is_floating_point<Num>>>
Interval<Number> Interval<Number>::root(int deg) const
	{
		assert(this->isConsistent());
        if( deg % 2 == 0 )
        {
            if( mUpperBoundType != BoundType::INFTY &&  mContent.upper() < carl::constant_zero<Number>().get() )
                return Interval<Number>::emptyInterval();
            if( mLowerBoundType == BoundType::INFTY || mContent.lower() < carl::constant_zero<Number>().get() )
            {
                if( mUpperBoundType == BoundType::INFTY )
                {
                    return Interval<Number>(BoostInterval(carl::constant_zero<Number>().get()), BoundType::WEAK, mUpperBoundType);
                }
                else
                {
                    return Interval<Number>(boost::numeric::nth_root(BoostInterval(carl::constant_zero<Number>().get(),mContent.upper()), deg), BoundType::WEAK, mUpperBoundType);
                }
            }
        }
		return Interval<Number>(boost::numeric::nth_root(mContent, deg), mLowerBoundType, mUpperBoundType);
	}

template<typename Number>
template<typename Num, EnableIf<std::is_floating_point<Num>>>
void Interval<Number>::root_assign(unsigned deg)
	{
		*this = this->root(deg);
	}

/*******************************************************************************
 * Boolean operations
 ******************************************************************************/

template<typename Number>
bool Interval<Number>::intersectsWith(const Interval<Number>& rhs) const
{
    return set_intersect(*this, rhs);
}

template<typename Number>
	Interval<Number> Interval<Number>::intersect(const Interval<Number>& rhs) const
	{
		return set_intersection(*this, rhs);
	}

template<typename Number>
	Interval<Number>& Interval<Number>::intersect_assign(const Interval<Number>& rhs)
	{
		*this = set_intersection(*this, rhs);
        return *this;
	}

	template<typename Number>
	bool Interval<Number>::unite(const Interval<Number>& rhs, Interval<Number>& resultA, Interval<Number>& resultB) const
	{
		return set_union(*this, rhs, resultA, resultB);
	}

	template<typename Number>
	bool Interval<Number>::difference(const Interval<Number>& rhs, Interval<Number>& resultA, Interval<Number>& resultB) const
	{
		return set_difference(*this, rhs, resultA, resultB);
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
                    // Shouldn't happen
                    assert(false);
                    return false;
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

}
