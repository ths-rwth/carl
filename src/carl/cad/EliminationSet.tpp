
#include "EliminationSet.h"

#include <forward_list>

namespace carl {
namespace CAD {

template <typename Coefficient>
bool EliminationSet<Coefficient>::hasParents(const UnivariatePolynomialPtr<Coefficient>& p) const {
	auto parentsEntry =  this->parentsPerChild.find(p);
	return
		// There is an entry for this polynomial.
		(parentsEntry != this->parentsPerChild.end()) &&
		// This entry is meaningful, i.e. not an empty bucket-
		!parentsEntry->second.empty() &&
		(
			// The first entry in the bucket is not null, ...
			(parentsEntry->second.begin()->first != nullptr) ||
			// ... or there is at least another one.
			(parentsEntry->second.size() > 1)
		);
}

template <typename Coefficient>
std::pair<typename EliminationSet<Coefficient>::PolynomialSet::iterator, bool> EliminationSet<Coefficient>::insert(
		const UnivariatePolynomialPtr<Coefficient> r,
		const std::list<UnivariatePolynomialPtr<Coefficient>>& parents,
		bool avoidSingle
		)
{
	LOG4CPLUS_TRACE("carl.cad", "Adding " << r << " to eliminationset.");
	std::pair<typename PolynomialSet::iterator, bool> insertValue = this->polynomials.insert(r);
	typename PolynomialSet::iterator pos = insertValue.first;

	if (
		(
			(parents.empty() || parents.front() == nullptr) &&
			this->hasParents( *pos )
		) || (
			!parents.empty() &&
			(parents.front() != nullptr)
		)
	) {
		bool oneParentFound = false;
		UnivariatePolynomialPtr<Coefficient> parent1;
		for (auto parent: parents) {
			auto children = this->childrenPerParent.find(parent);
			if (children == this->childrenPerParent.end()) {
				this->childrenPerParent[parent].insert(*pos);
			} else {
				children->second.insert(*pos);
			}
			if (oneParentFound) {
				this->parentsPerChild[*pos].emplace(parent1, parent);
				oneParentFound = false;
			} else {
				parent1 = parent;
				oneParentFound = true;
			}
		}
		if (oneParentFound) {
			this->parentsPerChild[*pos].emplace(parent1, UnivariatePolynomialPtr<Coefficient>());
		}
	}

	if (insertValue.second) {
		auto queuePosition = std::lower_bound(this->mLiftingQueue.begin(), this->mLiftingQueue.end(), *pos, this->liftingOrder);
		if (queuePosition == this->mLiftingQueue.end()) {
			this->mLiftingQueue.push_back(*pos);
		} else {
			this->mLiftingQueue.insert(queuePosition, *pos);
		}

		queuePosition = std::lower_bound(this->mLiftingQueueReset.begin(), this->mLiftingQueueReset.end(), *pos, this->liftingOrder);
		if (queuePosition == this->mLiftingQueueReset.end()) {
			this->mLiftingQueueReset.push_back(*pos);
		} else {
			this->mLiftingQueueReset.insert(queuePosition, *pos);
		}

		if (!avoidSingle) {
			queuePosition = std::lower_bound(this->mSingleEliminationQueue.begin(), this->mSingleEliminationQueue.end(), *pos, this->liftingOrder);
			if (queuePosition == this->mSingleEliminationQueue.end()) {
				this->mSingleEliminationQueue.push_back(*pos);
			} else {
				this->mSingleEliminationQueue.insert(queuePosition, *pos);
			}
		}

		queuePosition = std::lower_bound(this->mPairedEliminationQueue.begin(), this->mPairedEliminationQueue.end(), *pos, this->liftingOrder);
		if (queuePosition == this->mPairedEliminationQueue.end()) {
			this->mPairedEliminationQueue.push_back(*pos);
		} else {
			this->mPairedEliminationQueue.insert(queuePosition, *pos);
		}
	}

	return insertValue;
}

template<typename Coefficient>
template<class InputIterator>
std::list<UnivariatePolynomialPtr<Coefficient>> EliminationSet<Coefficient>::insert(
		InputIterator first,
		InputIterator last,
		const std::list<UnivariatePolynomialPtr<Coefficient>>& parents,
		bool avoidSingle 
		)
{
	std::list<UnivariatePolynomialPtr<Coefficient>> inserted;
	for (InputIterator i = first; i != last; ++i) {
		std::pair<typename PolynomialSet::iterator, bool> insertValue = this->insert(*i, parents, avoidSingle);
		if (insertValue.second) {
			inserted.push_back(*insertValue.first);
		}
	}
	return inserted;
}

template<typename Coefficient>
std::pair<typename EliminationSet<Coefficient>::PolynomialSet::iterator, bool> EliminationSet<Coefficient>::insert(
		const UnivariatePolynomial<Coefficient>& r,
		const std::list<UnivariatePolynomialPtr<Coefficient>>& parents,
		bool avoidSingle
		)
{
	return this->insert(std::make_shared<UnivariatePolynomial<Coefficient>>(r), parents, avoidSingle);
}

template<typename Coefficient>
std::list<UnivariatePolynomialPtr<Coefficient>> EliminationSet<Coefficient>::insert(
		const EliminationSet<Coefficient>& s,
		bool avoidSingle
		)
{
	std::list<UnivariatePolynomialPtr<Coefficient>> inserted = std::list<UnivariatePolynomialPtr<Coefficient>>();
	for (typename PolynomialSet::const_iterator i = s.begin(); i != s.end(); ++i) {
		std::list<UnivariatePolynomialPtr<Coefficient>> parents = s.getParentsOf(*i);
		if (parents.empty()) {
			parents = std::list<UnivariatePolynomialPtr<Coefficient>>( 1, UnivariatePolynomialPtr<Coefficient>());
		}
		std::pair<typename PolynomialSet::iterator, bool> insertValue = this->insert(*i, parents, avoidSingle);
		if(insertValue.second) {
			inserted.push_front(*insertValue.first);
		}
	}
	return inserted;
}

template<typename Coefficient>
bool EliminationSet<Coefficient>::insertAmend(EliminationSet<Coefficient>& s) {
	std::forward_list<UnivariatePolynomialPtr<Coefficient>> toDelete;
	for (typename PolynomialSet::iterator i = s.begin(); i != s.end(); ++i) {
		typename PolynomialSet::const_iterator iValue = this->polynomials.find(*i);
		if (iValue != this->end()) { // polynomial is already present in the set
			toDelete.push_front(*i); // mark as to-remove
		}
	}
	for (auto i = toDelete.begin(); i != toDelete.end(); ++i) {
		s.erase(*i); // remove polynomial from s
	}
	return !toDelete.empty();
}

}
}
