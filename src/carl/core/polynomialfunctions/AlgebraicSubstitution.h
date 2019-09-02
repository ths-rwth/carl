#pragma once

/**
 * @file AlgebraicSubstitution.h
 * This file contains carl::algebraic_substitution which performs what we call an ``algebraic substitution``.
 * We substitute the ``algebraic part`` of a variable assignment into a multivariate polynomial to obtain a univariate polynomial in the remaining variable.
 * The algebraic part is a minimal polynomial, in practice the defining polynomial of a real algebraic number.
 * We implement two strategies: GROEBNER and RESULTANT.
 * 
 * In both cases you need to make sure that the minimal polynomials are not only reducible over Q, but are actually minimal polynomials within a tower of field extensions characterized by the given polynomials.
 * If one fails to do this, the resulting polynomial may vanish identically.
 * In some cases one can exclude that this happens.
 * 
 * Such minimal polynomials can be computed using FieldExtensions.
 * If the resultant strategy is used, it is important that the defining polynomials are used from top to bottom.
 * Thus the first defining polynomials may contain all variables, the last must be univariate.
 */

#include "../MultivariatePolynomial.h"
#include "../UnivariatePolynomial.h"
#include "../../converter/CoCoAAdaptor.h"
#include "Resultant.h"

namespace carl {

/**
 * Implements algebraic substitution by Gröbner basis computation.
 * Essentially we take all polynomials and compute a Gröbner basis with respect to an elimination order, having the remaining variable at the end.
 * The result is then the polynomial in the last variable only.
 */
template<typename Number, typename Coeff>
UnivariatePolynomial<Number> algebraic_substitution_groebner(const UnivariatePolynomial<Coeff>& p, const std::vector<UnivariatePolynomial<Number>>& polynomials) {
	std::vector<Variable> varOrder;
	std::vector<MultivariatePolynomial<Number>> polys;
	for (const auto& poly: polynomials) {
		varOrder.emplace_back(poly.mainVar());
		polys.emplace_back(poly);
	}
	varOrder.emplace_back(p.mainVar());
	polys.emplace_back(p);
	try {
		CoCoAAdaptor<MultivariatePolynomial<Number>> ca(varOrder, true);
		CARL_LOG_DEBUG("carl.algsubs", "Computing GBasis of " << polys << " with order " << varOrder);
		auto res = ca.GBasis(polys);
		CARL_LOG_DEBUG("carl.algsubs", "-> " << res);
		for (const auto& poly: res) {
			carlVariables vars;
			poly.gatherVariables(vars);
			if (vars == carlVariables({ p.mainVar() })) {
				CARL_LOG_DEBUG("carl.algsubs", "-> " << poly)
				return poly.toUnivariatePolynomial();
			}
		}
	} catch (const CoCoA::ErrorInfo& e) {
		CARL_LOG_ERROR("carl.algsubs", "Computation of GBasis failed: " << e << " -> " << CoCoA::context(e));
	}
	return UnivariatePolynomial<Number>(p.mainVar());
}

/**
 * Implements algebraic substitution by resultant computation.
 * We iteratively compute the resultant of the input polynomial with each of the defining polynomials.
 * Eventually we obtain a polynomial univariate in the remaining variable, our result.
 */
template<typename Number, typename Coeff>
UnivariatePolynomial<Number> algebraic_substitution_resultant(const UnivariatePolynomial<Coeff>& p, const std::vector<UnivariatePolynomial<Number>>& polynomials) {
	Variable v = p.mainVar();
	UnivariatePolynomial<Coeff> cur = p;
	for (const auto& poly: polynomials) {
		if (!cur.has(poly.mainVar())) {
			continue;
		}
		UnivariatePolynomial<Coeff> poly_mv = poly.template convert<Coeff>();
		cur = cur.switchVariable(poly.mainVar()).prem(poly_mv);
		CARL_LOG_DEBUG("carl.algsubs", "Computing resultant of " << cur << " and " << poly_mv);
		cur = carl::resultant(cur, poly_mv);
		CARL_LOG_DEBUG("carl.algsubs", "-> " << cur);
	}
	UnivariatePolynomial<Number> result = cur.switchVariable(v).toNumberCoefficients();
	CARL_LOG_DEBUG("carl.algsubs", "Result: " << result);
	return result;
}

/// Indicates which strategy to use: resultants or Gröbner bases.
enum class AlgebraicSubstitutionStrategy {
	RESULTANT, GROEBNER
};

/**
 * Computes the algebraic substitution of the given defining polynomials into a multivariate polynomial p.
 * The result is a univariate polynomial in the main variable of p.
 */
template<typename Number, typename Coeff>
UnivariatePolynomial<Number> algebraic_substitution(const UnivariatePolynomial<Coeff>& p, const std::vector<UnivariatePolynomial<Number>>& polynomials, AlgebraicSubstitutionStrategy strategy = AlgebraicSubstitutionStrategy::RESULTANT) {
	CARL_LOG_DEBUG("carl.algsubs", "Substituting " << polynomials << " into " << p);
	switch (strategy) {
		case AlgebraicSubstitutionStrategy::GROEBNER:
			return algebraic_substitution_groebner(p, polynomials);
		case AlgebraicSubstitutionStrategy::RESULTANT:
			return algebraic_substitution_resultant(p, polynomials);
	}
}


}