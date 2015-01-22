/**
 * @file CADConstraints.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include <forward_list>
#include <iostream>
#include <vector>

#include "CADTypes.h"
#include "ConflictGraph.h"
#include "Constraint.h"
#include "../core/RealAlgebraicPoint.h"
#include "../io/streamingOperators.h"

namespace carl {
namespace cad {

template<typename Number>
class CADConstraints {
private:
	std::vector<cad::Constraint<Number>> constraints;
public:
	CADConstraints<Number>& operator=(const std::vector<cad::Constraint<Number>>& constraints) {
		this->constraints = constraints;
		return *this;
	}
	
	bool empty() const {
		return constraints.empty();
	}
	std::size_t size() const {
		return constraints.size();
	}
	auto begin() const -> decltype(constraints.begin()) {
		return constraints.begin();
	}
	auto end() const -> decltype(constraints.end()) {
		return constraints.end();
	}
	
	bool satisfiedBy(RealAlgebraicPoint<Number>& r, const std::vector<Variable>& variables) const {
		for (const auto& c: constraints) {
			if (!c.satisfiedBy(r, variables)) return false;
		}
		return true;
	}
	bool satisfiedBy(RealAlgebraicPoint<Number>& r, const std::vector<Variable>& variables, cad::ConflictGraph& conflictGraph) const {
		bool satisfied = true;
		std::forward_list<std::size_t> vertices;
		for (std::size_t i = 0; i < constraints.size(); i++) {
			if (constraints[i].satisfiedBy(r, variables)) {
				vertices.push_front(i);
			} else {
				satisfied = false;
			}
		}
		// store that the constraints are satisfied by r
		/// @todo What if r was already seen? this generates a new sample vertex.
		conflictGraph.addEdges(vertices.begin(), vertices.end());
		return satisfied;
	}
	
	template<typename Num>
	friend std::ostream& operator<<(std::ostream& os, const CADConstraints<Num>& c) {
		return os << c.constraints;
	}
};

}
}