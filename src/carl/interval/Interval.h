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
 * @version 2013-12-15
 */

#pragma once

#include "BoundType.h"
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
        BoostInterval mInterval;
        BoundType mLower;
        BoundType mUpper;
        
        /**
         * Constructors & Destructor
         */
		Interval::Interval() : mInterval(Number(0)), mLower(BoundType::STRICT), mUpper(BoundType::STRICT){}
		
		Interval::Interval(const Number& n) : mInterval(n), mLower(BoundType::WEAK), mUpper(BoundType::WEAK){}
		
		~Interval();
        
        /**
         * Getter & Setter
         */
		
		Number& lower () const
		{
			return mInterval.content();
		}
        
		Number& upper () const
		{
			return mUpper;
		}
		
		BoundType lowerType() const
		{
			return mLower
		}
		
		BoundType upperType() const
		{
			return mUpper;
		}
		
        /**
         * Arithmetic functions
         */
        
        /**
         * Overloaded operators
         */
        
        /**
         * Trigonometric functions
         */
        
        /**
         * Comparison operators
         */
    };   
}