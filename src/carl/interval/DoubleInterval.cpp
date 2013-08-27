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
        mLeftType  = INFINITY_BOUND;
        mRightType = INFINITY_BOUND;
    }

    DoubleInterval::DoubleInterval( const double& n ):
        DoubleInterval( n, WEAK_BOUND, n, WEAK_BOUND )
    {}

    DoubleInterval::DoubleInterval( const cln::cl_RA& n, bool overapproximate ):
        DoubleInterval( n, WEAK_BOUND, n, WEAK_BOUND, overapproximate, overapproximate )
    {}
    
    

    DoubleInterval::DoubleInterval( const BoostDoubleInterval& _interval ):
        DoubleInterval( _interval, ( _interval.lower() == INFINITY ? INFINITY_BOUND : WEAK_BOUND), ( _interval.upper() == INFINITY ? INFINITY_BOUND : WEAK_BOUND) )
    {}

    DoubleInterval::DoubleInterval( const double& left, const double& right ):
        DoubleInterval( left, ( left == INFINITY ? INFINITY_BOUND : WEAK_BOUND), right, ( right == INFINITY ? INFINITY_BOUND : WEAK_BOUND) )
    {}

    DoubleInterval::DoubleInterval( const BoostDoubleInterval& _interval, BoundType leftType, BoundType rightType ):
        mInterval( _interval ),
        mLeftType( leftType ),
        mRightType( rightType )
    {
        assert( _interval.lower() != NAN && _interval.lower() != -INFINITY && _interval.upper() != NAN && _interval.upper() != INFINITY );
        if( mLeftType == INFINITY_BOUND && mRightType == INFINITY_BOUND )
        {
            mInterval = BoostDoubleInterval( 0 );
        }
        else if( mLeftType == INFINITY_BOUND )
        {
            mInterval = BoostDoubleInterval( _interval.upper() );
        }
        else if( mRightType == INFINITY_BOUND )
        {
            mInterval = BoostDoubleInterval( _interval.lower() );
        }
        else if( (_interval.lower() == _interval.upper() && leftType != rightType) )
        {
            mLeftType = STRICT_BOUND;
            mRightType = STRICT_BOUND;
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
        if( left == -INFINITY ) mLeftType = INFINITY_BOUND;
        if( right == INFINITY ) mRightType = INFINITY_BOUND;
        if( mLeftType == INFINITY_BOUND && mRightType == INFINITY_BOUND )
        {
            mInterval = BoostDoubleInterval( 0 );
        }
        else if( mLeftType == INFINITY_BOUND )
        {
            mInterval = BoostDoubleInterval( right );
        }
        else if( mRightType == INFINITY_BOUND )
        {
            mInterval = BoostDoubleInterval( left );
        }
        else if( (left == right && (leftType == STRICT_BOUND || rightType == STRICT_BOUND)) || left > right )
        {
            mLeftType = STRICT_BOUND;
            mRightType = STRICT_BOUND;
            mInterval = BoostDoubleInterval( 0 );
        }
        else
        {
            mInterval = BoostDoubleInterval( left, right );
        }
    }

    DoubleInterval::DoubleInterval( const cln::cl_RA& left, BoundType leftType, const cln::cl_RA& right, BoundType rightType, bool overapproxleft, bool overapproxright ):
        mLeftType( leftType ),
        mRightType( rightType )
    {
        double dLeft = roundDown( left, overapproxleft );
        double dRight = roundUp( right, overapproxright );
        if( dLeft == -INFINITY ) mLeftType = INFINITY_BOUND;
        if( dRight == INFINITY ) mRightType = INFINITY_BOUND;
        if( mLeftType == INFINITY_BOUND && mRightType == INFINITY_BOUND )
        {
            mInterval =  BoostDoubleInterval( 0 );
        }
        else if( mLeftType == INFINITY_BOUND )
        {
            mInterval =  BoostDoubleInterval( dRight );
        }
        else if( mRightType == INFINITY_BOUND )
        {
            mInterval =  BoostDoubleInterval( dLeft );
        }
        else if( (left == right && leftType != rightType) || left > right )
        {
            mLeftType = STRICT_BOUND;
            mRightType = STRICT_BOUND;
            mInterval = BoostDoubleInterval( 0 );
        }
        else
        {
            mInterval =  BoostDoubleInterval( dLeft, dRight );
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
            mLeftType = WEAK_BOUND;
        }
        else if( _left > left() )
        {
            mLeftType   = STRICT_BOUND;
            mRightType  = STRICT_BOUND;
            mInterval = BoostDoubleInterval( 0 );
        }
    }

    void DoubleInterval::cutFrom( const double& _right )
    {
        if( _right >= left() && _right < right() )
        {
            setRight( _right );
            mRightType = WEAK_BOUND;
        }
        else if( _right < left() )
        {
            mLeftType   = STRICT_BOUND;
            mRightType  = STRICT_BOUND;
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
        if( mRightType != INFINITY_BOUND && right() < 0 )
        {
            return emptyInterval();
        }
        double lvalue = (mLeftType == INFINITY_BOUND || left() < 0) ? 0 : left();
        double rvalue = (mRightType == INFINITY_BOUND || right() < 0) ? 0 : right();
        if( lvalue > rvalue && mRightType == INFINITY_BOUND ) rvalue = lvalue;
        BoostDoubleInterval content = boost::numeric::sqrt( BoostDoubleInterval( lvalue, rvalue ) );
        BoundType leftType = mLeftType;
        BoundType rightType = mRightType;
        if( mLeftType == INFINITY_BOUND || left() < 0 )
        {
            leftType = WEAK_BOUND;
        }
        return DoubleInterval( content, leftType, rightType );
    }

    DoubleInterval DoubleInterval::mul( const DoubleInterval& _interval ) const
    {
        BoundType leftType = WEAK_BOUND;
        BoundType rightType = WEAK_BOUND;
        if( (mLeftType == INFINITY_BOUND && (_interval.right() > 0 || _interval.mRightType == INFINITY_BOUND))
            || (mRightType == INFINITY_BOUND && (_interval.left() < 0 || _interval.mLeftType == INFINITY_BOUND))
            || (_interval.mLeftType == INFINITY_BOUND && (right() > 0 || mRightType == INFINITY_BOUND))
            || (_interval.mRightType == INFINITY_BOUND && (right() < 0 || (left() < 0 || mLeftType == INFINITY_BOUND))) )
        {
            leftType = INFINITY_BOUND;
        }
        if( (mLeftType == INFINITY_BOUND && (_interval.right() < 0 || (_interval.left() < 0 || _interval.mLeftType == INFINITY_BOUND)))
            || (mRightType == INFINITY_BOUND && (_interval.left() > 0 || (_interval.right() > 0 || _interval.mRightType == INFINITY_BOUND)))
            || (_interval.mLeftType == INFINITY_BOUND && (right() < 0 || (left() < 0 || mLeftType == INFINITY_BOUND)))
            || (_interval.mRightType == INFINITY_BOUND && (left() > 0 || (right() > 0 || mRightType == INFINITY_BOUND))) )
        {
            rightType = INFINITY_BOUND;
        }
        return DoubleInterval( BoostDoubleInterval( mInterval*_interval.content() ), leftType, rightType );
    }

    DoubleInterval DoubleInterval::div( const DoubleInterval& _interval ) const throw ( std::invalid_argument )
    {
        if( _interval.contains( 0 ) ) throw ( std::invalid_argument( "Division by interval containing zero not allowed." ) );
        BoundType leftType = WEAK_BOUND;
        BoundType rightType = WEAK_BOUND;
        if( (mLeftType == INFINITY_BOUND && (_interval.right() > 0 || _interval.mRightType == INFINITY_BOUND))
            || (mRightType == INFINITY_BOUND && (_interval.left() < 0 || _interval.mLeftType == INFINITY_BOUND))
            || (_interval.mLeftType == INFINITY_BOUND && (right() > 0 || mRightType == INFINITY_BOUND))
            || (_interval.mRightType == INFINITY_BOUND && (right() < 0 || (left() < 0 || mLeftType == INFINITY_BOUND))) )
        {
            leftType = INFINITY_BOUND;
        }
        if( (mLeftType == INFINITY_BOUND && (_interval.right() < 0 || (_interval.left() < 0 || _interval.mLeftType == INFINITY_BOUND)))
            || (mRightType == INFINITY_BOUND && (_interval.left() > 0 || (_interval.right() > 0 || _interval.mRightType == INFINITY_BOUND)))
            || (_interval.mLeftType == INFINITY_BOUND && (right() < 0 || (left() < 0 || mLeftType == INFINITY_BOUND)))
            || (_interval.mRightType == INFINITY_BOUND && (left() > 0 || (right() > 0 || mRightType == INFINITY_BOUND))) )
        {
            rightType = INFINITY_BOUND;
        }
        return DoubleInterval( BoostDoubleInterval( mInterval/_interval.content() ), leftType, rightType );
    }

    bool DoubleInterval::div_ext( DoubleInterval& a, DoubleInterval& b, const DoubleInterval& o )
    {
        DoubleInterval inverseA, inverseB;
        bool          splitOccured;

        if( o.leftType() != INFINITY_BOUND && o.left() == 0 && o.rightType() != INFINITY_BOUND && o.right() == 0 )    // point interval 0
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

    DoubleInterval DoubleInterval::power( int _exp ) const
    {
        if( _exp % 2 == 0 )
        {
            if( mLeftType == INFINITY_BOUND && mRightType == INFINITY_BOUND )
            {
                return DoubleInterval();
            }
            else if( mLeftType == INFINITY_BOUND )
            {
                if( contains( 0 ) )
                {
                    return DoubleInterval( 0, WEAK_BOUND, 0, INFINITY_BOUND );
                }
                else
                {
                    return DoubleInterval( boost::numeric::pow( mInterval, _exp ), mRightType, INFINITY_BOUND );
                }
            }
            else if( mRightType == INFINITY_BOUND )
            {
                if( contains( 0 ) )
                {
                    return DoubleInterval( 0, WEAK_BOUND, 0, INFINITY_BOUND );
                }
                else
                {
                    return DoubleInterval( boost::numeric::pow( mInterval, _exp ), mLeftType, INFINITY_BOUND );
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
                    return DoubleInterval( boost::numeric::pow( mInterval, _exp ), WEAK_BOUND, rType );
                }
                else
                {
                    return DoubleInterval( boost::numeric::pow( mInterval, _exp ), lType, rType );
                }
            }
        }
        else
        {
            return DoubleInterval( boost::numeric::pow( mInterval, _exp ), mLeftType, mRightType );
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
            if( mLeftType == INFINITY_BOUND )
            {
                a = DoubleInterval( 0, INFINITY_BOUND, 0, WEAK_BOUND );
                b = DoubleInterval( BoostDoubleInterval( 1 ) / BoostDoubleInterval( right() ), WEAK_BOUND, INFINITY_BOUND );
            }
            else if( mRightType == INFINITY_BOUND )
            {
                a = DoubleInterval( BoostDoubleInterval( 1 ) / BoostDoubleInterval( left() ), INFINITY_BOUND, WEAK_BOUND );
                b = DoubleInterval( 0, WEAK_BOUND, 0, INFINITY_BOUND );
            }
            else if( left() == 0 && right() != 0 )
            {
                a = DoubleInterval( 0, INFINITY_BOUND, 0, INFINITY_BOUND );
                b = DoubleInterval( BoostDoubleInterval( 1 ) / BoostDoubleInterval( right() ), WEAK_BOUND, INFINITY_BOUND );
            }
            else if( left() != 0 && right() == 0 )
            {
                a = DoubleInterval( BoostDoubleInterval( 1 ) / BoostDoubleInterval( left() ), INFINITY_BOUND, WEAK_BOUND );
                b = unboundedInterval(); // todo: really the whole interval here?
            }
            else if( left() == 0 && right() == 0 )
            {
                a = unboundedInterval();
                return false;
            }
            else
            {
                a = DoubleInterval( BoostDoubleInterval( 1 ) / BoostDoubleInterval( left() ), INFINITY_BOUND, WEAK_BOUND );
                b = DoubleInterval( BoostDoubleInterval( 1 ) / BoostDoubleInterval( right() ), WEAK_BOUND, INFINITY_BOUND );
            }
            return true;
        }
        else
        {
            if( mLeftType == INFINITY_BOUND && right() != 0 )
            {
                a = DoubleInterval(  1 / right() , mRightType, 0,  WEAK_BOUND );
            }
            else if( mLeftType == INFINITY_BOUND && right() == 0 )
            {
                a = DoubleInterval( 0, INFINITY_BOUND, 0, WEAK_BOUND );
            }
            else if( mRightType == INFINITY_BOUND && left() != 0 )
            {
                a = DoubleInterval(  0 , WEAK_BOUND, 1  /  left(), mLeftType );
            }
            else if( mRightType == INFINITY_BOUND && left() == 0 )
            {
                a = DoubleInterval( 0, WEAK_BOUND, 0, INFINITY_BOUND );
            }
            else if( left() != 0 && right() != 0 )
            {
                a = DoubleInterval( BoostDoubleInterval( 1 ) / mInterval, mRightType, mLeftType );
            }
            else if( left() == 0 && right() != 0 )
            {
                a = DoubleInterval( BoostDoubleInterval( 1 ) / BoostDoubleInterval( right() ), mRightType, INFINITY_BOUND );
            }
            else if( left() != 0 && right() == 0 )
            {
                a = DoubleInterval( BoostDoubleInterval( 1 ) / BoostDoubleInterval( left() ), INFINITY_BOUND, mLeftType );
            }

            return false;
        }
    }


    double DoubleInterval::diameter() const
    {
        assert( DOUBLE_BOUNDS_OK( left(), mLeftType, right(), mRightType ));
        if( mLeftType == INFINITY_BOUND || mRightType == INFINITY_BOUND )
        {
            return -1;
        }
        return right() - left();
    }
    
    bool DoubleInterval::empty() const
    {
        return !(mLeftType == INFINITY_BOUND || mRightType == INFINITY_BOUND || left() < right() || ( left() == right() && mLeftType != STRICT_BOUND && mRightType != STRICT_BOUND ));
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
        else if( (mLeftType == STRICT_BOUND && left() >= 0) || (mLeftType == WEAK_BOUND && left() > 0) )
        {
            return Sign::POSITIVE;
        }
        else if( (mRightType == STRICT_BOUND && right() <= 0) || (mRightType == WEAK_BOUND && right() < 0) )
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
            case INFINITY_BOUND:
                break;
            case STRICT_BOUND:
                if( left() >= n )
                    return false;
                break;
            case WEAK_BOUND:
                if( left() > n )
                    return false;
        }
        // Invariant: n is not conflicting with left bound
        switch( mRightType )
        {
            case INFINITY_BOUND:
                break;
            case STRICT_BOUND:
                if( right() <= n )
                    return false;
                break;
            case WEAK_BOUND:
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
            case INFINITY_BOUND:
                break;
            default:
                if( left() > o.left() )
                    return false;
            case STRICT_BOUND:
                if( left() == o.left() && o.mLeftType != STRICT_BOUND )
                    return false;
                break;
        }
        // Invariant: left bound of o is not conflicting with left bound
        switch( mRightType )
        {
            case INFINITY_BOUND:
                break;
            default:
                if( right() < o.right() )
                    return false;
            case STRICT_BOUND:
                if( right() == o.right() && o.mRightType != STRICT_BOUND )
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
        if ( mLeftType != INFINITY_BOUND && o.leftType() != INFINITY_BOUND )
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
        else if ( mLeftType == INFINITY_BOUND && o.leftType() != INFINITY_BOUND )
        {
            lowerValue = o.left();
            maxLowest = o.leftType();
        }
        else if ( mLeftType != INFINITY_BOUND && o.leftType() == INFINITY_BOUND )
        {
            lowerValue = left();
            maxLowest = mLeftType;
        }
        else
        {
            lowerValue = 0;
            maxLowest = INFINITY_BOUND;
        }
        
        // determine value first by: UpperValue = min ( uppervalues ) where min considers infty.
        if ( mRightType != INFINITY_BOUND && o.rightType() != INFINITY_BOUND )
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
            if( maxLowest == INFINITY_BOUND )
            {
                lowerValue = upperValue;
            }
        }
        else if ( mRightType == INFINITY_BOUND && o.rightType() != INFINITY_BOUND )
        {
            upperValue = o.right();
            minUppest = o.rightType();
            if( maxLowest == INFINITY_BOUND )
            {
                lowerValue = upperValue;
            }
        }
        else if ( mRightType != INFINITY_BOUND && o.rightType() == INFINITY_BOUND )
        {
            upperValue = right();
            minUppest = mRightType;
            if( maxLowest == INFINITY_BOUND )
            {
                lowerValue = upperValue;
            }
        }
        else
        {
            upperValue = lowerValue;
            minUppest = INFINITY_BOUND;
        }
        if ( lowerValue > upperValue )
            return emptyInterval();
        return DoubleInterval(lowerValue, maxLowest, upperValue, minUppest );
        
//        if( (right() < o.left() && mRightType != INFINITY_BOUND && o.mLeftType != INFINITY_BOUND)
//                || (o.right() < left() && mLeftType != INFINITY_BOUND && o.mRightType != INFINITY_BOUND) )    // intersection empty
//            return DoubleInterval( 0, STRICT_BOUND, 0, STRICT_BOUND );
//        // Invariant: ( right() >= o.left() || mRightType == INFINITY_BOUND && o.mLeftType == INFINITY_BOUND ) && ( o.right() >= left() || mLeftType == INFINITY_BOUND && o.mRightType == INFINITY_BOUND )
//        BoundType leftBoundType  = (mLeftType == INFINITY_BOUND && o.mLeftType == INFINITY_BOUND) ? INFINITY_BOUND : WEAK_BOUND;
//        BoundType rightBoundType = (mRightType == INFINITY_BOUND && o.mRightType == INFINITY_BOUND) ? INFINITY_BOUND : WEAK_BOUND;
//        if( o.mLeftType == INFINITY_BOUND || (left() > o.left() && mLeftType != INFINITY_BOUND && o.mLeftType != INFINITY_BOUND ) || (left() == o.left() && mLeftType != STRICT_BOUND && o.mLeftType != STRICT_BOUND) )
//        {    // left() can be safely taken as weak bound of the intersection or is infinity
//            if( o.mRightType == INFINITY_BOUND || (mRightType != INFINITY_BOUND && right() < o.right() ) )
//                return DoubleInterval( left(), leftBoundType, right(), rightBoundType );    // right() can be safely taken as weak bound of the intersection
//            else if( mRightType == INFINITY_BOUND || right() > o.right() )
//                return DoubleInterval( left(), leftBoundType, o.right(), rightBoundType );    // o.right() can be safely taken as weak bound of the intersection
//            // Invariant: right() == o.mRight
//            if( mRightType == STRICT_BOUND || o.mRightType == STRICT_BOUND )
//                return DoubleInterval( left(), leftBoundType, right(), STRICT_BOUND );    // the new right type has to be strict
//            return DoubleInterval( left(), leftBoundType, right(), rightBoundType );
//        }
//        if( mLeftType == INFINITY_BOUND || left() < o.left() )
//        {    // o.left() can be safely taken as weak bound of the intersection
//            if( o.mRightType == INFINITY_BOUND || (mRightType != INFINITY_BOUND && right() < o.right() ) )
//                return DoubleInterval( o.left(), leftBoundType, right(), rightBoundType );    // right() can be safely taken as weak bound of the intersection
//            else if( mRightType == INFINITY_BOUND || right() > o.right() )
//                return DoubleInterval( o.left(), leftBoundType, o.right(), rightBoundType );    // o.right() can be safely taken as weak bound of the intersection
//            // Invariant: right() == o.mRight
//            if( mRightType == STRICT_BOUND || o.mRightType == STRICT_BOUND )
//                return DoubleInterval( o.left(), leftBoundType, right(), STRICT_BOUND );    // the new right type has to be strict
//            return DoubleInterval( o.left(), leftBoundType, right(), rightBoundType );
//        }
//        // Invariant: left() == o.left() && ( mLeftType == STRICT_BOUND || o.mLeftType == STRICT_BOUND )
//        assert( mLeftType == STRICT_BOUND || o.mLeftType == STRICT_BOUND );
//        // the new left type has to be strict
//        if( o.mRightType == INFINITY_BOUND || right() < o.right() )
//            return DoubleInterval( left(), STRICT_BOUND, right(), rightBoundType );    // right() can be safely taken as weak bound of the intersection
//        else if( mRightType == INFINITY_BOUND || right() > o.right() )
//            return DoubleInterval( left(), STRICT_BOUND, o.right(), rightBoundType );    // o.right() can be safely taken as weak bound of the intersection
//        // Invariant: right() == o.mRight
//        if( mRightType == STRICT_BOUND || o.mRightType == STRICT_BOUND )
//            return DoubleInterval( left(), STRICT_BOUND, right(), STRICT_BOUND );    // the new right type has to be strict
//        return DoubleInterval( left(), STRICT_BOUND, right(), rightBoundType );
    }

    double DoubleInterval::midpoint() const
    {
        double midpoint = getWeakestBoundType( mLeftType, mRightType ) == INFINITY_BOUND ? 0.0 : (left() + right() ) / 2.0;
        if( midpoint < left() ) return left();
        if( midpoint > right() ) return right();
        return midpoint;
    }

    DoubleInterval DoubleInterval::abs() const
    {
        BoundType rbt = ( mLeftType != INFINITY_BOUND && mRightType != INFINITY_BOUND ) ? (std::abs( left() ) <= std::abs( right() ) ? mRightType : mLeftType ) : INFINITY_BOUND;
        BoundType lbt = ( mLeftType == STRICT_BOUND && left() >= 0 ) ? STRICT_BOUND : WEAK_BOUND;
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
        mLeftType = mInterval.lower() == Checking::neg_inf() ? INFINITY_BOUND : WEAK_BOUND;
        mLeftType = mInterval.upper() == Checking::pos_inf() ? INFINITY_BOUND : WEAK_BOUND;
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
            case INFINITY_BOUND:
                return o.mLeftType != INFINITY_BOUND;
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
        switch( mLeftType )
        {
            case INFINITY_BOUND:
                return o.mLeftType != INFINITY_BOUND;
            default:
                return (right() >= o.right() );
        }
    }

    void DoubleInterval::dbgprint() const
    {
        std::cout.precision( 30 );
        if( mLeftType == INFINITY_BOUND )
            std::cout << "]-infinity";
        else
            std::cout << (mLeftType == STRICT_BOUND ? "]" : "[") << left();
        std::cout << ", ";
        if( mRightType == INFINITY_BOUND )
            std::cout << "infinity[";
        else
            std::cout << right() << (mRightType == WEAK_BOUND ? "]" : "[") << std::endl;
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
        if( d.leftType() == DoubleInterval::INFINITY_BOUND )
            str << "]-infinity";
        else
        {
            str.precision( 30 );
            str << (d.leftType() == DoubleInterval::STRICT_BOUND ? "]" : "[") << d.left();
            str.precision( 0 );
        }
        str << ", ";
        if( d.rightType() == DoubleInterval::INFINITY_BOUND )
            str << "infinity[";
        else
        {
            str.precision( 30 );
            str << d.right() << (d.rightType() == DoubleInterval::WEAK_BOUND ? "]" : "[");
            str.precision( 0 );
        }
        return str;
    }
}

