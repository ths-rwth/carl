/**
 * @file Numeric.tpp
 * @author Florian Corzilius <corzilius@cs.rwth-aachen.de>
 *
 * @version 2014-03-11
 * Created on 2014-03-11
 */

#include "typetraits.h"

namespace carl
{
    template<typename T>
    std::vector<T> Numeric<T>::mRationalPool = std::vector<T>();
    template<typename T>
    std::vector<ContentType> Numeric<T>::mFreeRationalIds = std::vector<ContentType>();
    
    template<typename T>
    ContentType Numeric<T>::allocate( const T& _value )
    {
        if( mFreeRationalIds.empty() )
        {
#ifdef __VS
            mRationalPool.push_back(_value);
#else
            mRationalPool.emplace_back( _value );
#endif
            assert( mRationalPool.size() + (size_t) HIGHTEST_INTEGER_VALUE - 1 <= std::numeric_limits<size_t>::max() );
            return ContentType(mRationalPool.size() + HIGHTEST_INTEGER_VALUE - 1);
        }
        else
        {
            ContentType id = mFreeRationalIds.back();
            mFreeRationalIds.pop_back();
            assert( id >= HIGHTEST_INTEGER_VALUE );
            mRationalPool[std::size_t(id-HIGHTEST_INTEGER_VALUE)] = _value;
            return id;
        }
    }
    
    template<typename T>
    ContentType Numeric<T>::allocate( ContentType _value )
    {
        if( mFreeRationalIds.empty() )
        {
            mRationalPool.emplace_back(carl::fromInt<T>(_value));
            assert( mRationalPool.size() + (size_t) HIGHTEST_INTEGER_VALUE - 1 <= std::numeric_limits<size_t>::max() );
            return ContentType(mRationalPool.size() + HIGHTEST_INTEGER_VALUE - 1);
        }
        else
        {
            ContentType id = mFreeRationalIds.back();
            mFreeRationalIds.pop_back();
            assert( id >= HIGHTEST_INTEGER_VALUE );
            mRationalPool[std::size_t(id-HIGHTEST_INTEGER_VALUE)] = carl::fromInt<T>(_value);
            return id;
        }
    }

    /**
     * Constructing from a Rational.
     * @param _value The Rational.
     */
    template<typename T>
    Numeric<T>::Numeric( const T& _value ):
        mContent( (carl::isInteger( _value ) && withinNativeRange(carl::getNum(_value))) ? carl::toInt<ContentType>( carl::getNum( _value ) ) : allocate( _value ) )
    {}

    /**
     * Constructing from an integer.
     * @param _value The integer.
	 * @param is_definitely_int
     */
    template<typename T>
    Numeric<T>::Numeric( ContentType _value, bool is_definitely_int ):
        mContent( is_definitely_int || IS_INT( _value ) ? _value : allocate( _value ) )
    {}
    
    /**
     * Copy constructor.
     * @param _value The Numeric to copy.
     */
    template<typename T>
    Numeric<T>::Numeric( const Numeric<T>& _value ):
        mContent( IS_INT( _value.mContent ) ? _value.mContent : allocate( _value.rational() ) )
    {}

    template<typename T>
    Numeric<T>::~Numeric()
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
    template<typename T>
    Numeric<T>& Numeric<T>::operator=( const T& _value )
    {
        if( IS_INT( this->mContent ) )
        {
            if( carl::isInteger( _value ) && withinNativeRange(_value) )
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
            if( carl::isInteger( _value ) && withinNativeRange(_value) )
            {
                mFreeRationalIds.push_back( this->mContent );
                this->mContent = carl::toInt<ContentType>( carl::getNum( _value ) );
            }
            else
            {
                this->rRational() = _value;
            }
        }
        return *this; // NOLINT
    }

    /**
     * Cast from an integer.
     * @param _value The integer.
     * @return The corresponding Numeric.
     */
    template<typename T>
    Numeric<T>& Numeric<T>::operator=( ContentType _value )
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
                this->rRational() = carl::fromInt<T>( _value );
        }
        return *this; // NOLINT
    }

    /**
     * Cast from a char array.
     * @param _value The char array.
     * @return The corresponding Numeric.
     */
    template<typename T>
    Numeric<T>& Numeric<T>::operator=( const Numeric<T>& _value )
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
        return *this; // NOLINT
    }

    /**
     * Compares whether this Numeric and the given one are equal.
     * @param _value The Numeric to compare with.
     * @return true, if the two Numerics are equal;
     *          false, otherwise.
     */
    template<typename T>
    bool Numeric<T>::operator==( const Numeric<T>& _value ) const
    {
        if( IS_INT( this->mContent ) )
        {
            if( IS_INT( _value.mContent ) )
                return this->mContent == _value.mContent;
            else
            {
                assert( _value.rational() != carl::fromInt<T>(this->mContent) );
                return false;
            }
        }
        else
        {
            if( IS_INT( _value.mContent ) )
            {
                assert( this->rational() != carl::fromInt<T>(_value.mContent) );
                return false;
            }
            else
            {
                return (this->rational() == _value.rational());
            }
        }
    }

    /**
     * Compares whether this Numeric and the given one are not equal.
     * @param _value The Numeric to compare with.
     * @return true, if the two Numerics are not equal;
     *          false, otherwise.
     */
    template<typename T>
    bool Numeric<T>::operator!=( const Numeric<T>& _value ) const
    {
        if( IS_INT( this->mContent ) )
        {
            if( IS_INT( _value.mContent ) )
                return this->mContent != _value.mContent;
            else
            {
                assert( _value.rational() != carl::fromInt<T>(this->mContent) );
                return true;
            }
        }
        else
        {
            if( IS_INT( _value.mContent ) )
            {
                assert( this->rational() != carl::fromInt<T>(_value.mContent) );
                return true;
            }
            else
            {
                return (this->rational() != _value.rational());
            }
        }
    }

    /**
     * Compares whether this Numeric is less than the given one.
     * @param _value The Numeric to compare with.
     * @return true, if this Numeric is less than the given one;
     *          false, otherwise.
     */
    template<typename T>
    bool Numeric<T>::operator<( const Numeric<T>& _value ) const
    {
        if( IS_INT( this->mContent ) )
        {
            if( IS_INT( _value.mContent ) )
                return this->mContent < _value.mContent;
            else
            {
                return _value.rational() > carl::fromInt<T>(this->mContent);
            }
        }
        else
        {
            if( IS_INT( _value.mContent ) )
            {
                return this->rational() < carl::fromInt<T>(_value.mContent);
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
    template<typename T>
    bool Numeric<T>::operator<=( const Numeric<T>& _value ) const
    {
        if( IS_INT( this->mContent ) )
        {
            if( IS_INT( _value.mContent ) )
                return this->mContent <= _value.mContent;
            else
            {
                return _value.rational() >= carl::fromInt<T>(this->mContent);
            }
        }
        else
        {
            if( IS_INT( _value.mContent ) )
            {
                return this->rational() <= carl::fromInt<T>(_value.mContent);
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
    template<typename T>
    bool Numeric<T>::operator>( const Numeric<T>& _value ) const
    {
        if( IS_INT( this->mContent ) )
        {
            if( IS_INT( _value.mContent ) )
                return this->mContent > _value.mContent;
            else
            {
                return _value.rational() < carl::fromInt<T>(this->mContent);
            }
        }
        else
        {
            if( IS_INT( _value.mContent ) )
            {
                return this->rational() > carl::fromInt<T>(_value.mContent);
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
    template<typename T>
    bool Numeric<T>::operator>=( const Numeric<T>& _value ) const
    {
        if( IS_INT( this->mContent ) )
        {
            if( IS_INT( _value.mContent ) )
                return this->mContent >= _value.mContent;
            else
            {
                return _value.rational() <= carl::fromInt<T>(this->mContent);
            }
        }
        else
        {
            if( IS_INT( _value.mContent ) )
            {
                return this->rational() >= carl::fromInt<T>(_value.mContent);
            }
            else
                return this->rational() >= _value.rational();
        }
    }
    
    /**
     * @param _valueA An integer.
     * @param _valueB An integer.
     * @result The quotient of this integer and the given integer.
     */
    template<typename T>
    Numeric<T>& div_assign( Numeric<T>& _valueA, const Numeric<T>& _valueB )
    {
        assert( isInteger( _valueA ) );
        assert( isInteger( _valueB ) );
        assert( abs( _valueA ) >= abs( _valueB ) );
        if( IS_INT( _valueA.mContent ) )
        {
            assert( IS_INT( _valueB.mContent ) );
            _valueA.mContent /= _valueB.mContent;
        }
        else
        {
            T& rat = _valueA.rRational();
            if( IS_INT( _valueB.mContent ) )
                rat = carl::div( carl::getNum( rat ), carl::fromInt<typename IntegralType<T>::type>( _valueB.mContent ) );
            else
                rat = carl::div( carl::getNum( rat ), carl::getNum( _valueB.rational() ) );
            if( Numeric<T>::withinNativeRange(rat) )
            {
                Numeric<T>::mFreeRationalIds.push_back( _valueA.mContent );
                _valueA.mContent = carl::toInt<ContentType>( carl::getNum( rat ) );
            }
        }
        return _valueA;
    }
    
    template<typename T>
    void Numeric<T>::gcd_( ContentType& _a, ContentType _b )
    {
        assert( _a > 0 && _b > 0 );
        ContentType c;
        while( _b != 0 )
        {
            c = _b;
            _b = _a%_b;
            _a = c;
        }
    }
  
    template<typename T>
    Numeric<T>& gcd_assign( Numeric<T>& _valueA, const Numeric<T>& _valueB )
    {
        assert( isInteger( _valueA ) );
        assert( isInteger( _valueB ) );
        if( _valueA.mContent == 0 || _valueB.mContent == 0 )
        {
            if( !(IS_INT( _valueA.mContent )) )
                Numeric<T>::mFreeRationalIds.push_back( _valueA.mContent );
            _valueA.mContent = 0;
        }
        if( IS_INT( _valueA.mContent ) )
        {
            _valueA.mContent = std::abs( _valueA.mContent );
            if( IS_INT( _valueB.mContent ) )
            {
                Numeric<T>::gcd_( _valueA.mContent, std::abs( _valueB.mContent ) );
            }
            else
            {
                typename IntegralType<T>::type a = carl::abs( carl::getNum( _valueB.rational() ) );
                a = carl::mod( a, carl::fromInt<typename IntegralType<T>::type>(_valueA.mContent) );
                if( a != 0 )
                    Numeric<T>::gcd_( _valueA.mContent, carl::toInt<ContentType>( a ) );
            }
        }
        else
        {
            if( IS_INT( _valueB.mContent ) )
            {
                typename IntegralType<T>::type a = carl::abs( carl::getNum( _valueA.rational() ) );
                Numeric<T>::mFreeRationalIds.push_back( _valueA.mContent );
                _valueA.mContent = std::abs( _valueB.mContent );
                a = carl::mod( a, carl::fromInt<typename IntegralType<T>::type>(_valueA.mContent) );
                if( a != 0 )
                    Numeric<T>::gcd_( _valueA.mContent, carl::toInt<ContentType>( a ) );
            }
            else
            {
                _valueA.maybeIntegralize( carl::gcd( carl::getNum( _valueA.rational() ), carl::getNum( _valueB.rational() ) ) );
            }
        }
        return _valueA;
    }
    
    /**
     * @param _valueA An integer.
     * @param _valueB An integer.
     * @result The quotient of the two given integer.
     */
    template<typename T>
    Numeric<T> div( const Numeric<T>& _valueA, const Numeric<T>& _valueB )
    {
        assert( isInteger( _valueA ) );
        assert( isInteger( _valueB ) );
        assert( abs( _valueA ) >= abs( _valueB ) );
        if( IS_INT( _valueA.content() ) )
        {
            assert( IS_INT( _valueB.content() ) );
            return Numeric<T>(_valueA.content() / _valueB.content());
        }
        else
        {
            if( IS_INT( _valueB.content() ) )
            {
                return Numeric<T>( carl::div( carl::getNum( _valueA.rational() ), carl::fromInt<typename IntegralType<T>::type>( _valueB.content() ) ) );
            }
            else
                return Numeric<T>( carl::div( carl::getNum( _valueA.rational() ), carl::getNum( _valueB.rational() ) ) );
        }
    }

    /**
     * Calculates the absolute value of the given Numeric.
     * @param _value The Numeric to calculate the Numeric for.
     * @return The absolute value of the given Numeric.
     */
    template<typename T>
    Numeric<T> abs( const Numeric<T>& _value )
    {
        if( IS_INT( _value.content() ) )
        {
            return Numeric<T>( std::abs( _value.content() ), true );
        }
        else
        {
            return Numeric<T>( carl::abs( _value.rational() ) ); 
        }
    }

    /**
     * Calculates the least common multiple of the two arguments.
     * Note, that this method can only be applied to integers.
     * @param _valueA An integer.
     * @param _valueB An integer.
     * @return The least common multiple of the two arguments.
     */
    template<typename T>
    Numeric<T> lcm( const Numeric<T>& _valueA, const Numeric<T>& _valueB )
    {
        assert( isInteger( _valueA ) );
        assert( isInteger( _valueB ) );
        if( IS_INT( _valueA.content() ) )
        {
            if( IS_INT( _valueB.content() ) )
            {
                ContentType g = std::abs( _valueA.content() );
                Numeric<T>::gcd_( g, std::abs( _valueB.content() ) );
                return Numeric<T>( (std::abs(_valueA.content()*_valueB.content())/g) );
            }
            else
                return Numeric<T>( carl::lcm( carl::fromInt<typename IntegralType<T>::type>( _valueA.content() ), carl::getNum( _valueB.rational() ) ) );
        }
        else
        {
            if( IS_INT( _valueB.content() ) )
                return Numeric<T>( carl::lcm( carl::getNum( _valueA.rational() ), carl::fromInt<typename IntegralType<T>::type>( _valueB.content() ) ) );
            else
                return Numeric<T>( carl::lcm( carl::getNum( _valueA.rational() ), carl::getNum( _valueB.rational() ) ) );
        }
    }

    /**
     * Calculates the greatest common divisor of the two arguments.
     * Note, that this method can only be applied to integers.
     * @param _valueA An integer.
     * @param _valueB An integer.
     * @return The least common divisor of the two arguments.
     */
    template<typename T>
    Numeric<T> gcd( const Numeric<T>& _valueA, const Numeric<T>& _valueB )
    {
        Numeric<T> result( _valueA );
        return gcd_assign( result, _valueB );
    }
    
    /**
     * Calculates the sum of the two given Numerics.
     * @param _valueA The first summand.
     * @param _valueB The second summand.
     * @return The sum of the two given Numerics.
     */
    template<typename T>
    Numeric<T> operator+( const Numeric<T>& _valueA, const Numeric<T>& _valueB )
    {
        Numeric<T> result( _valueA );
        result += _valueB;
        return result;
    }

    /**
     * Calculates the difference between the two given Numerics.
     * @param _valueA The minuend.
     * @param _valueB The subtrahend.
     * @return The difference between the two given Numerics.
     */
    template<typename T>
    Numeric<T> operator-( const Numeric<T>& _valueA, const Numeric<T>& _valueB )
    {
        Numeric<T> result( _valueA );
        result -= _valueB;
        return result;
    }

    /**
     * Calculates the product of the two given Numerics.
     * @param _valueA The first factor.
     * @param _valueB The second factor.
     * @return The product of the two given Numerics.
     */
    template<typename T>
    Numeric<T> operator*( const Numeric<T>& _valueA, const Numeric<T>& _valueB )
    {
        Numeric<T> result( _valueA );
        result *= _valueB;
        return result;
    }

    /**
     * Calculates the division of the two given Numerics.
     * @param _valueA The dividend.
     * @param _valueB The divisor.
     * @return The difference of the two given Numerics.
     */
    template<typename T>
    Numeric<T> operator/( const Numeric<T>& _valueA, const Numeric<T>& _valueB )
    {
        Numeric<T> result( _valueA );
        result /= _valueB;
        return result;
    }
    
    /**
     * Adds the value of the second given Numeric to the second given Numeric.
     * @param _valueA The Numeric to add to.
     * @param _valueB The Numeric to add.
     * @return The first given Numeric increased by the second given Numeric.
     */
    template<typename T>
    Numeric<T>& operator+=( Numeric<T>& _valueA, const Numeric<T>& _valueB )
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
                T rat = _valueB.rational();
                rat += fromInt<T>(_valueA.mContent);
                _valueA.maybeRationalize( rat );
            }
        }
        else
        {
            T& rat = _valueA.rRational();
            if( IS_INT( _valueB.mContent ) )
            {
                rat += fromInt<T>(_valueB.mContent);
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
    template<typename T>
    Numeric<T>& operator-=( Numeric<T>& _valueA, const Numeric<T>& _valueB )
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
                T rat = -_valueB.rational();
                rat += carl::fromInt<T>(_valueA.mContent);
                _valueA.maybeRationalize( rat );
            }
        }
        else
        {
            T& rat = _valueA.rRational();
            if( IS_INT( _valueB.mContent ) )
            {
                rat -= carl::fromInt<T>(_valueB.mContent);
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
    template<typename T>
    Numeric<T>& operator*=( Numeric<T>& _valueA, const Numeric<T>& _valueB )
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
                T rat = _valueB.rational();
                rat *= carl::fromInt<T>(_valueA.mContent);
                _valueA.maybeRationalize( rat );
            }
        }
        else
        {
            T& rat = _valueA.rRational();
            if( IS_INT( _valueB.mContent ) )
            {
                rat *= carl::fromInt<T>(_valueB.mContent);
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
    template<typename T>
    Numeric<T>& operator/=( Numeric<T>& _valueA, const Numeric<T>& _valueB )
    {
        if( IS_INT( _valueA.mContent ) )
        {
            T rat = carl::fromInt<T>(_valueA.mContent);
            if( IS_INT( _valueB.mContent ) )
            {
                rat /= carl::fromInt<T>(_valueB.mContent);
            }
            else
            {
                rat /= _valueB.rational();
            }
            _valueA.maybeRationalize( rat );
        }
        else
        {
            T& rat = _valueA.rRational();
            if( IS_INT( _valueB.mContent ) )
            {
                rat /= carl::fromInt<T>(_valueB.mContent);
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
    template<typename T>
    Numeric<T> operator-( const Numeric<T>& _value )
    {
        if( IS_INT( _value.content() ) )
            return Numeric<T>( -_value.content(), true );
        else
            return Numeric<T>( -_value.rational() );
    }

    /**
     * Increments the given Numeric by one.
     * @param _value The Numeric to increment.
     * @return The given Numeric incremented by one.
     */
    template<typename T>
    Numeric<T>& operator++( Numeric<T>& _value )
    {
        if( IS_INT( _value.mContent ) )
        {
            ++(_value.mContent);
            _value.maybeRationalize();
        }
        else
        {
            ++(_value.rRational());
            _value.maybeIntegralize();
        }
        return _value;
    }

    /**
     * Decrements the given Numeric by one.
     * @param _value The Numeric to decrement.
     * @return The given Numeric decremented by one.
     */
    template<typename T>
    Numeric<T>& operator--( Numeric<T>& _value )
    {
        if( IS_INT( _value.mContent ) )
        {
            --(_value.mContent);
            _value.maybeRationalize();
        }
        else
        {
            --(_value.rRational());
            _value.maybeIntegralize();
        }
        return _value;
    }

    /**
     * Prints the given Numerics representation on the given output stream.
     * @param _out The output stream to print on.
     * @param _value The Numeric to print.
     * @return The output stream after printing the given Numerics representation on it.
     */
    template<typename T>
    std::ostream& operator <<( std::ostream& _out, const Numeric<T>& _value )
    {
        if( IS_INT( _value.content() ) )
            _out << _value.content();
        else
            _out << _value.rational();
        return _out;
    }
} // end namespace carl
