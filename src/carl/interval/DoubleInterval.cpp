/*
 * GiNaCRA - GiNaC Real Algebra package
 * Copyright (C) 2010-2012  Ulrich Loup, Joachim Redies, Sebastian Junges
 *
 * This file is part of GiNaCRA.
 *
 * GiNaCRA is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GiNaCRA is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GiNaCRA.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


/**
 * @file DoubleInterval.cpp
 *
 * @author Stefan Schupp
 * @author Ulrich Loup
 * @author Sebastian Junges
 * @since 2012-10-11
 * @version 2013-08-23
 */

#include "DoubleInterval.h"

#include <cfloat>
#include <cmath>

using namespace boost::numeric;

namespace carl
{
    //////////////////////////////////
    //  Constructors & Destructors  //
    //////////////////////////////////

    DoubleInterval::DoubleInterval()
    {
        mInterval = BoostDoubleInterval( 0 );
        mLeftType  = BoundType::INFTY;
        mRightType = BoundType::INFTY;
    }

    DoubleInterval::DoubleInterval( const double& n ):
        DoubleInterval( n, BoundType::WEAK, n, BoundType::WEAK )
    {}

    DoubleInterval::DoubleInterval( const BoostDoubleInterval& _interval ):
        DoubleInterval( _interval, ( _interval.lower() == INFINITY ? BoundType::INFTY : BoundType::WEAK), ( _interval.upper() == INFINITY ? BoundType::INFTY : BoundType::WEAK) )
    {}

    DoubleInterval::DoubleInterval( const double& left, const double& right ):
        DoubleInterval( left, ( left == INFINITY ? BoundType::INFTY : BoundType::WEAK), right, ( right == INFINITY ? BoundType::INFTY : BoundType::WEAK) )
    {}

    DoubleInterval::DoubleInterval( const BoostDoubleInterval& _interval, BoundType leftType, BoundType rightType ):
        mInterval( _interval ),
        mLeftType( leftType ),
        mRightType( rightType )
    {
        assert( _interval.lower() != NAN && _interval.lower() != -INFINITY && _interval.upper() != NAN && _interval.upper() != INFINITY );
        if( mLeftType == BoundType::INFTY && mRightType == BoundType::INFTY )
        {
            mInterval = BoostDoubleInterval( 0 );
        }
        else if( mLeftType == BoundType::INFTY )
        {
            mInterval = BoostDoubleInterval( _interval.upper() );
        }
        else if( mRightType == BoundType::INFTY )
        {
            mInterval = BoostDoubleInterval( _interval.lower() );
        }
        else if( (_interval.lower() == _interval.upper() && leftType != rightType) )
        {
            mLeftType = BoundType::STRICT;
            mRightType = BoundType::STRICT;
            mInterval = BoostDoubleInterval( 0 );
        }
        else
        {
            mInterval = BoostDoubleInterval( _interval );
        }
    }

    DoubleInterval::DoubleInterval( const double left, BoundType leftType, const double right, BoundType rightType ):
        mLeftType( leftType ),
        mRightType( rightType )
    {
        assert( left != NAN && left != INFINITY && right != NAN && right != -INFINITY );
        if( left == -INFINITY ) mLeftType = BoundType::INFTY;
        if( right == INFINITY ) mRightType = BoundType::INFTY;
        if( mLeftType == BoundType::INFTY && mRightType == BoundType::INFTY )
        {
            mInterval = BoostDoubleInterval( 0 );
        }
        else if( mLeftType == BoundType::INFTY )
        {
            mInterval = BoostDoubleInterval( right );
        }
        else if( mRightType == BoundType::INFTY )
        {
            mInterval = BoostDoubleInterval( left );
        }
        else if( (left == right && (leftType == BoundType::STRICT || rightType == BoundType::STRICT)) || left > right )
        {
            mLeftType = BoundType::STRICT;
            mRightType = BoundType::STRICT;
            mInterval = BoostDoubleInterval( 0 );
        }
        else
        {
            mInterval = BoostDoubleInterval( left, right );
        }
    }


    DoubleInterval::~DoubleInterval(){}

    ///////////////////////
    //  Getter & Setter  //
    ///////////////////////

    void DoubleInterval::cutUntil( const double& _left )
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

    void DoubleInterval::cutFrom( const double& _right )
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

    //////////////////
    //  Arithmetic  //
    //////////////////

    DoubleInterval DoubleInterval::add( const DoubleInterval& o ) const
    {
        return DoubleInterval( mInterval + o.content(),
                              getWeakestBoundType( mLeftType, o.mLeftType ),
                              getWeakestBoundType( mRightType, o.mRightType ) );
    }

    DoubleInterval DoubleInterval::minus() const
    {
        return DoubleInterval( -right(), mRightType, -left(), mLeftType );
    }

    DoubleInterval DoubleInterval::sqrt() const
    {
        if( mRightType != BoundType::INFTY && right() < 0 )
        {
            return emptyInterval();
        }
        double lvalue = (mLeftType == BoundType::INFTY || left() < 0) ? 0 : left();
        double rvalue = (mRightType == BoundType::INFTY || right() < 0) ? 0 : right();
        if( lvalue > rvalue && mRightType == BoundType::INFTY ) rvalue = lvalue;
        BoostDoubleInterval content = boost::numeric::sqrt( BoostDoubleInterval( lvalue, rvalue ) );
        BoundType leftType = mLeftType;
        BoundType rightType = mRightType;
        if( mLeftType == BoundType::INFTY || left() < 0 )
        {
            leftType = BoundType::WEAK;
        }
        return DoubleInterval( content, leftType, rightType );
    }

    DoubleInterval DoubleInterval::mul( const DoubleInterval& _interval ) const
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

    DoubleInterval DoubleInterval::div( const DoubleInterval& _interval ) const throw ( std::invalid_argument )
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

    bool DoubleInterval::div_ext( DoubleInterval& a, DoubleInterval& b, const DoubleInterval& o )
    {
        DoubleInterval inverseA, inverseB;
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
                splitOccured = o.inverse( inverseA, inverseB );
                if( !splitOccured )
                {
                    a = this->mul( inverseA );
                    return false;
                }
                else
                {
                    a = this->mul( inverseA );
                    b = this->mul( inverseB );

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

    DoubleInterval DoubleInterval::power( unsigned _exp ) const
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
                    return DoubleInterval( boost::numeric::pow( mInterval, (int)_exp ), mRightType, BoundType::INFTY );
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
                    return DoubleInterval( boost::numeric::pow( mInterval, (int)_exp ), mLeftType, BoundType::INFTY );
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
                    return DoubleInterval( boost::numeric::pow( mInterval, (int)_exp ), BoundType::WEAK, rType );
                }
                else
                {
                    return DoubleInterval( boost::numeric::pow( mInterval, (int)_exp ), lType, rType );
                }
            }
        }
        else
        {
            return DoubleInterval( boost::numeric::pow( mInterval, (int)_exp ), mLeftType, mRightType );
        }
    }

    bool DoubleInterval::inverse( DoubleInterval& a, DoubleInterval& b ) const
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
                a = DoubleInterval( 0, BoundType::INFTY, 0, BoundType::WEAK );
                b = DoubleInterval( BoostDoubleInterval( 1 ) / BoostDoubleInterval( right() ), BoundType::WEAK, BoundType::INFTY );
            }
            else if( mRightType == BoundType::INFTY )
            {
                a = DoubleInterval( BoostDoubleInterval( 1 ) / BoostDoubleInterval( left() ), BoundType::INFTY, BoundType::WEAK );
                b = DoubleInterval( 0, BoundType::WEAK, 0, BoundType::INFTY );
            }
            else if( left() == 0 && right() != 0 )
            {
                a = DoubleInterval( 0, BoundType::INFTY, 0, BoundType::INFTY );
                b = DoubleInterval( BoostDoubleInterval( 1 ) / BoostDoubleInterval( right() ), BoundType::WEAK, BoundType::INFTY );
            }
            else if( left() != 0 && right() == 0 )
            {
                a = DoubleInterval( BoostDoubleInterval( 1 ) / BoostDoubleInterval( left() ), BoundType::INFTY, BoundType::WEAK );
                b = unboundedInterval(); // todo: really the whole interval here?
            }
            else if( left() == 0 && right() == 0 )
            {
                a = unboundedInterval();
                return false;
            }
            else
            {
                a = DoubleInterval( BoostDoubleInterval( 1 ) / BoostDoubleInterval( left() ), BoundType::INFTY, BoundType::WEAK );
                b = DoubleInterval( BoostDoubleInterval( 1 ) / BoostDoubleInterval( right() ), BoundType::WEAK, BoundType::INFTY );
            }
            return true;
        }
        else
        {
            if( mLeftType == BoundType::INFTY && right() != 0 )
            {
                a = DoubleInterval(  1 / right() , mRightType, 0,  BoundType::WEAK );
            }
            else if( mLeftType == BoundType::INFTY && right() == 0 )
            {
                a = DoubleInterval( 0, BoundType::INFTY, 0, BoundType::WEAK );
            }
            else if( mRightType == BoundType::INFTY && left() != 0 )
            {
                a = DoubleInterval(  0 , BoundType::WEAK, 1  /  left(), mLeftType );
            }
            else if( mRightType == BoundType::INFTY && left() == 0 )
            {
                a = DoubleInterval( 0, BoundType::WEAK, 0, BoundType::INFTY );
            }
            else if( left() != 0 && right() != 0 )
            {
                a = DoubleInterval( BoostDoubleInterval( 1 ) / mInterval, mRightType, mLeftType );
            }
            else if( left() == 0 && right() != 0 )
            {
                a = DoubleInterval( BoostDoubleInterval( 1 ) / BoostDoubleInterval( right() ), mRightType, BoundType::INFTY );
            }
            else if( left() != 0 && right() == 0 )
            {
                a = DoubleInterval( BoostDoubleInterval( 1 ) / BoostDoubleInterval( left() ), BoundType::INFTY, mLeftType );
            }

            return false;
        }
    }


    double DoubleInterval::diameter() const
    {
        assert( DOUBLE_BOUNDS_OK( left(), mLeftType, right(), mRightType ));
        if( mLeftType == BoundType::INFTY || mRightType == BoundType::INFTY )
        {
            return -1;
        }
        return right() - left();
    }
    
    bool DoubleInterval::empty() const
    {
        return !(mLeftType == BoundType::INFTY || mRightType == BoundType::INFTY || left() < right() || ( left() == right() && mLeftType != BoundType::STRICT && mRightType != BoundType::STRICT ));
    }

    //////////////////
    //  Operations  //
    //////////////////

    Sign DoubleInterval::sgn()
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

    bool DoubleInterval::contains( const double& n ) const
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

    bool DoubleInterval::contains( const DoubleInterval& o ) const
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

    bool DoubleInterval::meets( double n ) const
    {
        return left() <= n && right() >= n;
    }

    DoubleInterval DoubleInterval::intersect( const DoubleInterval& o ) const
    {
        double lowerValue;
        double upperValue;
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
        return DoubleInterval(lowerValue, maxLowest, upperValue, minUppest );
        
//        if( (right() < o.left() && mRightType != BoundType::INFTY && o.mLeftType != BoundType::INFTY)
//                || (o.right() < left() && mLeftType != BoundType::INFTY && o.mRightType != BoundType::INFTY) )    // intersection empty
//            return DoubleInterval( 0, BoundType::STRICT, 0, BoundType::STRICT );
//        // Invariant: ( right() >= o.left() || mRightType == BoundType::INFTY && o.mLeftType == BoundType::INFTY ) && ( o.right() >= left() || mLeftType == BoundType::INFTY && o.mRightType == BoundType::INFTY )
//        BoundType leftBoundType  = (mLeftType == BoundType::INFTY && o.mLeftType == BoundType::INFTY) ? BoundType::INFTY : BoundType::WEAK;
//        BoundType rightBoundType = (mRightType == BoundType::INFTY && o.mRightType == BoundType::INFTY) ? BoundType::INFTY : BoundType::WEAK;
//        if( o.mLeftType == BoundType::INFTY || (left() > o.left() && mLeftType != BoundType::INFTY && o.mLeftType != BoundType::INFTY ) || (left() == o.left() && mLeftType != BoundType::STRICT && o.mLeftType != BoundType::STRICT) )
//        {    // left() can be safely taken as weak bound of the intersection or is infinity
//            if( o.mRightType == BoundType::INFTY || (mRightType != BoundType::INFTY && right() < o.right() ) )
//                return DoubleInterval( left(), leftBoundType, right(), rightBoundType );    // right() can be safely taken as weak bound of the intersection
//            else if( mRightType == BoundType::INFTY || right() > o.right() )
//                return DoubleInterval( left(), leftBoundType, o.right(), rightBoundType );    // o.right() can be safely taken as weak bound of the intersection
//            // Invariant: right() == o.mRight
//            if( mRightType == BoundType::STRICT || o.mRightType == BoundType::STRICT )
//                return DoubleInterval( left(), leftBoundType, right(), BoundType::STRICT );    // the new right type has to be strict
//            return DoubleInterval( left(), leftBoundType, right(), rightBoundType );
//        }
//        if( mLeftType == BoundType::INFTY || left() < o.left() )
//        {    // o.left() can be safely taken as weak bound of the intersection
//            if( o.mRightType == BoundType::INFTY || (mRightType != BoundType::INFTY && right() < o.right() ) )
//                return DoubleInterval( o.left(), leftBoundType, right(), rightBoundType );    // right() can be safely taken as weak bound of the intersection
//            else if( mRightType == BoundType::INFTY || right() > o.right() )
//                return DoubleInterval( o.left(), leftBoundType, o.right(), rightBoundType );    // o.right() can be safely taken as weak bound of the intersection
//            // Invariant: right() == o.mRight
//            if( mRightType == BoundType::STRICT || o.mRightType == BoundType::STRICT )
//                return DoubleInterval( o.left(), leftBoundType, right(), BoundType::STRICT );    // the new right type has to be strict
//            return DoubleInterval( o.left(), leftBoundType, right(), rightBoundType );
//        }
//        // Invariant: left() == o.left() && ( mLeftType == BoundType::STRICT || o.mLeftType == BoundType::STRICT )
//        assert( mLeftType == BoundType::STRICT || o.mLeftType == BoundType::STRICT );
//        // the new left type has to be strict
//        if( o.mRightType == BoundType::INFTY || right() < o.right() )
//            return DoubleInterval( left(), BoundType::STRICT, right(), rightBoundType );    // right() can be safely taken as weak bound of the intersection
//        else if( mRightType == BoundType::INFTY || right() > o.right() )
//            return DoubleInterval( left(), BoundType::STRICT, o.right(), rightBoundType );    // o.right() can be safely taken as weak bound of the intersection
//        // Invariant: right() == o.mRight
//        if( mRightType == BoundType::STRICT || o.mRightType == BoundType::STRICT )
//            return DoubleInterval( left(), BoundType::STRICT, right(), BoundType::STRICT );    // the new right type has to be strict
//        return DoubleInterval( left(), BoundType::STRICT, right(), rightBoundType );
    }

    double DoubleInterval::midpoint() const
    {
        double midpoint = getWeakestBoundType( mLeftType, mRightType ) == BoundType::INFTY ? 0.0 : (left() + right() ) / 2.0;
        if( midpoint < left() ) return left();
        if( midpoint > right() ) return right();
        return midpoint;
    }

    DoubleInterval DoubleInterval::abs() const
    {
        BoundType rbt = ( mLeftType != BoundType::INFTY && mRightType != BoundType::INFTY ) ? (std::abs( left() ) <= std::abs( right() ) ? mRightType : mLeftType ) : BoundType::INFTY;
        BoundType lbt = ( mLeftType == BoundType::STRICT && left() >= 0 ) ? BoundType::STRICT : BoundType::WEAK;
        return DoubleInterval( boost::numeric::abs( mInterval ), lbt, rbt );
    }

	void DoubleInterval::operator+=( const DoubleInterval& _interval )
	{
		mInterval += _interval.content();
        mLeftType = getWeakestBoundType( mLeftType, _interval.leftType() );
        mRightType = getWeakestBoundType( mRightType, _interval.rightType() );
	}

	void DoubleInterval::operator-=( const DoubleInterval& _interval )
	{
		mInterval -= _interval.content();
        mLeftType = getWeakestBoundType( mLeftType, _interval.rightType() );
        mRightType = getWeakestBoundType( mRightType, _interval.leftType() );
	}

	void DoubleInterval::operator*=( const DoubleInterval& _interval )
	{
		mInterval *= _interval.content();
//        mLeftType = mInterval.lower() == Checking::neg_inf() ? BoundType::INFTY : BoundType::WEAK;
//        mLeftType = mInterval.upper() == Checking::pos_inf() ? BoundType::INFTY : BoundType::WEAK;
        mLeftType = getWeakestBoundType( (mLeftType == BoundType::STRICT ? BoundType::WEAK : mLeftType), (_interval.leftType() == BoundType::STRICT ? BoundType::WEAK : _interval.leftType() ));
        mLeftType = getWeakestBoundType( (mRightType == BoundType::STRICT ? BoundType::WEAK : mRightType), (_interval.rightType() == BoundType::STRICT ? BoundType::WEAK : _interval.rightType() ));
	}

    ///////////////////////////
    // Relational Operations //
    ///////////////////////////

    bool DoubleInterval::isEqual( const DoubleInterval& _interval ) const
    {
        if( mLeftType != _interval.leftType() || mRightType != _interval.rightType() )
        {
            return false;
        }
        else
        {
//            return ( ( mInterval == _interval.content() ) == True );
            return boost::numeric::equal(mInterval, _interval.content());
        }
    }

    bool DoubleInterval::isLessOrEqual( const DoubleInterval& o ) const
    {
        /**  -----|------------|------------    <=
         *  ----------|------------|-------
         * or
         *  -----|------------|------------ <=
         *  ----------|-----|-------------- holds.
         */
        // only compare left bounds
        switch( mLeftType )
        {
            case BoundType::INFTY:
                return o.mLeftType == BoundType::INFTY;
            default:
                return (left() <= o.left() );
        }
    }

    bool DoubleInterval::isGreaterOrEqual( const DoubleInterval& o ) const
    {
        /**  ----------]------------[-------    >=
         *  -----]------------[------------
         * or
         *  ----------]------------[------- >=
         *  -------------]----[------------ holds.
         */
        // only compare right bounds
        switch( mRightType )
        {
            case BoundType::INFTY:
                return o.mRightType == BoundType::INFTY;
            default:
                return (right() >= o.right() );
        }
    }

    void DoubleInterval::dbgprint() const
    {
        std::cout.precision( 30 );
        if( mLeftType == BoundType::INFTY )
            std::cout << "]-infinity";
        else
            std::cout << (mLeftType == BoundType::STRICT ? "]" : "[") << left();
        std::cout << ", ";
        if( mRightType == BoundType::INFTY )
            std::cout << "infinity[";
        else
            std::cout << right() << (mRightType == BoundType::WEAK ? "]" : "[") << std::endl;
        std::cout.precision( 0 );
    }

    ////////////////////
    // Static Methods //
    ////////////////////


    ////////////////////////////
    //  Auxiliary Functions:  //
    ////////////////////////////

//    double DoubleInterval::roundDown( const cln::cl_RA& o, bool overapproximate )
//    {
//        double result = cln::double_approx(o);
//        if( result == -INFINITY ) return result;
//        if( result == INFINITY ) return DBL_MAX;
//        // If the cln::cl_RA cannot be represented exactly by a double, round.
//        if( overapproximate || cln::rationalize( cln::cl_R( result ) ) != o )
//        {
//            if( result == -DBL_MAX ) return -INFINITY;
//            return std::nextafter( result, -INFINITY );
//        }
//        else
//        {
//            return result;
//        }
//    }
//
//    double DoubleInterval::roundUp( const cln::cl_RA& o, bool overapproximate )
//    {
//        double result = cln::double_approx(o);
//        if( result == INFINITY ) return result;
//        if( result == -INFINITY ) return -DBL_MAX;
//        // If the cln::cl_RA cannot be represented exactly by a double, round.
//        if( overapproximate || cln::rationalize( cln::cl_R( result ) ) != o )
//        {
//            if( result == DBL_MAX ) return INFINITY;
//            return std::nextafter( result, INFINITY );
//        }
//        else
//        {
//            return result;
//        }
//    }

    // unary arithmetic operators of DoubleInterval
    const DoubleInterval DoubleInterval::operator -( const DoubleInterval& lh ) const
    {
        return lh.minus();
    }
    
    std::ostream& operator << (std::ostream& str, const DoubleInterval& d)
    {
        if( d.leftType() == BoundType::INFTY )
            str << "]-infinity";
        else
        {
            str.precision( 30 );
            str << (d.leftType() == BoundType::STRICT ? "]" : "[") << d.left();
            str.precision( 0 );
        }
        str << ", ";
        if( d.rightType() == BoundType::INFTY )
            str << "infinity[";
        else
        {
            str.precision( 30 );
            str << d.right() << (d.rightType() == BoundType::WEAK ? "]" : "[");
            str.precision( 0 );
        }
        return str;
    }
}

