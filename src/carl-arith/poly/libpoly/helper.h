#pragma once

/**
 * Helpers due to the shortcomings of libpoly's C++ API.
 * 
 */
namespace carl::poly_helper {

inline poly::Polynomial construct_poly(const lp_polynomial_context_t* c, poly::Variable v) {
    poly::Polynomial poly;
    lp_polynomial_construct_simple(poly.get_internal(), c, poly::Integer(1).get_internal(), v.get_internal(), 1);
    return poly;
}

inline poly::Polynomial construct_poly(const lp_polynomial_context_t* c, poly::Integer i) {
    poly::Polynomial poly;
    lp_polynomial_construct_simple(poly.get_internal(), c, i.get_internal(), lp_variable_null, 0) ;
    return poly;
}
    
}
