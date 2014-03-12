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
#include <cln/cln.h>
#include "numbers.h"

namespace carl
{
    typedef int ContentType;
    const ContentType HIGHTEST_INTEGER_VALUE = 1000000;
    
    class Numeric
    {   
    private:
        // Members:
        ContentType mContent;

    public:
        
        
        static std::vector<cln::cl_RA> mRationalPool;
        static std::vector<size_t>     mFreeRationalIds;
        
        // Constructors/Destructor:
        
        /**
         * Default constructor.
         */
        Numeric();
        Numeric( const cln::cl_RA& );
        template<bool is_definitely_int = false>
        Numeric( ContentType );
        Numeric( const Numeric& );
        ~Numeric();
        
        ContentType content() const
        {
            return mContent;
        }

        // Methods:

        Numeric& operator=( const cln::cl_RA& );
        Numeric& operator=( ContentType );
        Numeric& operator=( const Numeric& );

        bool operator==( const Numeric& ) const;
        bool operator!=( const Numeric& ) const;
        bool operator<( const Numeric& ) const;
        bool operator<=( const Numeric& ) const;
        bool operator>( const Numeric& ) const;
        bool operator>=( const Numeric& ) const;
        
        Numeric divide( const Numeric& ) const;
        Numeric& divideBy( const Numeric& );
        
        friend Numeric& operator+=( Numeric&, const Numeric& );
        friend Numeric& operator-=( Numeric&, const Numeric& );
        friend Numeric& operator*=( Numeric&, const Numeric& );
        friend Numeric& operator/=( Numeric&, const Numeric& );
        
        friend Numeric& operator++( Numeric& );
        friend Numeric& operator--( Numeric& );
        
        friend Numeric operator-( const Numeric& );
        
        friend Numeric abs( const Numeric& );
        friend Numeric lcm( const Numeric&, const Numeric& );
        friend Numeric gcd( const Numeric&, const Numeric& );
        
        friend std::ostream& operator <<( std::ostream&, const Numeric& );
        
        static ContentType allocate( const cln::cl_RA& );
        static ContentType allocate( ContentType );
        
        cln::cl_RA toRational() const
        {
            if( std::abs( this->mContent ) < HIGHTEST_INTEGER_VALUE )
                return cln::cl_RA( this->mContent );
            else
                return mRationalPool[(size_t)this->mContent-(size_t)HIGHTEST_INTEGER_VALUE];
        }
        
        Numeric& gcd( const Numeric& );
        
    private:
        
        inline const cln::cl_RA& rational() const
        {
            assert( std::abs( this->mContent ) >= HIGHTEST_INTEGER_VALUE );
            return mRationalPool[(size_t)this->mContent-(size_t)HIGHTEST_INTEGER_VALUE];
        }
        
        inline cln::cl_RA& rRational()
        {
            assert( std::abs( this->mContent ) >= HIGHTEST_INTEGER_VALUE );
            return mRationalPool[(size_t)this->mContent-(size_t)HIGHTEST_INTEGER_VALUE];
        }
        
        inline void maybeRationalize()
        {
            if( std::abs( this->mContent ) >= HIGHTEST_INTEGER_VALUE )
                this->mContent = allocate( this->mContent );
        }
        
        inline void maybeRationalize( const cln::cl_RA& _rat )
        {
            if( carl::isInteger( _rat ) && carl::abs( carl::getNum( _rat ) ) < HIGHTEST_INTEGER_VALUE )
            {
                this->mContent = carl::toInt<ContentType>( carl::getNum( _rat ) );
            }
            else
            {
                this->mContent = allocate( _rat );
            }
        }

        inline void maybeIntegralize( const cln::cl_RA& _rat )
        {
            if( carl::isInteger( _rat ) && carl::abs( carl::getNum( _rat ) ) < HIGHTEST_INTEGER_VALUE )
            {
                mFreeRationalIds.push_back( this->mContent );
                this->mContent = carl::toInt<ContentType>( carl::getNum( _rat ) );
            }
        }
    };

    Numeric abs( const Numeric& );
    Numeric lcm( const Numeric&, const Numeric& );
    Numeric gcd( const Numeric&, const Numeric& );
    Numeric operator+( const Numeric&, const Numeric& );
    Numeric operator-( const Numeric&, const Numeric& );
    Numeric operator*( const Numeric&, const Numeric& );
    Numeric operator/( const Numeric&, const Numeric& );
    Numeric& operator+=( Numeric&, const Numeric& );
    Numeric& operator-=( Numeric&, const Numeric& );
    Numeric& operator*=( Numeric&, const Numeric& );
    Numeric& operator/=( Numeric&, const Numeric& );
    Numeric operator-( const Numeric& );
    Numeric& operator++( Numeric& );
    Numeric& operator--( Numeric& );
    std::ostream& operator <<( std::ostream&, const Numeric& );
    
    #define IS_INT( value ) abs( value ) < HIGHTEST_INTEGER_VALUE
    
    inline bool isInteger( const Numeric& _value )
    {
        if( std::abs( _value.content() ) < HIGHTEST_INTEGER_VALUE )
            return true;
        else
            return carl::isInteger( Numeric::mRationalPool[(size_t)_value.content()-(size_t)HIGHTEST_INTEGER_VALUE] );
    }
} // namespace carl

