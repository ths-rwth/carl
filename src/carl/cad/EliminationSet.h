/* 
 * @file   EliminationSet.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include <list>
#include <set>
#include <utility>
#include <unordered_map>
#include <unordered_set>

#include "../core/UnivariatePolynomial.h"
#include "../core/logging.h"

namespace carl {
namespace CAD {
	
/**
 * 
 */
	
/** Reminder
 * - SingleEliminationQueue p -> PairedEliminationQueue mit p und p'
 * - UnivariatePolynomialPtr sind Multivariat!
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
	typedef std::pair<UnivariatePolynomialPtr<Coefficient>, UnivariatePolynomialPtr<Coefficient>> PolynomialPair;
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
	
	/**
	 * A set of polynomials.
	 */
	typedef std::unordered_set<UnivariatePolynomialPtr<Coefficient>> PolynomialSet;

	/**
	 * A mapping from one polynomial to a sorted range of other polynomials.
	 */
	typedef std::unordered_map<UnivariatePolynomialPtr<Coefficient>, PolynomialSet> PolynomialBucketMap;
	
	/// set of elimination parents
	typedef std::set<PolynomialPair, PolynomialPairIsLess> parentbucket;
	/// mapping one polynomial pointer to a set of elimination parents
	typedef std::unordered_map<UnivariatePolynomialPtr<Coefficient>, parentbucket> parentbucket_map;

	
// public types
public:
	typedef bool (*PolynomialComparator)( const UnivariatePolynomialPtr<Coefficient>&, const UnivariatePolynomialPtr<Coefficient>& );

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
	std::list<UnivariatePolynomialPtr<Coefficient>> mSingleEliminationQueue;
	/**
	 * Elimination queue containing all polynomials not yet considered for paired elimination.
	 * Access permits reset of the queue, automatic update after insertion of new elements and a pop method.
	 */
	std::list<UnivariatePolynomialPtr<Coefficient>> mPairedEliminationQueue;
	
	/**
	 * Lifting queue containing all polynomials not yet considered for lifting.
	 * Access permits reset of the queue, automatic update after insertion of new elements and a pop method.
	 */
	std::list<UnivariatePolynomialPtr<Coefficient>> mLiftingQueue;
	/**
	 * Lifting queue containing a reset state for the lifting queue, which is a copy of the original container contents, but can be set to a concrete state
	 */
	std::list<UnivariatePolynomialPtr<Coefficient>> mLiftingQueueReset;
	
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
	
	//TODO: constructor
	
	std::list<UnivariatePolynomialPtr<Coefficient>&> getParentsOf(const UnivariatePolynomialPtr<Coefficient>& p) const;

	/**
	 * Checks if the given elimination polynomial has non-trivial parents, i.e. if it has more than a single parent.
     * @param p Univariate polynomial
     * @return true, if the given polynomial has non-trivial parents.
     */
	bool hasParents(const UnivariatePolynomialPtr<Coefficient>& p) const;
	
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
			const UnivariatePolynomialPtr<Coefficient> r,
			const std::list<UnivariatePolynomialPtr<Coefficient>>& parents = std::list<UnivariatePolynomialPtr<Coefficient>>( 1, UnivariatePolynomialPtr<Coefficient>() ),
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
	std::list<UnivariatePolynomialPtr<Coefficient>> insert(
			InputIterator first,
			InputIterator last,
			const std::list<UnivariatePolynomialPtr<Coefficient>>& parents = std::list<UnivariatePolynomialPtr<Coefficient>>( 1, UnivariatePolynomialPtr<Coefficient>() ),
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
			const std::list<UnivariatePolynomialPtr<Coefficient>>& parents = std::list<UnivariatePolynomialPtr<Coefficient>>( 1, UnivariatePolynomialPtr<Coefficient>()),
			bool avoidSingle = false
			);
	
	/**
	 * Insert the contents of the set s into this set.
	 * @param s
	 * @param avoidSingle If true, all polynomials added are not added to the single-elimination queue (default: false).
	 * @return the list of polynomials actually added, which might be smaller than the input set
	 */
	std::list<UnivariatePolynomialPtr<Coefficient>> insert(
			const EliminationSet<Coefficient>& s,
			bool avoidSingle = false
			);
	/**
	 * Pretend to insert the contents of the set s into this set, but in fact, remove all polynomials <b>existing</b> in this set from s.
	 * @param s
	 * @return true if a change to s was committed
	 */
	bool insertAmend(EliminationSet<Coefficient>& s);
};

}
}

#include "EliminationSet.tpp"