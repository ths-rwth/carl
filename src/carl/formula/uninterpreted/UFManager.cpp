/**
 * @file UFManager.cpp
 * @author Florian Corzilius <corzilius@cs.rwth-aachen.de>
 * @since 2014-10-30
 * @version 2014-10-30
 */

#include "UFManager.h"
#include <string>

namespace carl {

	UninterpretedFunction UFManager::newUF(const UFContent* ufc) {
		auto iter = mUFIdMap.find(ufc);
		// Check if this uninterpreted function content has already been created
		if (iter != mUFIdMap.end()) {
			delete ufc;
			return UninterpretedFunction(iter->second);
		}
		// Create the uninterpreted function
		mUFIdMap.emplace(ufc, mUFs.size());
		UninterpretedFunction uf(mUFs.size());
		mUFs.push_back(ufc);
		return uf;
	}
}
