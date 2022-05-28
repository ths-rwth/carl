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
	return it->second;
}
void VariableMapper::setLibpolyVariableOrder(const std::vector<poly::Variable>& variableOrder) {
	//Write order back to libpoly
	lp_variable_order_t* vo = poly::Context::get_context().get_variable_order();
	lp_variable_order_clear(vo);
	for (const auto& var : variableOrder) {
		lp_variable_order_push(vo, var.get_internal());
	}
}
void VariableMapper::setLibpolyVariableOrder(const std::vector<carl::Variable>& variableOrder) {
	//convert to libpoly variables in the same ordering
	std::vector<poly::Variable> lp_order;
	for (const carl::Variable& var : variableOrder) {
		lp_order.emplace_back(getLibpolyVariable(var));
	}
	setLibpolyVariableOrder(lp_order);
}
void VariableMapper::setLibpolyMainVariable(const poly::Variable& mainVar) {
	CARL_LOG_DEBUG("carl.converter", "Main Var to set:  " << mainVar);

	lp_variable_order_t* vo = poly::Context::get_context().get_variable_order();
	//if (mainVar.get_internal() == lp_variable_order_top(vo)) {
	//	//mainVar is already correct
	//	CARL_LOG_DEBUG("carl.converter", "Main Var is already correct");
	//	return;
	//} else {
	//TODO das geht bestimmt besser...
	//take all known variables to in the VariableMapper
	std::vector<poly::Variable> known_vars;
	for (const auto& var : mVarsCarlLibpoly) {
		known_vars.push_back(var.second);
	}

	//new mainVar must be known
	assert(std::find(known_vars.begin(), known_vars.end(), mainVar) != known_vars.end());

	// remove var and push to back
	known_vars.erase(std::remove(known_vars.begin(),
								 known_vars.end(), mainVar),
					 known_vars.end());
	known_vars.insert(known_vars.end(), mainVar);

	//write the new order back to libpoly
	setLibpolyVariableOrder(known_vars);
	//	}
}
void VariableMapper::setLibpolyMainVariable(const carl::Variable& mainVar) {
	setLibpolyMainVariable(getLibpolyVariable(mainVar));
}

} // namespace carl

#endif