/**
 * Constraint.tpp
 * @author Florian Corzilius<corzilius@cs.rwth-aachen.de>
 * @author Sebastian Junges
 * @author Ulrich Loup
 * @since 2010-04-26
 * @version 2013-10-30
 */

#include "Constraint.h"
#include "../core/polynomialfunctions/Definiteness.h"
#include "../core/polynomialfunctions/Factorization.h"
#include "../core/polynomialfunctions/Substitution.h"

namespace carl {

template<typename Pol>
Constraint<Pol>::Constraint(bool _valid)
	: mLhs(RawConstraint<Pol>(_valid).mLhs), mRelation(RawConstraint<Pol>(_valid).mRelation) {
}

template<typename Pol>
Constraint<Pol>::Constraint(carl::Variable::Arg _var, Relation _rel, const typename Pol::NumberType& _bound)
	: mLhs(RawConstraint<Pol>(_var, _rel, _bound).mLhs), mRelation(RawConstraint<Pol>(_var, _rel, _bound).mRelation) {
}

template<typename Pol>
Constraint<Pol>::Constraint(const Pol& _lhs, Relation _rel)
	: mLhs(RawConstraint<Pol>(_lhs, _rel).mLhs), mRelation(RawConstraint<Pol>(_lhs, _rel).mRelation) {
}

template<typename Pol>
Constraint<Pol>::Constraint(const Constraint<Pol>& _constraint)
	: mLhs(_constraint.mLhs), mRelation(_constraint.mRelation) {}

template<typename Pol>
Constraint<Pol>::Constraint(Constraint<Pol>&& _constraint) noexcept
	: mLhs(std::move(_constraint.mLhs)), mRelation(_constraint.mRelation) {}

template<typename Pol>
Constraint<Pol>& Constraint<Pol>::operator=(const Constraint<Pol>& _constraint) {
	mLhs = _constraint.mLhs;
	mRelation = _constraint.mRelation;
	return *this;
}

template<typename Pol>
Constraint<Pol>& Constraint<Pol>::operator=(Constraint<Pol>&& _constraint) noexcept {
	mLhs = std::move(_constraint.mLhs);
	mRelation = _constraint.mRelation;
	return *this;
}

template<typename Pol>
unsigned Constraint<Pol>::satisfiedBy(const EvaluationMap<typename Pol::NumberType>& _assignment) const {
	//        std::cout << "Is  " << this->toString( 0, true, true ) << std::endl;
	//        std::cout << "satisfied by  " << std::endl;
	//        for( auto iter = _assignment.begin(); iter != _assignment.end(); ++iter )
	//            std::cout << iter->first << " in " << iter->second << std::endl;
	unsigned result = 2;
	Pol tmp = carl::substitute(mLhs, _assignment);
	if (tmp.isConstant()) {
		result = carl::evaluate((isZero(tmp) ? typename Pol::NumberType(0) : tmp.trailingTerm().coeff()), relation()) ? 1 : 0;
	}
	//        std::cout << "result is " << result << std::endl;
	//        std::cout << std::endl;
	return result;
}

template<typename Pol>
unsigned Constraint<Pol>::consistentWith(const EvaluationMap<Interval<double>>& _solutionInterval) const {
	if (variables().empty())
		return carl::evaluate(constantPart(), relation()) ? 1 : 0;
	else {
		auto& vars = variables().as_vector();
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
	if (variables().empty())
		return carl::evaluate(constantPart(), relation()) ? 1 : 0;
	else {
		auto& vars = variables().as_vector();
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
unsigned Constraint<Pol>::evaluate(const EvaluationMap<Interval<typename carl::UnderlyingNumberType<Pol>::type>>& _assignment) const {
	// 0 = False, 1 = Null, 2 = Maybe, 3 = True
	Interval<typename carl::UnderlyingNumberType<Pol>::type> res = IntervalEvaluation::evaluate(lhs(), _assignment);
	switch (relation()) {
	case Relation::EQ: {
		if (res.isZero())
			return 3;
		else if (res.contains(0))
			return 2;
		else
			return 0;
	}
	case Relation::NEQ: {
		if (res.isZero())
			return 0;
		else if (res.contains(0))
			return 2;
		else
			return 3;
	}
	case Relation::LESS: {
		if (res.isNegative())
			return 3;
		else if (res.isSemiPositive())
			return 0;
		else
			return 2;
	}
	case Relation::GREATER: {
		if (res.isPositive())
			return 3;
		else if (res.isSemiNegative())
			return 0;
		else
			return 2;
	}
	case Relation::LEQ: {
		if (res.isSemiNegative())
			return 3;
		else if (res.isPositive())
			return 0;
		else if (res.isSemiPositive())
			return 1;
		else
			return 2;
	}
	case Relation::GEQ: {
		if (res.isSemiPositive())
			return 3;
		else if (res.isNegative())
			return 0;
		else if (res.isSemiNegative())
			return 1;
		else
			return 2;
	}
	}
	assert(false);
	return 1;
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

template<typename Pol>
Pol Constraint<Pol>::coefficient(const Variable& _var, uint _degree) const {
	auto vi = varInfo<true>(_var);
	auto d = vi.coeffs().find(_degree);
	return d != vi.coeffs().end() ? d->second : Pol(typename Pol::NumberType(0));
}

template<typename Pol>
bool Constraint<Pol>::getSubstitution(Variable& _substitutionVariable, Pol& _substitutionTerm, bool _negated, const Variable& _exclude) const {
	if ((!_negated && relation() != Relation::EQ) || (_negated && relation() != Relation::NEQ))
		return false;
	for (const auto& var : variables()) {
		if (var == _exclude) continue;
		auto vi = varInfo<true>(var);
		if (vi.maxDegree() == 1) {
			auto d = vi.coeffs().find(1);
			assert(d != vi.coeffs().end());
			if (d->second.isConstant() && (var.type() != carl::VariableType::VT_INT || carl::isOne(carl::abs(d->second.constantPart())))) {
				_substitutionVariable = var;
				_substitutionTerm = Pol(_substitutionVariable) * d->second - lhs();
				_substitutionTerm /= d->second.constantPart();
				return true;
			}
		}
	}
	return false;
}

template<typename Pol>
bool Constraint<Pol>::getAssignment(Variable& _substitutionVariable, typename Pol::NumberType& _substitutionValue) const {
	if (relation() != Relation::EQ) return false;
	if (lhs().nrTerms() > 2) return false;
	if (lhs().nrTerms() == 0) return false;
	if (!lhs().lterm().isSingleVariable()) return false;
	if (lhs().nrTerms() == 1) {
		_substitutionVariable = lhs().lterm().getSingleVariable();
		_substitutionValue = 0;
		return true;
	}
	assert(lhs().nrTerms() == 2);
	if (!lhs().trailingTerm().isConstant()) return false;
	_substitutionVariable = lhs().lterm().getSingleVariable();
	_substitutionValue = -lhs().trailingTerm().coeff() / lhs().lterm().coeff();
	return true;
}

template<typename Pol>
bool Constraint<Pol>::isPseudoBoolean() const {
	return !variables().boolean().empty();
}
} // namespace carl
