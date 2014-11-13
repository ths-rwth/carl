/**
 * @file MonomialPool.cpp
 * @author Florian Corzilius <corzilius@cs.rwth-aachen.de>
 */

#include "MonomialPool.h"

#ifdef USE_MONOMIAL_POOL

namespace carl
{
#ifdef NEWPOOL
	Monomial::Arg MonomialPool::add( MonomialPool::PoolEntry&& pe, exponent totalDegree) {
		MONOMIAL_POOL_LOCK_GUARD
		auto iter = mPool.insert(pe);
		if (iter.second) {
			if (iter.first->monomial == nullptr) {
				if (totalDegree == 0) {
					iter.first->monomial.reset(new Monomial(iter.first->hash, iter.first->content));
				} else {
					iter.first->monomial.reset(new Monomial(iter.first->hash, iter.first->content, totalDegree));
				}
			}
			iter.first->monomial->mId = mIdAllocator;
			++mIdAllocator;
		}
		return iter.first->monomial;
	}

	Monomial::Arg MonomialPool::add( const Monomial::Arg& _monomial ) {
		assert(_monomial->id() == 0);
		return MonomialPool::add(std::move(PoolEntry(_monomial->hash(), _monomial->exponents(), _monomial)));
	}
	Monomial::Arg MonomialPool::add( Monomial::Content&& c, exponent totalDegree) {
		return MonomialPool::add(std::move(PoolEntry(Monomial::hashContent(c), std::move(c))), totalDegree);
	}
#else
	Monomial::Arg MonomialPool::add( const Monomial::Arg& _monomial )
	{
		MONOMIAL_POOL_LOCK_GUARD
		auto iterBoolPair = mPool.insert( _monomial );
		if( iterBoolPair.second )
		{
			_monomial->mId = mIdAllocator;  // id should be set here to avoid conflicts when multi-threading
			++mIdAllocator;
		}
		assert(_monomial == *iterBoolPair.first);
		return *iterBoolPair.first;
	}
#endif
	
	Monomial::Arg MonomialPool::create( Variable::Arg _var, exponent _exp )
	{
		return add(Monomial::Arg(new Monomial(_var, _exp)));
	}

	Monomial::Arg MonomialPool::create( std::vector<std::pair<Variable, exponent>>&& _exponents, exponent _totalDegree )
	{
		return add(std::move(_exponents), _totalDegree);
	}

	Monomial::Arg MonomialPool::create( const std::initializer_list<std::pair<Variable, exponent>>& _exponents, exponent _totalDegree )
	{
		return add(Monomial::Arg(new Monomial(_exponents, _totalDegree)));
	}

	Monomial::Arg MonomialPool::create( std::vector<std::pair<Variable, exponent>>&& _exponents )
	{
		return add(std::move(_exponents));
	}
} // end namespace carl

#endif