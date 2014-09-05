/**
 * @file RootFinder.tpp
 * @ingroup rootfinder
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */


#pragma once

#include "RootFinder.h"

#include "../RealAlgebraicNumberEvaluation.h"

namespace carl {
namespace rootfinder {

template<typename Coeff, typename Number>
std::list<RealAlgebraicNumberPtr<Number>> realRoots(
		const UnivariatePolynomial<Coeff>& p,
		const std::map<Variable, RealAlgebraicNumberPtr<Number>>& m,
		const Interval<Number>& interval,
		SplittingStrategy pivoting
) {
	LOG_FUNC("carl.core.rootfinder", p << " in " << p.mainVar() << ", " << m << ", " << interval);
	assert(m.count(p.mainVar()) == 0);
	
	if (p.isZero()) {
		LOGMSG_TRACE("carl.core.rootfinder", "p is 0 -> sampling " << interval.sample());
		return { RealAlgebraicNumberNR<Number>::create(interval.sample(), true) };
	}
	if (p.isConstant()) {
		LOGMSG_TRACE("carl.core.rootfinder", "p is constant but not zero -> no root");
		return {};
	}
	
	UnivariatePolynomial<Coeff> tmp(p);
	std::map<Variable, RealAlgebraicNumberIRPtr<Number>> IRmap;
	
	for (Variable v: tmp.gatherVariables()) {
		if (v == p.mainVar()) continue;
		assert(m.count(v) > 0);
		if (m.at(v)->isNumeric()) {
			tmp.substituteIn(v, Coeff(m.at(v)->value()));
		} else {
			IRmap[v] = std::static_pointer_cast<RealAlgebraicNumberIR<Number>>(m.at(v));
		}
	}
	if (IRmap.size() == 0) {
		return realRoots(tmp, interval, pivoting);
	} else {
		return realRoots(tmp, IRmap, interval, pivoting);
	}
}

template<typename Coeff, typename Number>
std::list<RealAlgebraicNumberPtr<Number>> realRoots(
		const UnivariatePolynomial<Coeff>& p,
		const std::list<Variable> variables,
		const std::list<RealAlgebraicNumberPtr<Number>> values,
		const Interval<Number>& interval,
		SplittingStrategy pivoting
) {
	std::map<Variable, RealAlgebraicNumberPtr<Number>> m;
	
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

template<typename Coeff, typename Number>
std::list<RealAlgebraicNumberPtr<Number>> realRoots(
		UnivariatePolynomial<Coeff>& p,
		const std::map<Variable, RealAlgebraicNumberIRPtr<Number>>& m,
		const Interval<Number>& interval,
		SplittingStrategy pivoting
) {
	LOG_FUNC("carl.core.rootfinder", p << " in " << p.mainVar() << ", " << m << ", " << interval);
	std::map<Variable, Interval<Number>> varToInterval;
	UnivariatePolynomial<Number> res = RealAlgebraicNumberEvaluation::evaluateCoefficients(p, m, varToInterval);
	return realRoots(res, interval, pivoting);
}

}
}
