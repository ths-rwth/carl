#pragma once

#include <limits>
#include <type_traits>

namespace carl {

template<typename Enum>
constexpr Enum invalid_enum_value() {
	return static_cast<Enum>(std::numeric_limits<typename std::underlying_type<Enum>::type>::max());
}

template<typename Enum>
constexpr auto underlying_enum_value(Enum e) {
	return static_cast<typename std::underlying_type<Enum>::type>(e);
}

}