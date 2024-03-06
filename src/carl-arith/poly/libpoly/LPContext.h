#pragma once

#include <carl-common/config.h>
#ifdef USE_LIBPOLY

#include "../../core/Variable.h"

#include <map>

#include <poly/poly.h>
#include <poly/polynomial_context.h>
#include <poly/polyxx.h>

#include <carl-common/util/streamingOperators.h>

#include "LPVariables.h"

namespace carl {

class LPContext {
    struct Data {
        std::vector<Variable> variable_order;

        lp_variable_order_t* lp_var_order;
        lp_polynomial_context_t* lp_context;

        Data(const std::vector<Variable>& v) : variable_order(v) {
            lp_var_order = lp_variable_order_new();
            // lp_context = lp_polynomial_context_new(0, LPVariables::getInstance().lp_var_db, lp_var_order);
            lp_context = (lp_polynomial_context_t*) malloc(sizeof(lp_polynomial_context_t));
            //lp_polynomial_context_construct(lp_context, 0, LPVariables::getInstance().lp_var_db, lp_var_order);
            lp_context->ref_count = 0;
            lp_context->var_db = LPVariables::getInstance().lp_var_db;
            lp_context->K = 0;
            lp_context->var_order = lp_var_order;
            #define TEMP_VARIABLE_SIZE 10
            lp_context->var_tmp = (lp_variable_t*)malloc(sizeof(lp_variable_t)*TEMP_VARIABLE_SIZE);
            for (size_t i = 0; i < TEMP_VARIABLE_SIZE; ++ i) {
                lp_context->var_tmp[i] = LPVariables::getInstance().lp_var_tmp[i];
            }
            lp_context->var_tmp_size = 0;
            lp_polynomial_context_attach(lp_context);
        }
        ~Data() {
            lp_variable_order_detach(lp_var_order);
            lp_polynomial_context_detach(lp_context);
        }
    };

    std::shared_ptr<Data> m_data;

public:
    /**
     * Default constructor shall not exist.
     */
    LPContext() = delete;

    LPContext(const LPContext& rhs) {
        m_data = rhs.m_data;
    }

    LPContext& operator=(const LPContext& rhs) {
        m_data = rhs.m_data;
        return *this;
    }

    std::optional<carl::Variable> carl_variable(lp_variable_t var) const {
        return LPVariables::getInstance().carl_variable(var);
    }

    lp_variable_t lp_variable(carl::Variable var) const {
        return LPVariables::getInstance().lp_variable(var);
    }

    std::optional<lp_variable_t> lp_variable_opt(carl::Variable var) const {
        return LPVariables::getInstance().lp_variable_opt(var);
    }


    /**
     * Construct a Context with a given order of carl::Variable in decreasing order of precedence
     */
    LPContext(const std::vector<Variable>& carl_var_order) : m_data(std::make_shared<Data>(carl_var_order)) {
        CARL_LOG_FUNC("carl.poly", carl_var_order);
        for (size_t i = 0; i < carl_var_order.size(); i++) {
            lp_variable_t poly_var = LPVariables::getInstance().lp_variable(carl_var_order[i]);
            lp_variable_order_push(m_data->lp_var_order, poly_var);            
        }
    };

    lp_polynomial_context_t* lp_context() {
        return m_data->lp_context;
    };

    const lp_polynomial_context_t* lp_context() const {
        return m_data->lp_context;
    };

    const std::vector<Variable>& variable_ordering() const {
        return m_data->variable_order;
    }

    bool has(const Variable& var) const {
        return std::find(variable_ordering().begin(), variable_ordering().end(), var) != variable_ordering().end();
    };

    /**
     * @brief Returns true if the underlying data is the same (by pointers).
     */
    inline bool operator==(const LPContext& rhs) const {
        return m_data == rhs.m_data;
    }
};

inline std::ostream& operator<<(std::ostream& os, const LPContext& ctx) {
    os << ctx.variable_ordering();
    return os;
}

} // namespace carl

#endif