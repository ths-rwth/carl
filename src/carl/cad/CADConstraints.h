/**
 * @file CADConstraints.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include <forward_list>
#include <vector>

#include "ConflictGraph.h"
#include "Constraint.h"
#include "../core/RealAlgebraicPoint.h"

namespace carl {
namespace cad {

template<typename Number>
class CADConstraints {
private:
	std::vector<cad::Constraint<Number>> constraints;
public:
	bool empty() const {
		return constraints.empty();
	}
	std::size_t size() const {
		return constraints.size();
	}
	
	CADConstraints<Number>& operator=(const std::vector<cad::Constraint<Number>>& constraints) {
		this->constraints = constraints;
		return *this;
	}
	
	bool satisfiedBy(RealAlgebraicPoint<Number>& r) const {
		for (const auto& c: constraints) {
			if (!c.satisfiedBy(r)) return false;
		}
		return true;
	}
	bool satisfiedBy(RealAlgebraicPoint<Number>& r, cad::ConflictGraph& conflictGraph) const {
		bool satisfied = true;
		std::forward_list<unsigned> vertices;
		for (std::size_t i = 0; i < constraints.size(); i++) {
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
};

}
}