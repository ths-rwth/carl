/* 
 * File:   FactorizedPolynomialCache.h
 * Author: Florian Corzilius
 *
 * Created on September 4, 2014, 11:24 AM
 */

#pragma once

#include <vector>
#include <stack>
#include <map>
#include <unordered_map>
#include <iostream>
#include <cassert>

namespace carl
{
    template<typename P>
    class FactorizedPolynomialCache;
    
    template<typename P>
    struct pointerPLess
    {
        bool operator()( const P* _argA, const P* _argB ) const
        {
            return (*_argA)<(*_argB);
        }
    };
    
    template<typename P>
    using Factorization = std::map<P*, size_t, pointerPLess<P>>;
    
    template<typename P>
    class PolynomialFactorizationPair
    {
        template<typename P1>
        friend class FactorizedPolynomialCache;
        
    private:
        // Members
        P*               mpPolynomial;
        Factorization<P> mFactorization;
        
    public:
        PolynomialFactorizationPair(); // no implementation
        PolynomialFactorizationPair( P* _polynomial, Factorization<P>&& _factorization );
        PolynomialFactorizationPair( P* _polynomial );
        PolynomialFactorizationPair( Factorization<P>&& _factorization );
        PolynomialFactorizationPair( const PolynomialFactorizationPair& ); // no implementation
        
        bool hasExpandedRepresentation() const
        {
            return mpPolynomial != nullptr;
        }
        
        const Factorization<P>& factorization() const
        {
            return mFactorization;
        }
        
        template<typename P2>
        friend bool operator==(const PolynomialFactorizationPair<P2>& _polyFactA, const PolynomialFactorizationPair<P2>& _polyFactB);
        
    private:
        const P* pPolynomial() const
        {
            return mpPolynomial;
        }
        
        void setPolynomial( P* _polynomial )
        {
            assert( _polynomial != nullptr );
            if( _polynomial != mpPolynomial )
            {
                assert( mpPolynomial != nullptr );
                mpPolynomial = _polynomial;
            }
        }
        
    };
} // namespace carl


/**
 */
namespace std
{
    template<typename P>
    struct hash<carl::PolynomialFactorizationPair<P>>
    {
        size_t operator()( const carl::PolynomialFactorizationPair<P>& _factPoly ) const 
        {
            // TODO: call getHash from the tuple and include the case, that the polynomial representation exists and use it for hashing instead of its factorization
            size_t result = 0;
			std::hash<P> h;
            for( auto polyExpPair = _factPoly.factorization().begin(); polyExpPair != _factPoly.factorization().end(); ++polyExpPair )
            {
                result = (result << 5) | (result >> (sizeof(size_t)*8 - 5));
                result ^= h( *polyExpPair->first );
                result ^= polyExpPair->second;
            }
            return result;
        }
    };
} // namespace std

namespace carl
{
    template<typename P>
    struct pointerPFPEqual
    {
        bool operator()( const PolynomialFactorizationPair<P>* _argA, const PolynomialFactorizationPair<P>* _argB ) const
        {
            return (*_argA)==(*_argB);
        }
    };
    
    template<typename P> 
    struct pointerPFPHash
    {
        size_t operator()( const PolynomialFactorizationPair<P>* _arg ) const
        {
            return std::hash<PolynomialFactorizationPair<P>>()( *_arg );
        }
    };
    
    template<typename P>
    class FactorizedPolynomial;
    
    template<typename P>
    class FactorizedPolynomialCache
    {
        
    public:
        typedef size_t Ref;
        
        struct CacheInfo
        {
            size_t usageCount;
            Ref refStoragePos;
            double activity;

            CacheInfo( double _activity ):
                usageCount( 1 ),
                refStoragePos( 0 ),
                activity( _activity )
            {}
        };
        
        typedef std::unordered_map<const PolynomialFactorizationPair<P>*, CacheInfo, pointerPFPHash<P>, pointerPFPEqual<P>> CacheType;
        
    private:
        // Members
        size_t    mMaxCacheSize;
        double    mCacheReductionAmount;
        double    mMaxActivity;
        double    mActivityIncrement;
        CacheType mCache;
        std::vector<typename CacheType::iterator> mCacheRefs;
        std::stack<Ref> mUnusedPositionsInCacheRefs;
        
    public:
            
        FactorizedPolynomialCache( size_t _maxCacheSize = 10000, double _cacheReductionAmount = 0.2 );
        FactorizedPolynomialCache( const FactorizedPolynomialCache& ); // no implementation
        
        Ref createFactorizedPolynomial( P* _polynomial );
        Ref createFactorizedPolynomial( P* _polynomial, Factorization<P>&& _factorization );
        Ref createFactorizedPolynomial( Factorization<P>&& _factorization );
        Ref cache( PolynomialFactorizationPair<P>* _toCache );
        void remove( Ref _refStoragePos );
        void clean();
        void updateActivity( Ref _refStoragePos );
        
        const PolynomialFactorizationPair<P>& get( Ref _refStoragePos ) const
        {
            assert( _refStoragePos < mCacheRefs.size() );
            assert( mCacheRefs[_refStoragePos] != mCache.end() );
            return *mCacheRefs[_refStoragePos]->first;
        }
        
    };
    
} // namespace carl


#include "FactorizedPolynomialCache.tpp"