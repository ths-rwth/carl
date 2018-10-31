/**
 * @file streamingOperators.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 *
 * This file contains streaming operators for most of the STL containers.
 */

#pragma once

#include <cassert>
#include <forward_list>
#include <iostream>
#include <list>
#include <map>
#include <optional>
#include <set>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <variant>
#include <vector>

namespace carl {

/*
template<typename T>
inline std::ostream& operator<<(std::ostream& os, const std::forward_list<T>& l);
template<typename T>
inline std::ostream& operator<<(std::ostream& os, const std::list<T>& l);
template<typename Key, typename Value, typename Comparator>
inline std::ostream& operator<<(std::ostream& os, const std::map<Key, Value, Comparator>& m);
template<typename Key, typename Value, typename Comparator>
inline std::ostream& operator<<(std::ostream& os, const std::multimap<Key, Value, Comparator>& m);
template<typename T>
inline std::ostream& operator<<(std::ostream& os, const std::optional<T>& o);
template<typename U, typename V>
inline std::ostream& operator<<(std::ostream& os, const std::pair<U, V>& p);
template<typename T, typename C>
inline std::ostream& operator<<(std::ostream& os, const std::set<T, C>& s);
template<std::size_t I, typename... T, typename std::enable_if<I == sizeof...(T), void>::type*>
inline std::ostream& operator<<(std::ostream& os, const std::tuple<T...>& t);
template<std::size_t I, typename... T, typename std::enable_if<I < sizeof...(T), void>::type*>
inline std::ostream& operator<<(std::ostream& os, const std::tuple<T...>& t);
template<typename Key, typename Value, typename H, typename E, typename A>
inline std::ostream& operator<<(std::ostream& os, const std::unordered_map<Key, Value, H, E, A>& m);
template<typename T, typename H, typename K, typename A>
inline std::ostream& operator<<(std::ostream& os, const std::unordered_set<T, H, K, A>& s);
template<typename T, typename... Tail>
inline std::ostream& operator<<(std::ostream& os, const std::variant<T, Tail...>& v);
template<typename T>
inline std::ostream& operator<<(std::ostream& os, const std::vector<T>& v);
*/

/**
 * Output a std::forward_list with arbitrary content.
 * The format is `[<item>, <item>, ...]`
 * @param os Output stream.
 * @param l list to be printed.
 * @return Output stream.
 */
template<typename T>
inline std::ostream& operator<<(std::ostream& os, const std::forward_list<T>& l) {
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
inline std::ostream& operator<<(std::ostream& os, const std::list<T>& l) {
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
inline std::ostream& operator<<(std::ostream& os, const std::map<Key, Value, Comparator>& m) {
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
 * Output a std::multimap with arbitrary content.
 * The format is `{<key>:<value>, <key>:<value>, ...}`
 * @param os Output stream.
 * @param m multimap to be printed.
 * @return Output stream.
 */
template<typename Key, typename Value, typename Comparator>
inline std::ostream& operator<<(std::ostream& os, const std::multimap<Key, Value, Comparator>& m) {
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
 * Output a std::optional with arbitrary content.
 * Prints `empty` if the optional holds no value and forwards the call to the content otherwise.
 * @param os Output stream.
 * @param o optional to be printed.
 * @return Output stream.
 */
template<typename T>
inline std::ostream& operator<<(std::ostream& os, const std::optional<T>& o) {
	if (o.has_value()) return os << *o;
	return os << "empty";
}

/**
 * Output a std::pair with arbitrary content.
 * The format is `(<first>, <second>)`
 * @param os Output stream.
 * @param p pair to be printed.
 * @return Output stream.
 */
template<typename U, typename V>
inline std::ostream& operator<<(std::ostream& os, const std::pair<U, V>& p) {
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
inline std::ostream& operator<<(std::ostream& os, const std::set<T, C>& s) {
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
inline std::ostream& operator<<(std::ostream& os, const std::tuple<T...>& /*unused*/) {
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
inline std::ostream& operator<<(std::ostream& os, const std::unordered_map<Key, Value, H, E, A>& m) {
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
inline std::ostream& operator<<(std::ostream& os, const std::unordered_set<T, H, K, A>& s) {
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
 * Output a std::variant with arbitrary content.
 * The call is simply forwarded to whatever content is currently stored in the variant.
 * @param os Output stream.
 * @param v variant to be printed.
 * @return Output stream.
 */
template<typename T, typename... Tail>
inline std::ostream& operator<<(std::ostream& os, const std::variant<T, Tail...>& v) {
	return std::visit([&os](const auto& value) -> auto& { return os << value; }, v);
}

/**
 * Output a std::vector with arbitrary content.
 * The format is `[<length>: <item>, <item>, ...]`
 * @param os Output stream.
 * @param v vector to be printed.
 * @return Output stream.
 */
template<typename T>
inline std::ostream& operator<<(std::ostream& os, const std::vector<T>& v) {
	os << "[" << v.size() << ": ";
	bool first = true;
	for (const auto& it: v) {
		if (!first) os << ", ";
		first = false;
		os << it;
	}
	return os << "]";
}

namespace detail {
	template<typename T>
	struct stream_joined_impl {
		std::string glue;
		const T& values;
	};
	template<typename T>
	std::ostream& operator<<(std::ostream& os, const stream_joined_impl<T>& sji) {
		auto it = sji.values.begin();
		if (it == sji.values.end()) return os;
		os << *it;
		for (++it; it != sji.values.end(); ++it) os << sji.glue << *it;
		return os;
	}
}
/**
 * Allows to easily output some container with all elements separated by some string.
 * Usage: `os << stream_joined(" ", container)`.
 * @param glue The intermediate string.
 * @param v The container to be printed.
 * @return A temporary object that implements `operator<<()`.
 */
template<typename T>
inline auto stream_joined(const std::string& glue, const T& v) {
	return detail::stream_joined_impl<T>{glue, v};
}

}
