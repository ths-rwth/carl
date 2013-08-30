/** 
 * @file:   GBProcedure.h
 * @author: Sebastian Junges
 *
 * @since August 26, 2013
 */

#pragma once
#include "Ideal.h"
#include "Reductor.h"
#include "../core/logging.h"

namespace carl
{

template<typename Polynomial, template<typename, template<typename> class > class Procedure, template<typename> class  AddingPolynomialPolicy>
class GBProcedure : private Procedure<Polynomial, AddingPolynomialPolicy>
{
	private:
	Ideal<Polynomial>* mGb;
	std::list<Polynomial> mInputScheduled;
	std::vector<Polynomial> mOrigGenerators;
	std::vector<size_t> mOrigGeneratorsIndices;
	
	public:
	GBProcedure() : Procedure<Polynomial, AddingPolynomialPolicy>()
	{
		mGb = new Ideal<Polynomial>();
		Procedure<Polynomial, AddingPolynomialPolicy>::setIdeal(mGb);
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
	    mInputScheduled.push_back( p );
        mOrigGenerators.push_back( p );
	}
	
	
	const Ideal<Polynomial>& getIdeal() const
	{
		return *mGb;
	}
	
	void calculate()
	{
		LOGMSG_INFO("carl.gb.gbproc", "Calculate gb");
		if( mGb->getGenerators( ).size( ) + mInputScheduled.size( ) == 0 )
		{
			return;
		}

		
		// Use procedure
		Procedure<Polynomial, AddingPolynomialPolicy>::calculate(mInputScheduled);
		// remove the just added polynomials from the set of input polynomials
		mInputScheduled.clear( );
		mGb->removeEliminated( );
		LOGMSG_DEBUG("carl.gb.gbproc", "GB, before reduction: " << *mGb);
		// We have to update our indices list. but we do this just before returning because in the next step
		// we further shrink the size.
		reduceGB( );
	}
	
	private:

	void reduceGB( )
	{
		for( size_t i = 0; i < mGb->nrGenerators(); ++i)
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
		mGb->removeEliminated( );
		LOGMSG_DEBUG("carl.gb.gbproc", "GB Reduction, minimal GB: " << *mGb);
	    // Calculate reduction
	    // The number of polynomials will not change anymore!
	    std::vector<size_t> toBeReduced( mGb->getOrderedIndices( ) );
	
	    Ideal<Polynomial>* reduced = new Ideal<Polynomial>();
	    for( std::vector<size_t>::const_iterator index = toBeReduced.begin( ); index != toBeReduced.end( ); ++index )
	    {
	        Reductor<Polynomial, Polynomial, void> reduct( *reduced, mGb->getGenerator( *index ) );
	        Polynomial res = reduct.fullReduce( ).normalize();
			LOGMSG_DEBUG("carl.gb.gbproc", "GB Reduction, reduced " << mGb->getGenerator(*index) << " to " << res);
	        reduced->addGenerator( res );
	    }
		delete mGb;
	    mGb = reduced;
	}
};
}