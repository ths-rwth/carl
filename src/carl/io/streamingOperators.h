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
#include <deque>

namespace carl {


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
template<typename... T>
std::ostream& operator<<(std::ostream& os, const std::tuple<T...>& t);
template<typename Key, typename Value, typename H, typename E, typename A>
inline std::ostream& operator<<(std::ostream& os, const std::unordered_map<Key, Value, H, E, A>& m);
template<typename T, typename H, typename K, typename A>
inline std::ostream& operator<<(std::ostream& os, const std::unordered_set<T, H, K, A>& s);
template<typename T, typename... Tail>
inline std::ostream& operator<<(std::ostream& os, const std::variant<T, Tail...>& v);
template<typename T>
inline std::ostream& operator<<(std::ostream& os, const std::vector<T>& v);
template<typename T>
inline std::ostream& operator<<(std::ostream& os, const std::deque<T>& v);


/**
 * Allows to easily output some container with all elements separated by some string.
 * Usage: `os << stream_joined(" ", container)`.
 * @param glue The intermediate string.
 * @param v The container to be printed.
 * @return A temporary object that implements `operator<<()`.
 */
template<typename T>
inline auto stream_joined(const std::string& glue, const T& v);

/**
 * Allows to easily output some container with all elements separated by some string.
 * An additional callable `f` takes care of writing an individual element to the stream.
 * Usage: `os << stream_joined(" ", container)`.
 * @param glue The intermediate string.
 * @param v The container to be printed.
 * @param f A callable taking a stream and an element of `v`.
 * @return A temporary object that implements `operator<<()`.
 */
template<typename T, typename F>
inline auto stream_joined(const std::string& glue, const T& v, F&& f);


/**
 * Output a std::forward_list with arbitrary content.
 * The format is `[<item>, <item>, ...]`
 * @param os Output stream.
 * @param l list to be printed.
 * @return Output stream.
 */
template<typename T>
inline std::ostream& operator<<(std::ostream& os, const std::forward_list<T>& l) {
	return os << "[" << stream_joined(", ", l) << "]";
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
	return os << "[" << l.size() << ": " << stream_joined(", ", l) << "]";
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
	return os << "{" << stream_joined(", ", m, [](auto& o, const auto& p){ o << p.first << " : " << p.second; }) << "}";
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
	return os << "{" << stream_joined(", ", m, [](auto& o, const auto& p){ o << p.first << " : " << p.second; }) << "}";
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
	return os << "{" << s.size() << ": " << stream_joined(", ", s) << "}";
}

namespace detail {
/**
 * Helper function that actually outputs a std::tuple.
 * The format is `(<item>, <item>, ...)`
 * @param os Output stream.
 * @param t tuple to be printed.
 * @return Output stream.
 */
template<typename Tuple, std::size_t... I>
std::ostream& stream_tuple_impl(std::ostream& os, const Tuple& t, std::index_sequence<I...>) {
	(..., (os << (I == 0 ? "(" : ", ") << std::get<I>(t)));
	return os << ")";
}
}

/**
 * Output a std::tuple with arbitrary content.
 * The format is `(<item>, <item>, ...)`
 * @param os Output stream.
 * @param t tuple to be printed.
 * @return Output stream.
 */
template<typename... T>
std::ostream& operator<<(std::ostream& os, const std::tuple<T...>& t) {
	return detail::stream_tuple_impl(os, t, std::make_index_sequence<sizeof...(T)>());
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
	return os << "{" << stream_joined(", ", m, [](auto& o, const auto& p){ o << p.first << " : " << p.second; }) << "}";
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
	return os << "{" << s.size() << ": " << stream_joined(", ", s) << "}";
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
	return os << "[" << v.size() << ": " << stream_joined(", ", v) << "]";
}

/**
 * Output a std::deque with arbitrary content.
 * The format is `[<length>: <item>, <item>, ...]`
 * @param os Output stream.
 * @param v vector to be printed.
 * @return Output stream.
 */
template<typename T>
inline std::ostream& operator<<(std::ostream& os, const std::deque<T>& v) {
	return os << "[" << v.size() << ": " << stream_joined(", ", v) << "]";
}

namespace detail {
	template<typename T, typename F>
	struct stream_joined_impl {
		std::string glue;
		const T& values;
		F callable;
	};
	template<typename T, typename F>
	std::ostream& operator<<(std::ostream& os, const stream_joined_impl<T,F>& sji) {
		auto it = sji.values.begin();
		if (it == sji.values.end()) {
			return os;
		}
		sji.callable(os, *it);
		for (++it; it != sji.values.end(); ++it) {
			os << sji.glue;
			sji.callable(os, *it);
		}
		return os;
	}
}
template<typename T>
inline auto stream_joined(const std::string& glue, const T& v) {
	auto f = [](auto& os, const auto& t){ os << t; };
	return detail::stream_joined_impl<T, decltype(f)>{glue, v, f};
}

template<typename T, typename F>
inline auto stream_joined(const std::string& glue, const T& v, F&& f) {
	return detail::stream_joined_impl<T,F>{glue, v, std::forward<F>(f)};
}

}
