/**
 * @file MonomialPool.cpp
 * @author Florian Corzilius <corzilius@cs.rwth-aachen.de>
 */

#include "MonomialPool.h"

#include "../io/streamingOperators.h"

namespace carl
{
#ifdef PRUNE_MONOMIAL_POOL
	Monomial::Arg MonomialPool::add( MonomialPool::PoolEntry&& pe, exponent totalDegree) {
		MONOMIAL_POOL_LOCK_GUARD
		auto iter = mPool.insert(std::move(pe));
		Monomial::Arg res;
		if (iter.second) {
			if (iter.first->monomial.expired()) {
				if (totalDegree == 0) {
					res = Monomial::Arg(new Monomial(iter.first->hash, iter.first->content));
					iter.first->monomial = res;
				} else {
					res = Monomial::Arg(new Monomial(iter.first->hash, iter.first->content, totalDegree));
					iter.first->monomial = res;
				}
			} else {
				res = iter.first->monomial.lock();
			}
			res->mId = mIDs.get();
		} else {
			res = iter.first->monomial.lock();
		}
		return res;
	}

	Monomial::Arg MonomialPool::add( const Monomial::Arg& _monomial ) {
		assert(_monomial->id() == 0);
		PoolEntry pe(_monomial->hash(), _monomial->exponents(), _monomial);
		MONOMIAL_POOL_LOCK_GUARD
		auto iter = mPool.insert(pe);
		if (iter.second) {
			_monomial->mId = mIDs.get();
			return _monomial;
		} else {
			assert(_monomial == iter.first->monomial.lock());
			return iter.first->monomial.lock();
		}
	}
#else
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
			iter.first->monomial->mId = mIDs.get();
		}
		return iter.first->monomial;
	}

	Monomial::Arg MonomialPool::add( const Monomial::Arg& _monomial ) {
		assert(_monomial->id() == 0);
		return MonomialPool::add(std::move(PoolEntry(_monomial->hash(), _monomial->exponents(), _monomial)));
	}
#endif
	Monomial::Arg MonomialPool::add( Monomial::Content&& c, exponent totalDegree) {
		return MonomialPool::add(PoolEntry(std::move(c)), totalDegree);
	}
	
	Monomial::Arg MonomialPool::create()
	{
		return add(Monomial::Arg());
	}

	Monomial::Arg MonomialPool::create( Variable::Arg _var, exponent _exp )
	{
		return add(Monomial::Arg(new Monomial(_var, _exp)));
	}

	Monomial::Arg MonomialPool::create( std::vector<std::pair<Variable, exponent>>&& _exponents, exponent _totalDegree )
	{
		return add(std::move(_exponents), _totalDegree);
	}

	Monomial::Arg MonomialPool::create( const std::initializer_list<std::pair<Variable, exponent>>& _exponents )
	{
		return add(Monomial::Arg(new Monomial(_exponents)));
	}

	Monomial::Arg MonomialPool::create( std::vector<std::pair<Variable, exponent>>&& _exponents )
	{
		return add(std::move(_exponents));
	}
} // end namespace carl
