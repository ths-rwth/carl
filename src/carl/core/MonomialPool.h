/**
 * @file MonomialPool.h
 * @author Florian Corzilius <corzilius@cs.rwth-aachen.de>
 */

#pragma once

#include "../config.h"
#include "../util/Common.h"
#include "../util/IDPool.h"
#include "../util/Singleton.h"
#include "Monomial.h"
#include "config.h"

#include <memory>
#include <unordered_set>

namespace carl{


	class MonomialPool : public Singleton<MonomialPool>
	{
		friend class Singleton<MonomialPool>;
		friend std::ostream& operator<<(std::ostream& os, const MonomialPool& mp);
		public:
			struct PoolEntry {
				Monomial::Content content;
				std::size_t hash;
				mutable std::weak_ptr<const Monomial> monomial;
				PoolEntry(std::size_t h, Monomial::Content c, const Monomial::Arg& m): content(std::move(c)), hash(h), monomial(m) {}
				PoolEntry(std::size_t h, Monomial::Content c): content(std::move(c)), hash(h) {
					assert(monomial.expired());
				}
				explicit PoolEntry(Monomial::Content c): content(std::move(c)), hash(Monomial::hashContent(content)) {
					assert(monomial.expired());
				}
			};
			struct hash {
				std::size_t operator()(const PoolEntry& p) const {
					return p.hash;
				}
			};
			struct equal {
				bool operator()(const PoolEntry& p1, const PoolEntry& p2) const {
					CARL_LOG_TRACE("carl.core.monomial", p1.content << " / " << p1.hash << " / " << p1.monomial.lock().get() << " == " << p2.content << " / " << p2.hash << " / " << p2.monomial.lock().get());
					if (p1.hash != p2.hash) {
						CARL_LOG_TRACE("carl.core.monomial", "No due to hash");
						return false;
					}
					if (p1.monomial.lock() && p2.monomial.lock()) {
						CARL_LOG_TRACE("carl.core.monomial", "Comparing pointers");
						return p1.monomial.lock() == p2.monomial.lock();
					}
					CARL_LOG_TRACE("carl.core.monomial", "Comparing content");
					return p1.content == p2.content;
				}
			};
		private:
			// Members:
			/// id allocator
			IDPool mIDs;
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
				mPool(_capacity)
			{
				mIDs.get();
				assert(mIDs.largestID() == 0);
				VariablePool::getInstance();
				CARL_LOG_DEBUG("carl.pool", "Monomialpool constructed");
			}
			
			~MonomialPool() {
				CARL_LOG_DEBUG("carl.pool", "Monomialpool destructed");
			}

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
			template<typename Number>
			Monomial::Arg create( Variable _var, Number&& _exp ) {
				return create(_var, carl::toInt<exponent>(std::forward<Number>(_exp)));
			}
			
			Monomial::Arg create( std::vector<std::pair<Variable, exponent>>&& _exponents, exponent _totalDegree );
			
			Monomial::Arg create( const std::initializer_list<std::pair<Variable, exponent>>& _exponents );
			
			Monomial::Arg create( std::vector<std::pair<Variable, exponent>>&& _exponents );

			void free(const Monomial* m) {
				CARL_LOG_TRACE("carl.core.monomial", "Freeing " << m);
				if (m == nullptr) return;
				if (m->id() == 0) return;
				MONOMIAL_POOL_LOCK_GUARD;
				PoolEntry pe(m->mHash, m->mExponents);
				auto it = mPool.find(pe);
				if (it != mPool.end()) {
					CARL_LOG_TRACE("carl.core.monomial", "Found " << it->content << " / " << it->hash);
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
			os << "\t" << entry.content << " / " << entry.hash << std::endl;
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
