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
			mContent(n),
			mLowerBoundType(BoundType::WEAK),
			mUpperBoundType(BoundType::WEAK)
		{}
		
		Interval(const Number& lower, const Number& upper) :
			mContent(lower, upper),
			mLowerBoundType(BoundType::WEAK),
			mUpperBoundType(BoundType::WEAK)
		{}
		
		Interval(const BoostInterval& content, BoundType lowerBoundType, BoundType upperBoundType) : mContent(content),
			mLowerBoundType(lowerBoundType),
			mUpperBoundType(upperBoundType)
		{}
		
		Interval(const Number& lower, BoundType lowerBoundType, const Number& upper, BoundType upperBoundType) :
		mContent(lower, upper),
		mLowerBoundType(lowerBoundType),
		mUpperBoundType(upperBoundType)
		{}
		
		Interval(const Interval<Number>& o) :
			mContent(BoostInterval(o.mContent)),
			mLowerBoundType(o.mLowerBoundType),
			mUpperBoundType(o.mUpperBoundType)
		{}
		
		template<typename N = Number, DisableIf<std::is_same<N, double>> = dummy >
		Interval(const double& n):
			mContent(carl::Interval<Number>::BoostInterval(n)),
			mLowerBoundType(BoundType::WEAK),
			mUpperBoundType(BoundType::WEAK)
		{}
		
		template<typename N = Number, DisableIf<std::is_same<N, double>> = dummy>
		Interval(double lower, double upper):
			mContent(Interval<Number>::BoostInterval(lower, upper)),
			mLowerBoundType(BoundType::WEAK),
			mUpperBoundType(BoundType::WEAK)
		{}
		
		template<typename N = Number, DisableIf<std::is_same<N, double>> = dummy>
		Interval(double lower, BoundType lowerBoundType, double upper, BoundType upperBoundType):
			mContent(Interval<Number>::BoostInterval(lower, upper)),
			mLowerBoundType(lowerBoundType),
			mUpperBoundType(upperBoundType)
		{}
		
		template<typename Rational>
		Interval(Rational n);
		template<typename Rational>
		Interval(Rational lower, Rational upper);
		template<typename Rational>
		Interval(Rational lower, BoundType lowerBoundType, Rational upper, BoundType upperBoundType);
		
		Interval<Number> unboundedInterval()
		{
			return Interval<Number>(Number(0), BoundType::INFTY, Number(0), BoundType::INFTY);
		}
		
		Interval<Number> emptyInterval()
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
	
	/*******************************************************************************
	 * Other operators
	 ******************************************************************************/
	
	template<typename Number>
	inline std::ostream& operator <<(std::ostream& str, const Interval<Number>& i);
	
}

#include "Interval.tpp"