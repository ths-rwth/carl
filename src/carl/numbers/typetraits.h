/* 
 * File:   typetraits.h
 * Author: Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

/*!
 * This file contains all type traits for our types.
 * We use the notation conventions of std, being lower cases with underscores.
 */

#pragma once

#include <cln/cln.h>
#include <gmpxx.h>
#include <type_traits>

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
 * Type trait  is_field. 
 * Default is false, but certain types which encode algebraic fields should be set to true. 
 * @see UnivariatePolynomial - CauchyBound for example.
 */
template<typename type>
struct is_field
{
	static const bool value = false;
};

template<>
struct is_field<cln::cl_RA>
{
	static const bool value = true;
};

template<>
struct is_field<mpq_class>
{
	static const bool value = true;
};

template<typename C>
struct is_field<GFNumber<C>>
{
	static const bool value = true;
};


/**
 * Type trait is_fraction. 
 * Default is false, but certain types which encode fractions should be set to true. 
 * Note that we consider integral types to be fractional.
 */
template<typename type>
struct is_fraction
{
	static const bool value = false;
};

template<>
struct is_fraction<cln::cl_I>
{
	static const bool value = true;
};
template<>
struct is_fraction<cln::cl_RA>
{
	static const bool value = true;
};

template<>
struct is_fraction<mpz_class>
{
	static const bool value = true;
};
template<>
struct is_fraction<mpq_class>
{
	static const bool value = true;
};


/**
 * Type trait is_integer.
 * Default is false, but certain types which encode integral types should be set to true. 
 */
template<typename type>
struct is_integer {
	static const bool value = false;
};

template<>
struct is_integer<cln::cl_I> {
	static const bool value = true;
};
template<>
struct is_integer<mpz_class> {
	static const bool value = true;
};
template<>
struct is_integer<long> {
	static const bool value = true;
};
template<>
struct is_integer<unsigned long> {
	static const bool value = true;
};
template<>
struct is_integer<unsigned> {
	static const bool value = true;
};
template<>
struct is_integer<int> {
	static const bool value = true;
};


/**
 * Type trait for the characteristic of the given field (template argument).
 * Default is 0, but certain types which encode algebraic fields should be set to true. 
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
 */

template<typename C>
struct is_finite_domain
{
	static constexpr bool value = false;
};

template<typename C>
struct is_finite_domain<GFNumber<C>>
{
	static constexpr bool value = true;
};


/**
 * Type trait is_number.
 * Default is false. Should be set to true for all number types to distinguish them from Polynomials for example.
 */
template<typename type>
struct is_number
{
	static const bool value = false;
};

template<>
struct is_number<int>
{
	static const bool value = true;
};

template<>
struct is_number<cln::cl_RA>
{
	static const bool value = true;
};

template<>
struct is_number<cln::cl_I>
{
	static const bool value = true;
};

template<>
struct is_number<mpq_class>
{
	static const bool value = true;
};

template<>
struct is_number<mpz_class>
{
	static const bool value = true;
};


/**
 * Type trait IntegralT.
 * Gives the corresponding integral type.
 * Default is int.
 */
template<typename RationalType>
struct IntegralT
{
	typedef int type;
};


template<>
struct IntegralT<double>
{
	typedef unsigned type;
};
template<>
struct IntegralT<cln::cl_RA>
{
	typedef cln::cl_I type;
};
template<>
struct IntegralT<cln::cl_I>
{
	typedef cln::cl_I type;
};
template<>
struct IntegralT<mpq_class>
{
	typedef mpz_class type;
};
template<>
struct IntegralT<mpz_class>
{
	typedef mpz_class type;
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