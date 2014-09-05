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
    FactorizedPolynomial<P>::FactorizedPolynomial( typename FactorizedPolynomialCache<P>::Ref _ref, FactorizedPolynomialCache<P>& _cache ):
        mCacheRef( _ref ),
        mrCache( _cache )
    {
        
    }
    
    template<typename P>
    FactorizedPolynomial<P>::FactorizedPolynomial( const FactorizedPolynomial<P>& _toCopy ):
        mCacheRef( _toCopy.mCacheRef ),
        mrCache( _toCopy.mrCache )
    {}
    
    template<typename P>
    FactorizedPolynomial<P>::~FactorizedPolynomial()
    {
        mrCache.remove( mCacheRef );
    }
        
    template<typename P>
    bool operator==(const FactorizedPolynomial<P>& _fpolyA, const FactorizedPolynomial<P>& _fpolyB)
    {
        return (*_fpolyA.mCacheRef->first) == (*_fpolyB.mCacheRef->first);
    }

    template<typename P>
    const FactorizedPolynomial<P> operator+(const FactorizedPolynomial<P>& _fpolyA, const FactorizedPolynomial<P>& _fpolyB)
    {

    }

    template<typename P>
    const FactorizedPolynomial<P> operator-(const FactorizedPolynomial<P>& _fpolyA, const FactorizedPolynomial<P>& _fpolyB)
    {

    }

    template<typename P>
    const FactorizedPolynomial<P> operator*(const FactorizedPolynomial<P>& _fpolyA, const FactorizedPolynomial<P>& _fpolyB)
    {

    }

    template<typename P>
    const FactorizedPolynomial<P> operator/(const FactorizedPolynomial<P>& _fpolyA, const FactorizedPolynomial<P>& _fpolyB)
    {

    }

    template<typename P>
    const FactorizedPolynomial<P> commonDivisor(const FactorizedPolynomial<P>& _fpolyA, const FactorizedPolynomial<P>& _fpolyB)
    {
        Factorization<P> cdFactorization;
        const Factorization<P>& factorizationA = _fpolyA.mrCache.get(_fpolyA.mCacheRef).factorization();
        const Factorization<P>& factorizationB = _fpolyB.mrCache.get(_fpolyB.mCacheRef).factorization();
        auto factorA = factorizationA.begin();
        auto factorB = factorizationB.begin();
        while( factorA != factorizationA.end() && factorB != factorizationB.end() )
        {
            if( *factorA->first == *factorB->first )
                cdFactorization.insert( cdFactorization.end(), std::make_pair( factorA->first, factorA->second < factorB->second ? factorA->second : factorB->second ) );
            else if( *factorA->first < *factorB->first )
                factorA++;
            else
                factorB++;
        }
        return FactorizedPolynomial<P>( _fpolyA.mrCache.createFactorizedPolynomial( std::move( cdFactorization ) ), _fpolyA.mrCache );
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
            _out << (*factorization.begin()->first);
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
                _out << (*polyExpPair->first);
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