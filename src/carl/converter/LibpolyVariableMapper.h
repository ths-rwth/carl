#pragma once

#include "../config.h"

#if defined(USE_LIBPOLY) || defined(RAN_USE_LIBPOLY)

#include "../core/Variable.h"
#include "../util/Common.h"
#include "../util/SFINAE.h"
#include "../util/Singleton.h"

#include <map>

#include <poly/polyxx.h>

namespace carl {

class VariableMapper : public Singleton<VariableMapper> {
	//mapping from carl variables to libpoly variables
	std::map<carl::Variable, poly::Variable> mVarsCarlLibpoly;
	//mapping from libpoly variables to carl variables
	std::map<poly::Variable, carl::Variable> mVarsLibolyCarl;

public:
	VariableMapper();

	/**
	 * Convert a carl variable to a libpoly variable
	 * If the variable has not been converted before, a new libpoly variable with the same name is created
	 * @return The corresponding libpoly variable
	 */
	poly::Variable getLibpolyVariable(const carl::Variable& var);

	/**
	 * Convert a Libpoly Variable to the corresponding Carl Variable
	 * Asserts that the libpoly variable has been converted from a carl variable before
	 * @return The corresponding carl variable
	 */
	carl::Variable getCarlVariable(const poly::Variable& var);

	/**
	 * Helper methods to alter the internal variable ordering in libpoly
	 * The variable ordering is global (we use the same context for all polynomials) and has to be checked for each carl-polynomial
	 * Important: Libpoly Polynomials do NOT automatically reorder according to the variable ordering
	 * the polynomial needs to be external or manually reordered (see lp_polynomial_ensure_order)
	 */
	void setLibpolyVariableOrder(const std::vector<carl::Variable>& variableOrder);
	void setLibpolyVariableOrder(const std::vector<poly::Variable>& variableOrder);
	void setLibpolyMainVariable(const carl::Variable& mainVar);
	void setLibpolyMainVariable(const poly::Variable& mainVar);
};

} // namespace carl

#endif