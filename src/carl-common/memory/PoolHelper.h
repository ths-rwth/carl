#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <utility>

namespace carl {
namespace pool {

/**
 * Mimics stdlibs default rehash policy for hashtables.
 * See https://gcc.gnu.org/onlinedocs/libstdc++/libstdc++-html-USERS-4.1/hashtable-source.html
 */
class RehashPolicy {
	static const int numPrimes = sizeof(unsigned long) != 8 ? 256 : 256 + 48;
	static const unsigned long primes[256 + 48 + 1];

	float mMaxLoadFactor; // stdlib uses 1
	float mGrowthFactor; // stdlib uses 2
	mutable std::size_t mNextResize;

public:
	RehashPolicy(float maxLoadFactor = 0.95f, float growthFactor = 2.f)
		: mMaxLoadFactor(maxLoadFactor), mGrowthFactor(growthFactor), mNextResize(0) {}

	std::size_t numBucketsFor(std::size_t numElements) const;
	std::pair<bool, std::size_t> needRehash(std::size_t numBuckets, std::size_t numElements) const;
};

} // namespace pool
} // namespace carl