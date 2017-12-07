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
 * Represent what is called an "indexed root expression".
 * Think of this as a algebraic real(= univariate poly + root index) that
 * replaces the univariate poly with a multivariate poly and a distinguished main variable.
 * You first have to plug in values for all non-main variables
 * to get univariate poly that only mentions the main variable.
 * When you do this, you basically end up with an algebraic real.
 * However, this algebraic real can vary depending on the values plugged in
 * for the non-main variables.
 * That's why you can also think of this as a function f: algReal^k -> algReal;
 * that maps multi-dimensional point (whose components are algebraic reals) to an
 * algebraic real.
 */
template<typename Poly>
class MultivariateRoot {
public:
	using Number = typename UnderlyingNumberType<Poly>::type;
	using RAN = RealAlgebraicNumber<Number>;
	using EvalMap = RealAlgebraicNumberEvaluation::RANMap<Number>;
private:
	static const Variable sVar;
	Poly mPoly;
	std::size_t mK;
public:
	MultivariateRoot(const Poly& p, std::size_t k): mPoly(p), mK(k)
	{
		assert(mRootIndex > 0);
	}

	std::size_t k() const noexcept {
		return mK;
	}

	const Poly& poly() const noexcept {
		return mPoly;
	}

	Poly poly(Variable var) const {
		return mPoly.substitute(sVar, Poly(var));
	}
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
	void substituteIn(Variable var, const Poly& poly) {
		mPoly.substituteIn(var, poly);
	}

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
	return os << "root(" << mr.poly() << ", " << mr.k() << ", " << MultivariateRoot<P>::var() << ")";
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
