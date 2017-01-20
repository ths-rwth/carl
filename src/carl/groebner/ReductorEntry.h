/** 
 * @file  ReductorEntry.h
 * @ingroup gb
 * @author Sebastian Junges
 */

#pragma once

#include "../core/Term.h"

#include <cassert>
#include <memory>

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
    using Coeff = typename Polynomial::CoeffType ;
    Polynomial mTail;
    Term<Coeff> mLead;
    Term<Coeff> mMultiple;

public:
    /**
     * Constructor with a factor and a polynomial
     * @param multiple
     * @param pol
     * Resulting polynomial = multiple * pol.
     */
    ReductorEntry(const Term<Coeff>&  multiple, const Polynomial& pol) :
    mTail(pol.tail()), mLead(multiple * pol.lterm()), mMultiple(multiple)
    {
		assert(!multiple.isZero());
    }

    /**
     * Constructor with implicit factor = 1
     * @param pol
     */
    explicit ReductorEntry(const Term<Coeff>& pol)
    : mTail(), mLead(pol), mMultiple(Term<Coeff>(Coeff(1)))
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
    const Term<Coeff>& getLead() const
    {
        return mLead;
    }

    /**
     * 
     * @return 
     */
    const Term<Coeff>& getMultiple() const
    {
        return mMultiple;
    }

    /**
     * Calculate p - lt(p).
     */
    void removeLeadingTerm()
    {
        assert(mTail.nrTerms() != 0);
		assert(!mMultiple.isZero());
		mLead = mMultiple * mTail.lterm();
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
			mLead = Term<Coeff>(newCoeff, mLead->monomial());
            return false;
        }
        else if(mTail.nrTerms() != 0)
        {
            removeLeadingTerm();
        }
        else
        {
            mLead = Term<Coeff>();
        }
        return true;
    }

    /**
     * 
     * @return true iff the polynomial equals zero
     */
    bool empty() const
    {
		assert(!mLead.isZero() || mTail.isZero());
        return mLead.isZero();
    }

    /**
     * Output the current polynomial
     * @param os
     */
    void print(std::ostream& os = std::cout)
    {
		assert(mMultiple);
        if(empty())
		{
			os << " ";
		}
		else
		{
			os << mLead << " +(" << mMultiple << " * " << mTail << ")";
		}
    }

    template<class C>
    friend std::ostream& operator <<(std::ostream& os, const ReductorEntry<C> rhs);

};

template<class C>
std::ostream& operator <<(std::ostream& os, const ReductorEntry<C> rhs)
{
	rhs.print(os);
	return os;
}
}
