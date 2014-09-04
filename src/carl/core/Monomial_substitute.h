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
	Monomial* m = new Monomial();
	m->mTotalDegree = mTotalDegree;
	for (auto ve : mExponents) 
	{
		auto it = substitutions.find(ve.first);
		if(it == substitutions.end())
		{
			m->mExponents.push_back(ve);
		}
		else
		{
			factor *= carl::pow(it->second, ve.second);
			m->mTotalDegree -= ve.second;
		}
	}
	if(m->mTotalDegree == 0)
	{
		assert(m->mExponents.size() == 0);
		delete m;
		LOGMSG_TRACE("carl.core.monomial", "Result: " << factor);
		return new Term<Coefficient>(factor);
	}
	LOGMSG_TRACE("carl.core.monomial", "Result: " << factor << "*" << *m);
	return new Term<Coefficient>(factor, std::shared_ptr<const Monomial>(m));	
}

template<typename Coefficient>
Term<Coefficient>* Monomial::substitute(const std::map<Variable,Term<Coefficient>>& substitutions, const Coefficient&  coeff) const
{
	LOG_FUNC("carl.core.monomial", *this << ", " << substitutions << ", " << coeff);
	Monomial m;
	m.mTotalDegree = mTotalDegree;
	Term<Coefficient> factor(coeff);
	for(auto ve : mExponents) 
	{
		auto it = substitutions.find(ve.first);
		if(it == substitutions.end())
		{
			m.mExponents.push_back(ve);
		}
		else
		{
			Term<Coefficient>* power = it->second.pow(ve.second);
			factor *= *power;
			delete power;
			m.mTotalDegree -= ve.second;
		}
	}
	if(m.mTotalDegree == 0)
	{
		assert(m.mExponents.size() == 0);
		LOGMSG_TRACE("carl.core.monomial", "Result: " << coeff*factor.coeff());
		return new Term<Coefficient>(factor.coeff());
	}
	
	if(factor.monomial())
	{
		LOGMSG_TRACE("carl.core.monomial", "Result: " << coeff*factor.coeff() << "*" << m* *factor.monomial());
		return new Term<Coefficient>(factor.coeff(),m * *factor.monomial());	
	}
	else
	{
		LOGMSG_TRACE("carl.core.monomial", "Result: " << coeff*factor.coeff() << "*" << m);
		return new Term<Coefficient>(factor.coeff(),m);	
	}
}


}