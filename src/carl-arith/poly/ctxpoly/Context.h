#pragma once

#include "../../core/Variable.h"
#include <carl-common/util/streamingOperators.h>
#include <memory>

namespace carl {

class Context {
    std::shared_ptr<std::vector<Variable>> m_variable_order;

public:
    Context() = delete;

    Context(const Context& ctx) : m_variable_order(ctx.m_variable_order) {}

    Context(Context&& ctx) : m_variable_order(std::move(ctx.m_variable_order)) {}

    Context& operator=(const Context& rhs) {
        m_variable_order = rhs.m_variable_order;
        return *this;
    }

    Context(const std::vector<Variable>& var_order) : m_variable_order(std::make_shared<std::vector<Variable>>(var_order)) {};

    const std::vector<Variable>& variable_ordering() const {
        return *m_variable_order;
    }

    bool has(const Variable& var) const {
        return std::find(variable_ordering().begin(), variable_ordering().end(), var) != variable_ordering().end();
    };

    inline bool operator==(const Context& rhs) const {
        return m_variable_order == rhs.m_variable_order;
    }
};

inline std::ostream& operator<<(std::ostream& os, const Context& ctx) {
    os << ctx.variable_ordering();
    return os;
}

} // namespace carl