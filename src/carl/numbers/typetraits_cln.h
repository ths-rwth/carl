/** 
 * @file   typetraits_cln.h
 * @ingroup cln
 * @ingroup typetraits
 * @author Sebastian Junges
 * @author Gereon Kremer
 */

#pragma once

#include <type_traits>
#include <cln/cln.h>


namespace carl {


template<>
struct is_field<cln::cl_RA>
{
	static const bool value = true;
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
struct is_integer<cln::cl_I> {
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
struct IntegralT<cln::cl_RA>
{
	typedef cln::cl_I type;
};
template<>
struct IntegralT<cln::cl_I>
{
	typedef cln::cl_I type;
};
}