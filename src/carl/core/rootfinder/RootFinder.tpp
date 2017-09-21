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
boost::optional<std::vector<RealAlgebraicNumber<Number>>> realRoots(
		const UnivariatePolynomial<Coeff>& p,
		const std::map<Variable, RealAlgebraicNumber<Number>>& m,
		const Interval<Number>& interval,
		SplittingStrategy pivoting
) {
	CARL_LOG_FUNC("carl.core.rootfinder", p << " in " << p.mainVar() << ", " << m << ", " << interval);
	assert(m.count(p.mainVar()) == 0);
	
	if (p.isZero()) {
		CARL_LOG_TRACE("carl.core.rootfinder", "p is 0 -> everything is a root");
		return boost::none;
	}
	if (p.isConstant()) {
		CARL_LOG_TRACE("carl.core.rootfinder", "p is constant but not zero -> no root");
		return std::vector<RealAlgebraicNumber<Number>>({});
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
		CARL_LOG_TRACE("carl.core.rootfinder", p << " in " << p.mainVar() << ", " << m << ", " << interval);
		std::map<Variable, Interval<Number>> varToInterval;
		UnivariatePolynomial<Number> evaledpoly = RealAlgebraicNumberEvaluation::evaluateCoefficients(tmp, IRmap, varToInterval);
		if (evaledpoly.isZero()) return boost::none;
		CARL_LOG_TRACE("carl.core.rootfinder", "Calling on " << evaledpoly);
		auto res = realRoots(evaledpoly, interval, pivoting);
		MultivariatePolynomial<Number> mvpoly(tmp);
		CARL_LOG_TRACE("carl.core.rootfinder", "Checking " << res << " on " << mvpoly);
		for (auto it = res.begin(); it != res.end();) {
			CARL_LOG_TRACE("carl.core.rootfinder", "Checking " << tmp.mainVar() << " = " << *it);
			IRmap[tmp.mainVar()] = *it;
			CARL_LOG_TRACE("carl.core.rootfinder", "Evaluating " << mvpoly << " on " << IRmap);
			if (!RealAlgebraicNumberEvaluation::evaluate(mvpoly, IRmap).isZero()) {
				CARL_LOG_TRACE("carl.core.rootfinder", "Purging spurious root " << *it);
				it = res.erase(it);
			} else {
				it++;
			}
		}
		return res;
	}
}

template<typename Coeff, typename Number>
boost::optional<std::vector<RealAlgebraicNumber<Number>>> realRoots(
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
