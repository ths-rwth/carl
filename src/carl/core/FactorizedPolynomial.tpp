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
    FactorizedPolynomial<P>::FactorizedPolynomial( const Coeff<P>& _coefficient ):
        mCacheRef( CACHE::NO_REF ),
        mpCache( nullptr ),
        mCoefficient( _coefficient )
    {
    }

    template<typename P>
    FactorizedPolynomial<P>::FactorizedPolynomial( const P& _polynomial, CACHE* _pCache ):
        mCacheRef( CACHE::NO_REF ),
        mpCache( _pCache ),
        mCoefficient( _polynomial.coprimeFactor() )
    {
        P poly = _polynomial.coprimeCoefficients();
        if ( poly.isConstant() )
        {
            mpCache = nullptr;
        }
        else
        {
            assert( mpCache != nullptr );
            Factorization<P> factorization;
            PolynomialFactorizationPair<P>* pfPair = new PolynomialFactorizationPair<P>( std::move( factorization), new P( poly ) );
            //Factorization is not set yet
            mCacheRef = mpCache->cache( pfPair );//, &carl::canBeUpdated, &carl::update );
            /*
             * The following is not very nice, but we know, that the hash won't change, once the polynomial 
             * representation is fixed, so we can add the factorizations content belatedly. It is necessary to do so
             * as otherwise the factorized polynomial (this) being the only factor, is not yet cached which leads to an assertion.
             */
            if ( poly != 1)
                pfPair->mFactorization.insert( std::make_pair( *this, 1 ) );

            //We can not check the factorization yet, but as we have set it, it should be correct.
            //pfPair->assertFactorization();
        }
    }
    
    template<typename P>
    FactorizedPolynomial<P>::FactorizedPolynomial( const P& _polynomial, Factorization<P>&& _factorization, const Coeff<P>& _coefficient, CACHE* _pCache ):
        mCacheRef( CACHE::NO_REF ),
        mpCache( _pCache ),
        mCoefficient( _coefficient )
    {
        assert( mCoefficient == _polynomial.coprimeFactor() );
        assert( mpCache != nullptr );
        mCacheRef = mpCache->cache( new PolynomialFactorizationPair<P>( std::move( _factorization ), new P( _polynomial ) ), &carl::canBeUpdated, &carl::update );
    }
    
    template<typename P>
    FactorizedPolynomial<P>::FactorizedPolynomial( Factorization<P>&& _factorization, const Coeff<P>& _coefficient, CACHE* _pCache ):
        mCacheRef( CACHE::NO_REF ),
        mpCache( _pCache ),
        mCoefficient( _coefficient )
    {
        if ( _factorization.empty() )
            mpCache = nullptr;
        else
        {
            assert( mpCache != nullptr );
            mCacheRef = mpCache->cache( new PolynomialFactorizationPair<P>( std::move( _factorization ) ), &carl::canBeUpdated, &carl::update );
        }
    }
    
    template<typename P>
    FactorizedPolynomial<P>::FactorizedPolynomial( const FactorizedPolynomial<P>& _toCopy ):
        mCacheRef( _toCopy.mCacheRef ),
        mpCache( _toCopy.mpCache )
    {
        if ( mpCache != nullptr )
            mpCache->reg( _toCopy.mCacheRef );
    }
    
    template<typename P>
    FactorizedPolynomial<P>::~FactorizedPolynomial()
    {
        if ( mpCache != nullptr )
        {
            mpCache->dereg( mCacheRef );
            mpCache = nullptr;
        }
    }
    
    template<typename P>
    FactorizedPolynomial<P>& FactorizedPolynomial<P>::operator=( const FactorizedPolynomial<P>& _fpoly )
    {
        assertCacheEqual( mpCache, _fpoly.cache() );
        if ( mpCache != nullptr )
        {
            mpCache->dereg( mCacheRef );
            mCacheRef = _fpoly.cacheRef();
            mpCache->reg( mCacheRef );
        }
        else
            mCacheRef = _fpoly.cacheRef();
        return *this;
    }
        
    template<typename P>
    bool operator==( const FactorizedPolynomial<P>& _fpolyA, const FactorizedPolynomial<P>& _fpolyB )
    {
        if ( _fpolyA.cache() == nullptr )
        {
            return _fpolyB.cache() == nullptr;
        }
        else
        {
            assert( _fpolyA.cache() == _fpolyB.cache() );
            if ( _fpolyA.cacheRef() == Cache<PolynomialFactorizationPair<P>>::NO_REF )
                return _fpolyB.cacheRef() == Cache<PolynomialFactorizationPair<P>>::NO_REF;
            else
                return _fpolyA.content() == _fpolyB.content();
        }
    }
        
    template<typename P>
    bool operator<( const FactorizedPolynomial<P>& _fpolyA, const FactorizedPolynomial<P>& _fpolyB )
    {
        assertCacheEqual( _fpolyA.cache(), _fpolyB.cache() );
        //TODO how to handle constants?
        return _fpolyA.content() < _fpolyB.content();
    }

    template<typename P>
    const FactorizedPolynomial<P> operator+( const FactorizedPolynomial<P>& _fpolyA, const FactorizedPolynomial<P>& _fpolyB )
    {
        _fpolyA.strengthenActivity();
        _fpolyB.strengthenActivity();
        assertCacheEqual( _fpolyA.cache(), _fpolyB.cache() );

        Coeff<P> coefficientCommon = carl::gcd( _fpolyA.coefficient(), _fpolyB.coefficient() );
        Coeff<P> coefficientRestA = _fpolyA.coefficient() / coefficientCommon;
        Coeff<P> coefficientRestB = _fpolyB.coefficient() / coefficientCommon;

        Factorization<P> factorizationRestA, factorizationRestB;
        Factorization<P> factorizationA, factorizationB;
        if ( _fpolyA.cacheRef() != Cache<PolynomialFactorizationPair<P>>::NO_REF )
            factorizationA = _fpolyA.factorization();
        if ( _fpolyB.cacheRef() != Cache<PolynomialFactorizationPair<P>>::NO_REF )
            factorizationB = _fpolyB.factorization();

        //Compute common divisor as factor of result
        Factorization<P> resultFactorization = commonDivisor( factorizationA, factorizationB, factorizationRestA, factorizationRestB );

        //Compute remaining sum
        P sum = computePolynomial( factorizationRestA ) * coefficientRestA;
        sum += computePolynomial( factorizationRestB ) * coefficientRestB;
        FactorizedPolynomial<P> fpolySum( sum, _fpolyA.cache() );
        resultFactorization.insert( resultFactorization.end(), std::pair<FactorizedPolynomial<P>, size_t>( fpolySum, 1 ) );
        return FactorizedPolynomial<P>( std::move( resultFactorization ), coefficientCommon, chooseCache( _fpolyA.cache(), _fpolyB.cache() ) );
    }

    template<typename P>
    const FactorizedPolynomial<P> operator-( const FactorizedPolynomial<P>& _fpolyA, const FactorizedPolynomial<P>& _fpolyB )
    {
        assertCacheEqual( _fpolyA.cache(), _fpolyB.cache() );
        Coeff<P> coefficient = -_fpolyB.coefficient();
        return _fpolyA + FactorizedPolynomial<P>( std::move( Factorization<P>( _fpolyB.factorization() ) ), coefficient, _fpolyB.cache() );
    }

    template<typename P>
    const FactorizedPolynomial<P> operator*( const FactorizedPolynomial<P>& _fpolyA, const FactorizedPolynomial<P>& _fpolyB )
    {
        _fpolyA.strengthenActivity();
        _fpolyB.strengthenActivity();
        assertCacheEqual( _fpolyA.cache(), _fpolyB.cache() );
        Factorization<P> resultFactorization;
        const Factorization<P>& factorizationA = _fpolyA.factorization();
        const Factorization<P>& factorizationB = _fpolyB.factorization();
        auto factorA = factorizationA.begin();
        auto factorB = factorizationB.begin();
        while( factorA != factorizationA.end() && factorB != factorizationB.end() )
        {
            if( factorA->first == factorB->first )
            {
                resultFactorization.insert( resultFactorization.end(), std::pair<FactorizedPolynomial<P>, size_t>(factorA->first, factorA->second + factorB->second ) );
                factorA++;
                factorB++;
            }
            else if( factorA->first < factorB->first )
            {
                resultFactorization.insert( resultFactorization.end(), *factorA );
                factorA++;
            }
            else
            {
                resultFactorization.insert( resultFactorization.end(), *factorB );
                factorB++;
            }
        }
        while ( factorA != factorizationA.end() )
        {
            resultFactorization.insert( resultFactorization.end(), *factorA );
            factorA++;
        }
        while ( factorB != factorizationB.end() )
        {
            resultFactorization.insert( resultFactorization.end(), *factorB );
            factorB++;
        }

        Coeff<P> coefficientResult = _fpolyA.coefficient() * _fpolyB.coefficient();
        return FactorizedPolynomial<P>( std::move( resultFactorization ), coefficientResult, chooseCache( _fpolyA.cache(), _fpolyB.cache() ) );
    }
    
    template<typename P>
    FactorizedPolynomial<P>& FactorizedPolynomial<P>::operator*=( const FactorizedPolynomial<P>& _fpoly )
    {
        FactorizedPolynomial<P> result = *this * _fpoly;
        return *this = result;
    }

    template<typename P>
    const FactorizedPolynomial<P> FactorizedPolynomial<P>::quotient(const FactorizedPolynomial<P>& _fdivisor) const
    {
        FactorizedPolynomial<P> result = lazyDiv( *this, _fdivisor );
        assert( _fdivisor * result == *this );
        return result;
    }

    template<typename P>
    const FactorizedPolynomial<P> lazyDiv( const FactorizedPolynomial<P>& _fpolyA, const FactorizedPolynomial<P>& _fpolyB )
    {
        _fpolyA.strengthenActivity();
        _fpolyB.strengthenActivity();
        assertCacheEqual( _fpolyA.cache(), _fpolyB.cache() );
        Factorization<P> resultFactorization;
        const Factorization<P>& factorizationA = _fpolyA.factorization();
        const Factorization<P>& factorizationB = _fpolyB.factorization();
        auto factorA = factorizationA.begin();
        auto factorB = factorizationB.begin();
        while( factorA != factorizationA.end() && factorB != factorizationB.end() )
        {
            if( factorA->first == factorB->first )
            {
                if ( factorA->second > factorB->second )
                    resultFactorization.insert( resultFactorization.end(), std::pair<FactorizedPolynomial<P>, size_t>(factorA->first, factorA->second - factorB->second ) );
                factorA++;
                factorB++;
            }
            else if( factorA->first < factorB->first )
            {
                resultFactorization.insert( resultFactorization.end(), *factorA );
                factorA++;
            }
            else
                factorB++;
        }
        while ( factorA != factorizationA.end() )
        {
            resultFactorization.insert( resultFactorization.end(), *factorA );
            factorA++;
        }

        Coeff<P> coefficientResult = _fpolyA.coefficient() / _fpolyB.coefficient();
        return FactorizedPolynomial<P>( std::move( resultFactorization ), coefficientResult, chooseCache( _fpolyA.cache(), _fpolyB.cache() ) );
    }

    template<typename P>
    const FactorizedPolynomial<P> lcm( const FactorizedPolynomial<P>& _fpolyA, const FactorizedPolynomial<P>& _fpolyB )
    {
        _fpolyA.strengthenActivity();
        _fpolyB.strengthenActivity();
        bool rehashFPolyA = false;
        bool rehashFPolyB = false;
        Coeff<P> coefficientLCM = carl::lcm( _fpolyA.coefficient(), _fpolyB.coefficient() );

        //Handle cases where one or both are constant
        if ( !existsFactorization( _fpolyA ) )
        {
            if ( !existsFactorization( _fpolyB ) )
                return FactorizedPolynomial<P>( coefficientLCM );
            else
                return FactorizedPolynomial<P>( std::move( Factorization<P>( _fpolyB.factorization() ) ), coefficientLCM, _fpolyB.cache() );
        }
        else if ( !existsFactorization( _fpolyB ) )
            return FactorizedPolynomial<P>( std::move( Factorization<P>( _fpolyA.factorization() ) ), coefficientLCM, _fpolyA.cache() );

        //Both polynomials are not constant
        Factorization<P> restAFactorization, restBFactorization;
        gcd( _fpolyA.content(), _fpolyB.content(), restAFactorization, restBFactorization, rehashFPolyA, rehashFPolyB );

        if( rehashFPolyA )
            _fpolyA.rehash();
        if( rehashFPolyB )
            _fpolyB.rehash();

        Factorization<P> lcmFactorization = _fpolyA.factorization();
        lcmFactorization.insert( restBFactorization.begin(), restBFactorization.end() );
        return FactorizedPolynomial<P>( std::move( lcmFactorization ), coefficientLCM, _fpolyA.cache() );
    }

    template<typename P>
    const FactorizedPolynomial<P> commonMultiple( const FactorizedPolynomial<P>& _fpolyA, const FactorizedPolynomial<P>& _fpolyB )
    {
        _fpolyA.strengthenActivity();
        _fpolyB.strengthenActivity();
        assertCacheEqual( _fpolyA.cache(), _fpolyB.cache() );
        Factorization<P> cmFactorization;
        const Factorization<P>& factorizationA = _fpolyA.factorization();
        const Factorization<P>& factorizationB = _fpolyB.factorization();
        auto factorA = factorizationA.begin();
        auto factorB = factorizationB.begin();
        while( factorA != factorizationA.end() && factorB != factorizationB.end() )
        {
            if( factorA->first == factorB->first )
            {
                cmFactorization.insert( cmFactorization.end(), factorA->second > factorB->second ? *factorA : *factorB );
                factorA++;
                factorB++;
            }
            else if( factorA->first < factorB->first )
            {
                cmFactorization.insert( cmFactorization.end(), *factorA );
                factorA++;
            }
            else
            {
                cmFactorization.insert( cmFactorization.end(), *factorB );
                factorB++;
            }
        }
        while ( factorA != factorizationA.end() )
        {
            cmFactorization.insert( cmFactorization.end(), *factorA );
            factorA++;
        }
        while ( factorB != factorizationB.end() )
        {
            cmFactorization.insert( cmFactorization.end(), *factorB );
            factorB++;
        }

        Coeff<P> coefficientCommon = carl::lcm( _fpolyA.coefficient(), _fpolyB.coefficient() );
        return FactorizedPolynomial<P>( std::move( cmFactorization ), coefficientCommon, chooseCache( _fpolyA.cache(), _fpolyB.cache() ) );
    }

    template<typename P>
    const FactorizedPolynomial<P> commonDivisor( const FactorizedPolynomial<P>& _fpolyA, const FactorizedPolynomial<P>& _fpolyB )
    {
        _fpolyA.strengthenActivity();
        _fpolyB.strengthenActivity();
        assertCacheEqual( _fpolyA.cache(), _fpolyB.cache() );
        Factorization<P> cdFactorization;
        const Factorization<P>& factorizationA = _fpolyA.factorization();
        const Factorization<P>& factorizationB = _fpolyB.factorization();
        auto factorA = factorizationA.begin();
        auto factorB = factorizationB.begin();
        while( factorA != factorizationA.end() && factorB != factorizationB.end() )
        {
            if( factorA->first == factorB->first )
            {
                cdFactorization.insert( cdFactorization.end(), factorA->second < factorB->second ? *factorA : *factorB );
                factorA++;
                factorB++;
            }
            else if( factorA->first < factorB->first )
                factorA++;
            else
                factorB++;
        }

        Coeff<P> coefficientCommon = carl::gcd( _fpolyA.coefficient(), _fpolyB.coefficient() );
        return FactorizedPolynomial<P>( std::move( cdFactorization ), coefficientCommon, chooseCache( _fpolyA.cache(), _fpolyB.cache() ) );
    }

    template<typename P>
    const Factorization<P> commonDivisor( const Factorization<P>& _fFactorizationA, const Factorization<P>& _fFactorizationB, Factorization<P>& _fFactorizationRestA, Factorization<P>& _fFactorizationRestB)
    {
        Factorization<P> resultFactorization;
        _fFactorizationRestA.clear();
        _fFactorizationRestB.clear();
        auto factorA = _fFactorizationA.begin();
        auto factorB = _fFactorizationB.begin();
        while( factorA != _fFactorizationA.end() && factorB != _fFactorizationB.end() )
        {
            if( factorA->first == factorB->first )
            {
                resultFactorization.insert( resultFactorization.end(), factorA->second < factorB->second ? *factorA : *factorB );
                factorA++;
                factorB++;
            }
            else if( factorA->first < factorB->first )
            {
                _fFactorizationRestA.insert( _fFactorizationRestA.end(), *factorA );
                factorA++;
            }
            else
            {
                _fFactorizationRestB.insert( _fFactorizationRestB.end(), *factorB );
                factorB++;
            }
        }
        while ( factorA != _fFactorizationA.end() )
        {
            _fFactorizationRestA.insert( _fFactorizationRestA.end(), *factorA );
            factorA++;
        }
        while ( factorB != _fFactorizationB.end() )
        {
            _fFactorizationRestB.insert( _fFactorizationRestB.end(), *factorB );
            factorB++;
        }

        return resultFactorization;
    }


    template<typename P>
    const FactorizedPolynomial<P> gcd( const FactorizedPolynomial<P>& _fpolyA, const FactorizedPolynomial<P>& _fpolyB, FactorizedPolynomial<P>& _fpolyRestA, FactorizedPolynomial<P>& _fpolyRestB )
    {
        _fpolyA.strengthenActivity();
        _fpolyB.strengthenActivity();
        bool rehashFPolyA = false;
        bool rehashFPolyB = false;

        Coeff<P> coefficientCommon = carl::gcd( _fpolyA.coefficient(), _fpolyB.coefficient() );
        Coeff<P> coefficientRestA = _fpolyA.coefficient() / coefficientCommon;
        Coeff<P> coefficientRestB = _fpolyB.coefficient() / coefficientCommon;

         //Handle cases where one or both are constant
        if ( !existsFactorization( _fpolyA ) )
        {
            if ( !existsFactorization( _fpolyB ) )
            {
                _fpolyRestA = FactorizedPolynomial<P>( coefficientRestA );
                _fpolyRestB = FactorizedPolynomial<P>( coefficientRestB );
                return FactorizedPolynomial<P>( coefficientCommon );
            }
            else
            {
                _fpolyRestA = FactorizedPolynomial<P>( coefficientRestA );
                _fpolyRestB = FactorizedPolynomial<P>( std::move( Factorization<P>( _fpolyB.factorization() ) ), coefficientRestB, _fpolyB.cache() );
                return FactorizedPolynomial<P>( std::move( Factorization<P>( _fpolyB.factorization() ) ), coefficientCommon, _fpolyB.cache() );
            }
        }
        else if ( !existsFactorization( _fpolyB ) )
        {
            _fpolyRestA = FactorizedPolynomial<P>( std::move( Factorization<P>( _fpolyA.factorization() ) ), coefficientRestA, _fpolyA.cache());
            _fpolyRestB = FactorizedPolynomial<P>( coefficientRestB );
            return FactorizedPolynomial<P>( std::move( Factorization<P>( _fpolyB.factorization() ) ), coefficientCommon, _fpolyA.cache() );
        }

        //Both polynomials are not constant
        Factorization<P> restAFactorization, restBFactorization;
        Factorization<P> gcdFactorization( gcd( _fpolyA.content(), _fpolyB.content(), restAFactorization, restBFactorization, rehashFPolyA, rehashFPolyB ) );

        if( rehashFPolyA )
            _fpolyA.rehash();
        if( rehashFPolyB )
            _fpolyB.rehash();

        _fpolyRestA = FactorizedPolynomial<P>( std::move( restAFactorization ), coefficientRestA, _fpolyRestA.cache());
        _fpolyRestB = FactorizedPolynomial<P>( std::move( restBFactorization ), coefficientRestB, _fpolyRestB.cache());
        return FactorizedPolynomial<P>( std::move( gcdFactorization ), coefficientCommon, chooseCache( _fpolyA.cache(), _fpolyB.cache() ) );
    }
    
    template <typename P>
    std::ostream& operator<<( std::ostream& _out, const FactorizedPolynomial<P>& _fpoly )
    {
        if ( existsFactorization( _fpoly ) )
            return ( _out << _fpoly.content() );
        else
            return ( _out << _fpoly.coefficient() );
    }
} // namespace carl