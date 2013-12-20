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
 * @version 2013-12-20
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

namespace carl
{
	template<typename Number, bool _isNative>
	struct policies
	{
		typedef carl::rounding<Number> rounding;
		typedef carl::checking<Number> checking;
	};
	
	template<typename Number>
	struct policies<Number, true>
	{
		typedef boost::numeric::interval_lib::save_state<boost::numeric::interval_lib::rounded_arith_opp<Number> > rounding;
		typedef boost::numeric::interval_lib::checking_no_nan<Number, boost::numeric::interval_lib::checking_no_nan<Number> > checking;
	};
	
    template<typename Number, bool _isNative = is_primitive<Number>::value>
    class Interval : public policies<Number, _isNative>
    {
	public:
        /***********************************************************************
         * Typedefs
         **********************************************************************/
		typedef typename policies<Number, _isNative>::checking checking;
		typedef typename policies<Number, _isNative>::rounding rounding;
		
		typedef boost::numeric::interval< Number, boost::numeric::interval_lib::policies< rounding, checking > > BoostInterval;
		typedef std::map<Variable, Interval<Number>> evalintervalmap;
		
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
		
		Interval<Number>::Interval() :
			mContent(Number(0)),
			mLowerBoundType(BoundType::STRICT),
			mUpperBoundType(BoundType::STRICT)
		{}
		
		Interval<Number>::Interval(const Number& n) :
			mContent(n),
			mLowerBoundType(BoundType::WEAK),
			mUpperBoundType(BoundType::WEAK)
		{}
		
		Interval<Number>::Interval(const Number& lower, const Number& upper) :
			mContent(lower, upper),
			mLowerBoundType(BoundType::WEAK),
			mUpperBoundType(BoundType::WEAK)
		{}
		
		Interval<Number>::Interval(const Number& lower, BoundType lowerBoundType, const Number& upper, BoundType upperBoundType) :
		mContent(lower, upper),
		mLowerBoundType(lowerBoundType),
		mUpperBoundType(upperBoundType)
		{}
		
		Interval<Number>::Interval(const Interval<Number>& o) :
			mContent(BoostInterval(o.mContent)),
			mLowerBoundType(o.mLowerBoundType),
			mUpperBoundType(o.mUpperBoundType)
		{}
		
		template<typename DisableIf<std::is_same<Number, double>>>
		Interval<Number>::Interval(double n);
		template<typename DisableIf<std::is_same<Number, double>::value>>
		Interval<Number>::Interval(double lower, double upper);
		template<typename DisableIf<std::is_same<Number, double>::value>>
		Interval<Number>::Interval(double lower, BoundType lowerBoundType, double upper, BoundType upperBoundType);
		
		template<typename Ratinal>
		Interval<Number>::Interval(Rational n);
		template<typename Rational>
		Interval<Number>::Interval(Rational lower, Rational upper);
		template<typename Rational>
		Interval<Number>::Interval(Rational lower, BoundType lowerBoundType, Rational upper, BoundType upperBoundType);
		
		~Interval();
        
        /***********************************************************************
         * Getter & Setter
         **********************************************************************/
		
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
		
		/***********************************************************************
		 * Transformations and advanced getters/setters
		 **********************************************************************/
		
		void set(const Number& lower, const Number& upper)
		{
			mContent = BoostInterval(lower, upper);
		}
		
		Number diameter() const;
		void diameter_assign();
		Number diameterRatio(const Interval<Number>& rhs) const;
		void diameterRatio_assign(const Interval<Number>& rhs);
		Number magnitude() const;
		void magnitude_assign();
		Number center() const;
		void center_assign();
		
		void bloat_by(const Number& width);
		void bloat_times(const Number& factor);
		void shrink_by(const Number& width);
		void shrink_times(const Number& factor);
		
		std::pair<Interval<Number>, Interval<Number>> split() const;
		std::list<Interval<Number>> split(unsigned n) const;
		
        /***********************************************************************
         * Arithmetic functions
         **********************************************************************/
		
		Interval<Number>& add(const Interval<Number>& rhs) const;
		void add_assign(const interval<Number>& rhs);
        Interval<Number>& sub(const Interval<Number>& rhs) const;
		void sub_assign(const Interval<Number>& rhs);
		Interval<Number>& mul(const Interval<Number>& rhs) const;
		void mul_assign(const Interval<Number>& rhs);
		Interval<Number>& div(const Interval<Number>& rhs) const;
		void div_assign(const Interval<Number>& rhs);
		std::pair<Interval<Number>, Interval<Number>> div_ext(const Interval<Number>& rhs) const;
		
		Interval<Number>& inverse() const;
		void inverse_assign();
		Interval<Number>& reciprocal() const;
		void reciprocal_assign();
		
		Interval<Number>& power(unsigned exp) const;
		void power_assign(unsigned exp);
		Interval<Number>& sqrt() const;
		void sqrt_assign();
		Interval<Number>& root(unsigned deg) const;
		void root_assign(unsigned deg);
		Interval<Number>& log() const;
		void log_assign();
		
        /***********************************************************************
         * Trigonometric functions
         **********************************************************************/
        
		Interval<Number>& sin() const;
		void sin_assign();
		Interval<Number>& cos() const;
		void cos_assign();
		Interval<Number>& tan() const;
		void tan_assign();
		
		Interval<Number>& asin() const;
		void asin_assign();
		Interval<Number>& acos() const;
		void acos_assign();
		Interval<Number>& atan() const;
		void atan_assign();
		
		Interval<Number>& sinh() const;
		void sinh_assign();
		Interval<Number>& cosh() const;
		void cosh_assign();
		Interval<Number>& tanh() const;
		void tanh_assign();
		
		Interval<Number>& asinh() const;
		void asinh_assign();
		Interval<Number>& acosh() const;
		void acosh_assign();
		Interval<Number>& atanh() const;
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
	
	template<typename Number>
	inline std::ostream& operator << <>(std::ostream& str, const INterval<Number>& i);
}

#include "Interval.tpp"