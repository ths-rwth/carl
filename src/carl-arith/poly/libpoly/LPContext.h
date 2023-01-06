#pragma once

#include <carl-common/config.h>
#ifdef USE_LIBPOLY

#include "../../core/Variable.h"

#include <map>

#include <poly/poly.h>
#include <poly/polynomial_context.h>
#include <poly/polyxx.h>

#include <carl-common/util/streamingOperators.h>

namespace carl {

class LPContext {
    struct Data {
        std::vector<Variable> variable_order;
        // mapping from carl variables to libpoly variables
        std::map<carl::Variable, lp_variable_t> vars_carl_libpoly;
        // mapping from libpoly variables to carl variables
        std::map<lp_variable_t, carl::Variable> vars_libpoly_carl;

        // lp_variable_db_t* lp_var_db;
        // lp_variable_order_t* lp_var_order;
        // lp_polynomial_context_t* lp_context;

        poly::Context poly_context;

        Data(const std::vector<Variable>& v) : variable_order(v) {}
        // ~Data() {
        //     lp_variable_db_detach(lp_var_db);
        //     lp_variable_order_detach(lp_var_order);
        //     lp_polynomial_context_detach(lp_context);
        // }
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

    carl::Variable carl_variable(lp_variable_t var) const {
        auto it = m_data->vars_libpoly_carl.find(var);
        assert(it != m_data->vars_libpoly_carl.end());
        CARL_LOG_TRACE("carl.poly", "Mapping libpoly variable " << lp_variable_db_get_name(m_data->poly_context.get_variable_db(), var) << " -> " << it->second);
        return it->second;
    }

    lp_variable_t lp_variable(carl::Variable var) const {
        auto it = m_data->vars_carl_libpoly.find(var);
        assert(it != m_data->vars_carl_libpoly.end());
        CARL_LOG_TRACE("carl.poly", "Mapping carl variable " << var << " -> " << lp_variable_db_get_name(m_data->poly_context.get_variable_db(), it->second));
        return it->second;
    }

    /**
     * Construct a Context with a given order of carl::Variable in decreasing order of precedence
     */
    LPContext(const std::vector<Variable>& carl_var_order) : m_data(std::make_shared<Data>(carl_var_order)) {
        CARL_LOG_FUNC("carl.poly", carl_var_order);
        // m_data->lp_var_db = lp_variable_db_new();
        // m_data->lp_var_order = lp_variable_order_new();
        // m_data->poly_context = lp_polynomial_context_new(0, m_data->lp_var_db, m_data->lp_var_order);
        for (size_t i = 0; i < carl_var_order.size(); i++) {
            std::string var_name = carl_var_order[i].name();
            lp_variable_t poly_var = lp_variable_db_new_variable(m_data->poly_context.get_variable_db(), &var_name[0]);
            lp_variable_order_push(m_data->poly_context.get_variable_order(), poly_var);
            CARL_LOG_TRACE("carl.poly", "Creating lp var for carl var " << carl_var_order[i] << " -> " << lp_variable_db_get_name(m_data->poly_context.get_variable_db(), poly_var));
            m_data->vars_carl_libpoly.emplace(carl_var_order[i], poly_var);
            m_data->vars_libpoly_carl.emplace(poly_var, carl_var_order[i]);
        }
    };

    poly::Context& poly_context() {
        return m_data->poly_context;
    }

    const poly::Context& poly_context() const {
        return m_data->poly_context;
    }

    lp_polynomial_context_t* lp_context() {
        return m_data->poly_context.get_polynomial_context();
    };

    const lp_polynomial_context_t* lp_context() const {
        return m_data->poly_context.get_polynomial_context();
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