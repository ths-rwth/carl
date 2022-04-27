/** 
 * @file   GBProcedure.h
 * @ingroup gb
 * @author Sebastian Junges
 *
 */

#pragma once
#include "Ideal.h"
#include "Reductor.h"
#include <carl-logging/carl-logging.h>
#include "../util/BitVector.h"

namespace carl
{

template<typename Polynomial>
class AbstractGBProcedure 
{
	public:
	virtual ~AbstractGBProcedure() = default;
	virtual void addPolynomial(const Polynomial& p) = 0;
	virtual void reset()= 0;
	virtual void calculate()= 0;
	
	
	virtual std::list<std::pair<BitVector, BitVector> > reduceInput()= 0;
	virtual const Ideal<Polynomial>& getIdeal() const = 0;
};
	
/**
 * A general class for Groebner Basis calculation. 
 * It is parameterized not only in the type of polynomial to be used,
 *  but also in the concrete procedure to be used,
 *  and the way polynomials should be added to this procedure.
 * 
 * Please notice that this class is designed to support incremental calls. 
 * Therefore, it holds a queue with the polynomials which are added. 
 * Only upon calling the calculate method, these polynoimials are added to the actual groebner basis.
 * 
 * Moreover, we can 
 * @ingroup gb 
 */
template<typename Polynomial, template<typename, template<typename> class > class Procedure, template<typename> class AddingPolynomialPolicy>
class GBProcedure : private Procedure<Polynomial, AddingPolynomialPolicy>, public AbstractGBProcedure<Polynomial>
{
private:
	/// The ideal represented by the current elements of the Groebner basis.
	std::shared_ptr<Ideal<Polynomial>> mGb;
	/// The polynomials which are added during the next call for calculate.
	std::list<Polynomial> mInputScheduled;
	/// The input polynomials
	std::vector<Polynomial> mOrigGenerators;
	/// Indices of the input polynomials.
	std::vector<size_t> mOrigGeneratorsIndices;

public:

	GBProcedure():
		Procedure<Polynomial, AddingPolynomialPolicy>(),
		mGb(new Ideal<Polynomial>),
		mInputScheduled(),
		mOrigGenerators(),
		mOrigGeneratorsIndices()
	{
		Procedure<Polynomial, AddingPolynomialPolicy>::setIdeal(mGb);
	}
	
	
	GBProcedure(const GBProcedure& old):
	    Procedure<Polynomial, AddingPolynomialPolicy>(old),
		mGb(new Ideal<Polynomial>(*old.mGb)),
		mInputScheduled(old.mInputScheduled),
		mOrigGenerators(old.mOrigGenerators),
		mOrigGeneratorsIndices(old.mOrigGeneratorsIndices)
	{
		Procedure<Polynomial, AddingPolynomialPolicy>::setIdeal(mGb);
	}
	
	virtual ~GBProcedure() = default;

	GBProcedure& operator=(const GBProcedure& rhs)
	{
		if(this == &rhs) return *this;
		mGb.reset(new Ideal<Polynomial>(*rhs.mGb));
		mInputScheduled = rhs.mInputScheduled;
		mOrigGenerators = rhs.mOrigGenerators;
		mOrigGeneratorsIndices = rhs.mOrigGeneratorsIndices;
		Procedure<Polynomial, AddingPolynomialPolicy>::setIdeal(mGb);
        Procedure<Polynomial, AddingPolynomialPolicy>::setCriticalPairs(rhs.pCritPairs);
		return *this;
	}
	
	/**
	 * Check whether a polynomial is scheduled to be added to the Groebner basis.
     * @return whether the input is empty.
     */
	bool inputEmpty() const
	{
		return mInputScheduled.empty();
	}

	/**
	 * The number of polynomials which were originally added to the GB.
     * @return number of polynomials added.
     */
	size_t nrOrigGenerators() const
	{
		return mOrigGenerators.size();
	}

	/**
	 * Add a polynmomial which is added to the groebner basis during the next calculate call.
     * @param p The polynomial to be added.
     */
	void addPolynomial(const Polynomial& p)
	{
		mInputScheduled.push_back(p);
		mOrigGenerators.push_back(p);
	}

	/**
	 * Checks whether the current representants of the GB contain a constant polynomial.
     * @return 
     */
	bool basisIsConstant() const
	{
		return getIdeal().isConstant();
	}
	
	/**
	 * 
     * @return 
     */
	std::list<Polynomial> listBasisPolynomials() const
	{
		return std::list<Polynomial>(getBasisPolynomials().begin(), getBasisPolynomials().end());
	}
	
	/**
	 * 
     * @return 
     */
	const std::vector<Polynomial>& getBasisPolynomials() const
	{
		return getIdeal().getGenerators();
	}
	
	void printScheduledPolynomials(bool breakLines = true, bool printReasons = true, std::ostream& os = std::cout) const
	{
		for(const Polynomial& p : mInputScheduled)
		{
			os << p;
			if(printReasons)
			{
				os << "[";
				p.getReasons().print(os);
				os << "]";
			}
			if(breakLines)
			{
				os << std::endl;
			}
			else
			{
				os.flush();
			}
			
		}
	}
	
	
	/**
	 * Remove all polynomials from the Groebner basis.
     */
	void reset() 
	{
		mGb.reset(new Ideal<Polynomial>());
		Procedure<Polynomial, AddingPolynomialPolicy>::setIdeal(mGb);
	}
	
	/**
	 * Get the ideal which encodes the GB.
     * @return 
     */
	const Ideal<Polynomial>& getIdeal() const
	{
		return *mGb;
	}

	/**
	 * Calculate the Groebner basis of the current GB union the scheduled polynomials.
     */
	void calculate()
	{
		CARL_LOG_INFO("carl.gb.gbproc", "Calculate gb");
		if(mGb->getGenerators().size() + mInputScheduled.size() == 0)
		{
			return;
		}
		// Use procedure
		Procedure<Polynomial, AddingPolynomialPolicy>::calculate(mInputScheduled);
		// remove the just added polynomials from the set of input polynomials
		mInputScheduled.clear();
		mGb->removeEliminated();
		CARL_LOG_DEBUG("carl.gb.gbproc", "GB, before reduction: " << *mGb);
		// We have to update our indices list. but we do this just before returning because in the next step
		// we further shrink the size.
		reduceGB();
	}

	/**
	 * Reduce the input polynomials using the other input polynomials and the current Groebner basis.
     * @return 
     */
	std::list<std::pair<BitVector, BitVector> > reduceInput()
	{
		CARL_LOG_TRACE("carl.gb.gbproc", "Reduce input");
		std::vector<Polynomial> toBeReduced;
		//We only schedule "new" input for reduction
		for(typename std::list<Polynomial>::const_iterator it = mInputScheduled.begin(); it != mInputScheduled.end(); ++it)
		{
			toBeReduced.push_back(*it);
		}
		std::sort(toBeReduced.begin(), toBeReduced.end(), Polynomial::compareByLeadingTerm);

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
			if(isZero(res))
			{
				result.push_back(std::pair<BitVector, BitVector>(index->getReasons(), res.getReasons()));
			}
			else // ( !isZero(res) )
			{
				CARL_LOG_TRACE("carl.gb.gbproc", "Add input polynomial " << res.normalize());
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
			bool divisible = false;

			CARL_LOG_TRACE("carl.gb.gbproc", "Check " << mGb->getGenerator(i));
			for(size_t j = 0; !divisible && j != mGb->nrGenerators(); ++j)
			{
				if(j == i) continue;

				divisible = mGb->getGenerator(i).lmon()->divisible(mGb->getGenerator(j).lmon());
				CARL_LOG_TRACE("carl.gb.gbproc", "" << (divisible ? "" : "not ") << "divisible by " << mGb->getGenerator(j));
			}

			if(divisible)
			{
				CARL_LOG_TRACE("carl.gb.gbproc", "Eliminate " << mGb->getGenerator(i));
				mGb->eliminateGenerator(i);
			}
		}
		mGb->removeEliminated();
		CARL_LOG_DEBUG("carl.gb.gbproc", "GB Reduction, minimal GB: " << *mGb);
		// Calculate reduction
		// The number of polynomials will not change anymore!
		std::vector<size_t> toBeReduced(mGb->getOrderedIndices());

		std::shared_ptr<Ideal<Polynomial>> reduced(new Ideal<Polynomial>());
		for(std::vector<size_t>::const_iterator index = toBeReduced.begin(); index != toBeReduced.end(); ++index)
		{
			Reductor<Polynomial, Polynomial> reduct(*reduced, mGb->getGenerator(*index));
			Polynomial res = reduct.fullReduce();
            if(!isZero(res))
            {
                res.normalize();
                CARL_LOG_DEBUG("carl.gb.gbproc", "GB Reduction, reduced " << mGb->getGenerator(*index) << " to " << res);
                reduced->addGenerator(res);
            }
		}

		mGb = reduced;
        Procedure<Polynomial, AddingPolynomialPolicy>::setIdeal(mGb);
	}
};
}
