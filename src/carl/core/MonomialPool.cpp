/**
 * @file MonomialPool.cpp
 * @author Florian Corzilius <corzilius@cs.rwth-aachen.de>
 */

#include "MonomialPool.h"

#include "../io/streamingOperators.h"

namespace carl
{	
	Monomial::Arg MonomialPool::add( Monomial::Content&& c, exponent totalDegree) {
		CARL_LOG_TRACE("carl.core.monomial", c << ", " << totalDegree);
		
		MONOMIAL_POOL_LOCK_GUARD
		
		underlying_set::insert_commit_data insert_data;
		auto res = mPool.insert_check(c, content_hash(), content_equal(), insert_data);
		if (!res.second) {
			return res.first->mWeakPtr.lock();
		} else {
			auto shared = std::shared_ptr<Monomial>(new Monomial(c, totalDegree));
			shared.get()->mId = mIDs.get();
			shared.get()->mWeakPtr = shared;
			mPool.insert_commit(*shared.get(), insert_data);
			return shared;
		}
	}
	
	Monomial::Arg MonomialPool::create( Variable _var, exponent _exp )
	{
		CARL_LOG_TRACE("carl.core.monomial", _var << ", " << _exp);
		return add(Monomial::Content(1, std::make_pair(_var, _exp)), _exp);
	}

	Monomial::Arg MonomialPool::create( std::vector<std::pair<Variable, exponent>>&& _exponents, exponent _totalDegree )
	{
		CARL_LOG_TRACE("carl.core.monomial", _exponents << ", " << _totalDegree);
		return add(std::move(_exponents), _totalDegree);
	}

	Monomial::Arg MonomialPool::create( const std::initializer_list<std::pair<Variable, exponent>>& _exponents )
	{
		//CARL_LOG_TRACE("carl.core.monomial", _exponents);
		return add(Monomial::Content(_exponents));
	}

	Monomial::Arg MonomialPool::create( std::vector<std::pair<Variable, exponent>>&& _exponents )
	{
		CARL_LOG_TRACE("carl.core.monomial", _exponents);
		return add(std::move(_exponents), 0);
	}
} // end namespace carl
