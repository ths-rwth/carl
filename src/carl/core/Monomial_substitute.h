#pragma once

#include "Monomial.h"
#include "Term.h"
#include "numbers.h"

namespace carl
{
template<typename Coefficient>
Term<Coefficient>* Monomial::substitute(const std::map<Variable,Coefficient>& substitutions, Coefficient factor) const
{
	Monomial* m = new Monomial();
	m->mTotalDegree = mTotalDegree;
	for(const VarExpPair& ve : mExponents) 
	{
		typename std::map<Variable,Coefficient>::const_iterator it = substitutions.find(ve.var);
		if(it == substitutions.end())
		{
			m->mExponents.push_back(ve);
		}
		else
		{
			factor *= pow(it->second, ve.exp);
			m->mTotalDegree -= ve.exp;
		}
	}
	if(m->mTotalDegree == 0)
	{
		assert(m->mExponents.size() == 0);
		delete m;
		return new Term<Coefficient>(factor);
	}
	return new Term<Coefficient>(factor, std::shared_ptr<const Monomial>(m));	
}

}