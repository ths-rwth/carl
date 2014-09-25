/* 
 * File:   typetraits.h
 * Author: stefan
 *
 * Created on August 28, 2014, 11:25 AM
 */

#pragma once
#include "../config.h"
#include "../FLOAT_T.h"

namespace carl
{
    template<typename F>
    struct IntegralType<carl::FLOAT_T<F> >
    {
        typedef cln::cl_I type;
    };

    template<typename C>
    struct is_rational<FLOAT_T<C>>: std::integral_constant<bool, is_rational<C>::value>
    {};
    
    template<typename C>
    struct is_float<carl::FLOAT_T<C>> : std::true_type
    {};
    
    #ifdef USE_MPFR_FLOAT
    template<>
    struct is_float<mpfr_t> : std::true_type
    {};
    #endif
}