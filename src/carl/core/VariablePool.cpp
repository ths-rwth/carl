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

VariablePool::VariablePool():
	Singleton(),
	mNextVarId (1 << Variable::VARIABLE_BITS_RESERVED_FOR_TYPE)
{
	LOGMSG_INFO("carl.varpool", "Constructor called");
}

Variable VariablePool::getFreshVariable(VariableType type) {
	unsigned tmp = 0;
	{
		std::lock_guard<std::mutex> lock(this->freshVarMutex);
		tmp = mNextVarId++;
	}
	LOGMSG_DEBUG("carl.varpool", "New variable of type " << type << " with id " << tmp);
	return Variable(tmp, type);
}

Variable VariablePool::getFreshVariable(const std::string& name, VariableType type) {
	Variable tmp = this->getFreshVariable(type);
	this->setName(tmp, name);
	return tmp;
}

Variable VariablePool::findVariableWithName(const std::string& name) const
{
    for(auto v : mFriendlyNames)
    {
        if(v.second == name)
        {
            return v.first;
        }
    }
    return Variable();
}

const std::string VariablePool::getName(Variable::Arg v, bool friendly) const {
	if (v == Variable::NO_VARIABLE) return "NO_VARIABLE";
	if (friendly) {
		std::map<Variable, std::string>::const_iterator it = mFriendlyNames.find(v);
		if (it == mFriendlyNames.end()) {
			return "x_" + std::to_string(v.getId());
		} else {
			return it->second;
		}
	} else {
		return "x_" + std::to_string(v.getId());
	}
}

void VariablePool::setName(Variable::Arg v, const std::string& name) {
	#ifdef CARL_USE_FRIENDLY_VARNAMES
	std::lock_guard<std::mutex> lock(this->freshVarMutex);
	mFriendlyNames[v] = name;
	#endif
}

}

