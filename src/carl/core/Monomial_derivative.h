/**
 * @file Monomial_derivative.h
 * @ingroup multirp
 */
#pragma once

#include "Monomial.h"
#include "Term.h"

namespace carl
{
template<typename Coefficient>
Term<Coefficient>* Monomial::derivative(Variable::Arg v) const
{
    // TODO code is very similar to divideBy(variable)...
    
     // Linear implementation, as we expect very small monomials.
    exponents_cIt it;
    if((it = std::find(mExponents.cbegin(), mExponents.cend(), v)) == mExponents.cend())
    {
        return new Term<Coefficient>();
    }
    else
    {
		// If the exponent is one, the variable does not occur in the new monomial.
        if(it->exp == 1)
        {
			// If it was the only variable, we get the one-term.
			if(mExponents.size() == 1) 
			{
				return new Term<Coefficient>((Coefficient)1);
			}

            Monomial* m = new Monomial();
            if(it != mExponents.begin())
            {
                m->mExponents.assign(mExponents.begin(), it);
            }
            m->mExponents.insert(m->mExponents.end(), it+1,mExponents.end());
            m->mTotalDegree = mTotalDegree - 1;
            return new Term<Coefficient>(1, m);
        }
        // We have to decrease the exponent of the variable by one.
        else
        {
            Monomial* m = new Monomial();
            m->mExponents.assign(mExponents.begin(), mExponents.end());
            m->mExponents[(unsigned)(it - mExponents.begin())].exp -= (unsigned)1;
            m->mTotalDegree = mTotalDegree - 1;
            return new Term<Coefficient>(it->exp, m);
        }
    }
}
}

