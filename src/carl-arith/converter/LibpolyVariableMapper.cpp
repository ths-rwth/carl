#include "LibpolyVariableMapper.h"

#include <carl-common/config.h>
#ifdef USE_LIBPOLY

namespace carl {

VariableMapper::VariableMapper() {
	CARL_LOG_DEBUG("carl.converter", "Initialized Carl <-> Libpoly Variable Mapper");
}

carl::Variable VariableMapper::getCarlVariable(const poly::Variable& var) {
	CARL_LOG_DEBUG("carl.converter", "Converting Libpoly Var: " << var);
	auto it = mVarsLibolyCarl.find(var);
	assert(it != mVarsLibolyCarl.end()); // Can only convert already known variable back
	CARL_LOG_TRACE("carl.converter", "-> " << it->second);
	return it->second;
}
poly::Variable VariableMapper::getLibpolyVariable(const carl::Variable& var) {
	CARL_LOG_DEBUG("carl.converter", "Converting Carl Var: " << var);
	auto it = mVarsCarlLibpoly.find(var);
	if (it == mVarsCarlLibpoly.end()) {
		CARL_LOG_DEBUG("carl.converter", "Variable is new");
		//Create a new libpoly variable with the same name
		it = mVarsCarlLibpoly.emplace(var, poly::Variable(var.name().c_str())).first;
		mVarsLibolyCarl.emplace(it->second, var);
	}
	CARL_LOG_TRACE("carl.converter", "-> " << it->second);
	return it->second;
}
} // namespace carl

#endif