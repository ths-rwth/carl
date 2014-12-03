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
Term<Coefficient> Monomial::substitute(const std::map<Variable,Coefficient>& substitutions, Coefficient factor) const
{
	CARL_LOG_FUNC("carl.core.monomial", *this << ", " << substitutions << ", " << factor);
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
		CARL_LOG_TRACE("carl.core.monomial", "Result: " << factor);
		return std::move(Term<Coefficient>(factor));
	}
	std::shared_ptr<const Monomial> result = createMonomial( std::move(newExps), tdegree );
	CARL_LOG_TRACE("carl.core.monomial", "Result: " << factor << "*" << result);
	return std::move(Term<Coefficient>(factor, result));
}

template<typename Coefficient>
Term<Coefficient> Monomial::substitute(const std::map<Variable,Term<Coefficient>>& substitutions, const Coefficient&  coeff) const
{
	CARL_LOG_FUNC("carl.core.monomial", *this << ", " << substitutions << ", " << coeff);
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
			factor *= it->second.pow(ve.second);
			tdegree -= ve.second;
		}
	}
	if(tdegree == 0)
	{
		assert(newExps.size() == 0);
		CARL_LOG_TRACE("carl.core.monomial", "Result: " << coeff*factor.coeff());
		return std::move(Term<Coefficient>(factor.coeff()));
	}
	std::shared_ptr<const Monomial> result = createMonomial( std::move(newExps), tdegree );
	if(factor.monomial())
	{
		CARL_LOG_TRACE("carl.core.monomial", "Result: " << coeff*factor.coeff() << "*" << (result * factor.monomial()));
		return std::move(Term<Coefficient>(factor.coeff(), (result * factor.monomial())));
	}
	else
	{
		CARL_LOG_TRACE("carl.core.monomial", "Result: " << coeff*factor.coeff() << "*" << result);
		return std::move(Term<Coefficient>(factor.coeff(), result));
	}
}


}