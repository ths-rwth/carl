/**
 * @file MonomialPool.h
 * @author Florian Corzilius <corzilius@cs.rwth-aachen.de>
 */

#pragma once

#include "config.h"
#include "Monomial.h"

#ifdef USE_MONOMIAL_POOL

#include <unordered_set>

#include "../util/Common.h"
#include "../util/Singleton.h"

namespace carl{

#define NEWPOOL

	class MonomialPool : public Singleton<MonomialPool>
	{
		friend class Singleton<MonomialPool>;
#ifdef NEWPOOL
		public:
			struct PoolEntry {
				std::size_t hash;
				Monomial::Content content;
				mutable Monomial::Arg monomial;
				PoolEntry(std::size_t h, const Monomial::Content& c, const Monomial::Arg& m): hash(h), content(c), monomial(m) {}
				PoolEntry(std::size_t h, Monomial::Content&& c): hash(h), content(std::move(c)), monomial(nullptr) {}
			};
			struct hash {
				std::size_t operator()(const PoolEntry& p) const {
					return p.hash;
				}
			};
			struct equal {
				bool operator()(const PoolEntry& p1, const PoolEntry& p2) const {
					if (p1.hash != p2.hash) return false;
					if (p1.monomial && p2.monomial) {
						return p1.monomial == p2.monomial;
					}
					return p1.content == p2.content;
				}
			};
#endif
		private:
			// Members:
			/// id allocator
			size_t mIdAllocator;
			/// The pool.
#ifdef NEWPOOL
			std::unordered_set<PoolEntry, MonomialPool::hash, MonomialPool::equal> mPool;
#else
			std::unordered_set<Monomial::Arg, std::hash<Monomial::Arg>> mPool;
#endif
			/// Mutex to avoid multiple access to the pool
			mutable std::mutex mMutex;
			
			#define MONOMIAL_POOL_LOCK_GUARD std::lock_guard<std::mutex> lock( mMutex );
			#define MONOMIAL_POOL_LOCK mMutexPool.lock();
			#define MONOMIAL_POOL_UNLOCK mMutexPool.unlock();
			
		protected:
			
			/**
			 * Constructor of the pool.
			 * @param _capacity Expected necessary capacity of the pool.
			 */
			MonomialPool( unsigned _capacity = 10000 ):
				Singleton<MonomialPool>(),
				mIdAllocator( 1 ),
				mPool(_capacity)
			{}

#ifdef NEWPOOL
			Monomial::Arg add( MonomialPool::PoolEntry&& pe, exponent totalDegree = 0 );
#endif
		public:
			
			/**
			 * Try to add the given monomial to the pool.
			 * @param _monomial The monomial to add.
			 * @return The corresponding monomial in the pool.
			 */
			Monomial::Arg add( const Monomial::Arg& _monomial );
#ifdef NEWPOOL
			Monomial::Arg add( Monomial::Content&& c, exponent totalDegree = 0 );
#endif
			
			Monomial::Arg create( Variable::Arg _var, exponent _exp );
			
			Monomial::Arg create( std::vector<std::pair<Variable, exponent>>&& _exponents, exponent _totalDegree );
			
			Monomial::Arg create( const std::initializer_list<std::pair<Variable, exponent>>& _exponents, exponent _totalDegree );
			
			Monomial::Arg create( std::vector<std::pair<Variable, exponent>>&& _exponents );
	};
} // end namespace carl

#endif

namespace carl {

template<typename... T>
inline Monomial::Arg createMonomial(T... t) {
#ifdef USE_MONOMIAL_POOL
	return MonomialPool::getInstance().create(std::forward<T>(t)...);
#else
	return std::make_shared<const Monomial>(std::forward<T>(t)...);
#endif
}

}