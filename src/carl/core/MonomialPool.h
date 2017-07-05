/**
 * @file MonomialPool.h
 * @author Florian Corzilius <corzilius@cs.rwth-aachen.de>
 */

#pragma once

#include "../config.h"
#include "../util/Common.h"
#include "../util/IDGenerator.h"
#include "../util/Singleton.h"
#include "Monomial.h"
#include "config.h"

#include <memory>
#include <unordered_set>

namespace carl{


	class MonomialPool : public Singleton<MonomialPool>
	{
		friend class Singleton<MonomialPool>;
		public:
#ifdef PRUNE_MONOMIAL_POOL
			struct PoolEntry {
				Monomial::Content content;
				std::size_t hash;
				mutable std::weak_ptr<const Monomial> monomial;
				PoolEntry(std::size_t h, Monomial::Content c, const Monomial::Arg& m): content(std::move(c)), hash(h), monomial(m) {}
				PoolEntry(std::size_t h, Monomial::Content c): content(std::move(c)), hash(h), monomial() {
					assert(monomial.expired());
				}
				explicit PoolEntry(Monomial::Content c): content(std::move(c)), hash(Monomial::hashContent(content)), monomial() {
					assert(monomial.expired());
				}
			};
#else
			struct PoolEntry {
				Monomial::Content content;
				std::size_t hash;
				mutable Monomial::Arg monomial;
				PoolEntry(std::size_t h, const Monomial::Content& c, const Monomial::Arg& m): content(c), hash(h), monomial(m) {}
				PoolEntry(std::size_t h, Monomial::Content c): content(std::move(c)), hash(h), monomial() {
				}
				explicit PoolEntry(Monomial::Content c): content(std::move(c)), hash(Monomial::hashContent(content)), monomial() {
				}
			};
#endif
			struct hash {
				std::size_t operator()(const PoolEntry& p) const {
					return p.hash;
				}
			};
			struct equal {
				bool operator()(const PoolEntry& p1, const PoolEntry& p2) const {
					if (p1.hash != p2.hash) return false;
#ifdef PRUNE_MONOMIAL_POOL
					if (p1.monomial.lock() && p2.monomial.lock()) {
						return p1.monomial.lock() == p2.monomial.lock();
					}
#else
					if (p1.monomial && p2.monomial) {
						return p1.monomial == p2.monomial;
					}
#endif
					return p1.content == p2.content;
				}
			};
		private:
			// Members:
			/// id allocator
			IDGenerator mIDs;
			//size_t mIdAllocator;
			/// The pool.
			std::unordered_set<PoolEntry, MonomialPool::hash, MonomialPool::equal> mPool;
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
				Singleton<MonomialPool>(),
				mIDs(),
				mPool(_capacity),
				mMutex()
			{}

			Monomial::Arg add( MonomialPool::PoolEntry&& pe, exponent totalDegree = 0 );
		public:
			
			/**
			 * Try to add the given monomial to the pool.
			 * @param _monomial The monomial to add.
			 * @return The corresponding monomial in the pool.
			 */
			Monomial::Arg add( const Monomial::Arg& _monomial );
			Monomial::Arg add( Monomial::Content&& c, exponent totalDegree = 0 );
			
			Monomial::Arg create();
			Monomial::Arg create( Variable _var, exponent _exp );
			
			Monomial::Arg create( std::vector<std::pair<Variable, exponent>>&& _exponents, exponent _totalDegree );
			
			Monomial::Arg create( const std::initializer_list<std::pair<Variable, exponent>>& _exponents );
			
			Monomial::Arg create( std::vector<std::pair<Variable, exponent>>&& _exponents );

#ifdef PRUNE_MONOMIAL_POOL
			void free(const Monomial* m) {
				if (m == nullptr) return;
				if (m->id() == 0) return;
				MONOMIAL_POOL_LOCK_GUARD;
				PoolEntry pe(m->mHash, m->mExponents);
				auto it = mPool.find(pe);
				if (it != mPool.end()) {
					mIDs.free(m->id());
					mPool.erase(it);
				}
			}
#endif


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
			std::size_t nextID() const {
				return mIDs.nextID();
			}
	};
} // end namespace carl

namespace carl {

template<typename... T>
inline Monomial::Arg createMonomial(T&&... t) {
	return MonomialPool::getInstance().create(std::forward<T>(t)...);
}

}
