/** 
 * @file   IntervalEvaluation.h
 *         Created on August 23, 2013, 1:12 PM
 * @author: Sebastian Junges
 *
 * 
 */


#pragma once

#include "DoubleInterval.h"

#include "../core/Monomial.h"
#include "../core/Term.h"
#include "../core/MultivariatePolynomial.h"

namespace carl
{
class IntervalEvaluation
{
public:
    static DoubleInterval evaluate(const Monomial& m, const std::map<Variable, DoubleInterval>&);
	template<typename Coeff>
	static DoubleInterval evaluate(const Term<Coeff>& t, const std::map<Variable, DoubleInterval>&);
	template<typename Coeff, typename Policy>
	static DoubleInterval evaluate(const MultivariatePolynomial<Coeff, Policy>& p, const std::map<Variable, DoubleInterval>&);
	
    
private:

};

inline DoubleInterval IntervalEvaluation::evaluate(const Monomial& m, const std::map<Variable, DoubleInterval>& map)
{
	DoubleInterval result(1);
	// TODO use iterator.
	for(size_t i = 0; i < m.nrVariables(); ++i)
	{
		// We expect every variable to be in the map.
		LOG_ASSERT(map.count(m[i].var) > (size_t)0, "Every variable is expected to be in the map.");
		result *= map.at(m[i].var).power(m[i].exp);
	}
	return result;
}

template<typename Coeff>
inline DoubleInterval IntervalEvaluation::evaluate(const Term<Coeff>& t, const std::map<Variable, DoubleInterval>& map)
{
	DoubleInterval result(t.coeff());
	if(t.monomial())
	{
		const Monomial& m = *t.monomial();
		// TODO use iterator.
		for(size_t i = 0; i < m.nrVariables(); ++i)
		{
			// We expect every variable to be in the map.
			assert(map.count(m[i].var) > 0);
			result *= map.at(m[i].var).power((int)m[i].exp);
		}
	}
	return result;
}

template<typename Coeff, typename Policy>
inline DoubleInterval IntervalEvaluation::evaluate(const MultivariatePolynomial<Coeff, Policy>& p, const std::map<Variable, DoubleInterval>& map)
{
    if(p.isZero())
    {
        return DoubleInterval(0);
    }
    else
    {
		DoubleInterval result(evaluate(*p[0], map)); 
		for(size_t i = 1; i < p.nrTerms(); ++i)
		{
			result += evaluate(*p[i], map);
		}
        return result;
    }
}


}


