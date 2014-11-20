/**
 * @file Monomial_substitute.h
 * @ingroup multirp
 * @author Sebastian Junges
 */

#pragma once

#include "Monomial.h"
#include "Term.h"
#include "../numbers/numbers.h"

namespace carl
{
template<typename Coefficient>
Term<Coefficient>* Monomial::substitute(const std::map<Variable,Coefficient>& substitutions, Coefficient factor) const
{
	LOG_FUNC("carl.core.monomial", *this << ", " << substitutions << ", " << factor);
    std::vector<std::pair<Variable, exponent>> newExps;
    exponent tdegree = mTotalDegree;
	for (auto ve : mExponents) 
	{
		auto it = substitutions.find(ve.first);
		if(it == substitutions.end())
		{
			newExps.push_back(ve);
		}
		else
		{
			factor *= carl::pow(it->second, ve.second);
			tdegree -= ve.second;
		}
	}
	if(tdegree == 0)
	{
		assert(newExps.size() == 0);
		LOGMSG_TRACE("carl.core.monomial", "Result: " << factor);
		return new Term<Coefficient>(factor);
	}
	std::shared_ptr<const Monomial> result = createMonomial( std::move(newExps), tdegree );
	LOGMSG_TRACE("carl.core.monomial", "Result: " << factor << "*" << result);
	return new Term<Coefficient>(factor, result);	
}

template<typename Coefficient>
Term<Coefficient>* Monomial::substitute(const std::map<Variable,Term<Coefficient>>& substitutions, const Coefficient&  coeff) const
{
	LOG_FUNC("carl.core.monomial", *this << ", " << substitutions << ", " << coeff);
    std::vector<std::pair<Variable, exponent>> newExps;
    exponent tdegree = mTotalDegree;
	Term<Coefficient> factor(coeff);
	for(auto ve : mExponents) 
	{
		auto it = substitutions.find(ve.first);
		if(it == substitutions.end())
		{
			newExps.push_back(ve);
		}
		else
		{
			Term<Coefficient>* power = it->second.pow(ve.second);
			factor *= *power;
			delete power;
			tdegree -= ve.second;
		}
	}
	if(tdegree == 0)
	{
		assert(newExps.size() == 0);
		LOGMSG_TRACE("carl.core.monomial", "Result: " << coeff*factor.coeff());
		return new Term<Coefficient>(factor.coeff());
	}
	std::shared_ptr<const Monomial> result = createMonomial( std::move(newExps), tdegree );
	if(factor.monomial())
	{
		LOGMSG_TRACE("carl.core.monomial", "Result: " << coeff*factor.coeff() << "*" << (result * factor.monomial()));
		return new Term<Coefficient>(factor.coeff(), (result * factor.monomial()));	
	}
	else
	{
		LOGMSG_TRACE("carl.core.monomial", "Result: " << coeff*factor.coeff() << "*" << result);
		return new Term<Coefficient>(factor.coeff(), result);	
	}
}


}