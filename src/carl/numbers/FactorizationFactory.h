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
class FactorizationFactory<uint> {
private:
	PrimeFactory<uint> mPrimes;
	std::vector<boost::optional<std::vector<uint>>> mCache;
public:
	FactorizationFactory() {
		mCache.emplace_back(std::vector<uint>({}));
		mCache.emplace_back(std::vector<uint>({1}));
	}
	/// Returns the factorization of n.
	const std::vector<uint>& operator()(uint n) {
		while (mCache.size() <= n) {
			mCache.emplace_back();
		}
		if (mCache[n]) {
			return *mCache[n];
		}
		assert(n >= 2);
		mCache[n] = std::vector<std::size_t>();
		uint cur = n;
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
