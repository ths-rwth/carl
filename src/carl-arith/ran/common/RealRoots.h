#pragma once

#include <type_traits>
#include <variant>
#include <vector>

namespace carl {

template<typename RAN /*, typename = std::enable_if_t<is_ran_type<RAN>::value> */>
class RealRootsResult {

public:
    using roots_t = std::vector<RAN>;

private:
    struct nullified_t {};
    struct non_univariate_t {};
    std::variant<roots_t, nullified_t, non_univariate_t> m_data;

    RealRootsResult(nullified_t)
        : m_data(nullified_t{}){};
    RealRootsResult(non_univariate_t)
        : m_data(non_univariate_t{}){};
    RealRootsResult(roots_t&& real_roots)
        : m_data(std::move(real_roots)){};

public:
    static RealRootsResult nullified_response() {
        return RealRootsResult(nullified_t{});
    }
    static RealRootsResult non_univariate_response() {
        return RealRootsResult(non_univariate_t{});
    }
    static RealRootsResult roots_response(roots_t&& real_roots) {
        return RealRootsResult(std::move(real_roots));
    }
    static RealRootsResult no_roots_response() {
        return RealRootsResult(std::vector<RAN>({}));
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

}