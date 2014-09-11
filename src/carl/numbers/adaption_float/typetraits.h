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
    struct is_rational<FLOAT_T<C>>
    {
            static const bool value = is_rational<C>().value();
    };
}

namespace std
{
    template<typename C>
    struct is_floating_point<carl::FLOAT_T<C>>
    {
        static const bool value = true;
    };
    
    #ifdef USE_MPFR_FLOAT
    template<>
    struct is_floating_point<mpfr_t>
    {
        static const bool value = true;
    };
    #endif
}