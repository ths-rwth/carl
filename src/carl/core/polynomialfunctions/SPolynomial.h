#pragma once

#include "../MultivariatePolynomial.h"

namespace carl {

template<typename C, typename O, typename P>
MultivariatePolynomial<C,O,P> SPolynomial(const MultivariatePolynomial<C,O,P>& p, const MultivariatePolynomial<C,O,P>& q) {
	assert(p.isConsistent());
	assert(q.isConsistent());
	assert(p.nrTerms() != 0);
	assert(q.nrTerms() != 0);

	if (p.nrTerms() == 1 && q.nrTerms() == 1) {
		return MultivariatePolynomial();
	} else if (p.nrTerms() == 1) {
		return -(p.lterm().calcLcmAndDivideBy(q.lmon()) * q.tail());
	} else if (q.nrTerms() == 1) {
		return (q.lterm().calcLcmAndDivideBy(p.lmon()) * p.tail());
	} else {
		return (p.tail() * q.lterm().calcLcmAndDivideBy(p.lmon())) - (q.tail() * p.lterm().calcLcmAndDivideBy(q.lmon()));
	}
}

}
