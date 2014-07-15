/**
 * @file SampleSet.h
 * @ingroup cad
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include <list>
#include <unordered_map>
#include <iostream>
#include <queue>
#include <utility>

#include "../core/logging.h"
#include "../core/RealAlgebraicNumber.h"
#include "../core/RealAlgebraicNumberIR.h"
#include "../core/RealAlgebraicNumberNR.h"

#include "CADSettings.h"

namespace carl {
namespace cad {

template<typename Number>
class SampleSet {
public:
	typedef typename std::set<RealAlgebraicNumberPtr<Number>>::iterator Iterator;
	typedef std::unordered_map<RealAlgebraicNumberIRPtr<Number>, RealAlgebraicNumberNRPtr<Number>> SampleSimplification;
private:
	/**
	 * A functor compatible to std::less<RealAlgebraicNumberPtr<Number>> that compares two samples according to a given order.
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
		bool operator()(const RealAlgebraicNumberPtr<Number>& lhs, const RealAlgebraicNumberPtr<Number>& rhs) const;

		/**
		 * Checks if the given sample is optimal with respect to the ordering.
		 * @param s Sample.
		 * @return True if s is optimal.
		 */
		bool isOptimal(const RealAlgebraicNumberPtr<Number>& s) const;

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
		 * a  is less than b if the property does not hold for a, but does for b.
         * @param l Property of a.
         * @param r Property of b.
         * @return (true, a<b) if a<b or a>b, (false, undefined) if a==b (with respect to the inspected property).
         */
		inline std::pair<bool, bool> compare(bool l, bool r) const {
			if (l xor r) return std::make_pair(true, l);
			return std::make_pair(false, false);
		}
		/**
		 * Compares two samples checking if they are integers.
         * @param lhs First Sample.
         * @param rhs Second Sample.
         * @return Comparison result.
         */
		inline std::pair<bool, bool> compareInt(const RealAlgebraicNumberPtr<Number>& lhs, const RealAlgebraicNumberPtr<Number>& rhs) const {
			return compare(lhs->isNumericRepresentation() && carl::isInteger(lhs->value()), rhs->isNumericRepresentation() && carl::isInteger(rhs->value()));
		}
		/**
		 * Compares two samples checking if they are rationals.
         * @param lhs First Sample.
         * @param rhs Second Sample.
         * @return Comparison result.
         */
		inline std::pair<bool, bool> compareRat(const RealAlgebraicNumberPtr<Number>& lhs, const RealAlgebraicNumberPtr<Number>& rhs) const {
			return compare(lhs->isNumericRepresentation(), rhs->isNumericRepresentation());
		}
		/**
		 * Compares two samples checking if they are roots.
         * @param lhs First Sample.
         * @param rhs Second Sample.
         * @return Comparison result.
         */
		inline std::pair<bool, bool> compareRoot(const RealAlgebraicNumberPtr<Number>& lhs, const RealAlgebraicNumberPtr<Number>& rhs) const {
			return compare(lhs->isRoot(), rhs->isRoot());
		}
	};
	
	/// Contains all samples in the order of their value.
	std::set<RealAlgebraicNumberPtr<Number>, carl::Less<Number>> mSamples;
	/// Contains all samples in the order specified by the comparator.
	std::set<RealAlgebraicNumberPtr<Number>, SampleComparator> mQueue;
	
	/**
	 * Change the ordering of mQueue.
     * @param ordering New ordering.
     */
	void resetOrdering(SampleOrdering ordering) {
		if (ordering != mQueue.key_comp().ordering()) {
			// Ordering differs from current ordering.
			LOGMSG_TRACE("carl.cad.sampleset", this << " " << __func__ << "( " << ordering << " )");
			if (mQueue.size() > 5) {
				LOGMSG_WARN("carl.cad.sampleset", "Reordering sample set of size " << mQueue.size());
			}
			std::set<RealAlgebraicNumberPtr<Number>, SampleComparator> newSet(ordering);
			// Copy samples to a new set with new ordering.
			newSet.insert(mQueue.begin(), mQueue.end());
			// Swap the sets.
			std::swap(mQueue, newSet);
		}
	}
	
public:
	SampleSet(SampleOrdering ordering = SampleOrdering::Default): mQueue(SampleComparator(ordering))
	{
		LOGMSG_TRACE("carl.cad.sampleset", this << " " << __func__ << "( " << ordering << " )");
	}

	/**
	 * Returns the current ordering.
	 * @return Ordering.
	 */
	SampleOrdering ordering() const {
		return this->mQueue.key_comp().ordering();
	}

	/**
	 * Insert a new sample into this sample set.
     * @param r Sample to insert.
     * @return An iterator to the inserted sample and a flag that indicates if the inserted value was new or already present.
     */
	std::pair<Iterator, bool> insert(RealAlgebraicNumberPtr<Number> r) {
		LOGMSG_TRACE("carl.cad.sampleset", this << " " << __func__ << "( " << r << " )");
		assert(this->isConsistent());
		auto res = this->mSamples.insert(r);
		if (!res.second) {
			// There is an equivalent sample.
			// Check if new sample is better (i.e. smaller) than the previous sample.
			if (mQueue.key_comp()(r, *res.first)) {
				LOGMSG_TRACE("carl.cad.sampleset", "\tsample already exists, replace in queue and update in samples");
				assert(mQueue.count(*res.first) == 1);
				std::size_t cnt = mQueue.erase(*res.first);
				assert(cnt == 1);
				mQueue.insert(r);
				mSamples.erase(res.first);
				mSamples.insert(r);
			}
		} else {
			mQueue.insert(r);
		}
		assert(this->isConsistent());
		return res;
	}
	
	/**
	 * Inserts a range of samples. Actually calls insert(s) for each sample s in the range.
	 * @param Start of range.
	 * @param End of range.
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
	SampleSet::Iterator remove(SampleSet::Iterator position) {
		assert(position != mSamples.end());
		LOGMSG_TRACE("carl.cad.sampleset", this << " " << __func__ << "( " << *position << " )");
		mQueue.erase(*position);
		auto res = mSamples.erase(position);
		assert(this->isConsistent());
		return res;
	}

	SampleSet::Iterator begin() {
		return this->mSamples.begin();
	}
	const SampleSet::Iterator begin() const {
		return this->mSamples.begin();
	}

	SampleSet::Iterator end() {
		return this->mSamples.end();
	}
	const SampleSet::Iterator end() const {
		return this->mSamples.end();
	}

	/**
	 * Retrieves the next sample according to the configured ordering.
	 * @return Next sample.
	 */
	inline RealAlgebraicNumberPtr<Number> next() const {
		assert(!mQueue.empty());
		return *mQueue.begin();
	}

	/**
	 * Checks if the best sample remaining is optimal with respect to the configured ordering.
	 * @return True, if next sample is optimal.
	 */
	inline bool hasOptimal() const {
		if (mQueue.empty()) return false;
		return mQueue.key_comp().isOptimal(*mQueue.begin());
	}

	/**
	 * Changes the ordering and retrieves the next sample according to this ordering.
     * @param ordering New ordering.
     * @return Next sample.
     */
	inline RealAlgebraicNumberPtr<Number> next(SampleOrdering ordering) {
		this->resetOrdering(ordering);
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
	bool simplify(const RealAlgebraicNumberIRPtr<Number> from, RealAlgebraicNumberNRPtr<Number> to);
	
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
	bool contains(const RealAlgebraicNumberPtr<Number> r) const {
		return mSamples.find(r) != mSamples.end();
	}

	/**
	 * Checks if there are any samples left.
     * @return True, if no sample is left.
     */
	bool empty() const {
		return this->mQueue.empty();
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
	friend void std::swap(SampleSet<Num>& lhs, SampleSet<Num>& rhs);

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

namespace std {
/**
 * Swaps the contents of two SampleSet objects.
 * @param lhs First SampleSet.
 * @param rhs Second SampleSet.
 */
template<typename Num>
void swap(carl::cad::SampleSet<Num>& lhs, carl::cad::SampleSet<Num>& rhs);
}

#include "SampleSet.tpp"
