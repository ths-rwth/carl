/* 
 * File:   FactorizedPolynomial.tpp
 * Author: Florian Corzilius
 *
 * Created on September 4, 2014, 10:55 AM
 */

#include "FactorizedPolynomial.h"
#include "UnivariatePolynomial.tpp"

#pragma once

namespace carl
{
    template<typename P>
    FactorizedPolynomial<P>::FactorizedPolynomial():
        mCacheRef( CACHE::NO_REF ),
        mpCache( nullptr ),
        mCoefficient( 0 )
    {
        assert( mpCache == nullptr || mCacheRef != CACHE::NO_REF );
    }
    
    template<typename P>
    FactorizedPolynomial<P>::FactorizedPolynomial( const CoeffType& _coefficient ):
        mCacheRef( CACHE::NO_REF ),
        mpCache( nullptr ),
        mCoefficient( _coefficient )
    {
    }

    template<typename P>
    FactorizedPolynomial<P>::FactorizedPolynomial( const P& _polynomial, const std::shared_ptr<CACHE>& _pCache, bool _polyNormalized ):
        mCacheRef( CACHE::NO_REF ),
        mpCache( _polynomial.isZero() ? nullptr : _pCache ),
        mCoefficient( _polynomial.isZero() ? CoeffType(0) : (_polyNormalized ? CoeffType(1) : CoeffType(1)/_polynomial.coprimeFactor()) )
    {
        assert( !_polyNormalized || (_polynomial.coprimeFactor() == CoeffType(1)) );
        if ( _polynomial.isConstant() )
        {
            mpCache = nullptr;
        }
        else
        {
            P poly = _polyNormalized ? _polynomial : P(_polynomial * (CoeffType(1) / mCoefficient));
            if ( !_polyNormalized && poly.lcoeff() < 0 )
            {
                poly *= CoeffType(-1);
                mCoefficient *= CoeffType(-1);
            }
            

            /*
             * The following is not very nice, but we know, that the hash won't change, once the polynomial 
             * representation is fixed, so we can add the factorizations content belatedly. It is necessary to do so
             * as otherwise the factorized polynomial (this) being the only factor, is not yet cached which leads to an assertion.
             */
            if ( _polyNormalized || mCoefficient == 1 )
            {
                assert( mpCache != nullptr );
                Factorization<P> factorization;
                PolynomialFactorizationPair<P>* pfPair = new PolynomialFactorizationPair<P>( std::move( factorization), new P(poly) );
                //Factorization is not set yet
                auto ret = mpCache->cache( pfPair, &carl::canBeUpdated, &carl::update );
                mCacheRef = ret.first;
                mpCache->reg( mCacheRef );
                if( ret.second )
                {
                    assert( content().mFactorization.empty() );
                    content().mFactorization.insert( std::make_pair( *this, 1 ) );
                }
                else
                {
                    delete pfPair;
                } 
            }
            else
            {
                // Factor is polynomial without coefficient
                FactorizedPolynomial factor( poly, mpCache, true );
                mCacheRef = factor.mCacheRef;
                mpCache->reg( mCacheRef );
            }
            //We can not check the factorization yet, but as we have set it, it should be correct.
            //pfPair->assertFactorization();
        }
        ASSERT_CACHE_REF_LEGAL( (*this) );
    }
    
    template<typename P>
    FactorizedPolynomial<P>::FactorizedPolynomial( Factorization<P>&& _factorization, const CoeffType& _coefficient, const std::shared_ptr<CACHE>& _pCache ):
        mCacheRef( CACHE::NO_REF ),
        mpCache( _pCache ),
        mCoefficient( _coefficient )
    {
        assert( _coefficient != 0 );
        if ( _factorization.empty() )
            mpCache = nullptr;
        else
        {
            assert( mpCache != nullptr );
            // TODO expensive
            for ( auto factor = _factorization.begin(); factor != _factorization.end(); factor++ )
                assert( factor->first.coefficient() == 1 );
            PolynomialFactorizationPair<P>* pfPair = new PolynomialFactorizationPair<P>( std::move( _factorization ) );
            auto ret = mpCache->cache( pfPair, &carl::canBeUpdated, &carl::update );
            mCacheRef = ret.first;
            mpCache->reg( mCacheRef );
            if( !ret.second )
            {
                delete pfPair;
            }
        }
        ASSERT_CACHE_REF_LEGAL( (*this) );
    }
    
    template<typename P>
    FactorizedPolynomial<P>::FactorizedPolynomial( const FactorizedPolynomial<P>& _toCopy ):
        mCacheRef( _toCopy.mCacheRef ),
        mpCache( _toCopy.mpCache ),
        mCoefficient( _toCopy.mCoefficient )
    {
        if ( mpCache != nullptr )
        {
            mpCache->reg( mCacheRef );
        }
        ASSERT_CACHE_REF_LEGAL( (*this) );
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
        ASSERT_CACHE_EQUAL( mpCache, _fpoly.pCache() );
        mCoefficient = _fpoly.mCoefficient;
        if( mCacheRef != _fpoly.cacheRef() )
        {
            if( mpCache != nullptr )
            {
                mpCache->dereg( mCacheRef );
                mCacheRef = _fpoly.cacheRef();
                assert( _fpoly.pCache() == nullptr || mCacheRef != CACHE::NO_REF );
                if( _fpoly.pCache() != nullptr )
                    mpCache->reg( mCacheRef );
                else
                    mpCache = nullptr;
            }
            else if( _fpoly.mpCache != nullptr )
            {
                mpCache = _fpoly.mpCache;
                mCacheRef = _fpoly.cacheRef();
                mpCache->reg( mCacheRef );
            }
        }
        else if( mpCache != nullptr )
        {
            mpCache->reg( mCacheRef );
        }
        ASSERT_CACHE_REF_LEGAL( (*this) );
        return *this;
    }
        
    template<typename P>
    bool operator==( const FactorizedPolynomial<P>& _fpolyA, const FactorizedPolynomial<P>& _fpolyB )
    {
        if( _fpolyA.pCache() == nullptr && _fpolyB.pCache() == nullptr )
        {
            return _fpolyA.coefficient() == _fpolyB.coefficient();
        }
        else if( _fpolyA.pCache() != nullptr && _fpolyB.pCache() != nullptr )
        {
            return _fpolyA.content() == _fpolyB.content();
        }
        return false;
    }
        
    template<typename P>
    bool operator<( const FactorizedPolynomial<P>& _fpolyA, const FactorizedPolynomial<P>& _fpolyB )
    {
        ASSERT_CACHE_EQUAL( _fpolyA.pCache(), _fpolyB.pCache() );
        if( _fpolyA.pCache() == nullptr && _fpolyB.pCache() == nullptr )
        {
            return _fpolyA.coefficient() < _fpolyB.coefficient();
        }
        else if( _fpolyA.pCache() != nullptr && _fpolyB.pCache() != nullptr )
        {
            return _fpolyA.content() < _fpolyB.content();
        }
        return true;
    }
    
    template<typename P>
    bool operator!=( const FactorizedPolynomial<P>& _fpolyA, const FactorizedPolynomial<P>& _fpolyB )
    {
        ASSERT_CACHE_EQUAL( _fpolyA.pCache(), _fpolyB.pCache() );
        if( _fpolyA.pCache() == nullptr && _fpolyB.pCache() == nullptr )
        {
            return _fpolyA.coefficient() != _fpolyB.coefficient();
        }
        else if( _fpolyA.pCache() != nullptr && _fpolyB.pCache() != nullptr )
        {
            return !(_fpolyA.content() == _fpolyB.content());
        }
        return true;
    }
    
    template<typename P>
    const P computePolynomial( const FactorizedPolynomial<P>& _fpoly )
    {
        if( _fpoly.pCache() == nullptr )
            return P( _fpoly.coefficient() );
        P result( computePolynomial( _fpoly.content() ) );
        result *= _fpoly.coefficient();
        return result;
    }
    
    template<typename P>
    Coeff<P> distributeCoefficients( Factorization<P>& _factorization )
    {
        Coeff<P> result(1);
        for ( auto factor = _factorization.begin(); factor != _factorization.end(); factor++ )
        {
            result *= carl::pow( factor->first.coefficient(), factor->second );
            factor->first.mCoefficient = 1;
        }
        return result;
    }

    template<typename P>
    const FactorizedPolynomial<P> operator-( const FactorizedPolynomial<P>& _fpoly )
    {
        FactorizedPolynomial<P> result( _fpoly );
        result.mCoefficient *= Coeff<P>( -1 );
        return result;
    }

    template<typename P>
    const FactorizedPolynomial<P> operator+( const FactorizedPolynomial<P>& _fpolyA, const FactorizedPolynomial<P>& _fpolyB )
    {
        ASSERT_CACHE_EQUAL( _fpolyA.pCache(), _fpolyB.pCache() );
        _fpolyA.strengthenActivity();
        _fpolyB.strengthenActivity();
        
        // Handle cases where one or both are constant
        if ( !existsFactorization( _fpolyA ) )
        {
            if ( existsFactorization( _fpolyB ) )
            {
                return FactorizedPolynomial<P>( _fpolyB.polynomial() * _fpolyB.coefficient() + _fpolyA.coefficient(), _fpolyB.pCache() );
            }
            else
            {
                return FactorizedPolynomial<P>( _fpolyA.coefficient() + _fpolyB.coefficient() );
            }
        }
        else if ( !existsFactorization( _fpolyB ) )
        {
            return FactorizedPolynomial<P>( _fpolyA.polynomial() * _fpolyA.coefficient() + _fpolyB.coefficient(), _fpolyA.pCache() );
        }

        Coeff<P> coefficientCommon = Coeff<P>(carl::gcd( carl::getNum( _fpolyA.coefficient() ), carl::getNum( _fpolyB.coefficient() ) )) /
                Coeff<P>(carl::lcm( carl::getDenom( _fpolyA.coefficient() ), carl::getDenom( _fpolyB.coefficient() ) ));
        Coeff<P> coefficientRestA = _fpolyA.coefficient() / coefficientCommon;
        Coeff<P> coefficientRestB = _fpolyB.coefficient() / coefficientCommon;

        Factorization<P> factorizationRestA, factorizationRestB;
        assert( existsFactorization( _fpolyA ) );
        const Factorization<P>& factorizationA = _fpolyA.factorization();
        assert( existsFactorization( _fpolyB ) );
        const Factorization<P>& factorizationB = _fpolyB.factorization();

        //Compute common divisor as factor of result
        Factorization<P> resultFactorization = commonDivisor( factorizationA, factorizationB, factorizationRestA, factorizationRestB );

        //Compute remaining sum
        P sum = computePolynomial( factorizationRestA ) * coefficientRestA;
        sum += computePolynomial( factorizationRestB ) * coefficientRestB;
        if ( sum.isConstant() )
        {
            coefficientCommon *= sum.constantPart();
        }
        else
        {
            FactorizedPolynomial<P> fpolySum( sum, _fpolyA.pCache() );
            coefficientCommon *= fpolySum.coefficient();
            fpolySum.mCoefficient = Coeff<P>(1);
            resultFactorization.insert( resultFactorization.end(), std::pair<FactorizedPolynomial<P>, size_t>( fpolySum, 1 ) );
        }
        return FactorizedPolynomial<P>( std::move( resultFactorization ), coefficientCommon, FactorizedPolynomial<P>::chooseCache( _fpolyA.pCache(), _fpolyB.pCache() ) );
    }

    template<typename P>
    FactorizedPolynomial<P>& FactorizedPolynomial<P>::operator+=( const CoeffType& _coef )
    {
        FactorizedPolynomial<P> result = *this + FactorizedPolynomial<P>( _coef );
        return *this = result;
    }

    template<typename P>
    FactorizedPolynomial<P>& FactorizedPolynomial<P>::operator+=( const FactorizedPolynomial<P>& _fpoly )
    {
        FactorizedPolynomial<P> result = *this + _fpoly;
        return *this = result;
    }

    template<typename P>
    FactorizedPolynomial<P>& FactorizedPolynomial<P>::operator-=( const CoeffType& _coef )
    {
        FactorizedPolynomial<P> result = *this + FactorizedPolynomial<P>( CoeffType( -1 ) * _coef );
        return *this = result;
    }

    template<typename P>
    FactorizedPolynomial<P>& FactorizedPolynomial<P>::operator-=( const FactorizedPolynomial<P>& _fpoly )
    {
        FactorizedPolynomial<P> result = *this - _fpoly;
        return *this = result;
    }

    template<typename P>
    const FactorizedPolynomial<P> operator-( const FactorizedPolynomial<P>& _fpolyA, const FactorizedPolynomial<P>& _fpolyB )
    {
        ASSERT_CACHE_EQUAL( _fpolyA.pCache(), _fpolyB.pCache() );
        Coeff<P> coefficient = -_fpolyB.coefficient();
        if ( existsFactorization( _fpolyB ) )
            return _fpolyA + FactorizedPolynomial<P>( std::move( Factorization<P>( _fpolyB.factorization() ) ), coefficient, _fpolyB.pCache() );
        else
            return _fpolyA + FactorizedPolynomial<P>( coefficient );
    }
    
    template<typename P>
    const FactorizedPolynomial<P> operator*( const Coeff<P>& _coeff, const FactorizedPolynomial<P>& _fpoly )
    {
        if( _coeff == 0 )
            return FactorizedPolynomial<P>();
        FactorizedPolynomial<P> result( _fpoly );
        result.mCoefficient *= _coeff;
        return result;
    }
    
    template<typename P>
    const FactorizedPolynomial<P> operator*( const FactorizedPolynomial<P>& _fpoly, const Coeff<P>& _coeff )
    {
        if( _coeff == 0 )
            return FactorizedPolynomial<P>();
        FactorizedPolynomial<P> result( _fpoly );
        result.mCoefficient *= _coeff;
        return result;
    }

    template<typename P>
    const FactorizedPolynomial<P> operator*( const FactorizedPolynomial<P>& _fpolyA, const FactorizedPolynomial<P>& _fpolyB )
    {
        ASSERT_CACHE_EQUAL( _fpolyA.pCache(), _fpolyB.pCache() );
        _fpolyA.strengthenActivity();
        _fpolyB.strengthenActivity();

        if( _fpolyA.isZero() || _fpolyB.isZero() )
            return FactorizedPolynomial<P>();

        if( !existsFactorization( _fpolyB ) )
        {
            FactorizedPolynomial<P> result( _fpolyA );
            result.mCoefficient *= _fpolyB.mCoefficient;
            return result;
        }
        else if( !existsFactorization( _fpolyA ) )
        {
            FactorizedPolynomial<P> result( _fpolyB );
            result.mCoefficient *= _fpolyA.mCoefficient;
            return result;
        }

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
        //TODO needed?
        coefficientResult *= distributeCoefficients( resultFactorization );
        return FactorizedPolynomial<P>( std::move( resultFactorization ), coefficientResult, FactorizedPolynomial<P>::chooseCache( _fpolyA.pCache(), _fpolyB.pCache() ) );
    }
    
    template<typename P>
    FactorizedPolynomial<P>& FactorizedPolynomial<P>::operator*=( const CoeffType& _coef )
    {
        if( _coef == 0 && mpCache != nullptr )
        {
            mpCache->dereg( mCacheRef );
            mCacheRef = CACHE::NO_REF;
            mpCache = nullptr;
        }
        mCoefficient *= _coef;
        ASSERT_CACHE_REF_LEGAL( (*this) );
        return *this;
    }
    
    template<typename P>
    FactorizedPolynomial<P>& FactorizedPolynomial<P>::operator*=( const FactorizedPolynomial<P>& _fpoly )
    {
        FactorizedPolynomial<P> result = *this * _fpoly;
        return *this = result;
    }

    template<typename P>
    FactorizedPolynomial<P>& FactorizedPolynomial<P>::operator/=( const CoeffType& _coef )
    {
        assert( _coef != 0 );
        this->mCoefficient /= _coef;
        return *this;
    }
    
    template<typename P>
    FactorizedPolynomial<P>& FactorizedPolynomial<P>::operator/=( const FactorizedPolynomial<P>& _fpoly )
    {
        assert( !_fpoly.isZero() );
        FactorizedPolynomial<P> result = this->quotient( _fpoly );
        return *this = result;
    }

    template<typename P>
    const FactorizedPolynomial<P> FactorizedPolynomial<P>::quotient(const FactorizedPolynomial<P>& _fdivisor) const
    {
        assert( !_fdivisor.isZero() );
        if( isZero() )
            return FactorizedPolynomial<P>();
        FactorizedPolynomial<P> result = lazyDiv( *this, _fdivisor );
        assert( computePolynomial( _fdivisor ) * computePolynomial( result ) == computePolynomial( *this ) );
        return result;
    }

    template<typename P>
    const FactorizedPolynomial<P> quotient( const FactorizedPolynomial<P>& _fpolyA, const FactorizedPolynomial<P>& _fpolyB )
    {
        assert( !_fpolyB.isZero() );
        return _fpolyA.quotient( _fpolyB );
    }

    template<typename P>
    const FactorizedPolynomial<P> lazyDiv( const FactorizedPolynomial<P>& _fpolyA, const FactorizedPolynomial<P>& _fpolyB )
    {
        assert( !_fpolyB.isZero() );
        if( _fpolyA.isZero() )
            return FactorizedPolynomial<P>();
        ASSERT_CACHE_EQUAL( _fpolyA.pCache(), _fpolyB.pCache() );
        _fpolyA.strengthenActivity();
        _fpolyB.strengthenActivity();
        // Handle cases where one or both are constant
        if( !existsFactorization( _fpolyB ) )
        {
            FactorizedPolynomial<P> result( _fpolyA );
            assert( _fpolyB.mCoefficient != 0 );
            result.mCoefficient /= _fpolyB.mCoefficient;
            return result;
        }
        else if( !existsFactorization( _fpolyA ) )
        {
            FactorizedPolynomial<P> result( _fpolyB );
            assert( _fpolyA.mCoefficient != 0 );
            result.mCoefficient /= _fpolyA.mCoefficient;
            return result;
        }
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
        assert( computePolynomial( _fpolyB ) * computePolynomial( resultFactorization ) * coefficientResult == computePolynomial( _fpolyA ) );
        return FactorizedPolynomial<P>( std::move( resultFactorization ), coefficientResult, FactorizedPolynomial<P>::chooseCache( _fpolyA.pCache(), _fpolyB.pCache() ) );
    }

    template<typename P>
    const FactorizedPolynomial<P> lcm( const FactorizedPolynomial<P>& _fpolyA, const FactorizedPolynomial<P>& _fpolyB )
    {
        assert( !_fpolyA.isZero() && !_fpolyB.isZero() );
        ASSERT_CACHE_EQUAL( _fpolyA.pCache(), _fpolyB.pCache() );
        _fpolyA.strengthenActivity();
        _fpolyB.strengthenActivity();
        bool rehashFPolyA = false;
        bool rehashFPolyB = false;
        Coeff<P> coefficientLCM = Coeff<P>(carl::lcm( carl::getNum(_fpolyA.coefficient()), carl::getNum(_fpolyB.coefficient()) )) /
                Coeff<P>(carl::gcd( carl::getDenom(_fpolyA.coefficient()), carl::getDenom(_fpolyB.coefficient()) ));
        // Handle cases where one or both are constant
        if( !existsFactorization( _fpolyB ) )
        {
            FactorizedPolynomial<P> result( _fpolyA );
            result.mCoefficient = coefficientLCM;
            return result;
        }
        else if( !existsFactorization( _fpolyA ) )
        {
            FactorizedPolynomial<P> result( _fpolyB );
            result.mCoefficient = coefficientLCM;
            return result;
        }

        //Both polynomials are not constant
        Factorization<P> restAFactorization, restBFactorization;
        gcd( _fpolyA.content(), _fpolyB.content(), restAFactorization, restBFactorization, rehashFPolyA, rehashFPolyB );

        if( rehashFPolyA )
            _fpolyA.rehash();
        if( rehashFPolyB )
            _fpolyB.rehash();

        //Compute lcm as A*restB
        Factorization<P> lcmFactorization;
        if( existsFactorization( _fpolyA ) )
            lcmFactorization.insert( _fpolyA.factorization().begin(), _fpolyA.factorization().end() );
        for ( auto factor = restBFactorization.begin(); factor != restBFactorization.end(); factor++ )
        {
            auto result = lcmFactorization.insert( *factor );
            if ( !result.second )
            {
                //Increment exponent for already existing factor
                result.first->second += factor->second;
            }
        }

        coefficientLCM *= distributeCoefficients( lcmFactorization );
        return FactorizedPolynomial<P>( std::move( lcmFactorization ), coefficientLCM, _fpolyA.pCache() );
    }

    template<typename P>
    const FactorizedPolynomial<P> commonMultiple( const FactorizedPolynomial<P>& _fpolyA, const FactorizedPolynomial<P>& _fpolyB )
    {
        assert( !_fpolyA.isZero() && !_fpolyB.isZero() );
        ASSERT_CACHE_EQUAL( _fpolyA.pCache(), _fpolyB.pCache() );
        _fpolyA.strengthenActivity();
        _fpolyB.strengthenActivity();
        
        Coeff<P> coefficientLCM = Coeff<P>(carl::lcm( carl::getNum(_fpolyA.coefficient()), carl::getNum(_fpolyB.coefficient()) )) /
                Coeff<P>(carl::gcd( carl::getDenom(_fpolyA.coefficient()), carl::getDenom(_fpolyB.coefficient()) ));
        // Handle cases where one or both are constant
        if( !existsFactorization( _fpolyB ) )
        {
            FactorizedPolynomial<P> result( _fpolyA );
            result.mCoefficient = coefficientLCM;
            return result;
        }
        else if( !existsFactorization( _fpolyA ) )
        {
            FactorizedPolynomial<P> result( _fpolyB );
            result.mCoefficient = coefficientLCM;
            return result;
        }
        
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
        return FactorizedPolynomial<P>( std::move( cmFactorization ), coefficientLCM, FactorizedPolynomial<P>::chooseCache( _fpolyA.pCache(), _fpolyB.pCache() ) );
    }

    template<typename P>
    const FactorizedPolynomial<P> commonDivisor( const FactorizedPolynomial<P>& _fpolyA, const FactorizedPolynomial<P>& _fpolyB )
    {
        assert( !_fpolyA.isZero() && !_fpolyB.isZero() );
        ASSERT_CACHE_EQUAL( _fpolyA.pCache(), _fpolyB.pCache() );
        _fpolyA.strengthenActivity();
        _fpolyB.strengthenActivity();
        Coeff<P> coefficientCommon = Coeff<P>(carl::gcd( carl::getNum( _fpolyA.coefficient() ), carl::getNum( _fpolyB.coefficient() ) )) /
                Coeff<P>(carl::lcm( carl::getDenom( _fpolyA.coefficient() ), carl::getDenom( _fpolyB.coefficient() ) ));
        // Handle cases where one or both are constant
        if( !existsFactorization( _fpolyA ) || !existsFactorization( _fpolyB ) )
        {
            return FactorizedPolynomial<P>( coefficientCommon );
        }
        
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

        return FactorizedPolynomial<P>( std::move( cdFactorization ), coefficientCommon, FactorizedPolynomial<P>::chooseCache( _fpolyA.pCache(), _fpolyB.pCache() ) );
    }

    template<typename P>
    const Factorization<P> commonDivisor( const Factorization<P>& _fFactorizationA, const Factorization<P>& _fFactorizationB, Factorization<P>& _fFactorizationRestA, Factorization<P>& _fFactorizationRestB)
    {
        assert( !_fFactorizationA.empty() && !_fFactorizationB.empty() );
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
    const FactorizedPolynomial<P> gcd( const FactorizedPolynomial<P>& _fpolyA, const FactorizedPolynomial<P>& _fpolyB )
    {
        assert( !_fpolyA.isZero() && !_fpolyB.isZero() );
        FactorizedPolynomial<P> restA, restB;
        return gcd( _fpolyA, _fpolyB, restA, restB );
    }

    template<typename P>
    const FactorizedPolynomial<P> gcd( const FactorizedPolynomial<P>& _fpolyA, const FactorizedPolynomial<P>& _fpolyB, FactorizedPolynomial<P>& _fpolyRestA, FactorizedPolynomial<P>& _fpolyRestB )
    {
        assert( !_fpolyA.isZero() && !_fpolyB.isZero() );
        _fpolyA.strengthenActivity();
        _fpolyB.strengthenActivity();
        bool rehashFPolyA = false;
        bool rehashFPolyB = false;

        Coeff<P> coefficientCommon = Coeff<P>(carl::gcd( carl::getNum( _fpolyA.coefficient() ), carl::getNum( _fpolyB.coefficient() ) )) /
                Coeff<P>(carl::lcm( carl::getDenom( _fpolyA.coefficient() ), carl::getDenom( _fpolyB.coefficient() ) ));
        Coeff<P> coefficientRestA = _fpolyA.coefficient() / coefficientCommon;
        Coeff<P> coefficientRestB = _fpolyB.coefficient() / coefficientCommon;
        
        //Handle cases where one or both are constant
        if ( !existsFactorization( _fpolyA ) )
        {
            if ( existsFactorization( _fpolyB ) )
            {
                _fpolyRestA = FactorizedPolynomial<P>( coefficientRestA );
                _fpolyRestB = FactorizedPolynomial<P>( std::move( Factorization<P>( _fpolyB.factorization() ) ), coefficientRestB, _fpolyB.pCache() );
                return FactorizedPolynomial<P>( coefficientCommon );
            }
            else
            {
                _fpolyRestA = FactorizedPolynomial<P>( coefficientRestA );
                _fpolyRestB = FactorizedPolynomial<P>( coefficientRestB );
                return FactorizedPolynomial<P>( coefficientCommon );
            }
        }
        else if ( !existsFactorization( _fpolyB ) )
        {
            _fpolyRestA = FactorizedPolynomial<P>( std::move( Factorization<P>( _fpolyA.factorization() ) ), coefficientRestA, _fpolyA.pCache());
            _fpolyRestB = FactorizedPolynomial<P>( coefficientRestB );
            return FactorizedPolynomial<P>( coefficientCommon );
        }

        //Both polynomials are not constant
        Factorization<P> restAFactorization, restBFactorization;
        Factorization<P> gcdFactorization( gcd( _fpolyA.content(), _fpolyB.content(), restAFactorization, restBFactorization, rehashFPolyA, rehashFPolyB ) );

        if( rehashFPolyA )
            _fpolyA.rehash();
        if( rehashFPolyB )
            _fpolyB.rehash();

        coefficientRestA *= distributeCoefficients( restAFactorization );
        coefficientRestB *= distributeCoefficients( restBFactorization );
        coefficientCommon *= distributeCoefficients( gcdFactorization );
        _fpolyRestA = FactorizedPolynomial<P>( std::move( restAFactorization ), coefficientRestA, _fpolyA.pCache());
        _fpolyRestB = FactorizedPolynomial<P>( std::move( restBFactorization ), coefficientRestB, _fpolyA.pCache());
        
        return FactorizedPolynomial<P>( std::move( gcdFactorization ), coefficientCommon, _fpolyA.pCache() );
    }
    
    template <typename P>
    std::ostream& operator<<( std::ostream& _out, const FactorizedPolynomial<P>& _fpoly )
    {
        if ( existsFactorization( _fpoly ) )
        {
            if ( _fpoly.coefficient() != 1 )
                _out << _fpoly.coefficient() << " * ";
            return _out << _fpoly.content();
        }
        else
            return ( _out << _fpoly.coefficient() );
    }
} // namespace carl
