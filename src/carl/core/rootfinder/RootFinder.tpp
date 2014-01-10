/* 
 * File:   RootFinder.tpp
 * Author: Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#include "RootFinder.h"


#pragma once

namespace carl {
namespace rootfinder {

template<typename Coeff, typename Number>
std::list<RealAlgebraicNumber<Number>*> realRoots(
		const UnivariatePolynomial<Coeff>& p,
		const std::map<Variable, RealAlgebraicNumber<Number>*>& m,
		const ExactInterval<Number>& interval,
		SplittingStrategy pivoting
) {
	assert(m.count(p.mainVar()) == 0);
	
	if (p.isZero()) {
		return { new RealAlgebraicNumberNR<Number>(interval.sample(), true) };
	}
	if (p.isConstant()) {
		return {};
	}
	
	UnivariatePolynomial<Coeff> tmp(p);
	std::map<Variable, RealAlgebraicNumberIR<Number>*> IRmap;
	
	for (Variable v: tmp.gatherVariables()) {
		if (v == p.mainVar()) continue;
		assert(m.count(v) > 0);
		if (m.at(v)->isNumeric()) {
			tmp.substituteIn(v, Coeff(m.at(v)->value()));
		} else {
			IRmap[v] = static_cast<RealAlgebraicNumberIR<Number>*>(m.at(v));
		}
	}
	if (IRmap.size() == 0) {
		return realRoots(tmp, interval, pivoting);
	} else {
		return realRoots(tmp, IRmap, interval, pivoting);
	}
}

template<typename Coeff, typename Number>
std::list<RealAlgebraicNumber<Number>*> realRoots(
		const UnivariatePolynomial<Coeff>& p,
		const std::list<Variable> variables,
		const std::list<RealAlgebraicNumber<Number>*> values,
		const ExactInterval<Number>& interval,
		SplittingStrategy pivoting
) {
	std::map<Variable, RealAlgebraicNumber<Number>*> m;
	
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
std::list<RealAlgebraicNumber<Number>*> realRoots(
		UnivariatePolynomial<Coeff>& p,
		const std::map<Variable, RealAlgebraicNumberIR<Number>*>& m,
		const ExactInterval<Number>& interval,
		SplittingStrategy pivoting
) {
	std::map<Variable, ExactInterval<Number>> varToInterval;
	
	UnivariatePolynomial<Number> res = RealAlgebraicNumberEvaluation::evaluateCoefficients(p, m, varToInterval);
	return realRoots(res, interval, pivoting);
}

}
}