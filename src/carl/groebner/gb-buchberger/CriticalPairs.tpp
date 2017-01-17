/**
 * @file CriticalPairs.tpp
 * @ingroup gb
 * @author Sebastian Junges
 */
#pragma once
#include "CriticalPairs.h"

namespace carl 
{

    template<template <class> class Datastructure, class Configuration>
    SPolPair CriticalPairs<Datastructure, Configuration>::pop( )
    {
        typename Configuration::Entry top = mDatastruct.top( );
        SPolPair ret = top->getFirst( );
        if( top->update( ) )
        {
            //empty, so we remove the top
            delete top;
            mDatastruct.pop( );
        }
        else
        {
            // not empty, get new leading element
            mDatastruct.decreaseTop( top );
        }
        return ret;
    }
    
    /**
     * 
     * @param lm
     * @param newpairs
     */
    template<template <class> class Datastructure, class Configuration>
    void CriticalPairs<Datastructure, Configuration>::elimMultiples( const Monomial::Arg& lm, const std::unordered_map<size_t, SPolPair>& newpairs )
    {
        typename Datastructure<Configuration>::const_iterator it( mDatastruct.begin( ) );
        while( it != mDatastruct.end( ) )
        {
            auto ps = it.get( )->getPairsBegin( );

            for( ++ps; ps != it.get( )->getPairsEnd( ); )
            {
                auto spp1 = newpairs.find(ps->mP1);
                auto spp2 = newpairs.find(ps->mP2);

                if( spp1 == newpairs.end( ) )
                {
                    ++ps;
                    continue;
                }
                if( spp2 == newpairs.end( ) )
                {
                    ++ps;
                    continue;
                }

                const Monomial::Arg & psLcm = ps->mLcm;
                if( psLcm->divisible( lm ) && psLcm != spp1->second.mLcm && psLcm != spp2->second.mLcm )
                {
                    ps = it.get( )->erase( ps );
                }
                else
                {
                    ++ps;
                }
            }
            if( it.get( )->getPairsBegin( ) == it.get( )->getPairsEnd( ) )
            {
                mDatastruct.popPosition( it );
            }
            else
            {
                it.next( );
            }
        }
    }
}
