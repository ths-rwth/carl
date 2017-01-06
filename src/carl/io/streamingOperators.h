/**
 * @file streamingOperators.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 *
 * This file contains streaming operators for most of the STL containers.
 */

#pragma once

#include <forward_list>
#include <iostream>
#include <list>
#include <map>
#include <set>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace carl {

/*
template<typename T>
std::ostream& operator<<(std::ostream& os, const std::forward_list<T>& l);
template<typename T>
std::ostream& operator<<(std::ostream& os, const std::list<T>& l);
template<typename Key, typename Value, typename Comparator>
std::ostream& operator<<(std::ostream& os, const std::map<Key, Value, Comparator>& m);
template<typename U, typename V>
std::ostream& operator<<(std::ostream& os, const std::pair<U, V>& p);
template<typename T, typename C>
std::ostream& operator<<(std::ostream& os, const std::set<T, C>& s);
template<std::size_t I, typename... T, typename std::enable_if<I == sizeof...(T), void>::type*>
std::ostream& operator<<(std::ostream& os, const std::tuple<T...>& t);
template<std::size_t I, typename... T, typename std::enable_if<I < sizeof...(T), void>::type*>
std::ostream& operator<<(std::ostream& os, const std::tuple<T...>& t);
template<typename Key, typename Value, typename H, typename E, typename A>
std::ostream& operator<<(std::ostream& os, const std::unordered_map<Key, Value, H, E, A>& m);
template<typename T, typename H, typename K, typename A>
std::ostream& operator<<(std::ostream& os, const std::unordered_set<T, H, K, A>& s);
template<typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v);
*/

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
	for (const auto& it: l) {
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
	for (const auto& it: l) {
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
template<typename Key, typename Value, typename Comparator>
std::ostream& operator<<(std::ostream& os, const std::map<Key, Value, Comparator>& m) {
	os << "{";
	bool first = true;
	for (const auto& it: m) {
		if (!first) os << ", ";
		first = false;
		os << it.first << " : " << it.second;
	}
	return os << "}";
}

/**
 * Output a std::pair with arbitrary content.
 * The format is `(<first>, <second>)`
 * @param os Output stream.
 * @param p pair to be printed.
 * @return Output stream.
 */
template<typename U, typename V>
std::ostream& operator<<(std::ostream& os, const std::pair<U, V>& p) {
	return os << "(" << p.first << ", " << p.second << ")";
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
	for (const auto& it: s) {
		if (!first) os << ", ";
		first = false;
		os << it;
	}
	return os << "}";
}

/**
 * Output a std::tuple with arbitrary content.
 * Final case for `I == 0`.
 * @param os Output stream.
 * @return Output stream.
 */
template<std::size_t I = 0, typename... T, typename std::enable_if<I == sizeof...(T), void>::type* = nullptr>
std::ostream& operator<<(std::ostream& os, const std::tuple<T...>& /*unused*/) {
	return os << ")";
}

/**
 * Output a std::tuple with arbitrary content.
 * The format is `(<item>, <item>, ...)`
 * @param os Output stream.
 * @param t tuple to be printed.
 * @return Output stream.
 */
template<std::size_t I = 0, typename... T, typename std::enable_if<I < sizeof...(T), void>::type* = nullptr>
std::ostream& operator<<(std::ostream& os, const std::tuple<T...>& t) {
	if (I == 0) os << "(" << std::get<I>(t);
	else os << ", " << std::get<I>(t);
	return operator<< <I+1>(os, t);
}

/**
 * Output a std::unordered_map with arbitrary content.
 * The format is `{<key>:<value>, <key>:<value>, ...}`
 * @param os Output stream.
 * @param m map to be printed.
 * @return Output stream.
 */
template<typename Key, typename Value, typename H, typename E, typename A>
std::ostream& operator<<(std::ostream& os, const std::unordered_map<Key, Value, H, E, A>& m) {
	os << "{";
	bool first = true;
	for (const auto& it: m) {
		if (!first) os << ", ";
		first = false;
		os << it.first << " : " << it.second;
	}
	return os << "}";
}

/**
 * Output a std::unordered_set with arbitrary content.
 * The format is `{<length>: <item>, <item>, ...}`
 * @param os Output stream.
 * @param m unordered_set to be printed.
 * @return Output stream.
 */
template<typename T, typename H, typename K, typename A>
std::ostream& operator<<(std::ostream& os, const std::unordered_set<T, H, K, A>& s) {
	os << "{" << s.size() << ": ";
	bool first = true;
	for (const auto& it: s) {
		if (!first) os << ", ";
		first = false;
		os << it;
	}
	return os << "}";
}

/**
 * Output a std::vector with arbitrary content.
 * The format is `[<length>: <item>, <item>, ...]`
 * @param os Output stream.
 * @param v vector to be printed.
 * @return Output stream.
 */
template<typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v) {
	os << "[" << v.size() << ": ";
	bool first = true;
	for (const auto& it: v) {
		if (!first) os << ", ";
		first = false;
		os << it;
	}
	return os << "]";
}

}
