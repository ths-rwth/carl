#pragma once

#include "numbers.h"

#include <mutex>

namespace carl {

/**
 * This class provides a convenient way to enumerate primes.
 */
template<typename T>
class PrimeFactory
{
	static std::vector<T> mPrimes;
#ifdef THREAD_SAFE
	static std::mutex mPrimeMutex;
#endif
	std::size_t mNext = 0;
public:
	/// Returns the number of already computed primes.
	std::size_t size() const {
		return mPrimes.size();
	}
	/// Provides const access to the computed primes. Asserts that id is smaller than size().
	const T& operator[](std::size_t id) const {
		assert(id < mPrimes.size());
		return mPrimes[id];
	}
	/// Provides access to the computed primes. If id is at least size(), the missing primes are computed on-the-fly.
	const T& operator[](std::size_t id) {
		while (id >= mPrimes.size()) nextPrime();
		return mPrimes[id];
	}
	/// Computed the next prime and returns it.
	const T& nextPrime();
};

template<typename T>
std::vector<T> PrimeFactory<T>::mPrimes = {
	2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41,
	43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97
};
#ifdef THREAD_SAFE
template<typename T>
std::mutex PrimeFactory<T>::mPrimeMutex;
#endif

namespace detail {
	inline uint next_prime(const uint& n, const PrimeFactory<uint>& pf) {
		for (uint res = n + 2; ; res += 2) {
			bool found = false;
			for (std::size_t i = 1; !found && (i < pf.size()); i++) {
				if (res % pf[i] == 0) found = true;
			}
			if (!found) return res;
		}
	}
	
	inline mpz_class next_prime(const mpz_class& n, const PrimeFactory<mpz_class>&) {
		mpz_class res;
		mpz_nextprime(res.get_mpz_t(), n.get_mpz_t());
		return res;
	}

#ifdef USE_CLN_NUMBERS
	inline cln::cl_I next_prime(const cln::cl_I& n, const PrimeFactory<cln::cl_I>&) {
		return cln::nextprobprime(n + 1);
	}
#endif
}

template<typename T>
const T& PrimeFactory<T>::nextPrime() {
	if (mNext == mPrimes.size()) {
#ifdef THREAD_SAFE
		std::lock_guard<std::mutex> guard(mPrimeMutex);
#endif
		mPrimes.emplace_back(detail::next_prime(mPrimes.back(), *this));
	}
    return mPrimes[mNext++];
}

}
