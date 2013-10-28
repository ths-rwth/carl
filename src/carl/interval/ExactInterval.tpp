/**
 * @author Stefan Schupp
 */

#pragma once
#include "ExactInterval.h"

#include "../core/Monomial.h"
#include "../core/Term.h"
#include "../core/MultivariatePolynomial.h"

#include <assert.h>


namespace carl
{
    using std::cout;
    using std::endl;


    //////////////////////////
    // Con- and destructors //
    //////////////////////////

    template<typename Numeric>
    ExactInterval<Numeric>::ExactInterval():
        mLeft( 0 ),
        mLeftType( INFINITY_BOUND ),
        mRight( 0 ),
        mRightType( INFINITY_BOUND )
    {}
    
    template<typename Numeric>
    ExactInterval<Numeric>::ExactInterval( const Numeric& l, BoundType lType, const Numeric& r, BoundType rType ):
        mLeft( l ),
        mLeftType( lType ),
        mRight( r ),
        mRightType( rType )
    {
        assert( BOUNDS_OK( l, lType, r, rType ));    // Specified bounds do not define an interval.
    }

    template<typename Numeric>
    ExactInterval<Numeric>::ExactInterval( const ExactInterval<Numeric>& i ):
        mLeft( i.mLeft ),
        mLeftType( i.mLeftType ),
        mRight( i.mRight ),
        mRightType( i.mRightType )
    {}

    template<typename Numeric>
    ExactInterval<Numeric>::~ExactInterval(){}

    ////////////////
    // Operations //
    ////////////////

    template<typename Numeric>
    Sign ExactInterval<Numeric>::sgn()
    {
        if( unbounded() )
            return Sign::ZERO;

        /** ----------0-[------------|-------
         * or
         *  ----------]0------------|--------
         */
        if( (mLeftType == STRICT_BOUND && mLeft >= 0) || (mLeftType == WEAK_BOUND && mLeft > 0) )
            return Sign::POSITIVE;

        /** -----------|------------0[-------
         * or
         *  ----------|------------]-0-------
         */
        if( (mRightType == STRICT_BOUND && mRight <= 0) || (mRightType == WEAK_BOUND && mRight < 0) )
            return Sign::NEGATIVE;

        /** ----------[0------------|-------
         * or
         *  ----------]-0------------|------
         * or
         *  ----------|------------0]--------
         * or
         *  ----------|-----------0-[--------
         * => zero is inside the interval
         */
        return Sign::ZERO;
    }

    template<typename Numeric>
    bool ExactInterval<Numeric>::contains( const Numeric& n ) const
    {
        switch( mLeftType )
        {
            case INFINITY_BOUND:
                break;
            case STRICT_BOUND:
                if( mLeft >= n )
                    return false;
                break;
            case WEAK_BOUND:
                if( mLeft > n )
                    return false;
        }
        // Invariant: n is not conflicting with left bound
        switch( mRightType )
        {
            case INFINITY_BOUND:
                break;
            case STRICT_BOUND:
                if( mRight <= n )
                    return false;
                break;
            case WEAK_BOUND:
                if( mRight < n )
                    return false;
                break;
        }
        return true;    // for open intervals: (mLeft < n && mRight > n) || (n == 0 && mLeft == Numeric( 0 ) && mRight == Numeric( 0 ))
    }

    template<typename Numeric>
    bool ExactInterval<Numeric>::contains( const ExactInterval<Numeric>& o ) const
    {
        switch( mLeftType )
        {
            case INFINITY_BOUND:
                break;
            case STRICT_BOUND:
                if( o.mLeftType == INFINITY_BOUND || mLeft > o.mLeft || ( mLeft == o.mLeft && o.mLeftType == WEAK_BOUND ) )
                    return false;
                break;
            case WEAK_BOUND:
                if( o.mLeftType == INFINITY_BOUND || mLeft >= o.mLeft )
                    return false;
        }
        // Invariant: left bound of o is not conflicting with left bound
        switch( mRightType )
        {
            case INFINITY_BOUND:
                break;
            case STRICT_BOUND:
                if( o.mRightType == INFINITY_BOUND || mRight < o.mRight || ( mRight == o.mRight && o.mRightType == WEAK_BOUND ) )
                    return false;
                break;
            case WEAK_BOUND:
                if( o.mRightType == INFINITY_BOUND || mRight <= o.mRight )
                    return false;
        }
        return true;    // for open intervals: mLeft <= o.mLeft && mRight >= o.mRight
    }

    template<typename Numeric>
    bool ExactInterval<Numeric>::meets( const Numeric& n ) const
    {
        return ( mLeft <= n || mLeftType == INFINITY_BOUND ) && ( mRight >= n || mRightType == INFINITY_BOUND );
    }

    template<typename Numeric>
    ExactInterval<Numeric> ExactInterval<Numeric>::intersect( const ExactInterval<Numeric>& o ) const
    {
        if( (mRight < o.mLeft && mRightType != INFINITY_BOUND && o.mLeftType != INFINITY_BOUND)
                || (o.mRight < mLeft && mLeftType != INFINITY_BOUND && o.mRightType != INFINITY_BOUND) )    // intersection empty
            return ExactInterval( 0, STRICT_BOUND, 0, STRICT_BOUND );
        // Invariant: ( mRight >= o.mLeft || mRightType == INFINITY_BOUND && o.mLeftType == INFINITY_BOUND ) && ( o.mRight >= mLeft || mLeftType == INFINITY_BOUND && o.mRightType == INFINITY_BOUND )
        BoundType leftBoundType  = mLeftType == INFINITY_BOUND && o.mLeftType == INFINITY_BOUND ? INFINITY_BOUND : WEAK_BOUND;
        BoundType rightBoundType = mRightType == INFINITY_BOUND && o.mRightType == INFINITY_BOUND ? INFINITY_BOUND : WEAK_BOUND;
        if( o.mLeftType == INFINITY_BOUND || mLeft > o.mLeft || (mLeft == o.mLeft && mLeftType != STRICT_BOUND && o.mLeftType != STRICT_BOUND) )
        {    // mLeft can be safely taken as weak bound of the intersection or is infinity
            if( o.mRightType == INFINITY_BOUND || (mRightType != INFINITY_BOUND && mRight < o.mRight) )
                return ExactInterval( mLeft, leftBoundType, mRight, rightBoundType );    // mRight can be safely taken as weak bound of the intersection
            else if( mRightType == INFINITY_BOUND || mRight > o.mRight )
                return ExactInterval( mLeft, leftBoundType, o.mRight, rightBoundType );    // o.mRight can be safely taken as weak bound of the intersection
            // Invariant: mRight == o.mRight
            if( mRightType == STRICT_BOUND || o.mRightType == STRICT_BOUND )
                return ExactInterval( mLeft, leftBoundType, mRight, STRICT_BOUND );    // the new right type has to be strict
            return ExactInterval( mLeft, leftBoundType, mRight, rightBoundType );
        }
        if( mLeftType == INFINITY_BOUND || mLeft < o.mLeft )
        {    // o.mLeft can be safely taken as weak bound of the intersection
            if( o.mRightType == INFINITY_BOUND || (mRightType != INFINITY_BOUND && mRight < o.mRight) )
                return ExactInterval( o.mLeft, leftBoundType, mRight, rightBoundType );    // mRight can be safely taken as weak bound of the intersection
            else if( mRightType == INFINITY_BOUND || mRight > o.mRight )
                return ExactInterval( o.mLeft, leftBoundType, o.mRight, rightBoundType );    // o.mRight can be safely taken as weak bound of the intersection
            // Invariant: mRight == o.mRight
            if( mRightType == STRICT_BOUND || o.mRightType == STRICT_BOUND )
                return ExactInterval( o.mLeft, leftBoundType, mRight, STRICT_BOUND );    // the new right type has to be strict
            return ExactInterval( o.mLeft, leftBoundType, mRight, rightBoundType );
        }
        // Invariant: mLeft == o.mLeft && ( mLeftType == STRICT_BOUND || o.mLeftType == STRICT_BOUND )
        assert( mLeftType == STRICT_BOUND || o.mLeftType == STRICT_BOUND );
        // the new left type has to be strict
        if( o.mRightType == INFINITY_BOUND || mRight < o.mRight )
            return ExactInterval( mLeft, STRICT_BOUND, mRight, rightBoundType );    // mRight can be safely taken as weak bound of the intersection
        else if( mRightType == INFINITY_BOUND || mRight > o.mRight )
            return ExactInterval( mLeft, STRICT_BOUND, o.mRight, rightBoundType );    // o.mRight can be safely taken as weak bound of the intersection
        // Invariant: mRight == o.mRight
        if( mRightType == STRICT_BOUND || o.mRightType == STRICT_BOUND )
            return ExactInterval( mLeft, STRICT_BOUND, mRight, STRICT_BOUND );    // the new right type has to be strict
        return ExactInterval( mLeft, STRICT_BOUND, mRight, rightBoundType );
    }

    template<typename Numeric>
    Numeric ExactInterval<Numeric>::midpoint() const
    {
        return getWeakestBoundType(mLeftType,mRightType) == INFINITY_BOUND ? Numeric(0) : (mLeft + mRight) / Numeric( 2 );
    }

    template<typename Numeric>
    ExactInterval<Numeric> ExactInterval<Numeric>::abs() const
    {
        Numeric l = mLeft >= 0 ? mLeft : -mLeft;
        Numeric r = mRight >= 0 ? mRight : -mRight;
        if( mLeft == 0 || mRight == 0 || mLeft.sgn() == mRight.sgn() )
        {
            if( l <= r )
                return ExactInterval( l, mLeftType, r, mRightType );
            else
                return ExactInterval( r, mRightType, l, mLeftType );
        }
        if( l >= r )
            return ExactInterval( 0, WEAK_BOUND, l, mLeftType );
        else
            return ExactInterval( 0, WEAK_BOUND, r, mRightType );
    }

    ///////////////////////////
    // Arithmetic Operations //
    ///////////////////////////

    template<typename Numeric>
    ExactInterval<Numeric> ExactInterval<Numeric>::add( const ExactInterval<Numeric>& o ) const
    {
        return ExactInterval( mLeft + o.mLeft,
                         getWeakestBoundType( mLeftType, o.mLeftType ),
                         mRight + o.mRight,
                         getWeakestBoundType( mRightType, o.mRightType ));
    }

    template<typename Numeric>
    ExactInterval<Numeric> ExactInterval<Numeric>::minus() const
    {
        return ExactInterval( -mRight, mRightType, -mLeft, mLeftType );
    }

    template<typename Numeric>
    ExactInterval<Numeric> ExactInterval<Numeric>::mul( const ExactInterval& o ) const
    {
        /*
                Numeric min  = mLeft * o.mLeft;
                Numeric max  = min;
                Numeric next = mLeft * o.mRight;
                if( next < min )
                    min = next;
                else if( max != next )
                    max = next;
                next = mRight * o.mLeft;
                if( next < min )
                    min = next;
                else if( next > max )
                    max = next;
                next = mRight * o.mRight;
                if( next < min )
                    min = next;
                else if( next > max )
                    max = next;


                return ExactInterval( min, getWeakestBoundType( mLeftType, o.mLeftType ), max, getWeakestBoundType( mRightType, o.mRightType ));
        */

        // true if equal to infty
        bool    mininfty, maxinfty, isSet;
        Numeric min, max;

        mininfty = (mLeftType == INFINITY_BOUND && (o.mRight > 0 || o.mRightType == INFINITY_BOUND))
                   || (mRightType == INFINITY_BOUND && (o.mLeft < 0 || o.mLeftType == INFINITY_BOUND))
                   || (o.mLeftType == INFINITY_BOUND && (mRight > 0 || mRightType == INFINITY_BOUND))
                   || (o.mRightType == INFINITY_BOUND && (mRight < 0 || (mLeft < 0 || mLeftType == INFINITY_BOUND)));

        maxinfty = (mLeftType == INFINITY_BOUND && (o.mRight < 0 || (o.mLeft < 0 || o.mLeftType == INFINITY_BOUND)))
                   || (mRightType == INFINITY_BOUND && (o.mLeft > 0 || (o.mRight > 0 || o.mRightType == INFINITY_BOUND)))
                   || (o.mLeftType == INFINITY_BOUND && (mRight < 0 || (mLeft < 0 || mLeftType == INFINITY_BOUND)))
                   || (o.mRightType == INFINITY_BOUND && (mLeft > 0 || (mRight > 0 || mRightType == INFINITY_BOUND)));

        // min calculation
        if( !mininfty )
        {
            isSet = false;
            if( mLeftType != INFINITY_BOUND )
            {
                if( o.mLeftType != INFINITY_BOUND )
                {
                    isSet = true;
                    min   = mLeft * o.mLeft;
                }
                if( o.mRightType != INFINITY_BOUND )
                {
                    if( min > mLeft * o.mRight ||!isSet )
                    {
                        min   = mLeft * o.mRight;
                        isSet = true;
                    }
                }
            }
            if( mRightType != INFINITY_BOUND )
            {
                if( o.mLeftType != INFINITY_BOUND )
                {
                    if( min > mRight * o.mLeft ||!isSet )
                    {
                        min   = mRight * o.mLeft;
                        isSet = true;
                    }
                }
                if( o.mRightType != INFINITY_BOUND )
                {
                    if( min > mRight * o.mRight ||!isSet )
                    {
                        min   = mRight * o.mRight;
                        isSet = true;
                    }

                }
            }
        }
        else
        {
            min = -1;
        }

        // max calculation
        if( !maxinfty )
        {
            isSet = false;
            if( mLeftType != INFINITY_BOUND )
            {
                if( o.mLeftType != INFINITY_BOUND )
                {
                    isSet = true;
                    max   = mLeft * o.mLeft;
                }
                if( o.mRightType != INFINITY_BOUND )
                {
                    if( max < mLeft * o.mRight ||!isSet )
                    {
                        max   = mLeft * o.mRight;
                        isSet = true;
                    }
                }
            }
            if( mRightType != INFINITY_BOUND )
            {
                if( o.mLeftType != INFINITY_BOUND )
                {
                    if( max < mRight * o.mLeft ||!isSet )
                    {
                        max   = mRight * o.mLeft;
                        isSet = true;
                    }
                }
                if( o.mRightType != INFINITY_BOUND )
                {
                    if( max < mRight * o.mRight ||!isSet )
                    {
                        max   = mRight * o.mRight;
                        isSet = true;
                    }
                }
            }
        }
        else
        {
            max = 1;
        }

        return ExactInterval( min, mininfty ? INFINITY_BOUND : WEAK_BOUND, max, maxinfty ? INFINITY_BOUND : WEAK_BOUND );

    }

    template<typename Numeric>
    ExactInterval<Numeric> ExactInterval<Numeric>::div( const ExactInterval<Numeric>& o ) const throw ( std::invalid_argument )
    {
        if( o.contains( 0 ))
            throw (std::invalid_argument( "Division by interval containing zero not allowed." ));
        Numeric min  = o.mLeft == 0 ? mLeft / o.mRight : mLeft / o.mLeft;    // only one, o.mLeft or o.mRight, might be 0
        Numeric max  = min;
        Numeric next = o.mRight == 0 ? mLeft / o.mLeft : mLeft / o.mRight;    // only one, o.mLeft or o.mRight, might be 0
        if( next < min )
            min = next;
        else if( max != next )
            max = next;
        next = o.mLeft == 0 ? mRight / o.mRight : mRight / o.mLeft;    // only one, o.mLeft or o.mRight, might be 0
        if( next < min )
            min = next;
        else if( next > max )
            max = next;
        next = o.mRight == 0 ? mRight / o.mLeft : mRight / o.mRight;    // only one, o.mLeft or o.mRight, might be 0
        if( next < min )
            min = next;
        else if( next > max )
            max = next;
        return ExactInterval( min, getWeakestBoundType( mLeftType, o.mLeftType ), max, getWeakestBoundType( mRightType, o.mRightType ));
    }

    template<typename Numeric>
    bool ExactInterval<Numeric>::div_ext( ExactInterval<Numeric>& a, ExactInterval<Numeric>& b, const ExactInterval<Numeric>& o )
    {
        ExactInterval<Numeric> inverseA, inverseB;
        bool     splitOccured;

        if( o.mLeft == 0 && o.mRight == 0 )    // point interval 0
        {
            splitOccured = false;
            if( this->contains( 0 ))
                a = ExactInterval( Numeric( -1 ), INFINITY_BOUND, Numeric( 1 ), INFINITY_BOUND );
            else
                a = emptyExactInterval();
            return false;
        }
        else
        {
            if( o.unbounded() )
            {
                a = unboundedExactInterval();
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

    template<typename Numeric>
    ExactInterval<Numeric> ExactInterval<Numeric>::power( unsigned e ) const
    {
        if( e % 2 || mLeft >= Numeric(0) )    // e is odd or left positive
            return ExactInterval( pow( mLeft, e ), mLeftType, pow( mRight, e ), mRightType );
        else if( mRight < Numeric(0) )
            return ExactInterval( pow( mRight, e ), mRightType, pow( mLeft, e ), mLeftType );
        Numeric lPower = pow( mLeft, e );
        Numeric rPower = pow( mRight, e );
        if( lPower <= rPower )
            return ExactInterval( Numeric(0), WEAK_BOUND, rPower, mRightType );
        return ExactInterval( Numeric(0), WEAK_BOUND, lPower, mLeftType );
    }

    template<typename Numeric>
    bool ExactInterval<Numeric>::inverse( ExactInterval<Numeric>& a, ExactInterval<Numeric>& b ) const
    {
        if( this->unbounded() )
        {
            a = ExactInterval(Numeric(0), STRICT_BOUND, Numeric(0), STRICT_BOUND);
            return false;
        }
        else if( this->contains( 0 ) && mLeft != 0 && mRight != 0 )
        {
            if( mLeftType == INFINITY_BOUND )
            {
                a = ExactInterval( Numeric( -1 ), INFINITY_BOUND, Numeric( 0 ), WEAK_BOUND );
                b = ExactInterval( Numeric( 1 / mRight ), WEAK_BOUND, Numeric( 1 ), INFINITY_BOUND );
            }
            else if( mRightType == INFINITY_BOUND )
            {
                a = ExactInterval( Numeric( -1 ), INFINITY_BOUND, Numeric( 1 / mLeft ), WEAK_BOUND );
                b = ExactInterval( Numeric( 0 ), WEAK_BOUND, Numeric( 1 ), INFINITY_BOUND );
            }
            else if( mLeft == 0 && mRight != 0 )
            {
                a = ExactInterval( Numeric( -1 ), INFINITY_BOUND, Numeric( 1 ), INFINITY_BOUND );
                b = ExactInterval( Numeric( 1 / mRight ), WEAK_BOUND, Numeric( 1 ), INFINITY_BOUND );
            }
            else if( mLeft != 0 && mRight == 0 )
            {
                a = ExactInterval( Numeric( -1 ), INFINITY_BOUND, Numeric( 1 / mLeft ), WEAK_BOUND );
                b = ExactInterval( Numeric( -1 ), INFINITY_BOUND, Numeric( 1 ), INFINITY_BOUND );
            }
            else if( mLeft == 0 && mRight == 0 )
            {
                a = ExactInterval(Numeric(-1), INFINITY_BOUND, Numeric(1), INFINITY_BOUND);
                return false;
            }
            else
            {
                a = ExactInterval( Numeric( -1 ), INFINITY_BOUND, Numeric( 1 / mLeft ), WEAK_BOUND );
                b = ExactInterval( Numeric( 1 / mRight ), WEAK_BOUND, Numeric( 1 ), INFINITY_BOUND );
            }
            return true;
        }
        else
        {
            if( mLeftType == INFINITY_BOUND && mRight != 0 )
            {
                a = ExactInterval( Numeric( 1 / mRight ), mRightType, Numeric( 0 ), WEAK_BOUND );
            }
            else if( mLeftType == INFINITY_BOUND && mRight == 0 )
            {
                a = ExactInterval( Numeric( -1 ), INFINITY_BOUND, Numeric( 0 ), WEAK_BOUND );
            }
            else if( mRightType == INFINITY_BOUND && mLeft != 0 )
            {
                a = ExactInterval( Numeric( 0 ), WEAK_BOUND, Numeric( 1 / mLeft ), mLeftType );
            }
            else if( mRightType == INFINITY_BOUND && mLeft == 0 )
            {
                a = ExactInterval( Numeric( 0 ), WEAK_BOUND, Numeric( 1 ), INFINITY_BOUND );
            }
            else if( mLeft != 0 && mRight != 0 )
            {
                a = ExactInterval( Numeric( 1 / mRight ), mRightType, Numeric( 1 / mLeft ), mLeftType );
            }
            else if( mLeft == 0 && mRight != 0 )
            {
                a = ExactInterval( Numeric( 1 / mRight ), mRightType, Numeric( 1 ), INFINITY_BOUND );
            }
            else if( mLeft != 0 && mRight == 0 )
            {
                a = ExactInterval( Numeric( -1 ), INFINITY_BOUND, Numeric( 1 / mLeft ), mLeftType );
            }
            return false;
        }
    }

    ///////////////////////////
    // Relational Operations //
    ///////////////////////////

    template<typename Numeric>
    bool ExactInterval<Numeric>::isEqual( const ExactInterval<Numeric>& o ) const
    {
        if( this == &o )
            return true;
        return ( mLeftType == o.mLeftType && mRightType == o.mRightType &&
                ( ( mLeftType == INFINITY_BOUND && mRight == o.mRight ) || ( mRightType == INFINITY_BOUND && mLeft == o.mLeft ) || ( mLeft == o.mLeft && mRight == o.mRight ) ) );
    }

    template<typename Numeric>
    bool ExactInterval<Numeric>::isLessOrEqual( const ExactInterval<Numeric>& o ) const
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
                return o.mLeftType == INFINITY_BOUND;
            default:
                return (mLeft <= o.mLeft);
        }
    }

    template<typename Numeric>
    bool ExactInterval<Numeric>::isGreaterOrEqual( const ExactInterval<Numeric>& o ) const
    {
        /** ----------]------------[------- >=
         *  -----]------------[------------
         * or
         *  ----------]------------[------- >=
         *  -------------]----[------------
         * or
         *  ----------]------------[------- >=
         *  ----------]-------[------------
         * or
         *  ----------]------------[------- >=
         *  -----]-----------------[-------
         * or
         *  ----------]------------[------- >=
         *  -------------]---------[-------
         * or
         *  ----------]------------[------- >=
         *  ----------]------------[------- holds.
         */
        // only compare right bounds
        switch( mRightType )
        {
            case INFINITY_BOUND:
                return o.mRightType == INFINITY_BOUND;
            default:
                return (mRight >= o.mRight);
        }
    }
    
    template<typename Numeric>
    std::ostream& operator << (std::ostream& str, const ExactInterval<Numeric>& d)
    {
        if( d.leftType() == INFINITY_BOUND )
            str << "]-infinity";
        else
        {
            str.precision( 30 );
            str << (d.leftType() == STRICT_BOUND ? "]" : "[") << d.left();
            str.precision( 0 );
        }
        str << ", ";
        if( d.rightType() == INFINITY_BOUND )
            str << "infinity[";
        else
        {
            str.precision( 30 );
            str << d.right() << (d.rightType() == WEAK_BOUND ? "]" : "[");
            str.precision( 0 );
        }
        return str;
    }

}
