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

TRAIT_TRUE(is_fundamental, bool);

TRAIT_TRUE(is_fundamental, char);
TRAIT_TRUE(is_fundamental, wchar_t);
TRAIT_TRUE(is_fundamental, char16_t);
TRAIT_TRUE(is_fundamental, char32_t);

TRAIT_TRUE(is_fundamental, signed char);
TRAIT_TRUE(is_fundamental, short int);
TRAIT_TRUE(is_fundamental, int);
TRAIT_TRUE(is_fundamental, long int);
TRAIT_TRUE(is_fundamental, long long int);

TRAIT_TRUE(is_fundamental, unsigned char);
TRAIT_TRUE(is_fundamental, unsigned short int);
TRAIT_TRUE(is_fundamental, unsigned int);
TRAIT_TRUE(is_fundamental, unsigned long int);
TRAIT_TRUE(is_fundamental, unsigned long long int);

TRAIT_TRUE(is_fundamental, float);
TRAIT_TRUE(is_fundamental, double);
TRAIT_TRUE(is_fundamental, long double);

}