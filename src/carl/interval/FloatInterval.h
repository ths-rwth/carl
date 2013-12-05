/**
 * @author Florian Corzilius
 * @author Sebastian Junges
 * @author Stefan Schupp
 */
#pragma once

#include <map>
#include <cassert>
#include <cmath>
#include <cfloat>

#include <boost/numeric/interval.hpp>
#include <boost/numeric/interval/interval.hpp>

#include <cln/cln.h>

#include "../core/Variable.h"
#include "../core/Sign.h"
#include "../numbers/numbers.h"
#include "BoundType.h"
#include "../numbers/FLOAT_T.h"
#include "checking.h"
#include "rounding.h"

using namespace boost::numeric::interval_lib;

namespace carl
{

template<typename FloatImplementation>
class FloatInterval
{
public:
	///////////////
	//  Typedef  //
	///////////////

	//typedef boost::numeric::interval_lib::save_state<boost::numeric::interval_lib::rounded_arith_opp<double> > Rounding;
	//typedef boost::numeric::interval_lib::checking_no_nan<double, boost::numeric::interval_lib::checking_no_nan<double> > Checking;

	typedef boost::numeric::interval< FLOAT_T<FloatImplementation>, boost::numeric::interval_lib::policies< carl::rounding<FloatImplementation>, carl::checking<FloatImplementation> > > BoostFloatInterval;
	typedef std::map<Variable, FloatInterval<FloatImplementation>> evalfloatintervalmap;

	/// Standard assertion for checking the input to constructors and setters: the interval bounds might define an empty interval but can never cross (left > right).
#define FLOAT_BOUNDS_OK( left, leftType, right, rightType )\
            (leftType == BoundType::INFTY || rightType == BoundType::INFTY || left <= right)

protected:

	///////////////
	//  Members  //
	///////////////
    
	BoostFloatInterval mInterval;
	BoundType mLeftType;
	BoundType mRightType;

public:

	//////////////////////////////////
	//  Constructors & Destructors  //
	//////////////////////////////////

	/** Empty Constructor creates a unbounded interval
	 */
	FloatInterval<FloatImplementation>()
        {
            mInterval = BoostFloatInterval( 0 );
            mLeftType = BoundType::INFTY;
            mRightType = BoundType::INFTY;
        }

	/** Creates point interval at n
	 * @param n
	 * @param overapproximate compute the approximations of the given bounds so that the interval is an over-approximation of the corresponding OpenInterval; otherwise under-approximate (default: false)
	 */
	FloatInterval<FloatImplementation>(const FloatImplementation& n);
	
	FloatInterval<FloatImplementation>(int n) : FloatInterval((FloatImplementation)n)  {}

	/** Creates (preferably point) interval at n
	 * @param n
	 * @param bool overapproximate
	 */
	template<typename Rational>
	FloatInterval<FloatImplementation>(const Rational& n, bool overapproximate = false);

	template<typename Rational>
	FloatInterval<FloatImplementation>(const Rational& lower, BoundType lowerType, const Rational& upper, BoundType upperType, bool overapproxleft = false, bool overapproxright = false);

        //TODO !!!
	/** Creates closed FloatInterval
	 * @param _content
	 */
	FloatInterval<FloatImplementation>(const BoostFloatInterval& _content);

	/** Creates closed FloatInterval
	 * @param left
	 * @param right
	 */
	FloatInterval<FloatImplementation>(const FLOAT_T<FloatImplementation>& left, const FLOAT_T<FloatImplementation>& right);
	FloatInterval<FloatImplementation>(int left, int right) : FloatInterval((FLOAT_T<FloatImplementation>)left, (FLOAT_T<FloatImplementation>)right) {}

	/** Creates FloatInterval with BoundTypes
	 * @param _content
	 * @param leftType
	 * @param rightType
	 */
	FloatInterval<FloatImplementation>(const BoostFloatInterval& _content, BoundType leftType, BoundType rightType);

	/** Creates FloatInterval with BoundTypes
	 * @param left
	 * @param leftType
	 * @param right
	 * @param rightType
	 */
	FloatInterval<FloatImplementation>(double left, BoundType leftType, double right, BoundType rightType);
        
	FloatInterval<FloatImplementation>(int left, BoundType leftType, double right, BoundType rightType) :
		FloatInterval<FloatImplementation>((double)left,leftType, right, rightType) {}
        
	FloatInterval<FloatImplementation>(double left, BoundType leftType, int right, BoundType rightType): 
		FloatInterval<FloatImplementation>(left, leftType, (double)right, rightType) {}
        
	FloatInterval<FloatImplementation>(int left, BoundType leftType, int right, BoundType rightType) : 
		FloatInterval<FloatImplementation>((double)left, leftType, (double)right, rightType) {}

	/** Destructor.
	 */
	~FloatInterval();

	///////////////////////
	//  Getter & Setter  //
	///////////////////////

	/**
	 * Get the whole interval.
	 * @return mInterval
	 */
	const BoostFloatInterval& content() const
	{
            return mInterval;
	}

	/**
	 * Get left bound
	 * @return The left bound.
	 */
	const FLOAT_T<FloatImplementation>& left() const
	{
            return mInterval.lower();
	}

	/**
	 * Get right bound
	 * @return The right bound.
	 */
	const FLOAT_T<FloatImplementation>& right() const
	{
            return mInterval.upper();
	}

	/**
	 * Set new left bound for the interval.
	 * @param l new left bound
	 */
	void setLeft(const FLOAT_T<FloatImplementation> l)
	{
            mInterval.set(l, mInterval.upper());
	}

	/**
	 * Set new left bound for the interval by rounding down.
	 * @param l new left bound
	 */
	void setLeft(const cln::cl_RA& l)
	{
            mInterval.set(roundDown(l), mInterval.upper());
	}

	/**
	 * Set new left bound type for the interval.
	 * @param lType new left bound type
	 */
	void setLeftType(BoundType lType)
	{
		mLeftType = lType;
	}

	/**
	 * Set new right bound for the interval.
	 * @param r new right bound
	 */
	void setRight(const FLOAT_T<FloatImplementation>& r)
	{
		mInterval.set(left(), r);
	}

	/**
	 * Set new right bound for the interval by rounding up.
	 * @param r new right bound
	 */
	void setRight(const cln::cl_RA& r)
	{
		mInterval.set(left(), roundUp(r));
	}

	/**
	 * Set new right bound type for the interval.
	 * @param rType new right bound type
	 */
	void setRightType(const BoundType& rType)
	{
            mRightType = rType;
	}

	/** Get left BoundType
	 * @return mLeftType
	 */
	BoundType leftType() const
	{
            return mLeftType;
	}

	/** Get right BoundType
	 * @return mRightType
	 */
	BoundType rightType() const
	{
            return mRightType;
	}

        /**
         * Set both bounds.
         * @param l
         * @param r
         */
        void set(const FLOAT_T<FloatImplementation>& l, const FLOAT_T<FloatImplementation>& r)
        {
            mInterval.set(l,r);
        }
        
        /**
         * Create a point interval.
         * @param c
         */
        void set(const FLOAT_T<FloatImplementation>& c)
        {
            mInterval.set(c, c);
        }
        
	/** Set left bound
	 * @param left
	 */
	void cutUntil(const FLOAT_T<FloatImplementation>& left);

	/** Set right bound
	 * @param right
	 */
	void cutFrom(const FLOAT_T<FloatImplementation>& right);
        
        /**
         * Split the interval at the midpoint.
         * @param _left
         * @param _right
         */
        void split(FloatInterval<FloatImplementation>& _left, FloatInterval<FloatImplementation>& _right) const;
        
        /**
         * Split the interval in n uniform intervals contained in the _result vector.
         * @param _result
         * @param n
         */
        void split(std::vector<FloatInterval<FloatImplementation>>& _result, const unsigned n) const;
        
        /**
         * Bloat the interval by the given width.
         * @param _width
         */
        void bloat(const FLOAT_T<FloatImplementation>& _width);

	//////////////////
	//  Arithmetic  //
	//////////////////

	/** Adds two intervals and returns their sum.
	 * @param o
	 * @return sum
	 */
	FloatInterval<FloatImplementation> add(const FloatInterval<FloatImplementation>& o) const;

	/** Returns the negative value.
	 * @return negative value
	 */
	FloatInterval<FloatImplementation> inverse() const;

	/** Returns the negative value.
	 * @return negative value
	 */
	FloatInterval<FloatImplementation> sqrt() const;

	/** Multiplies two intervals and returns their product.
	 * @param o
	 * @return product
	 */
	FloatInterval<FloatImplementation> mul(const FloatInterval<FloatImplementation>& o) const;

	/** Divides two intervals.
	 * @param o
	 * @return this interval divided by the argument
	 * @throws invalid_argument in case the argument interval contains zero
	 */
	FloatInterval<FloatImplementation> div(const FloatInterval<FloatImplementation>& o) const throw ( std::invalid_argument);

	/** Extended Intervaldivision with intervals containing 0
	 * @param a
	 * @param b
	 * @param o
	 * @return true if interval splitting (results in a and b), false else (results only in a)
	 */
	bool div_ext(FloatInterval<FloatImplementation>& a, FloatInterval<FloatImplementation>& b, const FloatInterval<FloatImplementation>& o);

	/** Computes the power to <code>e</code> of this interval.
	 * @param e exponent
	 * @return power to <code>e</code> of this interval
	 */
	FloatInterval<FloatImplementation> power(unsigned e) const;

	/** Computes the reciprocal to the interval with respect to division by zero and infinity
	 * @param a first result reference
	 * @param b second result reference
	 * @return true if the result contains two intervals, else false
	 */
	bool reciprocal(FloatInterval<FloatImplementation>& a, FloatInterval<FloatImplementation>& b) const;

        /**
         * Computes the exp of the interval.
         * @return 
         */
        FloatInterval<FloatImplementation> exp() const;
        
        /**
         * Computes the logarithm of the interval.
         * @return 
         */
        FloatInterval<FloatImplementation> log() const;
        
        /**
         * Computes the sinus of the interval.
         * @return 
         */
        FloatInterval<FloatImplementation> sin() const;
        
        /**
         * Computes the cosinus of the interval.
         * @return 
         */
        FloatInterval<FloatImplementation> cos() const;

	/**
	 * Calculates the diameter of the interval
	 * @return the diameter of the interval
	 */
	FLOAT_T<FloatImplementation> diameter() const;
        
        FLOAT_T<FloatImplementation> diameterRatio( const FloatInterval<FloatImplementation>& _interval) const;
        
        /**
         * Calculate the magnitude of the interval.
         * @return 
         */
        FLOAT_T<FloatImplementation> magnitude() const;

	//////////////////
	//  Operations  //
	//////////////////

	/**
	 * @return true if the bounds define an empty interval, false otherwise
	 */
	bool empty() const;

	/**
	 * @return true if the bounds define the whole real line, false otherwise
	 */
	bool unbounded() const
	{
		return mLeftType == BoundType::INFTY && mRightType == BoundType::INFTY;
	}

	/**
	 * Determine whether the interval lays entirely left of 0 (NEGATIVE_SIGN), right of 0 (POSITIVE_SIGN) or contains 0 (ZERO_SIGN).
	 * @return NEGATIVE_SIGN, if the interval lays entirely left of 0; POSITIVE_SIGN, if right of 0; or ZERO_SIGN, if contains 0.
	 */
	Sign sgn();

	/**
	 * @param n
	 * @return true in case n is contained in this Interval
	 */
	bool contains(const FLOAT_T<FloatImplementation>& n) const;

	/**
	 * @param o
	 * @return true in case o is a subset of this Interval
	 */
	bool contains(const FloatInterval<FloatImplementation>& o) const;

	/**
	 * Checks whether n is contained in the <b>closed</b> interval defined by the bounds.
	 * @param n
	 * @return true in case n meets the interval bounds or a point inbetween
	 */
	bool meets(FLOAT_T<FloatImplementation> n) const;

	/**
	 * @param o
	 * @return intersection with the given Interval and this Interval, or (0, 0) in case the intersection is empty
	 */
	FloatInterval<FloatImplementation> intersect(const FloatInterval<FloatImplementation>& o) const;

	/**
	 * @return the midpoint of this interval
	 */
	FLOAT_T<FloatImplementation> midpoint() const;

	/**
	 * Computes the absolute value of this interval, i.e. the maximum of the absolute values of its bounds.
	 * @return absolute value of the interval
	 * @see Marc Daumas, Guillaume Melquiond, and Cesar Munoz - "Guaranteed Proofs Using Interval Arithmetic".
	 */
	FloatInterval<FloatImplementation> abs() const;

	///////////////////////////
	// Relational Operations //
	///////////////////////////

	/**
	 * @param o
	 * @return true in case the other interval equals this
	 */
	bool isEqual(const FloatInterval<FloatImplementation>& o) const;

	/** Checks whether the left bound of this interval is less or equal the left bound of the other interval.
	 * @param o
	 * @return true if the left bound of this interval is less or equal the left bound of the other interval
	 */
	bool isLessOrEqual(const FloatInterval<FloatImplementation>& o) const;

	/** Checks whether the right bound of this interval is greater or equal the right bound of the other interval
	 * @param o
	 * @return true if the right bound of this interval is greater or equal the right bound of the other interval
	 */
	bool isGreaterOrEqual(const FloatInterval<FloatImplementation>& o) const;

	/**
	 * Prints out the Interval
	 */
	void dbgprint() const;


	////////////////////
	// Static Methods //
	////////////////////

	/**
	 * Creates the empty interval denoted by ]0,0[
	 * @return empty interval
	 */
	static FloatInterval<FloatImplementation> emptyInterval()
	{
		return FloatInterval<FloatImplementation>(BoostFloatInterval(0), BoundType::STRICT, BoundType::STRICT);
	}

	/**
	 * Creates the unbounded interval denoted by ]-infty,infty[
	 * @return empty intervaleval
	 */
	static FloatInterval<FloatImplementation> unboundedInterval()
	{
		return FloatInterval<FloatImplementation>();
	}


	/** Returns a down-rounded representation of the given numeric
	 * @param numeric o
	 * @param bool overapproximate
	 * @return double representation of o (underapprox) Note, that it can return the double INFINITY.
	 */
	template<typename Rational>
	static FLOAT_T<FloatImplementation> roundDown(const Rational& o, bool overapproximate = false);

	/** Returns a up-rounded representation of the given numeric
	 * @param numeric o
	 * @param bool overapproximate
	 * @return double representation of o (overapprox) Note, that it can return the double INFINITY.
	 */
	template<typename Rational>
	static FLOAT_T<FloatImplementation> roundUp(const Rational& o, bool overapproximate = false);

	void operator +=(const FloatInterval<FloatImplementation>&);
	void operator -=(const FloatInterval<FloatImplementation>&);
	void operator *=(const FloatInterval<FloatImplementation>&);

	// unary arithmetic operators of FloatInterval
	const FloatInterval<FloatImplementation> operator -(const FloatInterval<FloatImplementation>& lh) const;
	friend std::ostream& operator<<(std::ostream& str, const FloatInterval<FloatImplementation>&);


private:
	////////////////////////////
	//  Auxiliary Functions:  //
	////////////////////////////

	/** Return the bound type which corresponds to the weakest-possible type when combining all elements in two intervals.
	 * @param type1
	 * @param type2
	 * @return BoundType::INFTY if one of the given types is INIFNITY_BOUND, BoundType::STRICT if one of the given types is BoundType::STRICT
	 */
	inline static BoundType getWeakestBoundType(BoundType type1, BoundType type2)
	{
		return (type1 == BoundType::INFTY || type2 == BoundType::INFTY)
				? BoundType::INFTY : (type1 == BoundType::STRICT || type2 == BoundType::STRICT) ? BoundType::STRICT : BoundType::WEAK;
	}

}; // class FloatInterval


template<typename Rational>
FloatInterval::FloatInterval(const Rational& rat, bool overapprox) : 
FloatInterval(rat, BoundType::WEAK,rat, BoundType::WEAK, overapprox, overapprox)
{
    // TODO overapprox in both directions?
}

template<typename Rational>
FloatInterval::FloatInterval(const Rational& lower, BoundType lowerType, const Rational& upper, BoundType upperType, bool overapproxleft, bool overapproxright) : 
    mLeftType( lowerType ),
    mRightType( upperType )
{
	double dLeft = roundDown(lower, overapproxleft);
	double dRight = roundUp(upper, overapproxright);
	if(dLeft == -INFINITY) mLeftType = BoundType::INFTY;
	if(dRight == INFINITY) mRightType = BoundType::INFTY;
	if(mLeftType == BoundType::INFTY && mRightType == BoundType::INFTY)
	{
		mInterval = BoostFloatInterval(0);
	}
	else if(mLeftType == BoundType::INFTY)
	{
		mInterval = BoostFloatInterval(dRight);
	}
	else if(mRightType == BoundType::INFTY)
	{
		mInterval = BoostFloatInterval(dLeft);
	}
	else if((lower == upper && lowerType != upperType) || lower > upper)
	{
		mLeftType = BoundType::STRICT;
		mRightType = BoundType::STRICT;
		mInterval = BoostFloatInterval(0);
	}
	else
	{
		mInterval = BoostFloatInterval(dLeft, dRight);
	}
}

template<typename Rational>
double FloatInterval::roundDown(const Rational& o, bool overapproximate)
{
	double result = getDouble(o);
	if(result == -INFINITY) return result;
	if(result == INFINITY) return DBL_MAX;
	// If the cln::cl_RA cannot be represented exactly by a double, round.
//	Rational r = rationalize<Rational>(result);
	if(overapproximate || rationalize<Rational>(result) != o)
	{
		if(result == -DBL_MAX) return -INFINITY;
		return std::nextafter(result, -INFINITY);
	}
	else
	{
		return result;
	}
}

template<typename Rational>
double FloatInterval::roundUp(const Rational& o, bool overapproximate)
{
	double result = getDouble(o);
	if(result == INFINITY) return result;
	if(result == -INFINITY) return -DBL_MAX;
	// If the cln::cl_RA cannot be represented exactly by a double, round.
//	Rational r = rationalize<Rational>(result);
	if(overapproximate || rationalize<Rational>(result) != o)
	{
		if(result == DBL_MAX) return INFINITY;
		return std::nextafter(result, INFINITY);
	}
	else
	{
		return result;
	}
} 

inline const FloatInterval operator +(const FloatInterval& lh, const FloatInterval& rh)
{
	return lh.add(rh);
}

inline const FloatInterval operator +(const FloatInterval& lh, const double& rh)
{
	// TODO optimization potential
	return lh.add(FloatInterval(rh));
}

inline const FloatInterval operator +(const double& lh, const FloatInterval& rh)
{
	// TODO optimization potential
	return rh.add(FloatInterval(lh));
}

inline const FloatInterval operator -(const FloatInterval& lh, const FloatInterval& rh)
{
	return lh.add(rh.inverse());
}

inline const FloatInterval operator -(const FloatInterval& lh, const double& rh)
{
	return lh + (-rh);
}

inline const FloatInterval operator -(const double& lh, const FloatInterval& rh)
{
	return (-lh) +rh;
}

inline const FloatInterval operator *(const FloatInterval& lh, const FloatInterval& rh)
{
	return lh.mul(rh);
}

inline const FloatInterval operator *(const FloatInterval& lh, const double& rh)
{
	return FloatInterval(lh.mul(FloatInterval(rh)));
}

inline const FloatInterval operator *(const double& lh, const FloatInterval& rh)
{
	return rh * lh;
}

inline const FloatInterval operator /(const FloatInterval& lh, const double& rh) throw ( std::overflow_error)
{
	return lh.div(FloatInterval(rh));
}

inline const FloatInterval operator /(const double& lh, const FloatInterval& rh) throw ( std::overflow_error)
{
	FloatInterval result = FloatInterval(lh);
	result.div(rh);
	return result;
}

// relational operators

inline bool operator ==(const FloatInterval& lh, const FloatInterval& rh)
{
	return lh.isEqual(rh);
}

inline bool operator !=(const FloatInterval& lh, const FloatInterval& rh)
{
	return !lh.isEqual(rh);
}

inline bool operator <=(const FloatInterval& lh, const FloatInterval& rh)
{
	return lh.isLessOrEqual(rh);
}

inline bool operator >=(const FloatInterval& lh, const FloatInterval& rh)
{
	return lh.isGreaterOrEqual(rh);
}

} // namespace carl

namespace std
{
    template<>
    struct hash<carl::FloatInterval> {    
        size_t operator()(const carl::FloatInterval& double_interval) const 
        {
            return (  ((size_t) double_interval.left() ^ (size_t) double_interval.leftType())
                    ^ ((size_t) double_interval.right() ^ (size_t) double_interval.rightType()));
        }
    };
} // namespace std
