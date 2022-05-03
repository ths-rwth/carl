#pragma once

#include <limits>
#include <type_traits>

namespace carl {

/**
 * Returns an enum value that is (most probably) not a valid enum value.
 * This can be used to check whether methods that take enums properly handle invalid values.
 */
template<typename Enum>
constexpr Enum invalid_enum_value() {
	return static_cast<Enum>(std::numeric_limits<typename std::underlying_type<Enum>::type>::max());
}

/**
 * Casts an enum value to a value of the underlying number type.
 */
template<typename Enum>
constexpr auto underlying_enum_value(Enum e) {
	return static_cast<typename std::underlying_type<Enum>::type>(e);
}

}