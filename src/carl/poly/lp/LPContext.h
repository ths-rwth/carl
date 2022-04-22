#pragma once

#if defined(USE_LIBPOLY) || defined(RAN_USE_LIBPOLY)

#include "../../core/Variable.h"
#include "../../converter/LibpolyVariableMapper.h"

#include <map>

#include <poly/polyxx.h>
#include <poly/polynomial_context.h>
#include <poly/poly.h>

namespace carl {

class LPContext {
	lp_polynomial_context_t* mContext;

    const std::vector<Variable> mVariableOrder;

public:
	/**
	 * Default constructor shall not exist. Use LPPolynomial(Variable) instead.
	 */
	LPContext() = delete;

    ~LPContext() {
        lp_polynomial_context_detach(mContext);
    }

	/**
	 * Construct a Context with a given order of carl::Variable in decreasing order of precedence
	 */
    LPContext(const std::vector<Variable>& varOrder) : mVariableOrder(varOrder) {

        CARL_LOG_DEBUG("carl.poly.lp", "Creating context with variables: " << varOrder);

		// Add the corresponding libpoly variables into database and order
        lp_variable_db_t* var_db = lp_variable_db_new();
        lp_variable_order_t* var_order = lp_variable_order_new();
        
        std::string varName;
        poly::Variable polyVar; 
        for(size_t i=0; i < varOrder.size(); i++) {
            varName = varOrder[i].name();
            polyVar = VariableMapper::getInstance().getLibpolyVariable(varOrder[i]);
            lp_variable_db_add_variable(var_db, polyVar.get_internal(), &varName[0]);
            lp_variable_order_push(var_order, polyVar.get_internal());
        }

        // Create the context
        mContext = lp_polynomial_context_new(0, var_db, var_order);
        
        // Detach local references
        lp_variable_db_detach(var_db);
        lp_variable_order_detach(var_order);
	};

    lp_polynomial_context_t* getContext() {
        return mContext;
    };

    const lp_polynomial_context_t* getContext() const {
        return mContext;
    };

    const std::vector<Variable>& getVariableOrder() const {
        return mVariableOrder;
    };

    bool has(const Variable& var) const {
        return std::find(mVariableOrder.begin(), mVariableOrder.end(), var) != mVariableOrder.end();
    };


    /**
     * @brief Returns true if the underlying variable ordering is the same as the given one.
     */
    inline bool operator==(const LPContext& rhs) const {
        return mVariableOrder == rhs.mVariableOrder;
    }
};

}

#endif
