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

inline carl::UnivariatePolynomial<mpq_class> to_carl_univariate_polynomial(const lp_upolynomial_t* p, const carl::Variable& main_var){
	CARL_LOG_FUNC("carl.converter", p << ", " << main_var);
	
	lp_integer_t coeffs[lp_upolynomial_degree(p) + 1];
    for (std::size_t i = 0; i < lp_upolynomial_degree(p) + 1; ++i) {
    	lp_integer_construct(&coeffs[i]);
    }
    lp_upolynomial_unpack(p, coeffs);

    std::vector<mpq_class> carl_coeffs;
    for (std::size_t i = 0; i < lp_upolynomial_degree(p) + 1; ++i) {
      carl_coeffs.emplace_back(to_rational(&coeffs[i]));
      lp_integer_destruct(&coeffs[i]);
    }
	
	CARL_LOG_TRACE("carl.converter", "-> coefficients: " << carl_coeffs);
	return carl::UnivariatePolynomial<mpq_class>(main_var, carl_coeffs);
}


template<typename Coeff, EnableIf<is_subset_of_rationals_type<Coeff>> = dummy>
inline lp_upolynomial_t* to_libpoly_upolynomial(const carl::UnivariatePolynomial<Coeff>& p) {
    CARL_LOG_FUNC("carl.converter", p);

    if (carl::is_constant(p)) {
        CARL_LOG_TRACE("carl.converter", "Poly is constant");
		assert(carl::get_denom(p.lcoeff())>0);
		lp_upolynomial_t* res;
		lp_upolynomial_construct(lp_Z, 0, (lp_integer_t*)mpz_class(carl::get_num(p.lcoeff())).get_mpz_t());
        return res;
    }

    Coeff coprimeFactor = p.coprime_factor();
    CARL_LOG_TRACE("carl.converter", "Coprime Factor: " << coprimeFactor);
	mpz_class denominator;
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

    std::vector<mpz_class> coefficients;
	std::vector<const lp_integer_t*> coefficients_ptr;
    for (const auto& c : p.coefficients()) {
        coefficients.emplace_back(c * denominator);
		coefficients_ptr.emplace_back((lp_integer_t*)coefficients.back().get_mpz_t());
    }
	lp_upolynomial_t* res;
	lp_upolynomial_construct(lp_Z, p.coefficients().size()-1, reinterpret_cast<const lp_integer_t*>(coefficients_ptr.data()));
    return res;
}

/**
 * Convert a carl interval to a libpoly interval
 * This keeps the bound types
 */
inline lp_interval_t* to_libpoly_interval(const carl::Interval<mpq_class>& inter) {
	lp_value_t low;
	bool low_open;
	switch (inter.lower_bound_type()) {
	case BoundType::STRICT:
		lp_value_construct(&low, LP_VALUE_RATIONAL, &inter.lower());
		low_open = true;
		break;
	case BoundType::WEAK:
		lp_value_construct(&low, LP_VALUE_RATIONAL, &inter.lower());
		low_open = false;
		break;
	case BoundType::INFTY:
	default:
		low = *lp_value_minus_infinity();
		low_open = true;
		break;
	}
	lp_value_t up;
	bool up_open;
	switch (inter.upper_bound_type()) {
	case BoundType::STRICT:
		lp_value_construct(&up, LP_VALUE_RATIONAL, &inter.upper());
		up_open = true;
		break;
	case BoundType::WEAK:
		lp_value_construct(&up, LP_VALUE_RATIONAL, &inter.upper());
		up_open = false;
		break;
	case BoundType::INFTY:
	default:
		up = *lp_value_plus_infinity();
		up_open = false;
		break;
	}

	lp_interval_t* res = new lp_interval_t;
	lp_interval_construct(res, &low, low_open, &up, up_open);
	lp_value_destruct(&low);
	lp_value_destruct(&up);
	return res;
}

} // namespace carl

#endif