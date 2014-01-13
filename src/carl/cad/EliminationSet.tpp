
#include "EliminationSet.h"

namespace carl {
namespace cad {

template <typename Coefficient>
std::list<const typename EliminationSet<Coefficient>::UPolynomial*> EliminationSet<Coefficient>::getParentsOf(const UPolynomial* p) const {
	std::list<const UPolynomial*> parents;
	auto parentsEntry = this->parentsPerChild.find(p);
	if (parentsEntry != this->parentsPerChild.end()) {
		for (auto parent: parentsEntry->second) {
			parents.push_back(parent.first);
			parents.push_back(parent.second);
		}
	}
	return parents;
}

template <typename Coefficient>
bool EliminationSet<Coefficient>::hasParents(const UPolynomial* p) const {
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
		const UPolynomial* r,
		const std::list<const UPolynomial*>& parents,
		bool avoidSingle
		)
{
	LOGMSG_TRACE("carl.cad", "Adding " << *r << " to eliminationset.");
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
		const UPolynomial* parent1;
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
			this->parentsPerChild[*pos].emplace(parent1, nullptr);
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
std::list<const typename EliminationSet<Coefficient>::UPolynomial*> EliminationSet<Coefficient>::insert(
		InputIterator first,
		InputIterator last,
		const std::list<UPolynomial*>& parents,
		bool avoidSingle 
		)
{
	std::list<UPolynomial*> inserted;
	for (InputIterator i = first; i != last; ++i) {
		std::pair<typename PolynomialSet::iterator, bool> insertValue = this->insert(*i, parents, avoidSingle);
		if (insertValue.second) {
			inserted.push_back(*insertValue.first);
		}
	}
	return inserted;
}

template<typename Coefficient>
std::list<const typename EliminationSet<Coefficient>::UPolynomial*> EliminationSet<Coefficient>::insert(
		const EliminationSet<Coefficient>& s,
		bool avoidSingle
		)
{
	std::list<const UPolynomial*> inserted;
	for (auto i = s.polynomials.begin(); i != s.polynomials.end(); ++i) {
		std::list<const UPolynomial*> parents = s.getParentsOf(*i);
		if (parents.empty()) {
			parents = std::list<const UPolynomial*>( 1, nullptr);
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
	std::forward_list<const UPolynomial> toDelete;
	for (auto i: s) {
		auto iValue = this->polynomials.find(i);
		if (iValue != this->end()) { // polynomial is already present in the set
			toDelete.push_front(i); // mark as to-remove
		}
	}
	for (auto i: toDelete) {
		s.erase(i); // remove polynomial from s
	}
	return !toDelete.empty();
}

template<typename Coefficient>
size_t EliminationSet<Coefficient>::erase(const UPolynomial* p) {
	if (p == 0) return 0;
	
	// remove the child for each parent from the children mapping
	for (auto i:  this->parentsPerChild[p]) {
		if (i.first != 0) this->childrenPerParent[i.first].erase( p );
		if (i.second != 0) this->childrenPerParent[i.second].erase( p );
	}
	// remove the child from the parents mapping
	this->parentsPerChild.erase(p);
	// remove from lifting and elimination queues
	auto queuePosition = std::lower_bound(this->mLiftingQueue.begin(), this->mLiftingQueue.end(), p, this->liftingOrder);
	
	if (queuePosition != this->mLiftingQueue.end() && *queuePosition == p ) {
		this->mLiftingQueue.erase(queuePosition);
	}
	queuePosition = std::lower_bound(mLiftingQueueReset.begin(), mLiftingQueueReset.end(), p, this->liftingOrder);
	if( queuePosition != mLiftingQueueReset.end() && *queuePosition == p )
		mLiftingQueueReset.erase( queuePosition );
	queuePosition = std::lower_bound(mSingleEliminationQueue.begin(), mSingleEliminationQueue.end(), p, this->eliminationOrder);
	if( queuePosition != mSingleEliminationQueue.end() && *queuePosition == p )
		mSingleEliminationQueue.erase(queuePosition);
	queuePosition = std::lower_bound(mPairedEliminationQueue.begin(), mPairedEliminationQueue.end(), p, this->eliminationOrder);
	if( queuePosition != mPairedEliminationQueue.end() && *queuePosition == p )
		mPairedEliminationQueue.erase(queuePosition);
	// remove from main structure
	return this->polynomials.erase(p);
}

template<typename Coefficient>
std::forward_list<const typename EliminationSet<Coefficient>::UPolynomial*> EliminationSet<Coefficient>::removeByParent(const UPolynomial* parent) {
	std::forward_list<const UPolynomial*> deleted;
	if (parent == nullptr) // zero parent is reserved for "no parent"
		return deleted;
	// remove all lone children of parent
	auto position = this->childrenPerParent.find(parent);
	if (position == this->childrenPerParent.end())
		return deleted;    // parent not found
	for (const UPolynomial* child: position->second)	{
		// for all children of parent check their parents
		auto parents = this->parentsPerChild.find(child);
		if (parents == this->parentsPerChild.end() || parents->second.empty())
			continue;    // nothing to be done for this child
		typename parentbucket::const_iterator p = std::find_if( parents->second.begin(), parents->second.end(), PolynomialPairContains(parent));
		while (p != parents->second.end()) {
			// search matching parents
			parents->second.erase(p); // remove either single matching parent or parents which got divorced by the removed parent
			p = std::find_if( parents->second.begin(), parents->second.end(), PolynomialPairContains(parent));
		}

		if (parents->second.empty()) {
			// no parent was left for the child, so delete it
			this->parentsPerChild.erase( parents );
			auto queuePosition = std::lower_bound( mLiftingQueue.begin(), mLiftingQueue.end(), child, this->liftingOrder );
			if( queuePosition != mLiftingQueue.end() )
				mLiftingQueue.erase( queuePosition );
			queuePosition = std::lower_bound( mLiftingQueueReset.begin(), mLiftingQueueReset.end(), child, this->liftingOrder );
			if( queuePosition != mLiftingQueueReset.end() )
				mLiftingQueueReset.erase( queuePosition );
			queuePosition = std::lower_bound( mSingleEliminationQueue.begin(), mSingleEliminationQueue.end(), child, this->eliminationOrder );
			if( queuePosition != mSingleEliminationQueue.end() )
				mSingleEliminationQueue.erase( queuePosition );
			queuePosition = std::lower_bound( mPairedEliminationQueue.begin(), mPairedEliminationQueue.end(), child, this->eliminationOrder );
			if( queuePosition != mPairedEliminationQueue.end() )
				mPairedEliminationQueue.erase( queuePosition );
			deleted.push_front(child);
			this->polynomials.erase(child);
		}
	}
	// remove the information of parent itself
	this->childrenPerParent.erase(position);
	return deleted;
}

template<typename Coefficient>
const typename EliminationSet<Coefficient>::UPolynomial* EliminationSet<Coefficient>::find(const UPolynomial* p) {
	auto position = this->polynomials.find(p);
	return (position == this->polynomials.end() ? nullptr : *position);
}

template<typename Coefficient>
void swap(EliminationSet<Coefficient>& lhs, EliminationSet<Coefficient>& rhs) {
	std::swap(lhs.polynomials, rhs.polynomials);
	std::swap(lhs.mLiftingQueue, rhs.mLiftingQueue);
	std::swap(lhs.mLiftingQueueReset, rhs.mLiftingQueueReset);
	std::swap(lhs.mSingleEliminationQueue, rhs.mSingleEliminationQueue);
	std::swap(lhs.mPairedEliminationQueue, rhs.mPairedEliminationQueue);
	std::swap(lhs.childrenPerParent, rhs.childrenPerParent);
	std::swap(lhs.parentsPerChild, rhs.parentsPerChild);
	std::swap(lhs.liftingOrder, rhs.liftingOrder);
	std::swap(lhs.eliminationOrder, rhs.eliminationOrder);
}

template<typename Coefficient>
void EliminationSet<Coefficient>::clear() {
	this->polynomials.clear();
	this->mLiftingQueue.clear();
	this->mLiftingQueueReset.clear();
	this->mSingleEliminationQueue.clear();
	this->mPairedEliminationQueue.clear();
	this->childrenPerParent.clear();
	this->parentsPerChild.clear();
}

template<typename Coefficient>
void EliminationSet<Coefficient>::resetLiftingPositionsFully() {
	this->mLiftingQueue.assign( this->polynomials.begin(), this->polynomials.end() );
	this->mLiftingQueue.sort( this->liftingOrder );
}

template<typename Coefficient>
const typename EliminationSet<Coefficient>::UPolynomial* EliminationSet<Coefficient>::popNextSingleEliminationPosition() {
	const UPolynomial* p = mSingleEliminationQueue.front();
	mSingleEliminationQueue.pop_front();
	return p;
}

template<typename Coefficient>
std::list<const typename EliminationSet<Coefficient>::UPolynomial*> EliminationSet<Coefficient>::eliminateInto(
		const UPolynomial* p,
		EliminationSet<Coefficient>& destination,
		const Variable& variable,
		const CADSettings& setting
		)
{
	if (p->isConstant())
	{ /* constants can just be moved from this level to the next */
		if (p->isNumber())
		//if( GiNaC::is_exactly_a<numeric>( *p )) /* discard numerics completely */
		{
			this->erase(p);
			return {};
		}
		UPolynomial* pNewVar = new UPolynomial(p->switchVariable(variable));
		destination.insert(pNewVar, this->getParentsOf(p));
		if( setting.removeConstants || p->isNumber()) /* remove constant from this level and discard numerics completely */
			this->erase(p);
		else {
			auto queuePosition = std::lower_bound(mSingleEliminationQueue.begin(), mSingleEliminationQueue.end(), p, this->eliminationOrder);
			if( queuePosition != mSingleEliminationQueue.end() && *queuePosition == p )
				mSingleEliminationQueue.erase( queuePosition );
			queuePosition = std::lower_bound(mPairedEliminationQueue.begin(), mPairedEliminationQueue.end(), p, this->eliminationOrder);
			if( queuePosition != mPairedEliminationQueue.end() && *queuePosition == p )
				mPairedEliminationQueue.erase( queuePosition );
		}
		return { pNewVar };
	}

	EliminationSet<Coefficient> newEliminationPolynomials = EliminationSet<Coefficient>(this->liftingOrder, this->eliminationOrder);

	// PAIRED elimination with the new polynomials: (1) together with the existing ones (2) among themselves

	if( setting.equationsOnly ) {
		// (1) elimination with existing polynomials
		for (auto pol_it1: this->polynomials)
			eliminationEq( p, pol_it1, variable, newEliminationPolynomials, false );
		// (2) elimination with polynomial itself @todo: proof that we do not need that
		// eliminationEq( p, p, variable, newEliminationPolynomials, setting );
	} else {
		// (1) elimination with existing polynomials
		for (auto pol_it1: this->polynomials)
			elimination( p, pol_it1, variable, newEliminationPolynomials, false );
		// (2) elimination with polynomial itself @todo: proof that we do not need that
		// elimination( p, p, variable, newEliminationPolynomials, setting );
		
	}

	// !PAIRED (single) elimination

	if( setting.equationsOnly ) {
		eliminationEq( p, variable, newEliminationPolynomials, false );
	} else {
		elimination( p, variable, newEliminationPolynomials, false );
	}
	

	// optimizations
	if( setting.simplifyByFactorization )
		newEliminationPolynomials.factorize();
	newEliminationPolynomials.makePrimitive();
	newEliminationPolynomials.makeSquarefree();
	if( setting.simplifyByRootcounting )
		newEliminationPolynomials.removePolynomialsWithoutRealRoots();
	// insert the new polynomials of the last step into the new level (now currentLevel)
	return destination.insert( newEliminationPolynomials );
}

template<typename Coefficient>
std::list<const typename EliminationSet<Coefficient>::UPolynomial*> EliminationSet<Coefficient>::eliminateConstant(
		const UPolynomial* p,
		std::list<const UPolynomial*>& queue,
		std::list<const UPolynomial*>& otherqueue,
		bool avoidSingle,
		EliminationSet<Coefficient>& destination,
		const Variable& variable,
		const CADSettings& setting
		)
{
	if (p->isNumber()) { /* discard numerics completely */
		this->erase(p);
		return {};
	}
	const UPolynomial* pNewVar = new UPolynomial(p->switchVariable(variable));
	destination.insert(pNewVar, this->getParentsOf(p), avoidSingle);
	if( setting.removeConstants ) /* remove constant from this level */
		this->erase(p);
	else
	{
		if (!queue.empty())
			queue.pop_front();
		auto queuePosition = std::lower_bound(otherqueue.begin(), otherqueue.end(), p, this->eliminationOrder);
		if (queuePosition != otherqueue.end())
			otherqueue.erase(queuePosition);
	}
	return { pNewVar };
}

template<typename Coefficient>
std::list<const typename EliminationSet<Coefficient>::UPolynomial*> EliminationSet<Coefficient>::eliminateNextInto(
		EliminationSet<Coefficient>& destination,
		const Variable& variable,
		const CADSettings& setting,
		bool synchronous
		)
{
	const UPolynomial* p;
	bool avoidSingle = false;
	if (this->mPairedEliminationQueue.empty()) {
		if (!this->mSingleEliminationQueue.empty()) {
			p = this->mSingleEliminationQueue.front();
			if (p->isConstant()) {
				return this->eliminateConstant(p, this->mSingleEliminationQueue, this->mPairedEliminationQueue, false, destination, variable, setting);
			}
		} else return {};
	} else {
		p = this->mPairedEliminationQueue.front();
		if (p->isConstant()) {
			return this->eliminateConstant(p, this->mPairedEliminationQueue, this->mSingleEliminationQueue, false, destination, variable, setting);
		}
		avoidSingle = synchronous;
	}

	EliminationSet<Coefficient> newEliminationPolynomials = EliminationSet(this->liftingOrder, this->eliminationOrder);

	// PAIRED elimination with the new polynomials: (1) together with the existing ones (2) among themselves
	if (!mPairedEliminationQueue.empty()) {
		if( setting.equationsOnly ) {
			// (1) elimination with existing polynomials
			for (auto pol_it1: this->polynomials)
				eliminationEq( p, pol_it1, variable, newEliminationPolynomials, false );
			// (2) elimination with polynomial itself @todo: proof that we do not need that
			// eliminationEq( p, p, variable, newEliminationPolynomials, setting );	
		} else {
			// (1) elimination with existing polynomials
			for (auto pol_it1: this->polynomials)
				elimination( p, pol_it1, variable, newEliminationPolynomials, false );
			// (2) elimination with polynomial itself @todo: proof that we do not need that
			// elimination( p, p, variable, newEliminationPolynomials, setting );
		}
		mPairedEliminationQueue.pop_front();
	}

	// !PAIRED (single) elimination
	if( !mSingleEliminationQueue.empty() &&
			( ( !synchronous || p == mSingleEliminationQueue.front() ) || mPairedEliminationQueue.empty() ) )
	{
		p = mSingleEliminationQueue.front();
		if (setting.equationsOnly) {
			eliminationEq( p, variable, newEliminationPolynomials, false );
		} else {
			elimination( p, variable, newEliminationPolynomials, false );
		}
		mSingleEliminationQueue.pop_front();
	}

	// optimizations
	if( setting.simplifyByFactorization )
		newEliminationPolynomials.factorize();
	newEliminationPolynomials.makePrimitive();
	newEliminationPolynomials.makeSquarefree();
	if( setting.simplifyByRootcounting )
		newEliminationPolynomials.removePolynomialsWithoutRealRoots();
	// insert the new polynomials of the last step into the new level (now currentLevel)
	return destination.insert( newEliminationPolynomials, avoidSingle );
}

template<typename Coefficient>
void EliminationSet<Coefficient>::moveConstants(EliminationSet<Coefficient>& to, const Variable& variable ) {
	std::forward_list<const UPolynomial*> toDelete;
	for (auto p: this->polynomials) {
		if(p->isConstant()) {
			if (p->isNumber()) { // discard numerics completely
				to.insert(p->switchVariable(variable), this->getParentsOf(p));
			}	
			toDelete.push_front(p);
		}
	}
	for (auto p: toDelete) {
		this->erase(p);
	}
}

template<typename Coefficient>
void EliminationSet<Coefficient>::removeConstants() {
	if (this->empty()) return;
	std::forward_list<const UPolynomial*> toDelete;
	for (auto p: this->polynomials) {
		if (p->isConstant()) {   // found candidate for removal
			toDelete.push_front(p);
		}
	}
	for (auto p: toDelete) {
		this->erase(p);
	}
}

template<typename Coefficient>
void EliminationSet<Coefficient>::removePolynomialsWithoutRealRoots() {
	if (this->empty()) return;
	// collect polynomials which shall be deleted
	std::forward_list<const UPolynomial*> toDelete;
	for (auto p: this->polynomials) {
		if (p->degree() % 2 == 0 && p->isUnivariate())
		{    // this is a good candidate for having no real roots
			if (rootfinder::countRealRoots(*p) == 0) {
				toDelete.push_front(p);
			}
		}
	}
	// delete collected polynomials
	for (auto p: toDelete) {
		this->erase(p);
	}
}

template<typename Coefficient>
void EliminationSet<Coefficient>::makeSquarefree() {
	EliminationSet<Coefficient> squarefreeSet(this->liftingOrder, this->eliminationOrder);
	for (auto p: this->polynomials) {
		squarefreeSet.insert(new UPolynomial(p->squareFreePart()), this->getParentsOf(p));
	}
	std::swap(*this, squarefreeSet);
}

template<typename Coefficient>
void EliminationSet<Coefficient>::makePrimitive() {
	EliminationSet<Coefficient> primitiveSet(this->liftingOrder, this->eliminationOrder);
	for (auto p: this->polynomials) {
		if (p->isNumber()) continue; // numbers are discarded
		
		typename UnderlyingNumberType<Coefficient>::type c = p->numericContent();
		if( c != 1 && c != 0 ) // only non-trivial content parts are considered
			primitiveSet.insert(p->pseudoPrimpart(cad::MPolynomial<Coefficient>(c)), this->getParentsOf(p));
		else
			primitiveSet.insert(p, this->getParentsOf(p));
	}
	
	std::swap(*this, primitiveSet);
}


template<typename Coefficient>
void EliminationSet<Coefficient>::factorize() {
	EliminationSet<Coefficient> factorizedSet(this->liftingOrder, this->eliminationOrder);
	for (auto p: this->polynomials) {
		// insert the factors and omit the original
		// TODO: Perform multivariate factorization here.
		/*for (auto factor: p->factorization()) {
			factorizedSet.insert(factor.first, this->getParentsOf(p));
		}*/
		factorizedSet.insert(p, this->getParentsOf(p));
	}
	std::swap(*this, factorizedSet);
}

template<typename Coeff>
std::ostream& operator<<(std::ostream& os, const carl::cad::EliminationSet<Coeff>& s) {
	os << "{ ";
	for (auto p: s.polynomials) os << *p << " ";
	os << "}( ";
	for (auto i: s.mLiftingQueue) os << *i << " ";
	os << ")";
	return os;
}

template<typename Coeff>
std::list<const typename EliminationSet<Coeff>::UPolynomial*> EliminationSet<Coeff>::truncations(const UPolynomial* p) {
	std::list<const UPolynomial*> truncations;
	truncations.push_back(p);
	UPolynomial truncation(*p);
	while (!truncation.isConstant()) {
		truncation.truncate();
		truncations.push_back(new UPolynomial(truncation));
	}
	return truncations;
}

template<typename Coeff>
void EliminationSet<Coeff>::elimination(
			const UPolynomial* p,
			const Variable& variable,
			EliminationSet<Coeff>& eliminated,
			bool avoidSingle
) {
	std::list<const UPolynomial*> truncations = EliminationSet<Coeff>::truncations(p);
	std::list<const UPolynomial*> parents({p});
	for (auto it: truncations) {
		auto lcoeff = it->lcoeff();
		if (lcoeff.isNumber()) continue;
		eliminated.insert(new UPolynomial(variable, lcoeff), parents, avoidSingle);
		const UPolynomial diff = it->derivative();
		if (!diff.isZero()) {
			// TODO: continue implementing once subresultants are implemented
		}
	}
}

template<typename Coeff>
void EliminationSet<Coeff>::elimination(
		const UPolynomial* p,
		const UPolynomial* q,
		const Variable&,
		EliminationSet<Coeff>&,
		bool 
) {
	std::list<const UPolynomial*> truncations = EliminationSet<Coeff>::truncations(p);
	std::list<const UPolynomial*> parents({p, q});
	//for (auto it: truncations) {
		// TODO: continue implementing once subresultants are implemented
	//}
}

}
}

