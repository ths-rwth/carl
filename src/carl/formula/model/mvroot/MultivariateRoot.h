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

namespace carl {

/**
 * This is basically a hack to represent a function f: algReal^k -> algReal;
 * from multi-dimensional point whose components are algebraic reals to an
 * algebraic real.
 * Think of this class as representing a real algebraic number (root of a
 * univariate poly; normally stored as a univariate poly and a number,
 * representing which of the possibly many roots this number is) that is stored
 * as a multivariate poly (and a distinguished main variable, and a root
 * number/index) where you first have to plug in values for all variables that
 * are not the main variable to get the normally used univariate poly).
 * Thus this algebraic real can vary depending on the values plugged in for the
 * other variables.
 */
template<typename Poly>
class MultivariateRoot {
public:
	using Number = typename UnderlyingNumberType<Poly>::type;
	using RAN = RealAlgebraicNumber<Number>;
	using EvalMap = RealAlgebraicNumberEvaluation::RANMap<Number>;
private:
	Poly mPoly;
	std::size_t mRootIndex;
	Variable mMainVariable;
public:
	MultivariateRoot(const Poly& p, std::size_t rootIndex, Variable mainVariable):
		mPoly(p), mRootIndex(rootIndex), mMainVariable(mainVariable)
	{
		assert(mRootIndex > 0);
	}

	const Poly& poly() const noexcept {
		return mPoly;
	}
	Poly poly(Variable var) const {
		return mPoly.substitute(mMainVariable, Poly(var));
	}
	std::size_t k() const noexcept {
		return mRootIndex;
	}
	Variable var() const noexcept {
		return mMainVariable;
	}
	bool isUnivariate() const {
		return mPoly.isUnivariate();
	}
	std::set<Variable> gatherVariables() const {
		Variables var = mPoly.gatherVariables();
		var.erase(mMainVariable);
		return var;
	}
	void substituteIn(Variable::Arg var, const Poly& poly) {
		mPoly.substituteIn(var, poly);
	}

	boost::optional<RAN> evaluate(const EvalMap& algebraicPoint) const {
		CARL_LOG_DEBUG("carl.multivariateroot", "Evaluating " << *this << " against " << algebraicPoint);
		auto poly = mPoly.toUnivariatePolynomial(mMainVariable);
		std::map<Variable, Interval<Number>> intervalMap;
		auto unipoly = RealAlgebraicNumberEvaluation::evaluateCoefficients(poly, algebraicPoint, intervalMap);
		CARL_LOG_TRACE("carl.multivariateroot", "Intermediate polynomial is " << unipoly);
		auto roots = rootfinder::realRoots(unipoly);
		CARL_LOG_TRACE("carl.multivariateroot", "Got roots " << roots);
		if (roots.size() < mRootIndex) return boost::none;
		assert(roots.size() >= mRootIndex);
		auto it = roots.begin();
		std::advance(it, long(mRootIndex)-1);
		CARL_LOG_DEBUG("carl.multivariateroot", "Result is " << *it);
		return *it;
	}

	bool operator==(const MultivariateRoot& mr) const {
		return mRootIndex == mr.mRootIndex && mMainVariable == mr.mMainVariable && mPoly == mr.mPoly;
	}
};

template<typename P>
std::ostream& operator<<(std::ostream& os, const MultivariateRoot<P>& mr) {
	return os << "root(" << mr.poly() << ", " << mr.k() << ", " << mr.var() << ")";
}

}
