#pragma once

// TODO cleanup, move to ran folder

#include <carl-common/config.h>
#ifdef USE_LIBPOLY

#include <algorithm>
#include <map>

#include <carl-arith/poly/umvpoly/Term.h>
#include <carl-arith/interval/Interval.h>
#include <carl-arith/poly/umvpoly/UnivariatePolynomial.h>
#include <carl-arith/core/Variable.h>
#include <carl-arith/core/Variables.h>
#include <carl-arith/poly/umvpoly/functions/Degree.h>
#include <carl-arith/numbers/conversion/cln_gmp.h>
#include <carl-arith/core/Common.h>
#include <carl-common/meta/SFINAE.h>

#include <poly/polyxx.h>

namespace carl {

inline carl::UnivariatePolynomial<mpq_class> to_carl_univariate_polynomial(const poly::UPolynomial& p, const carl::Variable& main_var){
	CARL_LOG_FUNC("carl.converter", p << ", " << main_var);
	std::vector<mpq_class> carl_coeffs;
	for (const poly::Integer& coeff : poly::coefficients(p)) {
		carl_coeffs.emplace_back(*(poly::detail::cast_to_gmp(&coeff)));
	}
	CARL_LOG_TRACE("carl.converter", "-> coefficients: " << carl_coeffs);
	return carl::UnivariatePolynomial<mpq_class>(main_var, carl_coeffs);
}

template<typename Coeff, EnableIf<is_subset_of_rationals_type<Coeff>> = dummy>
inline poly::UPolynomial to_libpoly_upolynomial(const carl::UnivariatePolynomial<Coeff>& p) {
    CARL_LOG_FUNC("carl.converter", p);

    mpz_class denominator;

    if (carl::is_constant(p)) {
        CARL_LOG_TRACE("carl.converter", "Poly is constant");
        denominator = carl::get_denom(p.lcoeff());
        CARL_LOG_TRACE("carl.converter", "Coprime Factor/ Denominator: " << denominator);
        return poly::UPolynomial(poly::Integer(carl::get_num(p.lcoeff())));
    }
    Coeff coprimeFactor = p.coprime_factor();

    CARL_LOG_TRACE("carl.converter", "Coprime Factor: " << coprimeFactor);

    if (carl::get_denom(coprimeFactor) != 1) {
        // if coprime factor is not an integer
        denominator = coprimeFactor > 0 ? mpz_class(carl::get_num(coprimeFactor)) : mpz_class(-carl::get_num(coprimeFactor));
    } else if (coprimeFactor == 0) {
        // Wie kann das überhaupt sein? TODO
        denominator = mpz_class(1);
    } else {
        denominator = mpz_class(coprimeFactor);
    }
    CARL_LOG_TRACE("carl.converter", "Coprime factor/ denominator: " << denominator);

    std::vector<poly::Integer> coefficients;
    for (const auto& c : p.coefficients()) {
        coefficients.emplace_back(mpz_class(c * denominator));
    }
    CARL_LOG_TRACE("carl.converter", "-> coefficients: " << coefficients);
    return poly::UPolynomial(coefficients);
}

inline mpz_class to_integer(const lp_integer_t* m) {
	return *reinterpret_cast<const mpz_class*>(m);
}

inline mpq_class to_rational(const lp_integer_t* m) {
	return mpq_class(to_integer(m));
}

inline mpq_class to_rational(const lp_rational_t* m) {
	return *reinterpret_cast<const mpq_class*>(m);
}

inline mpq_class to_rational(const lp_dyadic_rational_t* m) {
	lp_integer_t num;
	lp_integer_construct(&num);
	lp_dyadic_rational_get_num(m, &num);

	lp_integer_t den;
	lp_integer_construct(&den);
	lp_dyadic_rational_get_den(m, &den);

	auto res = mpq_class(to_integer(&num), to_integer(&den));

	lp_integer_destruct(&num);
	lp_integer_destruct(&den);

	return res;
}

inline mpq_class to_rational(const lp_value_t* m){
	switch(m->type){
		case lp_value_type_t::LP_VALUE_INTEGER:
			return to_rational(&m->value.z);
		case lp_value_type_t::LP_VALUE_RATIONAL:
			return to_rational(&m->value.q);
		case lp_value_type_t::LP_VALUE_DYADIC_RATIONAL:
		 	return to_rational(&m->value.dy_q);
		default:
			CARL_LOG_ERROR("carl.converter", "Cannot convert libpoly value: " << m << " to rational.");
			assert(false);
			return mpq_class(0);
	}
}

/**
 * Convert a carl interval to a libpoly interval
 * This keeps the bound types
 */
inline poly::Interval to_libpoly_interval(const carl::Interval<mpq_class>& inter) {
	poly::Value low;
	bool low_open;
	switch (inter.lower_bound_type()) {
	case BoundType::STRICT:
		low = poly::Value(poly::Rational(inter.lower()));
		low_open = true;
		break;
	case BoundType::WEAK:
		low = poly::Value(poly::Rational(inter.lower()));
		low_open = false;
		break;
	case BoundType::INFTY:
		low = poly::Value::minus_infty();
		low_open = true;
		break;
	}
	poly::Value up;
	bool up_open;
	switch (inter.upper_bound_type()) {
	case BoundType::STRICT:
		up = poly::Value(poly::Rational(inter.upper()));
		up_open = true;
		break;
	case BoundType::WEAK:
		up = poly::Value(poly::Rational(inter.upper()));
		up_open = false;
		break;
	case BoundType::INFTY:
		up = poly::Value::plus_infty();
		up_open = false;
		break;
	}
	return poly::Interval(low, low_open, up, up_open);
}

} // namespace carl

#endif