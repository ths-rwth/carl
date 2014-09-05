/* 
 * File:   PolynomialFactorizationPair.tpp
 * Author: Florian Corzilius
 *
 * Created on September 5, 2014, 3:57 PM
 */

#pragma once

#include "PolynomialFactorizationPair.h"
#include "FactorizedPolynomial.h"

namespace carl
{   
    template<typename P>
    PolynomialFactorizationPair<P>::PolynomialFactorizationPair( Factorization<P>&& _factorization, P* _polynomial ):
        mHash( 0 ),
        mpPolynomial( _polynomial ),
        mFactorization( std::move( _factorization ) )
    {
        rehash();
    }
    
    template<typename P>
    void PolynomialFactorizationPair<P>::rehash()
    {
        if( mpPolynomial == nullptr )
        {
            assert( mFactorization.empty() );
            mHash = 0;
            for( auto polyExpPair = mFactorization.begin(); polyExpPair != mFactorization.end(); ++polyExpPair )
            {
                mHash = (mHash << 5) | (mHash >> (sizeof(size_t)*8 - 5));
                mHash ^= std::hash<FactorizedPolynomial<P>>()( polyExpPair->first );
                mHash ^= polyExpPair->second;
            }
        }
        else
        {
            mHash = std::hash<P>()( *mpPolynomial );
        }
    }
    
    template<typename P>
    bool operator==( const PolynomialFactorizationPair<P>& _polyFactA, const PolynomialFactorizationPair<P>& _polyFactB )
    {
        if( _polyFactA.mpPolynomial != nullptr && _polyFactB.mpPolynomial != nullptr )
        {
            return *_polyFactA.mpPolynomial == *_polyFactB.mpPolynomial;
        }
        else
        {
            auto iterA = _polyFactA.mFactorization.begin();
            auto iterB = _polyFactB.mFactorization.begin();
            while( iterA != _polyFactA.mFactorization.end() && iterB != _polyFactB.mFactorization.end() )
            {
                if( iterA->second != iterB->second || !(iterA->first == iterB->first) )
                    break;
                ++iterA; ++iterB;
            }
            return iterA == _polyFactA.mFactorization.end() && iterB == _polyFactB.mFactorization.end();
        }
    }
    
    template<typename P>
    bool operator<( const PolynomialFactorizationPair<P>& _polyFactA, const PolynomialFactorizationPair<P>& _polyFactB )
    {
        if( _polyFactA.mpPolynomial != nullptr && _polyFactB.mpPolynomial != nullptr )
        {
            return *_polyFactA.mpPolynomial < *_polyFactB.mpPolynomial;
        }
        else
        {
            auto iterA = _polyFactA.mFactorization.begin();
            auto iterB = _polyFactB.mFactorization.begin();
            while( iterA != _polyFactA.mFactorization.end() && iterB != _polyFactB.mFactorization.end() )
            {
                if( iterA->first < iterB->first )
                    return true;
                else if( iterA->first == iterB->first )
                {
                    if( iterA->second < iterB->second )
                        return true;
                    else if( iterA->second > iterB->second )
                        return false;
                }
                else
                    return false;
                ++iterA; ++iterB;
            }
            return false;
        }
    }
    
} // namespace carl