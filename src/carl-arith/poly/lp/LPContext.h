#pragma once

#include <carl-common/config.h>
#ifdef USE_LIBPOLY

#include "../../converter/LibpolyVariableMapper.h"
#include "../../core/Variable.h"

#include <map>

#include <poly/poly.h>
#include <poly/polynomial_context.h>
#include <poly/polyxx.h>

#include <carl-common/util/streamingOperators.h>

namespace carl {

class LPContext {
    lp_polynomial_context_t* mContext;

    std::vector<Variable> mVariableOrder;

public:
    /**
     * Default constructor shall not exist.
     */
    LPContext() = delete;

    ~LPContext() {
        lp_polynomial_context_detach(mContext);
    }

    LPContext(const LPContext& rhs) {
        mContext = rhs.mContext;
        lp_polynomial_context_attach(mContext);
        mVariableOrder = rhs.mVariableOrder;
    }

    LPContext& operator=(const LPContext& rhs) {
        mContext = rhs.mContext;
        lp_polynomial_context_attach(mContext);
        mVariableOrder = rhs.mVariableOrder;
        return *this;
    }

    /**
     * Construct a Context with a given order of carl::Variable in decreasing order of precedence
     */
    LPContext(const std::vector<Variable>& varOrder)
        : mVariableOrder(varOrder) {

        CARL_LOG_DEBUG("carl.poly", "Creating context with variables: " << varOrder);

        // Add the corresponding libpoly variables into database and order
        lp_variable_db_t* var_db = lp_variable_db_new();
        lp_variable_order_t* var_order = lp_variable_order_new();

        std::string varName;
        poly::Variable polyVar;

        // Libpoly handles the variable order exactly the other way around
        // i.e the main Variable is not the fist one but the last one
        for (size_t i = varOrder.size(); i-- > 0;) {
            varName = varOrder[i].name();
            CARL_LOG_DEBUG("carl.poly", "Variable name: " << varName << "  " << i);
            polyVar = VariableMapper::getInstance().getLibpolyVariable(varOrder[i]);
            lp_variable_db_add_variable(var_db, polyVar.get_internal(), &varName[0]);
            lp_variable_order_push(var_order, polyVar.get_internal());
        }

        // Create the context
        mContext = lp_polynomial_context_new(0, var_db, var_order);
    };

    LPContext(lp_polynomial_context_t* ctx) {
        mContext = ctx;
        lp_polynomial_context_attach(mContext); // attach context to the object

        // Build the variable order for carl Variables, this has to be done in reverse order
        // because libpoly handles the variable order exactly the other way around
        // i.e the main Variable is not the fist one but the last one
        const lp_variable_list_t* varList = lp_variable_order_get_list(mContext->var_order);
        for (size_t i = lp_variable_list_size(varList); i-- > 0;) {
            mVariableOrder.push_back(VariableMapper::getInstance().getCarlVariable(poly::Variable(varList->list[i])));
        }
    }

    lp_polynomial_context_t* getContext() {
        return mContext;
    };

    const lp_polynomial_context_t* getContext() const {
        return mContext;
    };

    std::vector<Variable> getVariableOrder() const {
        return mVariableOrder;
    }

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

inline std::ostream& operator<<(std::ostream& os, const LPContext& ctx) {
    os << ctx.getVariableOrder();
    return os;
}

} // namespace carl

#endif