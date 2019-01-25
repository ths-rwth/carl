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
    const typename Cache<T>::Ref Cache<T>::NO_REF = 0;

    template<typename T>
    Cache<T>::Cache( size_t _maxCacheSize, double _cacheReductionAmount, double _decay ):
        mMaxCacheSize( _maxCacheSize ),
        mCacheReductionAmount( _cacheReductionAmount ), // TODO: use it, but without the effort of quick select
        mDecay( _decay ),
        mCache(),
        mCacheRefs(),
        mUnusedPositionsInCacheRefs()
    {
        assert( _decay >= 0.9 && _decay <= 1.0 );
        mCache.reserve( _maxCacheSize ); // TODO: maybe no reservation of memory and let it grow dynamically
        mCacheRefs.reserve( _maxCacheSize ); // TODO: maybe no reservation of memory and let it grow dynamically
        // reserve the first entry with index 0 as default
        mCacheRefs.push_back( nullptr ); 
    }
    
    template<typename T>
    Cache<T>::~Cache()
    {
        mCacheRefs.clear();
        while( !mCache.empty() )
        {
            TypeInfoPair<T,Info>* tip = *mCache.begin();
            mCache.erase( mCache.begin() );
            T* t = tip->first;
            delete tip;
            delete t;
        }
    }
    
    template<typename T>
    std::pair<typename Cache<T>::Ref,bool> Cache<T>::cache( T* _toCache, bool (*_canBeUpdated)( const T&, const T& ), void (*_update)( const T&, const T& ) )
    {
        std::lock_guard<std::recursive_mutex> lock( mMutex );
        if( mCache.size() >= mMaxCacheSize ) // Clean, if the number of elements in the cache exceeds the threshold.
        {
            clean();
        }
        auto newElement = new TypeInfoPair<T,Info>(_toCache, Info(mMaxActivity));
        auto ret = mCache.insert( newElement );
        
        if( !ret.second ) // There is already an equal object in the cache.
        {
            // Try to update the entry in the cache by the information in the given object.
            if( (*_canBeUpdated)( *((*ret.first)->first), *_toCache ) )
            {
                TypeInfoPair<T,Info>* element = *ret.first;
                (*_update)( *element->first, *_toCache );
                mCache.erase( ret.first );
                element->first->rehash();
                auto retB = mCache.insert( element );
                assert( retB.second );
                for( const Ref& ref : element->second.refStoragePositions )
                    mCacheRefs[ref] = *retB.first;
                delete newElement;
                Info& info = (*retB.first)->second;
                assert( info.refStoragePositions.size() > 0);
                assert( info.refStoragePositions.front() > 0 );
                info.refStoragePositions.insert( info.refStoragePositions.end(), element->second.refStoragePositions.begin(), element->second.refStoragePositions.end() );
                return std::make_pair( info.refStoragePositions.front(), false );
            }
            else
                delete newElement;
        }
        else // Create a new entry in the cache.
        {
            if( mUnusedPositionsInCacheRefs.empty() ) // Get a brand new reference.
            {
                assert( mCacheRefs.size() > 0);
                (*ret.first)->second.refStoragePositions.push_back( mCacheRefs.size() );
                assert( !hasDuplicates( (*ret.first)->second.refStoragePositions ) );
                mCacheRefs.push_back( newElement );
            }
            else // Try to take the reference from the stack of old ones.
            {
                mCacheRefs[mUnusedPositionsInCacheRefs.top()] = newElement;
                assert( mUnusedPositionsInCacheRefs.top() > 0);
                newElement->second.refStoragePositions.push_back( mUnusedPositionsInCacheRefs.top() );
                assert( !hasDuplicates( newElement->second.refStoragePositions ) );
                mUnusedPositionsInCacheRefs.pop();
            }
            assert( mNumOfUnusedEntries < std::numeric_limits<ContentType>::max() );
            ++mNumOfUnusedEntries;
        }
        assert( (*ret.first)->second.refStoragePositions.size() > 0);
        assert( (*ret.first)->second.refStoragePositions.front() > 0 );
        return std::make_pair( (*ret.first)->second.refStoragePositions.front(), ret.second );
    }
    
    template<typename T>
    void Cache<T>::reg( Ref _refStoragePos )
    {
        std::lock_guard<std::recursive_mutex> lock( mMutex );
        assert( _refStoragePos < mCacheRefs.size() );
        TypeInfoPair<T,Info>* cacheRef = mCacheRefs[_refStoragePos];
        assert( cacheRef != nullptr );
        if( cacheRef->second.usageCount == 0 )
        {
            assert( mNumOfUnusedEntries > 0 );
            --mNumOfUnusedEntries;
        }
        assert( cacheRef->second.usageCount < std::numeric_limits<ContentType>::max() );
        ++cacheRef->second.usageCount;
    }
    
    template<typename T>
    void Cache<T>::dereg( Ref _refStoragePos )
    {
        std::lock_guard<std::recursive_mutex> lock( mMutex );
        assert( _refStoragePos < mCacheRefs.size() );
        TypeInfoPair<T,Info>* cacheRef = mCacheRefs[_refStoragePos];
        assert( cacheRef != nullptr );
        assert( cacheRef->second.usageCount > 0 );
        --cacheRef->second.usageCount;
        if( cacheRef->second.usageCount == 0 ) // no more usage
        {
            assert( mNumOfUnusedEntries < std::numeric_limits<ContentType>::max() );
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
        std::lock_guard<std::recursive_mutex> lock( mMutex );
        assert( _refStoragePos < mCacheRefs.size() );
        TypeInfoPair<T,Info>* cacheRef = mCacheRefs[_refStoragePos];
        assert( cacheRef != nullptr );
        mCache.erase( cacheRef );
        cacheRef->first->rehash();
        const Info& infoB = cacheRef->second;
        auto ret = mCache.insert( cacheRef );
        if( !ret.second )
        {
            Info& info = (*ret.first)->second;
            if( infoB.usageCount == 0 )
            {
                assert( mNumOfUnusedEntries > 0 );
                --mNumOfUnusedEntries;
            }
            else if( info.usageCount == 0 )
            {
                assert( mNumOfUnusedEntries > 0 );
                --mNumOfUnusedEntries;
            }
            assert( info.usageCount + infoB.usageCount >= info.usageCount );
            info.usageCount += infoB.usageCount;
            info.refStoragePositions.insert( info.refStoragePositions.end(), infoB.refStoragePositions.begin(), infoB.refStoragePositions.end() );
            assert( !hasDuplicates( info.refStoragePositions ) );
            assert( std::find( infoB.refStoragePositions.begin(), infoB.refStoragePositions.end(), _refStoragePos ) != infoB.refStoragePositions.end() );
            for( const Ref& ref : infoB.refStoragePositions )
            {
                assert( mCacheRefs[ref] != *(ret.first) );
                mCacheRefs[ref] = *(ret.first);
            }
            delete cacheRef->first;
            delete cacheRef;
        }
    }
    
    template<typename T>
    void Cache<T>::clean()
    {
        CARL_LOG_TRACE( "carl.util.cache", "Cleaning cache..." );
        if( double(mNumOfUnusedEntries) < (double(mCache.size()) * mCacheReductionAmount) )
        {
            if( mNumOfUnusedEntries > 0 )
            {
                // There are less entries we can delete than we want to delete: just delete them all
                for( auto iter = mCache.begin(); iter != mCache.end(); )
                {
                    if( (*iter)->second.usageCount == 0 )
                    {
                        iter = erase( iter );
                    }
                    else
                    {
                        ++iter;
                    }
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
                if( (*iter)->second.usageCount == 0 )
                {
                    noUsageEntries.push_back( iter );
                    limit += (*iter)->second.activity;
                }
            }
            limit = limit / double(noUsageEntries.size());
            // Remove all entries in the cache with no usage, which have an activity below the calculated median.
            for( auto iter = noUsageEntries.begin(); iter != noUsageEntries.end(); ++iter )
            {
                if( (**iter)->second.activity <= limit )
                {
                    erase( *iter );
                }
            }
        }
    }
    
    template<typename T>
    void Cache<T>::decayActivity()
    {
        std::lock_guard<std::recursive_mutex> lock( mMutex );
        mActivityIncrement *= (1 / mDecay);
    }
    
    template<typename T>
    void Cache<T>::strengthenActivity( Ref _refStoragePos )
    {
        assert( _refStoragePos < mCacheRefs.size() );
        TypeInfoPair<T,Info>* cacheRef = mCacheRefs[_refStoragePos];
        assert( cacheRef != nullptr );
        // update the activity of the cache entry at the given position
        if( (cacheRef->second.activity += mActivityIncrement) > mActivityThreshold )
        {
            std::lock_guard<std::recursive_mutex> lock( mMutex );
            // rescale if the threshold for the maximum activity has been exceeded
            for( auto iter = mCache.begin(); iter != mCache.end(); ++iter )
                (*iter)->second.activity *= mActivityDecrementFactor;
            mActivityIncrement *= mActivityDecrementFactor;
            mMaxActivity *= mActivityDecrementFactor;
        }
        // update the maximum activity
        std::lock_guard<std::recursive_mutex> lock( mMutex );
        if( mMaxActivity < cacheRef->second.activity )
            mMaxActivity = cacheRef->second.activity;
    }
    
    template<typename T>
    void Cache<T>::print( std::ostream& _out ) const
    {
        _out << "General cache information:" << std::endl;
        _out << "   desired maximum cache size                                 : "  << mMaxCacheSize << std::endl;
        _out << "   number of unused entries                                   : "  << mNumOfUnusedEntries << std::endl;
        _out << "   desired reduction amount when cleaning the cache (not used): "  << mCacheReductionAmount << std::endl;
        _out << "   maximum of all activities                                  : "  << mMaxActivity << std::endl;
        _out << "   the current value of the activity increment                : "  << mActivityIncrement << std::endl;
        _out << "   decay factor for the given activities                      : "  << mDecay << std::endl;
        _out << "   upper bound of the activities                              : "  << mActivityThreshold << std::endl;
        _out << "   scaling factor of the activities                           : "  << mActivityDecrementFactor << std::endl;
        _out << "   current size of the cache                                  : "  << mCache.size() << std::endl;
        _out << "   number of yet involved references                          : "  << mCacheRefs.size() << std::endl;
        _out << "   number of currently freed references                       : "  << mUnusedPositionsInCacheRefs.size() << std::endl;
        _out << "Cache contains:" << std::endl;
        for( auto iter = mCache.begin(); iter != mCache.end(); ++iter )
        {
            assert( (*iter)->first != nullptr );
            _out << "   " << *(*iter)->first << std::endl;
            _out << "                       usage count: " << (*iter)->second.usageCount << std::endl;
            _out << "        reference storage positions:";
            for( Ref ref : (*iter)->second.refStoragePositions )
                _out << "  " << ref;
            _out << "                          activity: " << (*iter)->second.activity << std::endl;
        }
    }
    
} // namespace carl
