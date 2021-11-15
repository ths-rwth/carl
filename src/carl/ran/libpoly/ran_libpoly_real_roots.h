#pragma once

#include <carl/config.h>

#ifdef RAN_USE_LIBPOLY

#include "../../../carl/core/UnivariatePolynomial.h"
#include "../../../carl/core/Variable.h"
#include "../../../carl/core/polynomialfunctions/Resultant.h"
#include "../real_roots_common.h"

#include "../../../carl/converter/LibpolyConverter.h"

#include "ran_libpoly.h"

namespace carl::ran::libpoly {

//Returns roots in ascending order
template<typename Coeff, typename Number = typename UnderlyingNumberType<Coeff>::type, EnableIf<std::is_same<Coeff, Number>> = dummy>
real_roots_result<real_algebraic_number_libpoly<Number>> real_roots(
	const UnivariatePolynomial<Coeff>& polynomial,
	const Interval<Number>& interval = Interval<Number>::unboundedInterval()) {

	CARL_LOG_DEBUG("carl.ran.libpoly", " Real roots of " << polynomial << " within " << interval);

	//Easy checks
	if (carl::isZero(polynomial)) {
		CARL_LOG_TRACE("carl.ran.libpoly", "poly is 0 -> nullified");
		return real_roots_result<real_algebraic_number_libpoly<Number>>::nullified_response();
	} else if (polynomial.isNumber()) {
		CARL_LOG_TRACE("carl.ran.libpoly", "poly is constant but not zero -> no root");
		return real_roots_result<real_algebraic_number_libpoly<Number>>::no_roots_response();
	}

	//We care for roots, so denominator is not important
	poly::UPolynomial upoly = LibpolyConverter::getInstance().toLibpolyUPolynomial(polynomial);

	poly::Interval inter_poly = LibpolyConverter::getInstance().toInterval(interval);

	//actual calculations
	std::vector<poly::AlgebraicNumber> roots = poly::isolate_real_roots(upoly);

	if (roots.empty()) {
		CARL_LOG_DEBUG("carl.ran.libpoly", "Poly has no roots");
		return real_roots_result<real_algebraic_number_libpoly<Number>>::no_roots_response();
	}

	//sort roots in ascending order
	std::sort(roots.begin(), roots.end(), std::less<poly::AlgebraicNumber>());

	//turn into real_roots_result
	std::vector<real_algebraic_number_libpoly<Number>> res;
	for (const auto& val : roots) {
		auto tmp = real_algebraic_number_libpoly<Number>(val);
		// filter out roots not in interval
		if (poly::contains(inter_poly, poly::Value(val))) {
			CARL_LOG_DEBUG("carl.ran.libpoly", " Found Root " << val);
			res.emplace_back(tmp);
		}
	}

	return real_roots_result<real_algebraic_number_libpoly<Number>>::roots_response(std::move(res));
}

template<typename Coeff, typename Number = typename UnderlyingNumberType<Coeff>::type, DisableIf<std::is_same<Coeff, Number>> = dummy>
real_roots_result<real_algebraic_number_libpoly<Number>> real_roots(
	const UnivariatePolynomial<Coeff>& polynomial,
	const Interval<Number>& interval = Interval<Number>::unboundedInterval()) {
	assert(polynomial.isUnivariate());
	return real_roots(polynomial.convert(std::function<Number(const Coeff&)>([](const Coeff& c) { return c.constantPart(); })), interval);
}

//Returns root in ascending order
//Finds nullification in rational cases
template<typename Coeff, typename Number = typename UnderlyingNumberType<Coeff>::type>
real_roots_result<real_algebraic_number_libpoly<Number>> real_roots(
	const UnivariatePolynomial<Coeff>& p,
	const std::map<Variable, real_algebraic_number_libpoly<Number>>& m,
	const Interval<Number>& interval = Interval<Number>::unboundedInterval()) {

	CARL_LOG_DEBUG("carl.ran.libpoly", " Real roots of " << p << " within " << interval << " with assignment: " << m);

	//Easy checks
	if (carl::isZero(p)) {
		CARL_LOG_DEBUG("carl.ran.libpoly", "poly is 0 -> nullified");
		return real_roots_result<real_algebraic_number_libpoly<Number>>::nullified_response();
	}
	if (carl::is_constant(p)) {
		CARL_LOG_DEBUG("carl.ran.libpoly", "poly is constant but not zero -> no root");
		return real_roots_result<real_algebraic_number_libpoly<Number>>::no_roots_response();
	}

	//Iterate over terms with mainVariable and check whether another Variable is 0
	UnivariatePolynomial<Coeff> polyCopy(p);
	carl::Variable mainVar = p.mainVar();

	CARL_LOG_DEBUG("carl.ran.libpoly", "Poly has mainVar:  " << mainVar);
	//Check nullification -- Easy cases
	//Substitute rational numbers, and convert non-rational algebraic numbers to libpoly values and add to a libpoly assignment
	poly::Assignment assignment;
	bool found_unassigned_var = false;
	for (Variable& v : carl::variables(polyCopy)) {
		if (v == mainVar) continue;
		if (m.count(v) == 0) {
			found_unassigned_var = true;
			continue;
		}
		if (m.at(v).is_numeric()) {
			CARL_LOG_DEBUG("carl.ran.libpoly", "Poly got rational -> substituting");
			//We substitute rational numbers, since that has good performance
			substitute_inplace(polyCopy, v, Coeff(m.at(v).value()));
		} else {
			lp_value_t val;
			//Turn into value
			lp_value_construct(&val, lp_value_type_t::LP_VALUE_ALGEBRAIC, m.at(v).get_internal());
			//That copies the value into the assignment
			assignment.set(VariableMapper::getInstance().getLibpolyVariable(v), poly::Value(&val));
			//Destroy the value, but dont free the algebraic number!
			lp_value_destruct(&val);
		}
	}
	CARL_LOG_DEBUG("carl.ran.libpoly", "After rational substitution: " << polyCopy);
	//mainVar does not occur in poly anymore --> Nullified
	if (carl::isZero(polyCopy)) {
		CARL_LOG_DEBUG("carl.ran.libpoly", "poly is 0 -> nullified");
		return real_roots_result<real_algebraic_number_libpoly<Number>>::nullified_response();
	} else if (!carl::variables(polyCopy).has(mainVar)) {
		CARL_LOG_DEBUG("carl.ran.libpoly", "poly does not contain mainVar after substituting rationals -> no roots");
		return real_roots_result<real_algebraic_number_libpoly<Number>>::no_roots_response();
	} else if (found_unassigned_var) {
		CARL_LOG_DEBUG("carl.ran.libpoly", "poly still contains unassigned variable -> non-univariate");
		return real_roots_result<real_algebraic_number_libpoly<Number>>::non_univariate_response();
	}

	//We care for roots, so denominator is not important
	poly::Polynomial poly_p = LibpolyConverter::getInstance().toLibpolyPolynomial(polyCopy);

	poly::Interval inter_poly = LibpolyConverter::getInstance().toInterval(interval);

	//Write the mainVar back to libpoly
	VariableMapper::getInstance().setLibpolyMainVariable(mainVar);
	//Reorder libpoly polynomials according to the global variable ordering
	lp_polynomial_ensure_order(poly_p.get_internal());

	CARL_LOG_DEBUG("carl.ran.libpoly", " Converted to Libpoly Assignment " << assignment);

	//to actual calculation -> only algebraic valued variables with are left in poly_p
	std::vector<poly::Value> roots = poly::isolate_real_roots(poly_p, assignment);

	CARL_LOG_DEBUG("carl.ran.libpoly", " Found roots " << roots);

	//If no roots or only 0 was returned we have to evaluate to check for nullification
	if (roots.empty() || (roots.size() == 1 && roots.front() == poly::Value(poly::Integer(0)))) {
		CARL_LOG_DEBUG("carl.ran.libpoly", " Checking for nullification -> Evaluation at " << mainVar << "= 1");
		assignment.set(VariableMapper::getInstance().getLibpolyVariable(mainVar), poly::Value(poly::Integer(1)));
		poly::Value eval_val = poly::evaluate(poly_p, assignment);
		CARL_LOG_DEBUG("carl.ran.libpoly", " Got eval_val " << eval_val);

		if (eval_val == poly::Value(poly::Integer(0))) {
			CARL_LOG_DEBUG("carl.ran.libpoly", "poly is 0 after substituting rational assignments -> nullified");
			return real_roots_result<real_algebraic_number_libpoly<Number>>::nullified_response();
		}
	}

	if (roots.empty()) {
		CARL_LOG_DEBUG("carl.ran.libpoly", "Poly has no roots");
		return real_roots_result<real_algebraic_number_libpoly<Number>>::no_roots_response();
	}

	//Sort, as we have to return the roots in ascending order
	//TODO: is that really needed?
	//See libpoly/src/polynomial/polynomial.c:1106
	std::sort(roots.begin(), roots.end(), std::less<poly::Value>());

	//turn result into real_roots_result
	std::vector<real_algebraic_number_libpoly<Number>> res;
	for (const poly::Value& val : roots) {
		auto tmp = real_algebraic_number_libpoly<Number>::create_from_value(val.get_internal());
		// filter out roots not in interval
		if (poly::contains(inter_poly, val)) {
			CARL_LOG_DEBUG("carl.ran.libpoly", " Found root " << val);
			res.emplace_back(tmp);
		}
	}

	return real_roots_result<real_algebraic_number_libpoly<Number>>::roots_response(std::move(res));
}
} // namespace carl::ran::libpoly

#endif