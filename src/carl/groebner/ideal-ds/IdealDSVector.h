/** 
 * @file:   IdealDSVector.h
 * @author: Sebastian Junges
 *
 * @since July 12, 2013
 */

#pragma once

#include <vector>
#include <unordered_set>

#include "PolynomialSorts.h"
#include "../../core/Term.h"
#include "../../core/VariablePool.h"
#include "../DivisionLookupResult.h"

namespace carl
{

template<class Polynomial>
class IdealDatastructureVector
{
public:

    IdealDatastructureVector(const std::vector<Polynomial>& generators, const std::unordered_set<size_t>& eliminated, const sortByLeadingTerm<Polynomial>& order)
    : mGenerators(generators), mEliminated(eliminated), mOrder(order)
    {
        mDivLists.resize(VariablePool::NrVariables(), std::vector<size_t> ());
    }

    IdealDatastructureVector(const IdealDatastructureVector& id)
    : mGenerators(id.mGenerators), mEliminated(id.mEliminated), mOrder(id.mOrder), mDivList(id.mDivList), mDivLists(id.mDivLists)
    {

    }

    virtual ~IdealDatastructureVector()
    {
    }

    /**
     * Should be called whenever an generator is added
     * @param fIndex
     */
    void addGenerator(size_t fIndex) const
    {
        mDivList.push_back(fIndex);
        std::sort(mDivList.begin(), mDivList.end(), mOrder);
    }

	
    /**
     * 
     * @param f
     * @return A divisionresult [divisor, factor]. 
     * 
     */
    DivisionLookupResult<Polynomial> getDivisor(const Term<typename Polynomial::CoeffType>& t) const
    {
        typename std::vector<size_t>::iterator end = mDivList.end();
        for(typename std::vector<size_t>::iterator it = mDivList.begin(); it != end;)
        {
            // First, we check whether the possible divisor is still in the ideal.
            // TODO As this mostly yields false, maybe, we should move it.
            if(mEliminated.count(*it) == 1)
            {
                mDivList.erase(it);
                continue;
            }
			
            Term<typename Polynomial::CoeffType>* divres = t.dividedBy(*mGenerators[*it].lterm());
			
            //Division succeeded, so we have found a divisor;
            //To eliminate, we have to negate the factor.
            if(divres != nullptr)
            {
				divres->negate();
                return DivisionLookupResult<Polynomial>(&mGenerators[*it], divres);
            }
            ++it;
        }
        //no divisor found
        return DivisionLookupResult<Polynomial>();
    }

    /**
     * Should be called if the generator set is reset.
     */
    void reset()
    {
        mDivList.clear();
        for(size_t i = 0; i < mGenerators.size(); ++i)
        {
            mDivList.push_back(i);
        }
        std::sort(mDivList.begin(), mDivList.end(), mOrder);
    }

private:
    /// A reference to the generators in the ideal
    const std::vector<Polynomial>& mGenerators;
    /// A reference to the indices of eliminated generators
    const std::unordered_set<size_t>& mEliminated;
    /// A object which orders the generators according their leading terms, given their indices
    const sortByLeadingTerm<Polynomial>& mOrder;
    // has to be mutable so we can remove zeroes found while looking for a divisor.
    // This is not strictly necessary, but may speed up the computation..
    mutable std::vector<size_t> mDivList;
    std::vector< std::vector<size_t> > mDivLists;
};


}
