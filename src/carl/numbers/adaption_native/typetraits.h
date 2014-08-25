/**
 * @file typetraits.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include "../../util/SFINAE.h"

namespace carl {


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
 * @todo Fix this?
 */
template<>
struct IntegralT<double>
{
	typedef unsigned type;
};



}