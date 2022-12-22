#pragma once

namespace carl {

template<typename CoeffType>
class VarInfo {

    /// Maximal degree variable occurs with.
    std::size_t m_max_degree = 0;
    /// Minimal non-zero degree variable occurs with.
    std::size_t m_min_degree = 0;
    /// Number of terms a variable occurs in.
    std::size_t m_num_occurences = 0;
    /// Coefficients of the variable. Maps from exponent to the coefficient.
    std::map<std::size_t, CoeffType> m_coeffs;

public:
    VarInfo() = default;
    // VarInfo(const VarInfo& varInfo) = default;
    VarInfo(std::size_t maxDegree, std::size_t min_degree, std::size_t occurence, std::map<std::size_t, CoeffType>&& coeffs) : m_max_degree(maxDegree), m_min_degree(min_degree), m_num_occurences(occurence), m_coeffs(coeffs) {}

    bool has_coeff() const {
        return !m_coeffs.empty();
    }

    std::size_t max_degree() const {
        return m_max_degree;
    }

    std::size_t min_degree() const {
        return m_min_degree;
    }

    std::size_t num_occurences() const {
        return m_num_occurences;
    }

    const std::map<std::size_t, CoeffType>& coeffs() const {
        return m_coeffs;
    }


    void raise_max_degree(std::size_t degree) {
        if (m_max_degree < degree) {
            m_max_degree = degree;
        }
    }

    void lower_min_degree(std::size_t degree) {
        if (m_min_degree == 0 || m_min_degree > degree) {
            m_min_degree = degree;
        }
    }

    void increase_num_occurences() {
        ++m_num_occurences;
    }

    template<typename Term>
    void update_coeff(std::size_t exponent, const Term& t) {
        auto it = m_coeffs.find(exponent);
        if (it == m_coeffs.end()) {
            m_coeffs.emplace(exponent, CoeffType(t));
        } else {
            it->second += t;
        }
    }

};

template<typename CoeffType>
class VarsInfo {
    std::map<Variable, VarInfo<CoeffType>> m_data;

public:
    VarInfo<CoeffType>& var(Variable var) {
        if (m_data.find(var) == m_data.end()) {
            m_data.emplace(var, VarInfo<CoeffType>());
        }
        return m_data[var];
    }
    const VarInfo<CoeffType>& var(Variable var) const {
        return m_data.at(var);
    }
    bool occurs(Variable var) const {
        return m_data.find(var) != m_data.end();
    }

    auto& data() {
        return m_data;
    }

    auto cbegin() const {
        return m_data.begin();
    }
    
    auto cend() const {
        return m_data.end();
    }
    
    auto begin() {
        return m_data.begin();
    }
    
    auto end() {
        return m_data.end();
    }
};

}
