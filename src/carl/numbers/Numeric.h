/**
 * @file Numeric.h
 * @author Florian Corzilius <corzilius@cs.rwth-aachen.de>
 *
 * @version 2014-03-11
 * Created on 2014-03-11
 */

// WARNING: THIS CLASS IS NOT YET THREAD SAFE!!!

#pragma once

#include "numbers.h"

#include <iostream>
#include <limits>
#include <vector>

namespace carl
{
    using ContentType = sint;
    const ContentType HIGHTEST_INTEGER_VALUE = (std::numeric_limits<ContentType>::max() >> ((std::numeric_limits<ContentType>::digits/2)+1))+1;
    
    template<typename T>
    class Numeric
    {   
    private:
        // Members:
        ContentType mContent;

    public:
        
        static std::vector<T>           mRationalPool;
        static std::vector<ContentType> mFreeRationalIds;
        
        // Constructors/Destructor:
        
        /**
         * Default constructor.
         */
        Numeric();
        Numeric( const T& /*unused*/ ); // NOLINT
        Numeric( ContentType /*unused*/, bool /*unused*/ = false ); // NOLINT
        Numeric( const Numeric<T>& /*unused*/ );
        ~Numeric();
        
        ContentType content() const
        {
            return mContent;
        }

        // Methods:

        Numeric<T>& operator=( const T& /*unused*/ );
        Numeric<T>& operator=( ContentType /*unused*/ );
        Numeric<T>& operator=( const Numeric<T>& /*unused*/ );

        bool operator==( const Numeric<T>& /*unused*/ ) const;
        bool operator!=( const Numeric<T>& /*unused*/ ) const;
        bool operator<( const Numeric<T>& /*unused*/ ) const;
        bool operator<=( const Numeric<T>& /*unused*/ ) const;
        bool operator>( const Numeric<T>& /*unused*/ ) const;
        bool operator>=( const Numeric<T>& /*unused*/ ) const;
        
        template<typename T1>
        friend Numeric<T1>& div_assign( Numeric<T1>& /*unused*/, const Numeric<T1>& /*unused*/ );
        template<typename T1>
        friend Numeric<T1>& gcd_assign( Numeric<T1>& /*unused*/, const Numeric<T1>& /*unused*/ );
        
        template<typename T1>
        friend Numeric<T1>& operator+=( Numeric<T1>& /*unused*/, const Numeric<T1>& /*unused*/ );
        template<typename T1>
        friend Numeric<T1>& operator-=( Numeric<T1>& /*unused*/, const Numeric<T1>& /*unused*/ );
        template<typename T1>
        friend Numeric<T1>& operator*=( Numeric<T1>& /*unused*/, const Numeric<T1>& /*unused*/ );
        template<typename T1>
        friend Numeric<T1>& operator/=( Numeric<T1>& /*unused*/, const Numeric<T1>& /*unused*/ );
        
        template<typename T1>
        friend Numeric<T1>& operator++( Numeric<T1>& /*unused*/ );
        template<typename T1>
        friend Numeric<T1>& operator--( Numeric<T1>& /*unused*/ );
        
        template<typename T1>
        friend Numeric<T1> operator-( const Numeric<T1>& /*unused*/ );
        
        template<typename T1>
        friend Numeric<T1> div( const Numeric<T1>& /*unused*/, const Numeric<T1>& /*unused*/ );
        template<typename T1>
        friend Numeric<T1> abs( const Numeric<T1>& /*unused*/ );
        template<typename T1>
        friend Numeric<T1> lcm( const Numeric<T1>& /*unused*/, const Numeric<T1>& /*unused*/ );
        template<typename T1>
        friend Numeric<T1> gcd( const Numeric<T1>& /*unused*/, const Numeric<T1>& /*unused*/ );
        
        template<typename T1>
        friend std::ostream& operator <<( std::ostream& /*unused*/, const Numeric<T1>& /*unused*/ );
        
        static ContentType allocate( const T& /*unused*/ );
        static ContentType allocate( ContentType /*unused*/ );
        static void gcd_( ContentType& /*unused*/, ContentType /*unused*/ );
        
        T toRational() const
        {
            if( std::abs( this->mContent ) < HIGHTEST_INTEGER_VALUE )
                return carl::fromInt<T>(this->mContent);
            else
                return mRationalPool[std::size_t(this->mContent)-std::size_t(HIGHTEST_INTEGER_VALUE)];
        }
        
        operator T() const
        {
            return this->toRational();
        }
        
        operator typename IntegralType<T>::type () const
        {
            if( std::abs( this->mContent ) < HIGHTEST_INTEGER_VALUE )
                return typename IntegralType<T>::type( this->mContent );
            else
            {
                T& rat = mRationalPool[std::size_t(this->mContent)-std::size_t(HIGHTEST_INTEGER_VALUE)];
                assert( carl::isInteger( rat ) );
                return carl::getNum( rat );
            }
        }
        
    private:
        
        inline const T& rational() const
        {
            assert( std::abs( this->mContent ) >= HIGHTEST_INTEGER_VALUE );
            return mRationalPool[std::size_t(this->mContent)-std::size_t(HIGHTEST_INTEGER_VALUE)];
        }
        
        inline T& rRational()
        {
            assert( std::abs( this->mContent ) >= HIGHTEST_INTEGER_VALUE );
            return mRationalPool[std::size_t(this->mContent)-std::size_t(HIGHTEST_INTEGER_VALUE)];
        }
        
        inline void maybeRationalize()
        {
            if( !(std::abs( this->mContent ) < HIGHTEST_INTEGER_VALUE) )
                this->mContent = allocate( this->mContent );
        }
        
        inline void maybeRationalize( const T& _rat )
        {
            if( carl::isInteger( _rat ) && carl::getNum( _rat ) > -carl::fromInt<T>(HIGHTEST_INTEGER_VALUE) && carl::getNum( _rat ) < carl::fromInt<T>(HIGHTEST_INTEGER_VALUE) )
            {
                this->mContent = carl::toInt<ContentType>( carl::getNum( _rat ) );
            }
            else
            {
                this->mContent = allocate( _rat );
            }
        }

        inline void maybeIntegralize()
        {
            const T& rat = this->rational();
            if( carl::isInteger( rat ) && carl::getNum( rat ) > -carl::fromInt<T>(HIGHTEST_INTEGER_VALUE) && carl::getNum( rat ) < carl::fromInt<T>(HIGHTEST_INTEGER_VALUE) )
            {
                mFreeRationalIds.push_back( this->mContent );
                this->mContent = carl::toInt<ContentType>( carl::getNum( rat ) );
            }
        }

        inline void maybeIntegralize( const T& _rat )
        {
            if( carl::isInteger( _rat ) && carl::getNum( _rat ) > -carl::fromInt<T>(HIGHTEST_INTEGER_VALUE) && carl::getNum( _rat ) < carl::fromInt<T>(HIGHTEST_INTEGER_VALUE) )
            {
                mFreeRationalIds.push_back( this->mContent );
                this->mContent = carl::toInt<ContentType>( carl::getNum( _rat ) );
            }
            else
            {
                this->rRational() = _rat;
            }
        }
    };

    template<typename T>
    Numeric<T>& div_assign( Numeric<T>& /*unused*/, const Numeric<T>& /*unused*/ );
    template<typename T>
    Numeric<T>& gcd_assign( Numeric<T>& /*unused*/, const Numeric<T>& /*unused*/ );
    template<typename T>
    Numeric<T> div( const Numeric<T>& /*unused*/, const Numeric<T>& /*unused*/ );
    template<typename T>
    Numeric<T> abs( const Numeric<T>& /*unused*/ );
    template<typename T>
    Numeric<T> lcm( const Numeric<T>& /*unused*/, const Numeric<T>& /*unused*/ );
    template<typename T>
    Numeric<T> gcd( const Numeric<T>& /*unused*/, const Numeric<T>& /*unused*/ );
    template<typename T>
    Numeric<T> operator+( const Numeric<T>& /*unused*/, const Numeric<T>& /*unused*/ );
    template<typename T>
    Numeric<T> operator-( const Numeric<T>& /*unused*/, const Numeric<T>& /*unused*/ );
    template<typename T>
    Numeric<T> operator*( const Numeric<T>& /*unused*/, const Numeric<T>& /*unused*/ );
    template<typename T>
    Numeric<T> operator/( const Numeric<T>& /*unused*/, const Numeric<T>& /*unused*/ );
    template<typename T>
    Numeric<T>& operator+=( Numeric<T>& /*unused*/, const Numeric<T>& /*unused*/ );
    template<typename T>
    Numeric<T>& operator-=( Numeric<T>& /*unused*/, const Numeric<T>& /*unused*/ );
    template<typename T>
    Numeric<T>& operator*=( Numeric<T>& /*unused*/, const Numeric<T>& /*unused*/ );
    template<typename T>
    Numeric<T>& operator/=( Numeric<T>& /*unused*/, const Numeric<T>& /*unused*/ );
    template<typename T>
    Numeric<T> operator-( const Numeric<T>& /*unused*/ );
    template<typename T>
    Numeric<T>& operator++( Numeric<T>& /*unused*/ );
    template<typename T>
    Numeric<T>& operator--( Numeric<T>& /*unused*/ );
    template<typename T>
    std::ostream& operator <<( std::ostream& /*unused*/, const Numeric<T>& /*unused*/ );
    
    #define IS_INT( value ) std::abs( value ) < HIGHTEST_INTEGER_VALUE
    
    template<typename T>
    inline bool isInteger( const Numeric<T>& _value )
    {
        if( std::abs( _value.content() ) < HIGHTEST_INTEGER_VALUE )
            return true;
        else
            return carl::isInteger( Numeric<T>::mRationalPool[std::size_t(_value.content())-std::size_t(HIGHTEST_INTEGER_VALUE)] );
    }
    
//    template<>
//    inline void reserve<Numeric<cln::cl_RA>>( size_t _expectedNumOfElements )
//    {
//        Numeric<cln::cl_RA>::mRationalPool.reserve( Numeric<cln::cl_RA>::mRationalPool.capacity() + _expectedNumOfElements );
//    }
    
    template<typename T>
    struct IntegralType<Numeric<T>> {
        using type = typename carl::IntegralType<T>::type;
    };
    
    template<typename Integer, typename T>
    inline Integer toInt(const Numeric<T>& n)
    {
        return carl::toInt<Integer>(T(n)); // TODO: this could be more efficient for native integer types
    }
    
} // namespace carl


#include "Numeric.tpp"
