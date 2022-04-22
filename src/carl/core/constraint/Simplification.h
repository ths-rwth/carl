#pragma once

#include "BasicConstraint.h"
#include "../polynomialfunctions/Definiteness.h"

namespace carl::constraint {

template<typename Pol>
BasicConstraint<Pol> init_bound(Variable var, Relation rel, const typename Pol::NumberType& bound) {
	CARL_LOG_FUNC("carl.constraint", var << ", " << rel << ", " << bound);
	auto lhs = Pol(var);
	switch (rel) {
	case Relation::GREATER:
		lhs = -lhs;
		if (var.type() == VariableType::VT_INT) {
			if (isInteger(bound))
				lhs += bound + typename Pol::NumberType(1);
			else
				lhs += carl::ceil(bound);
			rel = Relation::LEQ;
		} else {
			lhs += bound;
			rel = Relation::LESS;
		}
		break;
	case Relation::GEQ:
		lhs = -lhs;
		if (var.type() == VariableType::VT_INT) {
			if (isInteger(bound))
				lhs += bound;
			else
				lhs += carl::ceil(bound);
			rel = Relation::LEQ;
		} else {
			lhs += bound;
			rel = Relation::LEQ;
		}
		break;
	case Relation::LESS:
		if (var.type() == VariableType::VT_INT) {
			if (isInteger(bound))
				lhs -= (bound - typename Pol::NumberType(1));
			else
				lhs -= carl::floor(bound);
			rel = Relation::LEQ;
		} else {
			lhs -= bound;
		}
		break;
	case Relation::LEQ:
		if (var.type() == VariableType::VT_INT) {
			if (isInteger(bound))
				lhs -= bound;
			else
				lhs -= carl::floor(bound);
		} else {
			lhs -= bound;
		}
		break;
	case Relation::EQ:
		if (var.type() == VariableType::VT_INT) {
			if (isInteger(bound)) {
				lhs -= bound;
			} else {
				lhs = Pol(typename Pol::NumberType(0));
				rel = Relation::LESS;
			}
		} else {
			lhs -= bound;
		}
		break;
	case Relation::NEQ:
		if (var.type() == VariableType::VT_INT) {
			if (isInteger(bound)) {
				lhs -= bound;
			} else {
				lhs = Pol(typename Pol::NumberType(0));
				rel = Relation::EQ;
			}
		} else {
			lhs -= bound;
		}
		break;
	}
	return BasicConstraint<Pol>(std::move(lhs), rel);
}

template<typename Pol>
BasicConstraint<Pol> init_constraint(const Pol& lhs, Relation rel) {
	CARL_LOG_FUNC("carl.core.constraint", lhs << ", " << rel);
	if (lhs.isConstant()) {
		CARL_LOG_TRACE("carl.core.constraint", lhs << " is constant, we simply evaluate.");
		return evaluate(lhs.constantPart(), rel) ? BasicConstraint<Pol>(true) : BasicConstraint<Pol>(false);
	} else if (lhs.totalDegree() == 1 && (rel != Relation::EQ && rel != Relation::NEQ) && lhs.isUnivariate()) {
		if (carl::isNegative(lhs.lcoeff())) {
			CARL_LOG_TRACE("carl.core.constraint", "Normalizing leading coefficient of linear poly.");
			switch (rel) {
			case Relation::LESS:
				rel = Relation::GREATER;
				break;
			case Relation::GREATER:
				rel = Relation::LESS;
				break;
			case Relation::LEQ:
				rel = Relation::GEQ;
				break;
			case Relation::GEQ:
				rel = Relation::LEQ;
				break;
			default:
				assert(false);
				break;
			}
		}
		CARL_LOG_TRACE("carl.core.constraint", "Rewriting to bound");
		return init_bound<Pol>(lhs.getSingleVariable(), rel, (-lhs.constantPart()) / lhs.lcoeff());
	} else {
		CARL_LOG_TRACE("carl.core.constraint", "Normalizing " << lhs << " " << rel << " 0");
		auto new_lhs = isZero(lhs) ? Pol(typename Pol::NumberType(0)) : lhs.coprimeCoefficients();
		if (rel == Relation::EQ || rel == Relation::NEQ) {
			if (!isZero(new_lhs) && new_lhs.lterm().coeff() < typename Pol::NumberType(0)) new_lhs = -new_lhs;
		} else if (rel == Relation::LEQ || rel == Relation::LESS) {
			if (!isZero(new_lhs) && (lhs.lterm().coeff() < 0) != (new_lhs.lterm().coeff() < 0)) new_lhs = -new_lhs;
		} else if (rel == Relation::GREATER) {
			if (!isZero(new_lhs) && (lhs.lterm().coeff() < 0) == (new_lhs.lterm().coeff() < 0)) new_lhs = -new_lhs;
			rel = Relation::LESS;
		} else if (rel == Relation::GEQ) {
			if (!isZero(new_lhs) && (lhs.lterm().coeff() < 0) == (new_lhs.lterm().coeff() < 0)) new_lhs = -new_lhs;
			rel = Relation::LEQ;
		}
		
		return BasicConstraint<Pol>(std::move(new_lhs), rel);
	}
}

template<typename Pol>
inline void normalize_integer_inplace(BasicConstraint<Pol>& constraint) {
	auto vars = variables(constraint);
	if (!vars.integer().empty() && vars.real().empty()) {
		if (constraint.relation() == Relation::LESS) {
			constraint.lhs() += carl::constant_one<typename Pol::CoeffType>::get();
			constraint.relation()  = Relation::LEQ;
		} else if (constraint.relation()  == Relation::GREATER) {
			constraint.lhs() -= carl::constant_one<typename Pol::CoeffType>::get();
			constraint.relation()  = Relation::GEQ;
		}
	}
}

template<typename Pol>
inline unsigned is_consistent_definiteness(const BasicConstraint<Pol>& constraint, std::optional<Definiteness> lhs_definiteness = std::nullopt)  {
	if (constraint.lhs().isConstant()) {
		CARL_LOG_TRACE("carl.formula.constraint", "Lhs " << constraint.lhs() << " is constant");
		return carl::evaluate(constraint.lhs().constantPart(), constraint.relation()) ? 1 : 0;
	} else {
		if (!lhs_definiteness) {
            lhs_definiteness = carl::definiteness(constraint.lhs(), FULL_EFFORT_FOR_DEFINITENESS_CHECK);
        }
		CARL_LOG_TRACE("carl.formula.constraint", "Checking " << constraint.relation() << " against " << *lhs_definiteness);
		switch (constraint.relation()) {
		case Relation::EQ: {
			if (*lhs_definiteness == Definiteness::POSITIVE || *lhs_definiteness == Definiteness::NEGATIVE) return 0;
			break;
		}
		case Relation::NEQ: {
			if (*lhs_definiteness == Definiteness::POSITIVE || *lhs_definiteness == Definiteness::NEGATIVE) return 1;
			break;
		}
		case Relation::LESS: {
			if (*lhs_definiteness == Definiteness::NEGATIVE) return 1;
			if (*lhs_definiteness >= Definiteness::POSITIVE_SEMI) return 0;
			break;
		}
		case Relation::GREATER: {
			if (*lhs_definiteness == Definiteness::POSITIVE) return 1;
			if (*lhs_definiteness <= Definiteness::NEGATIVE_SEMI) return 0;
			break;
		}
		case Relation::LEQ: {
			if (*lhs_definiteness <= Definiteness::NEGATIVE_SEMI) return 1;
			if (*lhs_definiteness == Definiteness::POSITIVE) return 0;
			break;
		}
		case Relation::GEQ: {
			if (*lhs_definiteness >= Definiteness::POSITIVE_SEMI) return 1;
			if (*lhs_definiteness == Definiteness::NEGATIVE) return 0;
			break;
		}
		default:
			assert(false);
			return false;
		}
		return 2;
	}
}

template<typename Pol>
inline void normalize_consistency_inplace(BasicConstraint<Pol>& constraint, std::optional<Definiteness> lhs_definiteness = std::nullopt) {
	unsigned consistency = is_consistent_definiteness(constraint, lhs_definiteness);
	if (consistency == 0) {
		constraint = BasicConstraint<Pol>(false);
	} else if (consistency == 1) {
		constraint = BasicConstraint<Pol>(true);
	}
}

template<typename Pol>
inline bool simplify_nonlinear_univariate_monomial_inplace(BasicConstraint<Pol>& constraint, std::optional<Definiteness> lhs_definiteness = std::nullopt) {
	using PolyT = typename Pol::PolyType;

	auto vars = variables(constraint);
	if (!(vars.size() == 1 && !constraint.lhs().isLinear() && constraint.lhs().nrTerms() == 1)) return false;

    if (!lhs_definiteness) {
        lhs_definiteness = carl::definiteness(constraint.lhs(), FULL_EFFORT_FOR_DEFINITENESS_CHECK);
    }

	// Left-hand side is a non-linear univariate monomial
	Relation rel = constraint.relation();
	if ((*lhs_definiteness == Definiteness::POSITIVE_SEMI && rel == Relation::LEQ) || (*lhs_definiteness == Definiteness::NEGATIVE_SEMI && rel == Relation::GEQ))
		rel = Relation::EQ;
	Variable var = vars.as_vector()[0];

	switch (rel) {
	case Relation::EQ:
		constraint = BasicConstraint<Pol>(PolyT(var), rel);
		break;
	case Relation::NEQ:
		constraint = BasicConstraint<Pol>(PolyT(var), rel);
		break;
	case Relation::LEQ:
		if (*lhs_definiteness == Definiteness::NEGATIVE_SEMI) {
			constraint = init_constraint(PolyT(typename Pol::NumberType(-1)) * PolyT(var) * PolyT(var), rel);
		} else {
			constraint = init_constraint((constraint.lhs().trailingTerm().coeff() > 0 ? PolyT(typename Pol::NumberType(1)) : PolyT(typename Pol::NumberType(-1))) * PolyT(var), rel);
		}
		break;
	case Relation::GEQ:
		if (*lhs_definiteness == Definiteness::POSITIVE_SEMI) {
			constraint = init_constraint(PolyT(var) * PolyT(var), rel);
		} else {
			constraint = init_constraint((constraint.lhs().trailingTerm().coeff() > 0 ? PolyT(typename Pol::NumberType(1)) : PolyT(typename Pol::NumberType(-1))) * PolyT(var), rel);
		}
		break;
	case Relation::LESS:
		if (*lhs_definiteness == Definiteness::NEGATIVE_SEMI) {
			constraint = init_constraint(PolyT(var), Relation::NEQ);
		} else if (*lhs_definiteness == Definiteness::POSITIVE_SEMI) {
            constraint = init_constraint(PolyT(var) * PolyT(var), rel);
        } else {
            constraint = init_constraint((constraint.lhs().trailingTerm().coeff() > 0 ? PolyT(typename Pol::NumberType(1)) : PolyT(typename Pol::NumberType(-1))) * PolyT(var), rel);
        }
		break;
	case Relation::GREATER:
		if (*lhs_definiteness == Definiteness::POSITIVE_SEMI) {
			constraint = init_constraint(PolyT(var), Relation::NEQ);
		} else if (*lhs_definiteness == Definiteness::NEGATIVE_SEMI) {
            constraint = init_constraint(PolyT(typename Pol::NumberType(-1)) * PolyT(var) * PolyT(var), rel);
        } else {
            constraint = init_constraint((constraint.lhs().trailingTerm().coeff() > 0 ? PolyT(typename Pol::NumberType(1)) : PolyT(typename Pol::NumberType(-1))) * PolyT(var), rel);
        }
		break;
	default:
		assert(false);
	}

	return true;
}

template<typename Pol>
inline bool simplify_integer_inplace(BasicConstraint<Pol>& constraint) {
	auto vars = variables(constraint);

	if (vars.integer().empty() || !vars.real().empty()) return false;

	typename Pol::NumberType constPart = constraint.lhs().constantPart();
	if (constPart != typename Pol::NumberType(0)) {
		// Find the gcd of the coefficients of the non-constant terms.
		typename Pol::NumberType g = carl::abs(constraint.lhs().coprimeFactorWithoutConstant());
		assert(g != typename Pol::NumberType(0));
		if (carl::mod(carl::getNum(constPart), carl::getDenom(g)) != 0) {
			switch (constraint.relation()) {
			case Relation::EQ: {
				constraint = BasicConstraint<Pol>(Pol(typename Pol::NumberType(0)), Relation::LESS);
				break;
			}
			case Relation::NEQ: {
				constraint = BasicConstraint<Pol>(Pol(typename Pol::NumberType(0)), Relation::EQ);
				break;
			}
			case Relation::LEQ: {
				Pol newLhs = ((constraint.lhs() - constPart) * g);
				newLhs += carl::floor((constPart * g)) + typename Pol::NumberType(1);
				constraint = init_constraint(newLhs, Relation::LEQ);
				break;
			}
			case Relation::GEQ: {
				Pol newLhs = ((constraint.lhs() - constPart) * g);
				newLhs += carl::floor((constPart * g));
				constraint = init_constraint(newLhs, Relation::GEQ);
				break;
			}
			case Relation::LESS: {
				Pol newLhs = ((constraint.lhs() - constPart) * g);
				newLhs += carl::floor((constPart * g)) + typename Pol::NumberType(1);
				constraint = init_constraint(newLhs, Relation::LEQ);
				break;
			}
			case Relation::GREATER: {
				Pol newLhs = ((constraint.lhs() - constPart) * g);
				newLhs += carl::floor((constPart * g));
				constraint = init_constraint(newLhs, Relation::GEQ);
				break;
			}
			default:
				assert(false);
			}
			return true;
		}
	}
	return false;
}

template<typename Pol>
BasicConstraint<Pol> create_normalized_bound(Variable var, Relation rel, const typename Pol::NumberType& bound) {
    auto constraint = init_bound<Pol>(var,rel,bound);
    normalize_integer_inplace(constraint);
	auto lhs_definiteness = carl::definiteness(constraint.lhs(), FULL_EFFORT_FOR_DEFINITENESS_CHECK);
    simplify_nonlinear_univariate_monomial_inplace(constraint, lhs_definiteness);
    simplify_integer_inplace(constraint);
	return constraint;
}


template<typename Pol>
BasicConstraint<Pol> create_normalized_constraint(const Pol& lhs, Relation rel) {
    auto constraint = init_constraint(lhs,rel);
    normalize_integer_inplace(constraint);
	auto lhs_definiteness = carl::definiteness(constraint.lhs(), FULL_EFFORT_FOR_DEFINITENESS_CHECK);
    normalize_consistency_inplace(constraint, lhs_definiteness);
    if (!constraint.is_trivial_true() && !constraint.is_trivial_false()) {
        simplify_nonlinear_univariate_monomial_inplace(constraint, lhs_definiteness);
        simplify_integer_inplace(constraint);
    }
	return constraint;
}


}