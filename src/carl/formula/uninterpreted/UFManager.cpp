/**
 * @file UFManager.cpp
 * @author Florian Corzilius <corzilius@cs.rwth-aachen.de>
 * @since 2014-10-30
 * @version 2014-10-30
 */

#include "UFManager.h"
#include <string>

namespace carl {
	std::ostream& UFManager::print(std::ostream& _out, const UninterpretedFunction& _uf) const {
		assert(_uf.id() != 0);
		assert(_uf.id() < mUFs.size());
		const UFContent& ufc = *mUFs[_uf.id()];
		_out << ufc.name() << "(";
		bool first = true;
		for (const auto& dom: ufc.domain()) {
			if (first) {
				_out << " ";
				first = false;
			}
			_out << dom;
		}
		_out << ") " << ufc.codomain();
		return _out;
	}
	
	UninterpretedFunction UFManager::newUF(const UFContent* _ufc) {
		auto iter = mUFIdMap.find(_ufc);
		// Check if this uninterpreted function content has already been created
		if (iter != mUFIdMap.end()) {
			delete _ufc;
			return UninterpretedFunction(iter->second);
		}
		// Create the uninterpreted function
		mUFIdMap.emplace(_ufc, mUFs.size());
		UninterpretedFunction uf(mUFs.size());
		mUFs.push_back(_ufc);
		return uf;
	}
}
