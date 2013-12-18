/*
 * @file   EliminationSet.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 * 
 * ported from GiNaCRA:
 *	commit 00ba79d7117be72882c5864d44ff644cf4ccc76f
 *	Author: Ulrich Loup <loup@cs.rwth-aachen.de>
 *	Date:   Tue Dec 3 15:57:06 2013 +0100
 */

#pragma once

#include <forward_list>
#include <list>
#include <set>
#include <utility>
#include <unordered_map>
#include <unordered_set>

#include "../core/UnivariatePolynomial.h"
#include "../core/logging.h"

#include "CADSettings.h"

namespace carl {
namespace cad {
	
/**
 * 
 */
	
/** Reminder
 * - SingleEliminationQueue p -> PairedEliminationQueue mit p und p'
 * - UnivariatePolynomial* sind Multivariat!
 * - Ein EliminiationSet pro Variable
 */
template<typename Coefficient>
class EliminationSet {
	
// private types
private:
	
	/**
	 * Represents a pair of polynomials. 
	 * Used to store the ancestors of a polynomial. If one is nullptr, the polynomial has only a single ancestor.
	 */
	typedef std::pair<UnivariatePolynomial<Coefficient>*, UnivariatePolynomial<Coefficient>*> PolynomialPair;
	
	/**
	 * Functor that compares two PolynomialPair objects.
     */
	struct PolynomialPairIsLess {
		unsigned int length(const PolynomialPair& p) {
			if (p.first == nullptr && p.second == nullptr) return 0;
			if (p.first == nullptr || p.second == nullptr) return 1;
			return 2;
		}
		bool operator()(const PolynomialPair& a, const PolynomialPair& b) {
			unsigned int l1 = this->length(a), l2 = this->length(b);
			if (l1 < l2) return true;
			if (l2 < l1) return false;
			if (less(a.first, b.first)) return true;
			if (a.first == b.first) return less(a.second, b.second);
			return false;
		}
	};
	
	struct PolynomialPairContains {
	private:
		const UnivariatePolynomial<Coefficient>* p;
	public:
		PolynomialPairContains(const UnivariatePolynomial<Coefficient>* p) : p(p) {
		}
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
	typedef std::unordered_set<const UnivariatePolynomial<Coefficient>*> PolynomialSet;

	/**
	 * A mapping from one polynomial to a sorted range of other polynomials.
	 */
	typedef std::unordered_map<const UnivariatePolynomial<Coefficient>*, PolynomialSet> PolynomialBucketMap;
	
	/// set of elimination parents
	typedef std::set<PolynomialPair, PolynomialPairIsLess> parentbucket;
	/// mapping one polynomial pointer to a set of elimination parents
	typedef std::unordered_map<const UnivariatePolynomial<Coefficient>*, parentbucket> parentbucket_map;

	
// public types
public:
	typedef UnivariatePolynomialComparator<Coefficient> PolynomialComparator;

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
	
	/**
	 * Elimination queue containing all polynomials not yet considered for non-paired elimination.
	 * Access permits reset of the queue, automatic update after insertion of new elements and a pop method.
	 */
	std::list<const UnivariatePolynomial<Coefficient>*> mSingleEliminationQueue;
	/**
	 * Elimination queue containing all polynomials not yet considered for paired elimination.
	 * Access permits reset of the queue, automatic update after insertion of new elements and a pop method.
	 */
	std::list<const UnivariatePolynomial<Coefficient>*> mPairedEliminationQueue;
	
	/**
	 * Lifting queue containing all polynomials not yet considered for lifting.
	 * Access permits reset of the queue, automatic update after insertion of new elements and a pop method.
	 */
	std::list<const UnivariatePolynomial<Coefficient>*> mLiftingQueue;
	/**
	 * Lifting queue containing a reset state for the lifting queue, which is a copy of the original container contents, but can be set to a concrete state
	 */
	std::list<const UnivariatePolynomial<Coefficient>*> mLiftingQueueReset;
	
	/// maps an entry from another EliminationSet (parent) to the entry of elimination polynomial belonging to the parent
	PolynomialBucketMap childrenPerParent;
	/// assigns to each elimination polynomial its parents
	parentbucket_map parentsPerChild;
	
// public members
public:
	
	
	/*
	 * This flag indicates whether this elimination set contains polynomials that are only valid within certain bounds.
	 */
	bool bounded;
	
	/**
	 * Constructs the elimination set with a given strict ordering f of univariate polynomials.
	 * @param f strict ordering for lifting queue (standard is UnivariatePolynomial::univariatePolynomialIsLess)
	 * @param g strict ordering for elimination queue (standard is UnivariatePolynomial::univariatePolynomialIsLess)
	 */
	EliminationSet(
			PolynomialComparator f = UnivariatePolynomialComparator<Coefficient>(),
			PolynomialComparator g = UnivariatePolynomialComparator<Coefficient>()
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
		bounded(false)
	{}

	/**
	 * Copy constructor.
	 * @param s
	 * @return
	 */
	EliminationSet(const EliminationSet<Coefficient>& s):
		polynomials(s.polynomials),
		eliminationOrder(s.eliminationOrder),
		liftingOrder(s.liftingOrder),
		mSingleEliminationQueue(s.mSingleEliminationQueue),
		mPairedEliminationQueue(s.mPairedEliminationQueue),
		mLiftingQueue(s.mLiftingQueue),
		mLiftingQueueReset(s.mLiftingQueueReset),
		childrenPerParent(s.childrenPerParent),
		parentsPerChild(s.parentsPerChild),
		bounded(s.bounded)
	{}
	
	///////////////
	// SELECTORS //
	///////////////
	
	std::list<UnivariatePolynomial<Coefficient>*> getParentsOf(const UnivariatePolynomial<Coefficient>* p) const;

	/**
	 * Checks if the given elimination polynomial has non-trivial parents, i.e. if it has more than a single parent.
     * @param p Univariate polynomial
     * @return true, if the given polynomial has non-trivial parents.
     */
	bool hasParents(const UnivariatePolynomial<Coefficient>* p) const;
	
	/*
	 * Set a new order for the elimination queue.
	 * @param order New order function.
	 */
	void setEliminationOrder( PolynomialComparator order );
	/*
	 * Set a new order for the lifting queue.
	 * @param order New order function.
	 */
	void setLiftingOrder( PolynomialComparator order );

	long unsigned size() const {
		return this->polynomials.size();
	}

	bool empty() const {
		return this->polynomials.empty();
	}

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
			const UnivariatePolynomial<Coefficient>* r,
			const std::list<UnivariatePolynomial<Coefficient>*>& parents = std::list<UnivariatePolynomial<Coefficient>*>(),
			bool avoidSingle = false
			);
	
	/**
	 * Insert all polynomials from first to last (excl. last), while all have the same parent.
	 * @param first
	 * @param last
	 * @param parents parents of the elimination (optional, standard is one 0 parent)
	 * @param avoidSingle If true, all polynomials added are not added to the single-elimination queue (default: false).
	 * @return the list of polynomials actually added to the set, which might be smaller than the input set
	 */
	template<class InputIterator>
	std::list<UnivariatePolynomial<Coefficient>*> insert(
			InputIterator first,
			InputIterator last,
			const std::list<UnivariatePolynomial<Coefficient>*>& parents = std::list<UnivariatePolynomial<Coefficient>*>(),
			bool avoidSingle = false
			);
	
	/**
	 * Insert an object which is allocated newly and stored as its new pointer value.
	 * @param r
	 * @param parents parents of the elimination (optional, standard is one 0 parent)
	 * @param avoidSingle If true, all polynomials added are not added to the single-elimination queue (default: false).
	 * @return a pair, with its member pair::first set to an iterator pointing to either the newly inserted element or to the element that already had its same key in the map. The pair::second element in the pair is set to true if a new element was inserted or false if an element with the same key existed.
	 * @complexity logarithmic in the number of polynomials stored
	 * @see insert
	 */
	std::pair<typename PolynomialSet::iterator, bool> insert(
			const UnivariatePolynomial<Coefficient>& r,
			const std::list<UnivariatePolynomial<Coefficient>*>& parents = std::list<UnivariatePolynomial<Coefficient>*>(),
			bool avoidSingle = false
			);
	
	/**
	 * Insert the contents of the set s into this set.
	 * @param s
	 * @param avoidSingle If true, all polynomials added are not added to the single-elimination queue (default: false).
	 * @return the list of polynomials actually added, which might be smaller than the input set
	 */
	std::list<UnivariatePolynomial<Coefficient>*> insert(
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
	size_t erase(const UnivariatePolynomial<Coefficient>* p);
	
	/** Asserts that parent is removed.
	 * Removes all elimination polynomials from the set which have parent as only parent (lone polynomials) or as only other parent (divorce-suffering polynomials).
	 * A list of those deleted is returned.
	 * Elimination polynomials which have several parents among which parent is are not deleted, but their auxiliary data structures are updated.
	 * @param parent of the lone elimination polynomials to be removed
	 * @return list of elimination polynomials removed
	 * @complexity linear in the number of elimination polynomials which do belong to the given parent
	 */
	std::forward_list<const UnivariatePolynomial<Coefficient>*> removeByParent(const UnivariatePolynomial<Coefficient>* parent);

	/**
	 * Searches the set entry for the given polynomial p if exists, otherwise nullptr.
	 * @param p
	 * @return set entry for the given polynomial p if exists, otherwise nullptr
	 */
	const UnivariatePolynomial<Coefficient>* find(const UnivariatePolynomial<Coefficient>* p);
	
	/**
	 * Swaps the contents (all attributes) of the two EliminationSets.
	 * @see std::set::swap
	 */
	friend void swap(EliminationSet<Coefficient>& lhs, EliminationSet<Coefficient>& rhs);

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
	const UnivariatePolynomial<Coefficient>* nextLiftingPosition() {
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
	void resetLiftingPositions() {
		this->mLiftingQueue = this->mLiftingQueueReset;
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
	
	/**
	 * Return the next position in the single-elimination queue and remove it from the queue.
	 *
	 * If the single-elimination queue is empty the behavior of this method is undefined.
	 * @return the next position in the single-elimination queue
	 */
	const UnivariatePolynomial<Coefficient>* popNextSingleEliminationPosition();

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
	std::list<UnivariatePolynomial<Coefficient>*> eliminateInto(
			const UnivariatePolynomial<Coefficient>* p,
			EliminationSet<Coefficient>& destination,
			const Variable& variable,
			const CADSettings& setting
			);
	
	
	/// Determine whether _p is constant and possibly move it to the destination set while popping it from _queue and removing it from _otherqueue. _p is inserted into destination with avoidSingle=_avoidSingle.
	std::list<UnivariatePolynomial<Coefficient>*> eliminateConstant(
			const UnivariatePolynomial<Coefficient>* p,
			std::list<UnivariatePolynomial<Coefficient>*>& queue,
			std::list<UnivariatePolynomial<Coefficient>*>& otherqueue,
			bool avoidSingle,
			EliminationSet<Coefficient>& destination,
			const Variable& variable,
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
	std::list<UnivariatePolynomial<Coefficient>*> eliminateNextInto(
			EliminationSet<Coefficient>& destination,
			const Variable& variable,
			const CADSettings& setting,
			bool synchronous = true
			);


	
	////////////////
	// OPERATIONS //
	////////////////
	
	/**
	 * Move all constant polynomials from this set to the other set. Delete all polynomials which do not contain any variable.
	 * @param to
	 * @param variable
	 */
	void moveConstants(EliminationSet<Coefficient>& to, const Variable& variable);

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
	
	friend std::ostream& operator<<(std::ostream& os, const EliminationSet<Coefficient>& s);
	
	//////////////////////////////
	// STATIC AUXILIARY METHODS //
	//////////////////////////////
	
	/**
	 * Generates the set of truncations of the polynomial.
	 * @see "Algorithms in Real Algebraic Geometry" - Saugata Basu, Richard Pollack, Marie-Francoise Roy see page 21-22
	 * @param p The polynomial
	 * @complexity O ( p.deg() )
	 * @return The set of truncations
	 */
	static std::list<UnivariatePolynomial<Coefficient>*> truncation(const UnivariatePolynomial<Coefficient>* p);
	
	/**
	 * Performs all steps of a CAD elimination/projection operator which are related to one single polynomial.
	 * This elimination operator follows McCallums definition [McCallum - TR578, 1985].
	 *
	 *<p><strong>Note that the set returned by this method should be disjoint to the set returned by the two-polynomial variant of <code>eliminate</code>.</strong></p>
	 *
	 * Optimizations:
	 * <ul>
	 * <li> No numeric elimination polynomials are inserted into eliminated. </li>
	 * </ul>
	 *
	 * @param p input polynomial for the elimination procedure
	 * @param variable the new main variable for the returned set
	 * @param eliminated the set of eliminated polynomials to be augmented by the result of the elimination
	 * @param avoidSingle If true, the polynomial added to eliminated is not added to the single-elimination queue (default: false).
	 * @complexity O ( deg(P) ) subresultant computations. The degree of the output is bound by O(deg(P)^2)!
	 * @return a list of polynomials in which the main variable of p is eliminated
	 */
	static void elimination(
			const UnivariatePolynomial<Coefficient>* p,
			const Variable& variable,
			EliminationSet<Coefficient>& eliminated,
			bool avoidSingle
			);

	/**
	 * Performs all steps of a CAD elimination/projection operator which are related to a pair of polynomials.
	 * This elimination operator follows McCallums definition [McCallum - TR578, 1985].
	 *
	 *<p><strong>Note that the set returned by this method should be disjoint to the set returned by the single-polynomial variant of <code>eliminate</code>.</strong></p>
	 *
	 * Optimizations:
	 * <ul>
	 * <li> No numeric elimination polynomials are inserted into eliminated. </li>
	 * </ul>
	 *
	 * @param p first input polynomial for the elimination procedure
	 * @param q second input polynomial for the elimination procedure
	 * @param variable the new main variable for the returned set
	 * @param eliminated the set of eliminated polynomials to be augmented by the result of the elimination
	 * @param avoidSingle If true, the polynomials added to eliminated are not added to the single-elimination queue (default: false).
	 * @complexity O( deg(P)^2 ) subresultant computations. The degree of the output is bound by O(max(deg(P),deg(Q))^2)!
	 * @return a list of polynomials in which the main variable of p1 and p2 is eliminated
	 */
	static void elimination(
			const UnivariatePolynomial<Coefficient>* p,
			const UnivariatePolynomial<Coefficient>* q,
			const Variable& variable,
			EliminationSet<Coefficient>& eliminated,
			bool avoidSingle 
			);
	
	/**
	 * Performs all steps of a CAD elimination/projection operator which are related to one single polynomial.
	 *
	 * Optimizations:
	 * <ul>
	 * <li> No numeric elimination polynomials are inserted into eliminated. </li>
	 * </ul>
	 *
	 * @param p input polynomial for the elimination procedure constrained by an equation
	 * @param variable the new main variable for the returned set
	 * @param eliminated the set of eliminated polynomials to be augmented by the result of the elimination
	 * @param avoidSingle If true, the polynomial added to eliminated is not added to the single-elimination queue (default: false).
	 * @complexity
	 * @see Scott McCallum - On Projection in CAD-Based Quantifier Elimination with Equational Constraint
	 * @return a list of polynomials in which the main variable of p is eliminated
	 */
	static void eliminationEq(
			const UnivariatePolynomial<Coefficient>* p,
			const Variable& variable,
			EliminationSet<Coefficient>& eliminated,
			bool avoidSingle
			)
	{
		elimination(p, variable, eliminated, avoidSingle);
	}
	
	/**
	 * Performs all steps of a CAD elimination/projection operator which are related to a pair of polynomials.
	 *
	 * Optimizations:
	 * <ul>
	 * <li> No numeric elimination polynomials are inserted into eliminated. </li>
	 * </ul>
	 *
	 * @param p first input polynomial for the elimination procedure constrained by an equation
	 * @param q second input polynomial for the elimination procedure
	 * @param variable the new main variable for the returned set
	 * @param eliminated the set of eliminated polynomials to be augmented by the result of the elimination
	 * @param avoidSingle If true, the polynomial added to eliminated is not added to the single-elimination queue (default: false).
	 * @complexity
	 * @see Scott McCallum - On Projection in CAD-Based Quantifier Elimination with Equational Constraint
	 * @return a list of polynomials in which the main variable of p1 and p2 is eliminated
	 */
	static void eliminationEq(
			const UnivariatePolynomial<Coefficient>* p,
			const UnivariatePolynomial<Coefficient>* q,
			const Variable& variable,
			EliminationSet<Coefficient>& eliminated,
			bool avoidSingle
			)
	{
		elimination(p, q, variable, eliminated, avoidSingle);
	}
};

}
}

#include "EliminationSet.tpp"