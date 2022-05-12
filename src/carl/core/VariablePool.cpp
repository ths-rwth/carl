/**
 * @file VariablePool.cpp
 * @author Sebastian Junges
 */

#include "VariablePool.h"

#include "../initialize.h" // TODO get rid of this include
#include <carl-common/config.h>
#include <carl-logging/carl-logging.h>

namespace carl
{

VariablePool::VariablePool() noexcept {
	CARL_LOG_INFO("carl.varpool", "Constructor called");
	clear();
	setPrefix();
}

Variable VariablePool::getFreshVariable(VariableType type) noexcept {
	std::size_t tmp = 0;
	{
		FRESHVAR_LOCK_GUARD
		tmp = nextID(type)++;
	}
	CARL_LOG_DEBUG("carl.varpool", "New variable of type " << type << " with id " << tmp);
	return Variable(tmp, type);
}

Variable VariablePool::getFreshVariable(const std::string& name, VariableType type) {
	if (name.substr(0, mVariablePrefix.size()) == mVariablePrefix) {
		CARL_LOG_WARN("carl", "The prefix for auxiliary variable names \"" << mVariablePrefix << "\" is a prefix for the variable name \"" << name << "\".");
	}
	Variable tmp = getFreshVariable(type);
	setName(tmp, name);
	return tmp;
}

Variable VariablePool::getFreshPersistentVariable(VariableType type) noexcept {
	Variable res = getFreshVariable(type);
	if (res.id() >= mPersistentVariables.size()) {
		mPersistentVariables.resize(res.id()+1, std::make_pair(Variable::NO_VARIABLE, ""));
	}
	mPersistentVariables[res.id()] = std::make_pair(res, "");
	return res;
}

Variable VariablePool::getFreshPersistentVariable(const std::string& name, VariableType type) {
	Variable res = getFreshPersistentVariable(type);
	mPersistentVariables[res.id()] = std::make_pair(res, name);
	setName(res, name);
	return res;
}

Variable VariablePool::findVariableWithName(const std::string& name) const noexcept {
	for (const auto& v: mVariableNames) {
		if (v.second == name) return v.first;
	}
	return Variable::NO_VARIABLE;
}

std::string VariablePool::getName(Variable v, bool variableName) const {
	if (v.id() == 0) return "NO_VARIABLE";
	if (variableName) {
        SETNAME_LOCK_GUARD
		auto it = mVariableNames.find(v);
		if (it != mVariableNames.end()) {
			return it->second;
		}
	}
	switch (v.type()) {
		case VariableType::VT_BOOL:
			return mVariablePrefix + "b_" + std::to_string(v.id());
		case VariableType::VT_REAL:
			return mVariablePrefix + "r_" + std::to_string(v.id());
		case VariableType::VT_INT:
			return mVariablePrefix + "i_" + std::to_string(v.id());
		case VariableType::VT_UNINTERPRETED:
			return mVariablePrefix + "u_" + std::to_string(v.id());
        case VariableType::VT_BITVECTOR:
            return mVariablePrefix + "bv_" + std::to_string(v.id());
		default:
			CARL_LOG_ERROR("carl", "Variable has invalid type: " << v.type());
			assert(false);
			return "";
	}
}

void VariablePool::setName(Variable v, const std::string& name) {
	SETNAME_LOCK_GUARD
	mVariableNames[v] = name;
}

}
