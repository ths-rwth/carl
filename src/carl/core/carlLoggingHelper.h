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

namespace std {

/**
 * Output a std::forward_list with arbitrary content.
 * The format is `[<item>, <item>, ...]`
 * @param os Output stream.
 * @param l list to be printed.
 * @return Output stream.
 */
template<typename T>
std::ostream& operator<<(std::ostream& os, const std::forward_list<T>& l) {
	os << "[";
	bool first = true;
	for (auto it: l) {
		if (!first) os << ", ";
		first = false;
		os << it;
	}
	return os << "]";
}

/**
 * Output a std::list with arbitrary content.
 * The format is `[<length>: <item>, <item>, ...]`
 * @param os Output stream.
 * @param l list to be printed.
 * @return Output stream.
 */
template<typename T>
std::ostream& operator<<(std::ostream& os, const std::list<T>& l) {
	os << "[" << l.size() << ": ";
	bool first = true;
	for (auto it: l) {
		if (!first) os << ", ";
		first = false;
		os << it;
	}
	return os << "]";
}

/**
 * Output a std::map with arbitrary content.
 * The format is `{<key>:<value>, <key>:<value>, ...}`
 * @param os Output stream.
 * @param m map to be printed.
 * @return Output stream.
 */
template<typename Key, typename Value>
std::ostream& operator<<(std::ostream& os, const std::map<Key, Value>& m) {
	os << "{";
	bool first = true;
	for (auto it: m) {
		if (!first) os << ", ";
		first = false;
		os << it.first << " : " << it.second;
	}
	return os << "}";
}

/**
 * Output a std::set with arbitrary content.
 * The format is `{<length>: <item>, <item>, ...}`
 * @param os Output stream.
 * @param s set to be printed.
 * @return Output stream.
 */
template<typename T, typename C>
std::ostream& operator<<(std::ostream& os, const std::set<T, C>& s) {
	os << "{" << s.size() << ": ";
	bool first = true;
	for (auto it: s) {
		if (!first) os << ", ";
		first = false;
		os << it;
	}
	return os << "}";
}

/**
 * Output a std::unordered_map with arbitrary content.
 * The format is `{<key>:<value>, <key>:<value>, ...}`
 * @param os Output stream.
 * @param m map to be printed.
 * @return Output stream.
 */
template<typename Key, typename Value>
std::ostream& operator<<(std::ostream& os, const std::unordered_map<Key, Value>& m) {
	os << "{";
	bool first = true;
	for (auto it: m) {
		if (!first) os << ", ";
		first = false;
		os << it.first << " : " << it.second;
	}
	return os << "}";
}


template<typename U, typename V>
std::ostream& operator<<(std::ostream& os, const std::pair<U, V>& p) {
	return os << "(" << p.first << ", " << p.second << ")";
}


/**
 * Output a std::vector with arbitrary content.
 * The format is `[<length>: <item>, <item>, ...]`
 * @param os Output stream.
 * @param v Vector to be printed.
 * @return Output stream.
 */
template<typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v) {
	os << "[" << v.size() << ": ";
	bool first = true;
	for (auto it: v) {
		if (!first) os << ", ";
		first = false;
		os << it;
	}
	return os << "]";
}

}

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