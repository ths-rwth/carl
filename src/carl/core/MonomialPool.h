/**
 * @file MonomialPool.h
 * @author Florian Corzilius <corzilius@cs.rwth-aachen.de>
 */

//#define USE_MONOMIAL_POOL

#pragma once

#ifdef USE_MONOMIAL_POOL

#include "../util/Common.h"
#include "../util/Singleton.h"
#include "Monomial.h"

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
            FastSharedPointerSet<Monomial> mPool;
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
            std::shared_ptr<const Monomial> add( std::shared_ptr<const Monomial> _monomial );
            
            std::shared_ptr<const Monomial> create( Variable::Arg _var, exponent _exp );
            
            std::shared_ptr<const Monomial> create( std::vector<std::pair<Variable, exponent>>&& _exponents, exponent _totalDegree );
            
            std::shared_ptr<const Monomial> create( const std::initializer_list<std::pair<Variable, exponent>>& _exponents, exponent _totalDegree );
            
            std::shared_ptr<const Monomial> create( std::vector<std::pair<Variable, exponent>>&& _exponents );
    };
} // end namespace carl

#endif