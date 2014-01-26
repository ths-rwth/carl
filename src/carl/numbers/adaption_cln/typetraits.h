/** 
 * @file   typetraits.h
 * @ingroup cln
 * @ingroup typetraits
 * @author Sebastian Junges
 * @author Gereon Kremer
 */

#pragma once

#include <type_traits>
#include <cln/cln.h>


namespace carl {

/**
 * @ingroup typetraits
 * @ingroup cln
 */
template<>
struct is_rational<cln::cl_RA>
{
	static const bool value = true;
};


/**
 * @ingroup typetraits
 * @ingroup cln
 */
template<>
struct is_integer<cln::cl_I> {
	static const bool value = true;
};


/**
 * @ingroup typetraits
 * @ingroup cln
 */
template<>
struct IntegralT<cln::cl_RA>
{
	typedef cln::cl_I type;
};

/**
 * @ingroup typetraits
 * @ingroup cln
 */
template<>
struct IntegralT<cln::cl_I>
{
	typedef cln::cl_I type;
};
}