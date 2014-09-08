/* 
 * File:   FactorizedPolynomial.tpp
 * Author: Florian Corzilius
 *
 * Created on September 4, 2014, 10:55 AM
 */

#include "FactorizedPolynomial.h"

#pragma once

namespace carl
{
    template<typename P>
    FactorizedPolynomial<P>::FactorizedPolynomial( const P& _polynomial, Cache<PolynomialFactorizationPair<P>>& _cache ):
        mCacheRef( 0 ),
        mrCache( _cache )
    {
        Factorization<P> factorization;
        PolynomialFactorizationPair<P>* pfPair = new PolynomialFactorizationPair<P>( std::move( factorization), new P( _polynomial ) );
        mCacheRef = mrCache.cache( pfPair );
        pfPair->rFactorization().insert( std::make_pair( *this, 1 ) );
    }
    
    template<typename P>
    FactorizedPolynomial<P>::FactorizedPolynomial( const P& _polynomial, Factorization<P>&& _factorization, Cache<PolynomialFactorizationPair<P>>& _cache ):
        mCacheRef( 0 ),
        mrCache( _cache )
    {
        mCacheRef = mrCache.cache( new PolynomialFactorizationPair<P>( std::move( _factorization ), new P( _polynomial ) ) );
    }
    
    template<typename P>
    FactorizedPolynomial<P>::FactorizedPolynomial( Factorization<P>&& _factorization, Cache<PolynomialFactorizationPair<P>>& _cache ):
        mCacheRef( 0 ),
        mrCache( _cache )
    {
        mCacheRef = mrCache.cache( new PolynomialFactorizationPair<P>( std::move( _factorization ) ) );
    }
    
    template<typename P>
    FactorizedPolynomial<P>::FactorizedPolynomial( const FactorizedPolynomial<P>& _toCopy ):
        mCacheRef( _toCopy.mCacheRef ),
        mrCache( _toCopy.mrCache )
    {
        mrCache.reg( _toCopy.mCacheRef );
    }
    
    template<typename P>
    FactorizedPolynomial<P>::~FactorizedPolynomial()
    {
        mrCache.dereg( mCacheRef );
    }
    
    template<typename P>
    FactorizedPolynomial<P>& FactorizedPolynomial<P>::operator=( const FactorizedPolynomial<P>& _fpoly )
    {
        assert( &mrCache == &_fpoly.mrCache );
        mCacheRef = _fpoly.mCacheRef;
        return *this;
    }
        
    template<typename P>
    bool operator==(const FactorizedPolynomial<P>& _fpolyA, const FactorizedPolynomial<P>& _fpolyB)
    {
        assert( &_fpolyA.mrCache == &_fpolyB.mrCache );
        return _fpolyA.mrCache.get( _fpolyA.mCacheRef ) == _fpolyB.mrCache.get( _fpolyB.mCacheRef );
    }
        
    template<typename P>
    bool operator<(const FactorizedPolynomial<P>& _fpolyA, const FactorizedPolynomial<P>& _fpolyB)
    {
        assert( &_fpolyA.mrCache == &_fpolyB.mrCache );
        return _fpolyA.mrCache.get( _fpolyA.mCacheRef ) < _fpolyB.mrCache.get( _fpolyB.mCacheRef );
    }

    template<typename P>
    const FactorizedPolynomial<P> operator+(const FactorizedPolynomial<P>& _fpolyA, const FactorizedPolynomial<P>& _fpolyB)
    {
        assert( &_fpolyA.mrCache == &_fpolyB.mrCache );

    }

    template<typename P>
    const FactorizedPolynomial<P> operator-(const FactorizedPolynomial<P>& _fpolyA, const FactorizedPolynomial<P>& _fpolyB)
    {
        assert( &_fpolyA.mrCache == &_fpolyB.mrCache );

    }

    template<typename P>
    const FactorizedPolynomial<P> operator*(const FactorizedPolynomial<P>& _fpolyA, const FactorizedPolynomial<P>& _fpolyB)
    {
        assert( &_fpolyA.mrCache == &_fpolyB.mrCache );

    }

    template<typename P>
    const FactorizedPolynomial<P> operator/(const FactorizedPolynomial<P>& _fpolyA, const FactorizedPolynomial<P>& _fpolyB)
    {
        assert( &_fpolyA.mrCache == &_fpolyB.mrCache );
    }

    template<typename P>
    const FactorizedPolynomial<P> commonDivisor(const FactorizedPolynomial<P>& _fpolyA, const FactorizedPolynomial<P>& _fpolyB)
    {
        assert( &_fpolyA.mrCache == &_fpolyB.mrCache );
        Factorization<P> cdFactorization;
        const Factorization<P>& factorizationA = _fpolyA.mrCache.get(_fpolyA.mCacheRef).factorization();
        const Factorization<P>& factorizationB = _fpolyB.mrCache.get(_fpolyB.mCacheRef).factorization();
        auto factorA = factorizationA.begin();
        auto factorB = factorizationB.begin();
        while( factorA != factorizationA.end() && factorB != factorizationB.end() )
        {
            if( factorA->first == factorB->first )
                cdFactorization.insert( cdFactorization.end(), std::pair<FactorizedPolynomial<P>, size_t>( factorA->first, factorA->second < factorB->second ? factorA->second : factorB->second ) );
            else if( factorA->first < factorB->first )
                factorA++;
            else
                factorB++;
        }
        return FactorizedPolynomial<P>( std::move( cdFactorization ), _fpolyA.mrCache );
    }

    template<typename P>
    const FactorizedPolynomial<P> commonMultiple(const FactorizedPolynomial<P>& _fpolyA, const FactorizedPolynomial<P>& _fpolyB)
    {
        
    }

    template<typename P>
    const FactorizedPolynomial<P> gcd(const FactorizedPolynomial<P>& _fpolyA, const FactorizedPolynomial<P>& _fpolyB)
    {
        
    }
    
    template <typename P>
    std::ostream& operator<<(std::ostream& _out, const FactorizedPolynomial<P>& _fpoly)
    {
        const Factorization<P>& factorization = _fpoly.mrCache.get(_fpoly.mCacheRef).factorization();
        if( factorization.size() == 1 )
        {
            _out << factorization.begin()->first;
            assert( factorization.begin()->second != 0 );
            if( factorization.begin()->second > 1 )
            {
                _out << "^" << factorization.begin()->second;
            }
        }
        else
        {
            for( auto polyExpPair = factorization.begin(); polyExpPair != factorization.end(); ++polyExpPair )
            {
                if( polyExpPair != factorization.begin() )
                    _out << ") * (";
                else
                    _out << "(";
                _out << polyExpPair->first;
                assert( polyExpPair->second != 0 );
                if( polyExpPair->second > 1 )
                {
                    _out << "^" << polyExpPair->second;
                }
            }
            _out << ")";
        }
        return _out;
    }
} // namespace carl