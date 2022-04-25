/**
 * Constraint.tpp
 * @author Florian Corzilius<corzilius@cs.rwth-aachen.de>
 * @author Sebastian Junges
 * @author Ulrich Loup
 * @since 2010-04-26
 * @version 2013-10-30
 */

#include "Constraint.h"
#include "../../core/polynomialfunctions/Factorization.h"
#include "../../core/polynomialfunctions/Substitution.h"

namespace carl {

template<typename Pol>
unsigned Constraint<Pol>::consistentWith(const EvaluationMap<Interval<double>>& _solutionInterval) const {
	auto vars = variables();
	if (vars.empty())
		return carl::evaluate(lhs().constantPart(), relation()) ? 1 : 0;
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
		Interval<double> solutionSpace = IntervalEvaluation::evaluate(lhs(), _solutionInterval);
		if (solutionSpace.isEmpty())
			return 2;
		switch (relation()) {
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

template<typename Pol>
unsigned Constraint<Pol>::consistentWith(const EvaluationMap<Interval<double>>& _solutionInterval, Relation& _stricterRelation) const {
	_stricterRelation = relation();
	auto vars = variables();
	if (vars.empty())
		return carl::evaluate(lhs().constantPart(), relation()) ? 1 : 0;
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
		Interval<double> solutionSpace = IntervalEvaluation::evaluate(lhs(), _solutionInterval);
		if (solutionSpace.isEmpty())
			return 2;
		switch (relation()) {
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

template<typename Pol>
bool Constraint<Pol>::hasFinitelyManySolutionsIn(const Variable& _var) const {
	if (variables().has(_var))
		return true;
	if (relation() == Relation::EQ) {
		if (variables().size() == 1)
			return true;
		//TODO: else, if not too expensive (construct constraints being the side conditions)
	}
	return false;
}


} // namespace carl
