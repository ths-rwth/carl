#pragma once

#include "../../../core/logging.h"
#include "../../../core/rootfinder/RootFinder.h"
#include "../../../core/Variable.h"
#include "../../../numbers/numbers.h"
#include "../ran/RealAlgebraicNumber.h"
#include "../ran/RealAlgebraicNumberEvaluation.h"

#include <boost/optional.hpp>

#include <algorithm>
#include <iostream>
#include <tuple>

namespace carl {

/**
 * @file
 * Represent a dynamic root, also known as an "indexed root expression".
 * It's a generalization of an algebraic real(= univariate poly having roots + root index).
 * It uses a multivariate poly(with distinguished main/substitution variable)
 * and an root index. If we want to "evaluate" this root, we need to
 * plug in a subpoint, i.e., algebraic-real values for all non-main variables.
 * The result is basically an algebraic real.
 * However, since this algebraic real depends on the subpoint,
 * you can also think of this as a multi-argument function f: algReal^k -> algReal.
 */
template<typename Poly>
class MultivariateRoot {
public:
	using Number = typename UnderlyingNumberType<Poly>::type;
	using RAN = RealAlgebraicNumber<Number>;
	using EvalMap = RealAlgebraicNumberEvaluation::RANMap<Number>;
private:
  /**
   * Distinguished main/substitution variable, intended to be SMTRAT/globally
   * unique.
   */
	static const Variable sVar;
	Poly mPoly;
	std::size_t mK; // also called rootId, rootNumber or rootIndex
public:
  /**
   * @param poly Must mention the main/substituion variable and
   * should have a at least 'rootIdx'-many roots at each subpoint
   * where it is intended to be evaluated. We have no way to ensure/check that.
   * @param rootIdx Must be a positive number, because we count from 1, i.e.,
   * first root has index 1.
   */
	MultivariateRoot(const Poly& poly, std::size_t rootIdx): mPoly(poly), mK(rootIdx)
	{
		assert(rootIdx > 0);
	}

	/**
	 * Return the rootIndex, also know as k, rootNumber, rootId
	 */
	std::size_t k() const noexcept {
		return mK;
	}

	const Poly& poly() const noexcept {
		return mPoly;
	}

	/**
	 * Create of copy of the underlying polynomial with the
	 * main/substitution variable replaced by the given variable.
	 */
	Poly poly(Variable var) const {
		return mPoly.substitute(sVar, Poly(var));
	}

	/**
	 * @return The globally unique distinguished main/substitution variable
	 * to allow you to build a polynomial with this variable yourself.
	 */
	static Variable var() noexcept {
		return sVar;
	}

	bool isUnivariate() const {
		return mPoly.isUnivariate();
	}

	std::set<Variable> gatherVariables() const {
		Variables var = mPoly.gatherVariables();
		var.erase(sVar);
		return var;
	}

	/**
	 * Create a copy of the underlying polynomial with the given variable
	 * replaced by the given polynomial.
	 */
	void substituteIn(Variable var, const Poly& poly) {
		mPoly.substituteIn(var, poly);
	}

	/**
	 * Return the emerging algebraic real after pluggin in a subpoint, i.e.,
	 * replacing all non-main/substitution variables with algebraic reals.
	 * @param m must contain algebraic real assignments for all non-main
	 * variables.
	 * @return boost::none if the underlying polynomial has no 'rootIdx'th at
	 * the given subpoint.
	 */
	boost::optional<RAN> evaluate(const EvalMap& m) const {
		CARL_LOG_DEBUG("carl.multivariateroot", "Evaluating " << *this << " against " << m);
		auto poly = mPoly.toUnivariatePolynomial(sVar);
		auto roots = rootfinder::realRoots(poly, m);
		if (!roots || (roots->size() < mK)) {
			CARL_LOG_TRACE("carl.multivariateroot", mK << "th root does not exist.");
			return boost::none;
		}
		CARL_LOG_TRACE("carl.multivariateroot", "Taking " << mK << " of " << *roots << " roots");
		assert(roots->size() >= mK);
		assert(mK > 0);
		CARL_LOG_DEBUG("carl.multivariateroot", "Result is " << (*roots)[mK-1]);
		return (*roots)[mK-1];
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
	return os << "multivarRoot(" << mr.poly() << ", " << mr.k() << ", " << MultivariateRoot<P>::var() << ")";
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
