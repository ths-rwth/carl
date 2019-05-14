#pragma once

#include <limits>
#include <type_traits>

template<typename T>
constexpr T invalid_value() {
	return T(std::numeric_limits<typename std::underlying_type<T>::type>::max());
}