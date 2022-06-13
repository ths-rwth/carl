#pragma once

#include "../MultivariatePolynomial.h"

namespace carl {

/**
 * Calculates the S-Polynomial of two polynomials.
 */
template<typename C, typename O, typename P>
MultivariatePolynomial<C,O,P> SPolynomial(const MultivariatePolynomial<C,O,P>& p, const MultivariatePolynomial<C,O,P>& q) {
	assert(p.is_consistent());
	assert(q.is_consistent());
	assert(p.nr_terms() != 0);
	assert(q.nr_terms() != 0);

	if ((p.nr_terms() == 1) && (q.nr_terms() == 1)) {
		return MultivariatePolynomial<C,O,P>();
	} else if (p.nr_terms() == 1) {
		return -(p.lterm().calcLcmAndDivideBy(q.lmon()) * q.tail());
	} else if (q.nr_terms() == 1) {
		return (q.lterm().calcLcmAndDivideBy(p.lmon()) * p.tail());
	} else {
		return (p.tail() * q.lterm().calcLcmAndDivideBy(p.lmon())) - (q.tail() * p.lterm().calcLcmAndDivideBy(q.lmon()));
	}
}

}
