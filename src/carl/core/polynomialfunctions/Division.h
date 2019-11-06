#pragma once

#include "Quotient.h"
#include "to_univariate_polynomial.h"

#include "../MultivariatePolynomial.h"
#include "../UnivariatePolynomial.h"

namespace carl {

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
