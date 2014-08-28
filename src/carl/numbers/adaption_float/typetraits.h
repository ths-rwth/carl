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
    #ifdef USE_MPFR_FLOAT
    template<>
    struct is_float<mpfr_t>
    {
        static const bool value = true;
    };
    #endif

    template<typename F>
    struct IntegralT<carl::FLOAT_T<F> >
    {
        typedef cln::cl_I type;
    };

    template<typename C>
    struct is_rational<FLOAT_T<C>>
    {
            static const bool value = true;
    };
}

