/* 
 * File:   PolynomialFactorizationPair.tpp
 * Author: Florian Corzilius
 *
 * Created on September 5, 2014, 3:57 PM
 */

#pragma once

#include "PolynomialFactorizationPair.h"
#include "logging.h"
#include "FactorizedPolynomial.h"

namespace carl
{   
    template <typename P>
    std::ostream& operator<<( std::ostream& _out, const Factorization<P>& _factorization )
    {
        if( _factorization.empty() )
        {
            _out << "1";
        }
        else
        {
            for( auto polyExpPair = _factorization.begin(); polyExpPair != _factorization.end(); ++polyExpPair )
            {
                if( polyExpPair != _factorization.begin() )
                    _out << " * ";
                assert( polyExpPair->second > 0 );
                _out << "(" << polyExpPair->first << ")";
                if( polyExpPair->second > 1 )
                    _out << "^" << polyExpPair->second;
            }
        }
        return _out;
    }
    
    template<typename P>
    bool factorizationsEqual( const Factorization<P>& _factorizationA, const Factorization<P>& _factorizationB )
    {
        auto iterA = _factorizationA.begin();
        auto iterB = _factorizationB.begin();
        while( iterA != _factorizationA.end() && iterB != _factorizationB.end() )
        {
            if( iterA->second != iterB->second || !(iterA->first == iterB->first) )
                break;
            ++iterA; ++iterB;
        }
        return iterA == _factorizationA.end() && iterB == _factorizationB.end();
    }
    
    template<typename P>
    PolynomialFactorizationPair<P>::PolynomialFactorizationPair( Factorization<P>&& _factorization, P* _polynomial ):
        mHash( 0 ),
        mFactorization( std::move( _factorization ) ),
        mpPolynomial( _polynomial ),
        mIrreducible( -1 )
    {
        if ( mpPolynomial == nullptr )
        {
            if ( mFactorization.size() == 1 )
            {
                // No factorization -> set polynomial
                assert( existsFactorization( mFactorization.begin()->first ) );
                mpPolynomial = new P( mFactorization.begin()->first.content().mpPolynomial->pow( mFactorization.begin()->second ) );
                assert( mpPolynomial != nullptr );
            }
        }
        else
        {
            if ( mpPolynomial->isOne() )
            {
                assert( mFactorization.size() == 0);
                mpPolynomial = nullptr;
            }
        }

        // Check correctness
        assert( mpPolynomial == nullptr || mpPolynomial->coprimeFactor() == 1);
        assert( mpPolynomial == nullptr || mFactorization.empty() || assertFactorization() );

        rehash();
    }
    
    template<typename P>
    PolynomialFactorizationPair<P>::~PolynomialFactorizationPair()
    {
        delete mpPolynomial;
    }

    template<typename P>
    void PolynomialFactorizationPair<P>::rehash() const
    {
        std::lock_guard<std::recursive_mutex> lock( mMutex );
        if( mpPolynomial == nullptr )
        {
            assert( mFactorization.empty() || mFactorization.size() > 1 );
            mHash = 0;
            for( auto polyExpPair = mFactorization.begin(); polyExpPair != mFactorization.end(); ++polyExpPair )
            {
                mHash = (mHash << 5) | (mHash >> (sizeof(size_t)*8 - 5));
                mHash ^= std::hash<FactorizedPolynomial<P>>()( polyExpPair->first );
                mHash = (mHash << 5) | (mHash >> (sizeof(size_t)*8 - 5));
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
        if( &_polyFactA == &_polyFactB )
            return true;
        //TODO fix
        //if ( _polyFactA.mHash != _polyFactB.mHash )
        //    return false;
        std::lock_guard<std::recursive_mutex> lockA( _polyFactA.mMutex );
        std::lock_guard<std::recursive_mutex> lockB( _polyFactB.mMutex );
        if( _polyFactA.mpPolynomial != nullptr && _polyFactB.mpPolynomial != nullptr )
        {
            return *_polyFactA.mpPolynomial == *_polyFactB.mpPolynomial;
        }
        else
        {
            return factorizationsEqual( _polyFactA.factorization(), _polyFactB.factorization() );
        }
    }
    
    template<typename P>
    bool operator<( const PolynomialFactorizationPair<P>& _polyFactA, const PolynomialFactorizationPair<P>& _polyFactB )
    {
        if( &_polyFactA == &_polyFactB )
            return false;
        std::lock_guard<std::recursive_mutex> lockA( _polyFactA.mMutex );
        std::lock_guard<std::recursive_mutex> lockB( _polyFactB.mMutex );
        if( _polyFactA.mpPolynomial != nullptr && _polyFactB.mpPolynomial != nullptr )
        {
            return *_polyFactA.mpPolynomial < *_polyFactB.mpPolynomial;
        }
        else
        {
            auto iterA = _polyFactA.factorization().begin();
            auto iterB = _polyFactB.factorization().begin();
            while( iterA != _polyFactA.factorization().end() && iterB != _polyFactB.factorization().end() )
            {
                if( iterA->first < iterB->first )
                {
                    return true;
                }
                else if( iterA->first == iterB->first )
                {
                    if( iterA->second < iterB->second )
                    {
                        return true;
                    }
                    else if( iterA->second > iterB->second )
                    {
                        return false;
                    }
                }
                else
                {
                    return false;
                }
                ++iterA; ++iterB;
            }
            return iterA == _polyFactA.factorization().end();
        }
    }
    
    template<typename P>
    bool canBeUpdated( const PolynomialFactorizationPair<P>& _toUpdate, const PolynomialFactorizationPair<P>& _updateWith )
    {
        if( &_toUpdate == &_updateWith )
            return false;
        std::lock_guard<std::recursive_mutex> lockA( _toUpdate.mMutex );
        std::lock_guard<std::recursive_mutex> lockB( _updateWith.mMutex );
        assert( _toUpdate.getHash() == _updateWith.getHash() && _toUpdate == _updateWith );
        if( _toUpdate.mpPolynomial == nullptr && _updateWith.mpPolynomial != nullptr )
            return true;
        assert( _updateWith.mpPolynomial == nullptr || (*_toUpdate.mpPolynomial) == (*_updateWith.mpPolynomial) );
        return !_updateWith.factorization().empty() && !factorizationsEqual( _toUpdate.factorization(), _updateWith.factorization() );
    }

    template<typename P>
    void update( PolynomialFactorizationPair<P>& _toUpdate, PolynomialFactorizationPair<P>& _updateWith )
    {
        assert( canBeUpdated( _toUpdate, _updateWith ) ); // This assertion only ensures efficient use this method.
        assert( &_toUpdate != &_updateWith );
        assert( _toUpdate.mpPolynomial == nullptr || _updateWith.mpPolynomial == nullptr || *_toUpdate.mpPolynomial == *_updateWith.mpPolynomial );
        std::lock_guard<std::recursive_mutex> lockA( _toUpdate.mMutex );
        std::lock_guard<std::recursive_mutex> lockB( _updateWith.mMutex );
        if( _toUpdate.mpPolynomial == nullptr && _updateWith.mpPolynomial != nullptr )
            _toUpdate.mpPolynomial = _updateWith.mpPolynomial;
        // The factorization of the PolynomialFactorizationPair to update with can be empty, if constructed freshly by a polynomial.
        if( !factorizationsEqual( _toUpdate.factorization(), _updateWith.factorization() ) )
        {
            // Calculating the gcd refines both factorizations to the same factorization
            bool refineA = false;
            bool refineB = false;
            Factorization<P> restA, restB;
            typename P::CoeffType c( 0 );
            gcd( _toUpdate, _updateWith, restA, restB, c, refineA, refineB );
            assert( c == typename P::CoeffType( 0 ) || c == typename P::CoeffType( 1 ) );
        }
    }

    template<typename P>
    P computePolynomial( const Factorization<P>& _factorization )
    {
        P result( 1 );
        for (auto factor = _factorization.begin(); factor != _factorization.end(); factor++ )
        {
            assert( existsFactorization( factor->first ) );
            result *= computePolynomial( factor->first.content() ).pow( factor->second );
        }
        return result;
    }

    template<typename P>
    P computePolynomial( const PolynomialFactorizationPair<P>& _pfPair )
    {
        if( _pfPair.mpPolynomial != nullptr )
            return *_pfPair.mpPolynomial;
        return computePolynomial( _pfPair.factorization() );
    }

    template<typename P>
    typename P::CoeffType PolynomialFactorizationPair<P>::flattenFactorization() const
    {
        typename P::CoeffType result( 0 );
        if ( factorizedTrivially() )
        {
            return result;
        }
        std::lock_guard<std::recursive_mutex> lock( mMutex );
        for( auto factor = mFactorization.begin(); factor != mFactorization.end(); )
        {
            if( factor->first.content().factorizedTrivially() )
            {
                if ( factor->first.coefficient() != 1 )
                {
                    if( result == typename P::CoeffType( 0 ) )
                        result = typename P::CoeffType( 1 );
                    carl::exponent e = factor->second;
                    assert( e != 0 );
                    result *= carl::pow( factor->first.coefficient(), e );
                    factor->first.setCoefficient( 1 );
                }
                ++factor;
            }
            else
            {
                // Update factorization
                if( result == typename P::CoeffType( 0 ) )
                    result = typename P::CoeffType( 1 );
                const Factorization<P>& partFactorization = factor->first.factorization();
                assert( factor->first.coefficient() > 0 );
                carl::exponent e = factor->second;
                assert( e != 0 );
                result *= carl::pow( factor->first.coefficient(), e );
                factor = mFactorization.erase(factor);

                for( auto partFactor = partFactorization.begin(); partFactor != partFactorization.end(); partFactor++ )
                {
                    auto insertResult = mFactorization.insert( std::pair<FactorizedPolynomial<P>, carl::exponent>( partFactor->first, partFactor->second * e ) );
                    if ( !insertResult.second )
                    {
                        //Increment exponent for already existing factor
                        insertResult.first->second += partFactor->second * e;
                    }
                }
            }
        }
        assert( assertFactorization() );

        return result;
    }
    
    template<typename P>
    bool PolynomialFactorizationPair<P>::isIrreducible() const
    {
        if ( mIrreducible != -1 )
            return mIrreducible == 1;

        assert( mpPolynomial != nullptr );
        if ( mpPolynomial->isLinear() )
        {
            mIrreducible = 1;
            return true;
        }
        Definiteness definiteness =  mpPolynomial->definiteness();
        if ( definiteness == Definiteness::POSITIVE || definiteness == Definiteness::NEGATIVE )
        {
            mIrreducible = 1;
            return true;
        }
        mIrreducible = 0;
        return false;
    }

    template<typename P>
    void PolynomialFactorizationPair<P>::setNewFactors( const FactorizedPolynomial<P>& _fpolyA, carl::exponent exponentA, const FactorizedPolynomial<P>& _fpolyB, carl::exponent exponentB ) const
    {
        assert( mFactorization.size() == 1 );
        assert( factorizedTrivially() );
        assert( !_fpolyA.isOne() );
        assert( !_fpolyB.isOne() );
        //assert( _fpolyA.coefficient() == 1);
        //assert( _fpolyB.coefficient() == 1);
        assert( exponentA > 0 );
        assert( exponentB > 0 );
        mFactorization.clear();
        if( _fpolyA == _fpolyB )
        {
            mFactorization.insert ( std::pair<FactorizedPolynomial<P>, carl::exponent>( _fpolyA, exponentA+exponentB ) );
        }
        else
        {   
            mFactorization.insert ( std::pair<FactorizedPolynomial<P>, carl::exponent>( _fpolyA, exponentA ) );
            mFactorization.insert ( std::pair<FactorizedPolynomial<P>, carl::exponent>( _fpolyB, exponentB ) );
        }
        assert( mpPolynomial != nullptr );
        assert( *mpPolynomial == computePolynomial( mFactorization ) );
        assert( assertFactorization() );
    }

    template<typename P>
    Factorization<P> gcd( const PolynomialFactorizationPair<P>& _pfPairA, const PolynomialFactorizationPair<P>& _pfPairB, Factorization<P>& _restA, Factorization<P>& _restB, typename P::CoeffType& _coeff, bool& _pfPairARefined, bool& _pfPairBRefined )
    {
        std::cout << "at entry: " << std::endl;
        std::cout << "_pfPairA: " << _pfPairA << std::endl;
        std::cout << "_pfPairB: " << _pfPairB << std::endl;
        std::cout << "_restA: " << _restA << std::endl;
        std::cout << "_restB: " << _restB << std::endl;
        std::cout << "_coeff:" << _coeff << std::endl;
        std::cout << "_pfPairARefined: " << _pfPairARefined << std::endl;

        if( &_pfPairA == &_pfPairB )
            return _pfPairA.factorization();

        std::lock_guard<std::recursive_mutex> lockA( _pfPairA.mMutex );
        std::lock_guard<std::recursive_mutex> lockB( _pfPairB.mMutex );

        _coeff = typename P::CoeffType( 1 );
        _pfPairARefined = false;
        _pfPairBRefined = false;
        Factorization<P> result;
        _restA.clear();
        _restB.clear();
        typename P::CoeffType cA = _pfPairA.flattenFactorization();
        if( cA != typename P::CoeffType( 0 ) )
        {
            _pfPairARefined = true;
            _coeff *= cA;
        }
        typename P::CoeffType cB = _pfPairB.flattenFactorization();
        if( cB != typename P::CoeffType( 0 ) )
        {
            _pfPairBRefined = true;
            _coeff *= cB;
        }
        Factorization<P> factorizationA = _pfPairA.mFactorization;
        Factorization<P> factorizationB = _pfPairB.mFactorization;
        bool rest = true;
        CARL_LOG_DEBUG( "carl.core.factorizedpolynomial", "Compute GCD (internal) of " << _pfPairA << " and " << _pfPairB );

        while ( !factorizationA.empty() )
        {
            //Consider first factor in currently not checked factorization of A
            FactorizedPolynomial<P> factorA = factorizationA.begin()->first;
            carl::exponent exponentA = factorizationA.begin()->second;
            CARL_LOG_TRACE( "carl.core.factorizedpolynomial", "FactorA: (" << factorA << ")^" << exponentA );
            factorizationA.erase( factorizationA.begin() );
            rest = true;
            assert( factorA.factorization().size() == 1);

            while ( !factorA.isOne() && !factorizationB.empty() )
            {
                FactorizedPolynomial<P> factorB = factorizationB.begin()->first;
                carl::exponent exponentB = factorizationB.begin()->second;
                CARL_LOG_TRACE( "carl.core.factorizedpolynomial", "FactorB: (" << factorB << ")^" << exponentB );
                factorizationB.erase( factorizationB.begin() );
                assert( factorB.factorization().size() == 1);

                if( factorA == factorB )
                {
                    //Common factor found
                    carl::exponent exponentCommon = exponentA < exponentB ? exponentA : exponentB;
                    result.insert( result.end(), std::pair<FactorizedPolynomial<P>, carl::exponent>( factorA, exponentCommon ) );
                    CARL_LOG_TRACE( "carl.core.factorizedpolynomial", "Existing common factor: (" << factorA << ")^" << exponentCommon );
                    if (exponentA > exponentCommon)
                        factorizationA.insert( factorizationA.end(), std::pair<FactorizedPolynomial<P>, carl::exponent>( factorA, exponentA-exponentCommon ) );
                    if (exponentB > exponentCommon)
                        factorizationB.insert( factorizationB.end(), std::pair<FactorizedPolynomial<P>, carl::exponent>( factorB, exponentB-exponentCommon ) );
                    //No rest is remaining
                    rest = false;
                    break;
                }
                else
                {
                    P polGCD, polA, polB;
                    assert( existsFactorization( factorA ) );
                    assert( existsFactorization( factorB ) );
                    if ( factorA.content().isIrreducible() && factorB.content().isIrreducible() )
                        polGCD = P( 1 );
                    else
                    {
                        //Compute GCD of factors
                        assert( factorA.content().mpPolynomial != nullptr );
                        assert( factorB.content().mpPolynomial != nullptr );
                        polA = *factorA.content().mpPolynomial;
                        polB = *factorB.content().mpPolynomial;
                        polGCD = carl::gcd( polA, polB );
                    }

                    if (polGCD.isOne())
                    {
                        //No common factor
                        _restB.insert( _restB.end(), std::pair<FactorizedPolynomial<P>, carl::exponent>( factorB, exponentB ) );
                        CARL_LOG_TRACE( "carl.core.factorizedpolynomial", "No common factor, insert in restB: (" << factorB << ")^" << exponentB );
                    }
                    else
                    {
                        //New common factor
                        P remainA = polA.quotient( polGCD );
                        P remainB = polB.quotient( polGCD );
                        carl::exponent exponentCommon = exponentA < exponentB ? exponentA : exponentB;
                        std::shared_ptr<Cache<PolynomialFactorizationPair<P>>> cache = factorA.pCache();
                        //Set new part of GCD
                        FactorizedPolynomial<P> gcdResult( polGCD, cache );
                        result.insert( result.end(), std::pair<FactorizedPolynomial<P>, carl::exponent>( gcdResult,  exponentCommon ) );
                        CARL_LOG_TRACE( "carl.core.factorizedpolynomial", "New common factor: (" << gcdResult << ")^" << exponentCommon );

                        if (!remainA.isOne())
                        {
                            //Set new factorization
                            FactorizedPolynomial<P> polRemainA( remainA, cache );
                            factorA.content().setNewFactors( gcdResult, exponentA, polRemainA, exponentA );
                            _pfPairARefined = true;
                            CARL_LOG_TRACE( "carl.core.factorizedpolynomial", "RemainderA: " << polRemainA );
                            factorA = polRemainA;

                            //Add remaining factorization
                            if (exponentA > exponentCommon)
                                factorizationA.insert( factorizationA.end(), std::pair<FactorizedPolynomial<P>, carl::exponent>( gcdResult, exponentA-exponentCommon ) );
                        }
                        else
                        {
                            if ( exponentA > exponentCommon )
                            {
                                exponentA -= exponentCommon;
                            }
                            else
                                rest = false;
                        }
                        if (!remainB.isOne())
                        {
                            //Set new factorization
                            FactorizedPolynomial<P> polRemainB( remainB, cache );
                            factorB.content().setNewFactors( gcdResult, exponentB, polRemainB, exponentB );
                            _pfPairBRefined = true;
                            CARL_LOG_TRACE( "carl.core.factorizedpolynomial", "RemainderB: " << polRemainB );

                            //Add remaining factorization
                            if (exponentB > exponentCommon)
                                factorizationB.insert( factorizationB.end(), std::pair<FactorizedPolynomial<P>, carl::exponent>( gcdResult, exponentB-exponentCommon ) );
                            _restB.insert( _restB.end(), std::pair<FactorizedPolynomial<P>, carl::exponent>( polRemainB, exponentB) );
                        }
                        else if ( exponentB > exponentCommon )
                        {
                            _restB.insert( _restB.end(), std::pair<FactorizedPolynomial<P>, carl::exponent>( gcdResult, exponentB-exponentCommon) );
                        }
                    }
                }
            } //End of inner while
            //Insert remaining factorA into rest
            if ( !factorA.isOne() && rest )
                _restA.insert( _restA.begin(), std::pair<FactorizedPolynomial<P>, carl::exponent>( factorA, exponentA ) );
            //Reset factorizationB
            for ( auto itFactor = _restB.begin(); itFactor != _restB.end(); itFactor++ )
                factorizationB.insert( factorizationB.end(), *itFactor );
            _restB.clear();
        } //End of outer while
        _restB = factorizationB;

        cA = _pfPairA.flattenFactorization();
        if( cA != typename P::CoeffType( 0 ) )
        {
            _pfPairARefined = true;
            _coeff *= cA;
        }
        cB = _pfPairB.flattenFactorization();
        if( cB != typename P::CoeffType( 0 ) )
        {
            _pfPairBRefined = true;
            _coeff *= cB;
        }

        assert( _coeff == 1 );
        // Check correctness
        assert( _pfPairA.assertFactorization() );
        assert( _pfPairB.assertFactorization() );
        CARL_LOG_DEBUG( "carl.core.factorizedpolynomial", "GCD (internal) of " << _pfPairA << " and " << _pfPairB << ": " << result << " with rests " << _restA << " and " << _restB );
        std::cout << "at end: " << std::endl;
        std::cout << "_pfPairA: " << _pfPairA << std::endl;
        std::cout << "_pfPairB: " << _pfPairB << std::endl;
        std::cout << "_restA: " << _restA << std::endl;
        std::cout << "_restB: " << _restB << std::endl;
        std::cout << "_coeff:" << _coeff << std::endl;
        std::cout << "_pfPairARefined: " << _pfPairARefined << std::endl;
        std::cout << computePolynomial( result ) << " * " << computePolynomial( _restA ) << " == " << computePolynomial( _pfPairA ) << " failed." << std::endl;
        assert( computePolynomial( result ) * computePolynomial( _restA ) == computePolynomial( _pfPairA ) );
        assert( computePolynomial( result ) * computePolynomial( _restB ) == computePolynomial( _pfPairB ) );

        return result;
    }
    
    template <typename P>
    std::ostream& operator<<(std::ostream& _out, const PolynomialFactorizationPair<P>& _pfPair)
    {
        if( _pfPair.factorizedTrivially() )
        {
            assert( _pfPair.mpPolynomial != nullptr );
            _out << *_pfPair.mpPolynomial;
        }
        else
        {
            _out << _pfPair.factorization();
        }
        return _out;
    }
    
} // namespace carl
