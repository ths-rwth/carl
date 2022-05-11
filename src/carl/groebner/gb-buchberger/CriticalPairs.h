


/**
 * @file CriticalPairs.h
 * @ingroup gb
 * @author Sebastian Junges
 */
#pragma once

#include "../../core/CompareResult.h"
#include "../../core/MonomialOrdering.h"
#include <carl-common/datastructures/Heap.h>
#include "CriticalPairsEntry.h"

#include <unordered_map>

namespace carl
{

template< class Compare>
class CriticalPairConfiguration
{
public:
    using Entry = CriticalPairsEntry<Compare>*;
    using CompareResult = carl::CompareResult;

    static CompareResult compare( Entry e1, Entry e2 )
    {
        return Compare::compare( e1->getSortedFirstLCM( ), e2->getSortedFirstLCM( ) );
    }

    static bool cmpLessThan( CompareResult res )
    {
        return res == CompareResult::GREATER;
    }
    static const bool supportDeduplicationWhileOrdering = false;

    static bool cmpEqual( CompareResult res )
    {
        return res == CompareResult::EQUAL;
    }

    using Order = Compare;
    static const bool fastIndex = true;
};


/**
 * A data structure to store all the SPolynomial pairs which have to be checked.
 */
template<template <class> class Datastructure, class Configuration>
class CriticalPairs
{
public:

    CriticalPairs( ) : mDatastruct( Configuration( ) )
    {

    }

    /**
     * Add a list of s-pairs to the list.
     * @param pairs
     */
    void push( std::list<SPolPair> pairs )
    {
        if( pairs.empty( ) ) return;
        mDatastruct.push( new CriticalPairsEntry<typename Configuration::Order > ( std::move(pairs) ) );
    }

	/**
	 * Gets the first SPol from the data structure and removes it from the data structure. 
     * @return 
     */
    SPolPair pop( );
	/**
	 * Eliminate multiples of the given monomial.
     * @param lm
     * @param newpairs
     */
    void elimMultiples( const Monomial::Arg& lm, const std::unordered_map<size_t, SPolPair>& newpairs );
    
	/**
	 * Checks whether there are any pairs in the data structure.
     * @return 
     */
    bool empty( ) const
    {
        return mDatastruct.empty( );
    }

	/**
	 * Print the underlying data structure.
     */
    void print( ) const
    {
        mDatastruct.print( );
    }

	/**
	 * Checks the size of the data structure.
	 * Please notice that this is not necessarily the number of pairs in the data structure, as the underlying elements may be lists themselves.
     * @return 
     */
    unsigned size( ) const
    {
        return mDatastruct.size( );
    }

private:
    Datastructure<Configuration> mDatastruct;
};

typedef CriticalPairs<Heap, CriticalPairConfiguration<GrLexOrdering> > CritPairs;


}

#include "CriticalPairs.tpp"
