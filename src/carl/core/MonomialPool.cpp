/**
 * @file MonomialPool.cpp
 * @author Florian Corzilius <corzilius@cs.rwth-aachen.de>
 */


#include "MonomialPool.h"

#ifdef USE_MONOMIAL_POOL

namespace carl
{
    std::shared_ptr<const Monomial> MonomialPool::add( std::shared_ptr<const Monomial> _monomial )
    {
        MONOMIAL_POOL_LOCK_GUARD
        auto iterBoolPair = mPool.insert( _monomial );
        if( iterBoolPair.second )
        {
            _monomial->mOrder = mIdAllocator;  // id should be set here to avoid conflicts when multi-threading
            ++mIdAllocator;
        }
        return *iterBoolPair.first;   
    }
    
    std::shared_ptr<const Monomial> MonomialPool::create( Variable::Arg _var, exponent _exp )
    {
        std::shared_ptr<const Monomial> m = std::make_shared<Monomial>( Monomial( _var, _exp ) );
        return add( m );
    }

    std::shared_ptr<const Monomial> MonomialPool::create( std::vector<std::pair<Variable, exponent>>&& _exponents, exponent _totalDegree )
    {
        std::shared_ptr<const Monomial> m = std::make_shared<Monomial>( Monomial( std::move( _exponents ), _totalDegree ) );
        return add( m );
    }

    std::shared_ptr<const Monomial> MonomialPool::create( const std::initializer_list<std::pair<Variable, exponent>>& _exponents, exponent _totalDegree )
    {
        std::shared_ptr<const Monomial> m = std::make_shared<Monomial>( Monomial( _exponents, _totalDegree ) );
        return add( m );
    }

    std::shared_ptr<const Monomial> MonomialPool::create( std::vector<std::pair<Variable, exponent>>&& _exponents )
    {
        std::shared_ptr<const Monomial> m = std::make_shared<Monomial>( Monomial( std::move( _exponents ) ) );
        return add( m );
    }
} // end namespace carl

#endif