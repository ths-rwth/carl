#pragma once

#include "../CoCoAAdaptor.h"

namespace carl {

template<typename C, typename O, typename P>
class MultivariatePolynomial;

template<typename C, typename O, typename P>
std::vector<MultivariatePolynomial<C,O,P>> groebner_basis(const std::vector<MultivariatePolynomial<C,O,P>>& polys) {
	if (polys.size() <= 1) return polys;
    CoCoAAdaptor adaptor(polys);
    return adaptor.GBasis(polys);
}

}
