/**
 * @file MonomialPool.h
 * @author Florian Corzilius <corzilius@cs.rwth-aachen.de>
 */

#pragma once

#include <carl-common/config.h>
#include <carl-common/memory/IDPool.h>
#include <carl-common/memory/PoolHelper.h>
#include <carl-common/memory/Singleton.h>
#include "Monomial.h"

#include <boost/intrusive/unordered_set.hpp>
#include <memory>

namespace carl {

inline std::size_t hash_value(const carl::Monomial& monomial) {
	return monomial.hash();
}

class MonomialPool : public Singleton<MonomialPool> {
	friend class Singleton<MonomialPool>;
	friend std::ostream& operator<<(std::ostream& os, const MonomialPool& mp);

	struct content_equal {
		bool operator()(const Monomial::Content& content, const Monomial& monomial) const {
			return content == monomial.mExponents;
		}

		bool operator()(const Monomial& monomial, const Monomial::Content& content) const {
			return content == monomial.mExponents;
		}
	};

	struct content_hash {
		std::size_t operator()(const Monomial::Content& content) const {
			return Monomial::hashContent(content);
		}
	};

private:
	// Members:
	/// id allocator
	IDPool mIDs;
	//size_t mIdAllocator;
	pool::RehashPolicy mRehashPolicy;
	using underlying_set = boost::intrusive::unordered_set<Monomial>;
	std::unique_ptr<underlying_set::bucket_type[]> mPoolBuckets;
	/// The pool.
	underlying_set mPool;
	/// Mutex to avoid multiple access to the pool
	mutable std::recursive_mutex mMutex;

	#ifdef THREAD_SAFE
	#define MONOMIAL_POOL_LOCK_GUARD std::lock_guard<std::recursive_mutex> lock(mMutex);
	#define MONOMIAL_POOL_LOCK mMutex.lock();
	#define MONOMIAL_POOL_UNLOCK mMutex.unlock();
	#else
	#define MONOMIAL_POOL_LOCK_GUARD
	#define MONOMIAL_POOL_LOCK
	#define MONOMIAL_POOL_UNLOCK
	#endif

protected:
	/**
	 * Constructor of the pool.
	 * @param _capacity Expected necessary capacity of the pool.
	 */
	explicit MonomialPool(std::size_t _capacity = 1000)
		: mPoolBuckets(new underlying_set::bucket_type[mRehashPolicy.numBucketsFor(_capacity)]),
		  mPool(underlying_set::bucket_traits(mPoolBuckets.get(), mRehashPolicy.numBucketsFor(_capacity))) {
		mIDs.get();
		assert(mIDs.largestID() == 0);
		VariablePool::getInstance();
		CARL_LOG_DEBUG("carl.pool", "Monomialpool constructed");
	}

	~MonomialPool() {
		// for (const auto& m : mPool) m.mId = 0; // hacky fix for singletons not being destroyed in proper order
		// CARL_LOG_DEBUG("carl.pool", "Monomialpool destructed");
	}

	Monomial::Arg add(Monomial::Content&& c, exponent totalDegree = 0);

	void check_rehash() {
		auto rehash = mRehashPolicy.needRehash(mPool.bucket_count(), mPool.size());
		if (rehash.first) {
			auto new_buckets = new underlying_set::bucket_type[rehash.second];
			mPool.rehash(underlying_set::bucket_traits(new_buckets, rehash.second));
			mPoolBuckets.reset(new_buckets);
		}
	}

public:
	/**
	 * Creates a monomial from a variable and an exponent.
	 */
	Monomial::Arg create(Variable _var, exponent _exp);

	/**
	 * Creates a monomial from a variable and an exponent.
	 */
	template<typename Number>
	Monomial::Arg create(Variable _var, Number&& _exp) {
		return create(_var, static_cast<exponent>(std::forward<Number>(_exp)));
	}

	/**
	 * Creates a monomial from a list of variables and their exponents.
	 * 
	 * Note that the input is required to be sorted.
	 * 
	 * @param _exponents Sorted list of variables and exponents.
	 * @param _totalDegree Total degree.
	 */
	Monomial::Arg create(std::vector<std::pair<Variable, exponent>>&& _exponents, exponent _totalDegree);

	/**
	 * Creates a Monomial.
	 * 
	 * @param _exponents Possibly unsorted list of variables and epxonents.
	 */
	Monomial::Arg create(const std::initializer_list<std::pair<Variable, exponent>>& _exponents);

	/**
	 * Creates a monomial from a list of variables and their exponents.
	 * 
	 * Note that the input is required to be sorted.
	 * 
	 * @param Sorted list of variables and exponents.
	 */
	Monomial::Arg create(std::vector<std::pair<Variable, exponent>>&& _exponents);

	void free(const Monomial* m) {
		if (m == nullptr) return;
		if (m->id() == 0) return;
		CARL_LOG_TRACE("carl.core.monomial", "Freeing " << m);
		MONOMIAL_POOL_LOCK_GUARD;
		auto it = mPool.find(*m);
		if (it != mPool.end()) {
			CARL_LOG_TRACE("carl.core.monomial", "Found " << m->id());
			mIDs.free(m->id());
			mPool.erase(it);
		} else {
			CARL_LOG_TRACE("carl.core.monomial", "Not found in pool.");
		}
	}

	std::size_t size() const {
		return mPool.size();
	}
	std::size_t largestID() const {
		return mIDs.largestID();
	}
};

inline std::ostream& operator<<(std::ostream& os, const MonomialPool& mp) {
	os << "MonomialPool of size " << mp.size() << std::endl;
	for (const auto& entry : mp.mPool) {
		os << "\t" << entry << std::endl;
	}
	return os;
}

template<typename... T>
inline Monomial::Arg createMonomial(T&&... t) {
	return MonomialPool::getInstance().create(std::forward<T>(t)...);
}

} // namespace carl
