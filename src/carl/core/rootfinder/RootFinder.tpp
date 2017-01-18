/**
 * @file RootFinder.tpp
 * @ingroup rootfinder
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */


#pragma once

#include "RootFinder.h"

#include "../../formula/model/ran/RealAlgebraicNumberEvaluation.h"

namespace carl {
namespace rootfinder {

        
// hiervon eine thom version machen!!!
template<typename Coeff, typename Number>
std::list<RealAlgebraicNumber<Number>> realRoots(
		const UnivariatePolynomial<Coeff>& p,
		const std::map<Variable, RealAlgebraicNumber<Number>>& m,
		const Interval<Number>& interval,
		SplittingStrategy pivoting
) {
	CARL_LOG_FUNC("carl.core.rootfinder", p << " in " << p.mainVar() << ", " << m << ", " << interval);
	assert(m.count(p.mainVar()) == 0);
	
	if (p.isZero()) {
		CARL_LOG_TRACE("carl.core.rootfinder", "p is 0 -> sampling " << interval.sample());
		return { RealAlgebraicNumber<Number>(interval.sample(), true) };
	}
	if (p.isConstant()) {
		CARL_LOG_TRACE("carl.core.rootfinder", "p is constant but not zero -> no root");
		return {};
	}
	
	UnivariatePolynomial<Coeff> tmp(p);
	std::map<Variable, RealAlgebraicNumber<Number>> IRmap;
	
	for (Variable v: tmp.gatherVariables()) {
		if (v == p.mainVar()) continue;
		assert(m.count(v) > 0);
		if (m.at(v).isNumeric()) {
			tmp.substituteIn(v, Coeff(m.at(v).value()));
		} else {
			IRmap.emplace(v, m.at(v));
		}
	}
	if (IRmap.empty()) {
		return realRoots(tmp, interval, pivoting);
	} else {
		CARL_LOG_FUNC("carl.core.rootfinder", p << " in " << p.mainVar() << ", " << m << ", " << interval);
		std::map<Variable, Interval<Number>> varToInterval;
		UnivariatePolynomial<Number> res = RealAlgebraicNumberEvaluation::evaluateCoefficients(tmp, IRmap, varToInterval);
		CARL_LOG_FUNC("carl.core.rootfinder", "Calling on " << res);
		return realRoots(res, interval, pivoting);
	}
}

template<typename Coeff, typename Number>
std::list<RealAlgebraicNumber<Number>> realRoots(
		const UnivariatePolynomial<Coeff>& p,
		const std::list<Variable>& variables,
		const std::list<RealAlgebraicNumber<Number>>& values,
		const Interval<Number>& interval,
		SplittingStrategy pivoting
) {
	std::map<Variable, RealAlgebraicNumber<Number>> m;
	
	assert(variables.size() == values.size());
	auto varit = variables.begin();
	auto valit = values.begin();
	while (varit != variables.end()) {
		m[*varit] = *valit;
		varit++;
		valit++;
	}
	return realRoots(p, m, interval, pivoting);
}

}
}
