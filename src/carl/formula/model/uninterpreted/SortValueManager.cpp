/**
 * @file SortValueManager.cpp
 * @author Florian Corzilius <corzilius@cs.rwth-aachen.de>
 * @since 2014-10-24
 * @version 2014-10-24
 */

#include "SortValueManager.h"

namespace carl {

    SortValue SortValueManager::newSortValue(const carl::Sort& _sort) {
        auto res = mSortValueIDMap.emplace(_sort, 1);
        if (!res.second) {
            ++res.first->second;
        }
        return SortValue(_sort, res.first->second);
    }
	SortValue SortValueManager::defaultSortValue(const Sort& _sort) {
		return SortValue(_sort, 0);
	}
}
