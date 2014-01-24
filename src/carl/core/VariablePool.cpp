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
std::unique_ptr<VariablePool> VariablePool::instance = nullptr;
std::mutex VariablePool::singletonMutex;

VariablePool::VariablePool():
	mNextVarId (1 << Variable::VARIABLE_BITS_RESERVED_FOR_TYPE)
{
	LOGMSG_INFO("carl.varpool", "Constructor called");
}

VariablePool& VariablePool::getInstance() {
	if (VariablePool::instance == nullptr) {
		VariablePool::singletonMutex.lock();
		if (VariablePool::instance == nullptr) {
			VariablePool::instance = std::unique_ptr<VariablePool>(new VariablePool());
		}
		VariablePool::singletonMutex.unlock();
	}
	return *VariablePool::instance;
}

Variable VariablePool::getFreshVariable(VariableType type) {
	this->freshVarMutex.lock();
	unsigned tmp = mNextVarId++;
	this->freshVarMutex.unlock();
	LOGMSG_DEBUG("carl.varpool", "New variable of type " << type << " with id " << tmp);
	return Variable(tmp, type);
}

Variable VariablePool::getFreshVariable(const std::string& name, VariableType type) {
	Variable tmp = this->getFreshVariable(type);
	this->setName(tmp, name);
	return tmp;
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
	this->freshVarMutex.lock();
	mFriendlyNames[v] = name;
	this->freshVarMutex.unlock();
	#endif
}

}

