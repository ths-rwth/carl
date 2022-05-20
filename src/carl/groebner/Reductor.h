/** 
 * @file:   Reductor.h
 * @author: Sebastian Junges
 *
 * @since July 11, 2013
 */

#pragma once

#include "Ideal.h"
#include "ReductorEntry.h"
#include <carl-common/datastructures/Heap.h>
#include <carl-common/datastructures/BitVector.h>

namespace carl
{

/**
 * @ingroup gb
 *  Class with the settings for the reduction algorithm.
 */
template<class Polynomial>
class ReductorConfiguration
{
public:

	using EntryType = ReductorEntry<Polynomial>;
	using Entry = EntryType*;
	using CompareResult = carl::CompareResult;

	static CompareResult compare(Entry e1, Entry e2)
	{
		return Polynomial::OrderedBy::compare(e1->getLead(), e2->getLead());
	}

	static bool cmpLessThan(CompareResult res)
	{
		return res == CompareResult::LESS;
	}
	static const bool supportDeduplicationWhileOrdering = false;

	static bool cmpEqual(CompareResult res)
	{
		return res == CompareResult::EQUAL;
	}

	/**
	 * should only be called if the compare result was EQUAL
	 * eliminate duplicate leading monomials
	 * @param e1 upper entry
	 * @param e2 lower entry
	 * @return true if e1->lt is cancelled
	 */
	static bool deduplicate(Entry e1, Entry e2)
	{
		assert( *(e1->getLead()) ==  *(e2->getLead()));
		return e1->addCoefficient(e2->getLead().getCoeff());
	}

	static const bool fastIndex = true;
};

/**
 * A dedicated algorithm for calculating the remainder of a polynomial modulo a set of other polynomials. 
 * @ingroup gb
 */
template<typename InputPolynomial, typename PolynomialInIdeal, template <class> class Datastructure = carl::Heap, template <typename Polynomial> class Configuration = ReductorConfiguration>
class Reductor
{
	
protected:
	using Order = typename InputPolynomial::OrderedBy;
	using EntryType = typename Configuration<InputPolynomial>::EntryType;
	using Coeff = typename InputPolynomial::CoeffType;
private:
	const Ideal<PolynomialInIdeal>& mIdeal;
	Datastructure<Configuration<InputPolynomial>> mDatastruct;
	std::vector<Term<Coeff>> mRemainder;
	bool mReductionOccured;
	BitVector mReasons;
public:
	Reductor(const Ideal<PolynomialInIdeal>& ideal, const InputPolynomial& f) :
	mIdeal(ideal), mDatastruct(Configuration<InputPolynomial>()), mReductionOccured(false)
	{
		insert(f, Term<Coeff>(Coeff(1)));
		if(InputPolynomial::Policy::has_reasons)
		{
			mReasons = f.getReasons();
		}
				
	}

	Reductor(const Ideal<PolynomialInIdeal>& ideal, const Term<Coeff>& f) :
	mIdeal(ideal), mDatastruct(Configuration<InputPolynomial>())
	{
		insert(f);
	}

	virtual ~Reductor()	= default;

	/**
	 * The basic reduce routine on a priority queue.
	 * @return 
	 */
	bool reduce()
	{
		while(!mDatastruct.empty())
		{
			typename Configuration<InputPolynomial>::Entry entry;
			Term < Coeff > leadingTerm;
			// Find a leading term.
			do
			{
				// get actual leading term
				entry = mDatastruct.top();
				leadingTerm = entry->getLead();
				CARL_LOG_TRACE("carl.gb.reductor", "Intermediate leading term: " << leadingTerm);
				assert(!is_zero(leadingTerm));
				// update the data structure.
				// only insert non-empty polynomials.
				if(!updateDatastruct(entry)) break;
				typename Configuration<InputPolynomial>::Entry newentry = mDatastruct.top();
				while(entry != newentry && Term<Coeff>::monomialEqual(leadingTerm, (newentry->getLead())))
				{
					assert(!newentry->empty());
					leadingTerm = Term<Coeff>(leadingTerm.coeff() + newentry->getLead().coeff(), leadingTerm.monomial());
					if(!updateDatastruct(newentry)) break;
					newentry = mDatastruct.top();
				}
			}
			while(is_zero(leadingTerm) && !mDatastruct.empty());
			// Done finding leading term.
			//std::cout <<  "Leading term: " << *leadingTerm << std::endl;
			// We have found the leading term..
			if(is_zero(leadingTerm))
			{
				assert(mDatastruct.empty());
				//then the datastructure is empty, we are done.
				return true;
			}
			//std::cout <<  "Look for divisor.." << std::endl;
			
			//find a suitable reductor and the corresponding factor.
			DivisionLookupResult<PolynomialInIdeal> divres(mIdeal.getDivisor(leadingTerm));
			// check if the reduction succeeded.
			if(divres.success())
			{
				mReductionOccured = true;
				if(PolynomialInIdeal::Policy::has_reasons)
				{
					mReasons.calculateUnion(divres.mDivisor->getReasons());
				}
				if(divres.mDivisor->nr_terms() > 1)
				{
					insert(divres.mDivisor->tail(true), divres.mFactor);
				}
			}
			else
			{
				CARL_LOG_DEBUG("carl.gb.reductor", "Not reducible: " << leadingTerm);
				mRemainder.push_back(leadingTerm);
				return false;
			}
		}
		return true;
	}

	/**
	 * Gets the flag which indicates that a reduction has occurred  (p -> p' with p' != p)
	 * @return the value of the flag
	 */
	bool reductionOccured()
	{
		return mReductionOccured;
	}

	/**
	 * Uses the ideal to reduce a polynomial as far as possible.
	 * @return 
	 */
	InputPolynomial fullReduce()
	{
		//std::cout << "start full reduce" << std::endl;
		// TODO:
		// Do simple reductions first.
		while(!reduce())
		{
		//	std::cout << "done reducing" << std::endl;
			// no operation.
		}
		// TODO check whether this is sorted.
		InputPolynomial result(std::move(mRemainder), true, false);
		if(InputPolynomial::Policy::has_reasons)
		{
			result.setReasons(mReasons);
			mReasons.clear();
		}
		//std::cout << "done full reduce" << std::endl;
		return result;
				
	}
	
	
private:

 	
	
	
	/**
	 * A small routine which updates the underlying data structure for the polynomial which is reduced.
	 * @param entry
	 * @return 
	 */
	inline bool updateDatastruct(EntryType* entry)
	{
		assert(!mDatastruct.empty());
		if(is_zero(entry->getTail()))
		{
			mDatastruct.pop();
			delete entry;
			if(mDatastruct.empty()) return false;
		}
		else
		{
			entry->removeLeadingTerm();
			assert(!entry->empty());
			mDatastruct.decreaseTop(entry);
		}
		return true;
	}

	void insert(const InputPolynomial& g, const Term<Coeff>& fact)
	{
		if(!is_zero(g))
		{
			CARL_LOG_TRACE("carl.gb.reductor", "Insert polynomial: " << g << " * " << fact);
			mDatastruct.push(new EntryType(fact, g));
		}
	}

	void insert(const Term<Coeff>& g)
	{
		assert(g.getCoeff() != 0);
		mDatastruct.push(new EntryType(g));
	}


	//Origins mOrigins;
};


}
