/*
 * GiNaCRA - GiNaC Real Algebra package
 * Copyright (C) 2010-2012  Ulrich Loup, Joachim Redies, Sebastian Junges
 *
 * This file is part of GiNaCRA.
 *
 * GiNaCRA is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GiNaCRA is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GiNaCRA.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


/**
 * @file CriticalPairsEntry.h
 * @ingroup gb
 * @author Sebastian Junges
 */
#pragma once

#include <list>
#include "SPolPair.h"
#include "../../core/Monomial.h"

namespace carl
{

/**
 * @ingroup gb
 */
template<class Compare>
class CriticalPairsEntry
{
public:

    CriticalPairsEntry( const std::list<SPolPair>& pairs ) : mPairs( pairs )
    {
        mPairs.sort( SPolPairCompare<Compare > ( ) );
    }

    const Monomial& getSortedFirstLCM( ) const
    {
        return mPairs.front( ).mLcm;
    }

    const SPolPair getFirst( ) const
    {
        return mPairs.front( );
    }

    /**
     * removes the first element.
     * @return empty()
     */
    bool update( )
    {
        mPairs.pop_front( );
        return mPairs.empty( );
    }

    std::list<SPolPair>::const_iterator getPairsBegin( ) const
    {
        return mPairs.begin( );
    }

    std::list<SPolPair>::const_iterator getPairsEnd( ) const
    {
        return mPairs.end( );
    }

    std::list<SPolPair>::iterator getPairsBegin( )
    {
        return mPairs.begin( );
    }

    std::list<SPolPair>::iterator getPairsEnd( )
    {
        return mPairs.end( );
    }

    std::list<SPolPair>::iterator erase( std::list<SPolPair>::iterator it )
    {
        return mPairs.erase( it );
    }

    void print( std::ostream& os = std::cout )
    {
        for( std::list<SPolPair>::const_iterator it = mPairs.begin( ); it != mPairs.end( ); ++it )
        {
            it->print( os );
        }
    }
private:
    std::list<SPolPair> mPairs;
};
}
