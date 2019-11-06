/**
 * @file
 * A collection of functions to get the roots of univariate polynomials
 * whose coeffecients may be multivariate polynomials, after replacing
 * the variables in the coefficients by specific values.
 *
 * @ingroup rootfinder
 */


#pragma once

#include "RootFinder.h"
#include "Substitution.h"

#include "../../formula/model/ran/RealAlgebraicNumberEvaluation.h"
#include "../../formula/model/ran/RealAlgebraicNumberIntervalExtra.h"

namespace carl {
namespace rootfinder {


// hiervon eine thom version machen!!!
template<typename Coeff, typename Number>
std::vector<RealAlgebraicNumber<Number>> realRoots(
		const UnivariatePolynomial<Coeff>& poly,
		const std::map<Variable, RealAlgebraicNumber<Number>>& varToRANMap,
		const Interval<Number>& interval
) {
	#ifdef RAN_USE_Z3
	return realRootsZ3(poly, varToRANMap, interval);
	#endif

	CARL_LOG_FUNC("carl.core.rootfinder", poly << " in " << poly.mainVar() << ", " << varToRANMap << ", " << interval);
	assert(varToRANMap.count(poly.mainVar()) == 0);

	if (carl::isZero(poly)) {
		CARL_LOG_TRACE("carl.core.rootfinder", "poly is 0 -> everything is a root");
		return {};
	}
	if (poly.isNumber()) {
		CARL_LOG_TRACE("carl.core.rootfinder", "poly is constant but not zero -> no root");
		return std::vector<RealAlgebraicNumber<Number>>({});
	}

  // We want to simplify 'poly', but it's const, so make a copy.
	UnivariatePolynomial<Coeff> polyCopy(poly);
	std::map<Variable, RealAlgebraicNumber<Number>> IRmap;

	for (Variable v: carl::variables(polyCopy).underlyingVariables()) {
		if (v == poly.mainVar()) continue;
		if (varToRANMap.count(v) == 0) {
			CARL_LOG_TRACE("carl.core.rootfinder", "poly still contains unassigned variable " << v);
			return {};
		}
		assert(varToRANMap.count(v) > 0);
		if (varToRANMap.at(v).isNumeric()) {
			substitute_inplace(polyCopy, v, Coeff(varToRANMap.at(v).value()));
		} else {
			IRmap.emplace(v, varToRANMap.at(v));
		}
	}
	if (carl::isZero(polyCopy)) {
		CARL_LOG_TRACE("carl.core.rootfinder", "poly is 0 after substituting rational assignments -> everything is a root");
		return {};
	}
	if (IRmap.empty()) {
		assert(polyCopy.isUnivariate());
		return realRoots(polyCopy, interval);
	} else {
		CARL_LOG_TRACE("carl.core.rootfinder", poly << " in " << poly.mainVar() << ", " << varToRANMap << ", " << interval);
		assert(IRmap.find(polyCopy.mainVar()) == IRmap.end());

		std::optional<UnivariatePolynomial<Number>> evaledpoly;

		evaledpoly = RealAlgebraicNumberEvaluation::substitute_rans_into_polynomial(polyCopy, IRmap);

		if (!evaledpoly) return {};
		CARL_LOG_DEBUG("carl.core.rootfinder", "Calling on " << *evaledpoly);
		
		Constraint<MultivariatePolynomial<Number>> cons(MultivariatePolynomial<Number>(polyCopy), Relation::EQ);
		std::vector<RealAlgebraicNumber<Number>> roots;
		auto res = realRoots(*evaledpoly, interval);
		for (const auto& r: res) {
			CARL_LOG_DEBUG("carl.core.rootfinder", "Checking " << polyCopy.mainVar() << " = " << r);
			IRmap[polyCopy.mainVar()] = r;
			CARL_LOG_DEBUG("carl.core.rootfinder", "Evaluating " << cons << " on " << IRmap);
			if (RealAlgebraicNumberEvaluation::evaluate(cons, IRmap)) {
				roots.emplace_back(r);
			} else {
				CARL_LOG_DEBUG("carl.core.rootfinder", "Purging spurious root " << r);
			}
		}
		return roots;
	}
}

template<typename Coeff, typename Number>
std::vector<RealAlgebraicNumber<Number>> realRoots(
		const UnivariatePolynomial<Coeff>& poly,
		const std::list<Variable>& variables,
		const std::list<RealAlgebraicNumber<Number>>& values,
		const Interval<Number>& interval
) {
	std::map<Variable, RealAlgebraicNumber<Number>> varToRANMap;

	assert(variables.size() == values.size());
	auto varit = variables.begin();
	auto valit = values.begin();
	while (varit != variables.end()) {
		varToRANMap[*varit] = *valit;
		varit++;
		valit++;
	}
	return realRoots(poly, varToRANMap, interval);
}

}
}
