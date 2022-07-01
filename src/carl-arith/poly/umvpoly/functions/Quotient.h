#pragma once

#include "to_univariate_polynomial.h"

#include "../MultivariatePolynomial.h"
#include "../UnivariatePolynomial.h"

namespace carl {

/**
 * Calculates the quotient of a polynomial division.
 */
template<typename C, typename O, typename P>
MultivariatePolynomial<C,O,P> quotient(const MultivariatePolynomial<C,O,P>& dividend, const MultivariatePolynomial<C,O,P>& divisor) {
	assert(!carl::is_zero(divisor));
	if (dividend == divisor) {
		return MultivariatePolynomial<C,O,P>(1);
	}
	if (carl::is_one(divisor)) {
		return dividend;
	}
	//static_assert(is_field_type<C>::value, "Division only defined for field coefficients");
	MultivariatePolynomial<C,O,P> p(dividend);
	auto& tam = MultivariatePolynomial<C,O,P>::mTermAdditionManager;
	auto id = tam.getId(p.nr_terms());
	while(!carl::is_zero(p))
	{
		Term<C> factor;
		if (p.lterm().divide(divisor.lterm(), factor)) {
			//p -= factor * divisor;
			p.subtractProduct(factor, divisor);
			tam.template addTerm<true>(id, factor);
		}
		else
		{
			p.strip_lterm();
		}
	}
	MultivariatePolynomial<C,O,P> result;
	tam.readTerms(id, result.terms());
	result.reset_ordered();
	result.template makeMinimallyOrdered<false, true>();
	assert(result.is_consistent());
	assert(dividend.is_consistent());
	return result;
}

}
