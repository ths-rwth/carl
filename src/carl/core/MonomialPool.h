/**
 * @file MonomialPool.h
 * @author Florian Corzilius <corzilius@cs.rwth-aachen.de>
 */

//#define USE_MONOMIAL_POOL

#pragma once

#include "config.h"
#include "Monomial.h"

#ifdef USE_MONOMIAL_POOL

#include "../util/Common.h"
#include "../util/Singleton.h"

namespace carl
{   
	class MonomialPool : public Singleton<MonomialPool>
	{
		friend class Singleton<MonomialPool>;
		
		private:
			// Members:
			/// id allocator
			size_t mIdAllocator;
			/// The pool.
			std::unordered_set<Monomial::Arg, std::hash<Monomial::Arg>> mPool;
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
				mPool()
			{}
			
		public:
			
			/**
			 * Try to add the given monomial to the pool.
			 * @param _monomial The monomial to add.
			 * @return The corresponding monomial in the pool.
			 */
			Monomial::Arg add( const Monomial::Arg& _monomial );
			
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
	return MonomialPool::getInstance().create(t...);
#else
	return std::make_shared<const Monomial>(t...);
#endif
}

}