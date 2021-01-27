#pragma once

#include "../../../core/logging.h"
#include "../../../core/polynomialfunctions/Substitution.h"
#include "../../../core/Variable.h"
#include "../../../numbers/numbers.h"
#include "../../../ran/ran.h"
#include "../../../ran/real_roots.h"

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
	using RAN = RealAlgebraicNumber<Number>;
	using EvalMap = ran::RANMap<Number>;
private:
  /**
   * Distinguished, globally unique root-variable
   */
	static const Variable sVar;
	/// Polynomial defining this root.
	Poly mPoly;
	/// Specifies which root to consider.
	std::size_t mK;
public:
	/**
	 * @param poly Must mention the root-variable "_z" and
	 * should have a at least 'rootIdx'-many roots in "_z" at each subpoint
	 * where it is intended to be evaluated.
	 * @param k The index of the root of the polynomial in "_z".
	 * The first root has index 1, the second has index 2 and so on.
	 */
	MultivariateRoot(const Poly& poly, std::size_t k): mPoly(poly), mK(k) {
		assert(mK > 0);
	}

	/**
	 * Return k, the index of the root.
	 */
	std::size_t k() const noexcept {
		return mK;
	}

	/**
	 * @return the raw underlying polynomial that still mentions the root-variable "_z".
	 */
	const Poly& poly() const noexcept {
		return mPoly;
	}

	/**
	 * @return A copy of the underlying polynomial with the
	 * root-variable replaced by the given variable.
	 */
	Poly poly(Variable var) const {
		return carl::substitute(mPoly, sVar, Poly(var));
	}

	/**
	 * @return The globally-unique distinguished root-variable "_z"
	 * to allow you to build a polynomial with this variable yourself.
	 */
	static Variable var() noexcept {
		return sVar;
	}

	bool isUnivariate() const {
		return mPoly.isUnivariate();
	}

	/**
	 * @return The variables mentioned in underlying polynomial, excluding
	 * the root-variable "_z". For example, with an underlying poly p(x,y,_z)
	 * we return {x,y}.
	 */
	[[deprecated("Use carl::variables() instead.")]]
	std::set<Variable> gatherVariables() const {
		Variables var = mPoly.gatherVariables();
		var.erase(sVar);
		return var;
	}
	[[deprecated("Use carl::variables() instead.")]]
	void gatherVariables(carlVariables& vars) const {
		carl::variables(mPoly, vars);
		vars.erase(sVar);
	}

	/**
	 * Create a copy of the underlying polynomial with the given variable
	 * replaced by the given polynomial.
	 */
	void substituteIn(Variable var, const Poly& poly) {
		carl::substitute_inplace(mPoly, var, poly);
	}

	/**
	 * Return the emerging algebraic real after pluggin in a subpoint to replace
	 * all variables with algebraic reals that are not the root-variable "_z".
	 * @param m must contain algebraic real assignments for all variables that are not "_z".
	 * @return std::nullopt if the underlying polynomial has no root with index 'rootIdx' at
	 * the given subpoint.
	 */
	std::optional<RAN> evaluate(const EvalMap& m) const {
		CARL_LOG_DEBUG("carl.rootexpression", "Evaluate: " << *this << " against: " << m);
		auto poly = carl::to_univariate_polynomial(mPoly, sVar);
		auto result = carl::real_roots(poly, m);
		if (!result.is_univariate()) {
			CARL_LOG_TRACE("carl.rootexpression", poly << " is not univariate (nullified: " << result.is_nullified() << "; non-univariate: " << result.is_non_univariate() << ").");
			return std::nullopt;
		}
		CARL_LOG_DEBUG("carl.rootexpression", "Roots: " << result.roots());
		if (result.roots().size() < mK) {
			CARL_LOG_TRACE("carl.rootexpression", mK << "th root does not exist.");
			return std::nullopt;
		}
		CARL_LOG_TRACE("carl.rootexpression", "Take " << mK << "th of isolated roots " << result.roots());
		assert(result.roots().size() >= mK);
		assert(mK > 0);
		CARL_LOG_DEBUG("carl.rootexpression", "Result is " << result.roots()[mK-1]);
		return result.roots()[mK-1];
	}
};

template<typename Poly>
const Variable MultivariateRoot<Poly>::sVar = carl::VariablePool::getInstance().getFreshPersistentVariable("__z");

template<typename Poly>
inline bool operator==(const MultivariateRoot<Poly>& lhs, const MultivariateRoot<Poly>& rhs) {
	return std::forward_as_tuple(lhs.k(), lhs.poly()) == std::forward_as_tuple(rhs.k(), rhs.poly());
}
template<typename Poly>
inline bool operator<(const MultivariateRoot<Poly>& lhs, const MultivariateRoot<Poly>& rhs) {
	return std::forward_as_tuple(lhs.k(), lhs.poly()) < std::forward_as_tuple(rhs.k(), rhs.poly());
}

template<typename P>
std::ostream& operator<<(std::ostream& os, const MultivariateRoot<P>& mr) {
	return os << "rootExpr(" << mr.poly() << ", " << mr.k() << ", " << MultivariateRoot<P>::var() << ")";
}

/**
 * Add the variables mentioned in underlying polynomial, excluding
 * the root-variable "_z". For example, with an underlying poly p(x,y,_z)
 * we return {x,y}.
 */
template<typename Poly>
void variables(const MultivariateRoot<Poly>& mr, carlVariables& vars) {
	carl::variables(mr.poly(), vars);
	vars.erase(mr.var());
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
