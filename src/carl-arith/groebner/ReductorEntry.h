/** 
 * @file  ReductorEntry.h
 * @ingroup gb
 * @author Sebastian Junges
 */

#pragma once

#include <carl-arith/poly/umvpoly/Term.h>

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
		assert(!carl::is_zero(multiple));
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
        assert(mTail.nr_terms() != 0);
		assert(!carl::is_zero(mMultiple));
		mLead = mMultiple * mTail.lterm();
        mTail.strip_lterm();
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
        else if(mTail.nr_terms() != 0)
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
		assert(!carl::is_zero(mLead) || carl::is_zero(mTail));
        return carl::is_zero(mLead);
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
