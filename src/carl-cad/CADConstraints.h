/**
 * @file CADConstraints.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include <algorithm>
#include <forward_list>
#include <iostream>
#include <vector>

#include "CADTypes.h"
#include "ConflictGraph.h"
#include "Constraint.h"
#include "Variables.h"
#include <carl/formula/model/ran/RealAlgebraicPoint.h>
#include <carl/io/streamingOperators.h>

namespace carl {
namespace cad {

template<typename Number>
class CADConstraints {
private:
	std::vector<cad::Constraint<Number>> mConstraints;
	std::vector<std::vector<std::size_t>> mVariableLookup;
	
public:
	void set(const std::vector<cad::Constraint<Number>>& constraints, const cad::Variables& variables) {
		mConstraints = constraints;
		assert(variables.newSize() == 0);
		mVariableLookup.resize(variables.size());
		std::vector<bool> cons(mConstraints.size(), true);
		
		auto cur = mVariableLookup.begin();
		auto vit = variables.rbegin();
		for (vit++; vit != variables.rend(); vit++) {
			cur->clear();
			for (std::size_t c = 0; c < mConstraints.size(); c++) {
				if (!cons[c]) continue;
				const auto& cvars = mConstraints[c].getVariables();
				if (std::includes(variables.rbegin(), vit, cvars.begin(), cvars.end())) {
					cons[c] = false;
					cur->push_back(c);
				}
			}
			cur++;
		}
		cur->clear();
		for (std::size_t c = 0; c < mConstraints.size(); c++) {
			if (cons[c]) {
				cur->push_back(c);
			}
		}
	}
	
	bool empty() const {
		return mConstraints.empty();
	}
	std::size_t size() const {
		return mConstraints.size();
	}
	auto begin() const -> decltype(mConstraints.begin()) {
		return mConstraints.begin();
	}
	auto end() const -> decltype(mConstraints.end()) {
		return mConstraints.end();
	}
	
	bool satisfiedPartiallyBy(RealAlgebraicPoint<Number>& r, const std::vector<Variable>& variables) const {
		if (r.dim() == 0) return true;
		std::vector<Variable> vars(variables.begin() + (long)(variables.size() - r.dim()), variables.end());
		std::size_t dim = vars.size()-1;
		for (const auto& cid: mVariableLookup[dim]) {
			const auto& c = mConstraints[cid];
			if (!c.satisfiedBy(r, vars)) return false;
		}
		return true;
	}
	
	bool satisfiedPartiallyBy(RealAlgebraicPoint<Number>& r, const std::vector<Variable>& variables, cad::ConflictGraph<Number>& conflictGraph) const {
		if (r.dim() == 0) return true;
		bool satisfied = true;
		std::vector<Variable> vars(variables.begin() + (long)(variables.size() - r.dim()), variables.end());
		std::size_t dim = vars.size()-1;
		std::size_t sampleID = conflictGraph.newSample();
		for (const auto& cid: mVariableLookup[dim]) {
			const auto& c = mConstraints[cid];
			std::size_t constraintID = conflictGraph.getConstraint(c);
			CARL_LOG_DEBUG("carl.cad", "Checking if " << c << " is satisfied by " << r << " over " << vars);
			bool sat = c.satisfiedBy(r, vars);
			conflictGraph.set(constraintID, sampleID, !sat);
			satisfied = satisfied && sat;
		}
		return satisfied;
	}
	
	bool satisfiedBy(RealAlgebraicPoint<Number>& r, const std::vector<Variable>& variables) const {
		for (const auto& c: mConstraints) {
			if (!c.satisfiedBy(r, variables)) return false;
		}
		return true;
	}
	bool satisfiedBy(RealAlgebraicPoint<Number>& r, const std::vector<Variable>& variables, cad::ConflictGraph<Number>& conflictGraph) const {
		bool satisfied = true;
		std::size_t sampleID = conflictGraph.newSample();
		for (const auto& c: mConstraints) {
			std::size_t constraintID = conflictGraph.getConstraint(c);
			CARL_LOG_DEBUG("carl.cad", "Checking if " << c << " is satisfied by " << r << " over " << variables);
			bool sat = c.satisfiedBy(r, variables);
			conflictGraph.set(constraintID, sampleID, !sat);
			satisfied = satisfied && sat;
		}
		return satisfied;
	}
	
	template<typename Num>
	friend std::ostream& operator<<(std::ostream& os, const CADConstraints<Num>& c) {
		os << c.mConstraints << std::endl;
		for (std::size_t var = 0; var < c.mVariableLookup.size(); var++) {
			os << "\t" << var << " -> ";
			for (auto id: c.mVariableLookup[var]) {
				os << c.mConstraints[id] << ", ";
			}
			os << std::endl;
		}
		return os;
	}
};

}
}
