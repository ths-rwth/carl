/**
 * @file adaption_native/typetraits.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include "../../util/SFINAE.h"
#include "../typetraits.h"

namespace carl {


/**
 * States that `long` is an integer type.
 * @ingroup typetraits
 */
template<>
struct is_integer<long> {
	/// Value of this trait.
	static constexpr bool value = true;
};
/**
 * States that `int` is an integer type.
 * @ingroup typetraits
 */
template<>
struct is_integer<int> {
	/// Value of this trait.
	static constexpr bool value = true;
};

/**
 * States that `double` is a float type.
 * @ingroup typetraits
 */
template<>
struct is_float<double> {
	/// Value of this trait.
    static constexpr bool value = true;
};

/**
 * States that `float` is a float type.
 * @ingroup typetraits
 */
template<>
struct is_float<float> {
	/// Value of this trait.
    static constexpr bool value = true;
};

TRAIT_TRUE(is_primitive, bool);

TRAIT_TRUE(is_primitive, char);
TRAIT_TRUE(is_primitive, signed char);
TRAIT_TRUE(is_primitive, unsigned char);
TRAIT_TRUE(is_primitive, wchar_t);
TRAIT_TRUE(is_primitive, char16_t);
TRAIT_TRUE(is_primitive, char32_t);

TRAIT_TRUE(is_primitive, short);
TRAIT_TRUE(is_primitive, unsigned short);
TRAIT_TRUE(is_primitive, int);
TRAIT_TRUE(is_primitive, unsigned);
TRAIT_TRUE(is_primitive, long);
TRAIT_TRUE(is_primitive, unsigned long);
TRAIT_TRUE(is_primitive, long long);
TRAIT_TRUE(is_primitive, unsigned long long);

TRAIT_TRUE(is_primitive, float);
TRAIT_TRUE(is_primitive, double);
TRAIT_TRUE(is_primitive, long double);

}