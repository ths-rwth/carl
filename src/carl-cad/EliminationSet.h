/**
 * @file EliminationSet.h
 * @ingroup cad
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include <forward_list>
#include <list>
#include <memory>
#include <set>
#include <utility>
#include <unordered_map>
#include <unordered_set>

#include <carl/util/pointerOperations.h>
#include <carl/core/UnivariatePolynomial.h>
#include <carl/core/logging.h>

#include "CADTypes.h"
#include "CADSettings.h"
#include "Projection.h"

namespace carl {
namespace cad {
	
/**
 * This class stores the polynomials that are still to be eliminated in the CAD.
 * 
 * It organizes multiple queues (for single and paired elimination) as well as the relationship between polynomials.
 */
template<typename Coefficient>
class EliminationSet {
public:
	typedef carl::cad::UPolynomial<Coefficient> UPolynomial;
private:
	
	/**
	 * Represents a pair of polynomials. 
	 * Used to store the ancestors of a polynomial. If one is nullptr, the polynomial has only a single ancestor.
	 */
	typedef std::pair<const UPolynomial*, const UPolynomial*> PolynomialPair;
	
	/**
	 * Functor that compares two PolynomialPair objects.
     */
	struct PolynomialPairIsLess {
		std::less<UPolynomial> less;
		unsigned int length(const PolynomialPair& p) const {
			if (p.first == nullptr && p.second == nullptr) return 0;
			if (p.first == nullptr || p.second == nullptr) return 1;
			return 2;
		}
		bool operator()(const PolynomialPair& a, const PolynomialPair& b) const {
			unsigned int l1 = this->length(a), l2 = this->length(b);
			if (l1 < l2) return true;
			if (l2 < l1) return false;
			if (less(a.first, b.first)) return true;
			if (a.first == b.first) return less(a.second, b.second);
			return false;
		}
	};
	
	/**
	 * Functor that checks if a given PolynomialPair contains some polynomial.
	 */
	struct PolynomialPairContains {
	private:
		const UPolynomial* p;
	public:
		/**
		 * Constructor.
         * @param p Polynomial to search for.
         */
		PolynomialPairContains(const UPolynomial* _p) : p(_p) {
		}
		/**
		 * Checks if the PolynomialPair contains the polynomial that was passed to the constructor.
         * @param pp PolynomialPair.
         * @return If pp contains the polynomial.
         */
		bool operator()(const PolynomialPair& pp) {
			if (p == nullptr) {
				return (pp.first == nullptr) || (pp.second == nullptr);
			} else if (pp.first == nullptr) {
				return (pp.second != nullptr) && (*p == *(pp.second));
			} else if (pp.second == nullptr) {
				return (*p == *(pp.first));
			} else {
				return (*p == *(pp.first)) || (*p == *(pp.second));
			}
		}
	};
	/**
	 * A set of polynomials.
	 */
	typedef std::unordered_set<const UPolynomial*, carl::hash<UPolynomial*>, carl::equal_to<UPolynomial*>> PolynomialSet;
	//typedef std::set<const UPolynomial*> PolynomialSet;

	/**
	 * A mapping from one polynomial to a sorted range of other polynomials.
	 */
	typedef std::map<const UPolynomial*, PolynomialSet> PolynomialBucketMap;
	
	/**
	 * A set of PolynomialPair objects.
	 */
	typedef std::set<PolynomialPair, PolynomialPairIsLess> parentbucket;
	
	/**
	 * Maps a polynomial to a parentbucket.
	 * The parentbucket contains all pairs of polynomials that produce the stored polynomial when eliminated.
	 */
	typedef std::map<const UPolynomial*, parentbucket> parentbucket_map;

	
// public types
public:
	/**
	 * The comparator used for polynomials here.
	 */
	typedef std::less<cad::UPolynomial<Coefficient>> PolynomialComparator;

// private members
private:	
	/**
	 * Set of polynomials.
	 */
	PolynomialSet polynomials;
	
	/**
	 * PolynomialComparator that defines the order of the polynomials in the elimination queue.
	 */
	PolynomialComparator eliminationOrder;
	
	/**
	 * PolynomialComparator that defines the order of the polynomials in the lifting queue.
	 */
	PolynomialComparator liftingOrder;

	ProjectionType projectionType = ProjectionType::Brown;
	ProjectionOperator<const UPolynomial*> projection;
	template<typename... Args>
	void project(Args&&... args) const {
		projection(projectionType, std::forward<Args>(args)...);
	}

	/**
	 * Elimination queue containing all polynomials not yet considered for non-paired elimination.
	 * Access permits reset of the queue, automatic update after insertion of new elements and a pop method.
	 */
	std::list<const UPolynomial*> mSingleEliminationQueue;
	/**
	 * Elimination queue containing all polynomials not yet considered for paired elimination.
	 * Access permits reset of the queue, automatic update after insertion of new elements and a pop method.
	 */
	std::list<const UPolynomial*> mPairedEliminationQueue;
	
	/**
	 * Lifting queue containing all polynomials not yet considered for lifting.
	 * Access permits reset of the queue, automatic update after insertion of new elements and a pop method.
	 */
	std::list<const UPolynomial*> mLiftingQueue;
	/**
	 * Lifting queue containing a reset state for the lifting queue, which is a copy of the original container contents, but can be set to a concrete state
	 */
	std::list<const UPolynomial*> mLiftingQueueReset;
	
	/// maps an entry from another EliminationSet (parent) to the entry of elimination polynomial belonging to the parent
	PolynomialBucketMap childrenPerParent;
	/// assigns to each elimination polynomial its parents
	parentbucket_map parentsPerChild;

	/**
	 * Ownership of all polynomials created within this object are delegated to the polynomialOwner.
	 */
	PolynomialOwner<Coefficient>* polynomialOwner;

// public members
public:
	
	
	/*
	 * This flag indicates whether this elimination set contains polynomials that are only valid within certain bounds.
	 */
	bool bounded;
	
	EliminationSet() = delete;

	/**
	 * Constructs the elimination set with a given strict ordering f of univariate polynomials.
	 * @param owner PolynomialOwner object that recieves the ownership of all polynomials that are created within this EliminationSet.
	 * @param f strict ordering for lifting queue (standard is UnivariatePolynomial::univariatePolynomialIsLess)
	 * @param g strict ordering for elimination queue (standard is UnivariatePolynomial::univariatePolynomialIsLess)
	 */
	EliminationSet(
			PolynomialOwner<Coefficient>* owner,
			PolynomialComparator f = std::less<UPolynomial>(),
			PolynomialComparator g = std::less<UPolynomial>()
			):
		polynomials(),
		eliminationOrder(g),
		liftingOrder(f),
		mSingleEliminationQueue(),
		mPairedEliminationQueue(),
		mLiftingQueue(),
		mLiftingQueueReset(),
		childrenPerParent(),
		parentsPerChild(),
		polynomialOwner(owner),
		bounded(false)
	{}
	
	///////////////
	// SELECTORS //
	///////////////
	
	/**
	 * Give all parents of the specified elimination polynomial.
	 * @param p
	 * @return list of all parents of the specified elimination polynomial
	 */
	std::list<const UPolynomial*> getParentsOf(const UPolynomial* p) const;

	/**
	 * Checks if the given elimination polynomial has non-trivial parents, i.e. if it has more than a single parent.
     * @param p Univariate polynomial
     * @return true, if the given polynomial has non-trivial parents.
     */
	bool hasParents(const UPolynomial* p) const;

	/**
	 * Retrieves the list of polynomials.
     * @return List of polynomials.
     */
	const PolynomialSet& getPolynomials() const {
		return this->polynomials;
	}

	/*
	 * Set a new order for the elimination queue.
	 * @param order New order function.
	 */
	void setEliminationOrder(PolynomialComparator order);
	/*
	 * Set a new order for the lifting queue.
	 * @param order New order function.
	 */
	void setLiftingOrder(PolynomialComparator order) {
		this->liftingOrder = order;
	}

	/**
	 * Returns the number of polynomials stored in this elimination set.
     * @return Number of polynomials.
     */
	long unsigned size() const {
		return this->polynomials.size();
	}

	/**
	 * Checks if there polynomials in this elimination set.
     * @return If no polynomials are present.
     */
	bool empty() const {
		return this->polynomials.empty();
	}

	/**
	 * Checks if this elimination set fulfills certain consistency constraints.
     * @return If this set is consistent.
     */
	bool isConsistent() const;

	////////////////////
	// ACCESS METHODS //
	////////////////////
	
	/**
	 * Inserts an elimination polynomial with the specified parent into the set.
	 * @param r elimination polynomial
	 * @param parents parents of the elimination (optional, standard is (0) ), if more than 1 parent is given, the list is interpreted as concatenation of parent pairs, e.g. (a, 0, b, c) defines  the parents (a) and (b,c).
	 * @param avoidSingle If true, the polynomial added is not added to the single-elimination queue (default: false).
	 * @return a pair, with its member pair::first set to an iterator pointing to either the newly inserted element or to the element that already had its same key in the map. The pair::second element in the pair is set to true if a new element was inserted or false if an element with the same key existed.
	 * @complexity amortized: logarithmic in the number of polynomials stored, worst: linear
	 * @see std::set::insert
	 */
	std::pair<typename PolynomialSet::iterator, bool> insert(
			const UPolynomial* r,
			const std::list<const UPolynomial*>& parents = std::list<const UPolynomial*>(),
			bool avoidSingle = false
			);
	
	/**
	 * Inserts an elimination polynomial with the specified parent into the set.
	 * @param r elimination polynomial
	 * @param parents parents of the elimination (optional, standard is (0) ), if more than 1 parent is given, the list is interpreted as concatenation of parent pairs, e.g. (a, 0, b, c) defines  the parents (a) and (b,c).
	 * @param avoidSingle If true, the polynomial added is not added to the single-elimination queue (default: false).
	 * @return a pair, with its member pair::first set to an iterator pointing to either the newly inserted element or to the element that already had its same key in the map. The pair::second element in the pair is set to true if a new element was inserted or false if an element with the same key existed.
	 * @complexity amortized: logarithmic in the number of polynomials stored, worst: linear
	 * @see std::set::insert
	 */
	std::pair<typename PolynomialSet::iterator, bool> insert(
			const UPolynomial& r,
			const std::list<const UPolynomial*>& parents = std::list<const UPolynomial*>(),
			bool avoidSingle = false
			) {
		return this->insert(this->polynomialOwner->take(new UPolynomial(r)), parents, avoidSingle);
	}
	
	/**
	 * Insert all polynomials from first to last (excl. last), while all have the same parent.
	 * @param first
	 * @param last
	 * @param parents parents of the elimination (optional, standard is one 0 parent)
	 * @param avoidSingle If true, all polynomials added are not added to the single-elimination queue (default: false).
	 * @return the list of polynomials actually added to the set, which might be smaller than the input set
	 */
	template<class InputIterator>
	std::list<const UPolynomial*> insert(
			InputIterator first,
			InputIterator last,
			const std::list<UPolynomial*>& parents = std::list<UPolynomial*>(),
			bool avoidSingle = false
			);
	
	/**
	 * Insert the contents of the set s into this set.
	 * @param s
	 * @param avoidSingle If true, all polynomials added are not added to the single-elimination queue (default: false).
	 * @return the list of polynomials actually added, which might be smaller than the input set
	 */
	std::list<const UPolynomial*> insert(
			const EliminationSet<Coefficient>& s,
			bool avoidSingle = false
			);
	/**
	 * Pretend to insert the contents of the set s into this set, but in fact, remove all polynomials <b>existing</b> in this set from s.
	 * @param s
	 * @return true if a change to s was committed
	 */
	bool insertAmend(EliminationSet<Coefficient>& s);
	
	/**
	 * Removes a polynomial p completely from the set (and all internal auxiliary structures).
	 * @param p polynomial to be removed from the set
	 * @return 1 if p exited in the set, 0 otherwise
	 * @complexity linear in the number of elimination polynomials one level above the level represented by this elimination set, i.e. the parents
	 */
	size_t erase(const UPolynomial* p);
	
	/** Asserts that parent is removed.
	 * Removes all elimination polynomials from the set which have parent as only parent (lone polynomials) or as only other parent (divorce-suffering polynomials).
	 * A list of those deleted is returned.
	 * Elimination polynomials which have several parents among which parent is are not deleted, but their auxiliary data structures are updated.
	 * @param parent of the lone elimination polynomials to be removed
	 * @return list of elimination polynomials removed
	 * @complexity linear in the number of elimination polynomials which do belong to the given parent
	 */
	std::forward_list<const UPolynomial*> removeByParent(const UPolynomial* parent);

	/**
	 * Searches the set entry for the given polynomial p if exists, otherwise nullptr.
	 * @param p
	 * @return set entry for the given polynomial p if exists, otherwise nullptr
	 */
	const UPolynomial* find(const UPolynomial* p);

	/**
	 * Remove every data from this set.
	 */
	void clear();
	
	/////////////////////////////////
	// LIFTING POSITION MANAGEMENT //
	/////////////////////////////////
	
	/**
	 * Retrieve the next position for lifting.
	 * The lifting positions are stored in the order of the set of elimination polynomials, but can lack polynomials which were already popped.
	 *
	 * If the lifting queue is empty the behavior of this method is undefined.
	 * @return the smallest (w.r.t. set order) elimination polynomial not yet considered for lifting
	 * @complexity constant
	 */
	const UPolynomial* nextLiftingPosition() {
		return this->mLiftingQueue.front();
	}

	/**
	 * Pop the polynomial returned by nextLiftingPosition() from the lifting position queue.
	 * The lifting positions are stored in the order of the set of elimination polynomials, but can lack polynomials which were already popped.
	 *
	 * If the lifting queue is empty the behavior of this method is undefined.
	 * @complexity constant
	 */
	void popLiftingPosition() {
		this->mLiftingQueue.pop_front();
	}

	/**
	 * Gives true if all were popped already, false if a lifting position exists.
	 * @return true if all were popped already, false if a lifting position exists
	 */
	bool emptyLiftingQueue() const {
		return this->mLiftingQueue.empty();
	}

	/**
	 * Gives true if the lifting position queue contains all elimination polynomials, false otherwise.
	 * @return true if the lifting position queue contains all elimination polynomials, false otherwise
	 */
	bool fullLiftingQueue() const {
		return this->mLiftingQueue.size() == this->polynomials.size();
	}

	/**
	 * Re-build the lifting position queue from scratch using all polynomials in the set. The reset state is <b>not</b> changed.
	 * @complexity linear in the number of polynomials stored
	 */
	void resetLiftingPositionsFully();
	
	/**
	 * Re-build the lifting position queue just with the polynomials stored as reset state.
	 * @complexity linear in the number of polynomials stored
	 */
	void resetLiftingPositions(bool resetFully) {
		if (resetFully) {
			this->mLiftingQueue.assign( this->polynomials.begin(), this->polynomials.end() );
			this->mLiftingQueue.sort( this->liftingOrder );
		} else {
			this->mLiftingQueue = this->mLiftingQueueReset;
		}
	}

	/**
	 * Defines the reset state for lifting positions as the current lifting positions queue and all polynomials inserted in the future.
	 */
	void setLiftingPositionsReset() {
		this->mLiftingQueueReset = this->mLiftingQueue;
	}
	
	/////////////////////////////////////
	// ELIMINATION POSITION MANAGEMENT //
	/////////////////////////////////////
	
	const UPolynomial* nextSingleEliminationPosition();

	/**
	 * Return the next position in the single-elimination queue and remove it from the queue.
	 *
	 * If the single-elimination queue is empty the behavior of this method is undefined.
	 * @return the next position in the single-elimination queue
	 */
	const UPolynomial* popNextSingleEliminationPosition();

	/**
	 * Gives true if all single eliminations are done.
	 * @return true if all single eliminations are done
	 */
	bool emptySingleEliminationQueue() const {
	   return mSingleEliminationQueue.empty();
	}

	/**
	 * Gives true if all paired eliminations are done, false otherwise.
	 * @return true if all eliminations are done, false otherwise.
	 */
	bool emptyPairedEliminationQueue() const {
	   return mPairedEliminationQueue.empty();
	}

	/**
	 * Does the elimination of the polynomial p and stores the resulting polynomials into the specified
	 * destination set.
	 *
	 * The polynomial is erased from all queues.
	 * @param p
	 * @param destination
	 * @param variable the main variable of the destination elimination set
	 * @param setting
	 * @return list of polynomials added to destination
	 */
	std::list<const UPolynomial*> eliminateInto(
			const UPolynomial* p,
			EliminationSet<Coefficient>& destination,
			Variable::Arg variable,
			const CADSettings& setting
			);
	
	
	/// Determine whether _p is constant and possibly move it to the destination set while popping it from _queue and removing it from _otherqueue. _p is inserted into destination with avoidSingle=_avoidSingle.
	std::list<const UPolynomial*> eliminateConstant(
			const UPolynomial* p,
			std::list<const UPolynomial*>& queue,
			std::list<const UPolynomial*>& otherqueue,
			bool avoidSingle,
			EliminationSet<Coefficient>& destination,
			Variable::Arg variable,
			const CADSettings& setting
			);
	
	/**
	 * Does the elimination of the next polynomial in the elimination queue and stores the resulting polynomials into the specified
	 * destination set.
	 *
	 * Remarks:
	 * <ul>
	 *  <li>The resulting polynomials are always made square-free and primitive.</li>
	 * </ul>
	 *
	 * After the elimination, the polynomial is popped from the elimination queue.
	 * @param destination
	 * @param variable the main variable of the destination elimination set
	 * @param setting special settings for simplifications etc.
	 * @param synchronous (We use the assumption that single eliminations could be done already so that we need to catch up with the paired ones.)
	 *                    If true, the next single elimination is done only if the next single and the next paired elimination polynomials are equal;
	 *                      otherwise only the next paired elimination is done.
	 *                      In the latter case, the resulting polynomials are added to the destination set avoiding single eliminations, i.e., these polynomials inherit their "asynchronicity".
	 *                    If false the queues are queried without synchronizing.
	 *                    Always: If all paired eliminations are done, the next single elimination is done.
	 * @return list of polynomials added to destination
	 */
	std::list<const UPolynomial*> eliminateNextInto(
			EliminationSet<Coefficient>& destination,
			Variable::Arg variable,
			const CADSettings& setting,
			bool synchronous = false
			);


	
	////////////////
	// OPERATIONS //
	////////////////
	
	/**
	 * Move all constant polynomials from this set to the other set. Delete all polynomials which do not contain any variable.
	 * @param to
	 * @param variable
	 */
	void moveConstants(EliminationSet<Coefficient>& to, Variable::Arg variable);

	/**
	 * Removes all constant elements.
	 * @complexity linear in the number of elements stored in the set
	 */
	void removeConstants();
	
	/**
	 * Removes all elements without real roots.
	 * @complexity linear in the number of elements stored in the set
	 */
	void removePolynomialsWithoutRealRoots();
	
	/**
	 * Replaces all polynomials by their pseudo-separable part (currently uses UnivariatePolynomial::pseudoSepapart).
	 * @complexity linear in the number of elements stored in the set
	 */
	void makeSquarefree();
	
	/**
	 * Replaces all polynomials by their primitive part (currently uses UnivariatePolynomial::pseudoPrimpart).
	 * @complexity linear in the number of elements stored in the set
	 */
	void makePrimitive();
	
	/**
	 * Replaces all polynomials by their factors (currently uses GiNaC::factor).
	 * In order to work properly, GiNaC::factor should receive integer polynomials. Thus, before the factorization, all polynomials are transformed to integral polynomials.
	 * @complexity linear in the number of elements stored in the set
	 */
	void factorize();
	
	template<typename Coeff>
	friend std::ostream& operator<<(std::ostream& os, const EliminationSet<Coeff>& s);
	
	/**
	 * Swaps the contents (all attributes) of the two EliminationSets.
	 * @see std::set::swap
	 */
	template<typename Coeff>
#ifdef __VS
	friend void swap(EliminationSet<Coeff>& lhs, EliminationSet<Coeff>& rhs);
#else
	friend void std::swap(EliminationSet<Coeff>& lhs, EliminationSet<Coeff>& rhs);
#endif
};

}
}

#ifndef __VS
namespace std {
/**
 * Swaps two elimination sets.
 * @param lhs First set.
 * @param rhs Second set.
 */
template<typename Coeff>
void swap(carl::cad::EliminationSet<Coeff>& lhs, carl::cad::EliminationSet<Coeff>& rhs);
}
#endif

#include "EliminationSet.tpp"
