/** 
 * @file:   IdealDSVector.h
 * @author: Sebastian Junges
 *
 * @since July 12, 2013
 */

#pragma once

#include <carl/poly/umvpoly/Term.h>
#include <carl/core/VariablePool.h>
#include "../DivisionLookupResult.h"
#include "PolynomialSorts.h"

#include <cassert>
#include <unordered_set>
#include <vector>

namespace carl
{

template<class Polynomial>
class IdealDatastructureVector
{
public:

    IdealDatastructureVector(const std::vector<Polynomial>& generators, const std::unordered_set<size_t>& eliminated, const sortByLeadingTerm<Polynomial>& order)
    : mGenerators(generators), mEliminated(eliminated), mOrder(order), mDivList()
    {
        
    }

    IdealDatastructureVector(const IdealDatastructureVector& id)
    : mGenerators(id.mGenerators), mEliminated(id.mEliminated), mOrder(id.mOrder), mDivList(id.mDivList)
    {

    }

    virtual ~IdealDatastructureVector() = default;

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
     * @param t
     * @return A divisionresult [divisor, factor]. 
     * 
     */
    DivisionLookupResult<Polynomial> getDivisor(const Term<typename Polynomial::CoeffType>& t) const
    {
        for(auto it = mDivList.begin(); it != mDivList.end();)
        {
            // First, we check whether the possible divisor is still in the ideal.
            // TODO As this mostly yields false, maybe, we should move it.
            if(mEliminated.count(*it) == 1)
            {
                it = mDivList.erase(it);
                continue;
            }
			
            Term<typename Polynomial::CoeffType> divres;
			if (t.divide(mGenerators[*it].lterm(), divres)) {
				//Division succeeded, so we have found a divisor;
				//To eliminate, we have to negate the factor.
				divres.negate();
                return DivisionLookupResult<Polynomial>(&mGenerators[*it], divres);
				///@todo delete divres ?
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
};


}
