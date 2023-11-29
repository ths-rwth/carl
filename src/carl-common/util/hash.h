#pragma once

#include <utility>
#include <vector>
#include <set>

namespace carl {

/**
 * Add a value to the given hash seed.
 * This method is a copy of boost::hash_combine().
 * It is reimplemented here to avoid including all of boost/functional/hash.hpp for this single line of code.
 */
inline void hash_combine(std::size_t& seed, std::size_t value) {
	seed ^= value + 0x9e3779b9 + (seed<<6u) + (seed>>2u);
}

/**
 * Add hash of the given value to the hash seed.
 * Used `hash_combine` with the result of `std::hash<T>`.
 */
template<typename T>
inline void hash_add(std::size_t& seed, const T& value) {
	carl::hash_combine(seed, std::hash<T>()(value));
}

/**
 * Add hash of the given value to the hash seed.
 */
template<>
inline void hash_add(std::size_t& seed, const std::size_t& value) {
	carl::hash_combine(seed, value);
}

/**
 * Add hash of both elements of a `std::pair` to the seed.
 */
template<typename T1, typename T2>
inline void hash_add(std::size_t& seed, const std::pair<T1, T2>& p) {
	carl::hash_add(seed, p.first);
	carl::hash_add(seed, p.second);
}

/**
 * Add hash of all elements of a `std::vector` to the seed.
 */
template<typename T>
inline void hash_add(std::size_t& seed, const std::vector<T>& v) {
	for (const auto& t: v) carl::hash_add(seed, t);
}

template<typename T>
inline void hash_add(std::size_t& seed, const std::set<T>& s) {
	for (const auto& t: s) carl::hash_add(seed, t);
}

/**
 * Variadic version of `hash_add` to add an arbitrary number of values to the seed.
 */
template<typename First, typename... Tail>
inline void hash_add(std::size_t& seed, const First& value, Tail&&... tail) {
	carl::hash_add(seed, value);
	carl::hash_add(seed, std::forward<Tail>(tail)...);
}

/**
 * Hashes an arbitrary number of values.
 * Uses `hash_add` with a seed of `0`.
 */
template<typename... Args>
inline std::size_t hash_all(Args&&... args) {
	std::size_t seed = 0;
	hash_add(seed, std::forward<Args>(args)...);
	return seed;
}

/**
 * Utility functor to hash a sequence of object using an output iterator.
 */
template<typename T>
struct hash_inserter {
	using difference_type = void;
	using pointer = void;
	using reference = void;
	using value_type = void;
	using iterator_category = std::output_iterator_tag;

	std::size_t& seed;
	hash_inserter& operator=(const T& t) {
		hash_add(seed, t);
		return *this;
	}
	hash_inserter& operator*() { return *this; }
	hash_inserter& operator++() { return *this; }
	const hash_inserter operator++(int) { return *this; }
};

} // namespace carl
