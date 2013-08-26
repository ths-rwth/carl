/* 
 * File:   PolynomialSorts.h
 * Author: Sebastian Junges
 *
 */

#pragma once

/**
 * Sorts generators of an ideal by their leading terms.
 * @param generators
 */
template<class Polynomial>
class sortByLeadingTerm
{
public:

    sortByLeadingTerm( const std::vector<Polynomial>& generators ) :
    mGenerators( generators )
    {
    }

    sortByLeadingTerm( const sortByLeadingTerm& sBLT ) :
    mGenerators( sBLT.mGenerators )
    {
    }

    bool operator( )( size_t a, size_t b ) const
    {
        return Polynomial::compareByLeadingTerm( mGenerators[a], mGenerators[b] );
    }
private:
    const std::vector<Polynomial>& mGenerators;
};

/**
 * Sorts generators of an ideal by their number of terms.
 * @param generators
 */
template<class Polynomial>
class sortByPolSize
{
public:

    sortByPolSize( const std::vector<Polynomial>& generators ) :
    mGenerators( generators )
    {
    }

    bool operator( )( size_t a, size_t b ) const
    {
        return Polynomial::compareByNrTerms( mGenerators[a], mGenerators[b] );
    }
private:
    const std::vector<Polynomial>& mGenerators;
};
