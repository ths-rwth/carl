#pragma once

#include "Quotient.h"
#include "to_univariate_polynomial.h"

#include "../MultivariatePolynomial.h"
#include "../UnivariatePolynomial.h"

namespace carl {

/**
 * A strongly typed pair encoding the result of a division, 
 * being a quotient and a remainder.
 */
template<typename Type>
struct DivisionResult {
	Type quotient;
	Type remainder;
};

template<typename Coeff>
Term<Coeff> divide(const Term<Coeff>& t, const Coeff& c) {
	assert(!is_zero(c));
	return Term<Coeff>(t.coeff() / c, t.monomial());
}

template<typename Coeff>
bool try_divide(const Term<Coeff>& t, const Coeff& c, Term<Coeff>& res) {
	if (is_zero(c)) return false;
	res.coeff() = t.coeff() / c;
	res.monomial() = t.monomial();
	return true;
}

template<typename Coeff>
bool try_divide(const Term<Coeff>& t, Variable v, Term<Coeff>& res) {
	if (!t.monomial()) return false;
	if (t.monomial()->divide(v, res.monomial())) {
		res.coeff() = t.coeff();
		return true;
	}
	return false;
}

/**
 * Divides the polynomial by the given coefficient.
 * Applies if the coefficients are from a field.
 * @param divisor
 * @return 
 */
template<typename Coeff, typename Ordering, typename Policies>
MultivariatePolynomial<Coeff,Ordering,Policies> divide(const MultivariatePolynomial<Coeff,Ordering,Policies>& p, const Coeff& divisor) {
	static_assert(is_field_type<Coeff>::value);
	std::vector<Term<Coeff>> new_coeffs;
	for (const auto& t: p) {
		new_coeffs.emplace_back(divide(t, divisor));
	}
	MultivariatePolynomial<Coeff,Ordering,Policies> res(std::move(new_coeffs), false, p.isOrdered());
	assert(res.is_consistent());
	return res;
}

/**
 * Divides the polynomial by another polynomial.
 * If the divisor divides this polynomial, quotient contains the result of the division and true is returned.
 * Otherwise, false is returned and the content of quotient remains unchanged.
 * Applies if the coefficients are from a field.
 * Note that the quotient must not be *this.
 * @param divisor
 * @param quotient
 * @return 
 */
template<typename Coeff, typename Ordering, typename Policies>
bool try_divide(const MultivariatePolynomial<Coeff,Ordering,Policies>& dividend, const MultivariatePolynomial<Coeff,Ordering,Policies>& divisor, MultivariatePolynomial<Coeff,Ordering,Policies>& quotient) {
	static_assert(is_field_type<Coeff>::value, "Division only defined for field coefficients");
	if (carl::is_one(divisor)) {
		quotient = dividend;
		return true;
	}
	if (carl::is_zero(dividend)) {
		quotient = MultivariatePolynomial<Coeff,Ordering,Policies>();
		return true;
	}
	auto& tam = MultivariatePolynomial<Coeff,Ordering,Policies>::mTermAdditionManager;
	auto id = tam.getId(0);
	auto thisid = tam.getId(dividend.nr_terms());
	for (const auto& t: dividend) {
		tam.template addTerm<false,true>(thisid, t);
	}
	while (true) {
		Term<Coeff> factor = tam.getMaxTerm(thisid);
		if (carl::is_zero(factor)) break;
		if (factor.divide(divisor.lterm(), factor)) {
			for (const auto& t: divisor) {
				tam.template addTerm<true,true>(thisid, -factor*t);
			}
			//res.subtractProduct(factor, divisor);
			//p -= factor * divisor;
			tam.template addTerm<true>(id, factor);
		} else {
			return false;
		}
	}
	tam.readTerms(id, quotient.terms());
	tam.dropTerms(thisid);
	quotient.reset_ordered();
	quotient.template makeMinimallyOrdered<false, true>();
	assert(quotient.is_consistent());
	return true;
}

/**
 * Calculating the quotient and the remainder, such that for a given polynomial p we have
 * p = divisor * quotient + remainder.
 * @param divisor Another polynomial
 * @return A divisionresult, holding the quotient and the remainder.
 * @see
 * @note Division is only defined on fields
 */
template<typename Coeff, typename Ordering, typename Policies>
DivisionResult<MultivariatePolynomial<Coeff,Ordering,Policies>> divide(const MultivariatePolynomial<Coeff,Ordering,Policies>& dividend, const MultivariatePolynomial<Coeff,Ordering,Policies>& divisor) {
	static_assert(is_field_type<Coeff>::value, "Division only defined for field coefficients");
	MultivariatePolynomial<Coeff,Ordering,Policies> q;
	MultivariatePolynomial<Coeff,Ordering,Policies> r;
	MultivariatePolynomial<Coeff,Ordering,Policies> p = dividend;
	while(!is_zero(p)) {
		Term<Coeff> factor;
		if (try_divide(p.lterm(), divisor.lterm(), factor)) {
			q += factor;
			p.subtractProduct(factor, divisor);
			//p -= factor * divisor;
		} else {
			r += p.lterm();
			p.strip_lterm();
		}
	}
	assert(q.is_consistent());
	assert(r.is_consistent());
	assert(dividend == q * divisor + r);
	return DivisionResult<MultivariatePolynomial<Coeff,Ordering,Policies>> {q,r};
}

template<typename Coeff>
bool try_divide(const UnivariatePolynomial<Coeff>& dividend, const Coeff& divisor, UnivariatePolynomial<Coeff>& quotient) {
	static_assert(!is_field_type<Coeff>::value);
	static_assert(!is_number_type<Coeff>::value);
	assert(dividend.is_consistent());
	assert(divisor.is_consistent());
	Coeff quo;
	bool res = try_divide(Coeff(dividend), divisor, quo);
	CARL_LOG_TRACE("carl.core", Coeff(dividend) << " / " << divisor << " = " << quo);
	assert(quo.is_consistent());
	if (res) quotient = carl::to_univariate_polynomial(quo, dividend.main_var());
	return res;
}

template<typename Coeff>
DivisionResult<UnivariatePolynomial<Coeff>> divide(const UnivariatePolynomial<Coeff>& p, const Coeff& divisor) {
	assert(p.is_consistent());
	assert(divisor.is_consistent());
	if constexpr(is_field_type<Coeff>::value) {
		UnivariatePolynomial<Coeff> res(p);
		for (auto& c: res.coefficients()) {
			c = c / divisor;
		}
		return DivisionResult<UnivariatePolynomial<Coeff>> {res, UnivariatePolynomial(p.main_var()) };
	} else {
		CARL_LOG_ERROR("carl.core", "Called divide() with non-field number divisor " << divisor);
		return p;
	}
}

template<typename Coeff>
DivisionResult<UnivariatePolynomial<Coeff>> divide(const UnivariatePolynomial<Coeff>& p, const typename UnderlyingNumberType<Coeff>::type& divisor) {
	assert(p.is_consistent());
	static_assert(is_field_type<typename UnderlyingNumberType<Coeff>::type>::value);

	UnivariatePolynomial<Coeff> res(p);
	assert(res.is_consistent());
	for (auto& c: res.coefficients()) {
		c = divide(c, divisor);
	}
	assert(res.is_consistent());
	return DivisionResult<UnivariatePolynomial<Coeff>> {res, UnivariatePolynomial<Coeff>(p.main_var())};
}

/**
 * Divides the polynomial by another polynomial.
 * @param dividend Dividend.
 * @param divisor Divisor.
 * @return dividend / divisor.
 */
template<typename Coeff>
DivisionResult<UnivariatePolynomial<Coeff>> divide(const UnivariatePolynomial<Coeff>& dividend, const UnivariatePolynomial<Coeff>& divisor) {
	if constexpr (is_integer_type<Coeff>::value) {
		assert(!carl::is_zero(divisor));
		DivisionResult<UnivariatePolynomial<Coeff>> result {UnivariatePolynomial<Coeff>(dividend.main_var()), dividend};
		if(carl::is_zero(dividend)) return result;
		assert(dividend == divisor * result.quotient + result.remainder);

		std::vector<Coeff> coeffs(1+dividend.coefficients().size()-divisor.coefficients().size(), Coeff(0));

		uint degdiff = dividend.degree() - divisor.degree();
		for (std::size_t offset = 0; offset <= degdiff; offset++) {
			Coeff factor = carl::quotient(result.remainder.coefficients()[dividend.degree()-offset], divisor.lcoeff());
			result.remainder -= UnivariatePolynomial<Coeff>(dividend.main_var(), factor, degdiff - offset) * divisor;
			coeffs[degdiff-offset] += factor;
		}
		result.quotient = UnivariatePolynomial<Coeff>(dividend.main_var(), std::move(coeffs));
		assert(dividend == divisor * result.quotient + result.remainder);
		return result;
	} else if constexpr (is_field_type<Coeff>::value) {
		assert(!carl::is_zero(divisor));
		assert(dividend.main_var() == divisor.main_var());
		DivisionResult<UnivariatePolynomial<Coeff>> result {UnivariatePolynomial<Coeff>(dividend.main_var()), dividend};
		if(carl::is_zero(dividend)) return result;
		assert(dividend == divisor * result.quotient + result.remainder);
		if(divisor.degree() > dividend.degree())
		{
			return result;
		}
		std::vector<Coeff> coeffs(1+dividend.coefficients().size()-divisor.coefficients().size(), Coeff(0));
		
		do
		{
			Coeff factor = result.remainder.lcoeff()/divisor.lcoeff();
			uint degdiff = result.remainder.degree() - divisor.degree();
			result.remainder -= UnivariatePolynomial<Coeff>(dividend.main_var(), factor, degdiff) * divisor;
			coeffs[degdiff] += factor;
		}
		while(!carl::is_zero(result.remainder) && divisor.degree() <= result.remainder.degree());
		result.quotient = UnivariatePolynomial<Coeff>(dividend.main_var(), std::move(coeffs));
		assert(dividend == divisor * result.quotient + result.remainder);
		return result;
	} else {
		assert(false);
		return DivisionResult<UnivariatePolynomial<Coeff>>();
	}
}

template<typename C, typename O, typename P>
MultivariatePolynomial<C,O,P> operator/(const MultivariatePolynomial<C,O,P>& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	MultivariatePolynomial<C,O,P> res;
	bool flag = try_divide(lhs, rhs, res);
	assert(flag);
	return res;
}

}
