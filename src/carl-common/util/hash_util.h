#pragma once

#include <boost/functional/hash.hpp>

namespace std {
template<typename T>
struct hash<std::vector<T>> {
	std::size_t operator()(const std::vector<T>& v) const {
		std::size_t seed = 0;
		std::hash<T> h;
		for (const auto& t : v)
			boost::hash_combine(seed, h(t));
		return seed;
	}
};
} // namespace std