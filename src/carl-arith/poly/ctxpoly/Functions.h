#pragma once

#include "ContextPolynomial.h"

namespace carl {

template<typename Coeff, typename Ordering, typename Policies>
inline auto irreducible_factors(const ContextPolynomial<Coeff, Ordering, Policies>& p, bool constants = true) {
    std::vector<ContextPolynomial<Coeff, Ordering, Policies>> res;
    for (const auto& f : irreducible_factors(MultivariatePolynomial<Coeff, Ordering, Policies>(p.content()), constants)) {
        res.emplace_back(p.context(), f);
    }
    return res;
}

template<typename Coeff, typename Ordering, typename Policies>
inline auto discriminant(const ContextPolynomial<Coeff, Ordering, Policies>& p) {
    return ContextPolynomial<Coeff, Ordering, Policies>(p.context(), MultivariatePolynomial<Coeff, Ordering, Policies>(discriminant(p.content()))); 
}

template<typename Coeff, typename Ordering, typename Policies>
inline auto resultant(const ContextPolynomial<Coeff, Ordering, Policies>& p, const ContextPolynomial<Coeff, Ordering, Policies>& q) {
    assert(p.context() == q.context());
    return ContextPolynomial<Coeff, Ordering, Policies>(p.context(), MultivariatePolynomial<Coeff, Ordering, Policies>(resultant(p.content(), q.content()))); 
}

}