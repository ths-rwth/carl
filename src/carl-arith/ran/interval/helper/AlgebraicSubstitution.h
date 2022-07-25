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

#include <carl-arith/poly/umvpoly/MultivariatePolynomial.h>
#include <carl-arith/poly/umvpoly/UnivariatePolynomial.h>
#ifdef USE_COCOA
#include <carl-arith/poly/umvpoly/CoCoAAdaptor.h>
#endif

#include <carl-arith/poly/umvpoly/functions/Remainder.h>
#include <carl-arith/poly/umvpoly/functions/Resultant.h>
#include <carl-arith/poly/umvpoly/functions/to_univariate_polynomial.h>

namespace carl::ran::interval {

/**
 * Implements algebraic substitution by Gröbner basis computation.
 * Essentially we take all polynomials and compute a Gröbner basis with respect to an elimination order, having the remaining variable at the end.
 * The result is then the polynomial in the last variable only.
 */
template<typename Number>
std::optional<UnivariatePolynomial<Number>> algebraic_substitution_groebner(
	const std::vector<MultivariatePolynomial<Number>>& polynomials,
	const std::vector<Variable>& variables
) {
	#ifdef USE_COCOA
	Variable target = variables.back();
	try {
		CoCoAAdaptor<MultivariatePolynomial<Number>> ca(variables, true);
		CARL_LOG_DEBUG("carl.ran.interval", "Computing GBasis of " << polynomials << " with order " << variables);
		auto res = ca.GBasis(polynomials);
		CARL_LOG_DEBUG("carl.ran.interval", "-> " << res);
		for (const auto& poly: res) {
			if (carl::variables(poly) == carlVariables({ target })) {
				CARL_LOG_DEBUG("carl.ran.interval", "-> " << poly)
				return carl::to_univariate_polynomial(poly);
			}
		}
	} catch (const CoCoA::ErrorInfo& e) {
		CARL_LOG_ERROR("carl.ran.interval", "Computation of GBasis failed: " << e << " -> " << CoCoA::context(e));
	}
	#else
	CARL_LOG_ERROR("carl.ran.interval", "CoCoALib is not enabled");
	assert(false);
	#endif
	return std::nullopt;
	// return UnivariatePolynomial<Number>(target);
}

/**
 * Implements algebraic substitution by Gröbner basis computation.
 * Essentially we take all polynomials and compute a Gröbner basis with respect to an elimination order, having the remaining variable at the end.
 * The result is then the polynomial in the last variable only.
 */
template<typename Number>
std::optional<UnivariatePolynomial<Number>> algebraic_substitution_groebner(
	const UnivariatePolynomial<MultivariatePolynomial<Number>>& p,
	const std::vector<UnivariatePolynomial<MultivariatePolynomial<Number>>>& polynomials
) {
	std::vector<MultivariatePolynomial<Number>> polys;
	std::vector<Variable> varOrder;
	for (const auto& poly: polynomials) {
		polys.emplace_back(poly);
		varOrder.emplace_back(poly.main_var());
	}
	polys.emplace_back(p);
	varOrder.emplace_back(p.main_var());

	CARL_LOG_DEBUG("carl.ran.interval", "Converted " << p << " and " << polynomials << " to " << polys << " under " << varOrder);
	return algebraic_substitution_groebner(polys, varOrder);
}

/**
 * Implements algebraic substitution by resultant computation.
 * We iteratively compute the resultant of the input polynomial with each of the defining polynomials.
 * Eventually we obtain a polynomial univariate in the remaining variable, our result.
 * 
 * Note that we assume that the polynomials are in a triangular form where any polynomial may contain variables that are ``defined'' by the previous polynomials.
 */
template<typename Number>
std::optional<UnivariatePolynomial<Number>> algebraic_substitution_resultant(
	const UnivariatePolynomial<MultivariatePolynomial<Number>>& p,
	const std::vector<UnivariatePolynomial<MultivariatePolynomial<Number>>>& polynomials
) {
	Variable v = p.main_var();
	UnivariatePolynomial<MultivariatePolynomial<Number>> cur = p;
	for (auto it = polynomials.rbegin(); it != polynomials.rend(); ++it) {
		const auto& poly = *it;
		if (!cur.has(poly.main_var())) {
			continue;
		}
		cur = pseudo_remainder(switch_main_variable(cur, poly.main_var()), poly);
		CARL_LOG_DEBUG("carl.ran.interval", "Computing resultant of " << cur << " and " << poly);
		cur = carl::resultant(cur, poly);
		CARL_LOG_DEBUG("carl.ran.interval", "-> " << cur);
	}
	auto swpoly = switch_main_variable(cur, v);
	if (!swpoly.is_univariate()) {
		return std::nullopt;
	}
	UnivariatePolynomial<Number> result = swpoly.toNumberCoefficients();
	CARL_LOG_DEBUG("carl.ran.interval", "Result: " << result);
	return result;
}

/**
 * Implements algebraic substitution by resultant computation.
 * We iteratively compute the resultant of the input polynomial with each of the defining polynomials.
 * Eventually we obtain a polynomial univariate in the remaining variable, our result.
 * 
 * Note that we assume that the polynomials are in a triangular form where any polynomial may contain variables that are ``defined'' by the previous polynomials.
 */
template<typename Number>
std::optional<UnivariatePolynomial<Number>> algebraic_substitution_resultant(
	const std::vector<MultivariatePolynomial<Number>>& polynomials,
	const std::vector<Variable>& variables
) {
	auto p = carl::to_univariate_polynomial(polynomials.back(), variables.back());
	std::vector<UnivariatePolynomial<MultivariatePolynomial<Number>>> polys;

	for (std::size_t i = 0; i < polynomials.size() - 1; ++i) {
		polys.emplace_back(carl::to_univariate_polynomial(polynomials[i], variables[i]));
	}

	CARL_LOG_DEBUG("carl.ran.interval", "Converted " << polynomials << " under " << variables << " to " << p << " and " << polys);
	return algebraic_substitution_resultant(p, polys);
}

/// Indicates which strategy to use: resultants or Gröbner bases.
enum class AlgebraicSubstitutionStrategy {
	RESULTANT, GROEBNER
};

/**
 * Computes the algebraic substitution of the given defining polynomials into a multivariate polynomial p.
 * The result is a univariate polynomial in the main variable of p.
 */
template<typename Number>
std::optional<UnivariatePolynomial<Number>> algebraic_substitution(
	const UnivariatePolynomial<MultivariatePolynomial<Number>>& p,
	const std::vector<UnivariatePolynomial<MultivariatePolynomial<Number>>>& polynomials,
	AlgebraicSubstitutionStrategy strategy = AlgebraicSubstitutionStrategy::RESULTANT
) {
	CARL_LOG_DEBUG("carl.ran.interval", "Substituting " << polynomials << " into " << p);
	switch (strategy) {
		case AlgebraicSubstitutionStrategy::GROEBNER:
			return algebraic_substitution_groebner(p, polynomials);
		case AlgebraicSubstitutionStrategy::RESULTANT:
		default:
			return algebraic_substitution_resultant(p, polynomials);
	}
}

/**
 * Computes the algebraic substitution of the given defining polynomials into a multivariate polynomial p.
 * The result is a univariate polynomial in the main variable of p.
 */
template<typename Number>
std::optional<UnivariatePolynomial<Number>> algebraic_substitution(
	const std::vector<MultivariatePolynomial<Number>>& polynomials,
	const std::vector<Variable>& variables,
	AlgebraicSubstitutionStrategy strategy = AlgebraicSubstitutionStrategy::RESULTANT
) {
	CARL_LOG_WARN("carl.ran.interval", "Substituting " << polynomials << " into " << polynomials.back());
	switch (strategy) {
		case AlgebraicSubstitutionStrategy::GROEBNER:
			return algebraic_substitution_groebner(polynomials, variables);
		case AlgebraicSubstitutionStrategy::RESULTANT:
		default:
			return algebraic_substitution_resultant(polynomials, variables);
	}
}

}