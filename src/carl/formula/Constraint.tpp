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
	: mLhs(RawConstraint<Pol>(_valid).mLhs), mRelation(RawConstraint<Pol>(_valid).mRelation), mHash(CONSTRAINT_HASH(mLhs, mRelation, Pol)) {
}

template<typename Pol>
Constraint<Pol>::Constraint(carl::Variable::Arg _var, Relation _rel, const typename Pol::NumberType& _bound) {
	RawConstraint<Pol> raw(_var, _rel, _bound);
	mLhs = raw.mLhs;
	mRelation = raw.mRelation;
	mHash = CONSTRAINT_HASH(mLhs, mRelation, Pol);
}

template<typename Pol>
Constraint<Pol>::Constraint(const Pol& _lhs, Relation _rel) {
	RawConstraint<Pol> raw(_lhs, _rel);
	mLhs = raw.mLhs;
	mRelation = raw.mRelation;
	mHash = CONSTRAINT_HASH(mLhs, mRelation, Pol);
}

template<typename Pol>
Constraint<Pol>::Constraint(const Constraint<Pol>& _constraint)
	: mLhs(_constraint.mLhs), mRelation(_constraint.mRelation), mHash(_constraint.mHash), mFactorization(_constraint.mFactorization) {}

template<typename Pol>
Constraint<Pol>::Constraint(Constraint<Pol>&& _constraint) noexcept
	: mLhs(std::move(_constraint.mLhs)), mRelation(_constraint.mRelation), mHash(_constraint.mHash), mFactorization(std::move(_constraint.mFactorization)) {}

template<typename Pol>
Constraint<Pol>& Constraint<Pol>::operator=(const Constraint<Pol>& _constraint) {
	if (*this == _constraint) {
		if (mFactorization.empty()) {
			mFactorization = _constraint.mFactorization;
		}
	} else {
		mLhs = _constraint.mLhs;
		mRelation = _constraint.mRelation;
		mHash = _constraint.mHash;
		mFactorization = _constraint.mFactorization;
	}
	return *this;
}

template<typename Pol>
Constraint<Pol>& Constraint<Pol>::operator=(Constraint<Pol>&& _constraint) noexcept {
	if (*this == _constraint) {
		if (mFactorization.empty()) {
			mFactorization = std::move(_constraint.mFactorization);
		}
	} else {
		mLhs = std::move(_constraint.mLhs);
		mRelation = _constraint.mRelation;
		mHash = _constraint.mHash;
		mFactorization = std::move(_constraint.mFactorization);
	}
	return *this;
}

template<typename Pol>
unsigned satisfiedBy(const Constraint<Pol>& c, const EvaluationMap<typename Pol::NumberType>& _assignment) {
	unsigned result = 2;
	Pol tmp = carl::substitute(c.lhs(), _assignment);
	if (tmp.isConstant()) {
		result = carl::evaluate((isZero(tmp) ? typename Pol::NumberType(0) : tmp.trailingTerm().coeff()), c.relation()) ? 1 : 0;
	}
	return result;
}

template<typename Pol>
unsigned Constraint<Pol>::consistentWith(const EvaluationMap<Interval<double>>& _solutionInterval) const {
	auto vars = variables();
	if (vars.empty())
		return carl::evaluate(constantPart(), relation()) ? 1 : 0;
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
		return carl::evaluate(constantPart(), relation()) ? 1 : 0;
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
	if (hasVariable(_var))
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
bool Constraint<Pol>::isPseudoBoolean() const {
	return !variables().boolean().empty();
}

template<typename Pol>
signed compare(const Constraint<Pol>& _constraintA, const Constraint<Pol>& _constraintB) {
	/*
    * Check whether it holds that 
    * 
    *                      _constraintA  =  a_1*m_1+...+a_k*m_k + c ~ 0
    * and 
    *                      _constraintB  =  b_1*m_1+...+b_k*m_k + d ~ 0, 
    * 
    * where a_1,..., a_k, b_1,..., b_k, c, d are rational coefficients, 
    *       m_1,..., m_k are non-constant monomials and 
    *       exists a rational g such that 
    * 
    *                   a_i = g * b_i for all 1<=i<=k 
    *              or   b_i = g * a_i for all 1<=i<=k 
    */
	typename Pol::NumberType one_divided_by_a = _constraintA.lhs().coprimeFactorWithoutConstant();
	typename Pol::NumberType one_divided_by_b = _constraintB.lhs().coprimeFactorWithoutConstant();
	typename Pol::NumberType c = _constraintA.lhs().constantPart();
	typename Pol::NumberType d = _constraintB.lhs().constantPart();
	assert(carl::isOne(carl::getNum(carl::abs(one_divided_by_b))));
	Pol tmpA = (_constraintA.lhs() - c) * one_divided_by_a;
	Pol tmpB = (_constraintB.lhs() - d) * one_divided_by_b;
	//        std::cout << "tmpA = " << tmpA << std::endl;
	//        std::cout << "tmpB = " << tmpB << std::endl;
	if (tmpA != tmpB) return 0;
	bool termACoeffGreater = false;
	bool signsDiffer = (one_divided_by_a < carl::constant_zero<typename Pol::NumberType>::get()) != (one_divided_by_b < carl::constant_zero<typename Pol::NumberType>::get());
	typename Pol::NumberType g;
	if (carl::getDenom(one_divided_by_a) > carl::getDenom(one_divided_by_b)) {
		g = typename Pol::NumberType(carl::getDenom(one_divided_by_a)) / carl::getDenom(one_divided_by_b);
		if (signsDiffer)
			g = -g;
		termACoeffGreater = true;
		d *= g;
	} else {
		g = typename Pol::NumberType(carl::getDenom(one_divided_by_b)) / carl::getDenom(one_divided_by_a);
		if (signsDiffer)
			g = -g;
		c *= g;
	}
	// Apply the multiplication by a negative g to the according relation symbol, which
	// has to be turned around then.
	Relation relA = _constraintA.relation();
	Relation relB = _constraintB.relation();
	if (g < 0) {
		if (termACoeffGreater) {
			switch (relB) {
			case Relation::LEQ:
				relB = Relation::GEQ;
				break;
			case Relation::GEQ:
				relB = Relation::LEQ;
				break;
			case Relation::LESS:
				relB = Relation::GREATER;
				break;
			case Relation::GREATER:
				relB = Relation::LESS;
				break;
			default:
				break;
			}
		} else {
			switch (relA) {
			case Relation::LEQ:
				relA = Relation::GEQ;
				break;
			case Relation::GEQ:
				relA = Relation::LEQ;
				break;
			case Relation::LESS:
				relA = Relation::GREATER;
				break;
			case Relation::GREATER:
				relA = Relation::LESS;
				break;
			default:
				break;
			}
		}
	}
	//        std::cout << "c = " << c << std::endl;
	//        std::cout << "d = " << d << std::endl;
	//        std::cout << "g = " << g << std::endl;
	//        std::cout << "relA = " << relA << std::endl;
	//        std::cout << "relB = " << relB << std::endl;
	// Compare the adapted constant parts.
	switch (relB) {
	case Relation::EQ:
		switch (relA) {
		case Relation::EQ: // p+c=0  and  p+d=0
			if (c == d) return A_IFF_B;
			return NOT__A_AND_B;
		case Relation::NEQ: // p+c!=0  and  p+d=0
			if (c == d) return A_XOR_B;
			return B_IMPLIES_A;
		case Relation::LESS: // p+c<0  and  p+d=0
			if (c < d) return B_IMPLIES_A;
			return NOT__A_AND_B;
		case Relation::GREATER: // p+c>0  and  p+d=0
			if (c > d) return B_IMPLIES_A;
			return NOT__A_AND_B;
		case Relation::LEQ: // p+c<=0  and  p+d=0
			if (c <= d) return B_IMPLIES_A;
			return NOT__A_AND_B;
		case Relation::GEQ: // p+c>=0  and  p+d=0
			if (c >= d) return B_IMPLIES_A;
			return NOT__A_AND_B;
		default:
			return false;
		}
	case Relation::NEQ:
		switch (relA) {
		case Relation::EQ: // p+c=0  and  p+d!=0
			if (c == d) return A_XOR_B;
			return A_IMPLIES_B;
		case Relation::NEQ: // p+c!=0  and  p+d!=0
			if (c == d) return A_IFF_B;
			return 0;
		case Relation::LESS: // p+c<0  and  p+d!=0
			if (c >= d) return A_IMPLIES_B;
			return 0;
		case Relation::GREATER: // p+c>0  and  p+d!=0
			if (c <= d) return A_IMPLIES_B;
			return 0;
		case Relation::LEQ: // p+c<=0  and  p+d!=0
			if (c > d) return A_IMPLIES_B;
			if (c == d) return A_AND_B__IFF_C;
			return 0;
		case Relation::GEQ: // p+c>=0  and  p+d!=0
			if (c < d) return A_IMPLIES_B;
			if (c == d) return A_AND_B__IFF_C;
			return 0;
		default:
			return 0;
		}
	case Relation::LESS:
		switch (relA) {
		case Relation::EQ: // p+c=0  and  p+d<0
			if (c > d) return A_IMPLIES_B;
			return NOT__A_AND_B;
		case Relation::NEQ: // p+c!=0  and  p+d<0
			if (c <= d) return B_IMPLIES_A;
			return 0;
		case Relation::LESS: // p+c<0  and  p+d<0
			if (c == d) return A_IFF_B;
			if (c < d) return B_IMPLIES_A;
			return A_IMPLIES_B;
		case Relation::GREATER: // p+c>0  and  p+d<0
			if (c <= d) return NOT__A_AND_B;
			return 0;
		case Relation::LEQ: // p+c<=0  and  p+d<0
			if (c > d) return A_IMPLIES_B;
			return B_IMPLIES_A;
		case Relation::GEQ: // p+c>=0  and  p+d<0
			if (c < d) return NOT__A_AND_B;
			if (c == d) return A_XOR_B;
			return 0;
		default:
			return 0;
		}
	case Relation::GREATER: {
		switch (relA) {
		case Relation::EQ: // p+c=0  and  p+d>0
			if (c < d) return A_IMPLIES_B;
			return NOT__A_AND_B;
		case Relation::NEQ: // p+c!=0  and  p+d>0
			if (c >= d) return B_IMPLIES_A;
			return 0;
		case Relation::LESS: // p+c<0  and  p+d>0
			if (c >= d) return NOT__A_AND_B;
			return 0;
		case Relation::GREATER: // p+c>0  and  p+d>0
			if (c == d) return A_IFF_B;
			if (c > d) return B_IMPLIES_A;
			return A_IMPLIES_B;
		case Relation::LEQ: // p+c<=0  and  p+d>0
			if (c > d) return NOT__A_AND_B;
			if (c == d) return A_XOR_B;
			return 0;
		case Relation::GEQ: // p+c>=0  and  p+d>0
			if (c > d) return B_IMPLIES_A;
			return A_IMPLIES_B;
		default:
			return 0;
		}
	}
	case Relation::LEQ: {
		switch (relA) {
		case Relation::EQ: // p+c=0  and  p+d<=0
			if (c >= d) return A_IMPLIES_B;
			return NOT__A_AND_B;
		case Relation::NEQ: // p+c!=0  and  p+d<=0
			if (c < d) return B_IMPLIES_A;
			if (c == d) return A_AND_B__IFF_C;
			return 0;
		case Relation::LESS: // p+c<0  and  p+d<=0
			if (c < d) return B_IMPLIES_A;
			return A_IMPLIES_B;
		case Relation::GREATER: // p+c>0  and  p+d<=0
			if (c < d) return NOT__A_AND_B;
			if (c == d) return A_XOR_B;
			return 0;
		case Relation::LEQ: // p+c<=0  and  p+d<=0
			if (c == d) return A_IFF_B;
			if (c < d) return B_IMPLIES_A;
			return A_IMPLIES_B;
		case Relation::GEQ: // p+c>=0  and  p+d<=0
			if (c < d) return NOT__A_AND_B;
			if (c == d) return A_AND_B__IFF_C;
			return 0;
		default:
			return 0;
		}
	}
	case Relation::GEQ: {
		switch (relA) {
		case Relation::EQ: // p+c=0  and  p+d>=0
			if (c <= d) return A_IMPLIES_B;
			return NOT__A_AND_B;
		case Relation::NEQ: // p+c!=0  and  p+d>=0
			if (c > d) return B_IMPLIES_A;
			if (c == d) return A_AND_B__IFF_C;
			return 0;
		case Relation::LESS: // p+c<0  and  p+d>=0
			if (c > d) return NOT__A_AND_B;
			if (c == d) return A_XOR_B;
			return 0;
		case Relation::GREATER: // p+c>0  and  p+d>=0
			if (c < d) return B_IMPLIES_A;
			return A_IMPLIES_B;
		case Relation::LEQ: // p+c<=0  and  p+d>=0
			if (c > d) return NOT__A_AND_B;
			if (c == d) return A_AND_B__IFF_C;
			return 0;
		case Relation::GEQ: // p+c>=0  and  p+d>=0
			if (c == d) return A_IFF_B;
			if (c < d) return A_IMPLIES_B;
			return B_IMPLIES_A;
		default:
			return 0;
		}
	}
	default:
		return 0;
	}
}

} // namespace carl
