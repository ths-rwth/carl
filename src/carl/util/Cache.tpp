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
        mCacheReductionAmount( _cacheReductionAmount ),
        mMaxActivity( 0.0 ),
        mActivityIncrement( 1.0 ),
        mDecay( _decay ),
        mActivityTreshold( 1e100 ),
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
        auto ret = mCache.insert( std::make_pair( _toCache, Info( mMaxActivity ) ) );
        if( !ret.second )
        {
            if( ret.first->second.usageCount == 0 )
                --mNumOfUnusedEntries;
            ++ret.first->second.usageCount;
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
    
    template<typename T>
    void Cache<T>::reg( Ref _refStoragePos )
    {
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
        assert( _refStoragePos < mCacheRefs.size() );
        typename Container::iterator cacheRef = mCacheRefs[_refStoragePos];
        assert( cacheRef != mCache.end() );
        assert( cacheRef->second.usageCount > 0 );
        --cacheRef->second.usageCount;
        if( cacheRef->second.usageCount == 0 )
            ++mNumOfUnusedEntries;
    }
    
    template<typename T>
    template<typename ArgType>
    void Cache<T>::applyAndRehash( Ref _refStoragePos, void (*_f)( T&, ArgType ), ArgType _arg )
    {
        assert( _refStoragePos < mCacheRefs.size() );
        typename Container::iterator cacheRef = mCacheRefs[_refStoragePos];
        assert( cacheRef != mCache.end() );
        std::pair<T*,Info> element = *cacheRef;
        mCache.erase( cacheRef );
        (*_f)( *(element->first), _arg );
        auto ret = mCache.insert( element );
        assert( ret.second );
        mCacheRefs[_refStoragePos] = ret.first;
    }
    
    template<typename T>
    void Cache<T>::clean()
    {
        if( mNumOfUnusedEntries < (mCache.size()*mCacheReductionAmount) )
        {
            // There are less entries we can delete than we want to delete: just delete them all
            for( auto iter = mCache.begin(); iter != mCache.end(); )
            {
                if( iter->second.usageCounter == 0 )
                    iter = mCache.erase( iter );
                else
                    ++iter;
            }
        }
        else
        {
            std::vector<typename Container::iterator> noUsageEntries;
            double limit = 0.0;
            for( auto iter = mCache.begin(); iter != mCache.end(); ++iter )
            {
                if( iter->second.usageCounter == 0 )
                    noUsageEntries.push_back( iter );
                iter->second.activity *= mActivityDecrementFactor;
            }
        }
    }
    
    template<typename T>
    void Cache<T>::decayActivity()
    {
        mActivityIncrement *= (1 / mDecay);
    }
    
    template<typename T>
    void Cache<T>::updateActivity( Ref _refStoragePos )
    {
        assert( _refStoragePos < mCacheRefs.size() );
        typename Container::iterator cacheRef = mCacheRefs[_refStoragePos];
        assert( cacheRef != mCache.end() );
        if( (cacheRef->second.activity += mActivityIncrement) > mActivityTreshold )
        {
            // Rescale:
            for( auto iter = mCache.begin(); iter != mCache.end(); ++iter )
                iter->second.activity *= mActivityDecrementFactor;
            mActivityIncrement *= mActivityDecrementFactor;
        }
    }
    
    template<typename T>
    void Cache<T>::print()
    {
        std::cout << "Cache contains:" << std::endl;
        for( auto iter = mCache.begin(); iter != mCache.end(); ++iter )
        {
            if( iter->first == nullptr )
                std::cout << "   NULL" << std::endl;
            else
                std::cout << "   " << *iter->first << std::endl;
            std::cout << "                       usage count: " << iter->second.usageCount << std::endl;
            std::cout << "        reference storage position: " << iter->second.refStoragePos << std::endl;
            std::cout << "                          activity: " << iter->second.activity << std::endl;
        }
    }
    
} // namespace carl