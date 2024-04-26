#pragma once

#include <carl-common/config.h>
#ifdef USE_LIBPOLY

#include "LPPolynomial.h"

namespace carl {

/*
 * @brief wrapper for the gcd calculation using libpoly. Asserts that the polynomials have the same context
 * @param LPPolynomial p,q
 * @return gcd(p,q)
 */
inline LPPolynomial gcd(const LPPolynomial& p, const LPPolynomial& q) {
    lp_polynomial_t* res = lp_polynomial_new(p.context().lp_context());
    lp_polynomial_gcd(res, p.get_internal(), q.get_internal());
    return LPPolynomial(res, p.context());
}

/*
 * @brief wrapper for the lcm calculation using libpoly. Asserts that the polynomials have the same context
 * @param LPPolynomial p,q
 * @return lcm(p,q)
 */
inline LPPolynomial lcm(const LPPolynomial& p, const LPPolynomial& q) {
    lp_polynomial_t* res = lp_polynomial_new(p.context().lp_context());
    lp_polynomial_lcm(res, p.get_internal(), q.get_internal());
    return LPPolynomial(res, p.context());
}

/*
 * @brief wrapper for the polynomial content calculation using libpoly.
 * @param LPPolynomial p
 * @return content(p)
 */
inline LPPolynomial content(const LPPolynomial& p) {
    lp_polynomial_t* res = lp_polynomial_new(p.context().lp_context());
    lp_polynomial_cont(res, p.get_internal());
    return LPPolynomial(res, p.context());
}

inline LPPolynomial derivative(const LPPolynomial& p) {
    lp_polynomial_t* res = lp_polynomial_new(p.context().lp_context());
    lp_polynomial_derivative(res, p.get_internal());
    return LPPolynomial(res, p.context());
}

/*
 * @brief wrapper for the polynomial primitive_part calculation using libpoly.
 * @param LPPolynomial p
 * @return primitive_part(p)
 */
inline LPPolynomial primitive_part(const LPPolynomial& p) {
    lp_polynomial_t* res = lp_polynomial_new(p.context().lp_context());
    lp_polynomial_pp(res, p.get_internal());
    return LPPolynomial(res, p.context());
}

/*
 * @brief wrapper for the resultant calculation using libpoly. Asserts that the polynomials have the same context
 * @param LPPolynomial p,q
 * @return resultant(p,q)
 */
inline LPPolynomial resultant(const LPPolynomial& p, const LPPolynomial& q) {
    lp_polynomial_t* res = lp_polynomial_new(p.context().lp_context());
    lp_polynomial_resultant(res, p.get_internal(), q.get_internal());
    return LPPolynomial(res, p.context());
}

/*
 * @brief wrapper for the discriminant calculation using libpoly.
 * @param LPPolynomial p
 * @return discriminant(p)
 */
inline LPPolynomial discriminant(const LPPolynomial& p) {
    assert(p.context().lp_context() == lp_polynomial_get_context(p.get_internal()));
    if (lp_polynomial_degree(p.get_internal()) == 1) { // workaround for bug in the libpoly c++ wrapper
        return LPPolynomial(p.context(), 1);
    }
    // div(resultant(p, derivative(p)), leading_coefficient(p));
    lp_polynomial_t* ldcf = lp_polynomial_new(p.context().lp_context());
    lp_polynomial_get_coefficient(ldcf, p.get_internal(), lp_polynomial_degree(p.get_internal()));
    lp_polynomial_t* res = lp_polynomial_new(p.context().lp_context());
    lp_polynomial_derivative(res, p.get_internal());
    lp_polynomial_resultant(res, p.get_internal(), res);
    lp_polynomial_div(res, res, ldcf);
    lp_polynomial_delete(ldcf);
    return LPPolynomial(res, p.context());
}

/*
 * @brief wrapper for the factorization of LPPolynomials
 * @param LPPolynomial p
 * @return factorization(p)
 */
Factors<LPPolynomial> factorization(const LPPolynomial& p, bool includeConstant = true);

std::vector<LPPolynomial> irreducible_factors(const LPPolynomial& p, bool includeConstants = true);

/*
 * @brief wrapper for the square_free_factorization of LPPolynomials
 * @param LPPolynomial p
 * @return square_free_factorization(p)
 */
std::vector<LPPolynomial> square_free_factors(const LPPolynomial& p);

/*
 * @brief wrapper for the content_free_factors of LPPolynomials
 * @param LPPolynomial p
 * @return content_free_factors(p)
 */
std::vector<LPPolynomial> content_free_factors(const LPPolynomial& p);

std::vector<LPPolynomial> groebner_basis(const std::vector<LPPolynomial>& polys);

} // namespace carl

#endif