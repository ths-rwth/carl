#pragma once

#include "Degree.h"
#include "Quotient.h"
#include "to_univariate_polynomial.h"

#include "../MultivariatePolynomial.h"
#include "../UnivariatePolynomial.h"

namespace carl {

template<typename Coeff>
UnivariatePolynomial<Coeff> remainder_helper(const UnivariatePolynomial<Coeff>& dividend, const UnivariatePolynomial<Coeff>& divisor, const Coeff* prefactor = nullptr) {
	assert(!carl::isZero(divisor));
	if(carl::isZero(dividend)) {
		return dividend;
	}
	if(dividend.degree() < divisor.degree()) return dividend;
	assert(dividend.degree() >= divisor.degree());
	// Remainder in a field is zero by definition.
	if (is_field<Coeff>::value && is_constant(divisor)) {
		return UnivariatePolynomial<Coeff>(dividend.mainVar());
	}

	Coeff factor(0); // We have to initialize it to prevent a compiler error.
	if(prefactor != nullptr)
	{
		factor = carl::quotient(Coeff(*prefactor * dividend.lcoeff()), divisor.lcoeff());
		// There should be no remainder.
		assert(factor * divisor.lcoeff() == *prefactor * dividend.lcoeff());
	}
	else
	{
		factor = carl::quotient(dividend.lcoeff(), divisor.lcoeff());
		// There should be no remainder.
		assert(factor * divisor.lcoeff() == dividend.lcoeff());
	}

	std::vector<Coeff> coeffs;
	uint degdiff = dividend.degree() - divisor.degree();
	if(degdiff > 0)
	{
		coeffs = std::vector<Coeff>(dividend.coefficients().begin(), dividend.coefficients().begin() + long(degdiff));
	}
	if(prefactor != nullptr)
	{
		for(Coeff& c : coeffs)
		{
			c *= *prefactor;
		}
	}
	
	// By construction, the leading coefficient will be zero.
	if(prefactor != nullptr)
	{
		for(std::size_t i = 0; i < dividend.coefficients().size() - degdiff -1; ++i)
		{
			coeffs.push_back(dividend.coefficients()[i + degdiff] * *prefactor - factor * divisor.coefficients()[i]);
		}
	}
	else
	{
		for(std::size_t i = 0; i < dividend.coefficients().size() - degdiff -1; ++i)
		{
			coeffs.push_back(dividend.coefficients()[i + degdiff] - factor * divisor.coefficients()[i]);
		}
	}
	auto result = UnivariatePolynomial<Coeff>(dividend.mainVar(), std::move(coeffs));
	// strip zeros from the end as we might have pushed zeros.
	result.stripLeadingZeroes();
	
	if(carl::isZero(result) || result.degree() < divisor.degree())
	{
		return result;
	}
	else 
	{	
		return remainder_helper(result, divisor);
	}
}

template<typename Coeff>
UnivariatePolynomial<Coeff> remainder(const UnivariatePolynomial<Coeff>& dividend, const UnivariatePolynomial<Coeff>& divisor, const Coeff& prefactor) {
	return remainder_helper(dividend, divisor, &prefactor);
}

template<typename Coeff>
UnivariatePolynomial<Coeff> remainder(const UnivariatePolynomial<Coeff>& dividend, const UnivariatePolynomial<Coeff>& divisor) {
	static_assert(is_field<Coeff>::value, "Reduce must be called with a prefactor if the Coefficients are not from a field.");
	return remainder_helper(dividend, divisor);
}

/**
 * Calculates the pseudo-remainder.
 * @see @cite GCL92, page 55, Pseudo-Division Property
 */
template<typename Coeff>
UnivariatePolynomial<Coeff> pseudo_remainder(const UnivariatePolynomial<Coeff>& dividend, const UnivariatePolynomial<Coeff>& divisor) 
{
	assert(dividend.mainVar() == divisor.mainVar());
	Variable v = dividend.mainVar();
	if (divisor.degree() == 0) return UnivariatePolynomial<Coeff>(v);
	if (divisor.degree() > dividend.degree()) return dividend;

	UnivariatePolynomial<Coeff> reduct = divisor;
	reduct.truncate();
	UnivariatePolynomial<Coeff> res = dividend;

	std::size_t reductions = 0;
	while (true) {
		if (carl::isZero(res)) {
			return res;
		}
		if (divisor.degree() > res.degree()) {
			std::size_t degdiff = dividend.degree() - divisor.degree() + 1;
			if (reductions < degdiff) {
				res *= carl::pow(divisor.lcoeff(), degdiff - reductions);
			}
			return res;
		}
		std::vector<Coeff> newR(res.degree());
		Coeff lc = res.lcoeff();
		for (std::size_t i = 0; i < res.degree(); i++) {
			newR[i] = res.coefficients()[i] * divisor.lcoeff();
			assert(!newR[i].has(v));
		}
		if (res.degree() == divisor.degree()) {
			if (!carl::isZero(reduct)) {
				for (std::size_t i = 0; i <= reduct.degree(); i++) {
					newR[i] -= lc * reduct.coefficients()[i];
					assert(!newR[i].has(v));
				}
			}
		} else {
			assert(!lc.has(v));
			if (!carl::isZero(reduct)) {
				for (std::size_t i = 0; i <= reduct.degree(); i++) {
					newR[res.degree() - divisor.degree() + i] -= lc * reduct.coefficients()[i];
					assert(!newR[res.degree() - divisor.degree() + i].has(v));
				}
			}
		}
		res = UnivariatePolynomial<Coeff>(v, std::move(newR));
		reductions++;
	}
}

/**
 * Signed pseudoremainder.
 * see 
 * @param divisor
 * @return 
 */
template<typename Coeff>
UnivariatePolynomial<Coeff> signed_pseudo_remainder(const UnivariatePolynomial<Coeff>& dividend, const UnivariatePolynomial<Coeff>& divisor) {
	if(carl::isZero(dividend) || dividend.degree() < divisor.degree())
	{
		// According to definition.
		return dividend;
	}
	Coeff b = divisor.lcoeff();
	uint d = dividend.degree() - divisor.degree() + 1;
	if (d % 2 == 1) ++d;
	Coeff prefactor = pow(b,d);
	return remainder(dividend, divisor, &prefactor);
}

template<typename C, typename O, typename P>
MultivariatePolynomial<C,O,P> remainder(const MultivariatePolynomial<C,O,P>& dividend, const MultivariatePolynomial<C,O,P>& divisor) {
	static_assert(is_field<C>::value, "Division only defined for field coefficients");
	assert(!carl::isZero(divisor));
	if(&dividend == &divisor || carl::isOne(divisor) || dividend == divisor)
	{
		return MultivariatePolynomial<C,O,P>();
	}

	MultivariatePolynomial<C,O,P> remainder;
	MultivariatePolynomial p = dividend;
	while(!carl::isZero(p))
	{
		if(p.lterm().tdeg() < divisor.lterm().tdeg())
		{
			assert(!p.lterm().divisible(divisor.lterm()));
			if( O::degreeOrder )
			{
				remainder += p;
                assert(remainder.isConsistent());
				return remainder;
			}
			remainder += p.lterm();
			p.stripLT();
		}
		else
		{
			Term<C> factor;
			if (p.lterm().divide(divisor.lterm(), factor)) {
				p.subtractProduct(factor, divisor);
				//p -= factor * divisor;
			}
			else
			{
				remainder += p.lterm();
				p.stripLT();
			}
		}
	}
	assert(remainder.isConsistent());
	assert(dividend == quotient(dividend, divisor) * divisor + remainder);
	return remainder;
}

template<typename C, typename O, typename P>
MultivariatePolynomial<C,O,P> pseudo_remainder(const MultivariatePolynomial<C,O,P>& dividend, const MultivariatePolynomial<C,O,P>& divisor, Variable var) {
	assert(!carl::isZero(divisor));
	return MultivariatePolynomial<C,O,P>(pseudo_remainder(to_univariate_polynomial(dividend, var), to_univariate_polynomial(divisor, var)));
}

}
