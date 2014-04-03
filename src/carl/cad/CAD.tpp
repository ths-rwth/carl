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
		trace(),
		eliminationSets(),
		polynomials(),
		scheduledPolynomials(),
		newVariables(),
		iscomplete(false),
		interrupted(false),
		interrupts(),
		setting(cad::CADSettings::getSettings())
{
	// initialize root with empty node
	this->sampleTree.insert(this->sampleTree.begin(), nullptr);
}

template<typename Number>
CAD<Number>::CAD(cad::PolynomialOwner<Number>* parent):
		cad::PolynomialOwner<Number>(parent),
		variables(),
		sampleTree(),
		trace(),
		eliminationSets(),
		polynomials(),
		scheduledPolynomials(),
		newVariables(),
		iscomplete(false),
		interrupted(false),
		interrupts(),
		setting(cad::CADSettings::getSettings())
{
	// initialize root with empty node
	this->sampleTree.insert(this->sampleTree.begin(), nullptr);
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
	this->newVariables = v;
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
		trace( cad.trace ),
		eliminationSets( cad.eliminationSets ),
		polynomials( cad.polynomials ),
		scheduledPolynomials( cad.scheduledPolynomials ),
		iscomplete( cad.iscomplete ),
		interrupted( cad.interrupted ),
		setting( cad.setting )
{
}

template<typename Number>
unsigned CAD<Number>::indexOf(const Variable& v) const {
	for(unsigned int i = 0; i < this->variables.size(); ++i) {
		if (v == this->variables[i]) return i;
	}
	assert(false);
}

template<typename Number>
cad::SampleSet<Number> CAD<Number>::samplesAt(const sampleIterator& node) const {
	cad::SampleSet<Number> samples;
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
void CAD<Number>::printConstraints(const std::vector<cad::Constraint<Number>>& constraints, const std::string& filename) const {
	if( !constraints.empty() ){
		std::ofstream smtlibFile;
		smtlibFile.open(filename);
		smtlibFile << "(set-logic QF_NRA)\n(set-info :smt-lib-version 2.0)\n";
		// add all real-valued variables
		for (auto var: this->variables)
			smtlibFile << "(declare-fun " << var << " () Real)\n";

		smtlibFile << "(assert (and ";
		for (auto constraint: constraints) {
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

template<typename Number>
std::ostream& operator<<(std::ostream& os, const CAD<Number>& cad) {
	//os << endl << cad.getSetting() << endl;
	os << "Variables: " << cad.variables << std::endl;
	os << "Elimination sets:" << std::endl;
	unsigned level = 0;
	for (auto i: cad.getEliminationSets()) {
		os << "\tLevel " << level++ << ": " << i << std::endl;
	}
	os << "Sample tree:" << std::endl;
	os << cad.sampleTree << std::endl;
	os << "Number of samples computed: " << cad.samples().size() << endl;
	os << "CAD complete: " << cad.isComplete() << endl;
	return os;
}

template<typename Number>
bool CAD<Number>::prepareElimination() {
	LOGMSG_TRACE("carl.cad", "prepareElimination()");
	if (this->newVariables.empty() && this->scheduledPolynomials.empty()) {
		return false;
	}
	
	long unsigned newVariableCount = this->newVariables.size();
	
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
	
	/// @todo Check if this code is actually necessary
	if (this->setting.simplifyByGroebner) {
		// add new variables to the variable list pool
		//for (auto v: this->newVariables) {
			// VariableListPool::addVariable(v);
		//}
	}
	if (!this->newVariables.empty()) {
		// introduce new elimination levels and fill them appropriately
		// (1)
		
		LOGMSG_TRACE("carl.cad", "Adding " << this->newVariables.size() << " to " << this->variables.size() << " old variables.");
		// variables, newVariables = newVariables:variables, []
		this->newVariables.insert(this->newVariables.end(), this->variables.begin(), this->variables.end());
		this->variables.clear();
		std::swap(this->variables, this->newVariables);
		
		// extend trace
		/// @todo make this more efficient
		CADTrace newTrace(this->variables.size()+1, this->sampleTree.begin());
		long unsigned j = newVariableCount;
		assert(j + this->trace.size() <= newTrace.size());
		for (auto i: this->trace) {
			newTrace[j++] = i;
		}
		std::swap(this->trace, newTrace);
		
		// (1)
		/// @todo make this more efficient
		std::vector<cad::EliminationSet<Number>> sets(this->variables.size(), cad::EliminationSet<Number>(this, this->setting.order, this->setting.order));
		for (long unsigned i = newVariableCount; i < sets.size(); i++) {
			std::swap(sets[i], this->eliminationSets[i - newVariableCount]);
		}
		std::swap(this->eliminationSets, sets);
	}
	
	// add new polynomials to level 0, unifying their variables, and the list of all polynomials
	for (auto p: this->scheduledPolynomials) {
		auto tmp = p;
		if (p->mainVar() != this->variables.front()) {
			tmp = this->take(new UPolynomial(p->switchVariable(this->variables.front())));
		}
		this->polynomials.push_back(tmp);
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
	this->scheduledPolynomials.clear();
	LOGMSG_TRACE("carl.cad", "Done with prepareElimination()");
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
	for (auto b: bounds) {
		useBounds = useBounds && !b.second.isUnbounded();
	}
	
	if (useBounds) {
		// construct constraints and polynomials representing the bounds
		for (auto b: bounds) {
			unsigned l = b.first;
			if (l >= this->variables.size()) continue;
			// construct bound-related polynomials
			std::list<const UPolynomial*> tmp;
			if (b.second.lowerBoundType() != BoundType::INFTY) {
				tmp.push_back(this->take(new UPolynomial(this->variables[l], {MPolynomial(-b.second.lower()), MPolynomial(1)})));
				if (!this->setting.earlyLiftingPruningByBounds) {
					// need to add bound polynomial if no bounds are generated automatically
					this->eliminationSets[b.first].insert(tmp.back());
				}
			}
			if (b.second.upperBoundType() != BoundType::INFTY) {
				tmp.push_back(this->take(new UPolynomial(this->variables[l], {MPolynomial(-b.second.upper()), MPolynomial(1)})));
				if (!this->setting.earlyLiftingPruningByBounds) {
					// need to add bound polynomial if no bounds are generated automatically
					this->eliminationSets[l].insert(tmp.back());
				}
			}
			
			// eliminate bound-related polynomials
			l++;
			while (!tmp.empty() && l < this->variables.size()) {
				std::list<const UPolynomial*> tmp2;
				for (auto p: tmp) {
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
	this->residueSampleTree.clear();
	this->trace.clear();
	this->eliminationSets.clear();
	this->polynomials.clear();
	this->scheduledPolynomials.clear();
	this->newVariables.clear();
	this->iscomplete = false;
	this->interrupted = false;
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
	std::vector<cad::Constraint<Number>>& constraints,
	RealAlgebraicPoint<Number>& r,
	cad::ConflictGraph& conflictGraph,
	BoundMap& bounds,
	Deductions& deductions,
	bool next,
	bool checkTraceFirst,
	bool checkBounds)
{
	LOGMSG_DEBUG("carl.cad", "Checking the system");
	for (auto c: constraints) LOGMSG_DEBUG("carl.cad", "  " << c);
	LOGMSG_DEBUG("carl.cad", "within " << ( bounds.empty() ? "no bounds." : "these bounds:" ));
	for (auto b: bounds) LOGMSG_DEBUG("carl.cad", "  " << b.second << " for variable " << b.first);
	for (unsigned i = 0; i < this->eliminationSets.size(); i++) {
		LOGMSG_DEBUG("carl.cad", "  Level " << i << " (" << this->eliminationSets[i].size() << "): " << this->eliminationSets[i]);
	}

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
	LOGMSG_INFO("carl.cad", "Redirecting call to file " << filename << "...");
	
	// add bounds to the constraints
	for (auto b: bounds) {
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
	this->printConstraints(constraints, filename);
	LOGMSG_INFO("carl.cad", "done.");
#endif
	
	//////////////////////
	// Initialization
	
	this->interrupted = false;
	checkTraceFirst = checkTraceFirst || this->setting.warmRestart;
	bool useBounds = false;
	bool onlyStrictBounds = true;
	for (auto b: bounds) {
		if (!b.second.isUnbounded() && !b.second.isEmpty()) {
			useBounds = true;
		}
		if (b.second.lowerBoundType() == BoundType::WEAK || b.second.upperBoundType() == BoundType::WEAK) {
			onlyStrictBounds = false;
		}
	}

	if (this->setting.computeConflictGraph) {
		// add necessary conflict graph vertices if required
		for (auto i = conflictGraph.size(); i < constraints.size(); i++) {
			conflictGraph.addConstraintVertex();
		}
	}
	
	//////////////////////
	// Preprocessing
	
	// empty input
	if (constraints.empty()) {
		// check bounds for empty interval
		for (auto b: bounds) {
			if (b.second.isEmpty()) return false;
		}
		// each bound non-empty
		///@todo Maybe faster to generate a solution point directly here? In any case, we must fill r before returning true.
		//return true;
	}
	
	// try to solve the constraints by interval arithmetic
	if (this->setting.preSolveByBounds) {
		typename Interval<Number>::evalintervalmap m;
		for (auto b: bounds) {
			if (b.first >= this->variables.size()) continue;
			m[this->variables[b.first]] = b.second;
		}
		if (!m.empty()) {
			// there are bounds we can use
			for (auto constraint: constraints) {
				if (IntervalEvaluation::evaluate(constraint.getPolynomial(), m).sgn() != constraint.getSign()) {
					// the constraint is unsatisfiable!
					// this constraint is already the minimal infeasible set, so switch it with the last position in the constraints list
					std::swap(constraints.back(), constraint);
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
		if (this->setting.zeroDimEquations && !equations.empty()) {
			// @todo: incorporate bounds
			/* Remove all occurrences of polynomials belonging to the inequalities.
			 * If the cad is not build upon more than the constraints' polynomials, this step leaves only polynomials of equations.
			 */
			for (cad::Constraint<Number> c: strictInequalities) {
				this->removePolynomial(c.getPolynomial());
			}
			this->alterSetting(cad::CADSettings::getSettings(cad::EQUATIONSONLY, rootfinder::SplittingStrategy::DEFAULT, this->setting));
		} else {
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
	}
	
	//////////////////////
	// Main check procedure
	
	this->prepareElimination();
	
	std::vector<std::pair<const UPolynomial*, const UPolynomial*>> boundPolynomials(this->variables.size(), std::pair<UPolynomial*, UPolynomial*>());
	LOGMSG_TRACE("carl.cad", "Creating boundPolynomials of size " << this->variables.size());
	
	if (useBounds) {
		// construct constraints and polynomials representing the bounds
		for (auto b: bounds) {
			if (b.first >= this->variables.size()) continue;
			
			// construct bound-related polynomials
			std::list<const UPolynomial*> tmp;
			if (b.second.lowerBoundType() != BoundType::INFTY) {
				UPolynomial p(this->variables[b.first], {MPolynomial(Term<Number>(-b.second.lower())), MPolynomial(Term<Number>(1))});
				tmp.push_back(this->take(new UPolynomial(p.pseudoPrimpart())));
				this->eliminationSets[b.first].insert(tmp.back());
				this->iscomplete = false; // new polynomials induce new sample points
				assert(b.first < boundPolynomials.size());
				boundPolynomials[b.first].first = tmp.back();
			}
			if (b.second.upperBoundType() != BoundType::INFTY) {
				UPolynomial p(this->variables[b.first], {MPolynomial(Term<Number>(-b.second.upper())), MPolynomial(Term<Number>(1))});
				tmp.push_back(this->take(new UPolynomial(p.pseudoPrimpart())));
				this->eliminationSets[b.first].insert(tmp.back());
				this->iscomplete = false; // new polynomials induce new sample points
				boundPolynomials[b.first].first = tmp.back();
			}
			
			// eliminate bound-related polynomials only
			// l: variable index of the elimination destination
			unsigned l = b.first + 1;
			while (!tmp.empty() && l < this->variables.size()) {
				std::list<const UPolynomial*> tmp2;
				for (auto p: tmp) {
					auto res = this->eliminationSets[l-1].eliminateInto(p, this->eliminationSets[l], this->variables[l], this->setting);
					tmp2.insert(tmp2.begin(), res.begin(), res.end());
				}
				std::swap(tmp, tmp2);
				l++;
			}
		}
	}
	
	// call the main check function according to the settings
	bool satisfiable = this->mainCheck(constraints, bounds, r, conflictGraph, deductions, next, checkTraceFirst, useBounds, checkBounds);
	
	if (useBounds) {
		// possibly tweak the bounds
		if (this->setting.improveBounds) {
			if (satisfiable) {
				this->shrinkBounds(bounds, r);
			} else {
				this->widenBounds(bounds, constraints);
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
			for (auto p: this->polynomials) {
				if (p->mainVar() == this->variables.front()) {
					this->eliminationSets.front().insert(p);
				} else {
					this->eliminationSets.front().insert(this->take(new UPolynomial(p->switchVariable(this->variables.front()))));
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
		LOGMSG_DEBUG("carl.cad", "Result: sat (by sample point " << r << ")");
	} else {
		LOGMSG_DEBUG("carl.cad", "Result: unsat");
	}
	LOGMSG_TRACE("carl.cad", "Status:" << std::endl << *this);

	this->alterSetting(backup);
	return satisfiable;
}

template<typename Number>
template<typename InputIterator>
void CAD<Number>::addPolynomials(InputIterator first, InputIterator last, const std::vector<Variable>& v) {
	// add (only) the new polynomials to the list of new polynomials
	bool nothingAdded = true;
	for (InputIterator p = first; p != last; p++) {
		Variable var = v.front();
		if (!this->variables.empty()) var = this->variables.front();
		else if (!this->newVariables.empty()) var = this->newVariables.front();

		LOGMSG_TRACE("carl.core", "Adding " << **p);
		UPolynomial* up = new UPolynomial((*p)->toUnivariatePolynomial(var));
		if (std::find(this->scheduledPolynomials.begin(), this->scheduledPolynomials.end(), up) != this->scheduledPolynomials.end()) {
			// same polynomial was already considered in scheduled polynomials
			delete up;
			continue;
		}
		if (!this->eliminationSets.empty()) {
			if (this->eliminationSets.front().find(up) != nullptr) {
				// same polynomial was already considered in elimination polynomials
				delete up;
				continue;
			}
		}
		// schedule the polynomial for the next elimination
		this->scheduledPolynomials.push_back(this->take(up));
		nothingAdded = false;
	}
	
	if (nothingAdded) {
		// no polynomial to add, so do not touch the variables
		return;
	
	}
	// determine the variables differing from mVariables and add them to the front of the existing variables
	for (Variable var: v) {
		if (
			(std::find(this->variables.begin(), this->variables.end(), var) == this->variables.end()) &&
			(std::find(this->newVariables.begin(), this->newVariables.end(), var) == this->newVariables.end())
		) {
			// found a new variable
			this->newVariables.push_back(var);
		}
	}
}

template<typename Number>
void CAD<Number>::removePolynomial(const UPolynomial& polynomial) {
	LOGMSG_TRACE("carl.core", "Removing " << polynomial);
	// possibly remove the polynomial from the list of scheduled polynomials
	for (auto p = this->scheduledPolynomials.begin(); p != this->scheduledPolynomials.end(); p++) {
		if (**p == polynomial) {
			// in this case, there is neither any other occurrence of p in mScheduledPolynomials nor in mEliminationSets[0] (see addPolynomial for reason)
			this->scheduledPolynomials.erase(p);
			return;
		}
	}
	
	// remove the polynomial from the list of all polynomials
	for (auto p = this->polynomials.begin(); p != this->polynomials.end(); p++) {
		if (**p == polynomial) {
			this->polynomials.erase(p);
			break;
		}
	}
	
	// determine the level of the polynomial (first level from the top) and remove the respective pointer from it
	for (unsigned level = 0; level < this->eliminationSets.size(); level++) {
		// transform the polynomial according to possible optimizations in order to recognize its real shape in the elimination set
		//auto tmp = new UPolynomial(polynomial.pseudoPrimpart());
		auto tmp = new UPolynomial(polynomial);
		LOGMSG_TRACE("carl.core", "Removing " << *tmp << " from " << this->eliminationSets[level]);
		auto p = this->eliminationSets[level].find(tmp);
		delete tmp;
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
	LOGMSG_TRACE("carl.core", "Removing " << *p);
	
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
		for (auto parent: parents) {
			std::forward_list<const UPolynomial*> curParents = this->eliminationSets[l].removeByParent(parent);
			newParents.insert_after(newParents.before_begin(), curParents.begin(), curParents.end());
		}
		newParents.sort(UnivariatePolynomialComparator<MPolynomial>(this->setting.order));
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
	int maxDepth = this->sampleTree.max_depth();
	auto sampleTreeRoot = this->sampleTree.begin();
	for (int l = (int)dim - 1; l >= (int)level; l--) {
		// iterate from the leaves to the root (more efficient if several levels are to be cleaned)
		if (this->eliminationSets[(size_t)l].empty()) {
			// there is nothing more to be done for this level, so erase all samples up to one
			unsigned depth = dim - (unsigned)l;
			if ((int)depth <= maxDepth) {
				// merge everything into destinationNode
				auto destination = this->sampleTree.begin_fixed(sampleTreeRoot, depth);
				auto node = this->sampleTree.next_at_same_depth(destination);
				std::forward_list<typename tree<RealAlgebraicNumberPtr<Number>>::iterator> toDelete;
				while (this->sampleTree.is_valid(node)) {
					// merge each node's children into destinationNode's children
					this->sampleTree.merge(destination.begin(), destination.end(), node.begin(), node.end());
					toDelete.push_front(node);
					node = this->sampleTree.next_at_same_depth(node);
				}
				// clean the remaining nodes
				for (auto node: toDelete) {
					this->sampleTree.erase(node);
				}
			}
		}
	}
	
	// correct the trace
	auto node = this->sampleTree.begin_fixed(sampleTreeRoot, (unsigned)maxDepth);
	for (unsigned lTrace = 0; (int)lTrace <= maxDepth; lTrace++) {
		this->trace[lTrace] = node;
		node = this->sampleTree.parent(node);
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
		auto node = std::lower_bound(this->sampleTree.begin(parent), this->sampleTree.end(parent), sample, Less<Number>());
		
		bounds[index] = this->getBounds(node, sample);
		parent = node;
	}
	return bounds;
}

template<typename Number>
bool CAD<Number>::satisfies(RealAlgebraicPoint<Number>& r, const std::vector<cad::Constraint<Number>>& constraints) {
	for (unsigned i = 0; i < constraints.size(); i++) {
		if (!constraints[i].satisfiedBy(r)) return false;
	}
	return true;
}

template<typename Number>
bool CAD<Number>::satisfies(RealAlgebraicPoint<Number>& r, const std::vector<cad::Constraint<Number>>& constraints, cad::ConflictGraph& conflictGraph) {
	bool satisfied = true;
	std::forward_list<unsigned> vertices;
	for (unsigned i = 0; i < constraints.size(); i++) {
		if (constraints[i].satisfiedBy(r)) {
			vertices.push_front(i);
		} else {
			satisfied = false;
		}
	}
	// store that the constraints are satisfied by r
	conflictGraph.addEdges(vertices.begin(), vertices.end());
	return satisfied;
}

template<typename Number>
cad::SampleSet<Number> CAD<Number>::samples(
		const std::list<RealAlgebraicNumberPtr<Number>>& roots,
		cad::SampleSet<Number>& currentSamples,
		std::forward_list<RealAlgebraicNumberPtr<Number>>& replacedSamples,
		const Interval<Number>& bounds
) {
	cad::SampleSet<Number> newSampleSet;
	replacedSamples.clear();
	if (roots.empty()) return newSampleSet;
	
	bool boundsActive = !bounds.isEmpty() && !bounds.isUnbounded();
	
	for (auto i: roots) {
		auto insertValue = currentSamples.insert(i);
		if (!insertValue.second) {
			// value already in the list => do not include in newSampleSet
			if (!(*insertValue.first)->isRoot()) {
				// the new root is already contained, but only as sample value => switch to root and start sample construction from scratch
				assert(i->isRoot());
				RealAlgebraicNumberPtr<Number> r = *insertValue.first;
				auto pos = std::lower_bound(newSampleSet.begin(), newSampleSet.end(), r, Less<Number>());
				currentSamples.remove(insertValue.first);
				if (pos != newSampleSet.end()) {
					newSampleSet.remove(pos);
				}
				r->setIsRoot(true);
				insertValue = currentSamples.insert(r);
				newSampleSet.insert(r);
				replacedSamples.push_front(r);
			} else if (!(*insertValue.first)->isNumeric() && i->isNumeric()) {
				// there is already an interval-represented root with the same value present and it can be replaced by a numeric
				currentSamples.remove(insertValue.first);
				insertValue = currentSamples.insert(RealAlgebraicNumberNR<Number>::create(i->value(), true));
				// this value might have been added to newSamples already, so switch the root status there as well
				auto pos = std::lower_bound(newSampleSet.begin(), newSampleSet.end(), *insertValue.first, Less<Number>());
				if (pos != newSampleSet.end()) {
					newSampleSet.remove(pos);
					newSampleSet.insert(*insertValue.first);
				}
				replacedSamples.push_front(i);
			} else {
				// nothing changes by the new root, thus proceed with the next root
				continue;
			}
		} else {
			// we found a new sample
			// add the root to new samples (with root switch on)
			newSampleSet.insert(*insertValue.first);
		}
		// local set storing the elements which shall be added to currentSampleSet and newSampleSet in the end
		std::list<RealAlgebraicNumberNRPtr<Number>> currentSamplesIncrement;
		
		/** Situation: One, next or previous, has to be a root (assumption) or we meet one of the outmost positions.
		 * --------|-------------------|-----------------|---
		 *    previous        insertValue.first         next
		 *     (root?)              (root)            (root?)
		 */
		
		// next: right neighbor
		auto neighbor = insertValue.first;
		// -> next (safe here, but need to check for end() later)
		neighbor++;
		if (neighbor == currentSamples.end()) {
			// rightmost position
			// insert one rightmost sample (by adding 1 or taking the rightmost interval bound)
			if ((*insertValue.first)->isNumeric()) {
				currentSamplesIncrement.push_front(RealAlgebraicNumberNR<Number>::create((*insertValue.first)->value() + 1, false));
			} else {
				currentSamplesIncrement.push_front(RealAlgebraicNumberNR<Number>::create(std::static_pointer_cast<RealAlgebraicNumberIR<Number>>(*insertValue.first)->getInterval().upper(), false));
			}
		} else if ((*neighbor)->isRoot()) {
			// sample between neighbor and insertValue.first needed and will be added to newSampleSet
			if ((*insertValue.first)->isNumeric()) {
				if ((*neighbor)->isNumeric()) {
					assert((*insertValue.first)->value() < (*neighbor)->value());
					currentSamplesIncrement.push_front(RealAlgebraicNumberNR<Number>::create(Interval<Number>((*insertValue.first)->value(), BoundType::STRICT, (*neighbor)->value(), BoundType::STRICT).sample(), false));
				} else {
					currentSamplesIncrement.push_front(RealAlgebraicNumberNR<Number>::create(std::static_pointer_cast<RealAlgebraicNumberIR<Number>>(*neighbor)->getInterval().lower(), false));
				}
			} else {
				// interval representation, take right bound of insertValue.first which must be strictly between insertValue.first and neighbor
				currentSamplesIncrement.push_front(RealAlgebraicNumberNR<Number>::create(std::static_pointer_cast<RealAlgebraicNumberIR<Number>>(*insertValue.first)->getInterval().upper(), false));
			}
		}
		
		// previous: left neighbor
		neighbor = insertValue.first;
		if (neighbor == currentSamples.begin()) {
			// leftmost position
			// insert one leftmost sample (by subtracting 1 or taking the leftmost interval bound)
			if ((*insertValue.first)->isNumeric()) {
				currentSamplesIncrement.push_front(RealAlgebraicNumberNR<Number>::create((*insertValue.first)->value() - 1, false));
			} else {
				currentSamplesIncrement.push_front(RealAlgebraicNumberNR<Number>::create(std::static_pointer_cast<RealAlgebraicNumberIR<Number>>(*insertValue.first)->getInterval().lower(), false));
			}
		} else {
			neighbor--;
			// now neighbor is the left bound (can be safely determined now)
			if ((*neighbor)->isRoot()) {
				// sample between neighbor and insertValue.first needed and will be added to newSampleSet
				if ((*insertValue.first)->isNumeric()) {
					if ((*neighbor)->isNumeric()) {
						currentSamplesIncrement.push_front(RealAlgebraicNumberNR<Number>::create(Interval<Number>((*neighbor)->value(), BoundType::STRICT, (*insertValue.first)->value(), BoundType::STRICT).sample(), false));
					} else {
						currentSamplesIncrement.push_front(RealAlgebraicNumberNR<Number>::create(std::static_pointer_cast<RealAlgebraicNumberIR<Number>>(*neighbor)->getInterval().upper(), false));
					}
				} else {
					// interval representation, take left bound of insertValue.first which must be strictly between insertValue.first and neighbor
					currentSamplesIncrement.push_front(RealAlgebraicNumberNR<Number>::create(std::static_pointer_cast<RealAlgebraicNumberIR<Number>>(*insertValue.first)->getInterval().lower(), false));
				}
			}
		}
		
		if (boundsActive) {
			// remove samples which do not lie within the (weak) bounds
			for (auto i = currentSamplesIncrement.begin(); i != currentSamplesIncrement.end(); ) {
				if (bounds.meets((*i)->value())) i++;
				else i = currentSamplesIncrement.erase(i);
			}
		}
		newSampleSet.insert(currentSamplesIncrement.begin(), currentSamplesIncrement.end());
		currentSamples.insert(currentSamplesIncrement.begin(), currentSamplesIncrement.end());
	}
	return newSampleSet;
}

template<typename Number>
cad::SampleSet<Number> CAD<Number>::samples(
		const UPolynomial* p,
		const std::list<RealAlgebraicNumberPtr<Number>>& sample,
		const std::list<Variable>& variables,
		cad::SampleSet<Number>& currentSamples,
		std::forward_list<RealAlgebraicNumberPtr<Number>>& replacedSamples,
		const Interval<Number>& bounds,
		cad::CADSettings settings
) {
	assert(variables.size() == sample.size());
	return CAD<Number>::samples(
		carl::rootfinder::realRoots(*p, variables, sample, bounds, settings.splittingStrategy),
		currentSamples,
		replacedSamples,
		bounds
	);
}

template<typename Number>
template<class VariableIterator, class PolynomialIterator>
std::vector<Variable> CAD<Number>::orderVariablesGreeedily(
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
		for (auto p: eliminationOutput) {
			degreeSum += p->totalDegree();
		}
		variableMap[std::make_pair(degreeSum, eliminationOutput.size())].push_front(variable);
		variableCount++;
	}
	// transform variableOrder to a sorted vector
	std::vector<Variable> variableOrder(variableCount);
	for (auto i = variableMap.crbegin(); i != variableMap.crend(); i++) {
		for (auto j: i->second) {
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
		for (auto i: this->eliminationSets) {
			i.setLiftingOrder(setting.order);
		}
	}
	if (!this->setting.simplifyByGroebner && setting.simplifyByGroebner) {
		LOGMSG_WARN("carl.cad", "Changing simplifyByGroebner during computation is not supported yet.");
	}
	if (!this->setting.simplifyByRootcounting && setting.simplifyByRootcounting) {
		for (auto i: this->eliminationSets) {
			i.removePolynomialsWithoutRealRoots();
		}
	}
	if (!this->setting.simplifyByFactorization && setting.simplifyByFactorization) {
		for (auto i: this->eliminationSets) {
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
	if (!this->sampleTree.is_valid(node) && *node == nullptr) {
		// node is invalid
		return {};
	}
	
	std::list<RealAlgebraicNumberPtr<Number>> v;
	// proceed from the leaf up to the root while the children of root represent the last component of the sample point and the leaf the first
	if (this->setting.equationsOnly) {
		while (node != root) {
			if (!(*node)->isRoot()) return {};
			v.push_back(*node);
			node = this->sampleTree.parent(node);
		}
	} else if (this->setting.inequalitiesOnly) {
		while (node != root) {
			if ((*node)->isRoot()) return {};
			v.push_back(*node);
			node = this->sampleTree.parent(node);
		}
	} else {
		while (node != root) {
			v.push_back(*node);
			node = this->sampleTree.parent(node);
		}
	}
	return v;
}

template<typename Number>
typename CAD<Number>::CADTrace CAD<Number>::constructTraceAt(sampleIterator node, const sampleIterator& root ) const {
	CADTrace trace;
	while (node != root) {
		trace.push_back(node);
		node = this->sampleTree.parent(node);
	}
	trace.push_back(root);
	return trace;
}

template<typename Number>
std::pair<bool, bool> CAD<Number>::checkNode(
		sampleIterator node,
		bool fullRestart,
		bool excludePrevious,
		bool updateTrace,
		std::vector<cad::Constraint<Number>>& constraints,
		BoundMap& bounds,
		RealAlgebraicPoint<Number>& r,
		cad::ConflictGraph& conflictGraph,
		bool boundsNontrivial,
		bool checkBounds,
		unsigned dim
) {
	// for each node construct the path by iterating back to the root (no way to check the bounds from here since the depth of the leaf is still unknown)
	auto sampleList = this->constructSampleAt(node, this->sampleTree.begin());
	// settings demand not to take this sample (e.g., because only real roots are solutions)
	if (sampleList.empty()) {
		return std::make_pair(false, true);
	}
	RealAlgebraicPoint<Number> sample(sampleList);
	bool boundsOK = true;
	// offset for incomplete samples (sample is filled from behind)
	unsigned firstLevel = (unsigned)(this->variables.size() - sample.dim());
	
	// test if the sample _r is already outside the bounds (boundsOK=false) or if it can be checked against the constraints or further lifted (boundsOK=true)
	for (auto i: bounds) {
		// bounds correspond to mVariables indices, so shift those indices by firstLevel to the left
		if (i.first < this->variables.size() && firstLevel <= i.first && !sample[i.first - firstLevel]->containedIn(i.second)) {
			boundsOK = false;
			break;
		}
	}
	if (!boundsOK) {
		// this point did not match the bounds => continue searching
		return std::make_pair(false, true);
	}
	if (sample.dim() == dim) {
		// found a sample to check with the constraints
		if (excludePrevious) return std::make_pair(false, true);
		
		if (
			(this->setting.computeConflictGraph && this->satisfies(sample, constraints, conflictGraph)) ||
			(!this->setting.computeConflictGraph && this->satisfies(sample, constraints))
			) {
			r = sample;
			if (updateTrace) {
				this->trace = this->constructTraceAt(node, this->sampleTree.begin());
			}
			return std::make_pair(true, false);
		}
	} else {
		// found an incomplete sample, then first check the bounds and possibly restart lifting at the respective level
		// prepare the variables for lifting
		unsigned i = dim;
		std::list<Variable> variables;
		// TODO: Check this
		//for (auto component: sampleList) {
		for (unsigned int j = 0; j < sampleList.size(); j++) {
			i--;
			variables.push_front(this->variables[i]);
		}
		// perform lifting at the incomplete leaf (without elimination, only by the current elimination polynomials)
		if (this->liftCheck(node, sampleList, i, fullRestart, variables, constraints, bounds, boundsNontrivial, checkBounds, r, conflictGraph)) {
			return std::make_pair(true, false);
		}
	}
	return std::make_pair(false, false);
}

template<typename Number>
bool CAD<Number>::mainCheck(
		std::vector<cad::Constraint<Number>>& constraints,
		BoundMap& bounds,
		RealAlgebraicPoint<Number>& r,
		cad::ConflictGraph& conflictGraph,
		Deductions&,
		bool next,
		bool checkTraceFirst,
		bool boundsNontrivial,
		bool checkBounds
) {
	LOGMSG_TRACE("carl.cad", "mainCheck() on " << constraints);
#define CHECK_NODE( _node, _fullRestart, _excludePrevious, _updateTrace )\
	auto res = this->checkNode(_node, _fullRestart, _excludePrevious, _updateTrace, constraints, bounds, r, conflictGraph, boundsNontrivial, checkBounds, dim);\
	if (res.first) return true;\
	if (res.second) continue;
// END CHECK_NODE

	if (this->variables.empty()) {
		// there are no valid samples available
		// if there is no constraint, all constraints are satisfied; otherwise no constraint
		return constraints.empty();
	}
	
	const unsigned dim = (unsigned)this->variables.size();
	LOGMSG_TRACE("carl.cad", "mainCheck: dimension is " << dim);
	auto sampleTreeRoot = this->sampleTree.begin();
	int tmp = this->sampleTree.max_depth(sampleTreeRoot);
	assert(tmp >= 0);
	unsigned maxDepth = (unsigned)tmp;
	// if the elimination sets were extended (i.e. the sample tree is not developed completely), we obtain new samples already in phase one
	next = next && (maxDepth == dim);
	
	// unify the variables for each constraint to match the CAD's variable order
	for (unsigned i = 0; i < constraints.size(); ++i) {
		constraints[i].unifyVariables(this->variables);
	}
	
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
	
	LOGMSG_DEBUG("carl.cad", "mainCheck: Entering Phase 1...");
	
	if (checkTraceFirst && maxDepth != 0) {
		// only consider trace if there has been any sample before
		for (unsigned tracePos = 0; tracePos < dim; tracePos++) {
			// traverse the trace for satisfying samples (except for the last trace position, which is the sample tree root)
			auto node = this->trace[tracePos];
			CHECK_NODE(node, false, next, false)
		}
		// update maximum sample tree depth
		assert(this->sampleTree.max_depth(sampleTreeRoot) >= 0);
		maxDepth = (unsigned)this->sampleTree.max_depth(sampleTreeRoot);
	}
	
	/* Phase 2
	 * Invariant: There might be nodes in the sample tree which are not at the final depth and still need to be lifted.
	 * Check existing sample points and lift incomplete ones, i.e., check all leaves (not necessarily at an appropriate depth).
	 */
	if (maxDepth == 0) {
		// there is no sample component computed yet, so we are at the base level
		// perform an initial elimination so that the base level contains lifting positions
		int lastRes = 0;
		while (this->eliminationSets.back().emptyLiftingQueue() && (lastRes = this->eliminate(dim-1, bounds, boundsNontrivial))) {
			LOGMSG_DEBUG("carl.cad", "Waiting for something to lift, lastRes = " << lastRes << std::endl << *this);
		};
		
		// perform an initial lifting step in order to fill the tree once
		if (this->liftCheck(this->sampleTree.begin_leaf(), {}, dim, true, {}, constraints, bounds, boundsNontrivial, checkBounds, r, conflictGraph)) {
			// lifting yields a satisfying sample
			return true;
		}
	} else {
		// the sample tree contains valid sample points
		for (auto leaf = this->sampleTree.begin_leaf(); leaf != this->sampleTree.end_leaf(); leaf++) {
			// traverse the current sample tree leaves for satisfying samples
			CHECK_NODE(leaf, true, next, true)
		}
	}
	
	if (this->isComplete()) {
		// no leaf of the completely developed sample tree satisfied the constraints
		return false;
	}
	
	/* Phase 3
	 * Invariants: (1) The nodes of the sample tree carry all samples (sample components)
	 * of the lifting positions considered so far unless the tree is empty, what is due to the sample storage mechanism in liftCheck.
	 * (2) There might be nodes where not all lifting positions were considered so far.
	 *
	 * - Traverse all levels for open lifting positions and perform the lifting accordingly.
	 * - We start from the smallest level (0, 2, ..., dim-1) where lifting is still possible.
	 */
	
	// invariant: either the last level is completely developed (dim or 0), or something in between due to bounds
	assert(maxDepth == (unsigned)dim || maxDepth == (unsigned)0 || boundsNontrivial);
	
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
		for (auto node = this->sampleTree.begin_fixed(sampleTreeRoot, depth); this->sampleTree.is_valid(node) && (int)depth == this->sampleTree.depth(node); node = this->sampleTree.next_at_same_depth(node)) {
			// traverse all nodes at depth, i.e., sample points of dimension dim - level - 1 equaling the number of coefficient variables of the lifting position at level
			std::list<RealAlgebraicNumberPtr<Number>> sampleList = this->constructSampleAt(node, sampleTreeRoot);
			// no degenerate sample points are considered here because they were already discarded in Phase 2
			if (depth != sampleList.size()) continue;
			
			RealAlgebraicPoint<Number> sample(sampleList);
			bool boundsOK = true;
			// offset for incomplete samples (sample is filled from behind)
			unsigned firstLevel = (unsigned)(this->variables.size() - sample.dim());
			// test if the sample _r is already outside the bounds (boundsOK=false) or if it can be checked against the constraints or further lifted (boundsOK=true)
			for (auto i: bounds) {
				// bounds correspond to mVariables indices, so shift those indices by firstLevel to the left
				if (i.first < this->variables.size() && firstLevel <= i.first && !sample[i.first - firstLevel]->containedIn(i.second)) {
					boundsOK = false;
					break;
				}
			}
			// this point did not match the bounds => continue searching
			if (!boundsOK) continue;
			
			// prepare the variables for lifting
			unsigned i = dim;
			std::list<Variable> variables;
			// TODO: Check this
			//for (auto component: sampleList) {
			for (unsigned int j = 0; j < sampleList.size(); j++) {
				assert(i > 0);
				i--;
				variables.push_front(this->variables[i]);
			}
			assert(level + 1 == (int)i);
			// perform lifting at the incomplete leaf with the stored lifting queue (reset performed in liftCheck)
			if (liftCheck(node, sampleList, i, false, variables, constraints, bounds, boundsNontrivial, checkBounds, r, conflictGraph)) {
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
		for (auto i: this->eliminationSets) {
			i.setLiftingPositionsReset();
		}
	}
	
	return false;
}


template<typename Number>
typename CAD<Number>::sampleIterator CAD<Number>::storeSampleInTree(RealAlgebraicNumberPtr<Number> newSample, sampleIterator node) {
	sampleIterator newNode = std::lower_bound(this->sampleTree.begin(node), this->sampleTree.end(node), newSample, Less<Number>());
	if (newNode == this->sampleTree.end(node)) {
		newNode = this->sampleTree.append_child(node, newSample);
	} else if (Equal<Number>()(*newNode, newSample)) {
		newNode = this->sampleTree.replace(newNode, newSample);
	} else {
		newNode = this->sampleTree.insert(newNode, newSample);
	}
	return newNode;
}

template<typename Number>
bool CAD<Number>::liftCheck(
		sampleIterator node,
		const std::list<RealAlgebraicNumberPtr<Number>>& sample,
		unsigned openVariableCount,
		bool restartLifting,
		const std::list<Variable>& variables,
		const std::vector<cad::Constraint<Number>>& constraints,
		const BoundMap& bounds,
		bool boundsActive,
		bool checkBounds,
		RealAlgebraicPoint<Number>& r,
		cad::ConflictGraph& conflictGraph
) {
	if (checkBounds && boundsActive && !sample.empty()) {
		// bounds shall be checked and the level is non-empty
		// level should be non-empty
		assert(openVariableCount < this->variables.size());
		// see if bounds are given for the previous level
		auto bound = bounds.find(openVariableCount);
		if (bound != bounds.end()) {
			if (!sample.front()->containedIn(bound->second)) {
				return false;
			}
		}
	}
	
	// update the current trace node
	this->trace[openVariableCount] = node;
	
	// base level: zero variables left to substitute => evaluate the constraint
	if (openVariableCount == 0) {
		// check whether an interuption flag is set
		if (this->anAnswerFound()) {
			// interrupt the check procedure
			this->interrupted = true;
			LOGMSG_TRACE("carl.cad", "Returning true as an answer was found");
			return true;
		}
		RealAlgebraicPoint<Number> t(sample);
		if ((this->setting.computeConflictGraph && this->satisfies(t, constraints, conflictGraph)) ||
			(!this->setting.computeConflictGraph && this->satisfies(t, constraints))) {
			r = t;
			LOGMSG_TRACE("carl.cad", "Returning true as a satisfying sample was found");
			return true;
		}
		LOGMSG_TRACE("carl.cad", "Returning false...");
		return false;
	}
	
	// openVariableCount > 0: lifting
	// previous variable will be substituted next
	openVariableCount--;
	
	std::list<RealAlgebraicNumberPtr<Number>> extSample(sample.begin(), sample.end());
	std::list<Variable> newVariables(variables);
	// the first variable is always the last one lifted
	newVariables.push_front(this->variables[openVariableCount]);
	// see if bounds are given for this level
	auto bound = boundsActive ? bounds.find(openVariableCount) : bounds.end();
	bool boundActive = bounds.end() != bound;
	
	if (restartLifting) {
		// use the complete set of lifting positions
		this->eliminationSets[openVariableCount].resetLiftingPositionsFully();
	} else {
		// use the currently stored lifting queue
		this->eliminationSets[openVariableCount].resetLiftingPositions();
	}

	/*
	 * Main loop: performs all operations possible in one level > 0, in particular, 2 phases.
	 * Phase 1: Choose a lifting position and construct the corresponding samples.
	 * Phase 2: Choose a sample and trigger liftCheck for the next level with the chosen sample.
	 */
	
	// determines whether new samples shall be constructed regardless of other flags
	bool computeMoreSamples = false;
	// the current list of samples at this position in the sample tree
	cad::SampleSet<Number> currentSamples;
	currentSamples.insert(this->sampleTree.begin(node), this->sampleTree.end(node));
	// the current samples queue for this lifting process
	cad::SampleSet<Number> sampleSetIncrement;
	std::forward_list<RealAlgebraicNumberPtr<Number>> replacedSamples;
	
	// fill in a standard sample to ensure termination in the main loop
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
			sampleSetIncrement.insert(this->samples({RealAlgebraicNumberNR<Number>::create(bound->second.center(), true)}, currentSamples, replacedSamples));
		} else {
			sampleSetIncrement.insert(this->samples(boundRoots, currentSamples, replacedSamples));
		}
	} else {
		sampleSetIncrement.insert(this->samples({RealAlgebraicNumberNR<Number>::create(0, true)}, currentSamples, replacedSamples));
	}
	
	while (true) {
		LOGMSG_TRACE("carl.cad", "Phase 1 of lifting...");
		/* Phase 1
		 * Lifting position choice and corresponding sample construction.
		 */
		// loop if no samples are present at all or heuristics according to the respective setting demand to continue with a new lifting position, construct new samples
		while (
			computeMoreSamples || sampleSetIncrement.empty() ||
			(this->setting.preferNRSamples && sampleSetIncrement.emptyNR()) ||
			(this->setting.preferSamplesByIsRoot && this->setting.preferNonrootSamples && sampleSetIncrement.emptyNonroot()) ||
			(this->setting.preferSamplesByIsRoot && !this->setting.preferNonrootSamples && sampleSetIncrement.emptyRoot())
		) {
			LOGMSG_TRACE("carl.cad", "computing more samples.");
			// disable blind sample construction
			computeMoreSamples = false;
			replacedSamples.clear();
			if (this->eliminationSets[openVariableCount].emptyLiftingQueue()) {
				// break if all lifting positions are considered or the level is empty
				break;
			}
			
			if (boundActive && this->setting.earlyLiftingPruningByBounds) {
				// found bounds for the current lifting variable => remove all samples outside these bounds
				sampleSetIncrement.insert(this->samples(this->eliminationSets[openVariableCount].nextLiftingPosition(), sample, variables, currentSamples, replacedSamples, bound->second, this->setting));
			} else {
				sampleSetIncrement.insert(this->samples(this->eliminationSets[openVariableCount].nextLiftingPosition(), sample, variables, currentSamples, replacedSamples, Interval<Number>::unboundedInterval(), this->setting));
			}
			
			// replace all samples in the tree which were changed in the current samples list
			for (auto replacedSample: replacedSamples) {
				this->storeSampleInTree(replacedSample, node);
			}
			// discard lifting position just used for sample construction
			this->eliminationSets[openVariableCount].popLiftingPosition();
			// try to simplify the current samples even further
			auto simplification = sampleSetIncrement.simplify();
			if (simplification.second) {
				// simplification took place => replace all
				for (auto i: simplification.first) {
					currentSamples.simplify(i.first, i.second);
				}
			}
		}
		
		/* Phase 2
		 * Lifting of the current level.
		 */
		LOGMSG_TRACE("carl.cad", "Phase 2 of lifting...");
		while (!sampleSetIncrement.empty()) {
			// iterate through all samples found by the next() method
			LOGMSG_TRACE("carl.cad", "Iterating...");
			/*
			 * Sample choice
			 */
			RealAlgebraicNumberPtr<Number> newSample;
			if (this->setting.preferNRSamples) {
				if (sampleSetIncrement.emptyNR() && !this->eliminationSets[openVariableCount].emptyLiftingQueue()) {
					computeMoreSamples = true;
					// construct new samples until there are lifting positions
					break;
				}
				// otherwise take also IR samples (as implemented in nextNR)
				newSample = sampleSetIncrement.nextNR();
			} else if (this->setting.preferSamplesByIsRoot) {
				if (this->setting.preferNonrootSamples) {
					if (sampleSetIncrement.emptyNonroot() && 
						(!this->eliminationSets[openVariableCount].emptyLiftingQueue() xor this->setting.inequalitiesOnly)
						) {
						// no intermediate sample available, but still lifting positions available xor
						// no intermediate sample available, but no lifting position available and we do not want to consider root samples
						computeMoreSamples = true;
						break;
					}
					// otherwise take also IR samples (as implemented in nextNonroot)
					newSample = sampleSetIncrement.nextNonRoot();
				} else {
					if (sampleSetIncrement.emptyRoot() && 
						(!this->eliminationSets[openVariableCount].emptyLiftingQueue() xor this->setting.inequalitiesOnly)
						) {
						// no root sample available, but still lifting positions available xor
						// no root sample available, but no lifting position available and we do not want to consider intermediate samples
						// construct new samples until there are lifting positions or stop if all roots have been lifted
						computeMoreSamples = true;
						break;
					}
					// otherwise take also IR samples (as implemented in nextNonroot)
					newSample = sampleSetIncrement.nextRoot();
				}
			} else {
				// use FCFS
				newSample = sampleSetIncrement.next();
			}
			
			// Sample storage
			auto newNode = this->storeSampleInTree(newSample, node);
			// insert at the first position in order to meet the correct variable order
			extSample.push_front(newSample);
			
			// Lifting
			// start lifting with the fresh new sample at the next level for *all* lifting positions
			bool liftingSuccessful = this->liftCheck(newNode, extSample, openVariableCount, true, newVariables, constraints, bounds, boundsActive, checkBounds, r, conflictGraph);
			
			// Sample pop if lifting unsuccessful or at the last level, i.e. level == 0
			if (this->setting.preferNRSamples) {
				// remove sample from increment list because it was completely lifted (pop() uses heuristics already used in next())
				sampleSetIncrement.popNR();
			} else if (this->setting.preferSamplesByIsRoot) {
				if (this->setting.preferNonrootSamples) {
					sampleSetIncrement.popNonroot();
				} else {
					sampleSetIncrement.popRoot();
				}
			} else {
				sampleSetIncrement.pop();
			}

			// clean sample point component again
			extSample.pop_front();

			if (liftingSuccessful) {
				LOGMSG_TRACE("carl.cad", "Lifting was successfull");
				// there might still be samples left but not stored yet
				while (!sampleSetIncrement.empty()) {
					// store the remaining samples in the sample tree (without lifting)
					this->storeSampleInTree(sampleSetIncrement.next(), node);
					sampleSetIncrement.pop();
				}
				return true;
				LOGMSG_TRACE("carl.cad", "Returning true as lifting was successful");
			}
		}
		if (this->eliminationSets[openVariableCount].emptyLiftingQueue()) {
			LOGMSG_TRACE("carl.cad", "Lifting queue is empty");
			// all lifting positions used
			if (this->setting.equationsOnly || this->setting.inequalitiesOnly) {
				// there might still be samples not yet considered but unimportant for the current lifting
				LOGMSG_TRACE("carl.cad", "storing in sample tree");
				while (!sampleSetIncrement.empty()) {
					// store the remaining samples in the sample tree (without lifting)
					this->storeSampleInTree(sampleSetIncrement.next(), node);
					sampleSetIncrement.pop();
				}
			}
			break;
		}
	}
	LOGMSG_TRACE("carl.cad", "Returning false as nothing else happened");
	return false;
}

template<typename Number>
int CAD<Number>::eliminate(unsigned level, const BoundMap& bounds, bool boundsActive) {
	while (true) {
		if (!this->eliminationSets[level].emptyLiftingQueue()) return (int)level;
		unsigned l = level;
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
			LOGMSG_TRACE("carl.cad", "eliminate with bounds");
			// eliminate from level l-1 to level l
			for (; l <= level; l++) {
				while (!this->eliminationSets[l-1].emptySingleEliminationQueue()) {
					// the polynomial can be analyzed for zeros
					const UPolynomial* p = this->eliminationSets[l-1].popNextSingleEliminationPosition();
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
				LOGMSG_TRACE("carl.cad", "returning from eliminate(): " << (int)level);
				return (int)level;
			}
			assert(l == level+1);
			if (!this->setting.simplifyByRootcounting && level == this->eliminationSets.size()) {
				// possibly simplify base level (if not done by setting.simplifyByRootcounting)
				while (!this->eliminationSets.back().emptySingleEliminationQueue()) {
					// the polynomial can be analyzed for zeros (use paired-elimination queue because the single is always empty)
					const UPolynomial* p = this->eliminationSets.back().popNextSingleEliminationPosition();
					if (this->vanishesInBox(p, bounds, (unsigned)this->eliminationSets.size()-1)) break;
					// delete polynomial and try the next one
					this->eliminationSets.back().erase(p);
				}
			}
		} else {
			LOGMSG_TRACE("carl.cad", "eliminate without bounds in level " << l);
			for (; l <= level; l++) {
				this->eliminationSets[l-1].eliminateNextInto(this->eliminationSets[l], this->variables[l], this->setting, false);
				LOGMSG_TRACE("carl.cad", "eliminated" << std::endl << (l-1) << ": " << this->eliminationSets[l-1] << std::endl << l << ": " << this->eliminationSets[l]);
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
				LOGMSG_TRACE("carl.cad", "returning from eliminate(): " << (int)level);
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
	auto node = std::lower_bound(this->sampleTree.begin(parent), this->sampleTree.end(parent), sample, Less<Number>());
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
void CAD<Number>::widenBounds(BoundMap&, std::vector<cad::Constraint<Number>>&) {
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
					for (auto p: previous) {
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
			// reduce the trace at the given depth
			this->trace.erase(this->trace.end() - 1 - depth);
			if (depth <= maxDepth) {
				// remove the complete layer of samples from the sample tree at the given depth
				// fix the iterators to be deleted in a separate list independent of merging with the children
				std::queue<typename tree<RealAlgebraicNumberPtr<Number>>::iterator> toDelete;
				for (auto node = this->sampleTree.begin_fixed(this->sampleTree.begin(), depth); this->sampleTree.is_valid(node) && depth == this->sampleTree.depth(node); node = this->sampleTree.next_at_same_depth(node)) {
					toDelete.push(node);
				}
				while (!toDelete.empty()) {
					// move all children of every node to be deleted, to the current level in a sorted manner, including the subtrees
					auto node = toDelete.front();
					auto parent = this->sampleTree.parent(node);
					for (auto child = this->sampleTree.begin(node); child != this->sampleTree.end(node); child++) {
						auto newNode = std::lower_bound(this->sampleTree.begin(parent), this->sampleTree.end(parent), *child, Less<Number>());
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
bool CAD<Number>::vanishesInBox(const UPolynomial* p, const BoundMap& box, unsigned level, bool recuperate) {
	cad::CADSettings boxSetting = cad::CADSettings::getSettings();
	boxSetting.simplifyEliminationByBounds = false; // would cause recursion in vanishesInBox
	boxSetting.earlyLiftingPruningByBounds = true; // important for efficiency
	boxSetting.simplifyByRootcounting = false; // too much overhead
	boxSetting.trimVariables = false; // not needed for nothing is removed
	boxSetting.simplifyByFactorization = true; // mandatory for a square-free basis
	boxSetting.preSolveByBounds = true; // important for efficiency
	boxSetting.computeConflictGraph = false; // too much overhead and not needed
	boxSetting.numberOfDeductions = 0; // too much overhead and not needed
	std::vector<Variable> variables;
	BoundMap bounds;
	// variable index for the cad box
	unsigned j = 0;
	for (unsigned i = level; i < this->variables.size(); i++) {
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
	CAD<Number> cadbox(static_cast<cad::PolynomialOwner<Number>*>(this));
	LOGMSG_TRACE("carl.core", "Now in nested CAD " << &cadbox);
	cadbox.scheduledPolynomials.assign({p});
	cadbox.newVariables = variables;
	cadbox.setting = boxSetting;
	
	RealAlgebraicPoint<Number> r;
	std::vector<cad::Constraint<Number>> constraints(1, cad::Constraint<Number>(*p, Sign::ZERO, variables));
	if (cadbox.check(constraints, r, bounds, false, false, false)) {
		cadbox.completeElimination();
		LOGMSG_TRACE("carl.core", "Back from nested CAD " << &cadbox);
		if (recuperate) {
			// recuperate eliminated polynomials and go on with the elimination
			unsigned j = 0;
			for (unsigned i = level + 1; i < this->variables.size(); i++) {
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
		return true;
	}
	LOGMSG_TRACE("carl.core", "Back from nested CAD " << &cadbox);
	return false;
}

}
