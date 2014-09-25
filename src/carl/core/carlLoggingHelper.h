/**
 * @file carlLoggingHelper.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include <bitset>
#include <forward_list>
#include <iostream>
#include <list>
#include <map>
#include <set>
#include <unordered_map>
#include <utility>
#include <vector>

namespace carl {

/**
 * Returns the binary representation given value as bit string.
 * Note that this method is tailored to little endian systems.
 * @param a A value of any type
 * @param spacing Specifies if the bytes shall be separated by a space.
 * @return Bit string representing a.
 */
template<typename T>
std::string binary(const T& a, const bool& spacing = true)
{
	std::stringstream ss;
    const char* begin = reinterpret_cast<const char*>(&a);
	const char* end = begin + sizeof(T);
	while (begin != end) {
		end--;
		ss << std::bitset<8>((unsigned)*end);
		if (spacing && (begin != end)) ss << " ";
	}
	return ss.str();
}

/**
 * Returns the basename of a given filename.
 * @param s Filename.
 * @return Basename of s.
 */
inline std::string basename(const std::string& s) {
	return s.substr(s.rfind('/') + 1);
}

}