#pragma once

#include<carl-common/config.h>

namespace carl {

template<typename A, typename B, typename C>
ContextPolynomial<A, B, C> convert(const Context& context, const MultivariatePolynomial<A, B, C>& p) {
    return ContextPolynomial<A, B, C>(context, p);
}
template<typename A, typename B, typename C>
MultivariatePolynomial<A, B, C> convert(const ContextPolynomial<A, B, C>& p) {
    return p.as_multivariate();
}

#ifdef USE_LIBPOLY
template<typename A, typename B, typename C>
LPPolynomial convert(const LPContext& context, const MultivariatePolynomial<A, B, C>& p) {
    return LPPolynomial(context); // TODO add conversion here
}
template<typename A, typename B, typename C>
MultivariatePolynomial<A, B, C> convert(const LPPolynomial& p) {
    return MultivariatePolynomial<A, B, C>(); // TODO add conversion here
}
#endif

}