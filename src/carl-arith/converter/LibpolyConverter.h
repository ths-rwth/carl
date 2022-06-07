#pragma once

#include <carl-common/config.h>

#if defined(USE_LIBPOLY) || defined(RAN_USE_LIBPOLY)

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

class LibpolyConverter : public Singleton<LibpolyConverter> {

public:
	// LIBPOLY TO CARL CONVERSION
	carl::MultivariatePolynomial<mpq_class> toCarlMultivariatePolynomial(const poly::Polynomial& p);
	carl::MultivariatePolynomial<mpq_class> toCarlMultivariatePolynomial(const poly::Polynomial& p, const mpz_class& denominator);

	carl::UnivariatePolynomial<mpq_class> toCarlUnivariatePolynomial(const poly::UPolynomial& p, const carl::Variable& mainVar);
	carl::UnivariatePolynomial<mpq_class> toCarlUnivariatePolynomial(const poly::UPolynomial& p, const mpz_class& denominator, carl::Variable& mainVar);

	// CARL TO LIBPOLY CONVERSION -> These methods cannot be easily placed in the cpp file because of some linker issues due to templates

	template<typename Coeff, EnableIf<is_subset_of_rationals_type<Coeff>> = dummy>
	poly::Polynomial toLibpolyPolynomial(const carl::MultivariatePolynomial<Coeff>& p) {
		mpz_class placeholder;
		return toLibpolyPolynomial(p, placeholder);
	}
	
	template<typename Coeff, EnableIf<is_subset_of_rationals_type<Coeff>> = dummy>
	poly::Polynomial toLibpolyPolynomial(const carl::MultivariatePolynomial<Coeff>& p, mpz_class& denominator) {
		CARL_LOG_DEBUG("carl.converter", "Converting Carl Multivariate Poly " << p);
		if (carl::is_constant(p)) {
			CARL_LOG_DEBUG("carl.converter", "Poly is constant");
			denominator = carl::get_denom(p.constant_part());
			auto res = poly::Polynomial(poly::Integer(carl::get_num(p.constant_part())));
			return res;
		}
		//Libpoly polynomials have integer coefficients -> so we have to store the lcm of the coefficients of every term
		Coeff coprimeFactor = p.main_denom();
		if (carl::get_denom(coprimeFactor) != 1) {
			//if coprime factor is not an integer
			denominator = coprimeFactor > 0 ? mpz_class(1) : mpz_class(-1);
		} else if (coprimeFactor == 0) {
			//Bug? This case is just to be safe 
			denominator = mpz_class(1);
		} else {
			denominator = mpz_class(coprimeFactor);
		}
		CARL_LOG_DEBUG("carl.converter", "Coprime Factor/ Denominator: " << denominator);
		poly::Polynomial res(poly::Integer(0));
		//iterate over terms
		for (const carl::Term<Coeff>& term : p) {
			//Multiply by denominator to make the coefficient an integer
			poly::Polynomial t(poly::Integer(mpz_class(term.coeff() * denominator)));
			//iterate over monomial
			if (term.monomial()) {
				//A monomial is a vector of pairs <variable, power>
				for (const std::pair<carl::Variable, std::size_t>& var_pow : (*term.monomial()).exponents()) {
					//multiply 1*var^pow
					t *= poly::Polynomial(poly::Integer(1), VariableMapper::getInstance().getLibpolyVariable(var_pow.first), (unsigned)var_pow.second);
				}
			}
			res += t;
		}
		CARL_LOG_DEBUG("carl.converter", "Got Polynomial: " << res);
		return res;
	}
	
	template<typename Coeff, EnableIf<is_subset_of_rationals_type<Coeff>> = dummy>
	poly::UPolynomial toLibpolyUPolynomial(const carl::UnivariatePolynomial<Coeff>& p) {
		mpz_class placeholder;
		return toLibpolyUPolynomial(p, placeholder);
	}

	template<typename Coeff, EnableIf<is_subset_of_rationals_type<Coeff>> = dummy>
	poly::UPolynomial toLibpolyUPolynomial(const carl::UnivariatePolynomial<Coeff>& p, mpz_class& denominator) {
		carl::Variable placeholder;
		return toLibpolyUPolynomial(p, denominator, placeholder);
	}

	template<typename Coeff, EnableIf<is_subset_of_rationals_type<Coeff>> = dummy>
	poly::UPolynomial toLibpolyUPolynomial(const carl::UnivariatePolynomial<Coeff>& p, mpz_class& denominator, carl::Variable& mainVar) {
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
			//if coprime factor is not an integer
			denominator = coprimeFactor > 0 ? mpz_class(carl::get_num(coprimeFactor)) : mpz_class(-carl::get_num(coprimeFactor));
		} else if (coprimeFactor == 0) {
			//Wie kann das überhaupt sein? TODO
			denominator = mpz_class(1);
		} else {
			denominator = mpz_class(coprimeFactor);
		}
		CARL_LOG_DEBUG("carl.converter", "Coprime Factor/ Denominator: " << denominator);

		mainVar = p.mainVar();
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
	poly::Polynomial toLibpolyPolynomial(const carl::UnivariatePolynomial<carl::MultivariatePolynomial<Coeff>>& p) {
		mpz_class placeholder;
		return toLibpolyPolynomial(p, placeholder);
	}

	template<typename Coeff, EnableIf<is_subset_of_rationals_type<Coeff>> = dummy>
	poly::Polynomial toLibpolyPolynomial(const carl::UnivariatePolynomial<carl::MultivariatePolynomial<Coeff>>& p, mpz_class& denominator) {
		//turn into carl::Multivariate
		return toLibpolyPolynomial(carl::MultivariatePolynomial(p), denominator);
	}

	carl::UnivariatePolynomial<carl::MultivariatePolynomial<mpq_class>> toCarlUnivariatePolynomial(const poly::Polynomial& p) {
		return carl::to_univariate_polynomial(toCarlMultivariatePolynomial(p), VariableMapper::getInstance().getCarlVariable(poly::main_variable(p)));
	}

	carl::UnivariatePolynomial<carl::MultivariatePolynomial<mpq_class>> toCarlUnivariatePolynomial(const poly::Polynomial& p, const carl::Variable& mainVar) {
		return carl::to_univariate_polynomial(toCarlMultivariatePolynomial(p), mainVar);
	}

	carl::UnivariatePolynomial<carl::MultivariatePolynomial<mpq_class>> toCarlUnivariatePolynomial(const poly::Polynomial& p, const mpz_class& denominator, const carl::Variable& mainVar) {
		return carl::to_univariate_polynomial(toCarlMultivariatePolynomial(p, denominator), mainVar);
	}



	/**
	 * More conversion methods 
	 */
	template<typename Number>
	Number toNumber(const mpz_class& m);
	template<typename Number>
	Number toNumber(const mpq_class& m);
	template<typename Number>
	Number toNumber(const poly::Integer& m);
	template<typename Number>
	Number toNumber(const poly::Rational& m);
	template<typename Number>
	Number toNumber(const lp_rational_t* m);
	template<typename Number>
	Number toNumber(const poly::DyadicRational& m);
	template<typename Number>
	carl::Interval<Number> toInterval(const poly::DyadicInterval& inter) ;
	template<typename Number>
	carl::Interval<Number> toInterval(const poly::Interval& inter) ;
	template<typename Number>
	poly::Interval toInterval(const carl::Interval<Number>& inter);
	template<typename Number>
	poly::Rational toLibpolyRational(const Number& num);
	template<typename Number>
	poly::DyadicRational getDyadicApproximation(const Number& num, const unsigned int& precision = 2);
};

} // namespace carl

#endif