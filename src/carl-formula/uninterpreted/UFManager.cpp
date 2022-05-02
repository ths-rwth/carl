/**
 * @file UFManager.cpp
 * @author Florian Corzilius <corzilius@cs.rwth-aachen.de>
 * @since 2014-10-30
 * @version 2014-10-30
 */

#include "UFManager.h"
#include <string>

namespace carl {

	UninterpretedFunction UFManager::newUF(std::unique_ptr<UFContent>&& ufc) {
		auto iter = mUFIdMap.find(ufc.get());
		// Check if this uninterpreted function content has already been created
		if (iter != mUFIdMap.end()) {
			return UninterpretedFunction(iter->second);
		}
		// Create the uninterpreted function
		mUFIdMap.emplace(ufc.get(), mUFs.size());
		UninterpretedFunction uf(mUFs.size());
		mUFs.push_back(std::move(ufc));
		return uf;
	}
}
