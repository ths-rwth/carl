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

#include "../../../util/Common.h"
#include "../../../util/Singleton.h"
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
    SortValue newSortValue(const Sort& _sort);
	/**
	 * Returns the default value for the given sort.
	 * @param _sort The sort to return the default value for.
	 * @return The resulting sort value.
	 */
	SortValue defaultSortValue(const Sort& _sort);
};

/**
 * Creates a new value for the given sort.
 * @param _sort The sort to create a new value for.
 * @return The resulting sort value.
 */
inline SortValue newSortValue(const Sort& _sort) {
    return SortValueManager::getInstance().newSortValue(_sort);
}
/**
 * Returns the default value for the given sort.
 * @param _sort The sort to return the default value for.
 * @return The resulting sort value.
 */
inline SortValue defaultSortValue(const Sort& _sort) {
    return SortValueManager::getInstance().defaultSortValue(_sort);
}

}
