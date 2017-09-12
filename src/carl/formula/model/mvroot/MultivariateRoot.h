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
 * This class represents a root of a multivariate polynomial with respect to some assignment for the other variables.
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
		assert(mK > 0);
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
		CARL_LOG_TRACE("carl.multivariateroot", "Got roots " << *roots);
		assert(roots->size() >= mK);
		auto it = roots->begin();
		std::advance(it, long(mK)-1);
		CARL_LOG_DEBUG("carl.multivariateroot", "Result is " << *it);
		return *it;
	}
	
	bool operator==(const MultivariateRoot& mr) const {
		return std::tie(mK, mPoly) == std::tie(mr.mK, mr.mPoly);
	}
	bool operator<(const MultivariateRoot& mr) const {
		return std::tie(mK, mPoly) < std::tie(mr.mK, mr.mPoly);
	}
};

template<typename Poly>
const Variable MultivariateRoot<Poly>::sVar = carl::VariablePool::getInstance().getFreshPersistentVariable("__z");

template<typename P>
std::ostream& operator<<(std::ostream& os, const MultivariateRoot<P>& mr) {
	return os << "root(" << mr.poly() << ", " << mr.k() << ", " << MultivariateRoot<P>::var() << " = " << mr.var().getId() << ")";
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
