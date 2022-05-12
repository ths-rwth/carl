#pragma once

#include <carl/core/Common.h>
#include "BasicConstraint.h"
#include <carl/interval/Interval.h>
#include <carl/poly/umvpoly/functions/IntervalEvaluation.h>
#include <boost/logic/tribool_io.hpp>

namespace carl {

template<typename Number, typename Poly>
inline boost::tribool evaluate(const BasicConstraint<Poly>& c, const Assignment<Interval<Number>>& map) {
	return evaluate(evaluate(c.lhs(), map), c.relation());
}

/**
 * Checks whether this constraint is consistent with the given assignment from 
 * the its variables to interval domains.
 * @param _solutionInterval The interval domains of the variables.
 * @return 1, if this constraint is consistent with the given intervals;
 *          0, if this constraint is not consistent with the given intervals;
 *          2, if it cannot be decided whether this constraint is consistent with the given intervals.
 */
template<typename Pol>
static unsigned consistentWith(const BasicConstraint<Pol>& c, const Assignment<Interval<double>>& _solutionInterval) {
	auto vars = variables(c);
	if (vars.empty())
		return carl::evaluate(c.lhs().constantPart(), c.relation()) ? 1 : 0;
	else {
		auto varIter = vars.begin();
		auto varIntervalIter = _solutionInterval.begin();
		while (varIter != vars.end() && varIntervalIter != _solutionInterval.end()) {
			if (*varIter < varIntervalIter->first) {
				return 2;
			} else if (*varIter > varIntervalIter->first) {
				++varIntervalIter;
			} else {
				++varIter;
				++varIntervalIter;
			}
		}
		if (varIter != vars.end())
			return 2;
		Interval<double> solutionSpace = carl::evaluate(c.lhs(), _solutionInterval);
		if (solutionSpace.isEmpty())
			return 2;
		switch (c.relation()) {
		case Relation::EQ: {
			if (solutionSpace.isZero())
				return 1;
			else if (!solutionSpace.contains(0))
				return 0;
			break;
		}
		case Relation::NEQ: {
			if (!solutionSpace.contains(0))
				return 1;
			break;
		}
		case Relation::LESS: {
			if (solutionSpace.upperBoundType() != BoundType::INFTY) {
				if (solutionSpace.upper() < 0)
					return 1;
				else if (solutionSpace.upper() == 0 && solutionSpace.upperBoundType() == BoundType::STRICT)
					return 1;
			}
			if (solutionSpace.lowerBoundType() != BoundType::INFTY && solutionSpace.lower() >= 0)
				return 0;
			break;
		}
		case Relation::GREATER: {
			if (solutionSpace.lowerBoundType() != BoundType::INFTY) {
				if (solutionSpace.lower() > 0)
					return 1;
				else if (solutionSpace.lower() == 0 && solutionSpace.lowerBoundType() == BoundType::STRICT)
					return 1;
			}
			if (solutionSpace.upperBoundType() != BoundType::INFTY && solutionSpace.upper() <= 0)
				return 0;
			break;
		}
		case Relation::LEQ: {
			if (solutionSpace.upperBoundType() != BoundType::INFTY && solutionSpace.upper() <= 0)
				return 1;
			if (solutionSpace.lowerBoundType() != BoundType::INFTY) {
				if (solutionSpace.lower() > 0)
					return 0;
				else if (solutionSpace.lower() == 0 && solutionSpace.lowerBoundType() == BoundType::STRICT)
					return 0;
			}
			break;
		}
		case Relation::GEQ: {
			if (solutionSpace.lowerBoundType() != BoundType::INFTY && solutionSpace.lower() >= 0)
				return 1;
			if (solutionSpace.upperBoundType() != BoundType::INFTY) {
				if (solutionSpace.upper() < 0)
					return 0;
				else if (solutionSpace.upper() == 0 && solutionSpace.upperBoundType() == BoundType::STRICT)
					return 0;
			}
			break;
		}
		default: {
			std::cout << "Error in isConsistent: unexpected relation symbol." << std::endl;
			return 0;
		}
		}
		return 2;
	}
}

/**
 * Checks whether this constraint is consistent with the given assignment from 
 * the its variables to interval domains.
 * @param _solutionInterval The interval domains of the variables.
 * @param _stricterRelation This relation is set to a relation R such that this constraint and the given variable bounds
 *                           imply the constraint formed by R, comparing this constraint's left-hand side to zero.
 * @return 1, if this constraint is consistent with the given intervals;
 *          0, if this constraint is not consistent with the given intervals;
 *          2, if it cannot be decided whether this constraint is consistent with the given intervals.
 */
template<typename Pol>
static unsigned consistentWith(const BasicConstraint<Pol>& c, const Assignment<Interval<double>>& _solutionInterval, Relation& _stricterRelation) {
	_stricterRelation = c.relation();
	auto vars = variables(c);
	if (vars.empty())
		return carl::evaluate(c.lhs().constantPart(), c.relation()) ? 1 : 0;
	else {
		auto varIter = vars.begin();
		auto varIntervalIter = _solutionInterval.begin();
		while (varIter != vars.end() && varIntervalIter != _solutionInterval.end()) {
			if (*varIter < varIntervalIter->first) {
				return 2;
			} else if (*varIter > varIntervalIter->first) {
				++varIntervalIter;
			} else {
				++varIter;
				++varIntervalIter;
			}
		}
		if (varIter != vars.end())
			return 2;
		Interval<double> solutionSpace = carl::evaluate(c.lhs(), _solutionInterval);
		if (solutionSpace.isEmpty())
			return 2;
		switch (c.relation()) {
		case Relation::EQ: {
			if (solutionSpace.isZero())
				return 1;
			else if (!solutionSpace.contains(0))
				return 0;
			break;
		}
		case Relation::NEQ: {
			if (!solutionSpace.contains(0))
				return 1;
			if (solutionSpace.upperBoundType() == BoundType::WEAK && solutionSpace.upper() == 0) {
				_stricterRelation = Relation::LESS;
			} else if (solutionSpace.lowerBoundType() == BoundType::WEAK && solutionSpace.lower() == 0) {
				_stricterRelation = Relation::GREATER;
			}
			break;
		}
		case Relation::LESS: {
			if (solutionSpace.upperBoundType() != BoundType::INFTY) {
				if (solutionSpace.upper() < 0)
					return 1;
				else if (solutionSpace.upper() == 0 && solutionSpace.upperBoundType() == BoundType::STRICT)
					return 1;
			}
			if (solutionSpace.lowerBoundType() != BoundType::INFTY && solutionSpace.lower() >= 0)
				return 0;
			break;
		}
		case Relation::GREATER: {
			if (solutionSpace.lowerBoundType() != BoundType::INFTY) {
				if (solutionSpace.lower() > 0)
					return 1;
				else if (solutionSpace.lower() == 0 && solutionSpace.lowerBoundType() == BoundType::STRICT)
					return 1;
			}
			if (solutionSpace.upperBoundType() != BoundType::INFTY && solutionSpace.upper() <= 0)
				return 0;
			break;
		}
		case Relation::LEQ: {
			if (solutionSpace.upperBoundType() != BoundType::INFTY) {
				if (solutionSpace.upper() <= 0) {
					return 1;
				}
			}
			if (solutionSpace.lowerBoundType() != BoundType::INFTY) {
				if (solutionSpace.lower() > 0) {
					return 0;
				} else if (solutionSpace.lower() == 0) {
					if (solutionSpace.lowerBoundType() == BoundType::STRICT) {
						return 0;
					} else {
						_stricterRelation = Relation::EQ;
					}
				}
			}
			break;
		}
		case Relation::GEQ: {
			if (solutionSpace.lowerBoundType() != BoundType::INFTY) {
				if (solutionSpace.lower() >= 0)
					return 1;
			}
			if (solutionSpace.upperBoundType() != BoundType::INFTY) {
				if (solutionSpace.upper() < 0)
					return 0;
				else if (solutionSpace.upper() == 0) {
					if (solutionSpace.upperBoundType() == BoundType::STRICT)
						return 0;
					else
						_stricterRelation = Relation::EQ;
				}
			}
			break;
		}
		default: {
			std::cout << "Error in isConsistent: unexpected relation symbol." << std::endl;
			return 0;
		}
		}
		return 2;
	}
}

}