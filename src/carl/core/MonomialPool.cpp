/**
 * @file MonomialPool.cpp
 * @author Florian Corzilius <corzilius@cs.rwth-aachen.de>
 */

#include "MonomialPool.h"

#include "../io/streamingOperators.h"

namespace carl
{
	Monomial::Arg MonomialPool::add( MonomialPool::PoolEntry&& pe, exponent totalDegree) {
		MONOMIAL_POOL_LOCK_GUARD
		auto iter = mPool.insert(std::move(pe));
		Monomial::Arg res;
		if (iter.second) {
			CARL_LOG_TRACE("carl.core.monomial", "Was newly added");
			if (iter.first->monomial.expired()) {
				CARL_LOG_TRACE("carl.core.monomial", "Weakptr is expired");
				if (totalDegree == 0) {
					res = Monomial::Arg(new Monomial(iter.first->hash, iter.first->content));
					iter.first->monomial = res;
				} else {
					res = Monomial::Arg(new Monomial(iter.first->hash, iter.first->content, totalDegree));
					iter.first->monomial = res;
				}
			} else {
				res = iter.first->monomial.lock();
				CARL_LOG_TRACE("carl.core.monomial", "Got existing weakptr as " << res);
			}
			res->mId = mIDs.get();
			CARL_LOG_TRACE("carl.core.monomial", "ID = " << res->mId);
		} else {
			res = iter.first->monomial.lock();
			CARL_LOG_TRACE("carl.core.monomial", "Was already there as " << res);
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
	
	Monomial::Arg MonomialPool::add( Monomial::Content&& c, exponent totalDegree) {
		CARL_LOG_TRACE("carl.core.monomial", c << ", " << totalDegree);
		return MonomialPool::add(PoolEntry(std::move(c)), totalDegree);
	}
	
	Monomial::Arg MonomialPool::create()
	{
		return add(Monomial::Arg());
	}

	Monomial::Arg MonomialPool::create( Variable _var, exponent _exp )
	{
		CARL_LOG_TRACE("carl.core.monomial", _var << ", " << _exp);
		return add(Monomial::Arg(new Monomial(_var, _exp)));
	}

	Monomial::Arg MonomialPool::create( std::vector<std::pair<Variable, exponent>>&& _exponents, exponent _totalDegree )
	{
		CARL_LOG_TRACE("carl.core.monomial", _exponents << ", " << _totalDegree);
		return add(std::move(_exponents), _totalDegree);
	}

	Monomial::Arg MonomialPool::create( const std::initializer_list<std::pair<Variable, exponent>>& _exponents )
	{
		//CARL_LOG_TRACE("carl.core.monomial", _exponents);
		return add(Monomial::Arg(new Monomial(_exponents)));
	}

	Monomial::Arg MonomialPool::create( std::vector<std::pair<Variable, exponent>>&& _exponents )
	{
		CARL_LOG_TRACE("carl.core.monomial", _exponents);
		return add(std::move(_exponents));
	}
} // end namespace carl
