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

#include "../core/logging.h"
#include "../interval/IntervalEvaluation.h"
#include "../core/RealAlgebraicNumberSettings.h"
#include "../core/rootfinder/RootFinder.h"

namespace carl {

template<typename Number>
unsigned CAD<Number>::checkCallCount = 0;

template<typename Number>
CAD<Number>::CAD():
		variables(),
		sampleTree(),
		eliminationSets(),
		polynomials(),
		iscomplete(false),
		interrupted(false),
		interrupts(),
		setting(cad::CADSettings::getSettings())
{
	// initialize root with empty node
	this->sampleTree.setRoot(nullptr);
}

template<typename Number>
CAD<Number>::CAD(cad::PolynomialOwner<Number>* parent):
		variables(),
		sampleTree(),
		eliminationSets(),
		polynomials(parent),
		iscomplete(false),
		interrupted(false),
		interrupts(),
		setting(cad::CADSettings::getSettings())
{
	// initialize root with empty node
	this->sampleTree.setRoot(nullptr);
}

template<typename Number>
CAD<Number>::CAD(const std::vector<std::atomic_bool*>& i):
		CAD()
{
	this->interrupts = i;
}

template<typename Number>
CAD<Number>::CAD(const cad::CADSettings& setting):
		CAD()
{
	this->setting = setting;
}

template<typename Number>
CAD<Number>::CAD(const std::list<const UPolynomial*>& s, const std::vector<Variable>& v, const cad::CADSettings& setting):
		CAD()
{
	this->scheduledPolynomials.assign(s.begin(), s.end());
	this->variables.setNewVariables(v);
	this->setting = setting;
	this->prepareElimination();
}

template<typename Number>
CAD<Number>::CAD(const std::list<const UPolynomial*>& s, const std::vector<Variable>& v, const std::vector<std::atomic_bool*>& c, const cad::CADSettings& setting):
		CAD(s, v, setting)
{
	this->interrupts = c;
}

template<typename Number>
CAD<Number>::CAD(const CAD<Number>& cad):
		variables( cad.variables ),
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
	size_t dim  = this->variables.size();
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
	if( !constraints.empty() ){
		std::ofstream smtlibFile;
		smtlibFile.open(filename);
		smtlibFile << "(set-logic QF_NRA)\n(set-info :smt-lib-version 2.0)\n";
		// add all real-valued variables
		for (const auto& var: this->variables)
			smtlibFile << "(declare-fun " << var << " () Real)\n";

		smtlibFile << "(assert (and ";
		for (const auto& constraint: constraints) {
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
	//os << endl << cad.getSetting() << endl;
	os << "Variables: " << cad.variables << std::endl;
	os << "Polynomials: " << cad.polynomials << std::endl;
	os << "Elimination sets:" << std::endl;
	unsigned level = 0;
	for (const auto& i: cad.getEliminationSets()) {
		os << "\tLevel " << level++ << ": " << i << std::endl;
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
	if (this->variables.newEmpty() && (!polynomials.hasScheduled() || variables.empty())) {
		return false;
	}
	
	std::size_t newVariableCount = this->variables.newSize();
	
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

	if (!this->variables.newEmpty()) {
		// introduce new elimination levels and fill them appropriately
		// (1)
		
		CARL_LOG_TRACE("carl.cad", "Adding " << this->variables.newSize() << " to " << this->variables.size() << " old variables.");
		// variables, newVariables = newVariables:variables, []
		this->variables.appendNewToCur();
		
		// (1)
		/// @todo make this more efficient
		std::vector<cad::EliminationSet<Number>> sets(this->variables.size(), cad::EliminationSet<Number>(&this->polynomials, this->setting.order, this->setting.order));
		for (long unsigned i = newVariableCount; i < sets.size(); i++) {
			std::swap(sets[i], this->eliminationSets[i - newVariableCount]);
		}
		std::swap(this->eliminationSets, sets);
	}
	
	// add new polynomials to level 0, unifying their variables, and the list of all polynomials
	for (const auto& p: polynomials.getScheduled()) {
		auto tmp = p;
		if (p->mainVar() != this->variables.front()) {
			tmp = new UPolynomial(p->switchVariable(this->variables.front()));
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
	if (this->setting.simplifyByRootcounting && this->variables.size() == 1) {
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

template<typename Number>
void CAD<Number>::completeElimination(const CAD<Number>::BoundMap& bounds) {
	this->prepareElimination();
	bool useBounds = !bounds.empty();
	for (const auto& b: bounds) {
		useBounds = useBounds && !b.second.isUnbounded();
	}
	
	if (useBounds) {
		// construct constraints and polynomials representing the bounds
		for (const auto& b: bounds) {
			std::size_t l = b.first;
			if (l >= this->variables.size()) continue;
			// construct bound-related polynomials
			std::list<const UPolynomial*> tmp;
			if (b.second.lowerBoundType() != BoundType::INFTY) {
				tmp.push_back(this->polynomials.take(new UPolynomial(this->variables[l], {MPolynomial(-b.second.lower()), MPolynomial(1)})));
				if (!this->setting.earlyLiftingPruningByBounds) {
					// need to add bound polynomial if no bounds are generated automatically
					this->eliminationSets[b.first].insert(tmp.back());
				}
			}
			if (b.second.upperBoundType() != BoundType::INFTY) {
				tmp.push_back(this->polynomials.take(new UPolynomial(this->variables[l], {MPolynomial(-b.second.upper()), MPolynomial(1)})));
				if (!this->setting.earlyLiftingPruningByBounds) {
					// need to add bound polynomial if no bounds are generated automatically
					this->eliminationSets[l].insert(tmp.back());
				}
			}
			
			// eliminate bound-related polynomials
			l++;
			while (!tmp.empty() && l < this->variables.size()) {
				std::list<const UPolynomial*> tmp2;
				for (const auto& p: tmp) {
					auto res = this->eliminationSets[l-1].eliminateInto(p, this->eliminationSets[l], this->variables[l], this->setting);
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
				if (!this->vanishesInBox(p, bounds, l-1)) {
					this->eliminationSets[l-1].erase(p);
				}
			}
			while (!this->eliminationSets[l-1].emptyPairedEliminationQueue()) {
				this->eliminationSets[l-1].eliminateNextInto(this->eliminationSets[l], this->variables[l], this->setting);
			}
		}
	} else {
		// unbounded elimination from level l-1 to level l
		for (unsigned l = 1; l < this->eliminationSets.size(); l++) {
			while (	!this->eliminationSets[l-1].emptySingleEliminationQueue() ||
					!this->eliminationSets[l-1].emptyPairedEliminationQueue()) {
				this->eliminationSets[l-1].eliminateNextInto(this->eliminationSets[l], this->variables[l], this->setting, false);
			}
		}
	}
}

template<typename Number>
void CAD<Number>::clear() {
	this->variables.clear();
	this->sampleTree.clear();
	// Add empty root node
	this->sampleTree.insert(this->sampleTree.begin(), nullptr);
	this->eliminationSets.clear();
	this->polynomials.clear();
	this->scheduledPolynomials.clear();
	this->iscomplete = false;
	this->interrupted = false;
	this->interrupts.clear();
	this->checkCallCount = 0;
}

template<typename Number>
void CAD<Number>::complete() {
	RealAlgebraicPoint<Number> r;
	assert(this->variables.size() > 0);
	std::vector<cad::Constraint<Number>> c(1, cad::Constraint<Number>(UPolynomial(this->variables.front(), MPolynomial(1)), Sign::ZERO, this->variables));
	this->check(c, r, true);
}

template<typename Number>
bool CAD<Number>::check(
	std::vector<cad::Constraint<Number>>& _constraints,
	RealAlgebraicPoint<Number>& r,
	cad::ConflictGraph& conflictGraph,
	BoundMap& bounds,
	bool next,
	bool checkBounds)
{
	this->constraints = _constraints;
    #ifdef LOGGING_CARL
	CARL_LOG_DEBUG("carl.cad", "Checking the system");
	for (const auto& c: constraints) CARL_LOG_DEBUG("carl.cad", "  " << c);
	CARL_LOG_DEBUG("carl.cad", "within " << ( bounds.empty() ? "no bounds." : "these bounds:" ));
	for (const auto& b: bounds) {
		if (this->variables.size() > b.first) { CARL_LOG_DEBUG("carl.cad", "  " << b.second << " for variable " << this->variables[b.first]); }
		else { CARL_LOG_DEBUG("carl.cad", "  " << b.second << " for variable " << b.first); }
	}
	for (unsigned i = 0; i < this->eliminationSets.size(); i++) {
		CARL_LOG_DEBUG("carl.cad", "  Level " << i << " (" << this->eliminationSets[i].size() << "): " << this->eliminationSets[i]);
	}
    #endif
	assert(this->isSampleTreeConsistent());

#ifdef CAD_CHECK_REDIRECT
	CAD<Number>::checkCallCount++;
	this->setting.trimVariables = false;
	this->prepareElimination();
	std::string filename = "cad";
	std::stringstream stream;
	stream << this;
	filename += stream.str() + "_constraints";
	stream.str("");
	stream << CAD<Number>::checkCallCount;
	filename += stream.str() + ".smt2";
	CARL_LOG_INFO("carl.cad", "Redirecting call to file " << filename << "...");
	
	// add bounds to the constraints
	for (const auto& b: bounds) {
		if (b.first >= this->variables.size()) continue;
		
		switch (b.second.lowerBoundType()) {
			case BoundType::INFTY:
				break;
			case BoundType::STRICT:
				constraints.emplace_back(UPolynomial(this->variables[b.first], {MPolynomial(-b.second.lower()), MPolynomial(1)}), Sign::POSITIVE, this->variables);
				break;
			case BoundType::WEAK:
				constraints.emplace_back(UPolynomial(this->variables[b.first], {MPolynomial(-b.second.lower()), MPolynomial(1)}), Sign::NEGATIVE, this->variables, true);
				break;
			default:
				assert(false);
		}
		
		switch (b.second.upperBoundType()) {
			case BoundType::INFTY:
				break;
			case BoundType::STRICT:
				constraints.emplace_back(UPolynomial(this->variables[b.first], {MPolynomial(-b.second.upper()), MPolynomial(1)}), Sign::NEGATIVE, this->variables);
				break;
			case BoundType::WEAK:
				constraints.emplace_back(UPolynomial(this->variables[b.first], {MPolynomial(-b.second.upper()), MPolynomial(1)}), Sign::POSITIVE, this->variables, true);
				break;
			default:
				assert(false);
		}
	}
	this->printConstraints(filename);
	CARL_LOG_INFO("carl.cad", "done.");
#endif
	
	//////////////////////
	// Initialization
	
	this->interrupted = false;
	bool useBounds = false;
	bool onlyStrictBounds = true;
	for (const auto& b: bounds) {
		if (!b.second.isUnbounded() && !b.second.isEmpty()) {
			useBounds = true;
		}
		if (b.second.lowerBoundType() == BoundType::WEAK || b.second.upperBoundType() == BoundType::WEAK) {
			onlyStrictBounds = false;
		}
	}

	if (this->setting.computeConflictGraph) {
		// add necessary conflict graph vertices if required
		for (std::size_t i = conflictGraph.size(); i < constraints.size(); i++) {
			conflictGraph.addConstraintVertex();
		}
	}
	
	//////////////////////
	// Preprocessing
	
	// check bounds for empty interval
	for (const auto& b: bounds) {
		CARL_LOG_DEBUG("carl.cad", "Checking bound " << b.first << " : " << b.second);
		if (b.second.isEmpty()) return false;
	}
	if (constraints.empty() && useBounds) {
		// each bound non-empty plus empty input constraints
		return true;
	}
	
	// try to solve the constraints by interval arithmetic
	if (this->setting.preSolveByBounds) {
		typename Interval<Number>::evalintervalmap m;
		for (const auto& b: bounds) {
			if (b.first >= this->variables.size()) continue;
			m[this->variables[b.first]] = b.second;
		}
		if (!m.empty()) {
			// there are bounds we can use
			for (const auto& constraint: constraints) {
				/// @todo A Constraint may be negated.
				if (IntervalEvaluation::evaluate(constraint.getPolynomial(), m).sgn() != constraint.getSign()) {
					// the constraint is unsatisfiable!
					// this constraint is already the minimal infeasible set, so switch it with the last position in the constraints list
					exit(123);
					//std::swap(constraints.back(), constraint);
					conflictGraph = cad::ConflictGraph();
					return false;
				}
				// else: no additional check is needed!
			}
		}
	}
	
	// separate treatment of equations and inequalities
	cad::CADSettings backup = this->setting;
	if (this->setting.autoSeparateEquations) {
		std::vector<cad::Constraint<Number>> equations;
		std::vector<cad::Constraint<Number>> strictInequalities;
		std::vector<cad::Constraint<Number>> weakInequalities;
		
		for (cad::Constraint<Number> c: constraints) {
			if (c.getSign() == Sign::ZERO && !c.isNegated()) {
				equations.push_back(c);
			} else if (c.getSign() != Sign::ZERO && c.isNegated()) { 
				weakInequalities.push_back(c);
			} else {
				strictInequalities.push_back(c);
			}
		}
		if (weakInequalities.empty()) {
			if (!useBounds && strictInequalities.empty() && this->variables.size() <= 1) {
				// root-only samples not valid in general!
				this->alterSetting(cad::CADSettings::getSettings(cad::EQUATIONSONLY, rootfinder::SplittingStrategy::DEFAULT, this->setting));
			} else if (onlyStrictBounds && equations.empty()) {
				this->alterSetting(cad::CADSettings::getSettings(cad::INEQUALITIESONLY, rootfinder::SplittingStrategy::DEFAULT, this->setting));
			}
		}
		// else: mixed case, no optimization possible without zero-dimensional assumption
	}
	
	//////////////////////
	// Main check procedure
	
	this->prepareElimination();
	
	std::vector<std::pair<const UPolynomial*, const UPolynomial*>> boundPolynomials(this->variables.size(), std::pair<UPolynomial*, UPolynomial*>());
	CARL_LOG_TRACE("carl.cad", "Creating boundPolynomials of size " << this->variables.size());
	
	if (useBounds) {
		CARL_LOG_DEBUG("carl.cad", "Preprocess bounds");
		// construct constraints and polynomials representing the bounds
		for (const auto& b: bounds) {
			if (b.first >= this->variables.size()) continue;
			
			// construct bound-related polynomials
			std::list<const UPolynomial*> tmp;
			if (b.second.lowerBoundType() != BoundType::INFTY) {
				UPolynomial p(this->variables[b.first], {MPolynomial(Term<Number>(-b.second.lower())), MPolynomial(Term<Number>(1))});
				tmp.push_back(this->polynomials.take(new UPolynomial(p.pseudoPrimpart())));
				this->eliminationSets[b.first].insert(tmp.back());
				this->iscomplete = false; // new polynomials induce new sample points
				assert(b.first < boundPolynomials.size());
				boundPolynomials[b.first].first = tmp.back();
			}
			if (b.second.upperBoundType() != BoundType::INFTY) {
				UPolynomial p(this->variables[b.first], {MPolynomial(Term<Number>(-b.second.upper())), MPolynomial(Term<Number>(1))});
				tmp.push_back(this->polynomials.take(new UPolynomial(p.pseudoPrimpart())));
				this->eliminationSets[b.first].insert(tmp.back());
				this->iscomplete = false; // new polynomials induce new sample points
				boundPolynomials[b.first].first = tmp.back();
			}
			
			// eliminate bound-related polynomials only
			// l: variable index of the elimination destination
			std::size_t l = b.first + 1;
			while (!tmp.empty() && l < this->variables.size()) {
				std::list<const UPolynomial*> tmp2;
				for (const auto& p: tmp) {
					auto res = this->eliminationSets[l-1].eliminateInto(p, this->eliminationSets[l], this->variables[l], this->setting);
					tmp2.insert(tmp2.begin(), res.begin(), res.end());
				}
				std::swap(tmp, tmp2);
				l++;
			}
		}
	}
	
	// call the main check function according to the settings
	CARL_LOG_DEBUG("carl.cad", "Calling mainCheck...");
	bool satisfiable = this->mainCheck(bounds, r, conflictGraph, next, useBounds, checkBounds);
	CARL_LOG_DEBUG("carl.cad", "mainCheck returned " << satisfiable);
	
	if (useBounds) {
		CARL_LOG_DEBUG("carl.cad", "Postprocess bounds");
		// possibly tweak the bounds
		if (this->setting.improveBounds) {
			if (satisfiable) {
				this->shrinkBounds(bounds, r);
			} else {
				this->widenBounds(bounds);
			}
		}
		
		// restore elimination polynomials to their previous state due to possible bound-related simplifications
		for (unsigned l = 0; l < this->variables.size(); l++) {
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
		if (this->setting.simplifyEliminationByBounds) {
			// re-add the input polynomials to the top-level (for they could have been deleted)
			this->eliminationSets.front().clear();
			for (const auto& p: this->polynomials.getPolynomials()) {
				if (p->mainVar() == this->variables.front()) {
					this->eliminationSets.front().insert(p);
				} else {
					this->eliminationSets.front().insert(this->polynomials.take(new UPolynomial(p->switchVariable(this->variables.front()))));
				}
			}
		} else {
			// only reset the first elimination level
			this->eliminationSets.front().resetLiftingPositionsFully();
			this->eliminationSets.front().setLiftingPositionsReset();
		}
		for (unsigned l = 1; l < this->eliminationSets.size(); l++) {
			// reset every lifting queue besides the first elimination level
			this->eliminationSets[l].resetLiftingPositionsFully();
			this->eliminationSets[l].setLiftingPositionsReset();
		}
	}

	if (satisfiable) {
		CARL_LOG_DEBUG("carl.cad", "Result: sat (by sample point " << r << ")");
	} else {
		CARL_LOG_DEBUG("carl.cad", "Result: unsat");
	}
	CARL_LOG_TRACE("carl.cad", "Status:" << std::endl << *this);

	this->alterSetting(backup);
	return satisfiable;
}

template<typename Number>
void CAD<Number>::addPolynomial(const MPolynomial& p, const std::vector<Variable>& v) {
	CARL_LOG_TRACE("carl.cad", __func__ << "( " << p << ", " << v << " )");
	Variable var = v.front();
	if (!this->variables.empty()) var = this->variables.first();

	CARL_LOG_TRACE("carl.core", "Adding " << p);
	UPolynomial* up = new UPolynomial(p.toUnivariatePolynomial(var));
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
	this->variables.complete(v);
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
		// iterate from the leaves to the root (more efficient if several levels are to be cleaned)
		if (this->eliminationSets[(size_t)l].empty()) {
			// there is nothing more to be done for this level, so erase all samples
			unsigned depth = dim - (unsigned)l;
			assert(maxDepth <= this->sampleTree.max_depth());
			if (depth <= maxDepth) {
				// erase all samples on this level
				for (auto node = this->sampleTree.begin_depth(depth); node != this->sampleTree.end_depth(); ++node) {
					this->sampleTree.erase(node);
				}
				maxDepth = depth-1;
			}
		}
	}
}

template<typename Number>
std::vector<Interval<Number>> CAD<Number>::getBounds(const RealAlgebraicPoint<Number>& r) const {
	std::vector<Interval<Number>> bounds(this->variables.size());
	// initially, parent is the root
	auto parent = this->sampleTree.begin();
	
	for (int index = this->variables.size()-1; index >= 0; index--) {
		// tree is build upside down, index is in [mVariables.size()-1, 0]
		RealAlgebraicNumberPtr<Number> sample = r[index];
		if (this->sampleTree.begin(parent) == this->sampleTree.end(parent)) {
			// this tree level is empty
			bounds[index] = Interval<Number>::unboundedInterval();
			continue;
		}
		// search for the left and right boundaries in the first variable eliminated
		// does not compare less than r
		auto node = std::lower_bound(this->sampleTree.begin(parent), this->sampleTree.end(parent), sample, carl::less<RealAlgebraicNumberPtr<Number>>());
		
		bounds[index] = this->getBounds(node, sample);
		parent = node;
	}
	return bounds;
}

template<typename Number>
template<typename Inserter>
void CAD<Number>::addSamples(
		const RealAlgebraicNumberPtr<Number>& left,
		const RealAlgebraicNumberPtr<Number>& right,
		VariableType type,
		Inserter i
) {
	carl::Interval<Number> interval;
	if (left == nullptr) {
		if (right == nullptr) {
			i = RealAlgebraicNumberNR<Number>::create(carl::constant_zero<Number>::get(), false);
			return;
		} else if (right->isNumeric()) {
			i = RealAlgebraicNumberNR<Number>::create(carl::floor(right->value()) - 1, false);
			return;
		} else {
			auto rIR = std::static_pointer_cast<RealAlgebraicNumberIR<Number>>(right);
			i = RealAlgebraicNumberNR<Number>::create(carl::floor(rIR->getInterval().lower()) - 1, false);
			return;
		}
	} else if (left->isNumeric()) {
		if (right == nullptr) {
			i = RealAlgebraicNumberNR<Number>::create(carl::ceil(left->value()) + 1, false);
			return;
		} else if (right->isNumeric()) {
			interval.set(left->value(), right->value());
		} else {
			auto rIR = std::static_pointer_cast<RealAlgebraicNumberIR<Number>>(right);
			while (left->value() >= rIR->getInterval().lower()) rIR->refineAvoiding(left->value());
			interval.set(left->value(), rIR->getInterval().lower());
		}
	} else {
		auto lIR = std::static_pointer_cast<RealAlgebraicNumberIR<Number>>(left);
		if (right == nullptr) {
			i = RealAlgebraicNumberNR<Number>::create(carl::ceil(lIR->getInterval().upper()) + 1, false);
			return;
		} else if (right->isNumeric()) {
			while (lIR->getInterval().upper() >= right->value()) lIR->refineAvoiding(right->value());
			interval.set(lIR->getInterval().upper(), right->value());
		} else {
			auto rIR = std::static_pointer_cast<RealAlgebraicNumberIR<Number>>(right);
			while (lIR->getInterval().upper() >= rIR->getInterval().lower()) {
				lIR->refine();
				rIR->refine();
			}
			interval.set(lIR->getInterval().upper(), rIR->getInterval().lower());
		}
	}
	if (type == VariableType::VT_INT) {
		//std::cout << "Using integer exploration. Diameter: " << interval.diameter() << std::endl;
		if (interval.diameter() <= 1) {
			i = RealAlgebraicNumberNR<Number>::create(interval.sample(false), false);
		} else if (interval.diameter() < 7) {
			Number x = carl::ceil(interval.lower());
			while (interval.contains(x)) {
				i = RealAlgebraicNumberNR<Number>::create(x, false);
				x += carl::constant_one<Number>::get();
			}
		} else {
			i = RealAlgebraicNumberNR<Number>::create(carl::ceil(interval.lower()) + 1, false);
			i = RealAlgebraicNumberNR<Number>::create(interval.sample(false), false);
			i = RealAlgebraicNumberNR<Number>::create(carl::floor(interval.upper()) - 1, false);
		}
	} else {
		i = RealAlgebraicNumberNR<Number>::create(interval.sample(false), false);
	}
}

template<typename Number>
cad::SampleSet<Number> CAD<Number>::samples(
		std::size_t openVariableCount,
		const std::list<RealAlgebraicNumberPtr<Number>>& roots,
		cad::SampleSet<Number>& currentSamples,
		std::forward_list<RealAlgebraicNumberPtr<Number>>& replacedSamples,
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
		assert(!first->isRoot());
	}

	bool boundsActive = !bounds.isEmpty() && !bounds.isUnbounded();

	for (const auto& i: roots) {
		if (!i->containedIn(bounds)) continue;
		auto insertValue = currentSamples.insert(i);
		auto insertIt = std::get<0>(insertValue);
		if (!std::get<1>(insertValue)) {
			if (std::get<2>(insertValue)) {
				newSampleSet.insert(*insertIt);
				replacedSamples.push_front(*insertIt);
			}
		} else {
			// we found a new sample
			// add the root to new samples (with root switch on)
			newSampleSet.insert(*insertIt);
		}
		// local set storing the elements which shall be added to currentSampleSet and newSampleSet in the end
		std::list<RealAlgebraicNumberNRPtr<Number>> newSamples;
		
		/** Situation: One, next or previous, has to be a root (assumption) or we meet one of the outmost positions.
		 * --------|-------------------|-----------------|---
		 *    previous        insertValue.first         next
		 *     (root?)              (root)            (root?)
		 */
		
		//std::cout << *this << std::endl;
		carl::VariableType type = this->variables[openVariableCount].getType();
		if (!this->setting.exploreInteger) type = VariableType::VT_REAL;
		//std::cout << "Current var: " << openVariableCount << " -> " << this->variables[openVariableCount] << std::endl;
		//REGISTERED_ASSERT(openVariableCount > 1);
		// next: right neighbor
		auto neighbor = insertIt;
		// -> next (safe here, but need to check for end() later)
		neighbor++;
		if (neighbor == currentSamples.end()) {
			addSamples((*insertIt), nullptr, type, std::front_inserter(newSamples));
		} else if ((*neighbor)->isRoot()) {
			addSamples((*insertIt), (*neighbor), type, std::front_inserter(newSamples));
		}
		
		// previous: left neighbor
		neighbor = insertIt;
		if (neighbor == currentSamples.begin()) {
			addSamples(nullptr, (*insertIt), type, std::front_inserter(newSamples));
		} else {
			neighbor--;
			// now neighbor is the left bound (can be safely determined now)
			if ((*neighbor)->isRoot()) {
				addSamples((*neighbor), (*insertIt), type, std::front_inserter(newSamples));
			}
		}
		
		if (boundsActive) {
			// remove samples which do not lie within the (weak) bounds
			for (auto i = newSamples.begin(); i != newSamples.end(); ) {
				if (bounds.meets((*i)->value())) i++;
				else i = newSamples.erase(i);
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
		std::forward_list<RealAlgebraicNumberPtr<Number>>& replacedSamples,
		const Interval<Number>& bounds
) {
	assert(variables.size() == node.depth() + openVariableCount + 1);
	std::map<Variable, RealAlgebraicNumberPtr<Number>> m;
	auto valit = sampleTree.begin_path(node);
	for (std::size_t i = node.depth(); i > 0; i--) {
		m[variables[variables.size() - i]] = *valit;
		valit++;
	}
	return this->samples(
		openVariableCount,
		carl::rootfinder::realRoots(*p, m, bounds, this->setting.splittingStrategy),
		currentSamples,
		replacedSamples,
		bounds
	);
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
void CAD<Number>::alterSetting(const cad::CADSettings& setting) {
	// settings that require re-computation
	if (setting.order != this->setting.order) {
		// switch the order relation in all elimination sets
		for (auto& i: this->eliminationSets) {
			i.setLiftingOrder(setting.order);
		}
	}
	if (!this->setting.simplifyByRootcounting && setting.simplifyByRootcounting) {
		for (auto& i: this->eliminationSets) {
			i.removePolynomialsWithoutRealRoots();
		}
	}
	if (!this->setting.simplifyByFactorization && setting.simplifyByFactorization) {
		for (auto& i: this->eliminationSets) {
			i.factorize();
		}
	}
	
	this->setting = setting;
}

template<typename Number>
std::list<RealAlgebraicNumberPtr<Number>> CAD<Number>::constructSampleAt(sampleIterator node, const sampleIterator& root) const {
	/* Main sample construction loop macro augmented by a conditional argument for termination with an empty sample.
	 * @param _condition which has to be false for every node of the sample, otherwise an empty list is returned
	 */
	assert(this->sampleTree.begin() == root);
	if ((!this->sampleTree.is_valid(node) && *node == nullptr) || node == root) {
		// node is invalid
		return {};
	}
	
	std::list<RealAlgebraicNumberPtr<Number>> v;
	// proceed from the leaf up to the root while the children of root represent the last component of the sample point and the leaf the first
	if (this->setting.equationsOnly) {
		while (node != root) {
			if (!(*node)->isRoot()) return {};
			v.push_back(*node);
			node = this->sampleTree.get_parent(node);
		}
	} else if (this->setting.inequalitiesOnly) {
		while (node != root) {
			if ((*node)->isRoot()) return {};
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
std::pair<bool, bool> CAD<Number>::checkNode(
		sampleIterator node,
		bool fullRestart,
		bool excludePrevious,
		BoundMap& bounds,
		RealAlgebraicPoint<Number>& r,
		cad::ConflictGraph& conflictGraph,
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
		return std::make_pair(false, true);
	}
	RealAlgebraicPoint<Number> sample(sampleList);
	bool boundsOK = true;
	// offset for incomplete samples (sample is filled from behind)
	unsigned firstLevel = (unsigned)(this->variables.size() - sample.dim());
	CARL_LOG_TRACE("carl.cad", "first level " << firstLevel);
	
	// test if the sample _r is already outside the bounds (boundsOK=false) or if it can be checked against the constraints or further lifted (boundsOK=true)
	for (const auto& i: bounds) {
		// bounds correspond to mVariables indices, so shift those indices by firstLevel to the left
		if (i.first < this->variables.size() && firstLevel <= i.first && !sample[i.first - firstLevel]->containedIn(i.second)) {
			boundsOK = false;
			break;
		}
	}
	if (!boundsOK) {
		CARL_LOG_TRACE("carl.cad", "bound clash");
		// this point did not match the bounds => continue searching
		return std::make_pair(false, true);
	}
	if (sample.dim() == dim) {
		CARL_LOG_TRACE("carl.cad", "full dimension");
		// found a sample to check with the constraints
		if (excludePrevious) return std::make_pair(false, true);
		
		if (
			(this->setting.computeConflictGraph && constraints.satisfiedBy(sample, getVariables(), conflictGraph)) ||
			(!this->setting.computeConflictGraph && constraints.satisfiedBy(sample, getVariables()))
			) {
			r = sample;
			CARL_LOG_TRACE("carl.cad", "sample is good!");
			return std::make_pair(true, false);
		}
	} else {
		CARL_LOG_TRACE("carl.cad", "Incomplete sample " << sampleList << ", continue lifting");
		// found an incomplete sample, then first check the bounds and possibly restart lifting at the respective level
		// prepare the variables for lifting
		std::size_t i = dim;
		std::list<Variable> variables;
		// TODO: Check this
		//for (const auto& component: sampleList) {
		for (std::size_t j = 0; j < sampleList.size(); j++) {
			i--;
			variables.push_front(this->variables[i]);
		}
		// perform lifting at the incomplete leaf (without elimination, only by the current elimination polynomials)
		std::stack<std::size_t> satPath;
		if (this->liftCheck(node, i, fullRestart, variables, bounds, boundsNontrivial, checkBounds, r, conflictGraph, satPath)) {
			CARL_LOG_TRACE("carl.cad", "Incomplete sample " << sampleList << ", lifting succesfull");
			return std::make_pair(true, false);
		}
		CARL_LOG_TRACE("carl.cad", "Incomplete sample " << sampleList << ", lifting failed");
	}
	return std::make_pair(false, false);
}

template<typename Number>
bool CAD<Number>::mainCheck(
		BoundMap& bounds,
		RealAlgebraicPoint<Number>& r,
		cad::ConflictGraph& conflictGraph,
		bool next,
		bool boundsNontrivial,
		bool checkBounds
) {
	CARL_LOG_TRACE("carl.cad", __func__ << "( " << constraints << ", " << bounds << " )");

	if (this->variables.empty()) {
		// there are no valid samples available
		// if there is no constraint, all constraints are satisfied; otherwise no constraint
		return constraints.empty();
	}
	
	const std::size_t dim = this->variables.size();
	CARL_LOG_TRACE("carl.cad", "mainCheck: dimension is " << dim);
	auto sampleTreeRoot = this->sampleTree.begin();
	std::size_t tmp = this->sampleTree.max_depth(sampleTreeRoot);
	assert(tmp >= 0);
	unsigned maxDepth = (unsigned)tmp;
	// if the elimination sets were extended (i.e. the sample tree is not developed completely), we obtain new samples already in phase one
	next = next && (maxDepth == dim);
	
	// unify the variables for each constraint to match the CAD's variable order
	// @todo is this necessary? why?
	//for (unsigned i = 0; i < constraints.size(); ++i) {
	//	constraints[i].unifyVariables(this->variables);
	//}
	
	////////////
	// Main search strategy

	/* Three phases are preformed:
	 * Phase 1: Try to lift every sample ending in a node of the trace starting from the topmost node.
	 * Phase 2: Search the sample tree for already satisfying samples and lift the samples not yet lifted to the full dimension
	 *          (all possibly within given bounds).
	 *          Note that next == true skips...
	 * Phase 3: Lift at those sample tree nodes where lifting is still possible (possibly within given bounds).
	 */

	/* Phase 1
	 * Check or lift the possibly stored last sample point first.
	 *
	 * If the sample tree is non-trivial (maxDepth != 0), check the given sample point first.
	 * This can be, e.g., the sample which satisfied the last set of constraints.
	 */
	
	CARL_LOG_DEBUG("carl.cad", "mainCheck: Entering Phase 1...");
	
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
				return this->liftCheck(this->sampleTree.begin_leaf(), dim-this->sampleTree.begin_leaf().depth(), true, {}, bounds, boundsNontrivial, checkBounds, r, conflictGraph, satPath);
			}
			CARL_LOG_DEBUG("carl.cad", "Waiting for something to lift, lastRes = " << lastRes << std::endl << *this);
		};
		
		// perform an initial lifting step in order to fill the tree once
		std::stack<std::size_t> satPath;
		if (this->liftCheck(this->sampleTree.begin_leaf(), dim-this->sampleTree.begin_leaf().depth(), true, {}, bounds, boundsNontrivial, checkBounds, r, conflictGraph, satPath)) {
			// lifting yields a satisfying sample
			return true;
		}
	} else {
		CARL_LOG_TRACE("carl.cad", "maxDepth != 0, maxDepth = " << maxDepth);
		// the sample tree contains valid sample points
		for (auto leaf = this->sampleTree.begin_leaf(); leaf != this->sampleTree.end_leaf(); leaf++) {
			// traverse the current sample tree leaves for satisfying samples
			CARL_LOG_TRACE("carl.cad", this->sampleTree);
			auto res = this->checkNode(leaf, true, next, bounds, r, conflictGraph, boundsNontrivial, checkBounds, dim);
			if (res.first) return true;
			if (res.second) continue;
		}
	}
	CARL_LOG_TRACE("carl.cad", "Checking if CAD is complete");
	if (this->isComplete()) {
		// no leaf of the completely developed sample tree satisfied the constraints
		return false;
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
		// search base level with open lifting position
		int level = (int)dim - 1;
		for (; level >= 0; level--) {
			// stop at the first level which has a non-empty lifting queue
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
		assert(depth >= 0 && depth < dim);
		assert(depth <= (unsigned)this->sampleTree.max_depth());
		for (auto node = this->sampleTree.begin_depth(depth); node != this->sampleTree.end_depth(); ++node) {
			// traverse all nodes at depth, i.e., sample points of dimension dim - level - 1 equaling the number of coefficient variables of the lifting position at level
			std::list<RealAlgebraicNumberPtr<Number>> sampleList = this->constructSampleAt(node, sampleTreeRoot);
			// no degenerate sample points are considered here because they were already discarded in Phase 2
			if (depth != sampleList.size()) continue;
			
			RealAlgebraicPoint<Number> sample(sampleList);
			bool boundsOK = true;
			// offset for incomplete samples (sample is filled from behind)
			std::size_t firstLevel = this->variables.size() - sample.dim();
			// test if the sample _r is already outside the bounds (boundsOK=false) or if it can be checked against the constraints or further lifted (boundsOK=true)
			for (const auto& i: bounds) {
				// bounds correspond to mVariables indices, so shift those indices by firstLevel to the left
				if (i.first < this->variables.size() && firstLevel <= i.first && !sample[i.first - firstLevel]->containedIn(i.second)) {
					boundsOK = false;
					break;
				}
			}
			// this point did not match the bounds => continue searching
			if (!boundsOK) continue;
			
			// prepare the variables for lifting
			std::size_t i = dim;
			std::list<Variable> variables;
			// TODO: Check this
			//for (const auto& component: sampleList) {
			for (std::size_t j = 0; j < sampleList.size(); j++) {
				assert(i > 0);
				i--;
				variables.push_front(this->variables[i]);
			}
			assert(level + 1 == (int)i);
			// perform lifting at the incomplete leaf with the stored lifting queue (reset performed in liftCheck)
			std::stack<std::size_t> satPath;
			if (liftCheck(node, i, false, variables, bounds, boundsNontrivial, checkBounds, r, conflictGraph, satPath)) {
				// lifting yields a satisfying sample
				return true;
			}
		}
		this->eliminationSets[(unsigned)level].setLiftingPositionsReset();
	}
	
	if (!boundsNontrivial) {
		// CAD is computed completely if there were no bounds used during elimination and lifting
		this->iscomplete = true;
		// all liftings were considered, so store the reset states
		for (auto& i: this->eliminationSets) {
			i.setLiftingPositionsReset();
		}
	}
	
	return false;
}


template<typename Number>
typename CAD<Number>::sampleIterator CAD<Number>::storeSampleInTree(RealAlgebraicNumberPtr<Number> newSample, sampleIterator node) {
	CARL_LOG_FUNC("carl.cad", newSample << ", " << *node);
	auto newNode = std::lower_bound(this->sampleTree.begin_children(node), this->sampleTree.end_children(node), newSample, carl::less<RealAlgebraicNumberPtr<Number>>());
	if (newNode == this->sampleTree.end_children(node)) {
		newNode = this->sampleTree.append(node, newSample);
	} else if (carl::equal_to<RealAlgebraicNumberPtr<Number>>()(*newNode, newSample)) {
		assert(newSample->isRoot() || (!(*newNode)->isRoot()));
		newNode = this->sampleTree.replace(newNode, newSample);
		assert(newNode.depth() <= variables.size());
	} else {
		newNode = this->sampleTree.insert(newNode, newSample);
		assert(newNode.depth() <= variables.size());
	}
	return newNode;
}

template<typename Number>
bool CAD<Number>::baseLiftCheck(
		sampleIterator node,
		RealAlgebraicPoint<Number>& r,
		cad::ConflictGraph& conflictGraph
) {
	// check whether an interuption flag is set
	if (this->anAnswerFound()) {
		// interrupt the check procedure
		this->interrupted = true;
		CARL_LOG_TRACE("carl.cad", "Returning true as an answer was found");
		return true;
	}
	std::vector<RealAlgebraicNumberPtr<Number>> sample(sampleTree.begin_path(node), sampleTree.end_path());
	sample.pop_back();
	RealAlgebraicPoint<Number> t(std::move(sample));
	if ((this->setting.computeConflictGraph && constraints.satisfiedBy(t, getVariables(), conflictGraph)) ||
		(!this->setting.computeConflictGraph && constraints.satisfiedBy(t, getVariables()))) {
		r = t;
		CARL_LOG_TRACE("carl.cad", "Returning true as a satisfying sample was found");
		return true;
	}
	CARL_LOG_TRACE("carl.cad", "Returning false...");
	return false;
}

template<typename Number>
bool CAD<Number>::liftCheck(
		sampleIterator node,
		std::size_t openVariableCount,
		bool restartLifting,
		const std::list<Variable>& variables,
		const BoundMap& bounds,
		bool boundsActive,
		bool checkBounds,
		RealAlgebraicPoint<Number>& r,
		cad::ConflictGraph& conflictGraph,
		std::stack<std::size_t>& satPath
) {
	CARL_LOG_FUNC("carl.cad", *node << ", " << openVariableCount);
	assert(this->sampleTree.is_valid(node));
	if (checkBounds && boundsActive && (*node != nullptr)) {
		// bounds shall be checked and the level is non-empty
		// level should be non-empty
		assert(openVariableCount < this->variables.size());
		// see if bounds are given for the previous level
		auto bound = bounds.find(openVariableCount);
		if (bound != bounds.end()) {
			if (!(*node)->containedIn(bound->second)) {
				return false;
			}
		}
	}

	// base level: zero variables left to substitute => evaluate the constraint
	if (openVariableCount == 0) {
		return this->baseLiftCheck(node, r, conflictGraph);
	}
	
	// openVariableCount > 0: lifting
	// previous variable will be substituted next
	openVariableCount--;
	
	std::list<Variable> newVariables(variables);
	// the first variable is always the last one lifted
	newVariables.push_front(this->variables[openVariableCount]);
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
	// the current samples queue for this lifting process
	cad::SampleSet<Number> sampleSetIncrement(setting.sampleOrdering);
	std::forward_list<RealAlgebraicNumberPtr<Number>> replacedSamples;
	
	// fill in a standard sample to ensure termination in the main loop
	if (*node != nullptr) {
		CARL_LOG_TRACE("carl.cad", "Calling samples() for " << this->variables[node.depth() - 1]);
	}
	if (boundActive) {
		// add the bounds as roots and appropriate intermediate samples and start the lifting with this initial list
		std::list<RealAlgebraicNumberPtr<Number>> boundRoots;
		if (bound->second.lowerBoundType() != BoundType::INFTY) {
			boundRoots.push_back(RealAlgebraicNumberNR<Number>::create(bound->second.lower(), true));
		}
		if (bound->second.upperBoundType() != BoundType::INFTY) {
			boundRoots.push_back(RealAlgebraicNumberNR<Number>::create(bound->second.upper(), true));
		}
		if (boundRoots.empty()) {
			sampleSetIncrement.insert(this->samples(openVariableCount, {RealAlgebraicNumberNR<Number>::create(bound->second.center(), true)}, currentSamples, replacedSamples));
		} else {
			sampleSetIncrement.insert(this->samples(openVariableCount, boundRoots, currentSamples, replacedSamples));
		}
	} else {
		sampleSetIncrement.insert(this->samples(openVariableCount, {RealAlgebraicNumberNR<Number>::create(0, true)}, currentSamples, replacedSamples));
	}
	
	while (true) {
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
			
			if (*node != nullptr) {
				CARL_LOG_TRACE("carl.cad", "Calling samples() for " << this->variables[node.depth()-1]);
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
			RealAlgebraicNumberPtr<Number> newSample = sampleSetIncrement.next();

			// Sample storage
			auto newNode = this->storeSampleInTree(newSample, node);
			
			// Lifting
			// start lifting with the fresh new sample at the next level for *all* lifting positions
			bool liftingSuccessful = this->liftCheck(newNode, openVariableCount, true, newVariables, bounds, boundsActive, checkBounds, r, conflictGraph, satPath);
			
			///@todo warum hier pop() und nicht oben jeweils nach dem get()?
			// Sample pop if lifting unsuccessful or at the last level, i.e. level == 0
			sampleSetIncrement.pop();


			bool integralityBacktracking = false;
			if (liftingSuccessful) {
				CARL_LOG_TRACE("carl.cad", "Lifting was successfull");
				// there might still be samples left but not stored yet
				while (!sampleSetIncrement.empty()) {
					// store the remaining samples in the sample tree (without lifting)
					this->storeSampleInTree(sampleSetIncrement.next(), node);
					sampleSetIncrement.pop();
				}
				if (checkIntegrality(newNode)) {
					return true;
					CARL_LOG_TRACE("carl.cad", "Returning true as lifting was successful");
				} else {
					integralityBacktracking = true;
					CARL_LOG_ERROR("carl.cad", "Lifting was successful, but integrality is violated.");
				}
			} else if (!satPath.empty()) {
				// Sample was SAT, but not integral.
				if (checkIntegrality(newNode)) {
					CARL_LOG_ERROR("carl.cad", "Found sample, but integrality failed.");
				}
			}
			if (integralityBacktracking) {
				std::size_t id = 0;
				bool root = false;
				for (auto it = sampleTree.begin_children(node); it != sampleTree.end_children(node); ++it) {
					if (*it == *newNode) break;
					if ((*it)->isRoot() != root) id++;
					root = (*it)->isRoot();
				}
				satPath.push(id);
				return false;
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
	return false;
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
					if (this->vanishesInBox(p, bounds, l-1)) break;
					// delete polynomial and try the next one
					this->eliminationSets[l-1].erase(p);
				}
				this->eliminationSets[l-1].eliminateNextInto(this->eliminationSets[l], this->variables[l], this->setting);
				// store level of successful elimination
				level = (unsigned)l;
				
				if (this->setting.removeConstants) {
					// get rid of all constants moved to the current level
					for (; l < this->eliminationSets.size(); l++) {
						this->eliminationSets[l-1].moveConstants(this->eliminationSets[l], this->variables[l]);
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
					if (this->vanishesInBox(p, bounds, (unsigned)this->eliminationSets.size()-1)) break;
					// delete polynomial and try the next one
					this->eliminationSets.back().erase(p);
				}
			}
		} else {
			CARL_LOG_TRACE("carl.cad.elimination", "eliminate without bounds in level " << l);
			for (; l <= level; l++) {
				this->eliminationSets[l-1].eliminateNextInto(this->eliminationSets[l], this->variables[l], this->setting, false);
				CARL_LOG_TRACE("carl.cad", "eliminated" << std::endl << (l-1) << ": " << this->eliminationSets[l-1] << std::endl << l << ": " << this->eliminationSets[l]);
				level = (unsigned)l;
				if (this->setting.removeConstants) {
					// get rid of all constants moved to the current level
					for (; l < this->eliminationSets.size(); l++) {
						assert(l < this->eliminationSets.size());
						assert(l < this->variables.size());
						this->eliminationSets[l-1].moveConstants(this->eliminationSets[l], this->variables[l]);
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
Interval<Number> CAD<Number>::getBounds(const typename CAD<Number>::sampleIterator& parent, const RealAlgebraicNumberPtr<Number> sample) const {
	if (this->sampleTree.begin(parent) == this->sampleTree.end(parent)) {
		// this tree level is empty
		return Interval<Number>::unboundedExactInterval();
	}
	// search for the left and right boundaries in the first variable eliminated
	auto node = std::lower_bound(this->sampleTree.begin(parent), this->sampleTree.end(parent), sample, carl::less<RealAlgebraicNumberPtr<Number>>());
	auto neighbor = node;
	
	if (node == this->sampleTree.end(parent)) {
		// node is not in the tree level and all samples are smaller
		// well-defined since level non-empty
		neighbor--;
		if ((*neighbor)->isNumeric()) {
			return Interval<Number>((*neighbor)->value(), BoundType::STRICT, (*neighbor)->value()+1, BoundType::INFTY);
		} else {
			RealAlgebraicNumberIRPtr<Number> nIR = static_cast<RealAlgebraicNumberIRPtr<Number>>(*neighbor);
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
			RealAlgebraicNumberIRPtr<Number> nIR = static_cast<RealAlgebraicNumberIRPtr<Number>>(*neighbor);
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
				RealAlgebraicNumberIRPtr<Number> nIR = static_cast<RealAlgebraicNumberIRPtr<Number>>(*leftNeighbor);
				return Interval<Number>(nIR->upper(), BoundType::WEAK, nIR->upper()+1, BoundType::INFTY);
			}
		} else if ((*neighbor)->isNumeric()) {
			if ((*leftNeighbor)->isNumeric()) {
				return Interval<Number>((*leftNeighbor)->value(), BoundType::STRICT, (*neighbor)->value()+1, BoundType::STRICT);
			} else {
				RealAlgebraicNumberIRPtr<Number> nIR = static_cast<RealAlgebraicNumberIRPtr<Number>>(*leftNeighbor);
				return Interval<Number>(nIR->upper(), BoundType::WEAK, (*neighbor)->value(), BoundType::STRICT);
			}
		} else {
			RealAlgebraicNumberIRPtr<Number> nIR = static_cast<RealAlgebraicNumberIRPtr<Number>>(*neighbor);
			if ((*leftNeighbor)->isNumeric()) {
				return Interval<Number>((*leftNeighbor)->value(), BoundType::STRICT, nIR->lower(), BoundType::WEAK);
			} else {
				RealAlgebraicNumberIRPtr<Number> nlIR = static_cast<RealAlgebraicNumberIRPtr<Number>>(*leftNeighbor);
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
	assert(this->variables.size() == r.dim());
	
	for (unsigned level = 0; level < r.dim(); level++) {
		// shrink the bounds in each level
		auto bound = bounds.find(level);
		if (bounds.end() != bound) {
			// found bounds for this level
			if (r[level]->isNumeric()) {
				// give point interval representing the exact numeric value of this component
				bound->second.setLowerBoundType(BoundType::WEAK);
				bound->second.setLower( r[level]->value() );
				bound->second.setUpperBoundType(BoundType::WEAK);
				bound->second.setUpper( r[level]->value() );
			} else {
				// find a narrow interval within the bounds but with preferably small number representations
				RealAlgebraicNumberIRPtr<Number> rIR = std::static_pointer_cast<RealAlgebraicNumberIR<Number>>(r[level]);
				assert( rIR != 0 ); // non-numerical representations are by now only interval representations
				if (rIR->refineAvoiding(bound->second.lower()) || rIR->refineAvoiding(bound->second.upper())) {
					// found exact numeric representation anyway
					bound->second.setLowerBoundType(BoundType::WEAK);
					bound->second.setLower(r[level]->value());
					bound->second.setUpperBoundType(BoundType::WEAK);
					bound->second.setUpper(r[level]->value());
				} else {
					// translate given open interval into the bounds
					bound->second.setLowerBoundType(BoundType::STRICT);
					bound->second.setLower(rIR->lower());
					bound->second.setUpperBoundType(BoundType::STRICT);
					bound->second.setUpper(rIR->upper());
				}
			}
		}
	}
}

template<typename Number>
void CAD<Number>::trimVariables() {
	// tree is build upside down, depth is max_level - level + 1
	int depth = this->variables.size();
	int maxDepth = this->sampleTree.max_depth();
	// variables and elimination levels should always match
	assert(depth == this->eliminationSets.size());
	if (this->variables.empty()) return;
	
	// simultaneously remove elimination sets, variables and samples
	auto variable = this->variables.begin();
	for (auto eliminationSet = this->eliminationSets.begin(); eliminationSet != this->eliminationSets.end(); eliminationSet++) {
		if (eliminationSet->empty()) {
			/* In this level, *variable would have to be eliminated, but the level is empty (and not empty just because of certain bounds).
			 * Thus, there is no polynomial in further levels claiming this variable.
			 */
			if (eliminationSet != this->eliminationSets.begin()) {
				// check whether no previous elimination level claims the variable
				bool foundVariable = false;
				for (auto previous = eliminationSet-1; previous != this->eliminationSets.begin(); --previous) {
					for (const auto& p: previous) {
						if (p->has(*variable)) {
							foundVariable = true;
							break;
						}
					}
					if (foundVariable) break;
				}
				if (foundVariable) {
					eliminationSet++;
					continue;
				}
				// else: can safely remove the variable since it is not present in previous levels
			}
			// else: can safely remove the topmost variable

			eliminationSet = this->eliminationSets.erase(eliminationSet);
			variable = this->variables.erase(variable);
			if (depth <= maxDepth) {
				// remove the complete layer of samples from the sample tree at the given depth
				// fix the iterators to be deleted in a separate list independent of merging with the children
				std::queue<typename Tree<RealAlgebraicNumberPtr<Number>>::iterator> toDelete;
				for (auto node = this->sampleTree.begin_fixed(this->sampleTree.begin(), depth); this->sampleTree.is_valid(node) && depth == this->sampleTree.depth(node); node = this->sampleTree.next_at_same_depth(node)) {
					toDelete.push(node);
				}
				while (!toDelete.empty()) {
					// move all children of every node to be deleted, to the current level in a sorted manner, including the subtrees
					auto node = toDelete.front();
					auto parent = this->sampleTree.parent(node);
					for (auto child = this->sampleTree.begin(node); child != this->sampleTree.end(node); child++) {
						auto newNode = std::lower_bound(this->sampleTree.begin(parent), this->sampleTree.end(parent), *child, carl::less<RealAlgebraicNumberPtr<Number>>());
						if (newNode == this->sampleTree.end(parent)) {
							// the child is not contained in the siblings nor any child is greater than it
							this->sampleTree.append_child(parent, child);
						} else {
							// newNode is a sibling being greater than child or newNode == node or RealAlgebraicNumberFactory::equal( *newNode, *child )
							// makes sure that newNode does not occur as node in the future
							this->sampleTree.insert_subtree(newNode, child);
						}
						/* Remark:
						 * The current implementation permits several equal nodes as children of a node. This is semantically equivalent to
						 * merging the subtrees of equal nodes recursively.
						 * Approach for merging: reparent the child's children, sort the new children, unify them and proceed with reparenting recursively.
						 *
						 */
					}
					this->sampleTree.erase(node);
					toDelete.pop();
				}
			}
		} else {
			eliminationSet++;
			variable++;
		}
		depth--;
	}
}

template<typename Number>
bool CAD<Number>::vanishesInBox(const UPolynomial* p, const BoundMap& box, std::size_t level, bool recuperate) {
	cad::CADSettings boxSetting = cad::CADSettings::getSettings();
	boxSetting.simplifyEliminationByBounds = false; // would cause recursion in vanishesInBox
	boxSetting.earlyLiftingPruningByBounds = true; // important for efficiency
	boxSetting.simplifyByRootcounting = false; // too much overhead
	boxSetting.trimVariables = false; // not needed for nothing is removed
	boxSetting.simplifyByFactorization = true; // mandatory for a square-free basis
	boxSetting.preSolveByBounds = true; // important for efficiency
	boxSetting.computeConflictGraph = false; // too much overhead and not needed
	std::vector<Variable> variables;
	BoundMap bounds;
	// variable index for the cad box
	unsigned j = 0;
	for (std::size_t i = level; i < this->variables.size(); i++) {
		// prune the variables not occurring in p in order to trim the cadBox in advance
		if (p->has(this->variables[i])) {
			// the variable is actually occurring in p
			variables.push_back(this->variables[i]);
			auto bound = box.find(i);
			if (box.end() != bound) {
				bounds[j++] = bound->second;
			}
		}
	}
	
	// optimization for equations not valid in general
	boxSetting.equationsOnly = variables.size() <= 1;
	CAD<Number> cadbox(static_cast<cad::PolynomialOwner<Number>*>(&this->polynomials));
	CARL_LOG_INFO("carl.core", "Now in nested CAD " << &cadbox);
	cadbox.polynomials.schedule(p, false);
	cadbox.variables.setNewVariables(variables);
	cadbox.setting = boxSetting;
	
	RealAlgebraicPoint<Number> r;
	std::vector<cad::Constraint<Number>> constraints(1, cad::Constraint<Number>(MultivariatePolynomial<Number>(*p), Sign::ZERO, variables));
	if (cadbox.check(constraints, r, bounds, false, false)) {
		cadbox.completeElimination();
		CARL_LOG_TRACE("carl.core", "Back from nested CAD " << &cadbox);
		if (recuperate) {
			// recuperate eliminated polynomials and go on with the elimination
			std::size_t j = 0;
			for (std::size_t i = level + 1; i < this->variables.size(); i++) {
				// we start with level + 1 because p is already in mEliminationSets[level]
				// search for the variables actually occurring in cadBox
				while (j < cadbox.variables.size() && this->variables[i] != cadbox.variables[j]) {
					// cadBox.mVariables are ordered in the same way as mVariables and a subset of mVariables
					j++;
				}
				if (j >= cadbox.variables.size()) break;
				// recuperate the elimination polynomials corresponding to i
				// insert NOT avoiding single elimination (there might be elimination steps not done yet)
				this->eliminationSets[i].insert(cadbox.eliminationSets[j], false);
			}
		}
		CARL_LOG_INFO("carl.core", "Back from nested CAD " << &cadbox);
		return true;
	}
	CARL_LOG_INFO("carl.core", "Back from nested CAD " << &cadbox);
	return false;
}

}
