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
	UnivariatePolynomial<Coeff> tmp(p);
	assert(m.count(p.mainVar()) == 0);
	// Store RealAlgebraicNumberIR here.
	std::map<Variable, RealAlgebraicNumberIR<Number>*> IRmap;
	for (Variable v: tmp.gatherVariables()) {
		// Do not replace main variable.
		if (v == p.mainVar()) continue;
		// All other variables must be in the map.
		assert(m.count(v) > 0);
		if (m.at(v)->isNumeric()) {
			// Immediately replace numeric representations.
			tmp.substituteIn(v, Coeff(m.at(v)->value()));
		} else {
			// Defer interval representations for later.
			IRmap[v] = static_cast<RealAlgebraicNumberIR<Number>*>(m.at(v));
		}
	}
	if (IRmap.size() == 0) {
		// No interval representations, hence it is already univariate.
		return realRoots(tmp, interval, pivoting);
	} else {
		// Otherwise, go on with interval evaluation.
		return realRoots(tmp, IRmap, interval, pivoting);
	}
}

template<typename Coeff, typename Number>
std::list<RealAlgebraicNumber<Number>*> realRoots(
		UnivariatePolynomial<Coeff>& p,
		const std::map<Variable, RealAlgebraicNumberIR<Number>*>& m,
		const ExactInterval<Number>& interval,
		SplittingStrategy pivoting
) {
	
}

template<class InputRIterator, class InputVIterator, typename Coeff, typename Number>
std::list<RealAlgebraicNumber<Number>*> realRoots(
		const UnivariatePolynomial<Coeff>& p,
		InputRIterator rBegin,
		InputRIterator rEnd,
		InputVIterator vBegin,
		InputVIterator vEnd,
		SplittingStrategy pivoting,
		const ExactInterval<Number>& interval
) {
	if (p.isZero()) {
		return { new RealAlgebraicNumberNR<Number>(interval.sample(), true) };
	}
	if (p.isConstant()) {
		return {};
	}
	
	auto tmp = p;
	std::map<Variable, RealAlgebraicNumberIR<Number>*> varMap;
	InputRIterator sampleValue = rBegin;
	InputVIterator variable    = vBegin;
	// Preprocessing: substitute all numeric occurrences of r directly and collect interval roots in varMap
	while (variable != vEnd) {
		assert(sampleValue != rEnd);
		if ((*sampleValue)->isNumeric()) {
			tmp.substituteIn(*variable, Coeff((*sampleValue)->value()));
		} else {
			varMap[*variable] = static_cast<RealAlgebraicNumberIR<Number>*>(*sampleValue);
		}
		variable++;
		sampleValue++;
	}
	
	if (varMap.size() == 0) {
		return realRoots(tmp, interval, pivoting);
	} else {
		return realRootsEvalIR(tmp, varMap, pivoting, interval);
	}
}

template<typename Number, typename Coeff, EnableIf<is_number<Number>>>
std::list<RealAlgebraicNumber<Number>*> realRootsEvalIR(
		const UnivariatePolynomial<Coeff>& p,
		const std::map<Variable, RealAlgebraicNumberIR<Number>*>& m,
		SplittingStrategy pivoting,
		const ExactInterval<Number>& interval
) {
	
}

}
}