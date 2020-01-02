/**
 * @file MonomialPool.h
 * @author Florian Corzilius <corzilius@cs.rwth-aachen.de>
 */

#pragma once

#include "../config.h"
#include "../util/IDPool.h"
#include "../util/Singleton.h"
#include "Monomial.h"
#include "config.h"

#include <memory>
#include <boost/intrusive/unordered_set.hpp>

namespace carl{

	inline std::size_t hash_value(const carl::Monomial& monomial) {
		return monomial.hash();
	}

	class MonomialPool : public Singleton<MonomialPool>
	{
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
			/// The pool.
			using underlying_set = boost::intrusive::unordered_set<Monomial>; 
			std::unique_ptr<underlying_set::bucket_type[]> mPoolBuckets;
			underlying_set mPool;
			/// Mutex to avoid multiple access to the pool
			mutable std::recursive_mutex mMutex;
			
            #ifdef THREAD_SAFE
			#define MONOMIAL_POOL_LOCK_GUARD std::lock_guard<std::recursive_mutex> lock( mMutex );
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
			explicit MonomialPool( std::size_t _capacity = 10000 ):
				mPoolBuckets(new underlying_set::bucket_type[_capacity]),
				mPool(underlying_set::bucket_traits(mPoolBuckets.get(), _capacity))
			{
				mIDs.get();
				assert(mIDs.largestID() == 0);
				VariablePool::getInstance();
				CARL_LOG_DEBUG("carl.pool", "Monomialpool constructed");
			}
			
			~MonomialPool() {
				CARL_LOG_DEBUG("carl.pool", "Monomialpool destructed");
			}

			Monomial::Arg add( Monomial::Content&& c, exponent totalDegree = 0 );

		public:
			Monomial::Arg create( Variable _var, exponent _exp );

			template<typename Number>
			Monomial::Arg create( Variable _var, Number&& _exp ) {
				return create(_var, static_cast<exponent>(std::forward<Number>(_exp)));
			}
			
			Monomial::Arg create( std::vector<std::pair<Variable, exponent>>&& _exponents, exponent _totalDegree );
			
			Monomial::Arg create( const std::initializer_list<std::pair<Variable, exponent>>& _exponents );
			
			Monomial::Arg create( std::vector<std::pair<Variable, exponent>>&& _exponents );

			void free(const Monomial* m) {
				CARL_LOG_TRACE("carl.core.monomial", "Freeing " << m);
				if (m == nullptr) return;
				if (m->id() == 0) return;
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

			/**
			 * Clears everything already created in this pool.
			 */
			void clear() {
				mPool.clear();
				mIDs.clear();
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
		for (const auto& entry: mp.mPool) {
			os << "\t" << entry << std::endl;
		}
		return os;
	}
} // end namespace carl

namespace carl {

template<typename... T>
inline Monomial::Arg createMonomial(T&&... t) {
	return MonomialPool::getInstance().create(std::forward<T>(t)...);
}

}
