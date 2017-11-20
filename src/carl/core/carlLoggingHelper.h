#pragma once

#include <algorithm>
#include <bitset>
#include <cstdint>
#include <iostream>
#include <sstream>

namespace carl {

/**
 * Return the binary representation given value as bit string.
 * Note that this method is tailored to little endian systems.
 * @param a A value of any type
 * @param spacing Specifies if the bytes shall be separated by a space.
 * @return Bit string representing a.
 */
template<typename T>
std::string binary(const T& a, const bool& spacing = true)
{
	std::stringstream ss;
    const std::uint8_t* begin = reinterpret_cast<const std::uint8_t*>(&a); // NOLINT
	const std::uint8_t* end = begin + sizeof(T);
	while (begin != end) {
		end--;
		ss << std::bitset<8>(std::uint8_t(*end));
		if (spacing && (begin != end)) ss << " ";
	}
	return ss.str();
}

/**
 * Return the basename of a given filename.
 */
inline std::string basename(const std::string& filename) {
	return filename.substr(std::max(filename.rfind('/') + 1, filename.rfind('\\') + 1));
}

}
