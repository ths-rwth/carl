/* 
 * File:   Cache.tpp
 * Author: Florian Corzilius
 *
 * Created on September 4, 2014, 11:24 AM
 */

#pragma once

#include "Cache.h"


namespace carl
{   
    template<typename T>
    Cache<T>::Cache( size_t _maxCacheSize, double _cacheReductionAmount, double _decay ):
        mMaxCacheSize( _maxCacheSize ),
        mNumOfUnusedEntries( 0 ),
        mCacheReductionAmount( _cacheReductionAmount ), // TODO: use it, but without the effort of quick select
        mMaxActivity( 0.0 ),
        mActivityIncrement( 1.0 ),
        mDecay( _decay ),
        mActivityThreshold( 1e100 ),
        mActivityDecrementFactor( 1e-100 ),
        mCache(),
        mCacheRefs(),
        mUnusedPositionsInCacheRefs()
    {
        assert( _decay >= 0.9 && _decay <= 1.0 );
        mCache.reserve( _maxCacheSize ); // TODO: maybe no reservation of memory and let it grow dynamically
        mCacheRefs.reserve( _maxCacheSize ); // TODO: maybe no reservation of memory and let it grow dynamically
        // reserve the first entry with index 0 as default
        mCacheRefs.push_back( mCache.end() ); 
    }
    
    template<typename T>
    typename Cache<T>::Ref Cache<T>::cache( T* _toCache, bool (*_canBeUpdated)( const T&, const T& ), void (*_update)( T&, T& ) )
    {
        std::lock_guard<std::mutex> lock( mMutex );
        auto ret = mCache.insert( std::make_pair( _toCache, Info( mMaxActivity ) ) );
        
        if( !ret.second ) // There is already an equal object in the cache.
        {
            if( ret.first->second.usageCount == 0 )
                --mNumOfUnusedEntries;
            ++ret.first->second.usageCount;
            // Try to update the entry in the cache by the information in the given object.
            if( (*_canBeUpdated)( *(ret.first->first), *_toCache ) )
            {
                std::pair<T*,Info> element = *ret.first;
                mCache.erase( ret.first );
                (*_update)( *element.first, *_toCache );
                auto ret = mCache.insert( element );
                assert( ret.second );
                mCacheRefs[element.second.refStoragePos] = ret.first;
            }
            delete _toCache;
        }
        else // Create a new entry in the cache.
        {
            if( mUnusedPositionsInCacheRefs.empty() ) // Try to take the reference from the stack of old ones.
            {
                ret.first->second.refStoragePos = mCacheRefs.size();
                mCacheRefs.push_back( ret.first );
            }
            else // Get a brand new reference.
            {
                mCacheRefs[mUnusedPositionsInCacheRefs.top()] = ret.first;
                ret.first->second.refStoragePos = mUnusedPositionsInCacheRefs.top();
                mUnusedPositionsInCacheRefs.pop();
            }
            if( mCache.size() >= mMaxCacheSize ) // Clean, if the number of elements in the cache exceeds the threshold.
            {
                clean();
            }
        }
        return ret.first->second.refStoragePos;
    }
    
    template<typename T>
    void Cache<T>::reg( Ref _refStoragePos )
    {
        std::lock_guard<std::mutex> lock( mMutex );
        assert( _refStoragePos < mCacheRefs.size() );
        typename Container::iterator cacheRef = mCacheRefs[_refStoragePos];
        assert( cacheRef != mCache.end() );
        if( cacheRef->second.usageCount == 0 )
            --mNumOfUnusedEntries;
        ++cacheRef->second.usageCount;
    }
    
    template<typename T>
    void Cache<T>::dereg( Ref _refStoragePos )
    {
        std::lock_guard<std::mutex> lock( mMutex );
        assert( _refStoragePos < mCacheRefs.size() );
        typename Container::iterator cacheRef = mCacheRefs[_refStoragePos];
        assert( cacheRef != mCache.end() );
        assert( cacheRef->second.usageCount > 0 );
        --cacheRef->second.usageCount;
        if( cacheRef->second.usageCount == 0 ) // no more usage
        {
            ++mNumOfUnusedEntries;
            // If the cache contains more used elements than the maximum desired cache size, remove this entry directly.
            if( mCache.size() - mNumOfUnusedEntries >= mMaxCacheSize )
            {
                erase( cacheRef );
            }   
        }
    }
    
    template<typename T>
    void Cache<T>::rehash( Ref _refStoragePos )
    {
        std::lock_guard<std::mutex> lock( mMutex );
        assert( _refStoragePos < mCacheRefs.size() );
        typename Container::iterator cacheRef = mCacheRefs[_refStoragePos];
        assert( cacheRef != mCache.end() );
        std::pair<T*,Info> element = *cacheRef;
        mCache.erase( cacheRef );
        element.first->rehash();
        auto ret = mCache.insert( element );
        assert( ret.second );
        mCacheRefs[_refStoragePos] = ret.first;
    }
    
    template<typename T>
    void Cache<T>::clean()
    {
        if( mNumOfUnusedEntries < ((double) mCache.size() * mCacheReductionAmount) )
        {
            if( mNumOfUnusedEntries > 0 )
            {
                // There are less entries we can delete than we want to delete: just delete them all
                for( auto iter = mCache.begin(); iter != mCache.end(); )
                {
                    if( iter->second.usageCount == 0 )
                        iter = erase( iter );
                    else
                        ++iter;
                }
            }
            assert( mNumOfUnusedEntries == 0 );
        }
        else
        {
            std::vector<typename Container::iterator> noUsageEntries;
            // Calculate the expected median of the activities of all entries in the cache with no usage.
            double limit = 0.0;
            for( auto iter = mCache.begin(); iter != mCache.end(); ++iter )
            {
                if( iter->second.usageCount == 0 )
                {
                    noUsageEntries.push_back( iter );
                    limit += iter->second.activity;
                }
            }
            limit = limit / (double) noUsageEntries.size();
            // Remove all entries in the cache with no usage, which have an activity below the calculated median.
            for( auto iterIter = noUsageEntries.begin(); iterIter != noUsageEntries.end(); ++iterIter )
            {
                if( (*iterIter)->second.activity <= limit )
                {
                    erase( *iterIter );
                }
            }
        }
    }
    
    template<typename T>
    void Cache<T>::decayActivity()
    {
        std::lock_guard<std::mutex> lock( mMutex );
        mActivityIncrement *= (1 / mDecay);
    }
    
    template<typename T>
    void Cache<T>::strengthenActivity( Ref _refStoragePos )
    {
        assert( _refStoragePos < mCacheRefs.size() );
        typename Container::iterator cacheRef = mCacheRefs[_refStoragePos];
        assert( cacheRef != mCache.end() );
        // update the activity of the cache entry at the given position
        if( (cacheRef->second.activity += mActivityIncrement) > mActivityThreshold )
        {
            std::lock_guard<std::mutex> lock( mMutex );
            // rescale if the threshold for the maximum activity has been exceeded
            for( auto iter = mCache.begin(); iter != mCache.end(); ++iter )
                iter->second.activity *= mActivityDecrementFactor;
            mActivityIncrement *= mActivityDecrementFactor;
            mMaxActivity *= mActivityDecrementFactor;
        }
        // update the maximum activity
        std::lock_guard<std::mutex> lock( mMutex );
        if( mMaxActivity < cacheRef->second.activity )
            mMaxActivity = cacheRef->second.activity;
    }
    
    template<typename T>
    void Cache<T>::print()
    {
        std::cout << "General cache information:" << std::endl;
        std::cout << "   desired maximum cache size                                 : "  << mMaxCacheSize << std::endl;
        std::cout << "   number of unused entries                                   : "  << mNumOfUnusedEntries << std::endl;
        std::cout << "   desired reduction amount when cleaning the cache (not used): "  << mCacheReductionAmount << std::endl;
        std::cout << "   maximum of all activities                                  : "  << mMaxActivity << std::endl;
        std::cout << "   the current value of the activity increment                : "  << mActivityIncrement << std::endl;
        std::cout << "   decay factor for the given activities                      : "  << mDecay << std::endl;
        std::cout << "   upper bound of the activities                              : "  << mActivityThreshold << std::endl;
        std::cout << "   scaling factor of the activities                           : "  << mActivityDecrementFactor << std::endl;
        std::cout << "   current size of the cache                                  : "  << mCache.size() << std::endl;
        std::cout << "   number of yet involved references                          : "  << mCacheRefs.size() << std::endl;
        std::cout << "   number of currently freed references                       : "  << mUnusedPositionsInCacheRefs.size() << std::endl;
        std::cout << "Cache contains:" << std::endl;
        for( auto iter = mCache.begin(); iter != mCache.end(); ++iter )
        {
            assert( iter->first != nullptr );
            std::cout << "   " << *iter->first << std::endl;
            std::cout << "                       usage count: " << iter->second.usageCount << std::endl;
            std::cout << "        reference storage position: " << iter->second.refStoragePos << std::endl;
            std::cout << "                          activity: " << iter->second.activity << std::endl;
        }
    }
    
} // namespace carl