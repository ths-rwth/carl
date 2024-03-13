#pragma once

#include <carl-arith/numbers/numbers.h>

/**
 * Helpers due to the shortcomings of libpoly's C++ API.
 * 
 */
namespace carl::poly_helper {

inline lp_polynomial_t* construct_lp_poly(const lp_polynomial_context_t* c, lp_variable_t v) {
    lp_polynomial_t* p = lp_polynomial_alloc();
    lp_polynomial_construct_simple(p, c, mpz_class(1).get_mpz_t(), v, 1);
    return p;
}

inline lp_polynomial_t* construct_lp_poly(const lp_polynomial_context_t* c, const mpz_class& i) {
    lp_polynomial_t* p = lp_polynomial_alloc();
    lp_polynomial_construct_simple(p, c, i.get_mpz_t(), lp_variable_null, 0) ;
    return p;
}
    
}
