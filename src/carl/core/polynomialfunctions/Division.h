#pragma once

#include "Quotient.h"
#include "to_univariate_polynomial.h"

#include "../MultivariatePolynomial.h"
#include "../UnivariatePolynomial.h"

namespace carl {

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
	static_assert(is_field<Coeff>::value);
	std::vector<Term<Coeff>> new_coeffs;
	for (const auto& t: p) {
		new_coeffs.emplace_back(divide(t, divisor));
	}
	MultivariatePolynomial<Coeff,Ordering,Policies> res(std::move(new_coeffs), false, p.isOrdered());
	assert(res.isConsistent());
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
	static_assert(is_field<Coeff>::value, "Division only defined for field coefficients");
	if (carl::isOne(divisor)) {
		quotient = dividend;
		return true;
	}
	if (carl::isZero(dividend)) {
		quotient = MultivariatePolynomial<Coeff,Ordering,Policies>();
		return true;
	}
	auto& tam = MultivariatePolynomial<Coeff,Ordering,Policies>::mTermAdditionManager;
	auto id = tam.getId(0);
	auto thisid = tam.getId(dividend.nrTerms());
	for (const auto& t: dividend) {
		tam.template addTerm<false,true>(thisid, t);
	}
	while (true) {
		Term<Coeff> factor = tam.getMaxTerm(thisid);
		if (carl::isZero(factor)) break;
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
	tam.readTerms(id, quotient.getTerms());
	tam.dropTerms(thisid);
	quotient.reset_ordered();
	quotient.template makeMinimallyOrdered<false, true>();
	assert(quotient.isConsistent());
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
	static_assert(is_field<Coeff>::value, "Division only defined for field coefficients");
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
			p.stripLT();
		}
	}
	assert(q.isConsistent());
	assert(r.isConsistent());
	assert(dividend == q * divisor + r);
	return DivisionResult<MultivariatePolynomial<Coeff,Ordering,Policies>> {q,r};
}

template<typename Coeff>
bool try_divide(const UnivariatePolynomial<Coeff>& dividend, const Coeff& divisor, UnivariatePolynomial<Coeff>& quotient) {
	static_assert(!is_field<Coeff>::value);
	static_assert(!is_number<Coeff>::value);
	assert(dividend.isConsistent());
	assert(divisor.isConsistent());
	Coeff quo;
	bool res = try_divide(Coeff(dividend), divisor, quo);
	CARL_LOG_TRACE("carl.core", Coeff(dividend) << " / " << divisor << " = " << quo);
	assert(quo.isConsistent());
	if (res) quotient = carl::to_univariate_polynomial(quo, dividend.mainVar());
	return res;
}

template<typename Coeff>
DivisionResult<UnivariatePolynomial<Coeff>> divide(const UnivariatePolynomial<Coeff>& p, const Coeff& divisor) {
	assert(p.isConsistent());
	assert(divisor.isConsistent());
	if constexpr(is_field<Coeff>::value) {
		UnivariatePolynomial<Coeff> res(p);
		for (auto& c: res.coefficients()) {
			c = c / divisor;
		}
		return DivisionResult<UnivariatePolynomial<Coeff>> {res, UnivariatePolynomial(p.mainVar()) };
	} else {
		CARL_LOG_ERROR("carl.core", "Called divide() with non-field number divisor " << divisor);
		return p;
	}
}

template<typename Coeff>
DivisionResult<UnivariatePolynomial<Coeff>> divide(const UnivariatePolynomial<Coeff>& p, const typename UnderlyingNumberType<Coeff>::type& divisor) {
	assert(p.isConsistent());
	static_assert(is_field<typename UnderlyingNumberType<Coeff>::type>::value);

	UnivariatePolynomial<Coeff> res(p);
	assert(res.isConsistent());
	for (auto& c: res.coefficients()) {
		c = divide(c, divisor);
	}
	assert(res.isConsistent());
	return DivisionResult<UnivariatePolynomial<Coeff>> {res, UnivariatePolynomial<Coeff>(p.mainVar())};
}

template<typename Coeff>
DivisionResult<UnivariatePolynomial<Coeff>> divide(const UnivariatePolynomial<Coeff>& dividend, const UnivariatePolynomial<Coeff>& divisor) {
	if constexpr (is_integer<Coeff>::value) {
		assert(!carl::isZero(divisor));
		DivisionResult<UnivariatePolynomial<Coeff>> result {UnivariatePolynomial<Coeff>(dividend.mainVar()), dividend};
		if(carl::isZero(dividend)) return result;
		assert(dividend == divisor * result.quotient + result.remainder);

		std::vector<Coeff> coeffs(1+dividend.coefficients().size()-divisor.coefficients().size(), Coeff(0));

		uint degdiff = dividend.degree() - divisor.degree();
		for (std::size_t offset = 0; offset <= degdiff; offset++) {
			Coeff factor = carl::quotient(result.remainder.coefficients()[dividend.degree()-offset], divisor.lcoeff());
			result.remainder -= UnivariatePolynomial<Coeff>(dividend.mainVar(), factor, degdiff - offset) * divisor;
			coeffs[degdiff-offset] += factor;
		}
		result.quotient = UnivariatePolynomial<Coeff>(dividend.mainVar(), std::move(coeffs));
		assert(dividend == divisor * result.quotient + result.remainder);
		return result;
	} else if constexpr (is_field<Coeff>::value) {
		assert(!carl::isZero(divisor));
		assert(dividend.mainVar() == divisor.mainVar());
		DivisionResult<UnivariatePolynomial<Coeff>> result {UnivariatePolynomial<Coeff>(dividend.mainVar()), dividend};
		if(carl::isZero(dividend)) return result;
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
			result.remainder -= UnivariatePolynomial<Coeff>(dividend.mainVar(), factor, degdiff) * divisor;
			coeffs[degdiff] += factor;
		}
		while(!carl::isZero(result.remainder) && divisor.degree() <= result.remainder.degree());
		result.quotient = UnivariatePolynomial<Coeff>(dividend.mainVar(), std::move(coeffs));
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
