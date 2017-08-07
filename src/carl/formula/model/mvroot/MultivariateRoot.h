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

template<typename Poly>
class MultivariateRoot {
public:
	using Number = typename UnderlyingNumberType<Poly>::type;
	using RAN = RealAlgebraicNumber<Number>;
	using EvalMap = RealAlgebraicNumberEvaluation::RANMap<Number>;
private:
	Poly mPoly;
	std::size_t mK;
	Variable mVar;
public:
	MultivariateRoot(const Poly& p, std::size_t k, Variable v):
		mPoly(p), mK(k), mVar(v)
	{
		assert(mK > 0);
	}
	
	const Poly& poly() const noexcept {
		return mPoly;
	}
	Poly poly(Variable var) const {
		return mPoly.substitute(mVar, Poly(var));
	}
	std::size_t k() const noexcept {
		return mK;
	}
	Variable var() const noexcept {
		return mVar;
	}
	bool isUnivariate() const {
		return mPoly.isUnivariate();
	}
	std::set<Variable> gatherVariables() const {
		Variables var = mPoly.gatherVariables();
		var.erase(mVar);
		return var;
	}
	void substituteIn(Variable::Arg var, const Poly& poly) {
		mPoly.substituteIn(var, poly);
	}
	
	boost::optional<RAN> evaluate(const EvalMap& m) const {
		CARL_LOG_DEBUG("carl.multivariateroot", "Evaluating " << *this << " against " << m);
		auto poly = mPoly.toUnivariatePolynomial(mVar);
		std::map<Variable, Interval<Number>> intervalMap;
		auto unipoly = RealAlgebraicNumberEvaluation::evaluateCoefficients(poly, m, intervalMap);
		CARL_LOG_TRACE("carl.multivariateroot", "Intermediate polynomial is " << unipoly);
		auto roots = rootfinder::realRoots(unipoly);
		CARL_LOG_TRACE("carl.multivariateroot", "Got roots " << roots);
		if (roots.size() < mK) return boost::none;
		assert(roots.size() >= mK);
		auto it = roots.begin();
		std::advance(it, long(mK)-1);
		CARL_LOG_DEBUG("carl.multivariateroot", "Result is " << *it);
		return *it;
	}
	
	bool operator==(const MultivariateRoot& mr) const {
		return mK == mr.mK && mVar == mr.mVar && mPoly == mr.mPoly;
	}
};

template<typename P>
std::ostream& operator<<(std::ostream& os, const MultivariateRoot<P>& mr) {
	return os << "root(" << mr.poly() << ", " << mr.k() << ", " << mr.var() << ")";
}

}
