#pragma once

#include "Context.h"
#include "../umvpoly/UnivariatePolynomial.h"
#include "../umvpoly/MultivariatePolynomial.h"

namespace carl {

template<typename Coeff, typename Ordering = GrLexOrdering, typename Policies = StdMultivariatePolynomialPolicies<>>
class ContextPolynomial {
public: 
    using ContextType = Context;
    /// Number type within the coefficients.
	using NumberType = typename UnderlyingNumberType<Coeff>::type;
    // RAN type
	using RootType = typename UnivariatePolynomial<NumberType>::RootType; 

private:
    Context m_context;
    UnivariatePolynomial<MultivariatePolynomial<Coeff, Ordering, Policies>> m_content;

    static Variable main_var(const Context& ctx, const MultivariatePolynomial<Coeff, Ordering, Policies>& p) {
        auto pvars = carl::variables(p).as_vector();
        if (pvars.empty()) return carl::Variable::NO_VARIABLE;
        for (const auto& v : pvars) assert(std::find(ctx.variable_ordering().begin(), ctx.variable_ordering().end(), v) != ctx.variable_ordering().end());
        std::sort(pvars.begin(), pvars.end());
        for (auto it = ctx.variable_ordering().rbegin(); it != ctx.variable_ordering().rend(); it++) {
            if (binary_search(pvars.begin(), pvars.end(), *it)) return *it;
        }
        assert(false);
        return carl::Variable::NO_VARIABLE;
    }

public:
    ContextPolynomial(const Context& context, const MultivariatePolynomial<Coeff, Ordering, Policies>& p) : m_context(context), m_content(carl::to_univariate_polynomial(p, main_var(context, p))) {}

    ContextPolynomial(const Context& context, const UnivariatePolynomial<MultivariatePolynomial<Coeff, Ordering, Policies>>& p) : m_context(context), m_content(p) {}

    ContextPolynomial(Context&& context, UnivariatePolynomial<MultivariatePolynomial<Coeff, Ordering, Policies>>&& p) : m_context(std::move(context)), m_content(std::move(p)) {}

    ContextPolynomial(const Context& context, const Coeff& c) : ContextPolynomial(context, MultivariatePolynomial<Coeff, Ordering, Policies>(c)) {}

    operator MultivariatePolynomial<Coeff, Ordering, Policies> () const {
		return MultivariatePolynomial<Coeff, Ordering, Policies>(m_content);
	}

    operator const UnivariatePolynomial<MultivariatePolynomial<Coeff, Ordering, Policies>>& () const {
		return m_content;
	}

    operator UnivariatePolynomial<MultivariatePolynomial<Coeff, Ordering, Policies>>& () {
		return m_content;
	}

    const UnivariatePolynomial<MultivariatePolynomial<Coeff, Ordering, Policies>>& content() const {
		return m_content;
	}

    MultivariatePolynomial<Coeff, Ordering, Policies> as_multivariate() const {
		return MultivariatePolynomial<Coeff, Ordering, Policies>(m_content);
	}

    const Context& context() const {
        return m_context;
    }

    auto main_var() const {
		return m_content.main_var();
	}

    auto degree() const {
        return m_content.degree();
    }

    auto coefficients() const {
        std::vector<ContextPolynomial<Coeff, Ordering, Policies>> coeffs;
        for (const auto& coeff :  m_content.coefficients()) {
            coeffs.emplace_back(m_context, coeff);
        }
        return coeffs;
    }

    auto lcoeff() const {
        return ContextPolynomial<Coeff, Ordering, Policies>(m_context, m_content.lcoeff());
    }

    auto normalized() const {
        return ContextPolynomial<Coeff, Ordering, Policies>(m_context, as_multivariate().normalize());
    }

    auto constant_part() const {
        return m_content.constant_part();
    }
};

template<typename Coeff, typename Ordering, typename Policies>
struct needs_context_type<ContextPolynomial<Coeff, Ordering, Policies>> : std::true_type {};

template<typename Coeff, typename Ordering, typename Policies>
struct is_polynomial_type<ContextPolynomial<Coeff, Ordering, Policies>>: std::true_type {};

template<typename Coeff, typename Ordering, typename Policies>
inline bool is_constant(const ContextPolynomial<Coeff, Ordering, Policies>& p) {
    return is_constant(p.content());
}

template<typename Coeff, typename Ordering, typename Policies>
inline bool is_zero(const ContextPolynomial<Coeff, Ordering, Policies>& p) {
    return is_zero(p.content());
}

template<typename Coeff, typename Ordering, typename Policies>
inline bool is_linear(const ContextPolynomial<Coeff, Ordering, Policies>& p) {
    return is_linear(p.content());
}

template<typename Coeff, typename Ordering, typename Policies>
inline bool is_number(const ContextPolynomial<Coeff, Ordering, Policies>& p) {
    return is_constant(p);
}

template<typename Coeff, typename Ordering, typename Policies>
inline std::size_t level_of(const ContextPolynomial<Coeff, Ordering, Policies>& p) {
	if (is_number(p)) return 0;
	auto it = std::find(p.context().variable_ordering().begin(), p.context().variable_ordering().end(), p.main_var());
	assert(it != p.context().variable_ordering().end());
	return std::distance(p.context().variable_ordering().begin(), it)+1;
}

template<typename Coeff, typename Ordering, typename Policies>
inline void variables(const ContextPolynomial<Coeff, Ordering, Policies>& p, carlVariables& vars) {
	variables(p.content(), vars);
}

template<typename Coeff, typename Ordering, typename Policies>
bool operator<(const ContextPolynomial<Coeff, Ordering, Policies>& lhs, const ContextPolynomial<Coeff, Ordering, Policies>& rhs) {
    return lhs.content() < rhs.content();
}

template<typename Coeff, typename Ordering, typename Policies>
bool operator==(const ContextPolynomial<Coeff, Ordering, Policies>& lhs, const ContextPolynomial<Coeff, Ordering, Policies>& rhs) {
    return lhs.content() == rhs.content();
}

template<typename Coeff, typename Ordering, typename Policies>
std::ostream& operator<<(std::ostream& os, const ContextPolynomial<Coeff, Ordering, Policies>& rhs) {
    return operator<<(os, rhs.content());
}



}

namespace std {
template<typename Coeff, typename Ordering, typename Policies>
struct hash<carl::ContextPolynomial<Coeff, Ordering, Policies>> {
	std::size_t operator()(const carl::ContextPolynomial<Coeff, Ordering, Policies>& p) const {
		return std::hash<carl::UnivariatePolynomial<carl::MultivariatePolynomial<Coeff, Ordering, Policies>>>()(p.content());
	}
};

} // namespace std