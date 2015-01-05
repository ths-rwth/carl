/**
 * @file VariablePool.cpp
 * @author Sebastian Junges
 */

#include "VariablePool.h"

#include <mutex>

#include "initialize.h"
#include "config.h"
#include "logging.h"

namespace carl
{

VariablePool::VariablePool(): Singleton()
{
	CARL_LOG_INFO("carl.varpool", "Constructor called");
	clear();
}

Variable VariablePool::getFreshVariable(VariableType type) {
	unsigned tmp = 0;
	{
		std::lock_guard<std::mutex> lock(this->freshVarMutex);
		tmp = nextID(type)++;
	}
	CARL_LOG_DEBUG("carl.varpool", "New variable of type " << type << " with id " << tmp);
	return Variable(tmp, type);
}

Variable VariablePool::getFreshVariable(const std::string& name, VariableType type) {
	if (name.substr(0, mVariablePrefix.size()) == mVariablePrefix) {
		CARL_LOG_WARN("carl", "The prefix for auxiliary variable names \"" << mVariablePrefix << "\" is a prefix for the variable name \"" << name << "\".");
	}
	Variable tmp = this->getFreshVariable(type);
	this->setName(tmp, name);
	return tmp;
}

Variable VariablePool::findVariableWithName(const std::string& name) const
{
	for (auto v: mVariableNames) {
		if (v.second == name) return v.first;
	}
	return Variable::NO_VARIABLE;
}

const std::string VariablePool::getName(Variable::Arg v, bool variableName) const {
	if (v.getId() == 0) return "NO_VARIABLE";
	if (variableName) {
		std::map<Variable, std::string>::const_iterator it = mVariableNames.find(v);
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
	}
}

void VariablePool::setName(Variable::Arg v, const std::string& name) {
	#ifdef CARL_USE_FRIENDLY_VARNAMES
	std::lock_guard<std::mutex> lock(this->freshVarMutex);
	mVariableNames[v] = name;
	#endif
}

}

