#pragma once

#include "../../core/Variable.h"
#include <carl-common/util/streamingOperators.h>
#include <memory>

namespace carl {

class Context {
    std::shared_ptr<std::vector<Variable>> m_variable_order;

public:
    Context() = delete;

    Context(const Context& rhs) {
        m_variable_order = rhs.m_variable_order;
    }

    Context& operator=(const Context& rhs) {
        m_variable_order = rhs.m_variable_order;
        return *this;
    }

    Context(const std::vector<Variable>& varOrder) : m_variable_order(std::make_shared<std::vector<Variable>>(varOrder)) {};

    const std::vector<Variable>& variable_order() const {
        return *m_variable_order;
    }

    bool has(const Variable& var) const {
        return std::find(variable_order().begin(), variable_order().end(), var) != variable_order().end();
    };

    inline bool operator==(const Context& rhs) const {
        return m_variable_order == rhs.m_variable_order;
    }
};

inline std::ostream& operator<<(std::ostream& os, const Context& ctx) {
    os << ctx.variable_order();
    return os;
}

} // namespace carl