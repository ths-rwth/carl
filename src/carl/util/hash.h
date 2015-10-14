#pragma once

#include <boost/functional/hash.hpp>

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
}
