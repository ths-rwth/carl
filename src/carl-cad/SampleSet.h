/**
 * @file SampleSet.h
 * @ingroup cad
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include <algorithm>
#include <iostream>
#include <list>
#include <queue>
#include <unordered_map>
#include <utility>
#include <vector>

#include <carl/core/logging.h>
#include <carl/formula/model/ran/RealAlgebraicNumber.h>

#include "CADSettings.h"

namespace carl {
namespace cad {

template<typename Number>
class SampleSet {
public:
	typedef typename std::set<RealAlgebraicNumber<Number>>::iterator Iterator;
	typedef std::unordered_map<RealAlgebraicNumber<Number>, RealAlgebraicNumber<Number>> SampleSimplification;
	/**
	 * A functor compatible to std::less<RealAlgebraicNumber<Number>> that compares two samples according to a given order.
	 */
	struct SampleComparator {
	private:
		/// Ordering used by this functor.
		SampleOrdering mOrdering;
	public:
		/**
		 * Constructor from a given ordering.
         * @param ordering Ordering to be used by the resulting functor.
         */
		explicit SampleComparator(SampleOrdering ordering) : mOrdering(ordering) {}

		/**
		 * Comparison function implemented by this functor.
		 * Checks if lhs is less than rhs according to the ordering.
         * @param lhs Left sample.
         * @param rhs Right sample.
         * @return True if lhs < rhs.
         */
		bool operator()(const RealAlgebraicNumber<Number>& lhs, const RealAlgebraicNumber<Number>& rhs) const;

		/**
		 * Checks if the given sample is optimal with respect to the ordering.
		 * @param s Sample.
		 * @return True if s is optimal.
		 */
		bool isOptimal(const RealAlgebraicNumber<Number>& s) const;

		/**
		 * Returns the current ordering.
		 * @return Ordering.
		 */
		SampleOrdering ordering() const {
			return this->mOrdering;
		}
	private:
		/**
		 * Creates a comparison result from the information if a certain property holds for two objects.
		 * a is less than b if the property does not hold for a, but does for b.
		 * @param l Property of a.
		 * @param r Property of b.
		 * @return (true, a<b) if a<b or a>b, (false, undefined) if a==b (with respect to the inspected property).
		 */
		inline std::pair<bool, bool> compare(bool l, bool r) const {
			if (l ^ r) return std::make_pair(true, r);
			return std::make_pair(false, false);
		}
		/**
		 * Creates a comparison result from the rankings for a certain property of two objects.
		 * a is less than b if the ranking of a is less than the one of b.
		 * @param l Ranking of a.
		 * @param r Ranking of b.
		 * @return (true, a<b) if a<b or a>b, (false, undefined) if a==b (with respect to the inspected property).
		 */
		template<typename T>
		inline std::pair<bool, bool> compare(T l, T r) const {
			if (l != r) return std::make_pair(true, l < r);
			return std::make_pair(false, false);
		}

		/**
		 * Compares two samples checking if they are integers.
         * @param lhs First Sample.
         * @param rhs Second Sample.
         * @return Comparison result.
         */
		inline std::pair<bool, bool> compareInt(const RealAlgebraicNumber<Number>& lhs, const RealAlgebraicNumber<Number>& rhs) const {
			return compare(lhs.isNumeric() && carl::isInteger(lhs.value()), rhs.isNumeric() && carl::isInteger(rhs.value()));
		}
		/**
		 * Compares two samples checking if they are rationals.
         * @param lhs First Sample.
         * @param rhs Second Sample.
         * @return Comparison result.
         */
		inline std::pair<bool, bool> compareRat(const RealAlgebraicNumber<Number>& lhs, const RealAlgebraicNumber<Number>& rhs) const {
			return compare(lhs.isNumeric(), rhs.isNumeric());
		}
		/**
		 * Compares two samples with respect to their representation size.
		 * Requires that both samples have a numeric value.
		 * @param lhs First Sample.
		 * @param rhs Second Sample.
		 * @return Comparison result.
		 */
		inline std::pair<bool, bool> compareSize(const RealAlgebraicNumber<Number>& lhs, const RealAlgebraicNumber<Number>& rhs) const {
			assert(lhs.isNumeric() && rhs.isNumeric());
			return compare(carl::bitsize(lhs.value()), carl::bitsize(lhs.value()));
		}
		/**
		 * Compares two samples checking if they are roots.
         * @param lhs First Sample.
         * @param rhs Second Sample.
         * @return Comparison result.
         */
		inline std::pair<bool, bool> compareRoot(const RealAlgebraicNumber<Number>& lhs, const RealAlgebraicNumber<Number>& rhs) const {
			return compare(lhs.isRoot(), rhs.isRoot());
		}
	};
	
	/// Contains all samples in the order of their value.
	std::set<RealAlgebraicNumber<Number>> mSamples;

	SampleComparator mComp;
	std::vector<RealAlgebraicNumber<Number>> mHeap;
	
	/**
	 * Restore the ordering.
     */
	void restoreOrdering() {
		std::make_heap(mHeap.begin(), mHeap.end(), mComp);
	}
	/**
	 * Reset the ordering.
	 * @param ordering The new ordering.
     */
	void restoreOrdering(SampleOrdering ordering) {
		mComp = SampleComparator(ordering);
		restoreOrdering();
	}
	
public:
	
	SampleSet(SampleOrdering ordering = SampleOrdering::Default):
		mComp(ordering)
	{
		CARL_LOG_TRACE("carl.cad.sampleset", this << " " << __func__ << "( " << ordering << " )");
	}
	
	const std::vector<RealAlgebraicNumber<Number>>& getHeap() const {
		return mHeap;
	}

	/**
	 * Returns the current ordering.
	 * @return Ordering.
	 */
	SampleOrdering ordering() const {
		return mComp.ordering();
	}

	/**
	 * Retrieves the set of samples stored.
     * @return Sample set.
     */
	const std::set<RealAlgebraicNumber<Number>>& samples() const {
		return this->mSamples;
	}

	/**
	 * Insert a new sample into this sample set.
	 * If a sample of this value was already present, it takes care of updating the sample (in case the new one is numeric or a root).
     * @param r Sample to insert.
     * @return An iterator to the inserted sample, a flag that indicates if the insertion changed something and a flag that indicates if a value has been replaced or was new altogether.
     */
	std::tuple<Iterator, bool, bool> insert(const RealAlgebraicNumber<Number>& r);
	
	/**
	 * Inserts a range of samples. Actually calls insert(s) for each sample s in the range.
	 * @param first Start of range.
	 * @param last End of range.
	 */
	template<class InputIterator>
	void insert(InputIterator first, InputIterator last) {
		for (InputIterator i = first; i != last; i++) {
			this->insert(*i);
		}
	}
	
	/**
	 * Inserts another SampleSet.
	 * @param l Other SampleSet.
	 */
	void insert(const SampleSet& l) {
		this->insert(l.begin(), l.end());
	}
	
	/**
	 * Remove the sample at this position.
	 * @param position Valid iterator to a sample.
	 * @return Iterator to the next position in the container.
	 */
#ifdef __VS
	typename SampleSet::Iterator remove(typename SampleSet::Iterator position) {
#else
	SampleSet::Iterator remove(SampleSet::Iterator position) {
#endif
		assert(position != mSamples.end());
		CARL_LOG_TRACE("carl.cad.sampleset", this << " " << __func__ << "( " << *position << " )");
		auto it = std::find(mHeap.begin(), mHeap.end(), *position);
		std::swap(*it, mHeap.back());
		mHeap.pop_back();
		restoreOrdering();
		auto res = mSamples.erase(position);
		assert(this->isConsistent());
		return res;
	}

	/**
	 * Returns an iterator to the first sample in this sample set.
	 * @return Iterator to first sample.
	 */
#ifdef __VS
	typename SampleSet::Iterator begin() {
#else
	SampleSet::Iterator begin() {
#endif
		return this->mSamples.begin();
	}
	/**
	 * Returns an iterator to the first sample in this sample set.
	 * @return Iterator to first sample.
	 */
#ifdef __VS
	const typename SampleSet::Iterator begin() const {
#else
	const SampleSet::Iterator begin() const {
#endif
		return this->mSamples.begin();
	}

	/**
	 * Returns an iterator to the element after the last sample in this sample set.
	 * @return Iterator to end.
	 */
#ifdef __VS
	typename SampleSet::Iterator end() {
#else
	SampleSet::Iterator end() {
#endif
		return this->mSamples.end();
	}
	/**
	 * Returns an iterator to the element after the last sample in this sample set.
	 * @return Iterator to end.
	 */
#ifdef __VS
	const typename SampleSet::Iterator end() const {
#else
	const SampleSet::Iterator end() const {
#endif
		return this->mSamples.end();
	}

	/**
	 * Retrieves the next sample according to the configured ordering.
	 * @return Next sample.
	 */
	inline RealAlgebraicNumber<Number> next() const {
		assert(!mHeap.empty());
		return mHeap.front();
	}

	/**
	 * Checks if the best sample remaining is optimal with respect to the configured ordering.
	 * @return True, if next sample is optimal.
	 */
	inline bool hasOptimal() const {
		if (mHeap.empty()) return false;
		return mComp.isOptimal(next());
	}

	/**
	 * Changes the ordering and retrieves the next sample according to this ordering.
     * @param ordering New ordering.
     * @return Next sample.
     */
	inline RealAlgebraicNumber<Number> next(SampleOrdering ordering) {
		this->restoreOrdering(ordering);
		return this->next();
	}

	/**
	 * Removes the element returned by next() from the list.
	 * @complexity at most linear in the size of the list
	 */
	void pop();
	
	/**
	 * Replaces the interval-represented element from (if existing) by the numeric element to while maintaining the internal data structures.
	 * It is assumed that from->isRoot() == to->isRoot().
	 * @param from
	 * @param to
	 * @return true if the element was replaced, false otherwise
	 * @complexity logarithmic in the elements stored
	 */
	bool simplify(const RealAlgebraicNumber<Number>& from, RealAlgebraicNumber<Number>& to);
	
	/**
	 * Traverse all interval-represented samples and determine whether they could be simplified by numeric representations.
	 * If so, move these samples to the NRs.
	 * @param fast If fast is set to true, no refined is performed. Only samples that have already been refined are simplified.
	 * @return pair whose first component is a map from unsimplified real algebraic number pointers to real algebraic number pointers
	 * which were simplified; the second component is true if there were samples found which could be simplified.
	 * @complexity logarithmic in the number
	 */
	std::pair<SampleSimplification, bool> simplify(bool fast = false);
	
	/**
	 * Determines containment of r in the list.
	 * @return true if r is contained in the list, false otherwise
	 */
	bool contains(const RealAlgebraicNumber<Number> r) const {
		return mSamples.find(r) != mSamples.end();
	}

	/**
	 * Checks if there are any samples left.
     * @return True, if no sample is left.
     */
	bool empty() const {
		return mHeap.empty();
	}
	
	/**
	 * Output operator for SampleSet.
	 * @param os output stream
	 * @param s sample set
	 */
	template<typename Num>
	friend std::ostream& operator<<(std::ostream& os, const SampleSet<Num>& s);
	
	/**
	 * Swaps the contents (all attributes) of the two SampleSets.
	 * @see std::set::swap
	 */
	template<typename Num>
#ifdef __VS
	friend void swap(SampleSet<Num>& lhs, SampleSet<Num>& rhs);
#else
	friend void std::swap(SampleSet<Num>& lhs, SampleSet<Num>& rhs);
#endif

private:
	/**
	 * Checks if this sample set fulfills the following conditions:
	 * <ul>
	 * <li>mSamples and mQueue contain the same samples.</li>
	 * <li>The samples in mSamples are ordered by their value.</li>
	 * </ul>
	 * @return True, if this SampleSet is consistent.
	 */
	bool isConsistent() const;
};

}
}

#ifndef __VS
namespace std {
/**
 * Swaps the contents of two SampleSet objects.
 * @param lhs First SampleSet.
 * @param rhs Second SampleSet.
 */
template<typename Num>
void swap(carl::cad::SampleSet<Num>& lhs, carl::cad::SampleSet<Num>& rhs);
}
#endif

#include "SampleSet.tpp"
