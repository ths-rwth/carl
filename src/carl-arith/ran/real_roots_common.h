#pragma once

#include <type_traits>
#include <variant>
#include <vector>

namespace carl::ran {

template<typename RAN /*, typename = std::enable_if_t<is_ran<RAN>::value> */>
class real_roots_result {

public:
    using roots_t = std::vector<RAN>;

private:
    struct nullified_t {};
    struct non_univariate_t {};
    std::variant<roots_t, nullified_t, non_univariate_t> m_data;

    real_roots_result(nullified_t)
        : m_data(nullified_t{}){};
    real_roots_result(non_univariate_t)
        : m_data(non_univariate_t{}){};
    real_roots_result(roots_t&& real_roots)
        : m_data(std::move(real_roots)){};

public:
    static real_roots_result nullified_response() {
        return real_roots_result(nullified_t{});
    }
    static real_roots_result non_univariate_response() {
        return real_roots_result(non_univariate_t{});
    }
    static real_roots_result roots_response(roots_t&& real_roots) {
        return real_roots_result(std::move(real_roots));
    }
    static real_roots_result no_roots_response() {
        return real_roots_result(std::vector<RAN>({}));
    }

    bool is_nullified() const {
        return std::holds_alternative<nullified_t>(m_data);
    }

    bool is_univariate() const {
        return std::holds_alternative<roots_t>(m_data);
    }

    bool is_non_univariate() const {
        return std::holds_alternative<non_univariate_t>(m_data);
    }

    const roots_t& roots() const {
        return std::get<roots_t>(m_data);
    }
};

} // namespace carl::ran