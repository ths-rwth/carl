/* 
 * File:   PolynomialFactorizationPair.tpp
 * Author: Florian Corzilius
 *
 * Created on September 5, 2014, 3:57 PM
 */

#pragma once

#include "PolynomialFactorizationPair.h"

#include "FactorizedPolynomial.h"
#include <carl-logging/carl-logging.h>
#include <carl/poly/umvpoly/functions/Definiteness.h>
#include <carl/poly/umvpoly/functions/GCD.h>

namespace carl
{
    template <typename P>
    std::string factorizationToString( const Factorization<P>& _factorization, bool _infix, bool _friendlyVarNames )
    {
        if( _factorization.empty() )
        {
            return "1";
        }
        else
        {
            std::string result;
            if( _infix )
            {
                for( auto polyExpPair = _factorization.begin(); polyExpPair != _factorization.end(); ++polyExpPair )
                {
                    if( polyExpPair != _factorization.begin() )
                        result += " * ";
                    assert( polyExpPair->second > 0 );
                    result += "(" + polyExpPair->first.toString( true, _friendlyVarNames ) + ")";
                    if( polyExpPair->second > 1 )
                    {
                        std::stringstream s;
                        s << polyExpPair->second;
                        result += "^" + s.str();
                    }
                }
            }
            else
            {
                if( _factorization.size() == 1 && _factorization.begin()->second == 1 )
                {
                    return _factorization.begin()->first.toString( false, _friendlyVarNames );
                }
                result += "(*";
                for( auto polyExpPair = _factorization.begin(); polyExpPair != _factorization.end(); ++polyExpPair )
                {
                    assert( polyExpPair->second > 0 );
                    for( size_t i = 0; i < polyExpPair->second; ++i )
                        result += " " + polyExpPair->first.toString( false, _friendlyVarNames );
                }
                result += ")";
            }
            return result;
        }
    }
    
    template <typename P>
    std::ostream& operator<<( std::ostream& _out, const Factorization<P>& _factorization )
    {
        _out << factorizationToString( _factorization );
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
		mMutex(),
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
                mpPolynomial = new P( carl::pow(*mFactorization.begin()->first.content().mpPolynomial, mFactorization.begin()->second ) );
                assert( mpPolynomial != nullptr );
            }
        }
        else
        {
            if ( is_one(*mpPolynomial) )
            {
                assert( mFactorization.size() == 0);
                mpPolynomial = nullptr;
            }
        }

        // Check correctness
        assert( mpPolynomial == nullptr || carl::is_one(mpPolynomial->coprime_factor()) );
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
            if( factorizationsEqual( _polyFactA.factorization(), _polyFactB.factorization() ) )
                return true;
            else
            {
                // There is no way around this );
                if( _polyFactA.mpPolynomial == nullptr )
                    _polyFactA.mpPolynomial = new P( computePolynomial( _polyFactA.factorization() ) );
                if( _polyFactB.mpPolynomial == nullptr )
                    _polyFactB.mpPolynomial = new P( computePolynomial( _polyFactB.factorization() ) );
                return *_polyFactA.mpPolynomial == *_polyFactB.mpPolynomial;
            }
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
        assert( _toUpdate.hash() == _updateWith.hash() && _toUpdate == _updateWith );
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
        // The factorization of the PolynomialFactorizationPair to update which can be empty, if constructed freshly by a polynomial.
        if( _toUpdate.factorizedTrivially() && !_updateWith.factorizedTrivially() )
        {
            _toUpdate.mFactorization = _updateWith.mFactorization;
        }
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
        for (auto ft = _factorization.begin(); ft != _factorization.end(); ft++ )
        {
            result *= carl::pow(computePolynomial( ft->first ), ft->second );
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
        for( auto ft = mFactorization.begin(); ft != mFactorization.end(); )
        {
            if( ft->first.content().factorizedTrivially() )
            {
                if ( ft->first.coefficient() != 1 )
                {
                    if( result == typename P::CoeffType( 0 ) )
                        result = typename P::CoeffType( 1 );
                    carl::exponent e = ft->second;
                    assert( e != 0 );
                    result *= carl::pow( ft->first.coefficient(), e );
                    ft->first.setCoefficient( 1 );
                }
                ++ft;
            }
            else
            {
                // Update factorization
                if( result == typename P::CoeffType( 0 ) )
                    result = typename P::CoeffType( 1 );
                const Factorization<P>& partFactorization = ft->first.factorization();
                assert( carl::is_positive( ft->first.coefficient() ) );
                carl::exponent e = ft->second;
                assert( e != 0 );
                result *= carl::pow( ft->first.coefficient(), e );

                for( auto partFactor = partFactorization.begin(); partFactor != partFactorization.end(); partFactor++ )
                {
                    mFactorization.insert( std::pair<FactorizedPolynomial<P>, carl::exponent>( partFactor->first, partFactor->second * e ) );
                }
                ft = mFactorization.erase(ft);
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
        if ( mpPolynomial->is_linear() )
        {
            mIrreducible = 1;
            return true;
        }
        Definiteness definiteness = carl::definiteness(*mpPolynomial);
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
        assert( !_fpolyA.is_one() );
        assert( !_fpolyB.is_one() );
        //assert( carl::is_one(_fpolyA.coefficient()) );
        //assert( carl::is_one(_fpolyB.coefficient()) );
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
        CARL_LOG_DEBUG( "carl.core.factorizedpolynomial", "****************************************************" );
        CARL_LOG_DEBUG( "carl.core.factorizedpolynomial", "Compute GCD (internal) of " << _pfPairA << " and " << _pfPairB );
        if( &_pfPairA == &_pfPairB )
            return _pfPairA.factorization();
        std::lock_guard<std::recursive_mutex> lockA( _pfPairA.mMutex );
        std::lock_guard<std::recursive_mutex> lockB( _pfPairB.mMutex );
        _coeff = typename P::CoeffType( 1 );
        _pfPairARefined = false;
        _pfPairBRefined = false;
        _restA.clear();
        _restB.clear();
        // First flatten the factorizations of both polynomials
        typename P::CoeffType cA = _pfPairA.flattenFactorization();
        if( cA != typename P::CoeffType( 0 ) )
        {
            // Refinement took place -> set flag which indicates to update the first given factorized polynomial in its cache
            _pfPairARefined = true;
            _coeff *= cA;
        }
        typename P::CoeffType cB = _pfPairB.flattenFactorization();
        if( cB != typename P::CoeffType( 0 ) )
        {
            // Refinement took place -> set flag which indicates to update the second given factorized polynomial in its cache
            _pfPairBRefined = true;
            _coeff *= cB;
        }
//        Factorization<P> factorizationA = _pfPairA.mFactorization;
//        Factorization<P> factorizationB = _pfPairB.mFactorization;
//        Factorization<P> result;
        Factorization<P> factorizationA;
        Factorization<P> factorizationB;
        Factorization<P> result = commonDivisor( _pfPairA.mFactorization, _pfPairB.mFactorization, factorizationA, factorizationB );
        while ( !factorizationA.empty() )
        {
            //Consider first factor in currently not checked factorization of A
            FactorizedPolynomial<P> factorA = factorizationA.begin()->first;
            carl::exponent exponentA = factorizationA.begin()->second;
            factorizationA.erase( factorizationA.begin() );
            bool breaked = false;
            while ( !factorA.is_one() && !factorizationB.empty() )
            {
                // If the first factor is has a non trivial factorization -> flatten (this could be introduced during this loop even if we flatted beforehand)
                const Factorization<P>& currentFactorizationA = factorA.factorization();
                if( currentFactorizationA.size() != 1 )
                {
                    factorizationA.insert( currentFactorizationA.begin(), currentFactorizationA.end() );
                    breaked = true;
                    break;
                } else if ( currentFactorizationA.begin()->second > 1 ) {
                    // Polynomial has exponent > 1
                    exponentA *= currentFactorizationA.begin()->second;
                    factorA = currentFactorizationA.begin()->first;
                }
                // Take the second factor
                FactorizedPolynomial<P> factorB = factorizationB.begin()->first;
                carl::exponent exponentB = factorizationB.begin()->second;
                factorizationB.erase( factorizationB.begin() );
                // If the second factor is has a non trivial factorization -> flatten (this could be introduced during this loop even if we flatted beforehand)
                const Factorization<P>& currentFactorizationB = factorB.factorization();
                if( currentFactorizationB.size() != 1 )
                {
                    factorizationB.insert( currentFactorizationB.begin(), currentFactorizationB.end() );
                    continue;
                } else if ( currentFactorizationB.begin()->second > 1 ) {
                    // Polynomial has exponent > 1
                    exponentB *= currentFactorizationB.begin()->second;
                    factorB = currentFactorizationB.begin()->first;
                }
                CARL_LOG_DEBUG( "carl.core.factorizedpolynomial", "" );
                CARL_LOG_DEBUG( "carl.core.factorizedpolynomial", "####################################################" );
                CARL_LOG_DEBUG( "carl.core.factorizedpolynomial", "For first factorization: " );
                CARL_LOG_DEBUG( "carl.core.factorizedpolynomial", "      current factor: (" << factorA << ")^" << exponentA );
                CARL_LOG_DEBUG( "carl.core.factorizedpolynomial", "      open remainder: " << factorizationA );
                CARL_LOG_DEBUG( "carl.core.factorizedpolynomial", "    closed remainder: " << _restA );
                CARL_LOG_DEBUG( "carl.core.factorizedpolynomial", "For second factorization: " );
                CARL_LOG_DEBUG( "carl.core.factorizedpolynomial", "      current factor: (" << factorB << ")^" << exponentB );
                CARL_LOG_DEBUG( "carl.core.factorizedpolynomial", "      open remainder: " << factorizationB );
                CARL_LOG_DEBUG( "carl.core.factorizedpolynomial", "    closed remainder: " << _restB );
                CARL_LOG_DEBUG( "carl.core.factorizedpolynomial", "already found gcd is:" << result );
                CARL_LOG_DEBUG( "carl.core.factorizedpolynomial", "" );
                // Check if the factors are equal (even if we removed equal factor by commonDivisor() beforehand, common factors could be introduced during this loop)
                if( factorA == factorB )
                {
                    //Common factor found
                    carl::exponent exponentCommon = exponentA < exponentB ? exponentA : exponentB;
                    result.insert( std::pair<FactorizedPolynomial<P>, carl::exponent>( factorA, exponentCommon ) );
                    CARL_LOG_DEBUG( "carl.core.factorizedpolynomial", __LINE__ << ": add (" << factorA << ")^" << exponentCommon << " to gcd: " << result );
                    if (exponentA > exponentCommon)
                    {
                        factorizationA.insert( std::pair<FactorizedPolynomial<P>, carl::exponent>( factorA, exponentA-exponentCommon ) );
                        CARL_LOG_DEBUG( "carl.core.factorizedpolynomial", __LINE__ << ": add (" << factorA << ")^" << (exponentA-exponentCommon) << " to first open remainder: " << factorizationA );
                    }
                    if (exponentB > exponentCommon)
                    {
                        //Ignore FactorB as it has no remaining common factor with current FactorB
                        _restB.insert( std::pair<FactorizedPolynomial<P>, carl::exponent>( factorB, exponentB-exponentCommon ) );
                        CARL_LOG_DEBUG( "carl.core.factorizedpolynomial", __LINE__ << ": add (" << factorB << ")^" << (exponentB-exponentCommon) << " to second open remainder: " << _restB );
                    }
                    factorA = FactorizedPolynomial<P>( carl::constant_one<typename P::CoeffType>::get() );
                    CARL_LOG_DEBUG( "carl.core.factorizedpolynomial", __LINE__ << ": set current factor to: (1)^1" );
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
                        if (carl::is_negative(polGCD.lcoeff())) {
                            polGCD = -polGCD;
                        }
                        CARL_LOG_DEBUG( "carl.core.factorizedpolynomial", __LINE__ << ": GCD of " << polA << " and " << polB << ": " << polGCD);
                    }

                    if (is_one(polGCD))
                    {
                        //Ignore FactorB as it has no common factor with current FactorA
                        _restB.insert( std::pair<FactorizedPolynomial<P>, carl::exponent>( factorB, exponentB ) );
                        CARL_LOG_DEBUG( "carl.core.factorizedpolynomial", __LINE__ << ": add (" << factorB << ")^" << exponentB << " to second finished remainder: " << _restB );
                    }
                    else
                    {
                        //New common factor
                        P remainA = carl::quotient(polA, polGCD );
                        P remainB = carl::quotient(polB, polGCD );
                        carl::exponent exponentCommon = exponentA < exponentB ? exponentA : exponentB;
                        std::shared_ptr<Cache<PolynomialFactorizationPair<P>>> cache = factorA.pCache();
                        //Set new part of GCD
                        FactorizedPolynomial<P> gcdResult( polGCD, cache );
                        result.insert( std::pair<FactorizedPolynomial<P>, carl::exponent>( gcdResult,  exponentCommon ) );
                        CARL_LOG_DEBUG( "carl.core.factorizedpolynomial", __LINE__ << ": add (" << gcdResult << ")^" << exponentCommon << " to gcd: " << result );
                        if (is_one(remainA))
                        {
                            if ( exponentA > exponentCommon )
                            {
                                exponentA -= exponentCommon;
                            }
                            else
                            {
                                factorA = FactorizedPolynomial<P>( carl::constant_one<typename P::CoeffType>::get() );
                                CARL_LOG_DEBUG( "carl.core.factorizedpolynomial", __LINE__ << ": set current factor to: (1)^1" );
                            }
                        }
                        else
                        {
                            //Set new factorization
                            FactorizedPolynomial<P> polRemainA( remainA, cache );
                            factorA.content().setNewFactors( gcdResult, 1, polRemainA, 1 );
                            _pfPairARefined = true;
                            factorA = polRemainA;
                            //Add remaining factorization
                            if (exponentA > exponentCommon)
                            {
                                factorizationA.insert( std::pair<FactorizedPolynomial<P>, carl::exponent>( gcdResult, exponentA-exponentCommon ) );
                                CARL_LOG_DEBUG( "carl.core.factorizedpolynomial", __LINE__ << ": add (" << gcdResult << ")^" << (exponentA-exponentCommon) << " to first open remainder: " << factorizationA );
                            }
                        }

                        //Part of FactorB remains
                        if ( exponentB > exponentCommon )
                        {
                            factorizationB.insert( std::pair<FactorizedPolynomial<P>, carl::exponent>( gcdResult, exponentB-exponentCommon) );
                            CARL_LOG_DEBUG( "carl.core.factorizedpolynomial", __LINE__ << ": add (" << gcdResult << ")^" << (exponentB-exponentCommon) << " to second open remainder: " << factorizationB );
                        }

                        if (!is_one(remainB))
                        {
                            //Set new factorization
                            FactorizedPolynomial<P> polRemainB( remainB, cache );
                            factorB.content().setNewFactors( gcdResult, 1, polRemainB, 1 );
                            _pfPairBRefined = true;
                            //Ignore remaining factorization as it has no common factor with FactorA anymore
                            factorizationB.insert( std::pair<FactorizedPolynomial<P>, carl::exponent>( polRemainB, exponentB) );
                            CARL_LOG_DEBUG( "carl.core.factorizedpolynomial", __LINE__ << ": add (" << polRemainB << ")^" << exponentB << " to second open remainder: " << _restB );
                        }
                    }
                }
            } //End of inner while
            if( breaked )
                continue;
            //Insert remaining factorA into rest
            if( !factorA.is_one() )
            {
                _restA.insert( std::pair<FactorizedPolynomial<P>, carl::exponent>( factorA, exponentA ) );
                CARL_LOG_DEBUG( "carl.core.factorizedpolynomial", __LINE__ << ": add (" << factorA << ")^" << exponentA << " to first closed remainder: " << _restA );
            }
            //Reset factorizationB
            factorizationB.insert( _restB.begin(), _restB.end() );
            _restB.clear();
            CARL_LOG_DEBUG( "carl.core.factorizedpolynomial", __LINE__ << ": shift second closed remainder to second open remainder: " << factorizationB );
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
        assert( carl::is_one(_coeff) );
        // Check correctness
        assert( _pfPairA.assertFactorization() );
        assert( _pfPairB.assertFactorization() );
        CARL_LOG_DEBUG( "carl.core.factorizedpolynomial", "GCD (internal) of " << _pfPairA << " and " << _pfPairB << ": " << result << " with rests " << _restA << " and " << _restB );
        assert( computePolynomial( result ) * computePolynomial( _restA ) == computePolynomial( _pfPairA ) );
        assert( computePolynomial( result ) * computePolynomial( _restB ) == computePolynomial( _pfPairB ) );
        assert( carl::is_one(carl::gcd( computePolynomial( _restA ), computePolynomial( _restB ))));
        return result;
    }
    
    template<typename P>
    Factors<FactorizedPolynomial<P>> factor( const PolynomialFactorizationPair<P>& _pfPair, const typename P::CoeffType& _coeff )
    {
        typename P::CoeffType constantFactor = _coeff;
        bool allFactorsIrreducible = false;
        Factors<FactorizedPolynomial<P>> result;
        if( !_pfPair.mFactorization.empty() )
        {
            _pfPair.mMutex.lock();
            while( !allFactorsIrreducible )
            {
                _pfPair.flattenFactorization();
                allFactorsIrreducible = true;
                for( auto ft = _pfPair.mFactorization.begin(); ft != _pfPair.mFactorization.end(); )
                {
                    assert( existsFactorization( ft->first ) );
                    if( ft->first.content().isIrreducible() )
                    {
                        ++ft;
                    }
                    else
                    {
                        assert( ft->first.factorization().size() == 1 );
                        assert( carl::is_one( ft->first.coefficient() ) );
                        carl::exponent e = ft->second;
                        assert( e != 0 );
                        Factors<typename FactorizedPolynomial<P>::PolyType> factorFactorization = carl::factor( ft->first.polynomial() );
                        Factorization<P> refinement;
                        for( const auto& pt : factorFactorization )
                        {
                            if( pt.first.is_constant() )
                                constantFactor *= pt.first.constant_part();
                            else
                            {
                                FactorizedPolynomial<P> fp(pt.first, ft->first.pCache());
                                constantFactor *= carl::pow( fp.coefficient(), pt.second );
                                refinement.insert( std::pair<FactorizedPolynomial<P>, carl::exponent>( fp.coprime_coefficients(), pt.second ) );
                            }
                        }
                        ft = _pfPair.mFactorization.erase(ft);

                        for( auto partFactor = refinement.begin(); partFactor != refinement.end(); partFactor++ )
                        {
                            assert( existsFactorization( partFactor->first ) );
                            auto insertResult = _pfPair.mFactorization.insert( std::pair<FactorizedPolynomial<P>, carl::exponent>( partFactor->first, partFactor->second * e ) );
                            if( insertResult.first->first.factorization().size() > 1 ) // Note that factorization() takes care about the flattening
                            {
                                assert( insertResult.first->first.content().mIrreducible == 0 );
                                allFactorsIrreducible = false;
                            }
                            else
                            {
                                insertResult.first->first.content().mIrreducible = 1;
                            }
                        }
                    }
                }
            }
            _pfPair.mMutex.unlock();
            for( auto ft = _pfPair.mFactorization.begin(); ft != _pfPair.mFactorization.end(); ft++ )
            {
                result.insert( std::pair<FactorizedPolynomial<P>, carl::exponent>( ft->first, ft->second ) );
            }
        }
        result.insert( std::pair<FactorizedPolynomial<P>, unsigned>( FactorizedPolynomial<P>(constantFactor), 1 ) );
        return result;
    }
    
    template <typename P>
    std::ostream& operator<<(std::ostream& _out, const PolynomialFactorizationPair<P>& _pfPair)
    {
		if (_pfPair.factorizedTrivially()) {
			return _out << _pfPair.polynomial();
		} else {
			return _out << factorizationToString(_pfPair.factorization());
		}
    }
    
} // namespace carl
