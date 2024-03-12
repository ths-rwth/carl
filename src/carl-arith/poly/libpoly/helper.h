#pragma once

/**
 * Helpers due to the shortcomings of libpoly's C++ API.
 * 
 */
namespace carl::poly_helper {

inline poly::Polynomial construct_poly(const lp_polynomial_context_t* c, lp_variable_t v) {
    lp_polynomial_t* p = lp_polynomial_alloc();
    lp_polynomial_construct_simple(p, c, poly::Integer(1).get_internal(), v, 1);
    return poly::Polynomial(p);
}

inline poly::Polynomial construct_poly(const lp_polynomial_context_t* c, poly::Integer i) {
    lp_polynomial_t* p = lp_polynomial_alloc();
    lp_polynomial_construct_simple(p, c, i.get_internal(), lp_variable_null, 0) ;
    return poly::Polynomial(p);
}
    
}
