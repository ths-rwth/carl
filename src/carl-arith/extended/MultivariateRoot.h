#pragma once

#include <carl-logging/carl-logging.h>
#include <carl-arith/poly/umvpoly/functions/Substitution.h>
#include <carl-arith/core/Variable.h>
#include <carl-arith/numbers/numbers.h>
#include <carl-arith/ran/ran.h>


#include <optional>
#include <algorithm>
#include <iostream>
#include <tuple>

namespace carl {

/**
 * @file
 * Represent a dynamic root, also known as a "root expression".
 * It's a generalization of an algebraic real(= univariate poly having roots + root index).
 * It uses a multivariate poly with distinguished, root-variable "_z"
 * and an root index. If we want to "evaluate" this root, we need to
 * plug in a subpoint with algebraic-real values for all variables that are not "_z".
 * The result is basically an algebraic real.
 * And since this algebraic real depends on the subpoint,
 * you can also think of this as a multidimensional scalar function:
 * A root(p(x1,x2,_z), 1) value represents a function f(x1,x2) of algReal^2 to
 * an algebraic real. For example, f(a,b)= root(q(_z),1), where the resulting
 * algebraic real is the 1. root of poly q(_z) :=  p(a,b,_z) after pluggin a and b into poly p.
 */
template<typename Poly>
class MultivariateRoot {
public:
	using Number = typename UnderlyingNumberType<Poly>::type;
	using RAN = typename Poly::RootType;
private:
	/// Polynomial defining this root.
	Poly m_poly;
	/// Specifies which root to consider.
	std::size_t m_k;
	/// The main variable.
	Variable m_var;
public:
	/**
	 * @param poly Must mention the root-variable "_z" and
	 * should have a at least 'rootIdx'-many roots in "_z" at each subpoint
	 * where it is intended to be evaluated.
	 * @param k The index of the root of the polynomial in "_z".
	 * The first root has index 1, the second has index 2 and so on.
	 */
	MultivariateRoot(const Poly& poly, std::size_t k, Variable var): m_poly(poly), m_k(k), m_var(var) {
		assert(m_k > 0);
	}

	MultivariateRoot(): m_poly(), m_k(0), m_var() {}

	/**
	 * Return k, the index of the root.
	 */
	std::size_t k() const noexcept {
		return m_k;
	}

	/**
	 * @return the raw underlying polynomial that still mentions the root-variable "_z".
	 */
	const Poly& poly() const noexcept {
		return m_poly;
	}

	/**
	 * @return the raw underlying polynomial that still mentions the root-variable "_z".
	 */
	Poly& poly() noexcept {
		return m_poly;
	}

	/**
	 * @return The globally-unique distinguished root-variable "_z"
	 * to allow you to build a polynomial with this variable yourself.
	 */
	Variable var() const noexcept {
		return m_var;
	}

	bool is_univariate() const {
		return m_poly.is_univariate();
	}

	template<typename P>
	friend std::optional<typename MultivariateRoot<P>::RAN> evaluate(const MultivariateRoot<P>& mr, const carl::Assignment<typename MultivariateRoot<P>::RAN>& m);
};

template<typename Poly>
inline bool operator==(const MultivariateRoot<Poly>& lhs, const MultivariateRoot<Poly>& rhs) {
	return std::forward_as_tuple(lhs.var(), lhs.k(), lhs.poly()) == std::forward_as_tuple(lhs.var(), rhs.k(), rhs.poly());
}
template<typename Poly>
inline bool operator<(const MultivariateRoot<Poly>& lhs, const MultivariateRoot<Poly>& rhs) {
	return std::forward_as_tuple(lhs.var(), lhs.k(), lhs.poly()) < std::forward_as_tuple(lhs.var(), rhs.k(), rhs.poly());
}

template<typename P>
std::ostream& operator<<(std::ostream& os, const MultivariateRoot<P>& mr) {
	return os << "rootExpr(" << mr.poly() << ", " << mr.k() << ", " << mr.var() << ")";
}

/**
 * Add the variables mentioned in underlying polynomial, excluding
 * the root-variable "_z". For example, with an underlying poly p(x,y,_z)
 * we return {x,y}.
 */
template<typename Poly>
void variables(const MultivariateRoot<Poly>& mr, carlVariables& vars) {
	bool hadVar = vars.has(mr.var());
	carl::variables(mr.poly(), vars);
	if (!hadVar) vars.erase(mr.var());
}

/**
 * Create a copy of the underlying polynomial with the given variable
 * replaced by the given polynomial.
 */
template<typename Poly>
void substitute_inplace(MultivariateRoot<Poly>& mr, Variable var, const Poly& poly) {
	carl::substitute_inplace(mr.poly(), var, poly);
}

template<typename Poly>
MultivariateRoot<Poly> convert_to_mvroot(const typename MultivariateRoot<Poly>::RAN& ran, Variable var) {
	auto k = [&](){
		if (ran.is_numeric()) {
			return (std::size_t)1;
		} else {
			auto roots = carl::real_roots(ran.polynomial());
			auto it = std::find(roots.roots().begin(), roots.roots().end(), ran);
			assert(it != roots.roots().end());
			return (std::size_t)std::distance(roots.roots().begin(), it)+1;
		}
    }();
	return MultivariateRoot<Poly>(Poly(switch_main_variable(ran.polynomial(), var)), k, var);
}

/**
 * Return the emerging algebraic real after pluggin in a subpoint to replace
 * all variables with algebraic reals that are not the root-variable "_z".
 * @param m must contain algebraic real assignments for all variables that are not "_z".
 * @return std::nullopt if the underlying polynomial has no root with index 'rootIdx' at
 * the given subpoint.
 */
template<typename Poly>
std::optional<typename MultivariateRoot<Poly>::RAN> evaluate(const MultivariateRoot<Poly>& mr, const carl::Assignment<typename MultivariateRoot<Poly>::RAN>& m) {
	CARL_LOG_DEBUG("carl.rootexpression", "Evaluate: " << mr << " against: " << m);
	
	auto result = [&](){
		if constexpr(needs_context_type<Poly>::value) {
			return carl::real_roots(mr.poly(), m);
		} else {
			return carl::real_roots(carl::to_univariate_polynomial(mr.poly(), mr.var()), m);
		}
	}();
	if (!result.is_univariate()) {
		CARL_LOG_TRACE("carl.rootexpression", mr.poly() << " is not univariate (nullified: " << result.is_nullified() << "; non-univariate: " << result.is_non_univariate() << ").");
		return std::nullopt;
	}
	CARL_LOG_DEBUG("carl.rootexpression", "Roots: " << result.roots());
	if (result.roots().size() < mr.k()) {
		CARL_LOG_TRACE("carl.rootexpression", mr.k() << "th root does not exist.");
		return std::nullopt;
	}
	CARL_LOG_TRACE("carl.rootexpression", "Take " << mr.k() << "th of isolated roots " << result.roots());
	assert(result.roots().size() >= mr.k());
	assert(mr.k() > 0);
	CARL_LOG_DEBUG("carl.rootexpression", "Result is " << result.roots()[mr.k()-1]);
	return result.roots()[mr.k()-1];
}

}

namespace std {
	template<typename Pol>
	struct hash<carl::MultivariateRoot<Pol>> {
		std::size_t operator()(const carl::MultivariateRoot<Pol>& mv) const {
			return carl::hash_all(mv.poly(), mv.k());
		}
	};
}
