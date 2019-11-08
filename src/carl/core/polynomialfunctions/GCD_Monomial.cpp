#include "GCD_Monomial.h"

#include "../MonomialPool.h"

namespace carl {

Monomial::Arg gcd(const Monomial::Arg& lhs, const Monomial::Arg& rhs) {
	if(!lhs && !rhs) return nullptr;
	if(!lhs) return rhs;
	if(!rhs) return lhs;

	CARL_LOG_FUNC("carl.core.monomial", lhs << ", " << rhs);
	assert(lhs->isConsistent());
	assert(rhs->isConsistent());

	Monomial::Content newExps;
	uint expsum = 0;
	// Linear, as we expect small monomials.
	auto itright = rhs->begin();
	auto leftEnd = lhs->end();
	auto rightEnd = rhs->end();
	for(auto itleft = lhs->begin(); (itleft != leftEnd && itright != rightEnd);)
	{
		// Variable is present in both monomials.
		if(itleft->first == itright->first)
		{
			uint newExp = std::min(itleft->second, itright->second);
			newExps.emplace_back(itleft->first, newExp);
			expsum += newExp;
			++itright;
			++itleft;
		}
		else if(itleft->first < itright->first) 
		{
			++itleft;
		}
		else
		{
			assert(itleft->first > itright->first);
			++itright;
		}
	}
		// Insert remaining part
	std::shared_ptr<const Monomial> result;
	if (!newExps.empty()) {
		result = createMonomial(std::move(newExps), expsum);
	}
	CARL_LOG_TRACE("carl.core.monomial", "Result: " << result);
	return result;
}

}