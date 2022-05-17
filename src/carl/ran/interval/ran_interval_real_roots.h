#pragma once

#include "internal.h"
#include <carl/interval/Interval.h>
#include <carl-logging/carl-logging.h>
#include <carl/core/Sign.h>
#include <carl/poly/umvpoly/UnivariatePolynomial.h>

#include "RealRootIsolation.h"

#include "../real_roots_common.h"

#include <map>

namespace carl::ran::interval {

/**
 * Find all real roots of a univariate 'polynomial' with numeric coefficients within a given 'interval'.
 * The roots are sorted in ascending order.
 */
template<typename Coeff, typename Number = typename UnderlyingNumberType<Coeff>::type, EnableIf<std::is_same<Coeff, Number>> = dummy>
real_roots_result<RealAlgebraicNumberInterval<Number>> real_roots(
		const UnivariatePolynomial<Coeff>& polynomial,
		const Interval<Number>& interval = Interval<Number>::unbounded_interval()
) {
	if (carl::is_zero(polynomial)) {
		return real_roots_result<RealAlgebraicNumberInterval<Number>>::nullified_response();
	}
	CARL_LOG_DEBUG("carl.ran.realroots", polynomial << " within " << interval);
	carl::ran::interval::RealRootIsolation rri(polynomial, interval);
	auto r = rri.get_roots();
	CARL_LOG_DEBUG("carl.ran.realroots", "-> " << r);
	return real_roots_result<RealAlgebraicNumberInterval<Number>>::roots_response(std::move(r));
}

// TODO make generic?
/**
 * Find all real roots of a univariate 'polynomial' with non-numeric coefficients within a given 'interval'.
 * However, all coefficients must be types that contain numeric numbers that are retrievable by using .constantPart();
 * The roots are sorted in ascending order.
 */
template<typename Coeff, typename Number = typename UnderlyingNumberType<Coeff>::type, DisableIf<std::is_same<Coeff, Number>> = dummy>
real_roots_result<RealAlgebraicNumberInterval<Number>> real_roots(
		const UnivariatePolynomial<Coeff>& polynomial,
		const Interval<Number>& interval = Interval<Number>::unbounded_interval()
) {
	assert(polynomial.isUnivariate());
	return real_roots(polynomial.convert(std::function<Number(const Coeff&)>([](const Coeff& c){ return c.constantPart(); })), interval);
}

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
 * Returns a real_roots_result indicating whether the roots could be isolated or the polynomial
 * was not univariate or is nullified.  
 */
template<typename Coeff, typename Number>
real_roots_result<RealAlgebraicNumberInterval<Number>> real_roots(
		const UnivariatePolynomial<Coeff>& poly,
		const Assignment<RealAlgebraicNumberInterval<Number>>& varToRANMap,
		const Interval<Number>& interval = Interval<Number>::unbounded_interval()
) {
	CARL_LOG_FUNC("carl.ran.realroots", poly << " in " << poly.mainVar() << ", " << varToRANMap << ", " << interval);
	assert(varToRANMap.count(poly.mainVar()) == 0);

	if (carl::is_zero(poly)) {
		CARL_LOG_TRACE("carl.ran.realroots", "poly is 0 -> nullified");
		return real_roots_result<RealAlgebraicNumberInterval<Number>>::nullified_response();
	}
	if (poly.isNumber()) {
		CARL_LOG_TRACE("carl.ran.realroots", "poly is constant but not zero -> no root");
		return real_roots_result<RealAlgebraicNumberInterval<Number>>::no_roots_response();
	}

  	// We want to simplify 'poly', but it's const, so make a copy.
	UnivariatePolynomial<Coeff> polyCopy(poly);
	Assignment<RealAlgebraicNumberInterval<Number>> ir_map;

	for (Variable v: carl::variables(polyCopy)) {
		if (v == poly.mainVar()) continue;
		if (varToRANMap.count(v) == 0) {
			CARL_LOG_TRACE("carl.ran.realroots", "poly still contains unassigned variable " << v << " -> non-univariate");
			return real_roots_result<RealAlgebraicNumberInterval<Number>>::non_univariate_response();
		}
		assert(varToRANMap.count(v) > 0);
		if (varToRANMap.at(v).is_numeric()) {
			substitute_inplace(polyCopy, v, Coeff(varToRANMap.at(v).value()));
		} else {
			ir_map.emplace(v, varToRANMap.at(v));
		}
	}
	if (carl::is_zero(polyCopy)) {
		CARL_LOG_TRACE("carl.ran.realroots", "poly is 0 after substituting rational assignments -> nullified");
		return real_roots_result<RealAlgebraicNumberInterval<Number>>::nullified_response();
	}
	if (ir_map.empty()) {
		assert(polyCopy.isUnivariate());
		CARL_LOG_TRACE("carl.ran.realroots", "poly " << polyCopy << " is univariate after substituting rational assignments");
		return real_roots(polyCopy, interval);
	} else {
		CARL_LOG_TRACE("carl.ran.realroots", polyCopy << " in " << polyCopy.mainVar() << ", " << varToRANMap << ", " << interval);
		assert(ir_map.find(polyCopy.mainVar()) == ir_map.end());

		// substitute RANs with low degrees first
		OrderedAssignment<RealAlgebraicNumberInterval<Number>> ord_ass;
		for (const auto& ass : ir_map) ord_ass.emplace_back(ass);
		std::sort(ord_ass.begin(), ord_ass.end(), [](const auto& a, const auto& b){ 
			return a.second.polynomial().degree() > b.second.polynomial().degree();
		});

		std::optional<UnivariatePolynomial<Number>> evaledpoly = substitute_rans_into_polynomial(polyCopy, ord_ass);
		if (!evaledpoly) {
			CARL_LOG_TRACE("carl.ran.realroots", "poly still contains unassigned variable -> non-univariate");
			return real_roots_result<RealAlgebraicNumberInterval<Number>>::non_univariate_response();
		}
		if (carl::is_zero(*evaledpoly)) {
			CARL_LOG_TRACE("carl.ran.realroots", "got zero polynomial -> nullified");
			return real_roots_result<RealAlgebraicNumberInterval<Number>>::nullified_response();
		}

		CARL_LOG_TRACE("carl.ran.realroots", "Calling on " << *evaledpoly);
		BasicConstraint<MultivariatePolynomial<Number>> cons(MultivariatePolynomial<Number>(polyCopy), Relation::EQ);
		std::vector<RealAlgebraicNumberInterval<Number>> roots;
		auto res = real_roots(*evaledpoly, interval);
		for (const auto& r: res.roots()) { // TODO can be made more efficient!
			CARL_LOG_TRACE("carl.ran.realroots", "Checking " << polyCopy.mainVar() << " = " << r);
			ir_map[polyCopy.mainVar()] = r;
			CARL_LOG_TRACE("carl.ran.realroots", "Evaluating " << cons << " on " << ir_map);
			if (evaluate(cons, ir_map)) {
				roots.emplace_back(r);
			} else {
				CARL_LOG_TRACE("carl.ran.realroots", "Purging spurious root " << r);
			}
		}
		return real_roots_result<RealAlgebraicNumberInterval<Number>>::roots_response(std::move(roots));
	}
}

}