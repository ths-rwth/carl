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
inline LPPolynomial gcd(const LPPolynomial& p, const LPPolynomial& q) {
    return LPPolynomial(p.context(), poly::gcd(p.get_polynomial(), q.get_polynomial()));
}

/*
 * @brief wrapper for the lcm calculation using libpoly. Asserts that the polynomials have the same context
 * @param LPPolynomial p,q
 * @return lcm(p,q)
 */
inline LPPolynomial lcm(const LPPolynomial& p, const LPPolynomial& q) {
    return LPPolynomial(p.context(), poly::lcm(p.get_polynomial(), q.get_polynomial()));
}

/*
 * @brief wrapper for the polynomial content calculation using libpoly.
 * @param LPPolynomial p
 * @return content(p)
 */
inline LPPolynomial content(const LPPolynomial& p) {
    return LPPolynomial(p.context(), poly::content(p.get_polynomial()));
}

inline LPPolynomial derivative(const LPPolynomial& p) {
    return LPPolynomial(p.context(), poly::derivative(p.get_polynomial()));
}

/*
 * @brief wrapper for the polynomial primitive_part calculation using libpoly.
 * @param LPPolynomial p
 * @return primitive_part(p)
 */
inline LPPolynomial primitive_part(const LPPolynomial& p) {
    return LPPolynomial(p.context(), poly::primitive_part(p.get_polynomial()));
}

/*
 * @brief wrapper for the resultant calculation using libpoly. Asserts that the polynomials have the same context
 * @param LPPolynomial p,q
 * @return resultant(p,q)
 */
inline LPPolynomial resultant(const LPPolynomial& p, const LPPolynomial& q) {
    return LPPolynomial(p.context(), poly::resultant(p.get_polynomial(), q.get_polynomial()));
}

/*
 * @brief wrapper for the discriminant calculation using libpoly.
 * @param LPPolynomial p
 * @return discriminant(p)
 */
inline LPPolynomial discriminant(const LPPolynomial& p) {
    assert(p.context().lp_context() == lp_polynomial_get_context(p.get_polynomial().get_internal()));
    if (poly::degree(p.get_internal()) == 1) { // workaround for bug in the libpoly c++ wrapper
        return LPPolynomial(p.context(), 1);
    }
    return LPPolynomial(p.context(), poly::discriminant(p.get_polynomial()));
}

/*
 * @brief wrapper for the factorization of LPPolynomials, this is to be preferred when factorizing multiple polynomials with the same context
 * @param LPPolynomial p, CoCoaAdaptorLP adaptor
 * @return factorization(p)
 */
inline Factors<LPPolynomial> factorization(const LPPolynomial& p, const CoCoAAdaptorLP& adaptor, bool includeConstant = true) {
    #ifndef USE_COCOA
    CARL_LOG_FATAL("carl.poly", "factorization is not supported without libcocoa");
    #endif
    return adaptor.factorize(p, includeConstant);
}

/*
 * @brief wrapper for the factorization of LPPolynomials
 * @param LPPolynomial p
 * @return factorization(p)
 */
inline Factors<LPPolynomial> factorization(const LPPolynomial& p, bool includeConstant = true) {
    CoCoAAdaptorLP adaptor = CoCoAAdaptorLP(p.context());
    return factorization(p, adaptor, includeConstant);
}

inline std::vector<LPPolynomial> irreducible_factors(const LPPolynomial& p, const CoCoAAdaptorLP& adaptor, bool includeConstants = true) {
	#ifndef USE_COCOA
    CARL_LOG_FATAL("carl.poly", "factorization is not supported without libcocoa");
    #endif
    
    if (is_constant(p)) {
		if (includeConstants) {
			return {p};
		} else {
			return {};
		}
	} else if (p.total_degree() == 1) {
		return {p};
	}

    return adaptor.irreducible_factors(p, includeConstants);
}

inline std::vector<LPPolynomial> irreducible_factors(const LPPolynomial& p, bool includeConstants = true) {
    CoCoAAdaptorLP adaptor = CoCoAAdaptorLP(p.context());
    return irreducible_factors(p, adaptor, includeConstants);
}

/*
 * @brief wrapper for the square_free_factorization of LPPolynomials
 * @param LPPolynomial p
 * @return square_free_factorization(p)
 */
inline std::vector<LPPolynomial> square_free_factors(const LPPolynomial& p) {
    std::vector<poly::Polynomial> factors = poly::square_free_factors(p.get_polynomial());
    std::vector<LPPolynomial> result;
    for (const auto& factor : factors) {
        result.push_back(LPPolynomial(p.context(), std::move(factor)));
    }
    return result;
}

/*
 * @brief wrapper for the content_free_factors of LPPolynomials
 * @param LPPolynomial p
 * @return content_free_factors(p)
 */
inline std::vector<LPPolynomial> content_free_factors(const LPPolynomial& p) {
    std::vector<poly::Polynomial> factors = poly::content_free_factors(p.get_polynomial());
    std::vector<LPPolynomial> result;
    for (const auto& factor : factors) {
        result.push_back(LPPolynomial(p.context(), std::move(factor)));
    }
    return result;
}

inline std::vector<LPPolynomial> groebner_basis(const std::vector<LPPolynomial>& polys) {
    if (polys.size() <= 1) return polys;
    CoCoAAdaptorLP adaptor = CoCoAAdaptorLP(polys.at(0).context());
    return adaptor.GBasis(polys);
}

} // namespace carl

#endif