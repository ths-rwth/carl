#pragma once

#include <carl-common/config.h>
#ifdef USE_LIBPOLY

#include <algorithm>
#include <map>

#include <carl-arith/poly/umvpoly/MultivariatePolynomial.h>
#include <carl-arith/poly/umvpoly/Term.h>
#include <carl-arith/interval/Interval.h>
#include <carl-arith/poly/umvpoly/UnivariatePolynomial.h>
#include <carl-arith/core/Variable.h>
#include <carl-arith/core/Variables.h>
#include <carl-arith/poly/umvpoly/functions/Degree.h>
#include <carl-arith/numbers/conversion/cln_gmp.h>
#include <carl-arith/core/Common.h>
#include <carl-common/meta/SFINAE.h>
#include <carl-common/memory/Singleton.h>
#include "LibpolyVariableMapper.h"

#include <poly/polyxx.h>

namespace carl {

// LIBPOLY TO CARL CONVERSION
inline carl::MultivariatePolynomial<mpq_class> to_carl_multivariate_polynomial(const poly::Polynomial& p) {

    struct MonomialData {
        std::vector<carl::Term<mpq_class>> terms;
    };

    /**
     * This function is called for every monomial in a polynomial (by lp_polynomial_traverse)
     * The monomial is converted to a carl term and appended to the vector in MonomialData
     */
    auto collectMonomialData = [](const lp_polynomial_context_t* /*ctx*/, lp_monomial_t* m, void* d) {
        MonomialData* data = static_cast<MonomialData*>(d);
        carl::Term<mpq_class> term(mpz_class(&m->a));                                                              // m->a is the integer coefficient
        for (size_t i = 0; i < m->n; i++) {                                                                        // m->n is the capacity of the power array
            term *= carl::Term<mpq_class>(1, VariableMapper::getInstance().getCarlVariable(m->p[i].x), m->p[i].d); // p[i].x is the variable, p[i].d is the power
        }
        data->terms.emplace_back(term);
    };

    MonomialData data;
    CARL_LOG_DEBUG("carl.converter", "Converting Libpoly Polynomial " << p);
    lp_polynomial_traverse(p.get_internal(), collectMonomialData, &data);

    if (data.terms.empty()) {
        CARL_LOG_DEBUG("carl.converter", "Empty Poly, returning 0");
        return carl::MultivariatePolynomial<mpq_class>();
    } else {
        CARL_LOG_DEBUG("carl.converter", "Found Terms: " << data.terms);
        return carl::MultivariatePolynomial<mpq_class>(data.terms);
    }
}

inline carl::MultivariatePolynomial<mpq_class> to_carl_multivariate_polynomial(const poly::Polynomial& p, const mpz_class& denominator) {
    return (to_carl_multivariate_polynomial(p) / mpq_class(denominator));
}

inline carl::UnivariatePolynomial<mpq_class> to_carl_univariate_polynomial(const poly::UPolynomial& p, const carl::Variable& mainVar){
	CARL_LOG_DEBUG("carl.converter", "Converting Libpoly UPolynomial " << p);
	CARL_LOG_DEBUG("carl.converter", "Got Carl mainVar " << mainVar);
	std::vector<mpq_class> carl_coeffs;
	for (const poly::Integer& coeff : poly::coefficients(p)) {
		carl_coeffs.emplace_back(*(poly::detail::cast_to_gmp(&coeff)));
	}
	CARL_LOG_DEBUG("carl.converter", "Coefficient List" << carl_coeffs);
	return carl::UnivariatePolynomial<mpq_class>(mainVar, carl_coeffs);
}

inline carl::UnivariatePolynomial<mpq_class> to_carl_univariate_polynomial(const poly::UPolynomial& p, const mpz_class& denominator, carl::Variable& mainVar){
	return (to_carl_univariate_polynomial(p, mainVar) / mpq_class(denominator));
}

// CARL TO LIBPOLY CONVERSION -> These methods cannot be easily placed in the cpp file because of some linker issues due to templates

template<typename Coeff, EnableIf<is_subset_of_rationals_type<Coeff>> = dummy>
inline poly::Polynomial to_libpoly_polynomial(const carl::MultivariatePolynomial<Coeff>& p) {
    mpz_class placeholder;
    return to_libpoly_polynomial(p, placeholder);
}

template<typename Coeff, EnableIf<is_subset_of_rationals_type<Coeff>> = dummy>
inline poly::Polynomial to_libpoly_polynomial(const carl::MultivariatePolynomial<Coeff>& p, mpz_class& denominator) {
    CARL_LOG_DEBUG("carl.converter", "Converting Carl Multivariate Poly " << p);
    if (carl::is_constant(p)) {
        CARL_LOG_DEBUG("carl.converter", "Poly is constant");
        denominator = carl::get_denom(p.constant_part());
        auto res = poly::Polynomial(poly::Integer(carl::get_num(p.constant_part())));
        return res;
    }
    // Libpoly polynomials have integer coefficients -> so we have to store the lcm of the coefficients of every term
    Coeff coprimeFactor = p.main_denom();
    if (carl::get_denom(coprimeFactor) != 1) {
        // if coprime factor is not an integer
        denominator = coprimeFactor > 0 ? mpz_class(1) : mpz_class(-1);
    } else if (coprimeFactor == 0) {
        // Bug? This case is just to be safe
        denominator = mpz_class(1);
    } else {
        denominator = mpz_class(coprimeFactor);
    }
    CARL_LOG_DEBUG("carl.converter", "Coprime Factor/ Denominator: " << denominator);
    poly::Polynomial res(poly::Integer(0));
    // iterate over terms
    for (const carl::Term<Coeff>& term : p) {
        // Multiply by denominator to make the coefficient an integer
        poly::Polynomial t(poly::Integer(mpz_class(term.coeff() * denominator)));
        // iterate over monomial
        if (term.monomial()) {
            // A monomial is a vector of pairs <variable, power>
            for (const std::pair<carl::Variable, std::size_t>& var_pow : (*term.monomial()).exponents()) {
                // multiply 1*var^pow
                t *= poly::Polynomial(poly::Integer(1), VariableMapper::getInstance().getLibpolyVariable(var_pow.first), (unsigned)var_pow.second);
            }
        }
        res += t;
    }
    CARL_LOG_DEBUG("carl.converter", "Got Polynomial: " << res);
    return res;
}

template<typename Coeff, EnableIf<is_subset_of_rationals_type<Coeff>> = dummy>
inline poly::UPolynomial to_libpoly_upolynomial(const carl::UnivariatePolynomial<Coeff>& p, mpz_class& denominator, carl::Variable& mainVar) {
    CARL_LOG_DEBUG("carl.converter", "Converting Carl Univariate Poly " << p);
    if (carl::is_constant(p)) {
        CARL_LOG_DEBUG("carl.converter", "Poly is constant");
        denominator = carl::get_denom(p.lcoeff());
        CARL_LOG_DEBUG("carl.converter", "Coprime Factor/ Denominator: " << denominator);
        return poly::UPolynomial(poly::Integer(carl::get_num(p.lcoeff())));
    }
    Coeff coprimeFactor = p.coprime_factor();

    CARL_LOG_DEBUG("carl.converter", "Coprime Factor: " << coprimeFactor);

    if (carl::get_denom(coprimeFactor) != 1) {
        // if coprime factor is not an integer
        denominator = coprimeFactor > 0 ? mpz_class(carl::get_num(coprimeFactor)) : mpz_class(-carl::get_num(coprimeFactor));
    } else if (coprimeFactor == 0) {
        // Wie kann das überhaupt sein? TODO
        denominator = mpz_class(1);
    } else {
        denominator = mpz_class(coprimeFactor);
    }
    CARL_LOG_DEBUG("carl.converter", "Coprime Factor/ Denominator: " << denominator);

    mainVar = p.main_var();
    CARL_LOG_DEBUG("carl.converter", "Main Variable: " << mainVar);
    std::vector<poly::Integer> coefficients;
    for (const auto& c : p.coefficients()) {
        coefficients.emplace_back(mpz_class(c * denominator));
    }
    CARL_LOG_DEBUG("carl.converter", "Got Coefficients: " << coefficients);
    CARL_LOG_DEBUG("carl.converter", "Got Polynomial: " << poly::UPolynomial(coefficients));

    return poly::UPolynomial(coefficients);
}

template<typename Coeff, EnableIf<is_subset_of_rationals_type<Coeff>> = dummy>
inline poly::UPolynomial to_libpoly_upolynomial(const carl::UnivariatePolynomial<Coeff>& p, mpz_class& denominator) {
    carl::Variable placeholder;
    return to_libpoly_upolynomial(p, denominator, placeholder);
}

template<typename Coeff, EnableIf<is_subset_of_rationals_type<Coeff>> = dummy>
inline poly::UPolynomial to_libpoly_upolynomial(const carl::UnivariatePolynomial<Coeff>& p) {
    mpz_class placeholder;
    return to_libpoly_upolynomial(p, placeholder);
}


template<typename Coeff, EnableIf<is_subset_of_rationals_type<Coeff>> = dummy>
inline poly::Polynomial to_libpoly_polynomial(const carl::UnivariatePolynomial<carl::MultivariatePolynomial<Coeff>>& p) {
    mpz_class placeholder;
    return to_libpoly_polynomial(p, placeholder);
}

template<typename Coeff, EnableIf<is_subset_of_rationals_type<Coeff>> = dummy>
inline poly::Polynomial to_libpoly_polynomial(const carl::UnivariatePolynomial<carl::MultivariatePolynomial<Coeff>>& p, mpz_class& denominator) {
    // turn into carl::Multivariate
    return to_libpoly_polynomial(carl::MultivariatePolynomial(p), denominator);
}

inline carl::UnivariatePolynomial<carl::MultivariatePolynomial<mpq_class>> to_carl_univariate_polynomial(const poly::Polynomial& p) {
    return carl::to_univariate_polynomial(to_carl_multivariate_polynomial(p), VariableMapper::getInstance().getCarlVariable(poly::main_variable(p)));
}

inline carl::UnivariatePolynomial<carl::MultivariatePolynomial<mpq_class>> to_carl_univariate_polynomial(const poly::Polynomial& p, const carl::Variable& mainVar) {
    return carl::to_univariate_polynomial(to_carl_multivariate_polynomial(p), mainVar);
}

inline carl::UnivariatePolynomial<carl::MultivariatePolynomial<mpq_class>> to_carl_univariate_polynomial(const poly::Polynomial& p, const mpz_class& denominator, const carl::Variable& mainVar) {
    return carl::to_univariate_polynomial(to_carl_multivariate_polynomial(p, denominator), mainVar);
}

/**
 * More conversion methods
 */
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