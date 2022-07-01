/* 
 * File:   Cache.h
 * Author: Florian Corzilius
 *
 * Created on September 4, 2014, 11:24 AM
 */

#pragma once

#include "../util/container_types.h"

#include <cassert>
#include <limits>
#include <mutex>
#include <stack>
#include <unordered_set>
#include <vector>

namespace carl {   
    template<typename T, class I>
    using TypeInfoPair = std::pair<T*,I>;
    
    template<typename T, class I>
    bool operator==(const TypeInfoPair<T,I>& _tipA, const TypeInfoPair<T,I>& _tipB) {
        return *_tipA.first == *_tipB.first;
    }
}

namespace std {
    template<typename T, class I>
    struct hash<carl::TypeInfoPair<T,I>> {
        std::size_t operator()(const carl::TypeInfoPair<T,I>& _tip) const {
            return _tip.first->hash();
        }
    };
}

namespace carl
{   
    template<typename T>
    bool returnFalse( const T& /*unused*/, const T& /*unused*/) { return false; }
    
    template<typename T>
    void doNothing( const T& /*unused*/, const T& /*unused*/) {}
   
    template<typename T>
    class Cache {
        
    public:
        // The type of the reference of an entry in the cache.
        using Ref = std::size_t;
        
        struct Info {
            /**
             * Store the number of usages of the entry in the cache for which this information hold by external objects.
             */
            std::size_t usageCount;
            
            /**
             * Stores the reference of the entry in the cache for which this information hold.
             */
            std::vector<Ref> refStoragePositions;
            
            /**
             * Stores the activity of the entry in the cache for which this information hold. The activity states how often the entry
             * is involved in computations in the recent past.
             */
            double activity;

            explicit Info( double _activity ):
                usageCount(0),
                refStoragePositions(),
                activity(_activity)
            {}
        };
        
        using Container = std::unordered_set<TypeInfoPair<T,Info>*, pointerHash<TypeInfoPair<T,Info>>, pointerEqual<TypeInfoPair<T,Info>>>;
        
    private:
        // Members
        
        /**
         * The threshold for the cache's size which should not be exceeded, except more of the cache entries are still in use.
         */
        std::size_t mMaxCacheSize;
        
        /**
         * The current number of entries in the cache, which are not used.
         */
        std::size_t mNumOfUnusedEntries = 0;
        
        /**
         * The percentage of the cache, which shall be removed at best, if the cache size exceeds the threshold. (NOT YET USED)
         */
        double mCacheReductionAmount;
        
        /**
         * The threshold for the maximum activity. In case it is exceeded, all activities are rescaled.
         */
        double mMaxActivity = 0.0;
        
        /**
         * The reciprocal of the factor to multiply an activity with in order to increase it. 
         * This member can increased by a user interface.
         */
        double mActivityIncrement = 1.0;
        
        /**
         * The decay (between 0.9 and 1.0) of the given increments on activities. 
         * It is applied by increasing the increment by multiplying this members reciprocal to it.
         */
        double mDecay;
        
        /**
         * The threshold limiting the maximum activity. If this threshold is exceeded, all activities are rescaled.
         */
        double mActivityThreshold = 1e100;
        
        /**
         * The factor multiplied to all activities in order to rescale (decrease) them.
         */
        double mActivityDecrementFactor = 1e-100;
        
        /**
         * A mutex for situation where any member is changed. TODO: Refine the locking-situations. 
         */
        std::recursive_mutex mMutex;
        
        /**
         *  The container storing all cached entries. It maps the objects to store to cache information, which cover a usage counter, 
         *  the position in mCacheRefs, being the entries reference, and the activity of this entry.
         */
        Container mCache;
        
        /**
         * Stores at the reference of an entry in the cache an iterator to this entry. 
         * This reference can be used to access the entry outside this class.
         */
        std::vector<TypeInfoPair<T,Info>*> mCacheRefs;
        /// A stack containing free references, which have been used before but freed now.
        std::stack<Ref> mUnusedPositionsInCacheRefs;
        
    public:

        static const Ref NO_REF;

        // The constructor.
        explicit Cache( size_t _maxCacheSize = 10000, double _cacheReductionAmount = 0.2, double _decay = 0.98 );
        Cache( const Cache& ) = delete; // no implementation
        Cache& operator=( const Cache& ) = delete; // no implementation

        ~Cache();
        
        /**
         * Caches the given object.
         * @param _toCache The object to cache.
         * @param _canBeUpdated A function, which determines whether, in the case an equal object has already been cached, the given object
         *                      can update the information in this already cached object.
         * @param _update A function which updates an object in the cache, which is equal to the given object, by the information in the given object.
         *                After this function has been applied, the corresponding entry in the cache will be reinserted in it after been rehashed.
         * @return The reference of the entry, which can be used outside this class to access the entry.
         */
        std::pair<Ref,bool> cache( T* _toCache, bool (*_canBeUpdated)( const T&, const T& ) = &returnFalse<T>, void (*_update)( const T&, const T& ) = &doNothing<T> );
        
        /**
         * Registers the entry to the given reference. It mainly increases the usage counter of this entry in the cache.
         * @param _refStoragePos The reference of the entry to register.
         */
        void reg( Ref _refStoragePos );
        
        /**
         * Deregisters the entry to the given reference. It mainly decreases the usage counter of this entry in the cache.
         * @param _refStoragePos The reference of the entry to deregister.
         */
        void dereg( Ref _refStoragePos );
        
        /**
         * Removes and reinserts the entry with the given reference, after its hash value is recalculated.
         * @param _refStoragePos The reference of the entry to apply the given function to.
         * @return The new reference.
         */
        void rehash( Ref _refStoragePos );
        
        /**
         * Decays all activities by increasing the activity increment.
         */
        void decayActivity();
        
        /**
         * Strenghtens the activity of the entry in the cache with the given reference, by increasing its activity.
         * @param _refStoragePos The reference of the entry in the cache to strengthen its activity.
         */
        void strengthenActivity( Ref _refStoragePos );
        
        /**
         * Prints all information stored in this cache to std::cout.
         * @param _out The stream to print on.
         */
        void print( std::ostream& _out = std::cout ) const;
        
        /**
         * @param _refStoragePos The reference of the entry to obtain the object from. 
         * @return The object in the entry with the given reference.
         */
        const T& get( Ref _refStoragePos ) const
        {
            assert( _refStoragePos < mCacheRefs.size() );
            assert( mCacheRefs[_refStoragePos] != nullptr );
            assert( mCacheRefs[_refStoragePos]->second.usageCount > 0 );
            return *mCacheRefs[_refStoragePos]->first;
        }
        
    private:
        
        /**
         * Removes a certain amount of unused entries in the cache.
         */
        void clean();
        
        /**
         * Removes the entry at the given position in the cache.
         * @param _toRemove The position to the entry to remove from the cache.
         * @return 
         */
        std::size_t erase( TypeInfoPair<T,Info>* _toRemove )
        {
            assert( checkNumOfUnusedEntries() );
            std::lock_guard<std::recursive_mutex> lock( mMutex );
            assert( _toRemove->second.usageCount == 0 );
            for( const Ref& ref : _toRemove->second.refStoragePositions )
            {
                mCacheRefs[ref] = nullptr;
                assert (ref > 0);
                mUnusedPositionsInCacheRefs.push( ref );
            }
            assert( mNumOfUnusedEntries > 0 );
            --mNumOfUnusedEntries;
            assert(_toRemove->second.usageCount == 0);
            auto result = mCache.erase( _toRemove );
            T* toDel = _toRemove->first;
            delete _toRemove;
            delete toDel;
            assert( checkNumOfUnusedEntries() );
            return result;
        }
        
        /**
         * Removes the entry at the given position in the cache.
         * @param _toRemove The position to the entry to remove from the cache.
         * @return An iterator to the entry in the cache right after the entry which has to be removed.
         */
        typename Container::iterator erase( typename Container::iterator _toRemove )
        {
            assert( checkNumOfUnusedEntries() );
            std::lock_guard<std::recursive_mutex> lock( mMutex );
            assert( (*_toRemove)->second.usageCount == 0 );
            for( const Ref& ref : (*_toRemove)->second.refStoragePositions )
            {
                mCacheRefs[ref] = nullptr;
                assert (ref > 0);
                mUnusedPositionsInCacheRefs.push( ref );
            }
            assert( mNumOfUnusedEntries > 0 );
            --mNumOfUnusedEntries;
            assert((*_toRemove)->second.usageCount == 0);
            T* toDel = (*_toRemove)->first;
            TypeInfoPair<T,Info>* toDelB = *_toRemove;
            auto result = mCache.erase( _toRemove );
            delete toDelB;
            delete toDel;
            assert( checkNumOfUnusedEntries() );
            return result;
        }
        
        bool hasDuplicates(const std::vector<Ref>& _vec) const
        {
            std::set<Ref> vecEntries;
            for (const Ref& r: _vec) {
                if (!vecEntries.insert(r).second) {
                    return true;
                }
            }
            return false;
        }
        
        bool checkNumOfUnusedEntries() const
        {
            std::size_t actualNumOfUnusedEntries = 0;
            for( auto iter = mCache.begin(); iter != mCache.end(); ++iter )
            {
                if( (*iter)->second.usageCount == 0 )
                {
                    ++actualNumOfUnusedEntries;
                }
            }
            return mNumOfUnusedEntries == actualNumOfUnusedEntries;
        }
        
        size_t sumOfAllUsageCounts() const
        {
            std::size_t result = 0;
            for( auto iter = mCache.begin(); iter != mCache.end(); ++iter )
            {
                result += (*iter)->second.usageCount;
            }
            return result;
        }
        
    };
    
} // namespace carl


#include "Cache.tpp"
