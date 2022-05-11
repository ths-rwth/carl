


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