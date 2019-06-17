/**
 * @file SortValueManager.h
 * @author Florian Corzilius <corzilius@cs.rwth-aachen.de>
 * @since 2014-10-24
 * @version 2014-10-24
 */

#pragma once

#include <cassert>
#include <iostream>
#include <map>
#include <set>
#include <utility>
#include <vector>

#include <carl/util/Common.h>
#include <carl/util/Singleton.h>
#include "SortValue.h"

namespace carl {

/**
 * Implements a manager for sort values, containing the actual contents of these sort and allocating their ids.
 */
class SortValueManager : public Singleton<SortValueManager> {
	friend Singleton<SortValueManager>;

private:
	// Members.

	/// Stores for each sort the latest instantiated sort value.
	carl::FastMap<Sort, std::size_t> mSortValueIDMap;

	/**
	 * Constructs a sort value manager.
	 */
	SortValueManager() = default;

public:
	/**
     * Creates a new value for the given sort.
     * @param _sort The sort to create a new value for.
     * @return The resulting sort value.
     */
	SortValue newSortValue(const Sort& sort) {
		auto res = mSortValueIDMap.emplace(sort, 1);
		if (!res.second) {
			++res.first->second;
		}
		return SortValue(sort, res.first->second);
	}
	/**
	 * Returns the default value for the given sort.
	 * @param _sort The sort to return the default value for.
	 * @return The resulting sort value.
	 */
	SortValue defaultSortValue(const Sort& sort) const {
		return SortValue(sort, 0);
	}
};

/**
 * Creates a new value for the given sort.
 * @param _sort The sort to create a new value for.
 * @return The resulting sort value.
 */
inline SortValue newSortValue(const Sort& sort) {
	return SortValueManager::getInstance().newSortValue(sort);
}
/**
 * Returns the default value for the given sort.
 * @param _sort The sort to return the default value for.
 * @return The resulting sort value.
 */
inline SortValue defaultSortValue(const Sort& sort) {
	return SortValueManager::getInstance().defaultSortValue(sort);
}

} // namespace carl
