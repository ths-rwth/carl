#pragma once


#include <map>
#include <vector>

#include "RealAlgebraicNumber.h"
#include "../../../core/polynomialfunctions/LazardEvaluation.h"
#include "../../../core/polynomialfunctions/AlgebraicSubstitution.h"

#include "../../../util/SFINAE.h"

namespace carl {

namespace RealAlgebraicNumberEvaluation {

// TODO move somewhere else, integrate into evaluation
template<typename Coeff, typename Number>
bool vanishes(
		const UnivariatePolynomial<Coeff>& poly,
		const std::map<Variable, RealAlgebraicNumber<Number>>& varToRANMap
) {
	CARL_LOG_FUNC("carl.ran", "Check if " << poly << " vanishes under " << varToRANMap);
	assert(varToRANMap.find(poly.mainVar()) == varToRANMap.end());

	if (carl::isZero(poly)) {
		CARL_LOG_TRACE("carl.ran", "poly is 0");
		return true;
	}
	if (poly.isNumber()) {
		CARL_LOG_TRACE("carl.ran", "poly is constant but not zero");
		return false;
	}

	UnivariatePolynomial<Coeff> polyCopy(poly);
	std::map<Variable, RealAlgebraicNumber<Number>> IRmap;

	for (Variable v: polyCopy.gatherVariables()) {
		if (v == poly.mainVar()) continue;
		if (varToRANMap.count(v) == 0) {
			CARL_LOG_TRACE("carl.ran", "poly still contains unassigned variable " << v);
			return false;
		}
		assert(varToRANMap.count(v) > 0);
		if (varToRANMap.at(v).isNumeric()) {
			substitute_inplace(polyCopy, v, Coeff(varToRANMap.at(v).value()));
		} else {
			IRmap.emplace(v, varToRANMap.at(v));
		}
	}
	if (carl::isZero(polyCopy)) {
		CARL_LOG_TRACE("carl.ran", "poly is 0 after substituting rational assignments");
		return true;
	}
	if (IRmap.empty()) {
		assert(polyCopy.isUnivariate());
		CARL_LOG_TRACE("carl.ran", "poly is still univariate");
		return false;
	} else {
		CARL_LOG_TRACE("carl.ran", poly << " in " << poly.mainVar() << ", " << varToRANMap);
		assert(IRmap.find(polyCopy.mainVar()) == IRmap.end());

		LazardEvaluation<Number,MultivariatePolynomial<Number>> le((MultivariatePolynomial<Number>(polyCopy)));
		for(auto const& [var, val] : IRmap) {
			CARL_LOG_TRACE("carl.ran", "Substitute " << var << " -> " << val << " into " << le.getLiftingPoly());
			le.substitute(var, val, false);
			CARL_LOG_TRACE("carl.ran", "LE got " << le.getLiftingPoly());
			if (carl::isZero(le.getLiftingPoly())) {
				CARL_LOG_TRACE("carl.ran", "poly vanishes after plugging in " << var << " -> " << val);
				return true;
			}
		}

		return false;
	}
}

template<typename Number, typename Coeff>
UnivariatePolynomial<Number> substitute_rans_into_polynomial(
		const UnivariatePolynomial<Coeff>& p,
		const std::map<Variable, RealAlgebraicNumber<Number>>& m
) {
	std::vector<MultivariatePolynomial<Number>> polys;
	std::vector<Variable> varOrder;

	FieldExtensions<Number, MultivariatePolynomial<Number>> fe;
	for (const auto& vic: m) {
		varOrder.emplace_back(vic.first);
		auto res = fe.extend(vic.first, vic.second);
		if (res.first) {
			polys.emplace_back(vic.first - res.second);
		} else {
			polys.emplace_back(res.second);
		}
	}
	polys.emplace_back(p);
	varOrder.emplace_back(p.mainVar());

	return algebraic_substitution(polys, varOrder);
}

}
}