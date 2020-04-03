/**
 * @file CAD.tpp
 * @ingroup cad
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include <forward_list>
#include <fstream>
#include <vector>

#include "CAD.h"

#include <carl/core/logging.h>
#include <carl/interval/IntervalEvaluation.h>
#include <carl/core/polynomialfunctions/Representation.h>
#include <carl/core/polynomialfunctions/RootFinder.h>
#include <carl/thom/ThomRootFinder.h>
#include <carl/core/polynomialfunctions/SquareFreePart.h>

#define PERFORM_PARTIAL_CHECK false

namespace carl {

template<typename Number>
unsigned CAD<Number>::checkCallCount = 0;

template<typename Number>
CAD<Number>::CAD():
		mVariables(),
		sampleTree(),
		eliminationSets(),
		polynomials(),
		iscomplete(false),
		interrupted(false),
		interrupts(),
		setting(cad::CADSettings::getSettings())
{
	// initialize root with empty node
	this->sampleTree.setRoot(RealAlgebraicNumber<Number>(0, false));
}

template<typename Number>
CAD<Number>::CAD(cad::PolynomialOwner<Number>* parent):
		mVariables(),
		sampleTree(),
		eliminationSets(),
		polynomials(parent),
		iscomplete(false),
		interrupted(false),
		interrupts(),
		setting(cad::CADSettings::getSettings())
{
	// initialize root with empty node
	this->sampleTree.setRoot(RealAlgebraicNumber<Number>(0, false));
}

template<typename Number>
CAD<Number>::CAD(const std::vector<std::atomic_bool*>& i):
		CAD()
{
	this->interrupts = i;
}

template<typename Number>
CAD<Number>::CAD(const cad::CADSettings& _setting):
		CAD()
{
	this->setting = _setting;
}

template<typename Number>
CAD<Number>::CAD(const std::list<const UPolynomial*>& s, const std::vector<Variable>& v, const cad::CADSettings& _setting):
		CAD()
{
	this->scheduledPolynomials.assign(s.begin(), s.end());
	mVariables.setNewVariables(v);
	this->setting = _setting;
	this->prepareElimination();
}

template<typename Number>
CAD<Number>::CAD(const std::list<const UPolynomial*>& s, const std::vector<Variable>& v, const std::vector<std::atomic_bool*>& c, const cad::CADSettings& _setting):
		CAD(s, v, _setting)
{
	this->interrupts = c;
}

template<typename Number>
CAD<Number>::CAD(const CAD<Number>& cad):
		mVariables( cad.mVariables ),
		sampleTree( cad.sampleTree ),
		eliminationSets( cad.eliminationSets ),
		polynomials( cad.polynomials ),
		iscomplete( cad.iscomplete ),
		interrupted( cad.interrupted ),
		setting( cad.setting )
{
}

template<typename Number>
cad::SampleSet<Number> CAD<Number>::samplesAt(const sampleIterator& node) const {
	cad::SampleSet<Number> samples(setting.sampleOrdering);
	samples.insert(this->sampleTree.begin(node), this->sampleTree.end(node));
	return samples;
}

template<typename Number>
std::vector<RealAlgebraicPoint<Number>> CAD<Number>::samples() const {
	size_t dim  = mVariables.size();
	std::vector<RealAlgebraicPoint<Number>> s;
	for (auto leaf = this->sampleTree.begin_leaf(); leaf != this->sampleTree.end_leaf(); leaf++) {
		// for each leaf construct the path by iterating back to the root
		RealAlgebraicPoint<Number> sample(this->constructSampleAt(leaf, this->sampleTree.begin()));

		// discard points which are ill-formed (possible by intermediate nodes which did not yield valid child nodes)
		if (sample.dim() == dim) {
			s.push_back(sample);
		}
	}
	return s;
}

template<typename Number>
void CAD<Number>::printSampleTree(std::ostream& os) const {
	for (auto i = this->sampleTree.begin(); i != this->sampleTree.end(); i++) {
		for (unsigned d = 0; d != this->sampleTree.depth(i); d++) {
			os << " [";
		}
		print(*i, os);
		os << std::endl;
	}
}

template<typename Number>
void CAD<Number>::printConstraints(const std::string& filename) const {
	if( !mConstraints.empty() ){
		std::ofstream smtlibFile;
		smtlibFile.open(filename);
		smtlibFile << "(set-logic QF_NRA)\n(set-info :smt-lib-version 2.0)\n";
		// add all real-valued variables
		for (const auto& var: mVariables)
			smtlibFile << "(declare-fun " << var << " () Real)\n";

		smtlibFile << "(assert (and ";
		for (const auto& constraint: mConstraints) {
			switch (constraint.getSign()) {
				case Sign::ZERO: {
					if (constraint.isNegated())
						smtlibFile << " (<> " << constraint.getPolynomial() << " 0)";
					else
						smtlibFile << " (= " << constraint.getPolynomial() << " 0)";
					break;
				}
				case Sign::POSITIVE: {
					if (constraint.isNegated())
						smtlibFile << " (<= " << constraint.getPolynomial() << " 0)";
					else
						smtlibFile << " (> " << constraint.getPolynomial() << " 0)";
					break;
				}
				case Sign::NEGATIVE:
				{
					if (constraint.isNegated())
						smtlibFile << " (>= " << constraint.getPolynomial() << " 0)";
					else
						smtlibFile << " (< " << constraint.getPolynomial() << " 0)";
					break;
				}
			}
		}
		smtlibFile << "))\n";
		smtlibFile << "(check-sat)\n";

		smtlibFile << "(exit)";
		smtlibFile.close();
	}
}

/**
 * Print the CAD object information to the output stream.
 * @param os Output stream.
 * @param cad CAD object.
 * @return os.
 */
template<typename Number>
std::ostream& operator<<(std::ostream& os, const CAD<Number>& cad) {
	//os << std::endl << cad.getSetting() << std::endl;
	os << "Variables: " << cad.mVariables << std::endl;
	os << "Elimination sets:" << std::endl;
	unsigned level = 0;
	for (const auto& i: cad.getEliminationSets()) {
		os << level++ << ":\tP: " << i << std::endl;
	}
	os << "Sample tree:" << std::endl;
	os << cad.sampleTree << std::endl;
	os << "Number of samples computed: " << cad.samples().size() << std::endl;
	os << "CAD complete: " << cad.isComplete() << std::endl;
	return os;
}

template<typename Number>
bool CAD<Number>::prepareElimination() {
	CARL_LOG_TRACE("carl.cad", __func__ << "()");
	if (mVariables.newEmpty() && (!polynomials.hasScheduled() || mVariables.empty())) {
		return false;
	}

	std::size_t newVariableCount = mVariables.newSize();

	/* Algorithm overview:
	 *
	 * Part A: preparation of the elimination sets with the new polynomials
	 *
	 * (1) Add the existing variables to the new ones and swap the two lists. Extend other data structures accordingly.
	 * (2) Add as many levels to the **front** of eliminationSets as new variables were determined and add the prevailing elimination sets after the new slots.
	 *
	 * Part B: elimination of the new polynomials by pairwise elimination with the existing ones
	 * Note that the sample tree is not touched in regard to the new variables, this has to be done during the sample construction.
	 */

	/* Part A */

	if (!mVariables.newEmpty()) {
		// introduce new elimination levels and fill them appropriately
		// (1)

		CARL_LOG_TRACE("carl.cad", "Adding " << mVariables.newSize() << " to " << mVariables.size() << " old variables.");
		// variables, newVariables = newVariables:variables, []
		mVariables.appendNewToCur();

		// (1)
		/// @todo make this more efficient
		std::vector<cad::EliminationSet<Number>> sets(mVariables.size(), cad::EliminationSet<Number>(&this->polynomials, typename cad::EliminationSet<Number>::PolynomialComparator(this->setting.order), typename cad::EliminationSet<Number>::PolynomialComparator(this->setting.order)));
		for (std::size_t i = newVariableCount; i < sets.size(); i++) {
			std::swap(sets[i], this->eliminationSets[i - newVariableCount]);
		}
		std::swap(this->eliminationSets, sets);
	}

	// add new polynomials to level 0, unifying their variables, and the list of all polynomials
	for (const auto& p: polynomials.getScheduled()) {
		auto tmp = p;
		if (p->mainVar() != mVariables.front()) {
			tmp = new UPolynomial(switch_main_variable(*p, mVariables.front()));
			this->polynomials.take(tmp);
		}
		this->polynomials.addPolynomial(tmp);
		this->eliminationSets.front().insert(tmp);
	}

	// optimizations for the first elimination level
	if (this->setting.simplifyByFactorization) {
		this->eliminationSets.front().factorize();
	}
	this->eliminationSets.front().makePrimitive();
	this->eliminationSets.front().makeSquarefree();
	if (this->setting.simplifyByRootcounting && mVariables.size() == 1) {
		// this simplification is done for the base level in liftCheck
		this->eliminationSets.front().removePolynomialsWithoutRealRoots();
	}
	// done for the current scheduled polynomials
	polynomials.clearScheduled();
	CARL_LOG_TRACE("carl.cad", "Done with prepareElimination()");
	return newVariableCount != 0;
}

template<typename Number>
void CAD<Number>::clearElimination() {
	this->iscomplete = false;
	this->eliminationSets.front().clear();

	// re-add the input polynomials to the front level
	this->eliminationSets.front().insert(this->polynomials.begin(), this->polynomials.end());
}

#ifdef __VS
template<typename Number>
void CAD<Number>::completeElimination(const typename CAD<Number>::BoundMap& bounds) {
#else
template<typename Number>
void CAD<Number>::completeElimination(const CAD<Number>::BoundMap& bounds) {
#endif
	this->prepareElimination();
	bool useBounds = !bounds.empty();
	for (const auto& b: bounds) {
		useBounds = useBounds && !b.second.isInfinite();
	}

	if (useBounds) {
		// construct constraints and polynomials representing the bounds
		for (const auto& b: bounds) {
			std::size_t l = b.first;
			if (l >= mVariables.size()) continue;
			// construct bound-related polynomials
			std::list<const UPolynomial*> tmp;
			if (b.second.lowerBoundType() != BoundType::INFTY) {
				tmp.push_back(this->polynomials.take(new UPolynomial(mVariables[l], {MPolynomial(-b.second.lower()), MPolynomial(1)})));
				if (!this->setting.earlyLiftingPruningByBounds) {
					// need to add bound polynomial if no bounds are generated automatically
					this->eliminationSets[b.first].insert(tmp.back());
				}
			}
			if (b.second.upperBoundType() != BoundType::INFTY) {
				tmp.push_back(this->polynomials.take(new UPolynomial(mVariables[l], {MPolynomial(-b.second.upper()), MPolynomial(1)})));
				if (!this->setting.earlyLiftingPruningByBounds) {
					// need to add bound polynomial if no bounds are generated automatically
					this->eliminationSets[l].insert(tmp.back());
				}
			}

			// eliminate bound-related polynomials
			l++;
			while (!tmp.empty() && l < mVariables.size()) {
				std::list<const UPolynomial*> tmp2;
				for (const auto& p: tmp) {
					auto res = this->eliminationSets[l-1].eliminateInto(p, this->eliminationSets[l], mVariables[l], this->setting);
					tmp2.insert(tmp2.begin(), res.begin(), res.end());
				}
				std::swap(tmp, tmp2);
				l++;
			}
		}
	}

	if (this->setting.simplifyEliminationByBounds) {
		for (unsigned l = 1; l < this->eliminationSets.size(); l++) {
			while (! this->eliminationSets[l-1].emptySingleEliminationQueue()) {
				// the polynomial can be analyzed for zeros
				auto p = this->eliminationSets[l-1].popNextSingleEliminationPosition();
				CARL_LOG_DEBUG("carl.cad", "Checking whether " << *p << " vanishes in " << bounds);
				if (!this->vanishesInBox(p, bounds, l-1)) {
					this->eliminationSets[l-1].erase(p);
				}
			}
			while (!this->eliminationSets[l-1].emptyPairedEliminationQueue()) {
				this->eliminationSets[l-1].eliminateNextInto(this->eliminationSets[l], mVariables[l], this->setting);
			}
		}
	} else {
		// unbounded elimination from level l-1 to level l
		for (unsigned l = 1; l < this->eliminationSets.size(); l++) {
			while (	!this->eliminationSets[l-1].emptySingleEliminationQueue() ||
					!this->eliminationSets[l-1].emptyPairedEliminationQueue()) {
				this->eliminationSets[l-1].eliminateNextInto(this->eliminationSets[l], mVariables[l], this->setting, false);
			}
		}
	}
}

template<typename Number>
void CAD<Number>::clear() {
	mVariables.clear();
	this->sampleTree.clear();
	// Add empty root node
	this->sampleTree.insert(this->sampleTree.begin(), nullptr);
	this->eliminationSets.clear();
	this->polynomials.clear();
	this->polynomials.clearScheduled();
	this->iscomplete = false;
	this->interrupted = false;
	this->interrupts.clear();
	this->checkCallCount = 0;
}

template<typename Number>
void CAD<Number>::complete() {
	RealAlgebraicPoint<Number> r;
	assert(mVariables.size() > 0);
	std::vector<cad::Constraint<Number>> c(1, cad::Constraint<Number>(UPolynomial(mVariables.front(), MPolynomial(1)), Sign::ZERO, mVariables));
	this->check(c, r, true);
}

template<typename Number>
void CAD<Number>::tryEquationSeparation(bool useBounds, bool onlyStrictBounds) {
	bool hasEquations = false;
	bool hasStrict = false;
	bool hasWeak = false;
	for (const auto& c: mConstraints) {
		if (c.getSign() == Sign::ZERO && !c.isNegated()) {
			hasEquations = true;
		} else if (c.getSign() != Sign::ZERO && c.isNegated()) {
			hasWeak = true;
		} else {
			hasStrict = true;
		}
	}
	if (!hasWeak) {
		if (!useBounds && !hasStrict && mVariables.size() <= 1) {
			// root-only samples not valid in general!
			alterSetting(cad::CADSettings::getSettings(cad::EQUATIONSONLY, setting));
		} else if (onlyStrictBounds && !hasEquations) {
			alterSetting(cad::CADSettings::getSettings(cad::INEQUALITIESONLY, setting));
		}
	}
	// else: mixed case, no optimization possible without zero-dimensional assumption
}


template<typename Number>
cad::Answer CAD<Number>::check(
	std::vector<cad::Constraint<Number>>& _constraints,
	RealAlgebraicPoint<Number>& r,
	cad::ConflictGraph<Number>& conflictGraph,
	BoundMap& bounds,
	bool next,
	bool checkBounds)
{
	assert(this->sampleTree.isConsistent());
	this->prepareElimination();
	assert(this->sampleTree.isConsistent());
	mConstraints.set(_constraints, mVariables);
    #ifdef LOGGING
	CARL_LOG_DEBUG("carl.cad", "Checking the system");
	for (const auto& c: mConstraints) CARL_LOG_DEBUG("carl.cad", "  " << c);
	CARL_LOG_DEBUG("carl.cad", "within " << ( bounds.empty() ? "no bounds." : "these bounds:" ));
	for (const auto& b: bounds) {
		if (mVariables.size() > b.first) { CARL_LOG_DEBUG("carl.cad", "  " << b.second << " for variable " << mVariables[b.first]); }
		else { CARL_LOG_DEBUG("carl.cad", "  " << b.second << " for variable " << b.first); }
	}
	for (unsigned i = 0; i < this->eliminationSets.size(); i++) {
		CARL_LOG_DEBUG("carl.cad", "  Level " << i << " (" << this->eliminationSets[i].size() << "): " << this->eliminationSets[i]);
	}
    #endif
	assert(this->isSampleTreeConsistent());
	assert(this->sampleTree.isConsistent());

	//////////////////////
	// Initialization

	this->interrupted = false;
	bool useBounds = false;
	bool onlyStrictBounds = true;
	for (const auto& b: bounds) {
		if (!b.second.isInfinite() && !b.second.isEmpty()) {
			useBounds = true;
		}
		if (b.second.lowerBoundType() == BoundType::WEAK || b.second.upperBoundType() == BoundType::WEAK) {
			onlyStrictBounds = false;
		}
	}

	//////////////////////
	// Preprocessing
	assert(this->sampleTree.isConsistent());

	// check bounds for empty interval
	for (const auto& b: bounds) {
		CARL_LOG_DEBUG("carl.cad", "Checking bound " << b.first << " : " << b.second);
		if (b.second.isEmpty()) return cad::Answer::False;
	}
	if (mConstraints.empty() && useBounds) {
		// each bound non-empty plus empty input constraints
		return cad::Answer::True;
	}

	// try to solve the constraints by interval arithmetic
	if (this->setting.preSolveByBounds) {
		typename Interval<Number>::evalintervalmap m;
		for (const auto& b: bounds) {
			if (b.first >= mVariables.size()) continue;
			m[mVariables[b.first]] = b.second;
		}
		if (!m.empty()) {
			// there are bounds we can use
			for (const auto& constraint: mConstraints) {
				/// @todo A Constraint may be negated.
				if (IntervalEvaluation::evaluate(constraint.getPolynomial(), m).sgn() != constraint.getSign()) {
					// the constraint is unsatisfiable!
					// this constraint is already the minimal infeasible set, so switch it with the last position in the constraints list
					std::size_t sampleID = conflictGraph.newSample();
					std::size_t constraintID = conflictGraph.getConstraint(constraint);
					conflictGraph.set(constraintID, sampleID, true);
					CARL_LOG_DEBUG("carl.cad", "Conflicting for itself: " << constraint);
					return cad::Answer::False;
				}
				// else: no additional check is needed!
			}
		}
	}

	// separate treatment of equations and inequalities
	cad::CADSettings backup = this->setting;
	if (this->setting.autoSeparateEquations) {
		tryEquationSeparation(useBounds, onlyStrictBounds);
	}

	//////////////////////
	// Main check procedure

	std::vector<std::pair<const UPolynomial*, const UPolynomial*>> boundPolynomials(mVariables.size(), std::pair<UPolynomial*, UPolynomial*>());
	CARL_LOG_TRACE("carl.cad", "Creating boundPolynomials of size " << mVariables.size());

	if (useBounds) {
		CARL_LOG_DEBUG("carl.cad", "Preprocess bounds");
		// construct constraints and polynomials representing the bounds
		for (const auto& b: bounds) {
			if (b.first >= mVariables.size()) continue;

			// construct bound-related polynomials
			std::list<const UPolynomial*> tmp;
			if (b.second.lowerBoundType() != BoundType::INFTY) {
				UPolynomial p(mVariables[b.first], {MPolynomial(Term<Number>(-b.second.lower())), MPolynomial(Term<Number>(1))});
				tmp.push_back(this->polynomials.take(new UPolynomial(pseudo_primitive_part(p))));
				this->eliminationSets[b.first].insert(tmp.back());
				this->iscomplete = false; // new polynomials induce new sample points
				assert(b.first < boundPolynomials.size());
				boundPolynomials[b.first].first = tmp.back();
			}
			if (b.second.upperBoundType() != BoundType::INFTY) {
				UPolynomial p(mVariables[b.first], {MPolynomial(Term<Number>(-b.second.upper())), MPolynomial(Term<Number>(1))});
				tmp.push_back(this->polynomials.take(new UPolynomial(pseudo_primitive_part(p))));
				this->eliminationSets[b.first].insert(tmp.back());
				this->iscomplete = false; // new polynomials induce new sample points
				boundPolynomials[b.first].first = tmp.back();
			}

			// eliminate bound-related polynomials only
			// l: variable index of the elimination destination
			std::size_t l = b.first + 1;
			while (!tmp.empty() && l < mVariables.size()) {
				std::list<const UPolynomial*> tmp2;
				for (const auto& p: tmp) {
					auto res = this->eliminationSets[l-1].eliminateInto(p, this->eliminationSets[l], mVariables[l], this->setting);
					tmp2.insert(tmp2.begin(), res.begin(), res.end());
				}
				std::swap(tmp, tmp2);
				l++;
			}
		}
	}

	// call the main check function according to the settings
	CARL_LOG_DEBUG("carl.cad", "Calling mainCheck...");
	assert(this->sampleTree.isConsistent());
	cad::Answer satisfiable = this->mainCheck(bounds, r, conflictGraph, next, useBounds, checkBounds);
	assert(this->sampleTree.isConsistent());
	CARL_LOG_DEBUG("carl.cad", "mainCheck returned " << satisfiable);

	if (useBounds) {
		CARL_LOG_DEBUG("carl.cad", "Postprocess bounds");
		// possibly tweak the bounds
		if (this->setting.improveBounds) {
			if (satisfiable == cad::Answer::True) {
				this->shrinkBounds(bounds, r);
			} else {
				this->widenBounds(bounds);
			}
		}
		assert(this->sampleTree.isConsistent());
		// restore elimination polynomials to their previous state due to possible bound-related simplifications
		for (unsigned l = 0; l < mVariables.size(); l++) {
			// remove bound polynomials and their children
			if (boundPolynomials[l].first != nullptr) {
				// remove exclusively children if bound polynomials were not added
				this->removePolynomial(boundPolynomials[l].first, l, this->setting.earlyLiftingPruningByBounds);
			}
			if (boundPolynomials[l].second != nullptr) {
				// remove exclusively children if bound polynomials were not added
				this->removePolynomial(boundPolynomials[l].second, l, this->setting.earlyLiftingPruningByBounds);
			}
		}
		assert(this->sampleTree.isConsistent());
		if (this->setting.simplifyEliminationByBounds) {
			// re-add the input polynomials to the top-level (for they could have been deleted)
			this->eliminationSets.front().clear();
			for (const auto& p: this->polynomials.getPolynomials()) {
				if (p->mainVar() == mVariables.front()) {
					this->eliminationSets.front().insert(p);
				} else {
					this->eliminationSets.front().insert(this->polynomials.take(new UPolynomial(switch_main_variable(*p, mVariables.front()))));
				}
			}
		} else {
			// only reset the first elimination level
			this->eliminationSets.front().resetLiftingPositionsFully();
			this->eliminationSets.front().setLiftingPositionsReset();
		}
		assert(this->sampleTree.isConsistent());
		for (unsigned l = 1; l < this->eliminationSets.size(); l++) {
			// reset every lifting queue besides the first elimination level
			this->eliminationSets[l].resetLiftingPositionsFully();
			this->eliminationSets[l].setLiftingPositionsReset();
		}
	}

	if (satisfiable == cad::Answer::True) {
		CARL_LOG_DEBUG("carl.cad", "Result: sat (by sample point " << r << ")");
	} else if (satisfiable == cad::Answer::Unknown) {
		CARL_LOG_DEBUG("carl.cad", "Result: unknown");
	} else {
		CARL_LOG_DEBUG("carl.cad", "Result: unsat");
	}
	CARL_LOG_TRACE("carl.cad", "Status:" << std::endl << *this);

	this->alterSetting(backup);
	assert(this->sampleTree.isConsistent());
	return satisfiable;
}

template<typename Number>
void CAD<Number>::addPolynomial(const MPolynomial& p, const std::vector<Variable>& v) {
	CARL_LOG_TRACE("carl.cad", __func__ << "( " << p << ", " << v << " )");
	Variable var = v.front();
	if (!mVariables.empty()) var = mVariables.first();

	UPolynomial* up = new UPolynomial(carl::squareFreePart(carl::to_univariate_polynomial(p, var)));
	CARL_LOG_TRACE("carl.cad", "Adding" << std::endl << "original   " << carl::to_univariate_polynomial(p, var) << std::endl << "simplified " << *up);
	if (polynomials.isScheduled(up)) {
		// same polynomial was already considered in scheduled polynomials
		delete up;
		return;
	}
	if (!this->eliminationSets.empty()) {
		if (this->eliminationSets.front().find(up) != nullptr) {
			// same polynomial was already considered in elimination polynomials
			delete up;
			return;
		}
	}
	// schedule the polynomial for the next elimination
	this->polynomials.schedule(p, up);

	// determine the variables differing from mVariables and add them to the front of the existing variables
	mVariables.complete(v);
}

template<typename Number>
void CAD<Number>::removePolynomial(const MPolynomial& polynomial) {
	CARL_LOG_TRACE("carl.cad", __func__ << "( " << polynomial << " )");

	auto up = polynomials.removePolynomial(polynomial);
	if (up == nullptr) return;

	// determine the level of the polynomial (first level from the top) and remove the respective pointer from it
	for (unsigned level = 0; level < this->eliminationSets.size(); level++) {
		// transform the polynomial according to possible optimizations in order to recognize its real shape in the elimination set
		//auto tmp = new UPolynomial(polynomial.pseudoPrimpart());
		CARL_LOG_TRACE("carl.core", "Removing " << *up << " from " << this->eliminationSets[level]);
		auto p = this->eliminationSets[level].find(up);
		if (p != nullptr) {
			this->removePolynomial(p, level);
			return;
		}
	}
}

template<typename Number>
void CAD<Number>::removePolynomial(const UPolynomial* p, unsigned level, bool childrenOnly) {
	// no equivalent polynomial for p in any level
	if (p == nullptr) return;
	CARL_LOG_FUNC("carl.cad", *p << ", " << level << ", " << childrenOnly);
	assert(this->isSampleTreeConsistent());
	assert(this->sampleTree.isConsistent());

	/* Delete
	 * 1. the polynomial from the given level in the elimination sets,
	 * 2. all its parents from previous levels,
	 */

	if (!childrenOnly && (this->eliminationSets[level].hasParents(p) || !this->eliminationSets[level].erase(p))) {
		// polynomial did not exist in the given level or it stems from another polynomial as well
		return;
	}

	// remove all elimination polynomials being children of p starting at the level following p
	unsigned dim = (unsigned)this->eliminationSets.size();
	std::forward_list<const UPolynomial*> parents({ p });
	for (unsigned l = level+1; !parents.empty() && l < dim; l++) {
		std::forward_list<const UPolynomial*> newParents(parents);
		for (const auto& parent: parents) {
			std::forward_list<const UPolynomial*> curParents = this->eliminationSets[l].removeByParent(parent);
			newParents.insert_after(newParents.before_begin(), curParents.begin(), curParents.end());
		}
		newParents.sort(std::less<UPolynomial>(this->setting.order));
		newParents.unique();
		std::swap(parents, newParents);
	}

	/* Sample tree cleaning
	 * If an elimination level is empty, all samples of a sample tree level can be erased up to one where the lifting was complete.
	 * Over-approximation:
	 * - Take the first leaf of the level corresponding to the empty level.
	 * - Add all Children of the level which are not present yet (merging).
	 *
	 */
	std::size_t maxDepth = this->sampleTree.max_depth();
	auto sampleTreeRoot = this->sampleTree.begin();
	for (int l = (int)dim - 1; l >= (int)level; l--) {
		assert(this->sampleTree.isConsistent());
		// iterate from the leaves to the root (more efficient if several levels are to be cleaned)
		if (this->eliminationSets[(size_t)l].empty()) {
			// there is nothing more to be done for this level, so erase all samples
			unsigned depth = dim - (unsigned)l;
			assert(maxDepth <= this->sampleTree.max_depth());
			if (depth <= maxDepth) {
				// erase all samples on this level
				for (auto node = this->sampleTree.begin_depth(depth); node != this->sampleTree.end_depth(); ) {
					node = this->sampleTree.erase(node);
				}
				maxDepth = depth-1;
			}
		}
	}
	assert(this->sampleTree.isConsistent());
}

template<typename Number>
std::vector<Interval<Number>> CAD<Number>::getBounds(const RealAlgebraicPoint<Number>& r) const {
	std::vector<Interval<Number>> bounds(mVariables.size());
	// initially, parent is the root
	auto parent = this->sampleTree.begin();

	for (int index = mVariables.size()-1; index >= 0; index--) {
		// tree is build upside down, index is in [mVariables.size()-1, 0]
		RealAlgebraicNumber<Number> sample = r[index];
		if (this->sampleTree.begin(parent) == this->sampleTree.end(parent)) {
			// this tree level is empty
			bounds[index] = Interval<Number>::unboundedInterval();
			continue;
		}
		// search for the left and right boundaries in the first variable eliminated
		// does not compare less than r
		auto node = std::lower_bound(this->sampleTree.begin(parent), this->sampleTree.end(parent), sample);

		bounds[index] = this->getBounds(node, sample);
		parent = node;
	}
	return bounds;
}

template<typename Number>
cad::SampleSet<Number> CAD<Number>::samples(
		std::size_t openVariableCount,
		const std::list<RealAlgebraicNumber<Number>>& roots,
		cad::SampleSet<Number>& currentSamples,
		std::forward_list<RealAlgebraicNumber<Number>>& replacedSamples,
		const Interval<Number>& bounds
) {
	cad::SampleSet<Number> newSampleSet(currentSamples.ordering());
	replacedSamples.clear();
	if (roots.empty()) {
		return newSampleSet;
	}
	CARL_LOG_FUNC("carl.cad", roots << ", " << currentSamples.samples() << ", " << bounds);
	if (!currentSamples.samples().empty()) {
		// Sanity check: Assert that outermost sample is a root.
		auto first = *currentSamples.samples().begin();
		assert(!first.isRoot());
	}

	bool boundsActive = !bounds.isEmpty() && !bounds.isInfinite();

	for (const auto& root: roots) {
		CARL_LOG_TRACE("carl.cad", "\tWorking on " << root);
		if (!root.containedIn(bounds)) {
			CARL_LOG_TRACE("carl.cad", "\tout of bounds " << bounds << " -> ignoring");
			continue;
		}
		auto insertValue = currentSamples.insert(root);
		auto insertIt = std::get<0>(insertValue);
		if (!std::get<1>(insertValue)) {
			if (std::get<2>(insertValue)) {
				newSampleSet.insert(*insertIt);
				replacedSamples.push_front(*insertIt);
				CARL_LOG_TRACE("carl.cad", "\treplaced another sample");
			} else {
				CARL_LOG_TRACE("carl.cad", "\tsample already exists as " << *insertIt);
			}
		} else {
			// we found a new sample
			// add the root to new samples (with root switch on)
			newSampleSet.insert(*insertIt);
			CARL_LOG_TRACE("carl.cad", "\tadded as new sample");
		}
		// local set storing the elements which shall be added to currentSampleSet and newSampleSet in the end
		std::list<RealAlgebraicNumber<Number>> newSamples;

		/** Situation: One, next or previous, has to be a root (assumption) or we meet one of the outmost positions.
		 * --------|-------------------|-----------------|---
		 *    previous        insertValue.first         next
		 *     (root?)              (root)            (root?)
		 */

		//std::cout << *this << std::endl;
		//carl::VariableType type = mVariables[openVariableCount].getType();
		//if (!this->setting.exploreInteger) type = VariableType::VT_REAL;
		//std::cout << "Current var: " << openVariableCount << " -> " << mVariables[openVariableCount] << std::endl;
		//REGISTERED_ASSERT(openVariableCount > 1);
		// next: right neighbor
		auto neighbor = insertIt;
		// -> next (safe here, but need to check for end() later)
		neighbor++;
		if (neighbor == currentSamples.end()) {
			newSamples.push_front(carl::sampleAbove(*insertIt));
		} else if (neighbor->isRoot()) {
			newSamples.push_front(carl::sampleBetween(*insertIt, *neighbor));
		}

		// previous: left neighbor
		neighbor = insertIt;
		if (neighbor == currentSamples.begin()) {
			newSamples.push_front(carl::sampleBelow(*insertIt));
		} else {
			neighbor--;
			// now neighbor is the left bound (can be safely determined now)
			if (neighbor->isRoot()) {
				newSamples.push_front(carl::sampleBetween(*neighbor, *insertIt));
			}
		}

		if (boundsActive) {
			// remove samples which do not lie within the (weak) bounds
			for (auto sit = newSamples.begin(); sit != newSamples.end(); ) {
				if (bounds.meets(sit->value())) sit++;
				else sit = newSamples.erase(sit);
			}
		}
		newSampleSet.insert(newSamples.begin(), newSamples.end());
		currentSamples.insert(newSamples.begin(), newSamples.end());
	}
	CARL_LOG_TRACE("carl.cad", " -> " << currentSamples);
	return newSampleSet;
}

template<typename Number>
cad::SampleSet<Number> CAD<Number>::samples(
		std::size_t openVariableCount,
		const UPolynomial* p,
		sampleIterator node,
		cad::SampleSet<Number>& currentSamples,
		std::forward_list<RealAlgebraicNumber<Number>>& replacedSamples,
		const Interval<Number>& bounds
) {
	assert(mVariables.size() == node.depth() + openVariableCount + 1);
	std::map<Variable, RealAlgebraicNumber<Number>> m;
	auto valit = sampleTree.begin_path(node);
	for (std::size_t i = node.depth(); i > 0; i--) {
		m[mVariables[mVariables.size() - i]] = *valit;
		valit++;
	}
	CARL_LOG_FUNC("carl.cad", *p << " on " << m);
	auto roots = carl::rootfinder::realRoots(*p, m, bounds);
	if (roots.empty()) {
		return this->samples(
			openVariableCount,
	        //carl::realRootsThom(*p, m, bounds),
			{ RealAlgebraicNumber<Number>(0) },
			currentSamples,
			replacedSamples,
			bounds
		);
	} else {
		return this->samples(
			openVariableCount,
	        //carl::realRootsThom(*p, m, bounds),
			std::list<RealAlgebraicNumber<Number>>(roots.begin(), roots.end()),
			currentSamples,
			replacedSamples,
			bounds
		);
	}
}

template<typename Number>
template<class VariableIterator, class PolynomialIterator>
std::vector<Variable> CAD<Number>::orderVariablesGreedily(
		VariableIterator firstVariable,
		VariableIterator lastVariable,
		PolynomialIterator firstPolynomial,
		PolynomialIterator lastPolynomial
) {
	// maps each sum of total degrees of the elimination set to its variables being responsible for the respective elimination set
	std::map<std::pair<int, int>, std::forward_list<Variable>> variableMap;
	cad::CADSettings s = cad::CADSettings::getSettings();
	unsigned variableCount = 0;

	for (auto variable = firstVariable; variable != lastVariable; variable++) {
		// build the first elimination set w.r.t. variable and measure its sum of total degrees
		cad::EliminationSet<Number> eliminationInput(this);
		// add input polynomials to temporary input set, unifying their variables
		for (auto p = firstPolynomial; p != lastPolynomial; p++) {
			if (!p.isConstant()) {
				eliminationInput.insert(p);
			}
		}
		// perform the elimination step
		cad::EliminationSet<Number> eliminationOutput(this);
		while (!(eliminationInput.emptySingleEliminationQueue() || eliminationInput.emptyPairedEliminationQueue())) {
			eliminationInput.eliminateNextInto(eliminationOutput, variable, s);
		}
		int degreeSum = 0;
		for (const auto& p: eliminationOutput) {
			degreeSum += p->totalDegree();
		}
		variableMap[std::make_pair(degreeSum, eliminationOutput.size())].push_front(variable);
		variableCount++;
	}
	// transform variableOrder to a sorted vector
	std::vector<Variable> variableOrder(variableCount);
	for (auto i = variableMap.crbegin(); i != variableMap.crend(); i++) {
		for (const auto& j: i->second) {
			variableOrder[--variableCount] = j;
		}
	}
	return variableOrder;
}

template<typename Number>
void CAD<Number>::alterSetting(const cad::CADSettings& _setting) {
	// settings that require re-computation
	if (_setting.order != this->setting.order) {
		// switch the order relation in all elimination sets
		for (auto& i: this->eliminationSets) {
			i.setLiftingOrder(typename cad::EliminationSet<Number>::PolynomialComparator(_setting.order));
		}
	}
	if (!this->setting.simplifyByRootcounting && _setting.simplifyByRootcounting) {
		for (auto& i: this->eliminationSets) {
			i.removePolynomialsWithoutRealRoots();
		}
	}
	if (!this->setting.simplifyByFactorization && _setting.simplifyByFactorization) {
		for (auto& i: this->eliminationSets) {
			i.factorize();
		}
	}

	this->setting = _setting;
}

template<typename Number>
std::list<RealAlgebraicNumber<Number>> CAD<Number>::constructSampleAt(sampleIterator node, const sampleIterator& root) const {
	/* Main sample construction loop macro augmented by a conditional argument for termination with an empty sample.
	 * @param _condition which has to be false for every node of the sample, otherwise an empty list is returned
	 */
	assert(this->sampleTree.begin() == root);
	if ((!this->sampleTree.is_valid(node) && node.isRoot()) || node == root) {
		// node is invalid
		return {};
	}

	std::list<RealAlgebraicNumber<Number>> v;
	// proceed from the leaf up to the root while the children of root represent the last component of the sample point and the leaf the first
	if (this->setting.equationsOnly) {
		while (node != root) {
			if (!node->isRoot()) return {};
			v.push_back(*node);
			node = this->sampleTree.get_parent(node);
		}
	} else if (this->setting.inequalitiesOnly) {
		while (node != root) {
			if (node->isRoot()) return {};
			v.push_back(*node);
			node = this->sampleTree.get_parent(node);
		}
	} else {
		while (node != root) {
			assert(sampleTree.is_valid(node));
			v.push_back(*node);
			node = this->sampleTree.get_parent(node);
			assert(sampleTree.is_valid(node));
			assert(node != sampleTree.end());
		}
	}
	return v;
}

template<typename Number>
typename CAD<Number>::CheckNodeResult CAD<Number>::checkNode(
		sampleIterator node,
		bool fullRestart,
		bool excludePrevious,
		BoundMap& bounds,
		RealAlgebraicPoint<Number>& r,
		cad::ConflictGraph<Number>& conflictGraph,
		bool boundsNontrivial,
		bool checkBounds,
		std::size_t dim
) {
	assert(this->sampleTree.is_valid(node));
	CARL_LOG_TRACE("carl.cad", __func__ << "( " << *node << ", " << bounds << " )");
	// for each node construct the path by iterating back to the root (no way to check the bounds from here since the depth of the leaf is still unknown)
	auto sampleList = this->constructSampleAt(node, this->sampleTree.begin());
	// settings demand not to take this sample (e.g., because only real roots are solutions)
	if (sampleList.empty()) {
		CARL_LOG_TRACE("carl.cad", "sample is empty");
		return CNR_SKIP;
	}
	RealAlgebraicPoint<Number> sample(sampleList);
	bool boundsOK = true;
	// offset for incomplete samples (sample is filled from behind)
	unsigned firstLevel = (unsigned)(mVariables.size() - sample.dim());
	CARL_LOG_TRACE("carl.cad", "first level " << firstLevel);

	// test if the sample _r is already outside the bounds (boundsOK=false) or if it can be checked against the constraints or further lifted (boundsOK=true)
	for (const auto& i: bounds) {
		// bounds correspond to mVariables indices, so shift those indices by firstLevel to the left
		if (i.first < mVariables.size() && firstLevel <= i.first && !sample[i.first - firstLevel].containedIn(i.second)) {
			boundsOK = false;
			break;
		}
	}
	if (!boundsOK) {
		CARL_LOG_TRACE("carl.cad", "bound clash");
		// this point did not match the bounds => continue searching
		return CNR_SKIP;
	}
	if (sample.dim() == dim) {
		CARL_LOG_TRACE("carl.cad", "full dimension");
		// found a sample to check with the constraints
		if (excludePrevious) return CNR_SKIP;

		if (
			(this->setting.computeConflictGraph && mConstraints.satisfiedBy(sample, getVariables(), conflictGraph)) ||
			(!this->setting.computeConflictGraph && mConstraints.satisfiedBy(sample, getVariables()))
			) {
			r = sample;
			CARL_LOG_TRACE("carl.cad", "sample is good!");
			return CNR_TRUE;
		}
	} else {
		CARL_LOG_TRACE("carl.cad", "Incomplete sample " << sampleList << ", continue lifting");
		// found an incomplete sample, then first check the bounds and possibly restart lifting at the respective level
		// prepare the variables for lifting
		std::size_t i = dim;
		std::list<Variable> vars;
		// TODO: Check this
		//for (const auto& component: sampleList) {
		for (std::size_t j = 0; j < sampleList.size(); j++) {
			i--;
			vars.push_front(mVariables[i]);
		}
		// perform lifting at the incomplete leaf (without elimination, only by the current elimination polynomials)
		std::stack<std::size_t> satPath;
		CARL_LOG_DEBUG("carl.cad", "lifting on incomplete node " << *node);
		cad::Answer status = this->liftCheck(node, i, fullRestart, vars, bounds, boundsNontrivial, checkBounds, r, conflictGraph, satPath);
		///@todo Handle answers
		if (status == cad::Answer::True) {
			CARL_LOG_TRACE("carl.cad", "Incomplete sample " << sampleList << ", lifting succesfull");
			return CNR_TRUE;
		} else if (status == cad::Answer::Unknown) {
			CARL_LOG_DEBUG("carl.cad", "Incomplete sample " << sampleList << ", lifting got unknown");
			return CNR_UNKNOWN;
		}
		CARL_LOG_TRACE("carl.cad", "Incomplete sample " << sampleList << ", lifting failed");
	}
	return CNR_FALSE;
}

template<typename Number>
cad::Answer CAD<Number>::mainCheck(
		BoundMap& bounds,
		RealAlgebraicPoint<Number>& r,
		cad::ConflictGraph<Number>& conflictGraph,
		bool next,
		bool boundsNontrivial,
		bool checkBounds
) {
	CARL_LOG_TRACE("carl.cad", __func__ << "( " << mConstraints << ", " << bounds << " )");
	assert(this->sampleTree.isConsistent());

	if (mVariables.empty()) {
		// there are no valid samples available
		// if there is no constraint, all constraints are satisfied; otherwise no constraint
		if (mConstraints.empty()) return cad::Answer::True;
		else return cad::Answer::False;
	}

	const std::size_t dim = mVariables.size();
	CARL_LOG_TRACE("carl.cad", "mainCheck: dimension is " << dim);
	auto sampleTreeRoot = this->sampleTree.begin();
	std::size_t tmp = this->sampleTree.max_depth(sampleTreeRoot);
	assert(tmp >= 0);
	unsigned maxDepth = (unsigned)tmp;
	// if the elimination sets were extended (i.e. the sample tree is not developed completely), we obtain new samples already in phase one
	next = next && (maxDepth == dim);

	// unify the variables for each constraint to match the CAD's variable order
	// @todo is this necessary? why?
	//for (unsigned i = 0; i < mConstraints.size(); ++i) {
	//	mConstraints[i].unifyVariables(mVariables);
	//}

	////////////
	// Main search strategy

	/* 
	 * Phase 2: Search the sample tree for already satisfying samples and lift the samples not yet lifted to the full dimension
	 *          (all possibly within given bounds).
	 *          Note that next == true skips...
	 * Phase 3: Lift at those sample tree nodes where lifting is still possible (possibly within given bounds).
	 */

	/* Phase 2
	 * Invariant: There might be nodes in the sample tree which are not at the final depth and still need to be lifted.
	 * Check existing sample points and lift incomplete ones, i.e., check all leaves (not necessarily at an appropriate depth).
	 */
	CARL_LOG_TRACE("carl.cad", __func__ << ": Phase 2");
	CARL_LOG_TRACE("carl.cad", *this);
	if (maxDepth == 0) {
		CARL_LOG_TRACE("carl.cad", "maxDepth == 0");
		// there is no sample component computed yet, so we are at the base level
		// perform an initial elimination so that the base level contains lifting positions
		int lastRes = 0;
		while (this->eliminationSets.back().emptyLiftingQueue()) {
			CARL_LOG_TRACE("carl.cad", this->eliminationSets.back());
			lastRes = this->eliminate(dim-1, bounds, boundsNontrivial);
			if (lastRes == -1) {
				CARL_LOG_TRACE("carl.cad", "Lifting");
				// eliminate will not be able to produce a new polynomial.
				std::stack<std::size_t> satPath;
				CARL_LOG_DEBUG("carl.cad", "lifting on " << *this->sampleTree.begin_leaf());
				return this->liftCheck(this->sampleTree.begin_leaf(), dim-this->sampleTree.begin_leaf().depth(), true, {}, bounds, boundsNontrivial, checkBounds, r, conflictGraph, satPath);
			}
			CARL_LOG_DEBUG("carl.cad", "Waiting for something to lift, lastRes = " << lastRes << std::endl << *this);
		};

		// perform an initial lifting step in order to fill the tree once
		std::stack<std::size_t> satPath;
		CARL_LOG_DEBUG("carl.cad", "lifting on " << *this->sampleTree.begin_leaf());
		cad::Answer status = this->liftCheck(this->sampleTree.begin_leaf(), dim-this->sampleTree.begin_leaf().depth(), true, {}, bounds, boundsNontrivial, checkBounds, r, conflictGraph, satPath);
		if (status == cad::Answer::True) {
			// lifting yields a satisfying sample
			return cad::Answer::True;
		} else if (status == cad::Answer::Unknown) {
			return cad::Answer::Unknown;
		}
	} else {
		CARL_LOG_TRACE("carl.cad", "maxDepth != 0, maxDepth = " << maxDepth);
		std::vector<typename Tree::template LeafIterator<false>> leafs;
		for (auto it = this->sampleTree.begin_leaf(); it != this->sampleTree.end_leaf(); it++) leafs.push_back(it);
		typename cad::SampleSet<Number>::SampleComparator comp(setting.sampleOrdering);
		//std::cout << "Before:";
		//for (const auto& it: leafs) std::cout << " " << *it << "(" << (void*)&it << ")";
		//std::cout << std::endl;
		//std::cout << leafs.size() << std::endl;
		//std::cout << "Sorting from [" << (void*)(&*leafs.begin()) << " to " << (void*)(&*leafs.end()) << ")" << std::endl;
		std::sort(leafs.begin(), leafs.end(), [&](const LeafIterator& lhs, const LeafIterator& rhs){ 
			std::stack<bool> l, r;
			for (auto it = sampleTree.begin_path(lhs); it != sampleTree.end_path(); it++) l.push(it->isIntegral());
			for (auto it = sampleTree.begin_path(rhs); it != sampleTree.end_path(); it++) r.push(it->isIntegral());
			while (!l.empty() && !r.empty()) {
				if (l.top() != r.top()) return l.top();
				l.pop();
				r.pop();
			}
			if (l.empty()) {
				while (!r.empty()) {
					if (!r.top()) return true;
					r.pop();
				}
				return false;
			} else if (r.empty()) {
				while (!l.empty()) {
					if (!l.top()) return false;
					l.pop();
				}
				return true;
			} else {
				return false;
			}
		});
		//std::cout << "After:";
		//for (const auto& it: leafs) std::cout << " " << *it << "(" << (void*)&it << ")";
		//std::cout << std::endl;
		// the sample tree contains valid sample points
		//for (auto leaf = this->sampleTree.begin_leaf(); leaf != this->sampleTree.end_leaf(); leaf++) {
		for (const auto& leaf: leafs) {
			// traverse the current sample tree leaves for satisfying samples
			CARL_LOG_TRACE("carl.cad", this->sampleTree);
			auto res = this->checkNode(leaf, true, next, bounds, r, conflictGraph, boundsNontrivial, checkBounds, dim);
			if (res == CNR_TRUE) return cad::Answer::True;
			if (res == CNR_UNKNOWN) return cad::Answer::Unknown;
			if (res == CNR_SKIP) continue;
		}
	}
	CARL_LOG_TRACE("carl.cad", "Checking if CAD is complete");
	if (this->isComplete()) {
		// no leaf of the completely developed sample tree satisfied the constraints
		return cad::Answer::False;
	}
	CARL_LOG_TRACE("carl.cad", "CAD is not complete");

	/* Phase 3
	 * Invariants: (1) The nodes of the sample tree carry all samples (sample components)
	 * of the lifting positions considered so far unless the tree is empty, what is due to the sample storage mechanism in liftCheck.
	 * (2) There might be nodes where not all lifting positions were considered so far.
	 *
	 * - Traverse all levels for open lifting positions and perform the lifting accordingly.
	 * - We start from the smallest level (0, 2, ..., dim-1) where lifting is still possible.
	 */

	maxDepth = (unsigned)this->sampleTree.max_depth(sampleTreeRoot);
	// invariant: either the last level is completely developed (dim or 0), or something in between due to bounds
	assert(maxDepth == (unsigned)dim || maxDepth == (unsigned)0 || boundsNontrivial);
	CARL_LOG_TRACE("carl.cad", __func__ << ": Phase 3");

	while (true) {
		//bool didProgress = false;
		// search base level with open lifting position
		int level = (int)dim - 1;
		CARL_LOG_TRACE("carl.cad", "Elimination sets:");
		for (; level >= 0; level--) {
			// stop at the first level which has a non-empty lifting queue
			CARL_LOG_TRACE("carl.cad", level << " -> " << eliminationSets[(unsigned)level]);
			if (!this->eliminationSets[(unsigned)level].emptyLiftingQueue()) break;
		}
		if (level == -1) {
			// no lifting position available, try elimination up to some level
			// now !mEliminationSets[level].emptyLiftingQueue() or level == -1
			level = this->eliminate(dim-1, bounds, boundsNontrivial);
			if (level == -1) {
				// no lifting position was able to be created in the base level or a level before this
				break;
			}
			// reset all lifting positions before this level
			for (unsigned l = 0; (int)l < level; l++) {
				this->eliminationSets[l].resetLiftingPositionsFully();
				this->eliminationSets[l].setLiftingPositionsReset();
			}
		}

		// lift all nodes at the corresponding tree depth according to the found lifting positions
		unsigned depth = (unsigned)((int)dim - level - 1);
		CARL_LOG_TRACE("carl.cad", "Current depth = " << depth << ", level = " << level);
		CARL_LOG_TRACE("carl.cad", this->sampleTree);
		assert(depth >= 0 && depth < dim);
		assert(depth <= (unsigned)this->sampleTree.max_depth());
		for (auto node = this->sampleTree.begin_depth(depth); node != this->sampleTree.end_depth(); node++) {
			// traverse all nodes at depth, i.e., sample points of dimension dim - level - 1 equaling the number of coefficient variables of the lifting position at level
			std::list<RealAlgebraicNumber<Number>> sampleList = this->constructSampleAt(node, sampleTreeRoot);
			// no degenerate sample points are considered here because they were already discarded in Phase 2
			if (depth != sampleList.size()) continue;

			RealAlgebraicPoint<Number> sample(sampleList);
			if (PERFORM_PARTIAL_CHECK) {
				CARL_LOG_DEBUG("carl.cad", "Partial check for " << *node);
				auto partialAnswer = partialLiftCheck(node, conflictGraph);
				if (partialAnswer == cad::Answer::False) continue;
			}
			bool boundsOK = true;
			// offset for incomplete samples (sample is filled from behind)
			std::size_t firstLevel = mVariables.size() - sample.dim();
			// test if the sample _r is already outside the bounds (boundsOK=false) or if it can be checked against the constraints or further lifted (boundsOK=true)
			for (const auto& i: bounds) {
				// bounds correspond to mVariables indices, so shift those indices by firstLevel to the left
				if (i.first < mVariables.size() && firstLevel <= i.first && !sample[i.first - firstLevel].containedIn(i.second)) {
					boundsOK = false;
					break;
				}
			}
			// this point did not match the bounds => continue searching
			if (!boundsOK) continue;
			
			// prepare the variables for lifting
			std::size_t i = dim;
			std::list<Variable> vars;
			// TODO: Check this
			//for (const auto& component: sampleList) {
			for (std::size_t j = 0; j < sampleList.size(); j++) {
				assert(i > 0);
				i--;
				vars.push_front(mVariables[i]);
			}
			assert(level + 1 == (int)i);
			// perform lifting at the incomplete leaf with the stored queue (reset performed in liftCheck)
			std::stack<std::size_t> satPath;
			CARL_LOG_DEBUG("carl.cad", "lifting on " << *node);
			cad::Answer status = liftCheck(node, i, false, vars, bounds, boundsNontrivial, checkBounds, r, conflictGraph, satPath);
			//didProgress = true;
			///@todo Handle answers
			if (status == cad::Answer::True) {
				// lifting yields a satisfying sample
				return cad::Answer::True;
			} else if (status == cad::Answer::Unknown) {
				return cad::Answer::Unknown;
			}
		}
		this->eliminationSets[(unsigned)level].setLiftingPositionsReset();
		//if (!didProgress) break;
	}

	if (!boundsNontrivial) {
		//std::cout << "Reseting lifting positions " << std::endl;
		// CAD is computed completely if there were no bounds used during elimination and lifting
		this->iscomplete = true;
		// all liftings were considered, so store the reset states
		for (auto& i: this->eliminationSets) {
			i.setLiftingPositionsReset();
		}
	}

	return cad::Answer::False;
}


template<typename Number>
typename CAD<Number>::sampleIterator CAD<Number>::storeSampleInTree(RealAlgebraicNumber<Number> newSample, sampleIterator node) {
	CARL_LOG_FUNC("carl.cad", newSample << ", " << *node);
	auto newNode = std::lower_bound(this->sampleTree.begin_children(node), this->sampleTree.end_children(node), newSample);
	if (newNode == this->sampleTree.end_children(node)) {
		newNode = this->sampleTree.append(node, newSample);
	} else if (*newNode == newSample) {
		assert(newSample.isRoot() || (!newNode->isRoot()));
		newNode = this->sampleTree.replace(newNode, newSample);
		assert(newNode.depth() <= mVariables.size());
	} else {
		newNode = this->sampleTree.insert(newNode, newSample);
		assert(newNode.depth() <= mVariables.size());
	}
	assert(this->sampleTree.isConsistent());
	return newNode;
}

template<typename Number>
cad::Answer CAD<Number>::baseLiftCheck(
		sampleIterator node,
		RealAlgebraicPoint<Number>& r,
		cad::ConflictGraph<Number>& conflictGraph
) {
	// check whether an interuption flag is set
	if (this->anAnswerFound()) {
		// interrupt the check procedure
		this->interrupted = true;
		assert(this->sampleTree.isConsistent());
		CARL_LOG_TRACE("carl.cad", "Returning true as an answer was found");
		return cad::Answer::True;
	}
	std::vector<RealAlgebraicNumber<Number>> sample(sampleTree.begin_path(node), sampleTree.end_path());
	sample.pop_back();
	RealAlgebraicPoint<Number> t(std::move(sample));
	if ((this->setting.computeConflictGraph && mConstraints.satisfiedBy(t, getVariables(), conflictGraph)) ||
		(!this->setting.computeConflictGraph && mConstraints.satisfiedBy(t, getVariables()))) {
		r = t;
		CARL_LOG_TRACE("carl.cad", "Returning true as a satisfying sample was found");
		return cad::Answer::True;
	}
	CARL_LOG_TRACE("carl.cad", "Returning false...");
	return cad::Answer::False;
}

template<typename Number>
cad::Answer CAD<Number>::partialLiftCheck(
		sampleIterator node,
		cad::ConflictGraph<Number>& conflictGraph
) {
	std::vector<RealAlgebraicNumber<Number>> sample(sampleTree.begin_path(node), sampleTree.end_path());
	sample.pop_back();
	RealAlgebraicPoint<Number> t(std::move(sample));
	if ((this->setting.computeConflictGraph && mConstraints.satisfiedPartiallyBy(t, getVariables(), conflictGraph)) ||
		(!this->setting.computeConflictGraph && mConstraints.satisfiedPartiallyBy(t, getVariables()))) {
		return cad::Answer::True;
	}
	CARL_LOG_DEBUG("carl.cad", "Early abort for sample " << t);
	//if (sampleTree.is_leaf(node)) {
	//	sampleTree.append(node, RealAlgebraicNumber<Number>(0));
	//}
	return cad::Answer::False;
}

template<typename Number>
cad::Answer CAD<Number>::liftCheck(
		sampleIterator node,
		std::size_t openVariableCount,
		bool restartLifting,
		const std::list<Variable>& _variables,
		const BoundMap& bounds,
		bool boundsActive,
		bool checkBounds,
		RealAlgebraicPoint<Number>& r,
		cad::ConflictGraph<Number>& conflictGraph,
		std::stack<std::size_t>& satPath
) {
	if (this->anAnswerFound()) {
		this->interrupted = true;
		assert(this->sampleTree.isConsistent());
		return cad::Answer::True;
	}
	CARL_LOG_FUNC("carl.cad", *node << ", " << openVariableCount);
	CARL_LOG_DEBUG("carl.cad", "Lifting " << std::vector<RealAlgebraicNumber<Number>>(sampleTree.begin_path(node), sampleTree.end_path()));
	CARL_LOG_DEBUG("carl.cad", "Current state:" << std::endl << *this);
	assert(this->sampleTree.is_valid(node));

	if (checkBounds && boundsActive && (!node.isRoot())) {
		// bounds shall be checked and the level is non-empty
		// level should be non-empty
		assert(openVariableCount < mVariables.size());
		// see if bounds are given for the previous level
		auto bound = bounds.find(openVariableCount);
		if (bound != bounds.end()) {
			if (!node->containedIn(bound->second)) {
				return cad::Answer::False;
			}
		}
	}
	
	if (setting.ignoreRoots) {
		if (node->isRoot() && (!node->isIntegral())) return cad::Answer::False;
	}
	
	if (PERFORM_PARTIAL_CHECK) {
		auto partialAnswer = partialLiftCheck(node, conflictGraph);
		if (partialAnswer == cad::Answer::False) return cad::Answer::False;
	}
	
	//if (!node.isRoot()) {
	//	if (integerHeuristicActive(cad::IntegerHandling::SPLIT_ASSIGNMENT, openVariableCount) || integerHeuristicActive(cad::IntegerHandling::SPLIT_PATH, openVariableCount)) {
	//		if (!node->isIntegral()) {
	//			assert(openVariableCount < mVariables.size());
	//			CARL_LOG_DEBUG("carl.cad", "Variables: " << mVariables);
	//			CARL_LOG_DEBUG("carl.cad", "OpenVariableCount = " << openVariableCount);
	//			std::vector<RealAlgebraicNumber<Number>> sample(sampleTree.begin_path(node), sampleTree.end_path());
	//			sample.pop_back();
	//			r = RealAlgebraicPoint<Number>(std::move(sample));
	//			CARL_LOG_DEBUG("carl.cad", "Lazy split at " << r);
	//			return cad::Answer::Unknown;
	//		}
	//	}
	//}

	// base level: zero variables left to substitute => evaluate the constraint
	if (openVariableCount == 0) {
		return this->baseLiftCheck(node, r, conflictGraph);
	}

	// openVariableCount > 0: lifting
	// previous variable will be substituted next
	openVariableCount--;

	std::list<Variable> newVariables(_variables);
	// the first variable is always the last one lifted
	newVariables.push_front(mVariables[openVariableCount]);
	// see if bounds are given for this level
	auto bound = boundsActive ? bounds.find(openVariableCount) : bounds.end();
	bool boundActive = bounds.end() != bound;

	// restore the lifting queue.
	this->eliminationSets[openVariableCount].resetLiftingPositions(restartLifting);

	/*
	 * Main loop: performs all operations possible in one level > 0, in particular, 2 phases.
	 * Phase 1: Choose a lifting position and construct the corresponding samples.
	 * Phase 2: Choose a sample and trigger liftCheck for the next level with the chosen sample.
	 */

	// determines whether new samples shall be constructed regardless of other flags
	bool computeMoreSamples = false;
	// the current list of samples at this position in the sample tree
	cad::SampleSet<Number> currentSamples(setting.sampleOrdering);
	currentSamples.insert(this->sampleTree.begin_children(node), this->sampleTree.end_children(node));
	CARL_LOG_DEBUG("carl.cad", "Getting old sample points: " << currentSamples);
	// the current samples queue for this lifting process
	cad::SampleSet<Number> sampleSetIncrement(setting.sampleOrdering);
	std::forward_list<RealAlgebraicNumber<Number>> replacedSamples;

	// fill in a standard sample to ensure termination in the main loop
	if (boundActive) {
		// add the bounds as roots and appropriate intermediate samples and start the lifting with this initial list
		std::list<RealAlgebraicNumber<Number>> boundRoots;
		if (bound->second.lowerBoundType() != BoundType::INFTY) {
			boundRoots.push_back(RealAlgebraicNumber<Number>(bound->second.lower(), true));
		}
		if (bound->second.upperBoundType() != BoundType::INFTY) {
			boundRoots.push_back(RealAlgebraicNumber<Number>(bound->second.upper(), true));
		}
		if (boundRoots.empty()) {
			sampleSetIncrement.insert(this->samples(openVariableCount, {RealAlgebraicNumber<Number>(carl::center(bound->second), true)}, currentSamples, replacedSamples));
		} else {
			sampleSetIncrement.insert(this->samples(openVariableCount, boundRoots, currentSamples, replacedSamples));
		}
	} else {
		sampleSetIncrement.insert(this->samples(openVariableCount, {RealAlgebraicNumber<Number>(0, true)}, currentSamples, replacedSamples));
	}
	CARL_LOG_DEBUG("carl.cad", "Adding new samples " << sampleSetIncrement);

	while (true) {
		if (this->anAnswerFound()) break;
		CARL_LOG_TRACE("carl.cad", __func__ << ": Phase 1");
		/* Phase 1
		 * Lifting position choice and corresponding sample construction.
		 */
		// loop if no samples are present at all or heuristics according to the respective setting demand to continue with a new lifting position, construct new samples
		while (
			computeMoreSamples || !sampleSetIncrement.hasOptimal()
		) {
			CARL_LOG_TRACE("carl.cad", "computing more samples.");
			// disable blind sample construction
			computeMoreSamples = false;
			replacedSamples.clear();
			if (this->eliminationSets[openVariableCount].emptyLiftingQueue()) {
				// break if all lifting positions are considered or the level is empty
				break;
			}
			auto next = this->eliminationSets[openVariableCount].nextLiftingPosition();

			if (!node.isRoot()) {
				CARL_LOG_TRACE("carl.cad", "Calling samples() for " << mVariables[node.depth()-1]);
			}
			if (boundActive && this->setting.earlyLiftingPruningByBounds) {
				// found bounds for the current lifting variable => remove all samples outside these bounds
				sampleSetIncrement.insert(this->samples(openVariableCount, next, node, currentSamples, replacedSamples, bound->second));
			} else {
				sampleSetIncrement.insert(this->samples(openVariableCount, next, node, currentSamples, replacedSamples));
			}

			// replace all samples in the tree which were changed in the current samples list
			for (const auto& replacedSample: replacedSamples) {
				this->storeSampleInTree(replacedSample, node);
			}
			// discard lifting position just used for sample construction
			this->eliminationSets[openVariableCount].popLiftingPosition();
			// try to simplify the current samples even further
			auto simplification = sampleSetIncrement.simplify();
			if (simplification.second) {
				// simplifications are visible in currentSamples due to shared pointer.
				// only do a fast simplification.
				currentSamples.simplify(true);
			}
		}
		/*if (integerHeuristicActive(cad::IntegerHandling::SPLIT_EARLY, openVariableCount)) {
			Interval<Number> bound = Interval<Number>::unboundedInterval();
			if (checkBounds) {
				CARL_LOG_DEBUG("carl.cad", "Variables: " << mVariables);
				CARL_LOG_DEBUG("carl.cad", "OpenVariableCount = " << openVariableCount);
				CARL_LOG_DEBUG("carl.cad", "Retrieving bounds for " << mVariables[openVariableCount]);
				assert(openVariableCount < mVariables.size());
				auto b = bounds.find(openVariableCount);
				if (b != bounds.end()) bound = b->second;
			}
			CARL_LOG_DEBUG("carl.cad", "Checking if we should split early within " << bound);
			for (const auto& newSample: sampleSetIncrement) {
				if (!newSample.containedIn(bound)) continue;
				if (!newSample.isIntegral()) {
					std::vector<RealAlgebraicNumber<Number>> sample(sampleTree.begin_path(node), sampleTree.end_path());
					sample.pop_back();
					sample.insert(sample.begin(), newSample);
					r = RealAlgebraicPoint<Number>(std::move(sample));
					CARL_LOG_DEBUG("carl.cad", "Eager split at " << r);
					// there might still be samples left but not stored yet
					while (!sampleSetIncrement.empty()) {
						// store the remaining samples in the sample tree (without lifting)
						this->storeSampleInTree(sampleSetIncrement.next(), node);
						sampleSetIncrement.pop();
					}
					return cad::Answer::Unknown;
				}
			}
		}*/

		/* Phase 2
		 * Lifting of the current level.
		 */
		CARL_LOG_TRACE("carl.cad", __func__ << ": Phase 2");
		while (!sampleSetIncrement.empty()) {
			// iterate through all samples found by the next() method
			/*
			 * Sample choice
			 */
			if (!this->eliminationSets[openVariableCount].emptyLiftingQueue() && sampleSetIncrement.hasOptimal()) {
				computeMoreSamples = true;
				break;
			}
			CARL_LOG_DEBUG("carl.cad", "Getting next sample from heap: " << sampleSetIncrement.getHeap());
			RealAlgebraicNumber<Number> newSample = sampleSetIncrement.next();
			CARL_LOG_DEBUG("carl.cad", "Got " << newSample);

			// Sample storage
			auto newNode = this->storeSampleInTree(newSample, node);

			// Lifting
			// start lifting with the fresh new sample at the next level for *all* lifting positions
			CARL_LOG_DEBUG("carl.cad", "lifting on " << newSample);
			cad::Answer liftingSuccessful = this->liftCheck(newNode, openVariableCount, true, newVariables, bounds, boundsActive, checkBounds, r, conflictGraph, satPath);

			///@todo warum hier pop() und nicht oben jeweils nach dem get()?
			// Sample pop if lifting unsuccessful or at the last level, i.e. level == 0
			sampleSetIncrement.pop();


			bool integralityBacktracking = false;
			///@todo Handle answers
			if (liftingSuccessful == cad::Answer::True) {
				CARL_LOG_TRACE("carl.cad", "Lifting was successfull");
				// there might still be samples left but not stored yet
				while (!sampleSetIncrement.empty()) {
					// store the remaining samples in the sample tree (without lifting)
					this->storeSampleInTree(sampleSetIncrement.next(), node);
					sampleSetIncrement.pop();
				}
				if (this->setting.integerHandling == cad::IntegerHandling::BACKTRACK && !checkIntegrality(newNode)) {
					integralityBacktracking = true;
					CARL_LOG_ERROR("carl.cad", "Lifting was successful, but integrality is violated.");
				} else {
					return cad::Answer::True;
					CARL_LOG_TRACE("carl.cad", "Returning true as lifting was successful");
				}
			} else if (liftingSuccessful == cad::Answer::Unknown) {
				CARL_LOG_DEBUG("carl.cad", "Got unknown, propagating...");
				return liftingSuccessful;
			} else if (!satPath.empty()) {
				// Sample was SAT, but not integral.
				if (checkIntegrality(newNode)) {
					CARL_LOG_ERROR("carl.cad", "Found sample, but integrality failed.");
				}
			}
			if (integralityBacktracking) {
				std::size_t id = 0;
				bool root = false;
				for (auto it = sampleTree.begin_children(node); it != sampleTree.end_children(node); it++) {
					if (*it == *newNode) break;
					if (it->isRoot() != root) id++;
					root = it->isRoot();
				}
				satPath.push(id);
				return cad::Answer::False;
			}
		}
		if (this->eliminationSets[openVariableCount].emptyLiftingQueue()) {
			CARL_LOG_TRACE("carl.cad", "Lifting queue is empty");
			// all lifting positions used
			if (this->setting.equationsOnly || this->setting.inequalitiesOnly) {
				// there might still be samples not yet considered but unimportant for the current lifting
				CARL_LOG_TRACE("carl.cad", "storing in sample tree");
				while (!sampleSetIncrement.empty()) {
					// store the remaining samples in the sample tree (without lifting)
					this->storeSampleInTree(sampleSetIncrement.next(), node);
					sampleSetIncrement.pop();
				}
			}
			break;
		}
	}
	CARL_LOG_TRACE("carl.cad", "Returning false as nothing else happened");
	return cad::Answer::False;
}

template<typename Number>
int CAD<Number>::eliminate(std::size_t level, const BoundMap& bounds, bool boundsActive) {
	CARL_LOG_FUNC("carl.cad.elimination", level << ", " << bounds);
	while (true) {
		if (!this->eliminationSets[level].emptyLiftingQueue()) return (int)level;
		std::size_t l = level;
		// find the first level where elimination polynomials can be generated
		int ltmp = (int)l;
		do {
			ltmp--;
		} while (ltmp >= 0 && this->eliminationSets[(unsigned)ltmp].emptySingleEliminationQueue() && this->eliminationSets[(unsigned)ltmp].emptyPairedEliminationQueue());
		// check if no further elimination possible
		if (ltmp < 0) {
			return -1;
		}
		// eliminate one polynomial per level down to the current level
		l = (unsigned)ltmp + 1;

		if (boundsActive && this->setting.simplifyEliminationByBounds) {
			// eliminate from level l-1 to level l
			for (; l <= level; l++) {
				while (!this->eliminationSets[l-1].emptySingleEliminationQueue()) {
					// the polynomial can be analyzed for zeros
					const UPolynomial* p = this->eliminationSets[l-1].nextSingleEliminationPosition();
					CARL_LOG_DEBUG("carl.cad", "Checking whether " << *p << " vanishes in " << bounds);
					if (this->vanishesInBox(p, bounds, l-1)) break;
					// delete polynomial and try the next one
					this->eliminationSets[l-1].erase(p);
				}
				this->eliminationSets[l-1].eliminateNextInto(this->eliminationSets[l], mVariables[l], this->setting);
				// store level of successful elimination
				level = (unsigned)l;

				if (this->setting.removeConstants) {
					// get rid of all constants moved to the current level
					for (; l < this->eliminationSets.size(); l++) {
						this->eliminationSets[l-1].moveConstants(this->eliminationSets[l], mVariables[l]);
					}
					this->eliminationSets.back().removeConstants();
				}
				// possible change to the completeness status
				this->iscomplete = false;
				return (int)level;
			}
			assert(l == level+1);
			if (!this->setting.simplifyByRootcounting && level == this->eliminationSets.size()) {
				// possibly simplify base level (if not done by setting.simplifyByRootcounting)
				while (!this->eliminationSets.back().emptySingleEliminationQueue()) {
					// the polynomial can be analyzed for zeros (use paired-elimination queue because the single is always empty)
					const UPolynomial* p = this->eliminationSets.back().nextSingleEliminationPosition();
					CARL_LOG_DEBUG("carl.cad", "Checking whether " << *p << " vanishes in " << bounds);
					if (this->vanishesInBox(p, bounds, (unsigned)this->eliminationSets.size()-1)) break;
					// delete polynomial and try the next one
					this->eliminationSets.back().erase(p);
				}
			}
		} else {
			CARL_LOG_TRACE("carl.cad.elimination", "eliminate without bounds in level " << l);
			for (; l <= level; l++) {
				this->eliminationSets[l-1].eliminateNextInto(this->eliminationSets[l], mVariables[l], this->setting, false);
				CARL_LOG_TRACE("carl.cad", "eliminated" << std::endl << (l-1) << ": " << this->eliminationSets[l-1] << std::endl << l << ": " << this->eliminationSets[l]);
				level = (unsigned)l;
				if (this->setting.removeConstants) {
					// get rid of all constants moved to the current level
					for (; l < this->eliminationSets.size(); l++) {
						assert(l < this->eliminationSets.size());
						assert(l < mVariables.size());
						this->eliminationSets[l-1].moveConstants(this->eliminationSets[l], mVariables[l]);
					}
					this->eliminationSets.back().removeConstants();
				}
				// possible change to the completeness status
				this->iscomplete = false;
				CARL_LOG_TRACE("carl.cad", "returning from eliminate(): " << (int)level);
				return (int)level;
			}
		}
	}
}

template<typename Number>
Interval<Number> CAD<Number>::getBounds(const typename CAD<Number>::sampleIterator& parent, const RealAlgebraicNumber<Number> sample) const {
	if (this->sampleTree.begin(parent) == this->sampleTree.end(parent)) {
		// this tree level is empty
		return Interval<Number>::unboundedExactInterval();
	}
	// search for the left and right boundaries in the first variable eliminated
	auto node = std::lower_bound(this->sampleTree.begin(parent), this->sampleTree.end(parent), sample);
	auto neighbor = node;

	if (node == this->sampleTree.end(parent)) {
		// node is not in the tree level and all samples are smaller
		// well-defined since level non-empty
		neighbor--;
		if ((*neighbor)->isNumeric()) {
			return Interval<Number>((*neighbor)->value(), BoundType::STRICT, (*neighbor)->value()+1, BoundType::INFTY);
		} else {
			RealAlgebraicNumber<Number> nIR = static_cast<RealAlgebraicNumber<Number>>(*neighbor);
			return Interval<Number>(nIR->upper(), BoundType::WEAK, nIR->upper()+1, BoundType::INFTY);
		}
	} else if (node == this->sampleTree.begin(parent)) {
		// node is the left-most (intermediate) sample
		// well-defined since level non-empty
		neighbor++;
		if (neighbor == this->sampleTree.end(parent)) {
			return Interval<Number>::unboundedExactInterval();
		} else if ((*neighbor)->isNumeric()) {
			return Interval<Number>((*neighbor)->value()-1, BoundType::INFTY, (*neighbor)->value(), BoundType::STRICT);
		} else {
			RealAlgebraicNumber<Number> nIR = static_cast<RealAlgebraicNumber<Number>>(*neighbor);
			return Interval<Number>(nIR->lower()-1, BoundType::INFTY, nIR->lower(), BoundType::WEAK);
		}
	} else {
		// node has left neighbor
		auto leftNeighbor = node;
		leftNeighbor--;
		// well-defined since level non-empty
		neighbor++;

		if (neighbor == this->sampleTree.end(parent)) {
			if ((*leftNeighbor)->isNumeric()) {
				return Interval<Number>((*leftNeighbor)->value(), BoundType::STRICT, (*leftNeighbor)->value()+1, BoundType::INFTY);
			} else {
				RealAlgebraicNumber<Number> nIR = static_cast<RealAlgebraicNumber<Number>>(*leftNeighbor);
				return Interval<Number>(nIR->upper(), BoundType::WEAK, nIR->upper()+1, BoundType::INFTY);
			}
		} else if ((*neighbor)->isNumeric()) {
			if ((*leftNeighbor)->isNumeric()) {
				return Interval<Number>((*leftNeighbor)->value(), BoundType::STRICT, (*neighbor)->value()+1, BoundType::STRICT);
			} else {
				RealAlgebraicNumber<Number> nIR = static_cast<RealAlgebraicNumber<Number>>(*leftNeighbor);
				return Interval<Number>(nIR->upper(), BoundType::WEAK, (*neighbor)->value(), BoundType::STRICT);
			}
		} else {
			RealAlgebraicNumber<Number> nIR = static_cast<RealAlgebraicNumber<Number>>(*neighbor);
			if ((*leftNeighbor)->isNumeric()) {
				return Interval<Number>((*leftNeighbor)->value(), BoundType::STRICT, nIR->lower(), BoundType::WEAK);
			} else {
				RealAlgebraicNumber<Number> nlIR = static_cast<RealAlgebraicNumber<Number>>(*leftNeighbor);
				return Interval<Number>(nlIR->upper(), BoundType::WEAK, (*neighbor)->value(), BoundType::STRICT);
			}
		}
	}
}

template<typename Number>
void CAD<Number>::widenBounds(BoundMap&) {
}

template<typename Number>
void CAD<Number>::shrinkBounds(BoundMap& bounds, const RealAlgebraicPoint<Number>& r) {
	// the size of variables should be compatible to the dimension of the given point
	assert(this->anAnswerFound() || mVariables.size() == r.dim());

	for (unsigned level = 0; level < r.dim(); level++) {
		// shrink the bounds in each level
		auto bound = bounds.find(level);
		if (bounds.end() != bound) {
			// found bounds for this level
			if (r[level].isNumeric()) {
				// give point interval representing the exact numeric value of this component
				bound->second.setLowerBoundType(BoundType::WEAK);
				bound->second.setLower( r[level].value() );
				bound->second.setUpperBoundType(BoundType::WEAK);
				bound->second.setUpper( r[level].value() );
			} else {
				// find a narrow interval within the bounds but with preferably small number representations
				//assert(r[level] != 0); // non-numerical representations are by now only interval representations
				if (r[level].refineAvoiding(bound->second.lower()) || r[level].refineAvoiding(bound->second.upper())) {
					// found exact numeric representation anyway
					bound->second.setLowerBoundType(BoundType::WEAK);
					bound->second.setLower(r[level].value());
					bound->second.setUpperBoundType(BoundType::WEAK);
					bound->second.setUpper(r[level].value());
				} else {
					// translate given open interval into the bounds
					bound->second.setLowerBoundType(BoundType::STRICT);
					bound->second.setLower(r[level].getInterval().lower());
					bound->second.setUpperBoundType(BoundType::STRICT);
					bound->second.setUpper(r[level].getInterval().upper());
				}
			}
		}
	}
}

template<typename Number>
bool CAD<Number>::vanishesInBox(const UPolynomial* p, const BoundMap& box, std::size_t level, bool recuperate) {
	cad::CADSettings boxSetting = cad::CADSettings::getSettings();
	boxSetting.simplifyEliminationByBounds = false; // would cause recursion in vanishesInBox
	boxSetting.earlyLiftingPruningByBounds = true; // important for efficiency
	boxSetting.simplifyByRootcounting = false; // too much overhead
	boxSetting.simplifyByFactorization = true; // mandatory for a square-free basis
	boxSetting.preSolveByBounds = true; // important for efficiency
	boxSetting.computeConflictGraph = false; // too much overhead and not needed
	std::vector<Variable> vars;
	BoundMap bounds;
	// variable index for the cad box
	std::size_t j = 0;
	for (std::size_t i = level; i < mVariables.size(); i++) {
		// prune the variables not occurring in p in order to trim the cadBox in advance
		if (p->has(mVariables[i])) {
			// the variable is actually occurring in p
			vars.push_back(mVariables[i]);
			auto bound = box.find(i);
			if (box.end() != bound) {
				bounds[j++] = bound->second;
			}
		}
	}

	// optimization for equations not valid in general
	boxSetting.equationsOnly = vars.size() <= 1;
	CAD<Number> cadbox(static_cast<cad::PolynomialOwner<Number>*>(&this->polynomials));
	CARL_LOG_INFO("carl.core", "Now in nested CAD " << &cadbox);
	cadbox.polynomials.schedule(p, false);
	cadbox.mVariables.setNewVariables(vars);
	cadbox.setting = boxSetting;

	RealAlgebraicPoint<Number> r;
	std::vector<cad::Constraint<Number>> cons(1, cad::Constraint<Number>(MultivariatePolynomial<Number>(*p), Sign::ZERO, mVariables.getCurrent()));
	if (cadbox.check(cons, r, bounds, false, false) == cad::Answer::True) {
		cadbox.completeElimination();
		CARL_LOG_TRACE("carl.core", "Back from nested CAD " << &cadbox);
		if (recuperate) {
			// recuperate eliminated polynomials and go on with the elimination
			std::size_t k = 0;
			for (std::size_t i = level + 1; i < mVariables.size(); i++) {
				// we start with level + 1 because p is already in mEliminationSets[level]
				// search for the variables actually occurring in cadBox
				while (k < cadbox.mVariables.size() && mVariables[i] != cadbox.mVariables[k]) {
					// cadBox.mVariables are ordered in the same way as mVariables and a subset of mVariables
					k++;
				}
				if (k >= cadbox.mVariables.size()) break;
				// recuperate the elimination polynomials corresponding to i
				// insert NOT avoiding single elimination (there might be elimination steps not done yet)
				this->eliminationSets[i].insert(cadbox.eliminationSets[k], false);
			}
		}
		CARL_LOG_INFO("carl.core", "Back from nested CAD " << &cadbox);
		return true;
	}
	CARL_LOG_INFO("carl.core", "Back from nested CAD " << &cadbox);
	return false;
}

}
