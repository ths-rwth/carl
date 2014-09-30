/* 
 * File:   PolynomialFactorizationPair.tpp
 * Author: Florian Corzilius
 *
 * Created on September 5, 2014, 3:57 PM
 */

#pragma once

#include "PolynomialFactorizationPair.h"

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
        mpPolynomial( _polynomial )
    {
        if ( mFactorization.size() == 1 && mpPolynomial == nullptr )
        {
            // No factorization -> set polynomial
            mpPolynomial = new P( mFactorization.begin()->first.content().mpPolynomial->pow( mFactorization.begin()->second ) );
            assert( mpPolynomial != nullptr );
        }

        // Check correctness
        if ( mpPolynomial != nullptr )
        {
            assert( mpPolynomial->coprimeFactor() == 1);
            if ( !mFactorization.empty() )
                assertFactorization();
        }

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
            assert( getFactorization().empty() || getFactorization().size() > 1 );
            mHash = 0;
            for( auto polyExpPair = getFactorization().begin(); polyExpPair != getFactorization().end(); ++polyExpPair )
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
        if( &_polyFactA == &_polyFactB )
            return true;
        std::lock_guard<std::recursive_mutex> lockA( _polyFactA.mMutex );
        std::lock_guard<std::recursive_mutex> lockB( _polyFactB.mMutex );
        if( _polyFactA.mpPolynomial != nullptr && _polyFactB.mpPolynomial != nullptr )
        {
            return *_polyFactA.mpPolynomial == *_polyFactB.mpPolynomial;
        }
        else
        {
            return factorizationsEqual( _polyFactA.getFactorization(), _polyFactB.getFactorization() );
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
            auto iterA = _polyFactA.getFactorization().begin();
            auto iterB = _polyFactB.getFactorization().begin();
            while( iterA != _polyFactA.getFactorization().end() && iterB != _polyFactB.getFactorization().end() )
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
        return !factorizationsEqual( _toUpdate.getFactorization(), _updateWith.getFactorization() );
    }

    template<typename P>
    void update( PolynomialFactorizationPair<P>& _toUpdate, PolynomialFactorizationPair<P>& _updateWith )
    {
        assert( canBeUpdated( _toUpdate, _updateWith ) ); // This assertion only ensures efficient use this method.
        assert( &_toUpdate != &_updateWith );
        std::lock_guard<std::recursive_mutex> lockA( _toUpdate.mMutex );
        std::lock_guard<std::recursive_mutex> lockB( _updateWith.mMutex );
        if( _toUpdate.mpPolynomial == nullptr && _updateWith.mpPolynomial != nullptr )
            _toUpdate.mpPolynomial = _updateWith.mpPolynomial;
        if( !factorizationsEqual( _toUpdate.getFactorization(), _updateWith.getFactorization() ) )
        {
            // Calculating the gcd refines both factorizations to the same factorization
            gcd( _toUpdate, _updateWith );
        }
        _toUpdate.rehash();
    }

    template<typename P>
    P computePolynomial( const Factorization<P>& _fFactorization )
    {
        P result( 1 );
        for (auto factor = _fFactorization.begin(); factor != _fFactorization.end(); factor++ )
        {
            result *= factor->first.content().mpPolynomial->pow(factor->second);
        }
        return result;
    }

    template<typename P>
    void PolynomialFactorizationPair<P>::flattenFactorization() const
    {
        if ( mFactorization.size() == 1 && mFactorization.begin()->second == 1 )
        {
            return;
        }
        std::lock_guard<std::recursive_mutex> lock( mMutex );
        for ( auto factor = mFactorization.begin(); factor != mFactorization.end(); factor++ )
        {
            if (factor->first.rFactorization().size() > 1){
                //Update factorization
                Factorization<P> partFactorization = factor->first.rFactorization();
                size_t exponent = factor->second;
                factor = mFactorization.erase(factor);
                for ( auto partFactor = partFactorization.begin(); partFactor != partFactorization.end(); partFactor++ )
                {
                    mFactorization.insert( factor, std::pair<FactorizedPolynomial<P>, size_t>( partFactor->first, partFactor->second * exponent ) );
                }
            }
        }
        assertFactorization();
    }
    
    template<typename P>
    void PolynomialFactorizationPair<P>::setNewFactors( const FactorizedPolynomial<P>& _fpolyA, size_t exponentA, const FactorizedPolynomial<P>& _fpolyB, size_t exponentB ) const
    {
        Factorization<P> factorization = getFactorization();
        assert( factorization.size() == 1 );
        factorization.clear();
        factorization.insert ( factorization.end(), std::pair<FactorizedPolynomial<P>, size_t>( _fpolyA, exponentA ) );
        factorization.insert ( factorization.end(), std::pair<FactorizedPolynomial<P>, size_t>( _fpolyB, exponentB ) );
        assertFactorization();
        rehash();
    }

    template<typename P>
    Factorization<P> gcd( const PolynomialFactorizationPair<P>& _pfPairA, const PolynomialFactorizationPair<P>& _pfPairB, Factorization<P>& _restA, Factorization<P>& _restB, bool& _pfPairARefined, bool& _pfPairBRefined )
    {
        if( &_pfPairA == &_pfPairB )
            return _pfPairA.getFactorization();

        std::lock_guard<std::recursive_mutex> lockA( _pfPairA.mMutex );
        std::lock_guard<std::recursive_mutex> lockB( _pfPairB.mMutex );

        Factorization<P> result;
        _restA.clear();
        _restB.clear();
        Factorization<P> factorizationA = _pfPairA.getFactorization();
        Factorization<P> factorizationB = _pfPairB.getFactorization();

        for ( auto factorA = factorizationA.begin(); factorA != factorizationA.end(); factorA++ )
        {
            for ( auto factorB = factorizationB.begin(); factorB != factorizationB.end(); factorB++ )
            {
                if( factorA->first == factorB->first )
                {
                    //Common factor found
                    size_t exponent = factorA->second < factorB->second ? factorA->second : factorB->second;
                    result.insert( result.end(), std::pair<FactorizedPolynomial<P>, size_t>( factorA->first, exponent ) );
                }
                else
                {
                    //TODO (matthias) irreducible?
                    //Compute GCD of factors
                    assert( factorA->first.content().mpPolynomial != nullptr );
                    assert( factorB->first.content().mpPolynomial != nullptr );
                    P polA = *factorA->first.content().mpPolynomial;
                    P polB = *factorB->first.content().mpPolynomial;
                    P polGCD( carl::gcd( polA, polB ) );
                    Cache<PolynomialFactorizationPair<P>>& cache = factorA->first.mrCache;
                    FactorizedPolynomial<P> gcdResult( polGCD, cache );

                    //New common factor
                    if ( !gcdResult.isOne() )
                    {
                        //Compute remainders
                        P remainA, remainB;
                        bool correct = polA.divideBy( polGCD, remainA );
                        assert( correct );
                        correct = polB.divideBy( polGCD, remainB );
                        assert( correct );
                        size_t exponentA = factorA->second;
                        size_t exponentB = factorB->second;
                        size_t exponentCommon = exponentA < exponentB ? exponentA : exponentB;
                        result.insert( result.end(), std::pair<FactorizedPolynomial<P>, size_t>( gcdResult,  exponentCommon ) );

                        if (remainA != 1)
                        {
                            //Set new factorization
                            FactorizedPolynomial<P> polRemainA( remainA, cache );
                            factorA->first.content().setNewFactors( gcdResult, exponentA, polRemainA, exponentA );
                            _pfPairARefined = true;

                            //Add remaining factorization
                            if (exponentA > exponentCommon)
                                _restA.insert( _restA.end(), std::pair<FactorizedPolynomial<P>, size_t>( gcdResult, exponentA-exponentCommon ) );
                            _restA.insert( _restA.end(), std::pair<FactorizedPolynomial<P>, size_t>( polRemainA, exponentA) );
                        }
                        if (remainB != 1)
                        {
                            //Set new factorization
                            FactorizedPolynomial<P> polRemainB( remainB, cache );
                            factorB->first.content().setNewFactors( gcdResult, exponentB, polRemainB, exponentB );
                            _pfPairBRefined = true;

                            //Add remaining factorization
                            if (exponentB > exponentCommon)
                                _restB.insert( _restB.end(), std::pair<FactorizedPolynomial<P>, size_t>( gcdResult, exponentB-exponentCommon ) );
                            _restB.insert( _restB.end(), std::pair<FactorizedPolynomial<P>, size_t>( polRemainB, exponentB) );
                        }
                    }
                }
            }
        }

        // Check correctness
        _pfPairA.assertFactorization();
        _pfPairB.assertFactorization();
        assert( computePolynomial( result ) * computePolynomial( _restA ) == *_pfPairA.mpPolynomial);
        assert( computePolynomial( result ) * computePolynomial( _restB ) == *_pfPairB.mpPolynomial);

        return result;
    }
    
    template <typename P>
    std::ostream& operator<<(std::ostream& _out, const PolynomialFactorizationPair<P>& _pfPair)
    {
        if( _pfPair.getFactorization().size() == 1 && _pfPair.getFactorization().begin()->second )
        {
            assert( _pfPair.getFactorization().begin()->second == 1 );
            assert( _pfPair.mpPolynomial != nullptr );
            _out << *_pfPair.mpPolynomial;
        }
        else
        {   
            _out << _pfPair.getFactorization();
        }
        return _out;
    }
    
} // namespace carl