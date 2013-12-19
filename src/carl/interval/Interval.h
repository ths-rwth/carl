/* The class which contains the interval arithmetic including trigonometric 
 * functions. The template parameter contains the numbertype used for the 
 * boundaries. It is neccessary to implement the rounding and checking policies 
 * for any non-primitive type such that the desired inclusion property can be 
 * mainained.
 *
 * @file   Interval.h
 * @author Stefan Schupp <stefan.schupp@cs.rwth-aachen.de>
 *
 * @since	2013-12-13
 * @version 2013-12-19
 */

#pragma once

#include "BoundType.h"
#include "rounding.h"
#include "checking.h"
#include "../numbers/typetraits.h"

namespace carl
{
    template<Number>
    class Interval
    {
        /**
         * Typedefs
         */
		if(is_primitive<Number>)
		{
			typedef boost::numeric::interval_lib::save_state<boost::numeric::interval_lib::rounded_arith_opp<Number> > Rounding;
			
			typedef boost::numeric::interval_lib::checking_no_nan<Number, boost::numeric::interval_lib::checking_no_nan<Number> > Checking;
			
			typedef boost::numeric::interval< Number, boost::numeric::interval_lib::policies< Rounding, Checking > > BoostInterval;
		}
		else
		{
			typedef boost::numeric::interval< Number, boost::numeric::interval_lib::policies< carl::rounding<Number>, carl::checking<Number> > > BoostInterval;
		}
		
        typedef std::map<Variable, Interval<FloatImplementation>> evalintervalmap;
        /**
         * Members
         */
        BoostInterval mContent;
        BoundType mLowerBoundType;
        BoundType mUpperBoundType;
        
        /**
         * Constructors & Destructor
         */
		Interval::Interval() : mContent(Number(0)), mLowerBoundType(BoundType::STRICT), mUpperBoundType(BoundType::STRICT){}
		
		Interval::Interval(const Number& n) : mContent(n), mLowerBoundType(BoundType::WEAK), mUpperBoundType(BoundType::WEAK){}
		
		~Interval();
        
        /**
         * Getter & Setter
         */
		
		Number& lower() const
		{
			return mContent.lower();
		}
        
		Number& upper() const
		{
			return mContent.upper;
		}
		
		BoundType lowerBoundType() const
		{
			return mLower
		}
		
		BoundType upperBoundType() const
		{
			return mUpper;
		}
		
		void setLower(const Number& n)
		{
			mContent = BoostInterval(n, mContent.upper());
		}
        
		void setUpper(const Number& n)
		{
			mContent = BoostINterval(mContent.lower(), n);
		}
		
		void setLowerBoundType(BoundType b)
		{
			if(b == BoundType::INFTY)
			{
				mLowerBoundType = b;
				mContent = mUpperBoundType == BoundType::INFTY ? BoostInterval(Number(0)) : BoostInterval(mContent.lower());
			}
			else
			{
				mLowerBoundType = b;
			}
		}
		
		void setUpperBoundType(BoundType b)
		{
			if(b == BoundType::INFTY)
			{
				mUpperBoundType = b;
				mContent = mLowerBoundType == BoundType::INFTY ? BoostInterval(Number(0)) : BoostInterval(mContent.lower());
			}
			else
			{
				mUpperBoundType = b;
			}
		}
		
        /**
         * Arithmetic functions
         */
        
        /**
         * Trigonometric functions
         */
        
    };
	
	/**
	 * Overloaded arithmetics operators
	 */
	
	template<typename Number>
	inline const Interval<Number> operator +(const Interval<Number>& lhs, const Interval<Number>& rhs);
	template<typename Number>
	inline const Interval<Number> operator +(const Number& lhs, const Interval<Number>& rhs);
	template<typename Number>
	inline const Interval<Number> operator +(const Interval<Number>& lhs, const Number& rhs);
	template<typename Number>
	inline const Interval<Number> operator -(const Interval<Number>& lhs, const Interval<Number>& rhs);
	template<typename Number>
	inline const Interval<Number> operator -(const Number& lhs, const Interval<Number>& rhs);
	template<typename Number>
	inline const Interval<Number> operator -(const Interval<Number>& lhs, const Number& rhs);
	template<typename Number>
	inline const Interval<Number> operator *(const Interval<Number>& lhs, const Interval<Number>& rhs);
	template<typename Number>
	inline const Interval<Number> operator *(const Number& lhs, const Interval<Number>& rhs);
	template<typename Number>
	inline const Interval<Number> operator *(const Interval<Number>& lhs, const Number& rhs);
	template<typename Number>
	inline const Interval<Number> operator /(const Interval<Number>& lhs, const Interval<Number>& rhs);
	template<typename Number>
	inline const Interval<Number> operator /(const Number& lhs, const Interval<Number>& rhs);
	template<typename Number>
	inline const Interval<Number> operator /(const Interval<Number>& lhs, const Number& rhs);
	
	/**
	 * Comparison operators
	 */
	
	template<typename Number>
	inline bool operator ==(const Interval<Number>& lhs, const Interval<Number>& rhs);
	template<typename Number>
	inline bool operator !=(const Interval<Number>& lhs, const Interval<Number>& rhs);
	template<typename Number>
	inline bool operator <=(const Interval<Number>& lhs, const Interval<Number>& rhs);
	template<typename Number>
	inline bool operator >=(const Interval<Number>& lhs, const Interval<Number>& rhs);
	template<typename Number>
	inline bool operator <(const Interval<Number>& lhs, const Interval<Number>& rhs);
	template<typename Number>
	inline bool operator >(const Interval<Number>& lhs, const Interval<Number>& rhs);
	
	template<typename Number>
	
}