/**
 * @file MonomialPool.cpp
 * @author Florian Corzilius <corzilius@cs.rwth-aachen.de>
 */

#include "MonomialPool.h"

#ifdef USE_MONOMIAL_POOL

namespace carl
{
	Monomial::Arg MonomialPool::add( const Monomial::Arg& _monomial )
	{
		MONOMIAL_POOL_LOCK_GUARD
		auto iterBoolPair = mPool.insert( _monomial );
		if( iterBoolPair.second )
		{
			_monomial->mId = mIdAllocator;  // id should be set here to avoid conflicts when multi-threading
			++mIdAllocator;
		}
		return *iterBoolPair.first;   
	}
	
	Monomial::Arg MonomialPool::create( Variable::Arg _var, exponent _exp )
	{
		return add(Monomial::Arg(new Monomial(_var, _exp)));
	}

	Monomial::Arg MonomialPool::create( std::vector<std::pair<Variable, exponent>>&& _exponents, exponent _totalDegree )
	{
		return add(Monomial::Arg(new Monomial(std::move(_exponents), _totalDegree)));
	}

	Monomial::Arg MonomialPool::create( const std::initializer_list<std::pair<Variable, exponent>>& _exponents, exponent _totalDegree )
	{
		return add(Monomial::Arg(new Monomial(_exponents, _totalDegree)));
	}

	Monomial::Arg MonomialPool::create( std::vector<std::pair<Variable, exponent>>&& _exponents )
	{
		return add(Monomial::Arg(new Monomial(std::move(_exponents))));
	}
} // end namespace carl

#endif