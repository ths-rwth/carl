


/**
 * @file CriticalPairsEntry.h
 * @ingroup gb
 * @author Sebastian Junges
 */
#pragma once

#include <carl-arith/poly/umvpoly/Monomial.h>
#include "SPolPair.h"

#include <list>

namespace carl
{

/**
 * A list of SPol pairs which have to be checked by the Buchberger algorithm. 
 * We keep the list sorted according the compare ordering on SPol Pairs.
 * @ingroup gb
 */
template<class Compare>
class CriticalPairsEntry
{
public:

	/**
	 * Saves the list of pairs and sorts them according the configured ordering.
     * @param pairs
     */
    explicit CriticalPairsEntry(std::list<SPolPair>&& pairs) : mPairs(std::move(pairs))
    {
        mPairs.sort(SPolPairCompare<Compare>());
    }

	/**
	 * Get the LCM of the first element.
     * @return 
     */
    const Monomial::Arg& getSortedFirstLCM() const
    {
        return mPairs.front().mLcm;
    }

	/**
	 * Get the front of the list.
     * @return 
     */
    const SPolPair& getFirst() const {
        return mPairs.front();
    }

    /**
     * Removes the first element.
     * @return empty()
     */
    bool update() {
        mPairs.pop_front();
        return mPairs.empty();
    }

	/**
	 * The const iterator to the begin
     * @return begin of list
     */
    std::list<SPolPair>::const_iterator getPairsBegin() const noexcept {
        return mPairs.begin();
    }

	/**
	 * The const iterator to the end()
     * @return end of list
     */
    std::list<SPolPair>::const_iterator getPairsEnd() const noexcept {
        return mPairs.end();
    }

    /**
	 * The iterator to the end()
     * @return begin of list
     */
	std::list<SPolPair>::iterator getPairsBegin() noexcept {
        return mPairs.begin();
    }

	/**
	 * The iterator to the end()
     * @return end of list
     */
    std::list<SPolPair>::iterator getPairsEnd() noexcept {
        return mPairs.end();
    }

	/**
	 * Removes the element at the iterator.
     * @param it The iterator to the element to be erased.
     * @return The next element.
     */
    std::list<SPolPair>::iterator erase(std::list<SPolPair>::iterator it) {
        return mPairs.erase(it);
    }

    void print( std::ostream& os = std::cout )
    {
		for (const auto& p: mPairs) {
			p.print(os);
		}
    }
private:
    std::list<SPolPair> mPairs;
};
}
