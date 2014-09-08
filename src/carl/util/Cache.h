/* 
 * File:   Cache.h
 * Author: Florian Corzilius
 *
 * Created on September 4, 2014, 11:24 AM
 */

#pragma once

#include <vector>
#include <stack>
#include <unordered_map>
#include <cassert>


namespace carl
{
    template<typename T>
    struct pointerEqual
    {
        bool operator()( const T* _argA, const T* _argB ) const
        {
            return (*_argA)==(*_argB);
        }
    };
    
    template<typename T> 
    struct pointerHash
    {
        size_t operator()( const T* _arg ) const
        {
            return std::hash<T>()( *_arg );
        }
    };
    
    template<typename T>
    bool returnFalse( const T&, const T& ) { return false; }
    
    template<typename T>
    void doNothing( T&, T& ) {}
    
    template<typename T>
    class Cache
    {
        
    public:
        typedef size_t Ref;
        
        struct Info
        {
            size_t usageCount;
            Ref    refStoragePos;
            double activity;

            Info( double _activity ):
                usageCount( 1 ),
                refStoragePos( 0 ),
                activity( _activity )
            {}
        };
        
        typedef std::unordered_map<T*, Info, pointerHash<T>, pointerEqual<T>> Container;
        
    private:
        // Members
        size_t                                    mMaxCacheSize;
        size_t                                    mNumOfUnusedEntries;
        double                                    mCacheReductionAmount;
        double                                    mMaxActivity;
        double                                    mActivityIncrement;
        double                                    mDecay;
        double                                    mActivityTreshold;
        double                                    mActivityDecrementFactor;
        Container                                 mCache;
        std::vector<typename Container::iterator> mCacheRefs;
        std::stack<Ref>                           mUnusedPositionsInCacheRefs;
        
    public:
            
        Cache( size_t _maxCacheSize = 10000, double _cacheReductionAmount = 0.2, double _decay = 0.98 );
        Cache( const Cache& ); // no implementation
        
        Ref cache( T* _toCache, bool (*_canBeUpdated)( const T&, const T& ) = &returnFalse<T>, void (*_update)( T&, T& ) = &doNothing<T> );
        void reg( Ref _refStoragePos );
        void dereg( Ref _refStoragePos );
        template<typename ArgType>
        void applyAndRehash( Ref _refStoragePos, void (*_f)( T&, ArgType ), ArgType _arg );
        void clean();
        void decayActivity();
        void updateActivity( Ref _refStoragePos );
        void print();
        
        const T& get( Ref _refStoragePos ) const
        {
            assert( _refStoragePos < mCacheRefs.size() );
            assert( mCacheRefs[_refStoragePos] != mCache.end() );
            return *mCacheRefs[_refStoragePos]->first;
        }
        
    };
    
} // namespace carl


#include "Cache.tpp"