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

#include "../../core/Monomial.h"
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
