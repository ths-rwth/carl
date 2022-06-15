#pragma once

#include <carl-common/config.h>
#ifdef USE_LIBPOLY

#include "LPPolynomial.h"

#include "CoCoAAdaptorLP.h"

namespace carl {

/*
 * @brief wrapper for the gcd calculation using libpoly. Asserts that the polynomials have the same context
 * @param LPPolynomial p,q
 * @return gcd(p,q)
 */
LPPolynomial gcd(const LPPolynomial& p, const LPPolynomial& q) {
    return LPPolynomial(poly::gcd(p.getPolynomial(), q.getPolynomial()));
}

/*
 * @brief wrapper for the lcm calculation using libpoly. Asserts that the polynomials have the same context
 * @param LPPolynomial p,q
 * @return lcm(p,q)
 */
LPPolynomial lcm(const LPPolynomial& p, const LPPolynomial& q) {
    return LPPolynomial(poly::lcm(p.getPolynomial(), q.getPolynomial()));
}

/*
 * @brief wrapper for the polynomial content calculation using libpoly.
 * @param LPPolynomial p
 * @return content(p)
 */
LPPolynomial content(const LPPolynomial& p) {
    return LPPolynomial(poly::content(p.getPolynomial()));
}

/*
 * @brief wrapper for the polynomial primitive_part calculation using libpoly.
 * @param LPPolynomial p
 * @return primitive_part(p)
 */
LPPolynomial primitive_part(const LPPolynomial& p) {
    return LPPolynomial(poly::primitive_part(p.getPolynomial()));
}

/*
 * @brief wrapper for the resultant calculation using libpoly. Asserts that the polynomials have the same context
 * @param LPPolynomial p,q
 * @return resultant(p,q)
 */
LPPolynomial resultant(const LPPolynomial& p, const LPPolynomial& q) {
    return LPPolynomial(poly::resultant(p.getPolynomial(), q.getPolynomial()));
}

/*
 * @brief wrapper for the discriminant calculation using libpoly.
 * @param LPPolynomial p
 * @return discriminant(p)
 */
LPPolynomial discriminant(const LPPolynomial& p) {
    return LPPolynomial(poly::discriminant(p.getPolynomial()));
}

/*
 * @brief wrapper for the factorization of LPPolynomials
 * @param LPPolynomial p
 * @return content_free_factors(p)
 */
Factors<LPPolynomial> factorization(const LPPolynomial& p, bool includeConstant = true) {
#ifndef USE_COCOA
    CARL_LOG_FATAL("carl.poly", "factorization is not supported without libcocoa");
#endif

    CoCoAAdaptorLP adaptor = CoCoAAdaptorLP(p.getContext());

    return adaptor.factorize(p, includeConstant);
}

/*
 * @brief wrapper for the factorization of LPPolynomials, this is to be preferred when factorizing multiple polynomials with the same context
 * @param LPPolynomial p, CoCoaAdaptorLP adaptor
 * @return content_free_factors(p)
 */
Factors<LPPolynomial> factorization(const LPPolynomial& p, const CoCoAAdaptorLP& adaptor, bool includeConstant = true) {
    return adaptor.factorize(p, includeConstant);
}

/*
 * @brief wrapper for the square_free_factorization of LPPolynomials
 * @param LPPolynomial p
 * @return square_free_factorization(p)
 */
std::vector<LPPolynomial> square_free_factors(const LPPolynomial& p) {
    std::vector<poly::Polynomial> factors = poly::square_free_factors(p.getPolynomial());
    std::vector<LPPolynomial> result;
    for (const auto& factor : factors) {
        result.push_back(LPPolynomial(std::move(factor)));
    }
    return result;
}

/*
 * @brief wrapper for the content_free_factors of LPPolynomials
 * @param LPPolynomial p
 * @return content_free_factors(p)
 */
std::vector<LPPolynomial> content_free_factors(const LPPolynomial& p) {
    std::vector<poly::Polynomial> factors = poly::content_free_factors(p.getPolynomial());
    std::vector<LPPolynomial> result;
    for (const auto& factor : factors) {
        result.push_back(LPPolynomial(std::move(factor)));
    }
    return result;
}

} // namespace carl

#endif