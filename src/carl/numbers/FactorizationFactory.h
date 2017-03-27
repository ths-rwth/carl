#pragma once

#include "PrimeFactory.h"

#include <boost/optional.hpp>

#include <vector>

namespace carl {

/**
 * This class provides a cached factorization for numbers.
 */
template<typename T>
class FactorizationFactory;

/**
 * This class provides a cached prime factorization for std::size_t.
 * Factorizations contain all prime factors, including multiples.
 * Additionally, we define:
 * - factorization(0) = {}
 * - factorization(1) = {1}
 */
template<>
class FactorizationFactory<std::size_t> {
private:
	PrimeFactory<std::size_t> mPrimes;
	std::vector<boost::optional<std::vector<std::size_t>>> mCache;
public:
	FactorizationFactory() {
		mCache.emplace_back(std::vector<std::size_t>({}));
		mCache.emplace_back(std::vector<std::size_t>({1}));
	}
	/// Returns the factorization of n.
	const std::vector<std::size_t>& operator()(std::size_t n) {
		while (mCache.size() <= n) {
			mCache.emplace_back();
		}
		if (mCache[n]) {
			return *mCache[n];
		}
		assert(n >= 2);
		mCache[n] = std::vector<std::size_t>();
		std::size_t cur = n;
		for (std::size_t i = 0; ;) {
			if (cur % mPrimes[i] == 0) {
				mCache[n]->push_back(mPrimes[i]);
				cur /= mPrimes[i];
			} else {
				i++;
			}
			if (cur == 1) break;
		}
		return *mCache[n];
	}
	
};

}
