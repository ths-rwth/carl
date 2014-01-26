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
 * @file   BuchbergerStats.h
 * @author Sebastian Junges
 *
 */

#pragma once

namespace carl
{

/**
 * A little class for gathering statistics about the Buchberger algorithm calls.
 */
class BuchbergerStats
{
public:
    static BuchbergerStats* getInstance( );

    /**
     *  Count that we found a TSQ which had a constant trailing term
     */
    void TSQWithConstant( )
    {
        mNrOfTSQWithConstant++;
    }

    /**
     * Count that we found a TSQ which did not have a constant trailing term
     */
    void TSQWithoutConstant( )
    {
        mNrOfTSQWithoutConstant++;
    }

    /**
     * Count that we could reduce a single term polynomial by calculating the Squarefree part
     */
    void SingleTermSFP( )
    {
        mNrOfSingleTermSFP++;
    }

    void ReducibleIdentity( )
    {
        mNrOfReducibleIdentities++;
    }
    /**
     *  Count that we take and reduce another S-Pair
     */
    void TreatSPair( )
    {
        mNrOfReductions++;
    }

    /**
     * Count that an S-Pair reduced to some non zero polynomial
     */
    void NonZeroReduction( )
    {
        mNrOfNonZeroReductions++;
    }

    unsigned getNrTSQWithConstant( ) const
    {
        return mNrOfTSQWithConstant;
    }

    unsigned getNrTSQWithoutConstant( ) const
    {
        return mNrOfTSQWithoutConstant;
    }

    unsigned getSingleTermSFP( ) const
    {
        return mNrOfSingleTermSFP;
    }
    
    unsigned getNrReducibleIdentities( ) const
    {
        return mNrOfReducibleIdentities;
    }
protected:

    BuchbergerStats( ) :
    mNrOfTSQWithConstant( 0 ),
    mNrOfTSQWithoutConstant( 0 ),
    mNrOfSingleTermSFP( 0 ),
    mNrOfReducibleIdentities( 0 ),
    mNrOfReductions( 0 ),
    mNrOfNonZeroReductions( 0 )
    {
    }
    unsigned mNrOfTSQWithConstant;
    unsigned mNrOfTSQWithoutConstant;
    unsigned mNrOfSingleTermSFP;
    unsigned mNrOfReducibleIdentities;
    unsigned mNrOfReductions;
    unsigned mNrOfNonZeroReductions;

private:
    static BuchbergerStats* instance;
};
}