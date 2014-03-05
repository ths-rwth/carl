/** 
 * @file   adaption_gmpxx/typetraits.h
 * @ingroup typetraits
 * @ingroup gmpxx
 * @author Sebastian Junges
 * @author Gereon Kremer
 *
 */

#pragma once

#include "../../util/platform.h"
#include <type_traits>
CLANG_WARNING_DISABLE("-Wsign-conversion")
#include <gmpxx.h>
CLANG_WARNING_RESET


namespace carl 
{
/**
 * @ingroup typetraits
 * @ingroup gmpxx
 */
template<>
struct is_integer<mpq_class>
{
	static const bool value = true;
};

/**
 * @ingroup typetraits
 * @ingroup gmpxx
 */
template<>
struct is_rational<mpz_class>
{
	static const bool value = true;
};

/**
 * @ingroup typetraits
 * @ingroup gmpxx
 */
template<>
struct IntegralT<mpq_class>
{
	typedef mpz_class type;
};

/**
 * @ingroup typetraits
 * @ingroup gmpxx
 */
template<>
struct IntegralT<mpz_class>
{
	typedef mpz_class type;
};

}