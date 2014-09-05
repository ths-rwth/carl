/* 
 * File:   FactorizedPolynomialCache.h
 * Author: Florian Corzilius
 *
 * Created on September 4, 2014, 11:24 AM
 */

#pragma once

#include <vector>
#include <stack>
#include <unordered_map>
#include <cassert>
#include "PolynomialFactorizationPair.h"


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
    class FactorizedPolynomialCache
    {
        
    public:
        typedef size_t Ref;
        
        struct CacheInfo
        {
            size_t usageCount;
            Ref    refStoragePos;
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
        size_t                                    mMaxCacheSize;
        double                                    mCacheReductionAmount;
        double                                    mMaxActivity;
        double                                    mActivityIncrement;
        CacheType                                 mCache;
        std::vector<typename CacheType::iterator> mCacheRefs;
        std::stack<Ref>                           mUnusedPositionsInCacheRefs;
        
    public:
            
        FactorizedPolynomialCache( size_t _maxCacheSize = 10000, double _cacheReductionAmount = 0.2 );
        FactorizedPolynomialCache( const FactorizedPolynomialCache& ); // no implementation
        
        Ref cache( PolynomialFactorizationPair<P>* _toCache );
        void reg( Ref _refStoragePos );
        void dereg( Ref _refStoragePos );
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