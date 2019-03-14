/**
 * @file CAD.h
 * @ingroup cad
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 * 
 * ported from GiNaCRA:
 *	commit 00ba79d7117be72882c5864d44ff644cf4ccc76f
 *	Author: Ulrich Loup <loup@cs.rwth-aachen.de>
 *	Date:   Tue Dec 3 15:57:06 2013 +0100
 */

/// @todo Fix bug in CAD concerning CAD::removePolynomial().
/*
 * beim Aufschreiben des CAD::removePolynomial-Algorithmus' ist mir aufgefallen, 
 * dass darin ein Teil zu Problemen führen kann, sobald ein Polynom entfernt 
 * wird, während die Elimination nicht vollständig durchgeführt worden war:
 * 
 * Falls ein Eliminationslevel leer ist, nehme ich an, dass die Variable nicht 
 * mehr gebraucht wird und beschneide entsprechend den Sample-Tree. Das ist im 
 * Falle einer unvollständigen Elimination falsch.
 * 1. Lösung: Bestimme a.H.v. den Eingabepolynomen, ob tatsächlich die 
 * vermeintlich überflüssige Variable nicht mehr vorkommt, bevor der Sample-Tree 
 * beschnitten wird.
 * 2. Lösung: Entferne nie Variablen (u.U. ineffizient).
 * 
 * Als schnellen Fix könntest du den Teil in CAD::removePolynomial(pPtr, level, 
 * childrenOnly), der den Sample-Tree löscht, auskommentieren.
 */

#pragma once

#include <atomic>
#include <list>
#include <memory>
#include <unordered_map>
#include <vector>

#include <carl/core/UnivariatePolynomial.h>
#include <carl/core/MultivariatePolynomial.h>
#include <carl/core/Variable.h>
#include <carl/formula/model/ran/RealAlgebraicNumber.h>
#include <carl/formula/model/ran/RealAlgebraicPoint.h>
#include <carl/util/carlTree.h>

#include "CADConstraints.h"
#include "CADPolynomials.h"
#include "CADTypes.h"
#include "CADSettings.h"
#include "ConflictGraph.h"
#include "Constraint.h"
#include "EliminationSet.h"
#include "SampleSet.h"
#include "Variables.h"

namespace carl {

/**
 * This class implements the core of the CAD algorithm.
 */
template<typename Number>
class CAD {
public:
	/// Type of univariate polynomials.
	typedef typename cad::CADPolynomials<Number>::UPolynomial UPolynomial;
	/// Type of multivariate polynomials.
	typedef typename cad::CADPolynomials<Number>::MPolynomial MPolynomial;

	typedef tree<RealAlgebraicNumber<Number>> Tree;
	/// Type of an iterator over the samples.
	typedef typename Tree::iterator sampleIterator;
	typedef typename Tree::template LeafIterator<false> LeafIterator;
	/// Type of a map of variable bounds.
	typedef std::unordered_map<std::size_t, Interval<Number>> BoundMap;
private:
	
	cad::Variables mVariables;
	
	/**
	 * Sample components built during the CAD lifting arranged in a tree.
	 */
	carl::tree<RealAlgebraicNumber<Number>> sampleTree;

	/**
	 * Lists of polynomials occurring in every elimination level (immutable; new polynomials are appended at the tail)
	 */
	std::vector<cad::EliminationSet<Number>> eliminationSets;
	
	/**
	 * Stores the original polynomials and the queue of polynomials that are scheduled.
	 */
	cad::CADPolynomials<Number> polynomials;
	
	/**
	 * flag indicating whether the sample construction is completed or not
	 */
	bool iscomplete;
	
	/**
	 * flag indicating whether the check procedure terminated with true because of an interrupt
	 */
	bool interrupted;
	
	/**
	 * vector of Booleans: If any of them is true, we have to terminate a running check procedure.
	 */
	std::vector<std::atomic_bool*> interrupts;
	
	/**
	 * setting for internal heuristics
	 */
	cad::CADSettings setting;
	
	cad::CADConstraints<Number> mConstraints;
	
	static unsigned checkCallCount;

public:
	//////////////////////////////////
	// Constructors and Destructors //
	//////////////////////////////////

	/*
	 * Standard constructor doing nothing.
	 */
	CAD();

	/*
	 * Constructor that delegates the ownership of polynomials to the given parent.
	 */
	CAD(cad::PolynomialOwner<Number>* parent);

	/*
	 * Constructor just passing interruption flags.
	 * @param interruptionFlags
	 */
	CAD(const std::vector<std::atomic_bool*>& interruptionFlags);

	/**
	 * Constructor just passing a specific setting and initializing everything empty.
	 * @param setting
	 */
	CAD(const cad::CADSettings& setting);

	/**
	 *
	 * @param s input polynomials whose solution space is covered by this cad.
	 * @param v main symbols of the polynomials in desired order of elimination (lifting order is vice versa!)
	 * @param setting a setting type for a collection of CAD settings (standard option is the standard option of CADSettings::getSettings( ))
	 * @complexity linear in the size of v and s
	 */
	CAD(const std::list<const UPolynomial*>& s, const std::vector<Variable>& v, const cad::CADSettings& setting = cad::CADSettings::getSettings());

	/**
	 *
	 * @param s input polynomials whose solution space is covered by this cad.
	 * @param v main symbols of the polynomials in desired order of elimination (lifting order is vice versa!)
	 * @param c vector of Booleans: if any of them is true, we have to terminate a running check procedure
	 * @param setting a setting type for a collection of CAD settings (standard option is the standard option of CADSettings::getSettings( ))
	 * @complexity linear in the size of v and s
	 */
	CAD(const std::list<const UPolynomial*>& s, const std::vector<Variable>& v, const std::vector<std::atomic_bool*>& c, const cad::CADSettings& setting = cad::CADSettings::getSettings());

	/**
	 * Copy constructor.
	 */
	CAD(const CAD& cad);
	
	///////////////
	// Selectors //
	///////////////
	
	/**
	* @return the current setting of the CAD object
	*/
	const cad::CADSettings& getSetting() const {
		return this->setting;
	}
	
	const Tree& getSampleTree() const {
		return sampleTree;
	}

	/**
	* @return list of main variables of the polynomials of this cad
	*/
	const std::vector<Variable>& getVariables() const {
		return mVariables.getCurrent();
	}
	
	/**
	 * Sets with successively eliminated variables due to a CAD projection. Index i corresponds to the set where i variables were eliminated.
	 * <br/ >For i>0, the i-th set was obtained by eliminating the variable i-1.
	 * @return all eliminations of the polynomials and the polynomials themselves (at position 0) computed so far
	 */
	const std::vector<cad::EliminationSet<Number>>& getEliminationSets() const {
		return this->eliminationSets;
	}

	const cad::EliminationSet<Number>& getEliminationSet(std::size_t n) const {
		return this->eliminationSets[n];
	}

	/**
	 * @return true if the cad is computed completely, false if there are still samples to compute
	 */
	bool isComplete() const {
		return this->iscomplete;
	}

	/**
	 * @return true if the check procedure terminated with true because of an interrupt
	 * The interrupted flag is cleared every time the check method is called
	 */
	bool isInterupted() const {
		return this->interrupted;
	}
	
	/**
	 * Collects all samples which were constructed for this sample tree node.
	 * @return all samples for this sample tree node (all children)
	 */
	cad::SampleSet<Number> samplesAt(const sampleIterator& node) const;
	
	/**
	 * Generates all real algebraic points of cad cells computed so far. Incomplete sample computations are completed with zero entries.
	 * @return all samples computed so far
	 */
	std::vector<RealAlgebraicPoint<Number>> samples() const;

	
	///////////////
	// Operators //
	///////////////

	/**
	 * Prints the current sample tree to the designated output stream.
	 * @param os output stream (standard value is <code>std::cout</code>)
	 */
	void printSampleTree(std::ostream& os = std::cout) const;

	/**
	 * Prints the current sample tree to the designated output stream.
	 * @param constraints
	 * @param filename
	 */
	void printConstraints(const std::string& filename = cad::DEFAULT_CAD_OUTPUTFILE) const;

	template<typename Num>
	friend std::ostream& operator<<(std::ostream& os, const CAD<Num>& cad);

	
	//////////////////////////////
	// Operations on CAD object //
	//////////////////////////////
	
	/**
	 * Prepares all elimination steps with the scheduled polynomials.
	 *
	 * Remarks:
	 * <ul>
	 *  <li>The polynomials of the starting level are always made square-free and primitive.</li>
	 * </ul>
	 * @return true if new variables were added, false otherwise
	 * @complexity linear in variables.size()
	 */
	bool prepareElimination();

	/**
	 * Clear any data related to elimination stored in the CAD object without changing the setting.
	 */
	void clearElimination();

	/**
	 * Run all possible elimination steps at once.
	 * @param bounds During elimination, remove all polynomials which do not vanish within the bounds (default: no bounds).
	 * @complexity O( 2^(2^mVariables.size()) )
	 */
	void completeElimination(const BoundMap& bounds = BoundMap());

	/**
	 * Clear any data stored in the CAD object without changing the setting.
	 */
	void clear();

	/**
	 * Computes all samples in this cad.
	 */
	void complete();
	
	void tryEquationSeparation(bool useBounds, bool onlyStrictBounds);
	
	
	/**
	 * Checks an arbitrary constraint for satisfiability on this set of samples. The cad is extended if there are still samples not computed.
	 * Remarks:
	 * <ul>
	 * <li>If all input constraints are build upon factors of the polynomials underlying this cad, then <code>check</code> returns a sound and complete result.</li>
	 * <li>If there is a constraint whose polynomial is no factor of the polynomials underlying this cad, then only soundness is guaranteed.</li>
	 * <li>If, in turn. there is a cad polynomial which does not belong to any constraint, the desired search heuristics might be very inefficient because they traverse useless samples. Thus, as a general rule, <b>avoid checking a set of constraints on a cad not build upon the exact corresponding set of constraint polynomials</b>.</li>
	 * <li>If the method gets an empty set of constraints but bounds defined, then the bounds are just checked for consistency but no solution point is computed, which is corresponding to the empty list of variables.
	 * </ul>
	 * @param constraints conjunction of input constraints, which might get altered in their variable order
	 * @param r contains a satisfying sample if the constraints are satisfiable by the cad, undefined otherwise (also if no constraint is given)
	 * @param conflictGraph contains a conflict graph if the input problem was unsatisfiable. The conflict graph has all constraints as vertices
	 * which were false on the points satisfying the last constraint. The edges are the aforementioned points so that two vertices are connected
	 * iff they where both false on a point satisfying the last constraint. Assuming, the last constraint is part of any reason of the
	 * conflict, then the conflict graph can be used to derive the minimal reason. The conflict graph is represented as adjacency matrix here, where
	 * the vertices are represented by their indices in the input vector.
	 *
	 * If true is returned, the value of conflictGraph is undefined.
	 * 
	 * @param bounds initial bounds for the variables represented by their index.
	 * The list is re-assigned by the method: if the result is true, the list contains a refined initial assignment.
	 * If the result is false, the list contains bounds for some variables. The box spanned by these bounds form a domain where no point satisfies the constraints.
	 * @param next If set to true the method tries to compute a sample which was not found in previous runs before (if earlyLiftingPruning is on). If set to false, all
	 * previously computed samples are traversed first and then lifting is continued.
	 * @param checkBounds
	 * @return true if the constraints are satisfied by a cell in the cad or no constraint is given and the bounds are not conflicting, false otherwise
	 */
	cad::Answer check(	std::vector<cad::Constraint<Number>>& constraints,
				RealAlgebraicPoint<Number>& r,
				cad::ConflictGraph<Number>& conflictGraph,
				BoundMap& bounds,
				bool next = false,
				bool checkBounds = true );

	/// Reduced-parameter version of CAD::check.
	cad::Answer check(	std::vector<cad::Constraint<Number>>& _constraints,
				RealAlgebraicPoint<Number>& r,
				BoundMap& bounds,
				bool next = false,
				bool checkBounds = true)
	{
		cad::ConflictGraph<Number> cg;
		return this->check(_constraints, r, cg, bounds, next, checkBounds);
	}

	/**
	 * Insert the given polynomial into the cad.
	 * Creates a copy from the given polynomial.
	 *
	 * @param p polynomial to be added
	 * @param v the polynomial's variables (parameters and main variable)
	 * @complexity quadratic in the number of the variables and linear in the number of polynomials
	 */
	void addPolynomial(const MPolynomial& p, const std::vector<Variable>& v);
	
	/**
	 * Removes a polynomial from the first elimination level where it occurs and possibly from the list of scheduled polynomials.
	 * Moreover, all elimination levels are safely cleaned of all elimination polynomials stemming from p.
	 * @param polynomial Polynomial to be removed
	 */
	void removePolynomial(const MPolynomial& polynomial);

	/**
	 * Removes a polynomial by its pointer pPtr from the input polynomials of the CAD (elimination level 0) or the specified level.
	 * Moreover, all elimination levels are safely cleaned of all elimination polynomials stemming from p.
	 * @param p
	 * @param level
	 * @param childrenOnly only remove the children of pPtr (recursively)
	 */
	void removePolynomial(const UPolynomial* p, unsigned level = 0, bool childrenOnly = false);
	
	/**
	 * Get the boundaries of the cad cell intervals in each level for the solution point r.
	 * @param r
	 * @return one Interval per variable index, representing the bounds of r in the respective level of the sample tree
	 */
	std::vector<Interval<Number>> getBounds(const RealAlgebraicPoint<Number>& r) const;
	
	///////////////////////////
	// PUBLIC STATIC METHODS //
	///////////////////////////

	/**
	 * Constructs the samples at the base level of a CAD construction, provided a set of prevailing samples.
	 * This method only returns samples which are new, i.e. not contained in currentSamples. The new samples are already added to currentSamples.
	 * @param roots list of real roots
	 * @param currentSamples samples already present where the new samples shall be integrated. The new samples are already added to currentSamples.
	 * @param replacedSamples samples being replaced in currentSamples (due to simplification or root preference) but not added to the new samples
	 * @param bounds the resulting sample set does not contain any samples outside the given bounds (standard: no bounds)
	 * @return a set of sample points for the given univariate polynomial sorted in ascending order.
	 * @complexity linear in <code>roots.size()</code>
	 */
	cad::SampleSet<Number> samples(
			std::size_t openVariableCount,
			const std::list<RealAlgebraicNumber<Number>>& roots,
			cad::SampleSet<Number>& currentSamples,
			std::forward_list<RealAlgebraicNumber<Number>>& replacedSamples,
			const Interval<Number>& bounds = Interval<Number>::unboundedInterval()
	);

	/**
	* Constructs the samples for <code>p</code> given the sample values <code>sample</code> with their corresponding variables for the coefficient polynomials.
	* @param p univariate polynomial with coefficients in the given variables. <code>p</code> is univariate in a variable not contained in <code>variables</code>.
	* @param sample list of sample components in order corresponding to the variables
	* @param variables variables of the coefficients of p
	* @param currentSamples samples already present where the new samples shall be integrated. Each new sample is automatically inserted in this list.
	* @param replacedSamples samples being replaced in currentSamples (due to simplification or root preference) but not added to the new samples
	* @param bounds the resulting sample set does not contain any samples outside the given bounds (standard: no bounds)
	* @return a set of sample points for the given univariate polynomial
	* @complexity linear in the number of roots of <code>p</code> plus the complexity of <code>RealAlgebraicNumberFactory::realRoots( p )</code>
	*/
	cad::SampleSet<Number> samples(
			std::size_t openVariableCount,
			const UPolynomial* p,
			sampleIterator node,
			cad::SampleSet<Number>& currentSamples,
			std::forward_list<RealAlgebraicNumber<Number>>& replacedSamples,
			const Interval<Number>& bounds = Interval<Number>::unboundedInterval()
	);

	/**
	* Computes a variable order from the given range of variables [firstVariable, lastVariable[
	* based on a Greedy algorithm (see below) working on the given range of polynomials [firstPolynomial, lastPolynomial[.
	*
	* @param firstVariable first variable to be considered in the given range
	* @param lastVariable iterator to the last element of the range not excluded in the range itself
	* @param firstPolynomial first polynomial to be considered in the given range
	* @param lastPolynomial iterator to the last element of the range not excluded in the range itself
	* @return a variable order with statistically good degree and size bounds
	* @see Dolzmann Seidl Sturm - Efficient Projection Orders for CAD
	* @complexity cubic in the number of variables
	*/
	template<class VariableIterator, class PolynomialIterator>
	std::vector<Variable> orderVariablesGreedily(
			VariableIterator firstVariable,
			VariableIterator lastVariable,
			PolynomialIterator firstPolynomial,
			PolynomialIterator lastPolynomial
	);

	////////////////
	// Heuristics //
	////////////////

	/**
	* Change the setting of the current CAD object performing all necessary changes such as lifting position reset.
	* @param setting
	*/
	void alterSetting(const cad::CADSettings& setting);
	
private:
	///////////////////////
	// AUXILIARY METHODS //
	///////////////////////
	
	bool integerHeuristicActive(cad::IntegerHandling heuristic, std::size_t variable) const {
		if (this->setting.integerHandling != heuristic) return false;
		return mVariables[variable].getType() == VariableType::VT_INT;
	}
	
	/**
	 * Constructs the path from the given node to the root and conjoins all RealAlgebraicNumbers on the nodes of the path.
	 *
	 * Remarks:
	 * <ul>
	 * <li> The path computed could be incomplete, i.e., it is not of the same length as variables exist. </li>
	 * <li> The path computed could contain only roots or only intermediate points depending on the settings. </li>
	 * </ul>
	 *
	 * @param node
	 * @param root of the sample tree
	 * @return RealAlgebraicPoint as list belonging to leaf
	 */
	std::list<RealAlgebraicNumber<Number>> constructSampleAt(sampleIterator node, const sampleIterator& root) const;

	/**
	 * Helper method for mainCheck() routine.
	 * 
	 * Search for a satisfying sample at _node, doing either a full lifting (_fullrestart=true) or use
	 * the current lifting queue (if the sample point still has to be lifted), and exclude a previously found sample point
	 * if _excludePrevious is set (then execute continue). Also update the trace if _updateTrace is set.
     * @param node
     * @param fullRestart
     * @param excludePrevious
	 * @param bounds
	 * @param r
	 * @param conflictGraph
	 * @param boundsNontrivial
	 * @param checkBounds
	 * @param dim
     * @return 
     */
	enum CheckNodeResult { CNR_SKIP, CNR_TRUE, CNR_FALSE, CNR_UNKNOWN};
	CheckNodeResult checkNode(
		sampleIterator node,
		bool fullRestart,
		bool excludePrevious,
		BoundMap& bounds,
		RealAlgebraicPoint<Number>& r,
		cad::ConflictGraph<Number>& conflictGraph,
		bool boundsNontrivial,
		bool checkBounds,
		std::size_t dim
	);
	
	/**
	 * Auxiliary check method implementing a check strategy in several phases. The arguments are the same as in CAD::check.
	 *
	 * Phase 1: Try to lift every sample ending in a node of the trace starting from the topmost node.
	 * Phase 2: Search the sample tree for already satisfying samples and lift the samples not yet lifted to the full dimension (all possibly within given bounds).
	 * Phase 3: Lift at those sample tree nodes where lifting is still possible (possibly within given bounds).
	 *
	 * @param bounds
	 * @param r
	 * @param conflictGraph
	 * @param next
	 * @param boundsNontrivial true if there are non-trivial bounds defined
	 * @param checkBounds
	 * @return
	 */
public:
	cad::Answer mainCheck(
			BoundMap& bounds,
			RealAlgebraicPoint<Number>& r,
			cad::ConflictGraph<Number>& conflictGraph,
			bool next,
			bool boundsNontrivial,
			bool checkBounds
	);
	
	/**
	 * Helper method for liftCheck().
	 * 
	 * Stores _newSample as a child of _node in mSampleTree in order.
	 * A possibly existing equal sample is replaced.
	 * The new child node is stored in and declared as newNode.
     * @param newSample
     * @param node
     */
	sampleIterator storeSampleInTree(RealAlgebraicNumber<Number> newSample, sampleIterator node);
	
	cad::Answer baseLiftCheck(
		sampleIterator node,
		RealAlgebraicPoint<Number>& r,
		cad::ConflictGraph<Number>& conflictGraph
	);
	
	cad::Answer partialLiftCheck(
		sampleIterator node,
		cad::ConflictGraph<Number>& conflictGraph
	);

	/**
	 * Constructs sample points for the given number of open variables openVariableCount by lifting
	 * the polynomials available in the lifting queue in the corresponding level of CAD::eliminationSets.
	 * The method performs every lifting for the given number of open variables and stops if either a satisfying
	 * sample point is found (in case of which already computed sample components are stored in the sample tree),
	 * or there is no lifting position available any more for any number of open variables less then or equal openVariableCount.
	 *
	 * Remarks:
	 * <ul>
	 *  <li>The method computes all samples for the specified node of the sample tree by using the specified lifting positions. </li>
	 *  <li>If bounds are specified or the settings demand it, not all samples computed could be used for lifting themselves, but they are just stored.</li>
	 * </ul>
	 *
	 * @param node of the current level (initiate with child of mSampleTree root)
	 * @param openVariableCount number of variables still to be substituted by lifting
	 * @param restartLifting If set to true enables the method to use all elimination polynomials of the respective level for lifting; if set to false, lifting is just performed by the current lifting position queue for this level. Setting this to true is standard as it is used when the method calls itself recursively for the next level. The other case can be used to perform only partial lifting from a specific point.
	 * @param sample list of sample components in order corresponding to the variables. The sample values (and the corresponding variables) are stored in reverse order compared to the lifting order. This is crucial to meet the same variable order as for the constraints.
	 * @param variables list of variables. Note that the first variable is always the last one lifted.
	 * @param constraints conjunction of constraints for the final check of against the current constructed RealAlgebraicPoint
	 * @param bounds bounds for the variables represented by their index. If bounds for variables are set, the lifting in the corresponding dimension will only be performed within these bounds.
	 * @param boundsActive true if bounds are defined, false otherwise
	 * @param checkBounds if true, all points are checked against the bounds
	 * @param r RealAlgebraicPoint which contains the satisfying sample point if the check results true
	 * @param conflictGraph This is a conflict graph. See CAD::check for a full description.
	 * @param satPath Indices of regions that lead to a satisfying sample. This is used, if we backtrack due to integrality errors.
	 * @return <code>true</code> if from <code>node</code> a path in the sample tree can be constructed so that the corresponding sample satisfies the <code>c</code>, <code>false</code> otherwise.
	 */
	cad::Answer liftCheck(
			sampleIterator node,
			std::size_t openVariableCount,
			bool restartLifting,
			const std::list<Variable>& variables,
			const BoundMap& bounds,
			bool boundsActive,
			bool checkBounds,
			RealAlgebraicPoint<Number>& r,
			cad::ConflictGraph<Number>& conflictGraph,
			std::stack<std::size_t>& satPath
	);
	
	/**
	 * If eliminationSets[level].emptyLiftingQueue() is true,
	 * perform elimination steps so that eliminationSets[level] or eliminationSets[l] for any l smaller than level
	 * gains new polynomials.
	 * If this was successful or !eliminationSets[level].emptyLiftingQueue(), the method returns true; otherwise false.
	 * @param level Level into which new polynomials shall be eliminated.
	 * @param bounds bounds for the variables represented by their index. If bounds for variables are set, the lifting in the corresponding dimension will only be performed within these bounds.
	 * @param boundsActive true if bounds are defined, false otherwise
	 * @return the level to which at least one new polynomial was added due to elimination;
	 * or -1 if no more polynomials could be eliminated into the given level or a level before
	 */
	int eliminate(std::size_t level, const BoundMap& bounds, bool boundsActive);

	/**
	 * Get the boundaries of the cad cell interval defined by the children of the given sample tree node for the given sample.
	 * @param parent
	 * @param sample
	 * @return a bounding Interval for sample at parent's children
	 */
	Interval<Number> getBounds(const sampleIterator& parent, const RealAlgebraicNumber<Number> sample) const;

	/** CURRENTLY DISABLED
	 *
	 * Based on the sample tree, the given bounds are widened to the maximal CAD cell boundary for every variable.
	 *
	 * This method does no exact maximization of the cell, but takes the maximum boundaries from the sample tree and checks whether the problem is still unsatisfiable.
	 * @param bounds
	 * @param constraints
	 */
	void widenBounds(BoundMap& bounds);

	/**
	 * The given bounds are shrunk to the a value close to the given (satisfying) point for every variable.
	 * @param bounds
	 * @param r real algebraic point
	 */
	void shrinkBounds(BoundMap& bounds, const RealAlgebraicPoint<Number>& r);

	/**
	 * Determines whether p (of elimination set at level) has a root in the given box.
	 * @param p
	 * @param box
	 * @param level
	 * @param recuperate if true, the polynomials computed are recuperated into this CAD's elimination sets (default: true)
	 * @return true if p has a root in the given box, false otherwise
	 */
	bool vanishesInBox(const UPolynomial* p, const BoundMap& box, std::size_t level, bool recuperate = true);

	/**
	 * Checks whether one of the flags indicating whether to stop a currently running check procedure is set to true.
	 * @return True, if this is the case.
	 */
	bool anAnswerFound() const {
		for (auto iter: this->interrupts) {
			if (iter->load()) return true;
		}
		return false;
	}

	template<typename It>
	bool isSampleConsistent(It node) const {
		bool lastRoot = false;
		for (auto cur = sampleTree.begin_children(node); cur != sampleTree.end_children(node); cur++) {
			if (cur->isRoot() && lastRoot) return false;
			lastRoot = cur->isRoot();
			if (!isSampleConsistent(cur)) return false;
		}
		return true;
	}

	bool isSampleTreeConsistent() const {
		bool isOk = isSampleConsistent(this->sampleTree.begin());
		if (!isOk) {
			CARL_LOG_ERROR("carl.cad", "SampleTree: " << this->sampleTree);
		}
		return true;
		assert(isOk);
		return isOk;
	}

	/**
	 * Check if the sample point represented by a given node in the sample tree matches the integrality constraints imposed by the variables.
	 * @param node Node in sample tree.
	 * @return If sample fulfills integrality constraints.
	 */
	template<typename It>
	bool checkIntegrality(It node) const {
		for (auto pit = sampleTree.begin_path(node); pit.depth() != 0; ++pit) {
			Variable var = mVariables[pit.depth() - 1];
			if ((var.type() == VariableType::VT_INT) && (!pit->isIntegral())) return false;
		}
		return true;
	}
};

}

#include "CAD.tpp"
