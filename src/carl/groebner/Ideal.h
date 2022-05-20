/** 
 * @file   Ideal.h
 * @ingroup gb
 * @author Sebastian Junges
 */

#pragma once

#include "ideal-ds/IdealDSVector.h"
#include "ideal-ds/PolynomialSorts.h"

#include <carl/poly/umvpoly/MultivariatePolynomial.h>
#include <carl/poly/umvpoly/Term.h>
#include <unordered_set>

namespace carl
{

/**
 * @ingroup gb
 */
template <class Polynomial, template<class> class Datastructure = IdealDatastructureVector, int CacheSize = 0>
class Ideal
{
private:
    std::vector<Polynomial> mGenerators;

    sortByLeadingTerm<Polynomial> mTermOrder = sortByLeadingTerm<Polynomial>(mGenerators);

    std::unordered_set<size_t> mEliminated;
    Datastructure<Polynomial> mDivisorLookup = Datastructure<Polynomial>(mGenerators, mEliminated, mTermOrder);
public:

    Ideal() = default;

    Ideal(const Polynomial& p1, const Polynomial& p2):
        mTermOrder(mGenerators),
        mDivisorLookup(mGenerators, mEliminated, mTermOrder)
    {
        addGenerator(p1);
        addGenerator(p2);
    }

    virtual ~Ideal() = default;
	
	Ideal(const Ideal& rhs):
		mGenerators(rhs.mGenerators), 
	    mTermOrder(mGenerators), 
	    mEliminated(rhs.mEliminated), 
	    mDivisorLookup(mGenerators, mEliminated, mTermOrder)
	{
		removeEliminated();
		mDivisorLookup.reset();
	}

    Ideal& operator=(const Ideal& rhs)
    {
        if(this == &rhs) return *this;
        this->mGenerators.assign(rhs.mGenerators.begin(), rhs.mGenerators.end());
        this->mEliminated = rhs.mEliminated;
		this->mDivisorLookup = Datastructure<Polynomial>(mGenerators, mEliminated, mTermOrder);
        removeEliminated();
		mDivisorLookup.reset();
        return *this;
    }

    size_t addGenerator(const Polynomial& f)
    {
        size_t lastIndex = mGenerators.size();
        mGenerators.push_back(f);
        mDivisorLookup.addGenerator(lastIndex);
        return lastIndex;
    }
	
	DivisionLookupResult<Polynomial> getDivisor(const Term<typename Polynomial::CoeffType>& t) const
	{
		return mDivisorLookup.getDivisor(t);
	}

    

    bool isDividable(const Term<typename Polynomial::CoeffType>& m)
    {
        return mDivisorLookup.isDividable(m);
    }

	size_t nrGenerators() const
	{
		return mGenerators.size();
	}
	
    std::vector<Polynomial>& getGenerators()
    {
        return mGenerators;
    }

    const std::vector<Polynomial>& getGenerators() const
    {
        return mGenerators;
    }


    const Polynomial& getGenerator(size_t index) const
    {
        return mGenerators[index];
    }

    std::vector<size_t> getOrderedIndices()
    {
        std::vector<size_t> orderedIndices;
        for(size_t i = 0; i < mGenerators.size(); ++i)
        {
            orderedIndices.push_back(i);
        }

        std::sort(orderedIndices.begin(), orderedIndices.end(), mTermOrder);
        return orderedIndices;

    }

    void eliminateGenerator(size_t index)
    {
        mEliminated.insert(index);
    }

    /**
     * Invalidates indices
     * @return a vector with the new indices
     */
    void removeEliminated()
    {
        std::vector<Polynomial> tempGen;
        for(size_t it = 0; it != mGenerators.size(); ++it)
        {
            if(mEliminated.count(it) == 0)
            {
                tempGen.push_back(mGenerators[it]);
            }
        }
        tempGen.swap(mGenerators);
        mEliminated.clear();

    }
	
	void clear()
	{
		mGenerators.clear();
		mEliminated.clear();
		mDivisorLookup.reset();
	}


    bool is_constant() const
    {
        return mGenerators.size() == 1 && mGenerators.front().is_constant();
    }

    /**
     * Checks whether all polynomials occurring in this ideal are linear.
     * @return 
     */
    bool is_linear() const
    {
        for(auto it = mGenerators.begin(); it != mGenerators.end(); ++it)
        {
            if(!it->is_linear()) return false;
        }
        return true;
    }

    /**
     * Gather all variables occurring in this ideal.
     * @return 
     */
    std::set<unsigned> gatherVariables() const
    {
        std::set<unsigned> vars;
        for(auto it = mGenerators.begin(); it != mGenerators.end(); ++it)
        {
            it->gatherVariables(vars);
        }
        return vars;
    }

//    std::set<unsigned> getSuperfluousVariables() const
//    {
//        std::set<unsigned> superfluous;
//        for(auto it = mGenerators.begin(); it != mGenerators.end(); ++it)
//        {
//            //Variables occurring in polynomials x + y. 
//            if(it->nrOfTerms() == 2 && it->lterm().tdeg() == 1)
//            {
//                superfluous.insert(it->lterm().getSingleVariableNr());
//            }
//        }
//        return superfluous;
//    }

	friend std::ostream& operator<<(std::ostream& os, const Ideal& rhs )
	{
		os << "{";
		for(Polynomial p : rhs.mGenerators)
		{
			os << p << std::endl;
		}
		return os << "}";
	}
	
    void print(bool printOrigins=true, std::ostream& os = std::cout) const
    {
        for(typename std::vector<Polynomial>::const_iterator it = mGenerators.begin(); it != mGenerators.end(); ++it)
        {
            os << *it;
            if(printOrigins)
            {
                os << " [";
				it->getReasons().print();
                os << "]";
            }
            os << ";\n";
        }
    }
};


}
