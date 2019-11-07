#include "GCD_Monomial.h"

namespace carl {

Monomial::Arg Monomial::gcd(const Monomial::Arg& lhs, const Monomial::Arg& rhs) {
	if(!lhs && !rhs) return nullptr;
	if(!lhs) return rhs;
	if(!rhs) return lhs;

	CARL_LOG_FUNC("carl.core.monomial", lhs << ", " << rhs);
	assert(lhs->isConsistent());
	assert(rhs->isConsistent());

	Content newExps;
	uint expsum = 0;
	// Linear, as we expect small monomials.
	auto itright = rhs->mExponents.cbegin();
	auto leftEnd = lhs->mExponents.cend();
	auto rightEnd = rhs->mExponents.cend();
	for(auto itleft = lhs->mExponents.cbegin(); (itleft != leftEnd && itright != rightEnd);)
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