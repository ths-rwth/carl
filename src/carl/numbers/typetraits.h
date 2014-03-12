/** 
 * @file   numbers/typetraits.h
 * @ingroup typetraits
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 * @author Sebastian Junges
 *
 * This file contains all type traits for our types.
 * We use the notation conventions of the STL, being lower cases with underscores.
 */

#pragma once

#include "../util/platform.h"
#include "config.h"
#include <type_traits>
#ifdef USE_MPFR_FLOAT
#include <mpfr.h>
#endif
CLANG_WARNING_DISABLE("-Wsign-conversion")
#include <gmpxx.h>
CLANG_WARNING_RESET

#include "../util/SFINAE.h"

namespace carl {

// 
// Forward declarations
//
template<typename IntegerT>
class GFNumber;

template<typename C>
class UnivariatePolynomial;

template<typename C, typename O, typename P>
class MultivariatePolynomial;

/**
 * @ingroup typetraits
 */
template<typename type>
struct is_real
{
	static const bool value = false;
};

/**
 * Type trait is_rational. 
 * Default is false, but certain types which encode rationals should be set to true. 
 */
template<typename type>
struct is_rational
{
	static const bool value = false;
};


/**
 * Type trait  is_field. 
 * Default, we set rationals and reals to true and others to false, but additional types which encode algebraic fields should be set to true. 
 * @ingroup typetraits
 * @see UnivariatePolynomial - CauchyBound for example.
 */
template<typename T>
struct is_field
{
	static const bool value = is_rational<T>::value || is_real<T>::value;
};
/**
 * @ingroup typetraits
 */
template<typename C>
struct is_field<GFNumber<C>>
{
	static const bool value = true;
};

/**
 * Type trait is_integer.
 * Default is false, but certain types which encode integral types should be set to true. 
 * @ingroup typetraits
 */
template<typename T>
struct is_integer {
	static const bool value = false;
};
/**
 * @ingroup typetraits
 */
template<>
struct is_integer<long> {
	static const bool value = true;
};
/**
 * @ingroup typetraits
 */
template<>
struct is_integer<int> {
	static const bool value = true;
};

/**
 * @ingroup typetraits
 */
template<typename T>
struct is_natural
{
	static constexpr bool value = false;
};

/**
 * @ingroup typetraits
 */
template<>
struct is_natural<unsigned> {
	static constexpr bool value = false;
};

/**
 * @ingroup typetraits
 */
template<>
struct is_natural<unsigned long> {
	static constexpr bool value = false;
};


/**
 * @ingroup typetraits
 */
template<typename Type>
struct is_subset_of_integers
{
	static constexpr bool value = is_integer<Type>::value || is_natural<Type>::value;
};

/**
 * @ingroup typetraits
 */
template<typename Type>
struct is_subset_of_rationals
{
	static constexpr bool value = is_rational<Type>::value || is_subset_of_integers<Type>::value;
};

/**
 * @ingroup typetraits
 */
template<typename Type>
struct is_subset_of_reals
{
	static constexpr bool value = is_real<Type>::value || is_subset_of_rationals<Type>::value;
};



/**
 * Type trait for the characteristic of the given field (template argument).
 * @see UnivariatePolynomial - squareFreeFactorization for example.
 */
template<typename type>
struct characteristic
{
	static const unsigned value = 0;
};


/**
 * Type trait is_finite_domain.
 * Default is false.
 * @ingroup typetraits
 */
template<typename C>
struct is_finite_domain
{
	static constexpr bool value = false;
};

/**
 * Type trait is_finite_domain.
 * Default is false.
 * @ingroup typetraits
 */
template<typename C>
struct is_finite_domain<GFNumber<C>>
{
	static constexpr bool value = true;
};


/**
 * Type trait is_number.
 * Default is that subsets of reals are true. Should be set to true for all number types to distinguish them from Polynomials for example.
 * @ingroup typetraits
 */
template<typename T>
struct is_number
{
	static constexpr bool value = is_subset_of_reals<T>::value;
};

/**
 * Type trait is float
 * Default is false. Should be set to true for all floating point numbers to enable floating point arithmetic, e.g. in the interval class.
 */

template<typename type>
struct is_float
{
    static const bool value = false;
};

#ifdef USE_MPFR_FLOAT
template<>
struct is_float<mpfr_t>
{
    static const bool value = true;
};
#endif

template<>
struct is_float<double>
{
    static const bool value = true;
};

template<>
struct is_float<float>
{
    static const bool value = true;
};

/**
 * Type trait is_primitive required for BoostIntervals to use the preimlpemented default rounding and checking policies.
 */
	
template<typename type>
struct is_primitive
{
	static const bool value = false;
};
	
	
/**
 * @ingroup typetraits
 * @see GFNumber
 */
template<typename C>
struct is_number<GFNumber<C>>
{
	static constexpr bool value = true;
};

template<>
struct is_primitive<double>
{
	static const bool value = true;
};

	template<>
struct is_primitive<long double>
{
	static const bool value = true;
};
	
template<>
struct is_primitive<int>
{
	static const bool value = true;
};

template<>
struct is_primitive<float>
{
	static const bool value = true;
};

template<>
struct is_primitive<unsigned>
{
	static const bool value = true;
};
/**
 * Gives the corresponding integral type.
 * Default is int.
 */
template<typename RationalType>
struct IntegralT
{
	typedef int type;
};

/**
 * @todo Fix this?
 */
template<>
struct IntegralT<double>
{
	typedef unsigned type;
};

template<typename C>
struct IntegralT<GFNumber<C>>
{
	typedef C type;
};


/**
 * Coefficient ring of numbers is just the type of the number. (TODO limit this to numbers)
 */
template<typename C>
struct CoefficientRing
{
	typedef C type;
};

template<typename C>
struct CoefficientRing<UnivariatePolynomial<C>>
{
	typedef C type;
};

template<typename C, typename O, typename P>
struct CoefficientRing<MultivariatePolynomial<C, O, P>>
{
	typedef C type;
};


/**
 * Obtains the underlying number type of a polynomial type.
 */
template<typename C>
struct UnderlyingNumberType
{
	typedef C type;
};

template<typename C>
struct UnderlyingNumberType<UnivariatePolynomial<C>>
{
	typedef typename UnderlyingNumberType<C>::type type;
};

template<typename C, typename O, typename P>
struct UnderlyingNumberType<MultivariatePolynomial<C, O, P>>
{
	typedef typename UnderlyingNumberType<C>::type type;
};
}

#include "adaption_cln/typetraits.h"
#include "adaption_gmpxx/typetraits.h"
