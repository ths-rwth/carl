/**
 * @file Numeric.h
 * @author Florian Corzilius <corzilius@cs.rwth-aachen.de>
 *
 * @version 2014-03-11
 * Created on 2014-03-11
 */

#pragma once

#include <iostream>
#include <assert.h>
#include <vector>
#include <limits>
#include "numbers.h"

namespace carl
{
    typedef long int ContentType;
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
        Numeric( const T& );
        Numeric( ContentType, bool = false );
        Numeric( const Numeric<T>& );
        ~Numeric();
        
        ContentType content() const
        {
            return mContent;
        }

        // Methods:

        Numeric<T>& operator=( const T& );
        Numeric<T>& operator=( ContentType );
        Numeric<T>& operator=( const Numeric<T>& );

        bool operator==( const Numeric<T>& ) const;
        bool operator!=( const Numeric<T>& ) const;
        bool operator<( const Numeric<T>& ) const;
        bool operator<=( const Numeric<T>& ) const;
        bool operator>( const Numeric<T>& ) const;
        bool operator>=( const Numeric<T>& ) const;
        
        template<typename T1>
        friend Numeric<T1>& div_assign( Numeric<T1>&, const Numeric<T1>& );
        template<typename T1>
        friend Numeric<T1>& gcd_assign( Numeric<T1>&, const Numeric<T1>& );
        
        template<typename T1>
        friend Numeric<T1>& operator+=( Numeric<T1>&, const Numeric<T1>& );
        template<typename T1>
        friend Numeric<T1>& operator-=( Numeric<T1>&, const Numeric<T1>& );
        template<typename T1>
        friend Numeric<T1>& operator*=( Numeric<T1>&, const Numeric<T1>& );
        template<typename T1>
        friend Numeric<T1>& operator/=( Numeric<T1>&, const Numeric<T1>& );
        
        template<typename T1>
        friend Numeric<T1>& operator++( Numeric<T1>& );
        template<typename T1>
        friend Numeric<T1>& operator--( Numeric<T1>& );
        
        template<typename T1>
        friend Numeric<T1> operator-( const Numeric<T1>& );
        
        template<typename T1>
        friend Numeric<T1> div( const Numeric<T1>&, const Numeric<T1>& );
        template<typename T1>
        friend Numeric<T1> abs( const Numeric<T1>& );
        template<typename T1>
        friend Numeric<T1> lcm( const Numeric<T1>&, const Numeric<T1>& );
        template<typename T1>
        friend Numeric<T1> gcd( const Numeric<T1>&, const Numeric<T1>& );
        
        template<typename T1>
        friend std::ostream& operator <<( std::ostream&, const Numeric<T1>& );
        
        static ContentType allocate( const T& );
        static ContentType allocate( ContentType );
        static void gcd_( ContentType&, ContentType );
        
        T toRational() const
        {
            if( std::abs( this->mContent ) < HIGHTEST_INTEGER_VALUE )
                return T( this->mContent );
            else
                return mRationalPool[(size_t)this->mContent-(size_t)HIGHTEST_INTEGER_VALUE];
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
                T& rat = mRationalPool[(size_t)this->mContent-(size_t)HIGHTEST_INTEGER_VALUE];
                assert( carl::isInteger( rat ) );
                return carl::getNum( rat );
            }
        }
        
    private:
        
        inline const T& rational() const
        {
            assert( std::abs( this->mContent ) >= HIGHTEST_INTEGER_VALUE );
            return mRationalPool[(size_t)this->mContent-(size_t)HIGHTEST_INTEGER_VALUE];
        }
        
        inline T& rRational()
        {
            assert( std::abs( this->mContent ) >= HIGHTEST_INTEGER_VALUE );
            return mRationalPool[(size_t)this->mContent-(size_t)HIGHTEST_INTEGER_VALUE];
        }
        
        inline void maybeRationalize()
        {
            if( !(std::abs( this->mContent ) < HIGHTEST_INTEGER_VALUE) )
                this->mContent = allocate( this->mContent );
        }
        
        inline void maybeRationalize( const T& _rat )
        {
            if( carl::isInteger( _rat ) && carl::getNum( _rat ) > -HIGHTEST_INTEGER_VALUE && carl::getNum( _rat ) < HIGHTEST_INTEGER_VALUE )
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
            if( carl::isInteger( rat ) && carl::getNum( rat ) > -HIGHTEST_INTEGER_VALUE && carl::getNum( rat ) < HIGHTEST_INTEGER_VALUE )
            {
                mFreeRationalIds.push_back( this->mContent );
                this->mContent = carl::toInt<ContentType>( carl::getNum( rat ) );
            }
        }

        inline void maybeIntegralize( const T& _rat )
        {
            if( carl::isInteger( _rat ) && carl::getNum( _rat ) > -HIGHTEST_INTEGER_VALUE && carl::getNum( _rat ) < HIGHTEST_INTEGER_VALUE )
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
    Numeric<T>& div_assign( Numeric<T>&, const Numeric<T>& );
    template<typename T>
    Numeric<T>& gcd_assign( Numeric<T>&, const Numeric<T>& );
    template<typename T>
    Numeric<T> div( const Numeric<T>&, const Numeric<T>& );
    template<typename T>
    Numeric<T> abs( const Numeric<T>& );
    template<typename T>
    Numeric<T> lcm( const Numeric<T>&, const Numeric<T>& );
    template<typename T>
    Numeric<T> gcd( const Numeric<T>&, const Numeric<T>& );
    template<typename T>
    Numeric<T> operator+( const Numeric<T>&, const Numeric<T>& );
    template<typename T>
    Numeric<T> operator-( const Numeric<T>&, const Numeric<T>& );
    template<typename T>
    Numeric<T> operator*( const Numeric<T>&, const Numeric<T>& );
    template<typename T>
    Numeric<T> operator/( const Numeric<T>&, const Numeric<T>& );
    template<typename T>
    Numeric<T>& operator+=( Numeric<T>&, const Numeric<T>& );
    template<typename T>
    Numeric<T>& operator-=( Numeric<T>&, const Numeric<T>& );
    template<typename T>
    Numeric<T>& operator*=( Numeric<T>&, const Numeric<T>& );
    template<typename T>
    Numeric<T>& operator/=( Numeric<T>&, const Numeric<T>& );
    template<typename T>
    Numeric<T> operator-( const Numeric<T>& );
    template<typename T>
    Numeric<T>& operator++( Numeric<T>& );
    template<typename T>
    Numeric<T>& operator--( Numeric<T>& );
    template<typename T>
    std::ostream& operator <<( std::ostream&, const Numeric<T>& );
    
    #define IS_INT( value ) std::abs( value ) < HIGHTEST_INTEGER_VALUE
    
    template<typename T>
    inline bool isInteger( const Numeric<T>& _value )
    {
        if( std::abs( _value.content() ) < HIGHTEST_INTEGER_VALUE )
            return true;
        else
            return carl::isInteger( Numeric<T>::mRationalPool[(size_t)_value.content()-(size_t)HIGHTEST_INTEGER_VALUE] );
    }
    
//    template<>
//    inline void reserve<Numeric<cln::cl_RA>>( size_t _expectedNumOfElements )
//    {
//        Numeric<cln::cl_RA>::mRationalPool.reserve( Numeric<cln::cl_RA>::mRationalPool.capacity() + _expectedNumOfElements );
//    }
} // namespace carl


#include "Numeric.tpp"
