#pragma once

#include "logging.h"
#include "RealAlgebraicNumber.h"
#include "RealAlgebraicNumberEvaluation.h"
#include "Variable.h"

#include "rootfinder/RootFinder.h"

#include "../numbers/numbers.h"

#include <algorithm>
#include <iostream>

namespace carl {

template<typename Poly>
class MultivariateRoot {
public:
	using Number = typename carl::UnderlyingNumberType<Poly>::type;
	using RAN = RealAlgebraicNumber<Number>;
	using EvalMap = RealAlgebraicNumberEvaluation::RANMap<Number>;
private:
	Poly mPoly;
	std::size_t mK;
	Variable mVar;
	mutable EvalMap mEvalMap;
	mutable RAN mRAN;
public:
	MultivariateRoot(const Poly& p, std::size_t k, Variable::Arg v):
		mPoly(p), mK(k), mVar(v), mEvalMap(), mRAN(0, false)
	{}
	
	const Poly& poly() const {
		return mPoly;
	}
	std::size_t k() const {
		return mK;
	}
	Variable::Arg var() const {
		return mVar;
	}
	
	const RAN& evaluate(const EvalMap& m) const {
		if (mEvalMap == m && mRAN.isRoot()) return mRAN;
		CARL_LOG_DEBUG("carl.multivariateroot", "Evaluating " << *this << " against " << m);
		mEvalMap = m;
		auto poly = mPoly.toUnivariatePolynomial(mVar);
		std::map<Variable, Interval<Number>> intervalMap;
		auto unipoly = RealAlgebraicNumberEvaluation::evaluateCoefficients(poly, mEvalMap, intervalMap);
		auto roots = rootfinder::realRoots(unipoly);
		assert(roots.size() >= mK);
		auto it = roots.begin();
		std::advance(it, mK-1);
		mRAN = *it;
		CARL_LOG_DEBUG("carl.multivariateroot", "Result is " << mRAN);
		return mRAN;
	}
};

template<typename P>
friend std::ostream& operator<<(std::ostream& os, const MultivariateRoot<P>& mr) {
	return os << "root(" << mr.poly() << ", " << mr.k() << ", " << mr.var() << ")";
}

}
