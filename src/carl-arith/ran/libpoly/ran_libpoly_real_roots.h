#pragma once

#include <carl-common/config.h>

#ifdef RAN_USE_LIBPOLY

#include <carl-arith/poly/umvpoly/UnivariatePolynomial.h>
#include <carl-arith/core/Variable.h>
#include "../real_roots_common.h"

#include <carl-arith/converter/LibpolyConverter.h>
#include <carl-logging/carl-logging.h>

#include "ran_libpoly.h"

namespace carl::ran::libpoly {

// Returns roots in ascending order
template<typename Coeff, typename Number = typename UnderlyingNumberType<Coeff>::type, EnableIf<std::is_same<Coeff, Number>> = dummy>
real_roots_result<RealAlgebraicNumberLibpoly<Number>> real_roots(
	const UnivariatePolynomial<Coeff>& polynomial,
	const Interval<Number>& interval = Interval<Number>::unbounded_interval()) {

	CARL_LOG_DEBUG("carl.ran.libpoly", " Real roots of " << polynomial << " within " << interval);

	// Easy checks
	if (carl::is_zero(polynomial)) {
		CARL_LOG_TRACE("carl.ran.libpoly", "poly is 0 -> nullified");
		return real_roots_result<RealAlgebraicNumberLibpoly<Number>>::nullified_response();
	} else if (polynomial.is_number()) {
		CARL_LOG_TRACE("carl.ran.libpoly", "poly is constant but not zero -> no root");
		return real_roots_result<RealAlgebraicNumberLibpoly<Number>>::no_roots_response();
	}

	// We care for roots, so denominator is not important
	poly::UPolynomial upoly = LibpolyConverter::getInstance().toLibpolyUPolynomial(polynomial);

	poly::Interval inter_poly = LibpolyConverter::getInstance().toInterval(interval);

	// actual calculations
	std::vector<poly::AlgebraicNumber> roots = poly::isolate_real_roots(upoly);

	if (roots.empty()) {
		CARL_LOG_DEBUG("carl.ran.libpoly", "Poly has no roots");
		return real_roots_result<RealAlgebraicNumberLibpoly<Number>>::no_roots_response();
	}

	// sort roots in ascending order
	std::sort(roots.begin(), roots.end(), std::less<poly::AlgebraicNumber>());

	// turn into real_roots_result
	std::vector<RealAlgebraicNumberLibpoly<Number>> res;
	for (const auto& val : roots) {
		auto tmp = RealAlgebraicNumberLibpoly<Number>(val);
		// filter out roots not in interval
		if (poly::contains(inter_poly, poly::Value(val))) {
			CARL_LOG_DEBUG("carl.ran.libpoly", " Found Root " << val);
			res.emplace_back(tmp);
		}
	}

	return real_roots_result<RealAlgebraicNumberLibpoly<Number>>::roots_response(std::move(res));
}

template<typename Coeff, typename Number = typename UnderlyingNumberType<Coeff>::type, DisableIf<std::is_same<Coeff, Number>> = dummy>
real_roots_result<RealAlgebraicNumberLibpoly<Number>> real_roots(
	const UnivariatePolynomial<Coeff>& polynomial,
	const Interval<Number>& interval = Interval<Number>::unbounded_interval()) {
	assert(polynomial.is_univariate());
	return real_roots(polynomial.convert(std::function<Number(const Coeff&)>([](const Coeff& c) { return c.constant_part(); })), interval);
}

// Returns root in ascending order
// Finds nullification in rational cases
template<typename Coeff, typename Number = typename UnderlyingNumberType<Coeff>::type>
real_roots_result<RealAlgebraicNumberLibpoly<Number>> real_roots(
	const UnivariatePolynomial<Coeff>& p,
	const std::map<Variable, RealAlgebraicNumberLibpoly<Number>>& m,
	const Interval<Number>& interval = Interval<Number>::unbounded_interval()) {

	CARL_LOG_DEBUG("carl.ran.libpoly", " Real roots of " << p << " within " << interval << " with assignment: " << m);

	// Easy checks
	if (carl::is_zero(p)) {
		CARL_LOG_DEBUG("carl.ran.libpoly", "poly is 0 -> nullified");
		return real_roots_result<RealAlgebraicNumberLibpoly<Number>>::nullified_response();
	}
	if (carl::is_constant(p)) {
		CARL_LOG_DEBUG("carl.ran.libpoly", "poly is constant but not zero -> no root");
		return real_roots_result<RealAlgebraicNumberLibpoly<Number>>::no_roots_response();
	}

	// Iterate over terms with mainVariable and check whether another Variable is 0
	UnivariatePolynomial<Coeff> polyCopy(p);
	carl::Variable mainVar = p.mainVar();

	CARL_LOG_DEBUG("carl.ran.libpoly", "Poly has mainVar:  " << mainVar);
	// Check nullification -- Easy cases
	// Substitute rational numbers, and convert non-rational algebraic numbers to libpoly values and add to a libpoly assignment
	poly::Assignment assignment;
	bool assignedRAN = false;
	for (Variable& v : carl::variables(polyCopy)) {
		if (v == mainVar) continue;
		if (m.count(v) == 0) {
			CARL_LOG_DEBUG("carl.ran.libpoly", "poly still contains unassigned variable -> non-univariate");
			return real_roots_result<RealAlgebraicNumberLibpoly<Number>>::non_univariate_response();
		}
		assert(m.count(v) > 0);
		if (m.at(v).is_numeric()) {
			CARL_LOG_DEBUG("carl.ran.libpoly", "Poly got rational -> substituting");
			// We substitute rational numbers, as that has good performance
			substitute_inplace(polyCopy, v, Coeff(m.at(v).value()));
		} else {
			// We convert algebraic numbers to libpoly values and add to assignment so we can substitute them later using libpoly
			lp_value_t val;
			// Turn into value
			lp_value_construct(&val, lp_value_type_t::LP_VALUE_ALGEBRAIC, m.at(v).get_internal());
			// That copies the value into the assignment
			assignment.set(VariableMapper::getInstance().getLibpolyVariable(v), poly::Value(&val));
			// Destroy the value, but dont free the algebraic number!
			lp_value_destruct(&val);

			assignedRAN = true;
		}
	}
	CARL_LOG_DEBUG("carl.ran.libpoly", "After rational substitution: " << polyCopy);
	CARL_LOG_DEBUG("carl.ran.libpoly", "Assignment: " << assignment);
	if (carl::is_zero(polyCopy)) {
		CARL_LOG_DEBUG("carl.ran.libpoly", "poly is 0 -> nullified");
		return real_roots_result<RealAlgebraicNumberLibpoly<Number>>::nullified_response();
	} else if (!assignedRAN) {
		CARL_LOG_DEBUG("carl.ran.libpoly", "poly is not 0 and has no assigned RANs -> non-univariate in mainVar now");
		// No algebraic numbers in assignment --> we can use the univariate method
		assert(polyCopy.is_univariate());
		return real_roots(polyCopy, interval);
	}
	// Handle case where the mainVar is not present anymore
	if (!carl::variables(polyCopy).has(mainVar)) {
		CARL_LOG_DEBUG("carl.ran.libpoly", "poly is not 0 but the mainVar is not present anymore -> evaluate to check for nullification");
		poly::Value eval_val = poly::evaluate(LibpolyConverter::getInstance().toLibpolyPolynomial(polyCopy), assignment);
		CARL_LOG_DEBUG("carl.ran.libpoly", "Evaluation: " << eval_val);
		if (eval_val == poly::Value(long(0))) {
			return real_roots_result<RealAlgebraicNumberLibpoly<Number>>::nullified_response();
		} else {
			return real_roots_result<RealAlgebraicNumberLibpoly<Number>>::no_roots_response();
		}
	}
	// We have a non-univariate polynomial with algebraic numbers in the assignment

	assert(!assignment.has(VariableMapper::getInstance().getLibpolyVariable(mainVar)));

	// We care for roots, so denominator is not important
	poly::Polynomial poly_p = LibpolyConverter::getInstance().toLibpolyPolynomial(polyCopy);
	poly::Interval inter_poly = LibpolyConverter::getInstance().toInterval(interval);

	// Write the mainVar back to libpoly
	VariableMapper::getInstance().setLibpolyMainVariable(mainVar);
	// Reorder libpoly polynomials according to the global variable ordering
	lp_polynomial_ensure_order(poly_p.get_internal());

	CARL_LOG_DEBUG("carl.ran.libpoly", " Converted to Libpoly Assignment " << assignment);

	// to actual calculation -> only algebraic valued variables with are left in poly_p
	std::vector<poly::Value> roots = poly::isolate_real_roots(poly_p, assignment);

	CARL_LOG_DEBUG("carl.ran.libpoly", " Found roots " << roots);

	// If no roots we have to evaluate to check for nullification
	if (roots.empty()) {
		CARL_LOG_DEBUG("carl.ran.libpoly", " Checking for nullification -> Evaluation at " << mainVar << "= 1");
		assignment.set(VariableMapper::getInstance().getLibpolyVariable(mainVar), poly::Value(long(1)));
		poly::Value eval_val = poly::evaluate(poly_p, assignment);
		CARL_LOG_DEBUG("carl.ran.libpoly", " Got eval_val " << eval_val);

		if (eval_val == poly::Value(long(0))) {
			CARL_LOG_DEBUG("carl.ran.libpoly", "poly is 0 after substituting rational assignments -> nullified");
			return real_roots_result<RealAlgebraicNumberLibpoly<Number>>::nullified_response();
		} else {
			CARL_LOG_DEBUG("carl.ran.libpoly", "Poly has no roots");
			return real_roots_result<RealAlgebraicNumberLibpoly<Number>>::no_roots_response();
		}
	}

	// Sort, as we have to return the roots in ascending order
	// TODO: is that really needed?
	// See libpoly/src/polynomial/polynomial.c:1106
	std::sort(roots.begin(), roots.end(), std::less<poly::Value>());

	// turn result into real_roots_result
	std::vector<RealAlgebraicNumberLibpoly<Number>> res;
	for (const poly::Value& val : roots) {
		auto tmp = RealAlgebraicNumberLibpoly<Number>::create_from_value(val.get_internal());
		// filter out roots not in interval
		if (poly::contains(inter_poly, val)) {
			CARL_LOG_DEBUG("carl.ran.libpoly", " Found root " << val);
			res.emplace_back(tmp);
		}
	}

	return real_roots_result<RealAlgebraicNumberLibpoly<Number>>::roots_response(std::move(res));
}
} // namespace carl::ran::libpoly

#endif