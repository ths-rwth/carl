/* 
 * File:   FactorizedPolynomialCache.tpp
 * Author: Florian Corzilius
 *
 * Created on September 4, 2014, 11:24 AM
 */

#pragma once

#include "FactorizedPolynomialCache.h"


namespace carl
{   
    template<typename P>
    FactorizedPolynomialCache<P>::FactorizedPolynomialCache( size_t _maxCacheSize, double _cacheReductionAmount ):
        mMaxCacheSize( _maxCacheSize ),
        mCacheReductionAmount( _cacheReductionAmount ),
        mMaxActivity( 0.0 ),
        mActivityIncrement( 1.0 ),
        mCache(),
        mCacheRefs(),
        mUnusedPositionsInCacheRefs()
    {
        mCache.reserve( _maxCacheSize );
        mCacheRefs.reserve( _maxCacheSize );
        mCacheRefs.push_back( mCache.end() ); // reserve the first entry with index 0 as default
    }
    
    template<typename P>
    typename FactorizedPolynomialCache<P>::Ref FactorizedPolynomialCache<P>::cache( PolynomialFactorizationPair<P>* _toCache )
    {
        auto ret = mCache.insert( std::make_pair( _toCache, CacheInfo( mMaxActivity ) ) );
        if( !ret.second )
        {
            ++ret.first->second.usageCount;
            if( !ret.first->first->hasExpandedRepresentation() && _toCache->hasExpandedRepresentation() )
            {
                //ret.first->first->setPolynomial( _toCache->pPolynomial() ); // TODO: remove this from cache and add it after update
            }
            // TODO: check if the given factorization can be used to refine the factorization of the cached polynomial 
            // TODO: remove this from cache and add it after update
            delete _toCache;
        }
        else
        {
            if( mUnusedPositionsInCacheRefs.empty() )
            {
                ret.first->second.refStoragePos = mCacheRefs.size();
                mCacheRefs.push_back( ret.first );
            }
            else
            {
                mCacheRefs[mUnusedPositionsInCacheRefs.top()] = ret.first;
                ret.first->second.refStoragePos = mUnusedPositionsInCacheRefs.top();
                mUnusedPositionsInCacheRefs.pop();
            }
        }
        return ret.first->second.refStoragePos;
    }
    
    template<typename P>
    void FactorizedPolynomialCache<P>::reg( typename FactorizedPolynomialCache<P>::Ref _refStoragePos )
    {
        assert( _refStoragePos < mCacheRefs.size() );
        typename CacheType::iterator cacheRef = mCacheRefs[_refStoragePos];
        assert( cacheRef != mCache.end() );
        ++cacheRef->second.usageCount;
    }
    
    template<typename P>
    void FactorizedPolynomialCache<P>::dereg( typename FactorizedPolynomialCache<P>::Ref _refStoragePos )
    {
        assert( _refStoragePos < mCacheRefs.size() );
        typename CacheType::iterator cacheRef = mCacheRefs[_refStoragePos];
        assert( cacheRef != mCache.end() );
        assert( cacheRef->second.usageCount > 0 );
        --cacheRef->second.usageCount;
    }
    
    template<typename P>
    void FactorizedPolynomialCache<P>::clean()
    {
        
    }
    
    template<typename P>
    void FactorizedPolynomialCache<P>::updateActivity( typename FactorizedPolynomialCache<P>::Ref _refStoragePos )
    {
        
    }
    
} // namespace carl