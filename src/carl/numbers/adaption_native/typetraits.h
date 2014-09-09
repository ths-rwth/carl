/**
 * @file adaption_native/typetraits.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include "../../util/SFINAE.h"

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


/**
 * States that `double` is a primitive type.
 * @ingroup typetraits
 */
template<>
struct is_primitive<double> {
	/// Value of this trait.
	static constexpr bool value = true;
};

/**
 * States that `long double` is a primitive type.
 * @ingroup typetraits
 */
template<>
struct is_primitive<long double> {
	/// Value of this trait.
	static constexpr bool value = true;
};

/**
 * States that `int` is a primitive type.
 * @ingroup typetraits
 */
template<>
struct is_primitive<int> {
	/// Value of this trait.
	static constexpr bool value = true;
};

/**
 * States that `float` is a primitive type.
 * @ingroup typetraits
 */
template<>
struct is_primitive<float> {
	/// Value of this trait.
	static constexpr bool value = true;
};

/**
 * States that `unsigned` is a primitive type.
 * @ingroup typetraits
 */
template<>
struct is_primitive<unsigned> {
	/// Value of this trait.
	static constexpr bool value = true;
};

}