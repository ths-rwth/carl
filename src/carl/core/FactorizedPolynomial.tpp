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
        /*
         * The following is not very nice, but we know, that the hash won't change, once the polynomial 
         * representation is fixed, so we can add the factorizations content belatedly. It is necessary to do so
         * as otherwise the factorized polynomial (this) being the only factor, is not yet cached which leads to an assertion.
         */
        pfPair->mFactorization.insert( std::make_pair( *this, 1 ) ); 
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
        assert( &mrCache == &_fpoly.cache() );
        mCacheRef = _fpoly.cacheRef();
        return *this;
    }
        
    template<typename P>
    bool operator==( const FactorizedPolynomial<P>& _fpolyA, const FactorizedPolynomial<P>& _fpolyB )
    {
        assert( &_fpolyA.cache() == &_fpolyB.cache() );
        return _fpolyA.content() == _fpolyB.content();
    }
        
    template<typename P>
    bool operator<( const FactorizedPolynomial<P>& _fpolyA, const FactorizedPolynomial<P>& _fpolyB )
    {
        assert( &_fpolyA.cache() == &_fpolyB.cache() );
        return _fpolyA.content() < _fpolyB.content();
    }

    template<typename P>
    const FactorizedPolynomial<P> operator+( const FactorizedPolynomial<P>& _fpolyA, const FactorizedPolynomial<P>& _fpolyB )
    {
        _fpolyA.strengthenActivity();
        _fpolyB.strengthenActivity();
        assert( &_fpolyA.cache() == &_fpolyB.cache() );
        // TODO: implementation
    }

    template<typename P>
    const FactorizedPolynomial<P> operator-( const FactorizedPolynomial<P>& _fpolyA, const FactorizedPolynomial<P>& _fpolyB )
    {
        _fpolyA.strengthenActivity();
        _fpolyB.strengthenActivity();
        assert( &_fpolyA.cache() == &_fpolyB.cache() );
        // TODO: implementation
    }

    template<typename P>
    const FactorizedPolynomial<P> operator*( const FactorizedPolynomial<P>& _fpolyA, const FactorizedPolynomial<P>& _fpolyB )
    {
        _fpolyA.strengthenActivity();
        _fpolyB.strengthenActivity();
        assert( &_fpolyA.cache() == &_fpolyB.cache() );
        // TODO: implementation
    }

    template<typename P>
    const FactorizedPolynomial<P> operator/( const FactorizedPolynomial<P>& _fpolyA, const FactorizedPolynomial<P>& _fpolyB )
    {
        _fpolyA.strengthenActivity();
        _fpolyB.strengthenActivity();
        assert( &_fpolyA.cache() == &_fpolyB.cache() );
        // TODO: implementation
    }

    template<typename P>
    const FactorizedPolynomial<P> commonDivisor( const FactorizedPolynomial<P>& _fpolyA, const FactorizedPolynomial<P>& _fpolyB )
    {
        _fpolyA.strengthenActivity();
        _fpolyB.strengthenActivity();
        assert( &_fpolyA.cache() == &_fpolyB.cache() );
        Factorization<P> cdFactorization;
        const Factorization<P>& factorizationA = _fpolyA.content().factorization();
        const Factorization<P>& factorizationB = _fpolyB.content().factorization();
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
    const FactorizedPolynomial<P> commonMultiple( const FactorizedPolynomial<P>& _fpolyA, const FactorizedPolynomial<P>& _fpolyB )
    {
        _fpolyA.strengthenActivity();
        _fpolyB.strengthenActivity();
        // TODO: implementation
    }

    template<typename P>
    const FactorizedPolynomial<P> gcd( const FactorizedPolynomial<P>& _fpolyA, const FactorizedPolynomial<P>& _fpolyB )
    {
        _fpolyA.strengthenActivity();
        _fpolyB.strengthenActivity();
        bool rehashFPolyA = false;
        bool rehashFPolyB = false;
        Factorization<P> gcdFactorization( gcd( _fpolyA.content(), _fpolyB.content(), rehashFPolyA, rehashFPolyB ) );
        if( rehashFPolyA )
            _fpolyA.rehash();
        if( rehashFPolyB )
            _fpolyB.rehash();
        return FactorizedPolynomial<P>( std::move( gcdFactorization ), _fpolyA.mrCache );
    }
    
    template <typename P>
    std::ostream& operator<<( std::ostream& _out, const FactorizedPolynomial<P>& _fpoly )
    {
        return (_out << _fpoly.content());
    }
} // namespace carl