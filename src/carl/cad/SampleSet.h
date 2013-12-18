/* 
 * File:   SampleSet.h
 * Author: Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include <list>
#include <unordered_map>
#include <utility>

#include "../core/RealAlgebraicNumber.h"
#include "../core/RealAlgebraicNumberIR.h"
#include "../core/RealAlgebraicNumberNR.h"

namespace carl {
namespace cad {

template<typename Number>
class SampleSet {
	
private:
	std::list<RealAlgebraicNumber<Number>*> samples;

	/// Queue containing all samples in the order of their insertion
	std::list<RealAlgebraicNumber<Number>*> queue;
	
	/// Queue containing all samples in numerical representation in the order of their insertion
	std::list<RealAlgebraicNumberNR<Number>*> NRqueue;
	
	/// Queue containing all samples in interval representation in the order of their insertion
	std::list<RealAlgebraicNumberIR<Number>*> IRqueue;

	/// Queue containing all non-root samples in the order of their insertion
	std::list<RealAlgebraicNumber<Number>*> nonRootQueue;
	
	/// Queue containing all root samples in the order of their insertion
	std::list<RealAlgebraicNumber<Number>*> rootQueue;
	
public:
	typedef typename std::list<RealAlgebraicNumber<Number>*>::iterator iterator;
	typedef typename std::list<RealAlgebraicNumberIR<Number>*>::iterator iteratorIR;
	typedef typename std::list<RealAlgebraicNumberNR<Number>*>::iterator iteratorNR;
	typedef std::unordered_map<RealAlgebraicNumberIR<Number>*, RealAlgebraicNumberNR<Number>*> SampleSimplification;
	
	/**
	 * Inserts an element into the sorted list at the correct position according to the order.
	 * @param r RealAlgebraicNumberPtr to be inserted
	 * @return a pair, with its member <code>pair::first</code> set to an iterator pointing to either the newly inserted element or to the element that already had its same value in the set.
	 * The <code>pair::second</code> element in the pair is set to <code>true</code> if a new element was inserted or <code>false</code> if an element with the same value existed.
	 * If a numeric representation is inserted, the method replaces a possibly existing interval representation.
	 * @complexity at most logarithmic in the size of the list
	 */
	std::pair<iterator, bool> insert(const RealAlgebraicNumber<Number>* r);
	
	/**
	 * Inserts a range of elements into the sorted list.
	 * @param first defining the first position to be inserted
	 * @param last defining the last position up to which all elements, but *last, are inserted
	 * @complexity at most logarithmic in the size of the list
	 */
	template<class InputIterator>
	void insert(InputIterator first, InputIterator last) {
		for (InputIterator i = first; i != last; i++) {
			this->insert(*i);
		}
	}
	
	/**
	 * Inserts another sampleList.
	 * @param l other sampleList
	 * @complexity at most logarithmic in the size of this list and linear in the size of l
	 */
	void insert(const SampleSet& l) {
		this->insert(l.samples.begin(), l.samples.end());
	}
	
	/**
	 * Safely remove the element at position.
	 * @param position
	 * @return the iterator to the next position in the container
	 */
	SampleSet::iterator remove(SampleSet::iterator position);
	
	/**
	 * Determines the next sample in the order of insertion with no particular preference.
	 *
	 * @return next sample in the order of insertion
	 */
	inline const RealAlgebraicNumber<Number>* next();
	
	/**
	 * Determines the next sample in the order of insertion, preferring the numerically represented samples.
	 *
	 * Before returning an interval-represented sample, the method tries to simplify the sample.
	 *
	 * @return next sample in the order of insertion, preferring the numerically represented samples
	 */
	inline const RealAlgebraicNumber<Number>* nextNR();
	
	/**
	 * Determines the next sample in the order of insertion, preferring the non-root samples.
	 *
	 * @return next sample in the order of insertion, preferring the non-root samples
	 */
	inline const RealAlgebraicNumber<Number>* nextNonRoot();
	
	/**
	 * Determines the next sample in the order of insertion, preferring the non-root samples.
	 *
	 * @return next sample in the order of insertion, preferring the non-root samples
	 */
	inline const RealAlgebraicNumber<Number>* nextRoot();
	
	/**
	 * Removes the element returned by next() from the list.
	 * @complexity at most linear in the size of the list
	 */
	void pop();
	
	/**
	 * Removes the element returned by nextNR() from the list.
	 * @complexity at most logarithmic in the size of the list and linear in the number of roots in the list
	 */
	void popNR();
	
	/**
	 * Removes the element returned by nextNonroot() from the list. The method does nothing if there is no non-root.
	 * @complexity at most linear in the size of the list
	 */
	void popNonroot();
	
	/**
	 * Removes the element returned by nextRoot() from the list. The method does nothing if there is no root.
	 * @complexity at most linear in the size of the list
	 */
	void popRoot();
	
	/**
	 * Replaces the interval-represented element from (if existing) by the numeric element to while maintaining the internal data structures.
	 * It is assumed that from->isRoot() == to->isRoot().
	 * @param from
	 * @param to
	 * @return true if the element was replaced, false otherwise
	 * @complexity logarithmic in the elements stored
	 */
	bool simplify(const RealAlgebraicNumberIR<Number>* from, const RealAlgebraicNumberNR<Number>* to);
	
	/**
	 * Replaces the interval-represented element from, whose iterator in mNRsIRs is given by fromIt, by the numeric element to while maintaining the internal data structures.
	 * It is assumed that from->isRoot() == to->isRoot().
	 * @param from
	 * @param to
	 * @param fromIt iterator to from in mNRsIRs.second, is updated if a replacement was done
	 * @return true if the element was replaced, false otherwise
	 * @complexity logarithmic in the elements stored
	 */
	bool simplify(const RealAlgebraicNumberIR<Number>* from, const RealAlgebraicNumberNR<Number>* to, iteratorIR& fromIt);
	
	/**
	 * Traverse all interval-represented samples and determine whether they could be simplified by numeric representations.
	 * If so, move these samples to the NRs.
	 * @return pair whose first component is a map from unsimplified real algebraic number pointers to real algebraic number pointers
	 * which were simplified; the second component is true if there were samples found which could be simplified.
	 * @complexity logarithmic in the number
	 */
	std::pair<SampleSimplification, bool> simplify();
	
	/**
	 * Determines containment of r in the list.
	 * @return true if r is contained in the list, false otherwise
	 */
	bool contains(const RealAlgebraicNumber<Number>* r) const;
	
	/**
	 * Answers whether there are numerically represented samples left.
	 * @return true if there is no NR sample left in the list, false otherwise
	 */
	bool emptyNR() const {
		return this->NRqueue.empty();
	}
	
	/**
	 * Answers whether there are interval-represented samples left.
	 * @return true if there is no IR sample left in the list, false otherwise
	 */
	bool emptyIR() const {
		return this->IRqueue.empty();
	}
	
	/**
	 * Answers whether there are non-root represented samples left.
	 * @return true if there is no non-root sample left in the list, false otherwise
	 */
	bool emptyNonroot() const {
		return this->nonRootQueue.empty();
	}

	/**
	 * Answers whether there are root samples left.
	 * @return true if there is no root sample left in the list, false otherwise
	 */
	bool emptyRoot() const {
		return this->rootQueue.empty();
	}
	
	/**
	 * Output operator for SampleSet.
	 * @param os output stream
	 * @param s sample set
	 */
	friend std::ostream& operator<<(std::ostream& os, const SampleSet& s);
	
private:
	///////////////////////
	// AUXILIARY METHODS //
	///////////////////////
	
	void removeFromNonrootRoot(const RealAlgebraicNumber<Number>* r);
	
	void removeFromQueue(const RealAlgebraicNumber<Number>* r);
	
	void removeFromNRIR(const RealAlgebraicNumber<Number>* r);
	
};

}
}

#include "SampleSet.tpp"
