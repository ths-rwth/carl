/* The class which contains the interval arithmetic including trigonometric 
 * functions. The template parameter contains the numbertype used for the 
 * boundaries. It is neccessary to implement the rounding and checking policies 
 * for any non-primitive type such that the desired inclusion property can be 
 * mainained.
 *
 * Requirements for the NumberType:
 * - Operators +,-,*,/ with the expected functionality
 * - Operators +=,-=,*=,/= with the expected functionality
 * - Operators <,>,<=,>=,==,!= with the expected functionality
 * - Operations abs, min, max, log, exp, power, sqrt
 * - Trigonometric functions sin, cos, tan, asin, acos, atan, sinh, cosh, tanh,
 *				asinh, acosh, atanh (these functions are needed for the 
 *				specialization of the rounding modes.
 * - Operator <<
 *
 * @file   Interval.h
 * @author Stefan Schupp <stefan.schupp@cs.rwth-aachen.de>
 *
 * @since	2013-12-13
 * @version 2014-01-07
 */

#pragma once

#include <map>
#include <cassert>
#include <list>
#include <sstream>


#include "BoundType.h"
#include "rounding.h"
#include "checking.h"
#include "../numbers/typetraits.h"
#include <boost/numeric/interval.hpp>
#include <boost/numeric/interval/interval.hpp>

#include "../core/Variable.h"
#include "../util/SFINAE.h"

namespace carl
{	
	template<typename Number>
	struct policies
	{
		typedef carl::rounding<Number> roundingP;
		typedef carl::checking<Number> checkingP;
	};
	
	template<>
	struct policies<double>
	{
		typedef boost::numeric::interval_lib::save_state<boost::numeric::interval_lib::rounded_transc_opp<double> > roundingP;
		typedef boost::numeric::interval_lib::checking_no_nan<double, boost::numeric::interval_lib::checking_no_nan<double> > checkingP;
	};
	
	// TODO: Create struct specialization for all types which are already covered by the standard boost interval policies.
	
    template<typename Number>
    class Interval : public policies<Number>
    {
	public:
        /***********************************************************************
         * Typedefs
         **********************************************************************/
		//typedef typename policies<Number>::checking checking;
		//typedef typename policies<Number>::rounding rounding;
		
		typedef boost::numeric::interval< Number, boost::numeric::interval_lib::policies< typename policies<Number>::roundingP, typename policies<Number>::checkingP > > BoostInterval;
		typedef std::map<Variable, Interval<Number> > evalintervalmap;
		
#define BOUNDS_OK( lower, lowerBoundType, upper, upperBoundType )\
(lowerBoundType == BoundType::INFTY || upperBoundType == BoundType::INFTY || lower <= upper)
		
#define IS_EMPTY(lower, lowerBoundType, upper, upperBoundType )\
(lowerBoundType == BoundType::STRICT && upperBoundType == BoundType::STRICT && lower == upper)
		
#define IS_UNBOUNDED(lower, lowerBoundType, upper, upperBoundType )\
(lowerBoundType == BoundType::INFTY && upperBoundType == BoundType::INFTY)
		
	protected:
		/***********************************************************************
         * Members
         **********************************************************************/
		
        BoostInterval mContent;
        BoundType mLowerBoundType;
        BoundType mUpperBoundType;
        
	public:
        /***********************************************************************
         * Constructors & Destructor
         **********************************************************************/
		
		Interval() :
			mContent(Number(0)),
			mLowerBoundType(BoundType::STRICT),
			mUpperBoundType(BoundType::STRICT)
		{}
		
		Interval(const Number& n) :
			mContent(n, n),
			mLowerBoundType(BoundType::WEAK),
			mUpperBoundType(BoundType::WEAK)
		{}
		
		Interval(const Number& lower, const Number& upper)
		{
			if (BOUNDS_OK(lower, BoundType::WEAK, upper, BoundType::WEAK)) {
				mContent = BoostInterval(lower,upper);
				mLowerBoundType = BoundType::WEAK;
				mUpperBoundType = BoundType::WEAK;
			}
			else{
				mContent = BoostInterval(Number(0));
				mLowerBoundType = BoundType::STRICT;
				mUpperBoundType = BoundType::STRICT;
			}
		}
		
		Interval(const BoostInterval& content, BoundType lowerBoundType, BoundType upperBoundType)
		{
			if (BOUNDS_OK(content.lower(), lowerBoundType, content.upper(), upperBoundType)) {
				if(IS_EMPTY(content.lower(), lowerBoundType, content.upper(), upperBoundType))
				{
					mContent = BoostInterval(Number(0));
					mLowerBoundType = BoundType::STRICT;
					mUpperBoundType = BoundType::STRICT;
				}
				if(IS_UNBOUNDED(content.lower(), lowerBoundType, content.upper(), upperBoundType))
				{
					mContent = BoostInterval(Number(0));
					mLowerBoundType = BoundType::INFTY;
					mUpperBoundType = BoundType::INFTY;
				}
				else if (lowerBoundType == BoundType::INFTY || upperBoundType == BoundType::INFTY)
				{
					mContent = BoostInterval(lowerBoundType == BoundType::INFTY ? content.upper() : content.lower());
					mLowerBoundType = lowerBoundType;
					mUpperBoundType = upperBoundType;
				}
				else
				{
					mContent = content;
					mLowerBoundType = lowerBoundType;
					mUpperBoundType = upperBoundType;
				}
			}
			else{
				mContent = BoostInterval(Number(0));
				mLowerBoundType = BoundType::STRICT;
				mUpperBoundType = BoundType::STRICT;
			}
		}
		
		Interval(const Number& lower, BoundType lowerBoundType, const Number& upper, BoundType upperBoundType)
		{
			if (BOUNDS_OK(lower, lowerBoundType, upper, upperBoundType)) {
				if(IS_EMPTY(lower, lowerBoundType, upper, upperBoundType))
				{
					mContent = BoostInterval(Number(0));
					mLowerBoundType = BoundType::STRICT;
					mUpperBoundType = BoundType::STRICT;
				}
				else if(IS_UNBOUNDED(lower, lowerBoundType, upper, upperBoundType))
				{
					mContent = BoostInterval(Number(0));
					mLowerBoundType = BoundType::INFTY;
					mUpperBoundType = BoundType::INFTY;
				}
				else if (lowerBoundType == BoundType::INFTY || upperBoundType == BoundType::INFTY)
				{
					mContent = BoostInterval(lowerBoundType == BoundType::INFTY ? upper : lower);
					mLowerBoundType = lowerBoundType;
					mUpperBoundType = upperBoundType;
				}
				else
				{
					mContent = BoostInterval(lower,upper);
					mLowerBoundType = lowerBoundType;
					mUpperBoundType = upperBoundType;
				}
			}
			else{
				mContent = BoostInterval(Number(0));
				mLowerBoundType = BoundType::STRICT;
				mUpperBoundType = BoundType::STRICT;
			}
		}
		
		Interval(const Interval<Number>& o) :
			mContent(BoostInterval(o.mContent)),
			mLowerBoundType(o.mLowerBoundType),
			mUpperBoundType(o.mUpperBoundType)
		{}
		
		template<typename N = Number, DisableIf<std::is_same<N, double>> = dummy >
		Interval(const double& n):
			mContent(carl::Interval<Number>::BoostInterval(n,n)),
			mLowerBoundType(BoundType::WEAK),
			mUpperBoundType(BoundType::WEAK)
		{}
		
		template<typename N = Number, DisableIf<std::is_same<N, double>> = dummy>
		Interval(double lower, double upper)
		{
			if (BOUNDS_OK(lower, BoundType::WEAK, upper, BoundType::WEAK)) {
				mContent = BoostInterval(lower,upper);
				mLowerBoundType = BoundType::WEAK;
				mUpperBoundType = BoundType::WEAK;
			}
			else{
				mContent = BoostInterval(Number(0));
				mLowerBoundType = BoundType::STRICT;
				mUpperBoundType = BoundType::STRICT;
			}
		}
		
		template<typename N = Number, DisableIf<std::is_same<N, double>> = dummy>
		Interval(double lower, BoundType lowerBoundType, double upper, BoundType upperBoundType)
		{
			if (BOUNDS_OK(lower, lowerBoundType, upper, upperBoundType)) {
				if(IS_EMPTY(lower, lowerBoundType, upper, upperBoundType))
				{
					mContent = BoostInterval(Number(0));
					mLowerBoundType = BoundType::STRICT;
					mUpperBoundType = BoundType::STRICT;
				}
				else if(IS_UNBOUNDED(lower, lowerBoundType, upper, upperBoundType))
				{
					mContent = BoostInterval(Number(0));
					mLowerBoundType = BoundType::INFTY;
					mUpperBoundType = BoundType::INFTY;
				}
				else if (lowerBoundType == BoundType::INFTY || upperBoundType == BoundType::INFTY)
				{
					mContent = BoostInterval(lowerBoundType == BoundType::INFTY ? upper : lower);
					mLowerBoundType = lowerBoundType;
					mUpperBoundType = upperBoundType;
				}
				else
				{
					mContent = BoostInterval(lower,upper);
					mLowerBoundType = lowerBoundType;
					mUpperBoundType = upperBoundType;
				}
			}
			else{
				mContent = BoostInterval(Number(0));
				mLowerBoundType = BoundType::STRICT;
				mUpperBoundType = BoundType::STRICT;
			}
		}
		
		template<typename N = Number, DisableIf<std::is_same<N, int>> = dummy >
		Interval(const int& n):
		mContent(carl::Interval<Number>::BoostInterval(n, n)),
		mLowerBoundType(BoundType::WEAK),
		mUpperBoundType(BoundType::WEAK)
		{}
		
		template<typename N = Number, DisableIf<std::is_same<N, int>> = dummy>
		Interval(int lower, int upper)
		{
			if (BOUNDS_OK(lower, BoundType::WEAK, upper, BoundType::WEAK)) {
				mContent = BoostInterval(lower,upper);
				mLowerBoundType = BoundType::WEAK;
				mUpperBoundType = BoundType::WEAK;
			}
			else{
				mContent = BoostInterval(Number(0));
				mLowerBoundType = BoundType::STRICT;
				mUpperBoundType = BoundType::STRICT;
			}
		}
		
		template<typename N = Number, DisableIf<std::is_same<N, int>> = dummy>
		Interval(int lower, BoundType lowerBoundType, int upper, BoundType upperBoundType)
		{
			if (BOUNDS_OK(lower, lowerBoundType, upper, upperBoundType)) {
				if(IS_EMPTY(lower, lowerBoundType, upper, upperBoundType))
				{
					mContent = BoostInterval(Number(0));
					mLowerBoundType = BoundType::STRICT;
					mUpperBoundType = BoundType::STRICT;
				}
				else if(IS_UNBOUNDED(lower, lowerBoundType, upper, upperBoundType))
				{
					mContent = BoostInterval(Number(0));
					mLowerBoundType = BoundType::INFTY;
					mUpperBoundType = BoundType::INFTY;
				}
				else if (lowerBoundType == BoundType::INFTY || upperBoundType == BoundType::INFTY)
				{
					mContent = BoostInterval(lowerBoundType == BoundType::INFTY ? upper : lower);
					mLowerBoundType = lowerBoundType;
					mUpperBoundType = upperBoundType;
				}
				else
				{
					mContent = BoostInterval(lower,upper);
					mLowerBoundType = lowerBoundType;
					mUpperBoundType = upperBoundType;
				}
			}
			else{
				mContent = BoostInterval(Number(0));
				mLowerBoundType = BoundType::STRICT;
				mUpperBoundType = BoundType::STRICT;
			}
		}
		
		template<typename Other>
		Interval(const Number& lower, const Other& upper)
		{
			if (BOUNDS_OK(lower, BoundType::WEAK, upper, BoundType::WEAK)) {
				mContent = BoostInterval(lower,upper);
				mLowerBoundType = BoundType::WEAK;
				mUpperBoundType = BoundType::WEAK;
			}
			else{
				mContent = BoostInterval(Number(0), Number(0));
				mLowerBoundType = BoundType::STRICT;
				mUpperBoundType = BoundType::STRICT;
			}
		}
		
		template<typename Other>
		Interval(const Other& lower, const Number& upper)
		{
			if (BOUNDS_OK(lower, BoundType::WEAK, upper, BoundType::WEAK)) {
				mContent = BoostInterval(lower,upper);
				mLowerBoundType = BoundType::WEAK;
				mUpperBoundType = BoundType::WEAK;
			}
			else{
				mContent = BoostInterval(Number(0));
				mLowerBoundType = BoundType::STRICT;
				mUpperBoundType = BoundType::STRICT;
			}
		}
		
		template<typename Other>
		Interval(const Number& lower, BoundType lowerBoundType, const Other& upper, BoundType upperBoundType)
		{
			if (BOUNDS_OK(lower, lowerBoundType, upper, upperBoundType)) {
				if(IS_EMPTY(lower, lowerBoundType, upper, upperBoundType))
				{
					mContent = BoostInterval(Number(0));
					mLowerBoundType = BoundType::STRICT;
					mUpperBoundType = BoundType::STRICT;
				}
				else if(IS_UNBOUNDED(lower, lowerBoundType, upper, upperBoundType))
				{
					mContent = BoostInterval(Number(0));
					mLowerBoundType = BoundType::INFTY;
					mUpperBoundType = BoundType::INFTY;
				}
				else if (lowerBoundType == BoundType::INFTY || upperBoundType == BoundType::INFTY)
				{
					mContent = BoostInterval(lowerBoundType == BoundType::INFTY ? upper : lower);
					mLowerBoundType = lowerBoundType;
					mUpperBoundType = upperBoundType;
				}
				else
				{
					mContent = BoostInterval(lower,upper);
					mLowerBoundType = lowerBoundType;
					mUpperBoundType = upperBoundType;
				}
			}
			else{
				mContent = BoostInterval(Number(0));
				mLowerBoundType = BoundType::STRICT;
				mUpperBoundType = BoundType::STRICT;
			}
		}
		
		template<typename Other>
		Interval(const Other& lower, BoundType lowerBoundType, const Number& upper, BoundType upperBoundType)
		{
			if (BOUNDS_OK(lower, lowerBoundType, upper, upperBoundType)) {
				if(IS_EMPTY(lower, lowerBoundType, upper, upperBoundType))
				{
					mContent = BoostInterval(Number(0));
					mLowerBoundType = BoundType::STRICT;
					mUpperBoundType = BoundType::STRICT;
				}
				else if(IS_UNBOUNDED(lower, lowerBoundType, upper, upperBoundType))
				{
					mContent = BoostInterval(Number(0));
					mLowerBoundType = BoundType::INFTY;
					mUpperBoundType = BoundType::INFTY;
				}
				else if (lowerBoundType == BoundType::INFTY || upperBoundType == BoundType::INFTY)
				{
					mContent = BoostInterval(lowerBoundType == BoundType::INFTY ? upper : lower);
					mLowerBoundType = lowerBoundType;
					mUpperBoundType = upperBoundType;
				}
				else
				{
					mContent = BoostInterval(lower,upper);
					mLowerBoundType = lowerBoundType;
					mUpperBoundType = upperBoundType;
				}
			}
			else{
				mContent = BoostInterval(Number(0));
				mLowerBoundType = BoundType::STRICT;
				mUpperBoundType = BoundType::STRICT;
			}
		}
		
		template<typename Rational>
		Interval(Rational n);
		template<typename Rational>
		Interval(Rational lower, Rational upper);
		template<typename Rational>
		Interval(Rational lower, BoundType lowerBoundType, Rational upper, BoundType upperBoundType);
		
		static Interval<Number> unboundedInterval()
		{
			return Interval<Number>(Number(0), BoundType::INFTY, Number(0), BoundType::INFTY);
		}
		
		static Interval<Number> emptyInterval()
		{
			return Interval<Number>(Number(0), BoundType::STRICT, Number(0), BoundType::STRICT);
		}
		
		~Interval()
		{}
        
        /***********************************************************************
         * Getter & Setter
         **********************************************************************/
		
		/**
		 * The getter for the lower boundary of the interval.
		 * @return lower interval boundary
		 */
		const Number& lower() const
		{
			return mContent.lower();
		}
        
		/**
		 * The getter for the upper boundary of the interval.
		 * @return upper interval boundary
		 */
		const Number& upper() const
		{
			return mContent.upper();
		}
		
		/**
		 * Returns a reference to the included boost interval.
		 * @return boost interval
		 */
		BoostInterval& rContent()
		{
			return mContent;
		}
		
		/**
		 * Returns a copy of the included boost interval.
		 * @return boost interval
		 */
		BoostInterval content() const
		{
			return mContent;
		}
		
		/**
		 * The getter for the lower bound type of the interval.
		 * @return lower bound type
		 */
		BoundType lowerBoundType() const
		{
			return mLowerBoundType;
		}
		
		/**
		 * The getter for the upper bound type of the interval.
		 * @return upper bound type
		 */
		BoundType upperBoundType() const
		{
			return mUpperBoundType;
		}
		
		/**
		 * The setter for the lower boundary of the interval.
		 * @param lower boundary
		 */
		void setLower(const Number& n)
		{
			mContent = BoostInterval(n, mContent.upper());
		}
        
		/**
		 * The setter for the upper boundary of the interval.
		 * @param upper boundary
		 */
		void setUpper(const Number& n)
		{
			mContent = BoostINterval(mContent.lower(), n);
		}
		
		/**
		 * The setter for the lower bound type of the interval.
		 * @param lower bound type
		 */
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
		
		/**
		 * The setter for the upper bound type of the interval.
		 * @param upper bound type
		 */
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
		 * The assignment operator.
		 * @param source interval
		 * @return
		 */
		Interval<Number>& operator =(const Interval<Number>& rhs)
		{
			mContent = rhs.content();
			mLowerBoundType = rhs.lowerBoundType();
			mUpperBoundType = rhs.upperBoundType();
			return *this;
		}
		
		/***********************************************************************
		 * Transformations and advanced getters/setters
		 **********************************************************************/
		
		/**
		 * Advanced setter to modify both boundaries at once.
		 * @param lower boundary
		 * @param upper boundary
		 */
		void set(const Number& lower, const Number& upper)
		{
			mContent = BoostInterval(lower, upper);
		}
		
		bool isUnbounded() const
		{
			return mLowerBoundType == BoundType::INFTY && mUpperBoundType == BoundType::INFTY;
		}
		
		bool isEmpty() const
		{
			return mContent.lower() == mContent.upper() && mLowerBoundType == BoundType::STRICT && mUpperBoundType == BoundType::STRICT;
		}
		
		/**
		 * Returns the diameter of the interval.
		 * @return diameter
		 */
		Number diameter() const;
		void diameter_assign();
		
		/**
		 * Returns the ratio of the diameters of the given intervals.
		 * @param interval
		 * @return ratio
		 */
		Number diameterRatio(const Interval<Number>& rhs) const;
		void diameterRatio_assign(const Interval<Number>& rhs);
		
		/**
		 * Returns the magnitude of the interval.
		 * @return magnitude
		 */
		Number magnitude() const;
		void magnitude_assign();
		
		/**
		 * Returns the center point of the interval.
		 * @return center
		 */
		Number center() const;
		void center_assign();
		
		/**
		 * Checks if the interval contains the given value.
		 * @param value
		 * @return
		 */
		bool contains(const Number& val) const;
		
		/**
		 * Checks if the interval contains the given interval.
		 * @param interval
		 * @return
		 */
		bool contains(const Interval<Number>& rhs) const;
		
		/**
		 * Checks if the given interval is a subset of the calling interval.
		 * @param interval
		 * @return
		 */
		bool subset(const Interval<Number>& rhs) const;
		
		/**
		 * Checks if the given interval is a proper subset of the calling interval.
		 * @param interval
		 * @return
		 */
		bool proper_subset(const Interval<Number>& rhs) const;
		
		/**
		 * Bloats the interval by the given value.
		 * @param width
		 */
		void bloat_by(const Number& width);
		
		/**
		 * Bloats the interval times the factor (multiplies the overall width).
		 * @param factor
		 */
		void bloat_times(const Number& factor);
		
		/**
		 * Shrinks the interval by the given value.
		 * @param width
		 */
		void shrink_by(const Number& width);
		
		/**
		 * Shrinks the interval by a multiple of its width.
		 * @param factor
		 */
		void shrink_times(const Number& factor);
		
		/**
		 * Splits the interval into 2 equally sized parts (strict-weak-cut)
		 * @return pair<interval, interval>
		 */
		std::pair<Interval<Number>, Interval<Number>> split() const;
		
		/**
		 * Splits the interval into n equally sized parts (strict-weak-cut)
		 * @return list<interval>
		 */
		std::list<Interval<Number>> split(unsigned n) const;
		
		/**
		 * Creates a string representation of the interval.
		 * @return string
		 */
		std::string toString() const;
		
		friend inline std::ostream& operator <<(std::ostream& str, const Interval<Number>& i)
		{
			str << "lower: " << i.mContent.lower() << ", upper: " << i.mContent.upper() << " ";
 			switch (i.mLowerBoundType) {
				case BoundType::INFTY:
					str << "]-INF, ";
					break;
				case BoundType::STRICT:
					str << "]" << i.mContent.lower() << ", ";
					break;
				case BoundType::WEAK:
					str << "[" << i.mContent.lower() << ", ";
			}
			switch (i.mUpperBoundType) {
				case BoundType::INFTY:
					str << "INF[";
					break;
				case BoundType::STRICT:
					str << i.mContent.upper() << "[";
					break;
				case BoundType::WEAK:
					str << i.mContent.upper() << "]";
			}
			return str;
		}
		
        /***********************************************************************
         * Arithmetic functions
         **********************************************************************/
		
		/**
		 * Adds two intervals according to natural interval arithmetic.
		 * @param interval
		 * @return result
		 */
		Interval<Number> add(const Interval<Number>& rhs) const;
		void add_assign(const Interval<Number>& rhs);
		
		/**
		 * Subtracts two intervals according to natural interval arithmetic.
		 * @param interval
		 * @return result
		 */
        Interval<Number> sub(const Interval<Number>& rhs) const;
		void sub_assign(const Interval<Number>& rhs);
		
		/**
		 * Multiplies two intervals according to natural interval arithmetic.
		 * @param interval
		 * @return result
		 */
		Interval<Number> mul(const Interval<Number>& rhs) const;
		void mul_assign(const Interval<Number>& rhs);
		
		/**
		 * Divides two intervals according to natural interval arithmetic.
		 * @param interval
		 * @return result
		 */
		Interval<Number> div(const Interval<Number>& rhs) const;
		void div_assign(const Interval<Number>& rhs);
		
		/**
		 * Implements extended interval division with intervals containting zero.
		 * @param interval
		 * @param result a
		 * @param result b
		 * @return split occured
		 */
		bool div_ext(const Interval<Number>& rhs, Interval<Number>& a, Interval<Number>& b) const;
		
		/**
		 * Calculates the additive inverse of an interval with respect to natural interval arithmetic.
		 * @return
		 */
		Interval<Number> inverse() const;
		void inverse_assign();
		
		/**
		 * Calculates the multiplicative inverse of an interval with respect to natural interval arithmetic.
		 * @param result a
		 * @param result b
		 * @return split occured
		 */
		bool reciprocal(Interval<Number>& a, Interval<Number>& b) const;
		
		/**
		 * Calculates the power of the interval with respect to natural interval arithmetic.
		 * @param exponent
 		 * @return result
		 */
		Interval<Number> power(unsigned exp) const;
		void power_assign(unsigned exp);
		
		/**
		 * Calculates the squareroot of the interval with respect to natural interval arithmetic.
		 * @return result
		 */
		Interval<Number> sqrt() const;
		void sqrt_assign();
		
		/**
		 * Calculates the nth root of the interval with respect to natural interval arithmetic.
		 * @param degree
		 * @return result
		 */
		Interval<Number> root(unsigned deg) const;
		void root_assign(unsigned deg);
		
		/**
		 * Calculates the logarithm of the interval with respect to natural interval arithmetic.
		 * @return result
		 */
		Interval<Number> log() const;
		void log_assign();
		
        /***********************************************************************
         * Trigonometric functions
         **********************************************************************/
        
		/**
		 * Calculates the sine of the given interval with respect to natural interval arithmetic.
		 * @return result
		 */
		Interval<Number> sin() const;
		void sin_assign();
		
		/**
		 * Calculates the cosine of the given interval with respect to natural interval arithmetic.
		 * @return result
		 */
		Interval<Number> cos() const;
		void cos_assign();
		
		/**
		 * Calculates the tangens of the given interval with respect to natural interval arithmetic.
		 * @return result
		 */
		Interval<Number> tan() const;
		void tan_assign();
		
		/**
		 * Calculates the arcussine of the given interval with respect to natural interval arithmetic.
		 * @return result
		 */
		Interval<Number> asin() const;
		void asin_assign();
		
		/**
		 * Calculates the arcuscosine of the given interval with respect to natural interval arithmetic.
		 * @return result
		 */
		Interval<Number> acos() const;
		void acos_assign();
		
		/**
		 * Calculates the arcustangens of the given interval with respect to natural interval arithmetic.
		 * @return result
		 */
		Interval<Number> atan() const;
		void atan_assign();
		
		/**
		 * Calculates the sine hyperbolicus of the given interval with respect to natural interval arithmetic.
		 * @return result
		 */
		Interval<Number> sinh() const;
		void sinh_assign();
		
		/**
		 * Calculates the cosine hyperbolicus of the given interval with respect to natural interval arithmetic.
		 * @return result
		 */
		Interval<Number> cosh() const;
		void cosh_assign();
		
		/**
		 * Calculates the tangens hyperbolicus of the given interval with respect to natural interval arithmetic.
		 * @return result
		 */
		Interval<Number> tanh() const;
		void tanh_assign();
		
		/**
		 * Calculates the arcussine hyperbolicus of the given interval with respect to natural interval arithmetic.
		 * @return result
		 */
		Interval<Number> asinh() const;
		void asinh_assign();
		
		/**
		 * Calculates the arcuscosine hyperbolicus of the given interval with respect to natural interval arithmetic.
		 * @return result
		 */
		Interval<Number> acosh() const;
		void acosh_assign();
		
		/**
		 * Calculates the arcustangens hyperbolicus of the given interval with respect to natural interval arithmetic.
		 * @return result
		 */
		Interval<Number> atanh() const;
		void atanh_assign();
		
		/***********************************************************************
         * Boolean Operations
         **********************************************************************/
		
		Interval<Number> intersect(const Interval<Number>& rhs) const;
		Interval<Number>& intersect_assign(const Interval<Number>& rhs);
		
		/***********************************************************************
         * Comparison functions
         **********************************************************************/
		
		Interval<Number> max(const Interval<Number>& rhs) const;
		void max_assign(const Interval<Number>& rhs);
		Interval<Number> min(const Interval<Number>& rhs) const;
		void min_assign(const Interval<Number>& rhs)
    };
	
	/***************************************************************************
	 * Overloaded arithmetics operators
	 **************************************************************************/
	
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
	
	/***************************************************************************
	 * Comparison operators
	 **************************************************************************/
	
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
	
}

#include "Interval.tpp"