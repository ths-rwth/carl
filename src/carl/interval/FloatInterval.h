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
        typedef FLOAT_T<FloatImplementation> float_t;
	typedef boost::numeric::interval< float_t, boost::numeric::interval_lib::policies< carl::rounding<FloatImplementation>, carl::checking<FloatImplementation> > > BoostFloatInterval;
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
	FloatInterval<FloatImplementation>(const FloatImplementation& n)
        {
            FloatInterval<FloatImplementation>(n, BoundType::WEAK, n, BoundType::WEAK);
        }
	
	FloatInterval<FloatImplementation>(int n) : FloatInterval((FloatImplementation)n)  {}

	/** Creates (preferably point) interval at n
	 * @param n
	 * @param bool overapproximate
	 */
//	template<typename Rational>
//	FloatInterval<FloatImplementation>(const Rational& n, bool overapproximate = false);

//	template<typename Rational>
//	FloatInterval<FloatImplementation>(const Rational& lower, BoundType lowerType, const Rational& upper, BoundType upperType, bool overapproxleft = false, bool overapproxright = false);

        //TODO !!!
	/** Creates closed FloatInterval
	 * @param _content
	 */
	FloatInterval<FloatImplementation>(const BoostFloatInterval& _content);

	/** Creates closed FloatInterval
	 * @param left
	 * @param right
	 */
	FloatInterval<FloatImplementation>(const float_t& left, const float_t& right)
        {
            FloatInterval<FloatImplementation>(left, BoundType::WEAK, right, BoundType::WEAK);
        }
	FloatInterval<FloatImplementation>(int left, int right) : FloatInterval((float_t)left, (float_t)right) {}

	/** Creates FloatInterval with BoundTypes
	 * @param _content
	 * @param leftType
	 * @param rightType
	 */
	FloatInterval<FloatImplementation>(const BoostFloatInterval& _content, BoundType leftType, BoundType rightType) : mInterval(_content), mLeftType(leftType), mRightType(rightType) {}

	/** Creates FloatInterval with BoundTypes
	 * @param left
	 * @param leftType
	 * @param right
	 * @param rightType
	 */
	FloatInterval<FloatImplementation>(double left, BoundType leftType, double right, BoundType rightType)
        {
            FloatInterval<FloatImplementation>((float_t)left, leftType, (float_t)right, rightType);
        }
        
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
	const float_t& left() const
	{
            return mInterval.lower();
	}

	/**
	 * Get right bound
	 * @return The right bound.
	 */
	const float_t& right() const
	{
            return mInterval.upper();
	}

	/**
	 * Set new left bound for the interval.
	 * @param l new left bound
	 */
	void setLeft(const float_t l)
	{
            mInterval.set(l, mInterval.upper());
	}

	/**
	 * Set new left bound for the interval by rounding down.
	 * @param l new left bound
	 */
//	void setLeft(const cln::cl_RA& l)
//	{
//            mInterval.set(roundDown(l), mInterval.upper());
//	}

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
	void setRight(const float_t& r)
	{
		mInterval.set(left(), r);
	}

	/**
	 * Set new right bound for the interval by rounding up.
	 * @param r new right bound
	 */
//	void setRight(const cln::cl_RA& r)
//	{
//		mInterval.set(left(), roundUp(r));
//	}

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
        void set(const float_t& l, const float_t& r)
        {
            mInterval.set(l,r);
        }
        
        /**
         * Create a point interval.
         * @param c
         */
        void set(const float_t& c)
        {
            mInterval.set(c, c);
        }
        
	/** Set left bound
	 * @param left
	 */
	void cutUntil(const float_t& _left)
        {
            if( _left > left() && _left <= right() )
            {
                setLeft( _left );
                mLeftType = BoundType::WEAK;
            }
            else if( _left > left() )
            {
                mLeftType   = BoundType::STRICT;
                mRightType  = BoundType::STRICT;
                mInterval = BoostDoubleInterval( 0 );
            }
        }

	/** Set right bound
	 * @param right
	 */
	void cutFrom(const float_t& _right)
        {
            if( _right >= left() && _right < right() )
            {
                setRight( _right );
                mRightType = BoundType::WEAK;
            }
            else if( _right < left() )
            {
                mLeftType   = BoundType::STRICT;
                mRightType  = BoundType::STRICT;
                mInterval = BoostDoubleInterval( 0 );
            }
        }
        
        /**
         * Split the interval at the midpoint.
         * @param _left
         * @param _right
         */
        void split(FloatInterval<FloatImplementation>& _left, FloatInterval<FloatImplementation>& _right) const
        {
            if(left() != right() || (leftType() == BoundType::INFTY && rightType() == BoundType::INFTY))
            {
                _left.mInterval.set(left(),midpoint());
                _left.setLeftType(leftType());
                _left.setRightType(BoundType::STRICT);

                _right.mInterval.set(midpoint(),right());
                _right.setLeftType(BoundType::WEAK);
                _right.setRightType(rightType());
            }
            else
            {
                _left = *this;
                _right = *this;
            }
        }
        
        /**
         * Split the interval in n uniform intervals contained in the _result vector.
         * @param _result
         * @param n
         */
        void split(std::vector<FloatInterval<FloatImplementation>>& _result, const unsigned n) const
        {
            float_t diameter = this->diameter();
            diameter /= n;

            DoubleInterval tmp;
            tmp.set(left(), left()+diameter);
            tmp.setLeftType(leftType());
            tmp.setRightType(BoundType::STRICT);
            _result.insert(_result.end(), tmp);

            for( unsigned i = 1; i < (n-1); ++i )
            {
                tmp.set(diameter*i, diameter*(i+1));
                _result.insert(_result.end(), tmp);
            }

            tmp.set(diameter*(n-1),diameter*n);
            tmp.setRightType(rightType());
            _result.insert(_result.end(), tmp);
        }
        
        /**
         * Bloat the interval by the given width.
         * @param _width
         */
        void bloat(const float_t& _width)
        {
            mInterval.set(mInterval.lower()-_width, mInterval.upper()+_width);
        }

	//////////////////
	//  Arithmetic  //
	//////////////////

	/** Adds two intervals and returns their sum.
	 * @param o
	 * @return sum
	 */
	FloatInterval<FloatImplementation> add(const FloatInterval<FloatImplementation>& o) const
        {
            return DoubleInterval( mInterval + o.content(),
                      getWeakestBoundType( mLeftType, o.mLeftType ),
                      getWeakestBoundType( mRightType, o.mRightType ) );
        }

	/** Returns the negative value.
	 * @return negative value
	 */
	FloatInterval<FloatImplementation> inverse() const
        {
            return DoubleInterval( -right(), mRightType, -left(), mLeftType );
        }

	/** Returns the negative value.
	 * @return negative value
	 */
	FloatInterval<FloatImplementation> sqrt() const
        {
            if( mRightType != BoundType::INFTY && right() < 0 )
            {
                return emptyInterval();
            }
            float_t lvalue = (mLeftType == BoundType::INFTY || left() < 0) ? 0 : left();
            float_t rvalue = (mRightType == BoundType::INFTY || right() < 0) ? 0 : right();
            if( lvalue > rvalue && mRightType == BoundType::INFTY ) rvalue = lvalue;
            BoostDoubleInterval content = boost::numeric::sqrt( BoostDoubleInterval( lvalue, rvalue ) ); //TODO: Sure to take boost::numeric::sqrt?
            BoundType leftType = mLeftType;
            BoundType rightType = mRightType;
            if( mLeftType == BoundType::INFTY || left() < 0 )
            {
                leftType = BoundType::WEAK;
            }
            return DoubleInterval( content, leftType, rightType );
        }

	/** Multiplies two intervals and returns their product.
	 * @param o
	 * @return product
	 */
	FloatInterval<FloatImplementation> mul(const FloatInterval<FloatImplementation>& o) const
        {
            BoundType leftType = BoundType::WEAK;
            BoundType rightType = BoundType::WEAK;
            if( (mLeftType == BoundType::INFTY && (_interval.right() > 0 || _interval.mRightType == BoundType::INFTY))
                || (mRightType == BoundType::INFTY && (_interval.left() < 0 || _interval.mLeftType == BoundType::INFTY))
                || (_interval.mLeftType == BoundType::INFTY && (right() > 0 || mRightType == BoundType::INFTY))
                || (_interval.mRightType == BoundType::INFTY && (right() < 0 || (left() < 0 || mLeftType == BoundType::INFTY))) )
            {
                leftType = BoundType::INFTY;
            }
            if( (mLeftType == BoundType::INFTY && (_interval.right() < 0 || (_interval.left() < 0 || _interval.mLeftType == BoundType::INFTY)))
                || (mRightType == BoundType::INFTY && (_interval.left() > 0 || (_interval.right() > 0 || _interval.mRightType == BoundType::INFTY)))
                || (_interval.mLeftType == BoundType::INFTY && (right() < 0 || (left() < 0 || mLeftType == BoundType::INFTY)))
                || (_interval.mRightType == BoundType::INFTY && (left() > 0 || (right() > 0 || mRightType == BoundType::INFTY))) )
            {
                rightType = BoundType::INFTY;
            }
            return DoubleInterval( BoostDoubleInterval( mInterval*_interval.content() ), leftType, rightType );
        }

	/** Divides two intervals.
	 * @param o
	 * @return this interval divided by the argument
	 * @throws invalid_argument in case the argument interval contains zero
	 */
	FloatInterval<FloatImplementation> div(const FloatInterval<FloatImplementation>& o) const throw ( std::invalid_argument)
        {
            if( _interval.contains( 0 ) ) throw ( std::invalid_argument( "Division by interval containing zero not allowed." ) );
            BoundType leftType = BoundType::WEAK;
            BoundType rightType = BoundType::WEAK;
            if( (mLeftType == BoundType::INFTY && (_interval.right() > 0 || _interval.mRightType == BoundType::INFTY))
                || (mRightType == BoundType::INFTY && (_interval.left() < 0 || _interval.mLeftType == BoundType::INFTY))
                || (_interval.mLeftType == BoundType::INFTY && (right() > 0 || mRightType == BoundType::INFTY))
                || (_interval.mRightType == BoundType::INFTY && (right() < 0 || (left() < 0 || mLeftType == BoundType::INFTY))) )
            {
                leftType = BoundType::INFTY;
            }
            if( (mLeftType == BoundType::INFTY && (_interval.right() < 0 || (_interval.left() < 0 || _interval.mLeftType == BoundType::INFTY)))
                || (mRightType == BoundType::INFTY && (_interval.left() > 0 || (_interval.right() > 0 || _interval.mRightType == BoundType::INFTY)))
                || (_interval.mLeftType == BoundType::INFTY && (right() < 0 || (left() < 0 || mLeftType == BoundType::INFTY)))
                || (_interval.mRightType == BoundType::INFTY && (left() > 0 || (right() > 0 || mRightType == BoundType::INFTY))) )
            {
                rightType = BoundType::INFTY;
            }
            return DoubleInterval( BoostDoubleInterval( mInterval/_interval.content() ), leftType, rightType );
        }

	/** Extended Intervaldivision with intervals containing 0
	 * @param a
	 * @param b
	 * @param o
	 * @return true if interval splitting (results in a and b), false else (results only in a)
	 */
	bool div_ext(FloatInterval<FloatImplementation>& a, FloatInterval<FloatImplementation>& b, const FloatInterval<FloatImplementation>& o)
        {
            FloatInterval<FloatImplementation> reciprocalA, reciprocalB;
            bool          splitOccured;

            if( o.leftType() != BoundType::INFTY && o.left() == 0 && o.rightType() != BoundType::INFTY && o.right() == 0 )    // point interval 0
            {
                splitOccured = false;
                if( this->contains( 0 ))
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
                if( o.unbounded() )
                {
                    a = unboundedInterval();
                    return false;
                }    // o.unbounded
                else
                {
                    //default case
                    splitOccured = o.reciprocal( reciprocalA, reciprocalB );
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
                }    // !o.unbounded

            }    // !pointinterval 0
        }

	/** Computes the power to <code>e</code> of this interval.
	 * @param e exponent
	 * @return power to <code>e</code> of this interval
	 */
	FloatInterval<FloatImplementation> power(unsigned e) const
        {
            assert(_exp <= INT_MAX );
            if( _exp % 2 == 0 )
            {
                if( mLeftType == BoundType::INFTY && mRightType == BoundType::INFTY )
                {
                    return DoubleInterval();
                }
                else if( mLeftType == BoundType::INFTY )
                {
                    if( contains( 0 ) )
                    {
                        return DoubleInterval( 0, BoundType::WEAK, 0, BoundType::INFTY );
                    }
                    else
                    {
                        return DoubleInterval( boost::numeric::pow( mInterval, (int)_exp ), mRightType, BoundType::INFTY ); //TODO: Use boost::numeric::pow?
                    }
                }
                else if( mRightType == BoundType::INFTY )
                {
                    if( contains( 0 ) )
                    {
                        return DoubleInterval( 0, BoundType::WEAK, 0, BoundType::INFTY );
                    }
                    else
                    {
                        return DoubleInterval( boost::numeric::pow( mInterval, (int)_exp ), mLeftType, BoundType::INFTY ); //TODO: Use boost::numeric::pow?
                    }
                }
                else
                {
                    BoundType rType = mRightType;
                    BoundType lType = mLeftType;
                    if( std::abs( left() ) > std::abs( right() ) )
                    {
                        rType = mLeftType;
                        lType = mRightType;
                    }
                    if( contains( 0 ) )
                    {
                        return DoubleInterval( boost::numeric::pow( mInterval, (int)_exp ), BoundType::WEAK, rType ); //TODO: Use boost::numeric::pow?
                    }
                    else
                    {
                        return DoubleInterval( boost::numeric::pow( mInterval, (int)_exp ), lType, rType ); //TODO: Use boost::numeric::pow?
                    }
                }
            }
            else
            {
                return DoubleInterval( boost::numeric::pow( mInterval, (int)_exp ), mLeftType, mRightType ); //TODO: Use boost::numeric::pow?
            }
        }

	/** Computes the reciprocal to the interval with respect to division by zero and infinity
	 * @param a first result reference
	 * @param b second result reference
	 * @return true if the result contains two intervals, else false
	 */
	bool reciprocal(FloatInterval<FloatImplementation>& a, FloatInterval<FloatImplementation>& b) const
        {
            if( this->unbounded() )
            {
                a = emptyInterval();
                return false;
            }
            else if( this->contains( 0 ) && left() != 0 && right() != 0 )
            {
                if( mLeftType == BoundType::INFTY )
                {
                    a = FloatInterval<FloatImplementation>( 0, BoundType::INFTY, 0, BoundType::WEAK );
                    b = FloatInterval<FloatImplementation>( BoostFloatInterval<FloatImplementation>( 1 ) / BoostFloatInterval<FloatImplementation>( right() ), BoundType::WEAK, BoundType::INFTY );
                }
                else if( mRightType == BoundType::INFTY )
                {
                    a = FloatInterval<FloatImplementation>( BoostFloatInterval<FloatImplementation>( 1 ) / BoostFloatInterval<FloatImplementation>( left() ), BoundType::INFTY, BoundType::WEAK );
                    b = FloatInterval<FloatImplementation>( 0, BoundType::WEAK, 0, BoundType::INFTY );
                }
                else if( left() == 0 && right() != 0 )
                {
                    a = FloatInterval<FloatImplementation>( 0, BoundType::INFTY, 0, BoundType::INFTY );
                    b = FloatInterval<FloatImplementation>( BoostFloatInterval<FloatImplementation>( 1 ) / BoostFloatInterval<FloatImplementation>( right() ), BoundType::WEAK, BoundType::INFTY );
                }
                else if( left() != 0 && right() == 0 )
                {
                    a = FloatInterval<FloatImplementation>( BoostFloatInterval<FloatImplementation>( 1 ) / BoostFloatInterval<FloatImplementation>( left() ), BoundType::INFTY, BoundType::WEAK );
                    b = unboundedInterval(); // todo: really the whole interval here?
                }
                else if( left() == 0 && right() == 0 )
                {
                    a = unboundedInterval();
                    return false;
                }
                else
                {
                    a = FloatInterval<FloatImplementation>( BoostFloatInterval<FloatImplementation>( 1 ) / BoostFloatInterval<FloatImplementation>( left() ), BoundType::INFTY, BoundType::WEAK );
                    b = FloatInterval<FloatImplementation>( BoostFloatInterval<FloatImplementation>( 1 ) / BoostFloatInterval<FloatImplementation>( right() ), BoundType::WEAK, BoundType::INFTY );
                }
                return true;
            }
            else
            {
                if( mLeftType == BoundType::INFTY && right() != 0 )
                {
                    a = FloatInterval<FloatImplementation>(  1 / right() , mRightType, 0,  BoundType::WEAK );
                }
                else if( mLeftType == BoundType::INFTY && right() == 0 )
                {
                    a = FloatInterval<FloatImplementation>( 0, BoundType::INFTY, 0, BoundType::WEAK );
                }
                else if( mRightType == BoundType::INFTY && left() != 0 )
                {
                    a = FloatInterval<FloatImplementation>(  0 , BoundType::WEAK, 1  /  left(), mLeftType );
                }
                else if( mRightType == BoundType::INFTY && left() == 0 )
                {
                    a = FloatInterval<FloatImplementation>( 0, BoundType::WEAK, 0, BoundType::INFTY );
                }
                else if( left() != 0 && right() != 0 )
                {
                    a = FloatInterval<FloatImplementation>( BoostFloatInterval<FloatImplementation>( 1 ) / mInterval, mRightType, mLeftType );
                }
                else if( left() == 0 && right() != 0 )
                {
                    a = FloatInterval<FloatImplementation>( BoostFloatInterval<FloatImplementation>( 1 ) / BoostFloatInterval<FloatImplementation>( right() ), mRightType, BoundType::INFTY );
                }
                else if( left() != 0 && right() == 0 )
                {
                    a = FloatInterval<FloatImplementation>( BoostFloatInterval<FloatImplementation>( 1 ) / BoostFloatInterval<FloatImplementation>( left() ), BoundType::INFTY, mLeftType );
                }

                return false;
            }
        }

        /**
         * Computes the exp of the interval.
         * @return 
         */
        FloatInterval<FloatImplementation> exp() const
        {
            float_t exp_low;
            float_t exp_up;
            mInterval.lower().exp(exp_low, CARL_RND::CARL_RNDD);
            mInterval.upper().exp(exp_up, CARL_RND::CARL_RNDU);
            return FloatInterval<FloatImplementation>( exp_low, mLeftType, exp_up, mRightType);
        }
        
        /**
         * Computes the logarithm of the interval.
         * @return 
         */
        FloatInterval<FloatImplementation> log() const
        {
            assert( left() > 0 );
            float_t log_low;
            float_t log_up;
            mInterval.lower().log(log_low, CARL_RND::CARL_RNDD);
            mInterval.upper().log(log_up, CARL_RND::CARL_RNDU);
            return FloatInterval<FloatImplementation>(log_low, mLeftType, log_up, mRightType);
        }
        
        /**
         * Computes the sinus of the interval.
         * @return 
         */
        FloatInterval<FloatImplementation> sin() const
        {
            float_t tmp_up,tmp_lo;
            float_t pi_up = 0;
            float_t pi_lo = 0;

            right().div(tmp_up, pi_lo, CARL_RND::CARL_RNDU);
            left().div(tmp_low, pi_up, CARL_RND::CARL_RNDD);
            tmp_up.mul_assign(2, CARL_RND::CARL_RNDU);
            tmp_lo.mul_assign(2, CARL_RND::CARL_RNDD);

            int iUp, iLo;
            tmp_up.floor(iUp);
            tmp_lo.floor(iLo);
            int iPeriod = iUp - iLo;

            FloatInterval<FloatImplementation> result(-1,1);
            if(iPeriod >= 4)
            {
                return result;
            }
            else
            {
                int modUp = iUp % 4;
                if(modUp < 0)
                {
                    modUp += 4;
                }

                int modLo = iLo % 4;
                if(modLo < 0)
                {
                    modLo += 4;
                }

                float_t tmp1, tmp2;
                left().sin(tmp1, CARL_RND::CARL_RNDD);
                right().sin(tmp2, CARL_RND::CARL_RNDU);

                switch(modLo)
                {
                case 0:
                        switch(modUp)
                        {
                            case 0:
                                if(iPeriod == 0)
                                {
                                    result.set(tmp1, tmp2);
                                }
                                break;
                            case 1:
                                result.set(tmp1 > tmp2 ? tmp2 : tmp1, 1);
                                break;
                            case 2:
                                result.set(tmp2, 1);
                                break;
                            case 3:
                                break;
                        }
                        break;
                case 1:
                        switch(modUp)
                        {
                            case 0:
                                result.set(-1, tmp1 > tmp2 ? tmp1 : tmp2);
                                break;
                            case 1:
                                if(iPeriod == 0)
                                {
                                    result.set(tmp2, tmp1);
                                }
                                break;
                            case 2:
                                result.set(tmp2, tmp1);
                                break;
                            case 3:
                                result.set(-1, tmp1);
                                break;
                        }
                        break;
                case 2:
                        switch(modUp)
                        {
                            case 0:
                                result.set(-1, tmp2);
                                break;
                            case 1:
                                break;
                            case 2:
                                    if(iPeriod == 0)
                                    {
                                        result.set(tmp2, tmp1);
                                    }
                                    break;
                            case 3:
                                result.set(-1, tmp1 > tmp2 ? tmp1 : tmp2 );
                                break;
                        }
                        break;
                case 3:
                        switch(modUp)
                        {
                            case 0:
                                result.set(tmp1, tmp2);
                                break;
                            case 1:
                                result.set(tmp1, 1);
                                break;
                            case 2:
                                result.set( tmp1 > tmp2 ? tmp2 : tmp1, 1);
                                break;
                            case 3:
                                if(iPeriod == 0)
                                {
                                    result.set(tmp1, tmp2);
                                }
                                break;
                        }
                        break;
                }
                return result;
            }
        }
        
        /**
         * Computes the cosinus of the interval.
         * @return 
         */
        FloatInterval<FloatImplementation> cos() const
        {
            float_t tmp_up, tmp_lo;
            float_t pi_up = 0;
            float_t pi_lo = 0;

            right().div(tmp_up, pi_lo, CARL_RND::CARL_RNDU);
            left().div(tmp_low, pi_up, CARL_RND::CARL_RNDD);
            tmp_up.mul_assign(2, CARL_RND::CARL_RNDU);
            tmp_lo.mul_assign(2, CARL_RND::CARL_RNDD);

            int iUp, iLo;
            tmp_up.floor(iUp);
            tmp_lo.floor(iLo);
            int iPeriod = iUp - iLo;

            FloatInterval<FloatImplementation> result(-1,1);
            if(iPeriod >= 4)
            {
                return result;
            }
            else
            {
                int modUp = iUp % 4;
                if(modUp < 0)
                    modUp += 4;

                int modLo = iLo % 4;
                if(modLo < 0)
                    modLo += 4;

                float_t tmp1, tmp2;
                left().sin(tmp1, CARL_RND::CARL_RNDD);
                right().sin(tmp2, CARL_RND::CARL_RNDU);

                switch(modLo)
                {
                case 0:
                        switch(modUp)
                        {
                        case 0:
                            if(iPeriod == 0)
                            {
                                result.set(tmp2, tmp1);
                            }
                            break;
                        case 1:
                            result.set(tmp2, tmp1);
                            break;
                        case 2:
                            result.set(-1, tmp1);
                            break;
                        case 3:
                            result.set(-1, tmp1 > tmp2 ? tmp1 : tmp2 );
                            break;
                        }
                        break;
                case 1:
                        switch(modUp)
                        {
                        case 0:
                            break;
                        case 1:
                            if(iPeriod == 0)
                            {
                                result.set(tmp2, tmp1);
                            }
                            break;
                        case 2:
                            result.set(-1, tmp1 > tmp2 ? tmp1 : tmp2 );
                            break;
                        case 3:
                            result.set(-1, tmp2);
                            break;
                        }
                        break;
                case 2:
                        switch(modUp)
                        {
                        case 0:
                            result.set(tmp1, 1);
                            break;
                        case 1:
                            result.set(tmp1 > tmp2 ? tmp2 : tmp1, 1);
                            break;
                        case 2:
                            if(iPeriod == 0)
                            {
                                result.set(tmp1, tmp2);
                            }
                            break;
                        case 3:
                            result.set(tmp1, tmp2);
                            break;
                        }
                        break;
                case 3:
                        switch(modUp)
                        {
                        case 0:
                            result.set(tmp1 > tmp2 ? tmp2 : tmp1, 1);
                            break;
                        case 1:
                            result.set(tmp2, 1);
                            break;
                        case 2:
                            break;
                        case 3:
                            if(iPeriod == 0)
                            {
                                result.set(tmp1, tmp2);
                            }
                            break;
                        }
                        break;
                }
                return result;
            }
        }

	/**
	 * Calculates the diameter of the interval
	 * @return the diameter of the interval
	 */
	float_t diameter() const
        {
            assert( FLOAT_BOUNDS_OK( left(), mLeftType, right(), mRightType ));
            if( mLeftType == BoundType::INFTY || mRightType == BoundType::INFTY )
            {
                return float_t(-1);
            }
            float_t res(right());
            res.sub_assign(left());
            return res;
        }
        
        float_t diameterRatio( const FloatInterval<FloatImplementation>& _interval) const
        {
            return diameter()/_interval.diameter();
        }
        
        /**
         * Calculate the magnitude of the interval.
         * @return 
         */
        float_t magnitude() const
        {
            assert( DOUBLE_BOUNDS_OK( left(), mLeftType, right(), mRightType));
            float_t inf,sup;
            mInterval.lower().abs(inf);
            mInterval.upper().abs(sup);
            return inf < sup ? sup : inf;
        }

	//////////////////
	//  Operations  //
	//////////////////

	/**
	 * @return true if the bounds define an empty interval, false otherwise
	 */
	bool empty() const
        {
            return !(mLeftType == BoundType::INFTY || mRightType == BoundType::INFTY || left() < right() || ( left() == right() && mLeftType != BoundType::STRICT && mRightType != BoundType::STRICT ));
        }

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
	Sign sgn() const
        {
            if( unbounded() )
            {
                return Sign::ZERO;
            }
            else if( (mLeftType == BoundType::STRICT && left() >= 0) || (mLeftType == BoundType::WEAK && left() > 0) )
            {
                return Sign::POSITIVE;
            }
            else if( (mRightType == BoundType::STRICT && right() <= 0) || (mRightType == BoundType::WEAK && right() < 0) )
            {
                return Sign::NEGATIVE;
            }
            else
            {
                return Sign::ZERO;
            }
        }

	/**
	 * @param n
	 * @return true in case n is contained in this Interval
	 */
	bool contains(const float_t& n) const
        {
            switch( mLeftType )
            {
                case BoundType::INFTY:
                    break;
                case BoundType::STRICT:
                    if( left() >= n )
                        return false;
                    break;
                case BoundType::WEAK:
                    if( left() > n )
                        return false;
            }
            // Invariant: n is not conflicting with left bound
            switch( mRightType )
            {
                case BoundType::INFTY:
                    break;
                case BoundType::STRICT:
                    if( right() <= n )
                        return false;
                    break;
                case BoundType::WEAK:
                    if( right() < n )
                        return false;
                    break;
            }
            return true;    // for open intervals: (left() < n && right() > n) || (n == 0 && left() == cln::cl_RA( 0 ) && right() == cln::cl_RA( 0 ))
        }

	/**
	 * @param o
	 * @return true in case o is a subset of this Interval
	 */
	bool contains(const FloatInterval<FloatImplementation>& o) const
        {
            switch( mLeftType )
            {
                case BoundType::INFTY:
                    break;
                default:
                    if( left() > o.left() )
                        return false;
                case BoundType::STRICT:
                    if( left() == o.left() && o.mLeftType != BoundType::STRICT )
                        return false;
                    break;
            }
            // Invariant: left bound of o is not conflicting with left bound
            switch( mRightType )
            {
                case BoundType::INFTY:
                    break;
                default:
                    if( right() < o.right() )
                        return false;
                case BoundType::STRICT:
                    if( right() == o.right() && o.mRightType != BoundType::STRICT )
                        return false;
                    break;
            }
            return true;    // for open intervals: left() <= o.left() && right() >= o.mRight
        }

	/**
	 * Checks whether n is contained in the <b>closed</b> interval defined by the bounds.
	 * @param n
	 * @return true in case n meets the interval bounds or a point inbetween
	 */
	bool meets(float_t n) const
        {
            return left() <= n && right() >= n;
        }

	/**
	 * @param o
	 * @return intersection with the given Interval and this Interval, or (0, 0) in case the intersection is empty
	 */
	FloatInterval<FloatImplementation> intersect(const FloatInterval<FloatImplementation>& o) const
        {
            float_t lowerValue;
            float_t upperValue;
            BoundType maxLowest;
            BoundType minUppest;
            // determine value first by: LowerValue = max ( lowervalues ) where max considers infty.
            if ( mLeftType != BoundType::INFTY && o.leftType() != BoundType::INFTY )
            {
                if ( left() < o.left() )
                {
                    lowerValue = o.left();
                    maxLowest = o.leftType();
                }
                else if ( o.left() < left() )
                {
                    lowerValue = left();
                    maxLowest = mLeftType;
                }
                else
                {
                    lowerValue = left();
                    maxLowest = getWeakestBoundType(mLeftType, o.leftType());
                }
            }
            else if ( mLeftType == BoundType::INFTY && o.leftType() != BoundType::INFTY )
            {
                lowerValue = o.left();
                maxLowest = o.leftType();
            }
            else if ( mLeftType != BoundType::INFTY && o.leftType() == BoundType::INFTY )
            {
                lowerValue = left();
                maxLowest = mLeftType;
            }
            else
            {
                lowerValue = 0;
                maxLowest = BoundType::INFTY;
            }

            // determine value first by: UpperValue = min ( uppervalues ) where min considers infty.
            if ( mRightType != BoundType::INFTY && o.rightType() != BoundType::INFTY )
            {
                if ( right() > o.right() )
                {
                    upperValue = o.right();
                    minUppest = o.rightType();
                }
                else if ( o.right() > right() )
                {
                    upperValue = right();
                    minUppest = mRightType;
                }
                else
                {
                    upperValue = right();
                    minUppest = getWeakestBoundType(mRightType, o.rightType());
                }
                if( maxLowest == BoundType::INFTY )
                {
                    lowerValue = upperValue;
                }
            }
            else if ( mRightType == BoundType::INFTY && o.rightType() != BoundType::INFTY )
            {
                upperValue = o.right();
                minUppest = o.rightType();
                if( maxLowest == BoundType::INFTY )
                {
                    lowerValue = upperValue;
                }
            }
            else if ( mRightType != BoundType::INFTY && o.rightType() == BoundType::INFTY )
            {
                upperValue = right();
                minUppest = mRightType;
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
            if ( lowerValue > upperValue )
                return emptyInterval();
            return FloatInterval<FloatImplementation>( lowerValue, maxLowest, upperValue, minUppest );
        }

	/**
	 * @return the midpoint of this interval
	 */
	float_t midpoint() const
        {
            float_t res(0);
            res.add_assign(left());
            res.add_assign(right());
            res.div_assign((float_t)2);
            res = getWeakestBoundType( mLeftType, mRightType ) == BoundType::INFTY ? 0.0;
            if( res < left() ) return left();
            if( res > right() ) return right();
            return res;
        }

	/**
	 * Computes the absolute value of this interval, i.e. the maximum of the absolute values of its bounds.
	 * @return absolute value of the interval
	 * @see Marc Daumas, Guillaume Melquiond, and Cesar Munoz - "Guaranteed Proofs Using Interval Arithmetic".
	 */
	FloatInterval<FloatImplementation> abs() const
        {
            float_t res;
            BoundType rbt = ( mLeftType != BoundType::INFTY && mRightType != BoundType::INFTY ) ? (std::abs( left() ) <= std::abs( right() ) ? mRightType : mLeftType ) : BoundType::INFTY;
            BoundType lbt = ( mLeftType == BoundType::STRICT && left() >= 0 ) ? BoundType::STRICT : BoundType::WEAK;
            return DoubleInterval( boost::numeric::abs( mInterval ), lbt, rbt );
        }

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
	static float_t roundDown(const Rational& o, bool overapproximate = false);

	/** Returns a up-rounded representation of the given numeric
	 * @param numeric o
	 * @param bool overapproximate
	 * @return double representation of o (overapprox) Note, that it can return the double INFINITY.
	 */
	template<typename Rational>
	static float_t roundUp(const Rational& o, bool overapproximate = false);

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
