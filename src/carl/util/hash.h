#pragma once

#include <boost/functional/hash.hpp>
#include <utility>
#include <vector>

namespace carl {
template<typename T>
inline void hash_add(std::size_t& seed, const T& value) {
	boost::hash_combine(seed, std::hash<T>()(value));
}

template<>
inline void hash_add(std::size_t& seed, const std::size_t& value) {
	boost::hash_combine(seed, value);
}

template<typename First, typename... Tail>
inline void hash_add(std::size_t& seed, const First& value, Tail&&... tail) {
	carl::hash_add(seed, value);
	carl::hash_add(seed, std::forward<Tail>(tail)...);
}

template<typename T1, typename T2>
inline void hash_add(std::size_t& seed, const std::pair<T1, T2>& p) {
	carl::hash_add(seed, p.first);
	carl::hash_add(seed, p.second);
}

template<typename T>
inline void hash_add(std::size_t& seed, const std::vector<T>& v) {
	for (const auto& t: v) carl::hash_add(seed, t);
}

template<typename... Args>
inline std::size_t hash_all(Args&&... args) {
	std::size_t seed = 0;
	hash_add(seed, std::forward<Args>(args)...);
	return seed;
}
} // namespace carl
