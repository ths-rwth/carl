/**
 * @file ExactInterval.tpp
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
        mLeftType( BoundType::INFTY ),
        mRight( 0 ),
        mRightType( BoundType::INFTY )
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
    ExactInterval<Numeric>::ExactInterval( const Numeric& l, const Numeric& r, BoundType type ):
		ExactInterval<Numeric>(l, type, r, type)
	{
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
        if( (mLeftType == BoundType::STRICT && mLeft >= 0) || (mLeftType == BoundType::WEAK && mLeft > 0) )
            return Sign::POSITIVE;

        /** -----------|------------0[-------
         * or
         *  ----------|------------]-0-------
         */
        if( (mRightType == BoundType::STRICT && mRight <= 0) || (mRightType == BoundType::WEAK && mRight < 0) )
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
            case BoundType::INFTY:
                break;
            case BoundType::STRICT:
                if( mLeft >= n )
                    return false;
                break;
            case BoundType::WEAK:
                if( mLeft > n )
                    return false;
        }
        // Invariant: n is not conflicting with left bound
        switch( mRightType )
        {
            case BoundType::INFTY:
                break;
            case BoundType::STRICT:
                if( mRight <= n )
                    return false;
                break;
            case BoundType::WEAK:
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
            case BoundType::INFTY:
                break;
            case BoundType::STRICT:
                if( o.mLeftType == BoundType::INFTY || mLeft > o.mLeft || ( mLeft == o.mLeft && o.mLeftType == BoundType::WEAK ) )
                    return false;
                break;
            case BoundType::WEAK:
                if( o.mLeftType == BoundType::INFTY || mLeft >= o.mLeft )
                    return false;
        }
        // Invariant: left bound of o is not conflicting with left bound
        switch( mRightType )
        {
            case BoundType::INFTY:
                break;
            case BoundType::STRICT:
                if( o.mRightType == BoundType::INFTY || mRight < o.mRight || ( mRight == o.mRight && o.mRightType == BoundType::WEAK ) )
                    return false;
                break;
            case BoundType::WEAK:
                if( o.mRightType == BoundType::INFTY || mRight <= o.mRight )
                    return false;
        }
        return true;    // for open intervals: mLeft <= o.mLeft && mRight >= o.mRight
    }

    template<typename Numeric>
    bool ExactInterval<Numeric>::meets( const Numeric& n ) const
    {
        return ( mLeft <= n || mLeftType == BoundType::INFTY ) && ( mRight >= n || mRightType == BoundType::INFTY );
    }

    template<typename Numeric>
    ExactInterval<Numeric> ExactInterval<Numeric>::intersect( const ExactInterval<Numeric>& o ) const
    {
        if( (mRight < o.mLeft && mRightType != BoundType::INFTY && o.mLeftType != BoundType::INFTY)
                || (o.mRight < mLeft && mLeftType != BoundType::INFTY && o.mRightType != BoundType::INFTY) || this->empty() || o.empty() )    // intersection empty
            return ExactInterval( 0, BoundType::STRICT, 0, BoundType::STRICT );
        // Invariant: ( mRight >= o.mLeft || mRightType == BoundType::INFTY && o.mLeftType == BoundType::INFTY ) && ( o.mRight >= mLeft || mLeftType == BoundType::INFTY && o.mRightType == BoundType::INFTY )
        BoundType leftBoundType  = mLeftType == BoundType::INFTY && o.mLeftType == BoundType::INFTY ? BoundType::INFTY : BoundType::WEAK;
        BoundType rightBoundType = mRightType == BoundType::INFTY && o.mRightType == BoundType::INFTY ? BoundType::INFTY : BoundType::WEAK;
        if( o.mLeftType == BoundType::INFTY || mLeft > o.mLeft || (mLeft == o.mLeft && mLeftType != BoundType::STRICT && o.mLeftType != BoundType::STRICT) )
        {    // mLeft can be safely taken as weak bound of the intersection or is infinity
            if( o.mRightType == BoundType::INFTY || (mRightType != BoundType::INFTY && mRight < o.mRight) )
                return ExactInterval( mLeft, leftBoundType, mRight, rightBoundType );    // mRight can be safely taken as weak bound of the intersection
            else if( mRightType == BoundType::INFTY || mRight > o.mRight )
                return ExactInterval( mLeft, leftBoundType, o.mRight, rightBoundType );    // o.mRight can be safely taken as weak bound of the intersection
            // Invariant: mRight == o.mRight
            if( mRightType == BoundType::STRICT || o.mRightType == BoundType::STRICT )
                return ExactInterval( mLeft, leftBoundType, mRight, BoundType::STRICT );    // the new right type has to be strict
            return ExactInterval( mLeft, leftBoundType, mRight, rightBoundType );
        }
        if( mLeftType == BoundType::INFTY || mLeft < o.mLeft )
        {    // o.mLeft can be safely taken as weak bound of the intersection
            if( o.mRightType == BoundType::INFTY || (mRightType != BoundType::INFTY && mRight < o.mRight) )
                return ExactInterval( o.mLeft, leftBoundType, mRight, rightBoundType );    // mRight can be safely taken as weak bound of the intersection
            else if( mRightType == BoundType::INFTY || mRight > o.mRight )
                return ExactInterval( o.mLeft, leftBoundType, o.mRight, rightBoundType );    // o.mRight can be safely taken as weak bound of the intersection
            // Invariant: mRight == o.mRight
            if( mRightType == BoundType::STRICT || o.mRightType == BoundType::STRICT )
                return ExactInterval( o.mLeft, leftBoundType, mRight, BoundType::STRICT );    // the new right type has to be strict
            return ExactInterval( o.mLeft, leftBoundType, mRight, rightBoundType );
        }
        // Invariant: mLeft == o.mLeft && ( mLeftType == BoundType::STRICT || o.mLeftType == BoundType::STRICT )
        assert( mLeftType == BoundType::STRICT || o.mLeftType == BoundType::STRICT );
        // the new left type has to be strict
        if( o.mRightType == BoundType::INFTY || mRight < o.mRight )
            return ExactInterval( mLeft, BoundType::STRICT, mRight, rightBoundType );    // mRight can be safely taken as weak bound of the intersection
        else if( mRightType == BoundType::INFTY || mRight > o.mRight )
            return ExactInterval( mLeft, BoundType::STRICT, o.mRight, rightBoundType );    // o.mRight can be safely taken as weak bound of the intersection
        // Invariant: mRight == o.mRight
        if( mRightType == BoundType::STRICT || o.mRightType == BoundType::STRICT )
            return ExactInterval( mLeft, BoundType::STRICT, mRight, BoundType::STRICT );    // the new right type has to be strict
        return ExactInterval( mLeft, BoundType::STRICT, mRight, rightBoundType );
    }

    template<typename Numeric>
    Numeric ExactInterval<Numeric>::midpoint() const
    {
        return getWeakestBoundType(mLeftType,mRightType) == BoundType::INFTY ? Numeric(0) : (mLeft + mRight) / Numeric( 2 );
    }

	template<typename Numeric>
    Numeric ExactInterval<Numeric>::sample() const
    {
		Numeric mid = this->midpoint();
		// TODO: check if mid is an integer already.
		Numeric midf = carl::floor(mid);
		if (this->contains(midf)) return midf;
		Numeric midc = carl::ceil(mid);
		if (this->contains(midc)) return midc;
		return mid;
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
            return ExactInterval( 0, BoundType::WEAK, l, mLeftType );
        else
            return ExactInterval( 0, BoundType::WEAK, r, mRightType );
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
    ExactInterval<Numeric> ExactInterval<Numeric>::inverse() const
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

        mininfty = (mLeftType == BoundType::INFTY && (o.mRight > 0 || o.mRightType == BoundType::INFTY))
                   || (mRightType == BoundType::INFTY && (o.mLeft < 0 || o.mLeftType == BoundType::INFTY))
                   || (o.mLeftType == BoundType::INFTY && (mRight > 0 || mRightType == BoundType::INFTY))
                   || (o.mRightType == BoundType::INFTY && (mRight < 0 || (mLeft < 0 || mLeftType == BoundType::INFTY)));

        maxinfty = (mLeftType == BoundType::INFTY && (o.mRight < 0 || (o.mLeft < 0 || o.mLeftType == BoundType::INFTY)))
                   || (mRightType == BoundType::INFTY && (o.mLeft > 0 || (o.mRight > 0 || o.mRightType == BoundType::INFTY)))
                   || (o.mLeftType == BoundType::INFTY && (mRight < 0 || (mLeft < 0 || mLeftType == BoundType::INFTY)))
                   || (o.mRightType == BoundType::INFTY && (mLeft > 0 || (mRight > 0 || mRightType == BoundType::INFTY)));

        // min calculation
        if( !mininfty )
        {
            isSet = false;
            if( mLeftType != BoundType::INFTY )
            {
                if( o.mLeftType != BoundType::INFTY )
                {
                    isSet = true;
                    min   = mLeft * o.mLeft;
                }
                if( o.mRightType != BoundType::INFTY )
                {
                    if( min > mLeft * o.mRight ||!isSet )
                    {
                        min   = mLeft * o.mRight;
                        isSet = true;
                    }
                }
            }
            if( mRightType != BoundType::INFTY )
            {
                if( o.mLeftType != BoundType::INFTY )
                {
                    if( min > mRight * o.mLeft ||!isSet )
                    {
                        min   = mRight * o.mLeft;
                        isSet = true;
                    }
                }
                if( o.mRightType != BoundType::INFTY )
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
            if( mLeftType != BoundType::INFTY )
            {
                if( o.mLeftType != BoundType::INFTY )
                {
                    isSet = true;
                    max   = mLeft * o.mLeft;
                }
                if( o.mRightType != BoundType::INFTY )
                {
                    if( max < mLeft * o.mRight ||!isSet )
                    {
                        max   = mLeft * o.mRight;
                        isSet = true;
                    }
                }
            }
            if( mRightType != BoundType::INFTY )
            {
                if( o.mLeftType != BoundType::INFTY )
                {
                    if( max < mRight * o.mLeft ||!isSet )
                    {
                        max   = mRight * o.mLeft;
                        isSet = true;
                    }
                }
                if( o.mRightType != BoundType::INFTY )
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

        return ExactInterval( min, mininfty ? BoundType::INFTY : BoundType::WEAK, max, maxinfty ? BoundType::INFTY : BoundType::WEAK );

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
        ExactInterval<Numeric> reciprocalA, reciprocalB;
        bool     splitOccured;

        if( o.mLeft == 0 && o.mRight == 0 )    // point interval 0
        {
            splitOccured = false;
            if( this->contains( 0 ))
                a = ExactInterval( Numeric( -1 ), BoundType::INFTY, Numeric( 1 ), BoundType::INFTY );
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
            return ExactInterval( Numeric(0), BoundType::WEAK, rPower, mRightType );
        return ExactInterval( Numeric(0), BoundType::WEAK, lPower, mLeftType );
    }

    template<typename Numeric>
    bool ExactInterval<Numeric>::reciprocal( ExactInterval<Numeric>& a, ExactInterval<Numeric>& b ) const
    {
        if( this->unbounded() )
        {
            a = ExactInterval(Numeric(0), BoundType::STRICT, Numeric(0), BoundType::STRICT);
            return false;
        }
        else if( this->contains( 0 ) && mLeft != 0 && mRight != 0 )
        {
            if( mLeftType == BoundType::INFTY )
            {
                a = ExactInterval( Numeric( -1 ), BoundType::INFTY, Numeric( 0 ), BoundType::WEAK );
                b = ExactInterval( Numeric( 1 / mRight ), BoundType::WEAK, Numeric( 1 ), BoundType::INFTY );
            }
            else if( mRightType == BoundType::INFTY )
            {
                a = ExactInterval( Numeric( -1 ), BoundType::INFTY, Numeric( 1 / mLeft ), BoundType::WEAK );
                b = ExactInterval( Numeric( 0 ), BoundType::WEAK, Numeric( 1 ), BoundType::INFTY );
            }
            else if( mLeft == 0 && mRight != 0 )
            {
                a = ExactInterval( Numeric( -1 ), BoundType::INFTY, Numeric( 1 ), BoundType::INFTY );
                b = ExactInterval( Numeric( 1 / mRight ), BoundType::WEAK, Numeric( 1 ), BoundType::INFTY );
            }
            else if( mLeft != 0 && mRight == 0 )
            {
                a = ExactInterval( Numeric( -1 ), BoundType::INFTY, Numeric( 1 / mLeft ), BoundType::WEAK );
                b = ExactInterval( Numeric( -1 ), BoundType::INFTY, Numeric( 1 ), BoundType::INFTY );
            }
            else if( mLeft == 0 && mRight == 0 )
            {
                a = ExactInterval(Numeric(-1), BoundType::INFTY, Numeric(1), BoundType::INFTY);
                return false;
            }
            else
            {
                a = ExactInterval( Numeric( -1 ), BoundType::INFTY, Numeric( 1 / mLeft ), BoundType::WEAK );
                b = ExactInterval( Numeric( 1 / mRight ), BoundType::WEAK, Numeric( 1 ), BoundType::INFTY );
            }
            return true;
        }
        else
        {
            if( mLeftType == BoundType::INFTY && mRight != 0 )
            {
                a = ExactInterval( Numeric( 1 / mRight ), mRightType, Numeric( 0 ), BoundType::WEAK );
            }
            else if( mLeftType == BoundType::INFTY && mRight == 0 )
            {
                a = ExactInterval( Numeric( -1 ), BoundType::INFTY, Numeric( 0 ), BoundType::WEAK );
            }
            else if( mRightType == BoundType::INFTY && mLeft != 0 )
            {
                a = ExactInterval( Numeric( 0 ), BoundType::WEAK, Numeric( 1 / mLeft ), mLeftType );
            }
            else if( mRightType == BoundType::INFTY && mLeft == 0 )
            {
                a = ExactInterval( Numeric( 0 ), BoundType::WEAK, Numeric( 1 ), BoundType::INFTY );
            }
            else if( mLeft != 0 && mRight != 0 )
            {
                a = ExactInterval( Numeric( 1 / mRight ), mRightType, Numeric( 1 / mLeft ), mLeftType );
            }
            else if( mLeft == 0 && mRight != 0 )
            {
                a = ExactInterval( Numeric( 1 / mRight ), mRightType, Numeric( 1 ), BoundType::INFTY );
            }
            else if( mLeft != 0 && mRight == 0 )
            {
                a = ExactInterval( Numeric( -1 ), BoundType::INFTY, Numeric( 1 / mLeft ), mLeftType );
            }
            return false;
        }
    }
    
    template<typename Numeric>
    ExactInterval<Numeric> ExactInterval<Numeric>::exp() const
    {
        return ExactInterval<Numeric>(exp(mLeft), mLeftType, exp(mRight), mRightType);
    }

    template<typename Numeric>
    ExactInterval<Numeric> ExactInterval<Numeric>::log() const
    {
        return ExactInterval<Numeric>(log(mLeft), mLeftType, log(mRight), mRightType);
    }
    
    template<typename Numeric>
    void ExactInterval<Numeric>::split(ExactInterval<Numeric>& _left, ExactInterval<Numeric>& _right) const
    {
        if(left() != right() || (leftType() == BoundType::INFTY && rightType() == BoundType::INFTY))
        {
            _left.set(left(),midpoint());
            _left.setLeftType(leftType());
            _left.setRightType(BoundType::STRICT);

            _right.set(midpoint(),right());
            _right.setLeftType(BoundType::WEAK);
            _right.setRightType(rightType());
        }
        else
        {
            _left = *this;
            _right = *this;
        }
    }
    
    template<typename Numeric>
    void ExactInterval<Numeric>::split(std::vector<ExactInterval<Numeric> >& _result, const unsigned n) const
    {
        Numeric diameter = this->diameter();
        diameter /= n;

        ExactInterval<Numeric> tmp;
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
    
    template<typename Numeric>
    void ExactInterval<Numeric>::bloat(const Numeric& _width)
    {
        set(left()-_width, right()+_width);
    }
    
    template<typename Numeric>
    Numeric ExactInterval<Numeric>::diameter() const
    {
        if( mLeftType == BoundType::INFTY || mRightType == BoundType::INFTY )
        {
            return -1;
        }
        return right() - left();
    }
    
    template<typename Numeric>
    Numeric ExactInterval<Numeric>::diameterRatio( const ExactInterval<Numeric>& _interval) const
    {
        return diameter()/_interval.diameter();
    }
    
    template<typename Numeric>
    Numeric ExactInterval<Numeric>::magnitude() const
    {
        Numeric inf = abs(left());
        Numeric sup = abs(right());
        return inf < sup ? sup : inf;
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
                ( ( mLeftType == BoundType::INFTY && mRight == o.mRight ) || ( mRightType == BoundType::INFTY && mLeft == o.mLeft ) || ( mLeft == o.mLeft && mRight == o.mRight ) ) );
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
		if( mRightType == BoundType::INFTY || o.leftType() == BoundType::INFTY )
		{
			return false;
		}
		else
		{
			return mRight <= o.mLeft;
		}
		
    }

    template<typename Numeric>
    bool ExactInterval<Numeric>::isGreaterOrEqual( const ExactInterval<Numeric>& o ) const
    {
        return o.isLessOrEqual(*this);
    }
	
	template<typename Numeric>
	void ExactInterval<Numeric>::operator+=( const ExactInterval<Numeric>& o )
    {
		*this = *this + o;
    }
	
	template<typename Numeric>
    void ExactInterval<Numeric>::operator-=( const ExactInterval<Numeric>& o )
    {
		*this = *this - o;
    }
	
	template<typename Numeric>
    void ExactInterval<Numeric>::operator*=( const ExactInterval<Numeric>& o )
    {
		*this = *this * o;
    }
    
    template<typename Numeric>
    std::ostream& operator << (std::ostream& str, const ExactInterval<Numeric>& d)
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
