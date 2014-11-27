/**
 * @file Monomial_derivative.h
 * @ingroup multirp
 */
#pragma once

#include "Monomial.h"
#include "MonomialPool.h"
#include "Term.h"

namespace carl
{
template<typename Coefficient>
Term<Coefficient> Monomial::derivative(Variable::Arg v) const
{
	LOG_FUNC("carl.core.monomial", *this << ", " << v);
    // TODO code is very similar to divideBy(variable)...
    
     // Linear implementation, as we expect very small monomials.
    auto it = std::find(mExponents.cbegin(), mExponents.cend(), v);
    if(it == mExponents.cend())
    {
		LOGMSG_TRACE("carl.core.monomial", "Result: 0");
        return std::move(Term<Coefficient>());
    }
    else
    {
		// If the exponent is one, the variable does not occur in the new monomial.
        if(it->second == 1)
        {
			// If it was the only variable, we get the one-term.
			if(mExponents.size() == 1) 
			{
				LOGMSG_TRACE("carl.core.monomial", "Result: 1");
				return std::move(Term<Coefficient>(carl::constant_one<Coefficient>::get()));
			}

            std::vector<std::pair<Variable, exponent>> newExps;
            if(it != mExponents.begin())
            {
                newExps.assign(mExponents.begin(), it);
            }
            newExps.insert(newExps.end(), it+1, mExponents.end());
			std::shared_ptr<const Monomial> result = createMonomial( std::move(newExps), mTotalDegree - 1 );
			LOGMSG_TRACE("carl.core.monomial", "Result: " << result);
            return std::move(Term<Coefficient>((Coefficient)1, result));
        }
        // We have to decrease the exponent of the variable by one.
        else
        {
            std::vector<std::pair<Variable, exponent>> newExps;
            newExps.assign(mExponents.begin(), mExponents.end());
            newExps[(unsigned)(it - mExponents.begin())].second -= (exponent)1;
			std::shared_ptr<const Monomial> result = createMonomial( std::move(newExps), mTotalDegree - 1 );
			LOGMSG_TRACE("carl.core.monomial", "Result: " << it->second << "*" << result);
            return std::move(Term<Coefficient>((Coefficient)it->second, result));
        }
    }
}
}

