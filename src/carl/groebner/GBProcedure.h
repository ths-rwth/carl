/** 
 * @file   GBProcedure.h
 * @ingroup gb
 * @author Sebastian Junges
 *
 */

#pragma once
#include "Ideal.h"
#include "Reductor.h"
#include "../core/logging.h"
#include "../util/BitVector.h"

namespace carl
{

/**
 * A general class for Groebner Basis calculation
 * @ingroup gb 
 */
template<typename Polynomial, template<typename, template<typename> class > class Procedure, template<typename> class AddingPolynomialPolicy>
class GBProcedure : private Procedure<Polynomial, AddingPolynomialPolicy>
{
private:
	std::shared_ptr<Ideal<Polynomial>> mGb;
	std::list<Polynomial> mInputScheduled;
	std::vector<Polynomial> mOrigGenerators;
	std::vector<size_t> mOrigGeneratorsIndices;

public:

	GBProcedure() : Procedure<Polynomial, AddingPolynomialPolicy>(),
	mGb(new Ideal<Polynomial>)
	{
		Procedure<Polynomial, AddingPolynomialPolicy>::setIdeal(mGb);
	}
	
	
	GBProcedure(const GBProcedure& old)
	:
	mGb(new Ideal<Polynomial>(*mGb)),
	mInputScheduled(old.mInputScheduled),
	mOrigGenerators(old.mOrigGenerators),
	mOrigGeneratorsIndices(old.mOrigGeneratorsIndices)
	{
		
	}
	
	virtual ~GBProcedure() {
		
	}

	GBProcedure& operator=(const GBProcedure& rhs)
	{
		if(this == &rhs) return *this;
		mGb = rhs.mGb;
		mInputScheduled = rhs.mInputScheduled;
		mOrigGenerators = rhs.mOrigGenerators;
		mOrigGeneratorsIndices = rhs.mOrigGeneratorsIndices;
		return *this;
	}
	
	bool inputEmpty() const
	{
		return mInputScheduled.empty();
	}

	size_t nrOrigGenerators() const
	{
		return mOrigGenerators.size();
	}

	void addPolynomial(const Polynomial& p)
	{
		mInputScheduled.push_back(p);
		mOrigGenerators.push_back(p);
	}

	bool basisIsConstant() const
	{
		return getIdeal().isConstant();
	}
	
	std::list<Polynomial> listBasisPolynomials() const
	{
		return std::list<Polynomial>(getBasisPolynomials().begin(), getBasisPolynomials().end());
	}
	
	const std::vector<Polynomial>& getBasisPolynomials() const
	{
		return getIdeal().getGenerators();
	}
	
	void reset() 
	{
		mGb.reset(new Ideal<Polynomial>());
		Procedure<Polynomial, AddingPolynomialPolicy>::setIdeal(mGb);
	}
	
	const Ideal<Polynomial>& getIdeal() const
	{
		return *mGb;
	}

	void calculate()
	{
		LOGMSG_INFO("carl.gb.gbproc", "Calculate gb");
		if(mGb->getGenerators().size() + mInputScheduled.size() == 0)
		{
			return;
		}
		// Use procedure
		Procedure<Polynomial, AddingPolynomialPolicy>::calculate(mInputScheduled);
		// remove the just added polynomials from the set of input polynomials
		mInputScheduled.clear();
		mGb->removeEliminated();
		LOGMSG_DEBUG("carl.gb.gbproc", "GB, before reduction: " << *mGb);
		// We have to update our indices list. but we do this just before returning because in the next step
		// we further shrink the size.
		reduceGB();
	}

	std::list<std::pair<BitVector, BitVector> > reduceInput()
	{
		LOGMSG_TRACE("carl.gb.gbproc", "Reduce input");
		std::vector<Polynomial> toBeReduced;
		//We only schedule "new" input for reduction
		for(typename std::list<Polynomial>::const_iterator it = mInputScheduled.begin(); it != mInputScheduled.end(); ++it)
		{
			toBeReduced.push_back(*it);
		}
		sort(toBeReduced.begin(), toBeReduced.end(), Polynomial::compareByLeadingTerm);

		// We will continue to generate new scheduled polynomials, which make the scheduled polynomials from before superfluous.
		mInputScheduled.clear();

		// We reduce with the whole ideal, that is, 
		// we also use polynomials to be added to reduce other polynomials which are about to be added.
		Ideal<Polynomial> reduced(*mGb);

		// If we are going to trace the origns, we need to trace them here as well.
		// Moreover, if we want to return deductions, 
		// that is done by return the bitvector together with the bitvector which represents the original.
		std::list<std::pair<BitVector, BitVector> > result;

		for(typename std::vector<Polynomial>::const_iterator index = toBeReduced.begin(); index != toBeReduced.end(); ++index)
		{
			Reductor<Polynomial, Polynomial> reduct(reduced, *index);
			Polynomial res = reduct.fullReduce();
			if(res.isZero())
			{
				//result.push_back(std::pair<BitVector, BitVector>(index->getOrigins().getBitVector(), res.getOrigins().getBitVector()));
			}
			else // ( !res.isZero( ) )
			{
				LOGMSG_TRACE("carl.gb.gbproc", "Add input polynomial " << res.normalize());
				// Use the polynomial to reduce other input polynomials.
				reduced.addGenerator(res.normalize());
				// And restore it as scheduled polynomial.
				mInputScheduled.push_back(res.normalize());
			}
		}

		return result;
	}
private:

	void reduceGB()
	{
		for(size_t i = 0; i < mGb->nrGenerators(); ++i)
		{
			bool dividable = false;

			LOGMSG_TRACE("carl.gb.gbproc", "Check " << mGb->getGenerator(i));
			for(size_t j = 0; !dividable && j != mGb->nrGenerators(); ++j)
			{
				if(j == i) continue;

				dividable = mGb->getGenerator(i).lmon()->dividableBy(*mGb->getGenerator(j).lmon());
				LOGMSG_TRACE("carl.gb.gbproc", "" << (dividable ? "" : "not ") << "dividable by " << mGb->getGenerator(j));
			}

			if(dividable)
			{
				LOGMSG_TRACE("carl.gb.gbproc", "Eliminate " << mGb->getGenerator(i));
				mGb->eliminateGenerator(i);
			}
		}
		mGb->removeEliminated();
		LOGMSG_DEBUG("carl.gb.gbproc", "GB Reduction, minimal GB: " << *mGb);
		// Calculate reduction
		// The number of polynomials will not change anymore!
		std::vector<size_t> toBeReduced(mGb->getOrderedIndices());

		std::shared_ptr<Ideal<Polynomial>> reduced(new Ideal<Polynomial>());
		for(std::vector<size_t>::const_iterator index = toBeReduced.begin(); index != toBeReduced.end(); ++index)
		{
			Reductor<Polynomial, Polynomial> reduct(*reduced, mGb->getGenerator(*index));
			Polynomial res = reduct.fullReduce().normalize();
			LOGMSG_DEBUG("carl.gb.gbproc", "GB Reduction, reduced " << mGb->getGenerator(*index) << " to " << res);
			reduced->addGenerator(res);
		}

		mGb = reduced;
	}
};
}