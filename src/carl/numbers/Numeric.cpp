/**
 * @file Numeric.cpp
 * @author Florian Corzilius <corzilius@cs.rwth-aachen.de>
 *
 * @version 2014-03-11
 * Created on 2014-03-11
 */

#include "Numeric.h"

using namespace std;

namespace carl
{
    
    vector<cln::cl_RA> Numeric::mRationalPool = vector<cln::cl_RA>();
    vector<size_t> Numeric::mFreeRationalIds = vector<size_t>();
    
   
    ContentType Numeric::allocate( const cln::cl_RA& _value )
    {
        if( mFreeRationalIds.empty() )
        {
            mRationalPool.emplace_back( _value );
            assert( (ContentType) mRationalPool.size() < HIGHTEST_INTEGER_VALUE );
            return (ContentType) mRationalPool.size() + HIGHTEST_INTEGER_VALUE;
        }
        else
        {
            size_t id = mFreeRationalIds.back();
            mFreeRationalIds.pop_back();
            mRationalPool[id-(size_t)HIGHTEST_INTEGER_VALUE] = _value;
            return (ContentType) id;
        }
    }
    
    ContentType Numeric::allocate( ContentType _value )
    {
        if( mFreeRationalIds.empty() )
        {
            mRationalPool.emplace_back( _value );
            assert( (ContentType) mRationalPool.size() < HIGHTEST_INTEGER_VALUE );
            return (ContentType) mRationalPool.size() + HIGHTEST_INTEGER_VALUE;
        }
        else
        {
            size_t id = mFreeRationalIds.back();
            mFreeRationalIds.pop_back();
            mRationalPool[id-(size_t)HIGHTEST_INTEGER_VALUE] = _value;
            return (ContentType) id;
        }
    }
    
    /**
     * Default constructor.
     */
    Numeric::Numeric():
        mContent( 0 )
    {}

    /**
     * Constructing from a Rational.
     * @param The Rational.
     */
    Numeric::Numeric( const cln::cl_RA& _value ):
        mContent( (carl::isInteger( _value ) && carl::abs( carl::getNum( _value ) ) < HIGHTEST_INTEGER_VALUE) ? carl::toInt<ContentType>( carl::getNum( _value ) ) : allocate( _value ) )
    {}

    /**
     * Constructing from an integer.
     * @param _value The integer.
     */
    Numeric::Numeric( ContentType _value, bool is_definitely_int ):
        mContent( is_definitely_int || IS_INT( _value ) ? _value : allocate( _value ) )
    {}
    
    /**
     * Copy constructor.
     * @param _value The Numeric to copy.
     */
    Numeric::Numeric( const Numeric& _value ):
        mContent( IS_INT( _value.mContent ) ? _value.mContent : allocate( _value.mContent ) )
    {}

    Numeric::~Numeric()
    {
        if( !(IS_INT( mContent )) )
        {
            mFreeRationalIds.push_back( mContent );
        }
    }

    /**
     * Cast from a rational.
     * @param _value The rational.
     * @return The corresponding Numeric.
     */
    Numeric& Numeric::operator=( const cln::cl_RA& _value )
    {
        if( IS_INT( this->mContent ) )
        {
            if( carl::isInteger( _value ) && carl::abs( carl::getNum( _value ) ) < HIGHTEST_INTEGER_VALUE )
            {
                this->mContent = carl::toInt<ContentType>( carl::getNum( _value ) );
            }
            else
            {
                this->mContent = allocate( _value );
            }
        }
        else
        {
            if( carl::isInteger( _value ) && carl::abs( carl::getNum( _value ) ) < HIGHTEST_INTEGER_VALUE )
            {
                mFreeRationalIds.push_back( this->mContent );
                this->mContent = carl::toInt<ContentType>( carl::getNum( _value ) );
            }
            else
            {
                this->rRational() = _value;
            }
        }
        return *this;
    }

    /**
     * Cast from an integer.
     * @param _value The integer.
     * @return The corresponding Numeric.
     */
    Numeric& Numeric::operator=( ContentType _value )
    {
        if( IS_INT( this->mContent ) )
        {
            if( IS_INT( _value ) )
                this->mContent = _value;
            else
                this->mContent = allocate( _value );
        }
        else
        {
            if( IS_INT( _value ) )
            {
                mFreeRationalIds.push_back( this->mContent );
                this->mContent = _value;
            }
            else
                this->rRational() = cln::cl_RA( _value );
        }
        return *this;
    }

    /**
     * Cast from a char array.
     * @param _value The char array.
     * @return The corresponding Numeric.
     */
    Numeric& Numeric::operator=( const Numeric& _value )
    {
        if( IS_INT( this->mContent ) )
        {
            if( IS_INT( _value.mContent ) )
                this->mContent = _value.mContent;
            else
                this->mContent = allocate( _value.rational() );
        }
        else
        {
            if( IS_INT( _value.mContent ) )
            {
                mFreeRationalIds.push_back( this->mContent );
                this->mContent = _value.mContent;
            }
            else
                this->rRational() = _value.rational();
        }
        return *this;
    }

    /**
     * Compares whether this Numeric and the given one are equal.
     * @param _value The Numeric to compare with.
     * @return true, if the two Numerics are equal;
     *          false, otherwise.
     */
    bool Numeric::operator==( const Numeric& _value ) const
    {
        if( IS_INT( this->mContent ) )
        {
            if( IS_INT( _value.mContent ) )
                return this->mContent == _value.mContent;
            else
            {
                assert( _value.rational() == cln::cl_RA( this->mContent ) );
                return false;
            }
        }
        else
        {
            if( IS_INT( _value.mContent ) )
            {
                assert( this->rational() == cln::cl_RA( _value.mContent ) );
                return false;
            }
            else
                return this->rational() == _value.rational();
        }
    }

    /**
     * Compares whether this Numeric and the given one are not equal.
     * @param _value The Numeric to compare with.
     * @return true, if the two Numerics are not equal;
     *          false, otherwise.
     */
    bool Numeric::operator!=( const Numeric& _value ) const
    {
        if( IS_INT( this->mContent ) )
        {
            if( IS_INT( _value.mContent ) )
                return this->mContent != _value.mContent;
            else
            {
                assert( _value.rational() != this->mContent );
                return true;
            }
        }
        else
        {
            if( IS_INT( _value.mContent ) )
            {
                assert( this->rational() != _value.mContent );
                return true;
            }
            else
                return this->rational() != _value.rational();
        }
    }

    /**
     * Compares whether this Numeric is less than the given one.
     * @param _value The Numeric to compare with.
     * @return true, if this Numeric is less than the given one;
     *          false, otherwise.
     */
    bool Numeric::operator<( const Numeric& _value ) const
    {
        if( IS_INT( this->mContent ) )
        {
            if( IS_INT( _value.mContent ) )
                return this->mContent < _value.mContent;
            else
            {
                return _value.rational() > this->mContent;
            }
        }
        else
        {
            if( IS_INT( _value.mContent ) )
            {
                return this->rational() < _value.mContent;
            }
            else
                return this->rational() < _value.rational();
        }
    }

    /**
     * Compares whether this Numeric is less or equal than the given one.
     * @param _value The Numeric to compare with.
     * @return true, if this Numeric is less or equal than the given one;
     *          false, otherwise.
     */
    bool Numeric::operator<=( const Numeric& _value ) const
    {
        if( IS_INT( this->mContent ) )
        {
            if( IS_INT( _value.mContent ) )
                return this->mContent <= _value.mContent;
            else
            {
                return _value.rational() >= this->mContent;
            }
        }
        else
        {
            if( IS_INT( _value.mContent ) )
            {
                return this->rational() <= _value.mContent;
            }
            else
                return this->rational() <= _value.rational();
        }
    }

    /**
     * Compares whether this Numeric is greater than the given one.
     * @param _value The Numeric to compare with.
     * @return true, if this Numeric is greater than the given one;
     *          false, otherwise.
     */
    bool Numeric::operator>( const Numeric& _value ) const
    {
        if( IS_INT( this->mContent ) )
        {
            if( IS_INT( _value.mContent ) )
                return this->mContent > _value.mContent;
            else
            {
                return _value.rational() < this->mContent;
            }
        }
        else
        {
            if( IS_INT( _value.mContent ) )
            {
                return this->rational() > _value.mContent;
            }
            else
                return this->rational() > _value.rational();
        }
    }

    /**
     * Compares whether this Numeric is greater or equal than the given one.
     * @param _value The Numeric to compare with.
     * @return true, if this Numeric is greater or equal than the given one;
     *          false, otherwise.
     */
    bool Numeric::operator>=( const Numeric& _value ) const
    {
        if( IS_INT( this->mContent ) )
        {
            if( IS_INT( _value.mContent ) )
                return this->mContent >= _value.mContent;
            else
            {
                return _value.rational() <= this->mContent;
            }
        }
        else
        {
            if( IS_INT( _value.mContent ) )
            {
                return this->rational() >= _value.mContent;
            }
            else
                return this->rational() >= _value.rational();
        }
    }
    
    Numeric Numeric::divide( const Numeric& _value ) const
    {
        assert( isInteger( *this ) );
        assert( isInteger( _value ) );
        if( IS_INT( this->mContent ) )
        {
            assert( IS_INT( _value.mContent ) );
            assert( this->mContent > _value.mContent );
            return this->mContent / _value.mContent;
        }
        else
        {
            if( IS_INT( _value.mContent ) )
            {
                return Numeric( this->rational() / _value.mContent );
            }
            else
                return Numeric( this->rational() / _value.rational() );
        }
    }
    
    
    Numeric& Numeric::divideBy( const Numeric& _value )
    {
        assert( isInteger( *this ) );
        assert( isInteger( _value ) );
        if( IS_INT( this->mContent ) )
        {
            assert( IS_INT( _value.mContent ) );
            assert( this->mContent > _value.mContent );
            this->mContent /= _value.mContent;
        }
        else
        {
            cln::cl_RA& rat = this->rRational();
            if( IS_INT( _value.mContent ) )
                rat /= cln::cl_RA( _value.mContent );
            else
                rat /= _value.rational();
            if( carl::abs( rat ) < HIGHTEST_INTEGER_VALUE )
            {
                mFreeRationalIds.push_back( mContent );
                this->mContent = carl::toInt<ContentType>( carl::getNum( rat ) );
            }
        }
        return *this;
    }
    
    void gcd_( ContentType& _a, ContentType _b )
    {
        assert( _a > 0 && _b > 0 );
        while( true )
        {
            if( _a > _b ) _a -= _b;
            if( _a < _b ) _b -= _a;
            else return;
        }
    }
    
    Numeric& Numeric::gcd( const Numeric& _value )
    {
        assert( isInteger( *this ) );
        assert( isInteger( _value ) );
        if( this->mContent == 0 || _value.mContent == 0 )
        {
            if( !(IS_INT( this->mContent )) )
                mFreeRationalIds.push_back( mContent );
            this->mContent = 0;
        }
        if( IS_INT( this->mContent ) )
        {
            if( IS_INT( _value.mContent ) )
            {
                gcd_( this->mContent, _value.mContent );
            }
            else
            {
                cln::cl_I a = carl::abs( carl::getNum( _value.rational() ) );
                this->mContent = std::abs( this->mContent );
                while( a > this->mContent ) a -= this->mContent;
                gcd_( this->mContent, carl::toInt<ContentType>( a ) );
            }
        }
        else
        {
            if( IS_INT( _value.mContent ) )
            {
                cln::cl_I a = carl::abs( carl::getNum( this->rational() ) );
                mFreeRationalIds.push_back( mContent );
                this->mContent = std::abs( _value.mContent );
                while( a > this->mContent ) a -= this->mContent;
                gcd_( this->mContent, carl::toInt<ContentType>( a ) );
            }
            else
            {
                this->maybeIntegralize( carl::gcd( carl::getNum( this->rational() ), carl::getNum( _value.rational() ) ) );
            }
        }
        return *this;
    }

    /**
     * Calculates the absolute value of the given Numeric.
     * @param _value The Numeric to calculate the Numeric for.
     * @return The absolute value of the given Numeric.
     */
    Numeric abs( const Numeric& _value )
    {
        if( IS_INT( _value.content() ) )
        {
            return Numeric( std::abs( _value.content() ), true );
        }
        else
        {
            return Numeric( carl::abs( _value.rational() ) ); 
        }
    }

    /**
     * Calculates the least common multiple of the two arguments.
     * Note, that this method can only be applied to integers.
     * @param _valueA An integer.
     * @param _valueB An integer.
     * @return The least common multiple of the two arguments.
     */
    Numeric lcm( const Numeric& _valueA, const Numeric& _valueB )
    {
        assert( isInteger( _valueA ) );
        assert( isInteger( _valueB ) );
        if( IS_INT( _valueA.content() ) )
        {
            if( IS_INT( _valueB.content() ) )
            {
                ContentType g = _valueA.content();
                gcd_( g, _valueB.content() );
                return Numeric( (std::abs(_valueA.content()*_valueB.content())/g) );
            }
            else
                return Numeric( carl::lcm( cln::cl_I( _valueA.content() ), carl::getNum( _valueB.rational() ) ) );
        }
        else
        {
            if( IS_INT( _valueB.content() ) )
                return Numeric( carl::lcm( carl::getNum( _valueA.rational() ), cln::cl_I( _valueB.content() ) ) );
            else
                return Numeric( carl::lcm( carl::getNum( _valueA.rational() ), carl::getNum( _valueB.rational() ) ) );
        }
    }

    /**
     * Calculates the greatest common divisor of the two arguments.
     * Note, that this method can only be applied to integers.
     * @param _valueA An integer.
     * @param _valueB An integer.
     * @return The least common divisor of the two arguments.
     */
    Numeric gcd( const Numeric& _valueA, const Numeric& _valueB )
    {
        return Numeric( _valueA ).gcd( _valueB );
    }
    
    /**
     * Calculates the sum of the two given Numerics.
     * @param _valueA The first summand.
     * @param _valueB The second summand.
     * @return The sum of the two given Numerics.
     */
    Numeric operator+( const Numeric& _valueA, const Numeric& _valueB )
    {
        Numeric result( _valueA );
        result += _valueB;
        return result;
    }

    /**
     * Calculates the difference between the two given Numerics.
     * @param _valueA The minuend.
     * @param _valueB The subtrahend.
     * @return The difference between the two given Numerics.
     */
    Numeric operator-( const Numeric& _valueA, const Numeric& _valueB )
    {
        Numeric result( _valueA );
        result -= _valueB;
        return result;
    }

    /**
     * Calculates the product of the two given Numerics.
     * @param _valueA The first factor.
     * @param _valueB The second factor.
     * @return The product of the two given Numerics.
     */
    Numeric operator*( const Numeric& _valueA, const Numeric& _valueB )
    {
        Numeric result( _valueA );
        result *= _valueB;
        return result;
    }

    /**
     * Calculates the division of the two given Numerics.
     * @param _valueA The dividend.
     * @param _valueB The divisor.
     * @return The difference of the two given Numerics.
     */
    Numeric operator/( const Numeric& _valueA, const Numeric& _valueB )
    {
        Numeric result( _valueA );
        result /= _valueB;
        return result;
    }
    
    /**
     * Adds the value of the second given Numeric to the second given Numeric.
     * @param _valueA The Numeric to add to.
     * @param _valueB The Numeric to add.
     * @return The first given Numeric increased by the second given Numeric.
     */
    Numeric& operator+=( Numeric& _valueA, const Numeric& _valueB )
    {
        if( IS_INT( _valueA.mContent ) )
        {
            if( IS_INT( _valueB.mContent ) )
            {
                _valueA.mContent += _valueB.mContent;
                _valueA.maybeRationalize();
            }
            else
            {
                cln::cl_RA rat = _valueB.rational();
                rat += _valueA.mContent;
                _valueA.maybeRationalize( rat );
            }
        }
        else
        {
            cln::cl_RA& rat = _valueA.rRational();
            if( IS_INT( _valueB.mContent ) )
            {
                rat += _valueB.mContent;
            }
            else
            {
                rat += _valueB.rational();
            }
            _valueA.maybeIntegralize( rat );
        }
        return _valueA;
    }

    /**
     * Subtracts the second given Numeric to the first given Numeric.
     * @param _valueA The Numeric to subtract from.
     * @param _valueB The Numeric to subtract.
     * @return The first given Numeric subtracted by the second given Numeric.
     */
    Numeric& operator-=( Numeric& _valueA, const Numeric& _valueB )
    {
        if( IS_INT( _valueA.mContent ) )
        {
            if( IS_INT( _valueB.mContent ) )
            {
                _valueA.mContent -= _valueB.mContent;
                _valueA.maybeRationalize();
            }
            else
            {
                cln::cl_RA rat = _valueB.rational();
                rat -= _valueA.mContent;
                _valueA.maybeRationalize( rat );
            }
        }
        else
        {
            cln::cl_RA& rat = _valueA.rRational();
            if( IS_INT( _valueB.mContent ) )
            {
                rat -= _valueB.mContent;
            }
            else
            {
                rat -= _valueB.rational();
            }
            _valueA.maybeIntegralize( rat );
        }
        return _valueA;
    }

    /**
     * Multiplies the second given Numeric to the first given Numeric.
     * @param _valueA The Numeric to multiply.
     * @param _valueB The Numeric to multiply by.
     * @return The first given Numeric multiplied by the second given Numeric.
     */
    Numeric& operator*=( Numeric& _valueA, const Numeric& _valueB )
    {
        if( IS_INT( _valueA.mContent ) )
        {
            if( IS_INT( _valueB.mContent ) )
            {
                _valueA.mContent *= _valueB.mContent;
                _valueA.maybeRationalize();
            }
            else
            {
                cln::cl_RA rat = _valueB.rational();
                rat *= cln::cl_RA( _valueA.mContent );
                _valueA.maybeRationalize( rat );
            }
        }
        else
        {
            cln::cl_RA& rat = _valueA.rRational();
            if( IS_INT( _valueB.mContent ) )
            {
                rat *= cln::cl_RA( _valueB.mContent );
            }
            else
            {
                rat *= _valueB.rational();
            }
            _valueA.maybeIntegralize( rat );
        }
        return _valueA;
    }

    /**
     * Divides the first given Numeric by the second given Numeric.
     * @param _valueA The Numeric to divide.
     * @param _valueB The Numeric to divide by.
     * @return The first given Numeric divided by the second given Numeric.
     */
    Numeric& operator/=( Numeric& _valueA, const Numeric& _valueB )
    {
        if( IS_INT( _valueA.mContent ) )
        {
            cln::cl_RA rat = cln::cl_RA( _valueA.mContent );
            if( IS_INT( _valueB.mContent ) )
            {
                rat /= cln::cl_RA( _valueB.mContent );
            }
            else
            {
                rat /= _valueB.rational();
            }
            _valueA.maybeRationalize( rat );
        }
        else
        {
            cln::cl_RA& rat = _valueA.rRational();
            if( IS_INT( _valueB.mContent ) )
            {
                rat /= cln::cl_RA( _valueB.mContent );
            }
            else
            {
                rat /= _valueB.rational();
            }
            _valueA.maybeIntegralize( rat );
        }
        return _valueA;
    }

    /**
     * Calculates the additive inverse of the given Numeric.
     * @param _value The Numeric to calculate the additive inverse for.
     * @return The additive inverse of the given Numeric.
     */
    Numeric operator-( const Numeric& _value )
    {
        if( IS_INT( _value.content() ) )
            return Numeric( -_value.content(), true );
        else
            return Numeric( -_value.rational() );
    }

    /**
     * Increments the given Numeric by one.
     * @param _value The Numeric to increment.
     * @return The given Numeric incremented by one.
     */
    Numeric& operator++( Numeric& _value )
    {
        if( IS_INT( _value.mContent ) )
            _value.mContent++;
        else
            _value.rRational()++;
        return _value;
    }

    /**
     * Decrements the given Numeric by one.
     * @param _value The Numeric to decrement.
     * @return The given Numeric decremented by one.
     */
    Numeric& operator--( Numeric& _value )
    {
        if( IS_INT( _value.mContent ) )
            _value.mContent--;
        else
            _value.rRational()--;
        return _value;
    }

    /**
     * Prints the given Numerics representation on the given output stream.
     * @param _out The output stream to print on.
     * @param _value The Numeric to print.
     * @return The output stream after printing the given Numerics representation on it.
     */
    ostream& operator <<( ostream& _out, const Numeric& _value )
    {
        if( IS_INT( _value.content() ) )
            _out << _value.content();
        else
            _out << _value.rational();
        return _out;
    }
} // end namespace carl
