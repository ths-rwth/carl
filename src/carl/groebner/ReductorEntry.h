/** 
 * @file  ReductorEntry.h
 * @ingroup gb
 * @author Sebastian Junges
 */

#pragma once

#include <memory>
#include <cassert>

#include "../core/Term.h"

namespace carl
{

/**
 * An entry in the reduction polynomial.
 * The class decodes a polynomial given by
 * mLead + mMultiple * mTail.
 * @ingroup gb
 */
template <class Polynomial>
class ReductorEntry
{
protected:
    typedef typename Polynomial::CoeffType   Coeff;
    Polynomial                         mTail;
    std::shared_ptr<const Term<Coeff>>    mLead;
    std::shared_ptr<const Term<Coeff>>    mMultiple;

public:
    /**
     * Constructor with a factor and a polynomial
     * @param multiple
     * @param pol
     * Resulting polynomial = multiple * pol.
     */
    ReductorEntry(std::shared_ptr<const Term<Coeff>>  multiple, const Polynomial& pol) :
    mTail(pol.tail()), mLead(std::make_shared<const Term<Coeff>>(*multiple * *pol.lterm())), mMultiple(multiple)
    {
    }

    /**
     * Constructor with implicit factor = 1
     * @param pol
     */
    ReductorEntry(const Term<Coeff>& pol)
    : mTail(), mLead(pol), mMultiple(std::make_shared<const Term<Coeff>>(Coeff(1)))
    {
    }

    /**
     * @return The tail of the polynomial, not multiplied by the correct factor!
     */
    const Polynomial& getTail() const
    {
        return mTail;
    }

    /**
     * 
     * @return 
     */
    std::shared_ptr<const Term<Coeff>> getLead() const
    {
        return mLead;
    }

    /**
     * 
     * @return 
     */
    std::shared_ptr<const Term<Coeff>> getMultiple() const
    {
        return mMultiple;
    }

    /**
     * Calculate p - lt(p).
     */
    void removeLeadingTerm()
    {
        assert(mTail.nrTerms() != 0);
		mLead = std::make_shared<const Term<Coeff>>(*mMultiple * *mTail.lterm());
        mTail.stripLT();
    }

    /**
     * 
     * @param coeffToBeAdded
     * @return 
     */
    bool addCoefficient(const Coeff& coeffToBeAdded)
    {
        assert(!empty());
		Coeff newCoeff = mLead->coeff() + coeffToBeAdded;
		
        if(newCoeff != 0)
        {
			mLead = std::make_shared<const Term<Coeff>>(newCoeff, mLead->monomial());
            return false;
        }
        else if(mTail.nrTerms() != 0)
        {
            removeLeadingTerm();
        }
        else
        {
            mLead.reset();
        }
        return true;
    }

    /**
     * 
     * @return true iff the polynomial equals zero
     */
    bool empty() const
    {
        return !mLead;
    }

    /**
     * Output the current polynomial
     * @param os
     */
    void print(std::ostream& os = std::cout)
    {
        if(empty()) os << " ";
        os << *mLead << " +(" << *mMultiple << " * " << mTail << ")";
    }

    template<class C>
    friend std::ostream& operator <<(std::ostream& os, const ReductorEntry<C> rhs);

};

template<class C>
std::ostream& operator <<(std::ostream& os, const ReductorEntry<C> rhs)
{
    return(os << rhs.mLead << " + " << rhs.mMultiple << " * " << rhs.mTail);
}
}

