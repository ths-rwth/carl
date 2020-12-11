#pragma once

#include "./ran_interval_extra.h"
#include <carl/interval/Interval.h>
#include <carl/core/logging.h>
#include <carl/core/Sign.h>
#include <carl/core/UnivariatePolynomial.h>

#include "RealRootIsolation.h"

#include <map>

namespace carl::ran {
namespace interval {

////////////////////////////////////////
////////////////////////////////////////
// realRoots() for univariate polynomials

/**
 * Find all real roots of a univariate 'polynomial' with numeric coefficients within a given 'interval'.
 * The roots are sorted in ascending order.
 */
template<typename Coeff, typename Number = typename UnderlyingNumberType<Coeff>::type, EnableIf<std::is_same<Coeff, Number>> = dummy>
std::vector<real_algebraic_number_interval<Number>> realRoots(
		const UnivariatePolynomial<Coeff>& polynomial,
		const Interval<Number>& interval = Interval<Number>::unboundedInterval()
) {
	CARL_LOG_DEBUG("carl.ran.realroots", polynomial << " within " << interval);
	carl::ran::interval::RealRootIsolation rri(polynomial, interval);
	auto r = rri.get_roots();
	CARL_LOG_DEBUG("carl.ran.realroots", "-> " << r);
	return r;
}

// TODO make generic?
/**
 * Find all real roots of a univariate 'polynomial' with non-numeric coefficients within a given 'interval'.
 * However, all coefficients must be types that contain numeric numbers that are retrievable by using .constantPart();
 * The roots are sorted in ascending order.
 */
template<typename Coeff, typename Number = typename UnderlyingNumberType<Coeff>::type, DisableIf<std::is_same<Coeff, Number>> = dummy>
std::vector<real_algebraic_number_interval<Number>> realRoots(
		const UnivariatePolynomial<Coeff>& polynomial,
		const Interval<Number>& interval = Interval<Number>::unboundedInterval()
) {
	assert(polynomial.isUnivariate());
	return realRoots(polynomial.convert(std::function<Number(const Coeff&)>([](const Coeff& c){ return c.constantPart(); })), interval);
}

////////////////////////////////////////
////////////////////////////////////////
// realRoots() for multivariate polynomials

/**
 * Replace all variables except one of the multivariate polynomial 'p' by
 * numbers as given in the mapping 'm', which creates a univariate polynomial,
 * and return all roots of that created polynomial.
 * Note that 'p' is represented as a univariate polynomial with polynomial coefficients.
 * Its main variable is not replaced and stays the main variable of the created polynomial.
 * However, all variables in the polynomial coefficients are replaced, which is why
 * <ul>
 *   <li>the main variable of 'p' must not be in 'm'</li>
 *   <li>all variables from the coefficients of 'p' must be in 'm'</li>
 * </ul>
 * The roots are sorted in ascending order.
 */
template<typename Coeff, typename Number>
std::vector<real_algebraic_number_interval<Number>> realRoots(
		const UnivariatePolynomial<Coeff>& poly,
		const std::map<Variable, real_algebraic_number_interval<Number>>& varToRANMap,
		const Interval<Number>& interval = Interval<Number>::unboundedInterval()
) {
	CARL_LOG_FUNC("carl.ran.realroots", poly << " in " << poly.mainVar() << ", " << varToRANMap << ", " << interval);
	assert(varToRANMap.count(poly.mainVar()) == 0);

	if (carl::isZero(poly)) {
		CARL_LOG_TRACE("carl.ran.realroots", "poly is 0 -> everything is a root");
		return {};
	}
	if (poly.isNumber()) {
		CARL_LOG_TRACE("carl.ran.realroots", "poly is constant but not zero -> no root");
		return std::vector<real_algebraic_number_interval<Number>>({});
	}

  // We want to simplify 'poly', but it's const, so make a copy.
	UnivariatePolynomial<Coeff> polyCopy(poly);
	std::map<Variable, real_algebraic_number_interval<Number>> IRmap;

	for (Variable v: carl::variables(polyCopy).underlyingVariables()) {
		if (v == poly.mainVar()) continue;
		if (varToRANMap.count(v) == 0) {
			CARL_LOG_TRACE("carl.ran.realroots", "poly still contains unassigned variable " << v);
			return {};
		}
		assert(varToRANMap.count(v) > 0);
		if (varToRANMap.at(v).is_numeric()) {
			substitute_inplace(polyCopy, v, Coeff(varToRANMap.at(v).value()));
		} else {
			IRmap.emplace(v, varToRANMap.at(v));
		}
	}
	if (carl::isZero(polyCopy)) {
		CARL_LOG_TRACE("carl.ran.realroots", "poly is 0 after substituting rational assignments -> everything is a root");
		return {};
	}
	if (IRmap.empty()) {
		assert(polyCopy.isUnivariate());
		return realRoots(polyCopy, interval);
	} else {
		CARL_LOG_TRACE("carl.ran.realroots", polyCopy << " in " << polyCopy.mainVar() << ", " << varToRANMap << ", " << interval);
		assert(IRmap.find(polyCopy.mainVar()) == IRmap.end());

		std::optional<UnivariatePolynomial<Number>> evaledpoly;

		evaledpoly = substitute_rans_into_polynomial(polyCopy, IRmap);

		if (!evaledpoly) return {};
		CARL_LOG_DEBUG("carl.ran.realroots", "Calling on " << *evaledpoly);
		
		Constraint<MultivariatePolynomial<Number>> cons(MultivariatePolynomial<Number>(polyCopy), Relation::EQ);
		std::vector<real_algebraic_number_interval<Number>> roots;
		auto res = realRoots(*evaledpoly, interval);
		for (const auto& r: res) { // TODO can be made more efficient!
			CARL_LOG_DEBUG("carl.ran.realroots", "Checking " << polyCopy.mainVar() << " = " << r);
			IRmap[polyCopy.mainVar()] = r;
			CARL_LOG_DEBUG("carl.ran.realroots", "Evaluating " << cons << " on " << IRmap);
			if (evaluate(cons, IRmap)) {
				roots.emplace_back(r);
			} else {
				CARL_LOG_DEBUG("carl.ran.realroots", "Purging spurious root " << r);
			}
		}
		return roots;
	}
}

}
}