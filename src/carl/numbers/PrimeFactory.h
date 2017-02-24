#pragma once

#include "numbers.h"

namespace carl {

template<typename T>
class PrimeFactory
{
	static std::vector<T> mPrimes;
	std::size_t mNext = 0;
public:
	const T& nextPrime();
};

template<typename T>
std::vector<T> PrimeFactory<T>::mPrimes = {
	2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41,
	43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97
};

template<typename T>
const T& PrimeFactory<T>::nextPrime() {
	if (mNext == mPrimes.size()) {
		mPrimes.emplace_back(carl::next_prime(mPrimes.back()));
	}
    return mPrimes[mNext++];
}

}
