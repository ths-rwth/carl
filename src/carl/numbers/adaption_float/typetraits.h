/*
 * File:   typetraits.h
 * Author: stefan
 *
 * Created on August 28, 2014, 11:25 AM
 */

#pragma once

#ifndef INCLUDED_FROM_NUMBERS_H
static_assert(false, "This file may only be included indirectly by numbers.h");
#endif

namespace carl
{
	template<typename F>
	class FLOAT_T;

    template<typename F>
    struct IntegralType<carl::FLOAT_T<F> >
    {
        #ifdef USE_CLN_NUMBERS
        using type = cln::cl_I;
        #else
        using type = mpz_class;
        #endif
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
