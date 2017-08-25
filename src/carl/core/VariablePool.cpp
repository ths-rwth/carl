/**
 * @file VariablePool.cpp
 * @author Sebastian Junges
 */

#include "VariablePool.h"

#include "initialize.h"
#include "config.h"
#include "logging.h"

namespace carl
{

VariablePool::VariablePool() noexcept:
    Singleton(),
    mNextIDs(),
	freshVarMutex(),
	setNameMutex(),
    mVariableNames(),
    mVariablePrefix()
{
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
	if (res.getId() >= mPersistentVariables.size()) {
		mPersistentVariables.resize(res.getId()+1, std::make_pair(Variable::NO_VARIABLE, ""));
	}
	mPersistentVariables[res.getId()] = std::make_pair(res, "");
	return res;
}

Variable VariablePool::getFreshPersistentVariable(const std::string& name, VariableType type) {
	Variable res = getFreshPersistentVariable(type);
	mPersistentVariables[res.getId()] = std::make_pair(res, name);
	setName(res, name);
	return res;
}

Variable VariablePool::findVariableWithName(const std::string& name) const noexcept {
	for (const auto& v: mVariableNames) {
		if (v.second == name) return v.first;
	}
	return Variable::NO_VARIABLE;
}

std::string VariablePool::getName(Variable::Arg v, bool variableName) const {
	if (v.getId() == 0) return "NO_VARIABLE";
	if (variableName) {
        SETNAME_LOCK_GUARD
		auto it = mVariableNames.find(v);
		if (it != mVariableNames.end()) {
			return it->second;
		}
	}
	switch (v.getType()) {
		case VariableType::VT_BOOL:
			return mVariablePrefix + "b_" + std::to_string(v.getId());
		case VariableType::VT_REAL:
			return mVariablePrefix + "r_" + std::to_string(v.getId());
		case VariableType::VT_INT:
			return mVariablePrefix + "i_" + std::to_string(v.getId());
		case VariableType::VT_UNINTERPRETED:
			return mVariablePrefix + "u_" + std::to_string(v.getId());
        case VariableType::VT_BITVECTOR:
            return mVariablePrefix + "bv_" + std::to_string(v.getId());
		default:
			CARL_LOG_ERROR("carl", "Variable has invalid type: " << v.getType());
			assert(false);
			return "";
	}
}

void VariablePool::setName(Variable::Arg v, const std::string& name) {
	#ifdef CARL_USE_FRIENDLY_VARNAMES
	SETNAME_LOCK_GUARD
	mVariableNames[v] = name;
	#endif
}

}
