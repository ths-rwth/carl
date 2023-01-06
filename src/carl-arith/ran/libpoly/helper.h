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

inline mpq_class to_rational(const poly::Integer& m) {
	return *poly::detail::cast_to_gmp(&m);
}

inline mpq_class to_rational(const poly::Rational& m) {
	return *poly::detail::cast_to_gmp(&m);
}

inline mpq_class to_rational(const lp_rational_t* m) {
	return *reinterpret_cast<const mpq_class*>(m);
}


inline mpz_class to_integer(const poly::Integer& m) {
	return *poly::detail::cast_to_gmp(&m);
}

inline mpq_class to_rational(const poly::DyadicRational& m) {
	return mpq_class(to_integer(poly::numerator(m)), to_integer(poly::denominator(m)));
}

inline mpq_class to_rational(const poly::Value& m){
	switch(m.get_internal()->type){
		case lp_value_type_t::LP_VALUE_INTEGER:
			return to_rational(poly::as_integer(m));
		case lp_value_type_t::LP_VALUE_DYADIC_RATIONAL:
			return to_rational(poly::as_dyadic_rational(m));
		case lp_value_type_t::LP_VALUE_RATIONAL:
			return to_rational(poly::as_rational(m));
		default:
			CARL_LOG_ERROR("carl.converter", "Cannot convert libpoly value: " << m << " to rational.");
			assert(false);
			return mpq_class(0);
	}
}

inline poly::Rational to_libpoly_rational(const mpq_class& num) {
	return poly::Rational(num);
}

//Exact for whole numbers
inline poly::DyadicRational get_libpoly_dyadic_approximation(const mpz_class& num) {
	return poly::DyadicRational(poly::Integer(num));
}

/**
 * Tries to convert num = a/b into a dyadic rational of the form c/2^d
 * We take d = precision * ceil(log(2,b)) and c = ceil((a * 2^d)/(b))
 * @return Approximation to num by a dyadic rational
 */
inline poly::DyadicRational get_libpoly_dyadic_approximation(const mpq_class& num, const unsigned int& precision) {
	CARL_LOG_DEBUG("carl.converter", "Starting Dyadic Approximation with: " << num);

	mpz_class a = num.get_num();
	mpz_class b = num.get_den();

	mpz_class d = (precision)*carl::ceil(carl::log(b)); //unsigned long
	assert(d.fits_ulong_p());

	mpz_class c = carl::ceil((a * carl::pow(mpq_class(2), d.get_ui())) / (b));

	if (!c.fits_slong_p()) {
		CARL_LOG_DEBUG("carl.converter", "Dyadic Rational: Fallback for overflow of numerator");
		poly::DyadicRational tmp = get_libpoly_dyadic_approximation(c); //Construct with arbitrary precision
		return poly::div_2exp(tmp, d.get_ui());
	}

	CARL_LOG_DEBUG("carl.converter", "Got d: " << d << " and c " << c);
	CARL_LOG_DEBUG("carl.converter", "Got dyadic number: " << poly::DyadicRational(c.get_si(), d.get_ui()));

	return poly::DyadicRational(c.get_si(), d.get_ui());
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

/**
 * Converts a libpoly dyadic interval to a carl interval
 * Keeps the bound types
 */
template<typename Number>
inline carl::Interval<Number> to_carl_interval(const poly::DyadicInterval& inter) {
	BoundType upper_bound = inter.get_internal()->a_open ? BoundType::STRICT : BoundType::WEAK ;
	BoundType lower_bound = inter.get_internal()->b_open ? BoundType::STRICT : BoundType::WEAK ;

	return carl::Interval<Number>(to_rational(poly::get_lower(inter)), lower_bound, to_rational(poly::get_upper(inter)), upper_bound);
}

/**
 * Converts a libpoly interval to a carl interval
 * Keeps the bound types
 */
template<typename Number>
inline carl::Interval<Number> to_carl_interval(const poly::Interval& inter) {
	BoundType upper_bound = inter.get_internal()->a_open ? BoundType::STRICT : BoundType::WEAK ;
	BoundType lower_bound = inter.get_internal()->b_open ? BoundType::STRICT : BoundType::WEAK ;

	return carl::Interval<Number>(to_rational(poly::get_lower(inter)), lower_bound, to_rational(poly::get_upper(inter)), upper_bound);
}

} // namespace carl

#endif