#pragma once

namespace carl {

/**
 * "Raw" constraint used internally to normalize and simplify constraints. 
 */
template<typename Pol>
struct RawConstraint {
	using PolyT = typename Pol::PolyType;

	/// The relation symbol comparing the polynomial considered by this constraint to zero.
	Relation mRelation = Relation::EQ;
	/// The polynomial which is compared by this constraint to zero.
	Pol mLhs;
	/// A container which includes all variables occurring in the polynomial considered by this constraint.
	carlVariables mVariables;
	/// Definiteness of the polynomial in this constraint.
	Definiteness mLhsDefiniteness = Definiteness::NON;

	unsigned is_consistent() const;

	bool is_integer() const {
		return !mVariables.integer().empty() && mVariables.real().empty();
	}

	RawConstraint(bool _true) {
		set_trivial(_true);
	}

	RawConstraint(Variable::Arg _var, const Relation _rel, const typename Pol::NumberType& _bound) {
		set_bound(_var, _rel, _bound);
		initialize();
		simplify();
	}

	RawConstraint(const Pol& _lhs, const Relation _rel) {
		set_constraint(_lhs, _rel);
		initialize();
		normalize_consistency();
		if (is_consistent() == 2) {
			simplify();
		}
	}

	void simplify() {
		if (simplify_nonlinear_univariate_monomial()) initialize();
		if (simplify_integer()) initialize();
	}


private:
	
	void set_trivial(bool _valid);
	void set_bound(Variable::Arg _var, Relation _rel, const typename Pol::NumberType& _bound);
	void set_constraint(const Pol& _lhs, Relation _rel);

	void initialize() {
		assert(mRelation != Relation::GREATER && mRelation != Relation::GEQ);
		CARL_LOG_DEBUG("carl.formula.constraint", "Initializing " << mLhs << " " << mRelation << " 0");
		mVariables.clear();
		carl::variables(mLhs, mVariables);
		CARL_LOG_DEBUG("carl.formula.constraint", "Variables: " << mVariables);
		normalize_integer();
		mLhsDefiniteness = carl::definiteness(mLhs, FULL_EFFORT_FOR_DEFINITENESS_CHECK);
		CARL_LOG_DEBUG("carl.formula.constraint", "Definiteness: " << mLhsDefiniteness);
	}

	void normalize_integer();
	void normalize_consistency();

	bool simplify_nonlinear_univariate_monomial();
	bool simplify_integer();
	
};

template<typename Pol>
inline void RawConstraint<Pol>::set_trivial(bool _valid) {
	CARL_LOG_DEBUG("carl.formula.constraint", "Create trivial constraint " << _valid);
	if (_valid) {
		mLhs = Pol(typename Pol::NumberType(0));
		mRelation = Relation::EQ;
	} else {
		mLhs = Pol(typename Pol::NumberType(0));
		mRelation = Relation::LESS;
	}
	mLhsDefiniteness = Definiteness::NON;
	mVariables.clear();
}

template<typename Pol>
inline void RawConstraint<Pol>::set_bound(Variable::Arg _var, Relation _rel, const typename Pol::NumberType& _bound) {
	CARL_LOG_DEBUG("carl.formula.constraint", "Normalizing bound " << _var << " " << _rel << " " << _bound);
	mLhs = Pol(_var);
	mRelation = _rel;
	switch (_rel) {
	case Relation::GREATER:
		mLhs = -mLhs;
		if (_var.type() == VariableType::VT_INT) {
			if (isInteger(_bound))
				mLhs += _bound + typename Pol::NumberType(1);
			else
				mLhs += carl::ceil(_bound);
			mRelation = Relation::LEQ;
		} else {
			mLhs += _bound;
			mRelation = Relation::LESS;
		}
		break;
	case Relation::GEQ:
		mLhs = -mLhs;
		if (_var.type() == VariableType::VT_INT) {
			if (isInteger(_bound))
				mLhs += _bound;
			else
				mLhs += carl::ceil(_bound);
			mRelation = Relation::LEQ;
		} else {
			mLhs += _bound;
			mRelation = Relation::LEQ;
		}
		break;
	case Relation::LESS:
		if (_var.type() == VariableType::VT_INT) {
			if (isInteger(_bound))
				mLhs -= (_bound - typename Pol::NumberType(1));
			else
				mLhs -= carl::floor(_bound);
			mRelation = Relation::LEQ;
		} else {
			mLhs -= _bound;
		}
		break;
	case Relation::LEQ:
		if (_var.type() == VariableType::VT_INT) {
			if (isInteger(_bound))
				mLhs -= _bound;
			else
				mLhs -= carl::floor(_bound);
		} else {
			mLhs -= _bound;
		}
		break;
	case Relation::EQ:
		if (_var.type() == VariableType::VT_INT) {
			if (isInteger(_bound)) {
				mLhs -= _bound;
			} else {
				mLhs = Pol(typename Pol::NumberType(0));
				mRelation = Relation::LESS;
			}
		} else {
			mLhs -= _bound;
		}
		break;
	case Relation::NEQ:
		if (_var.type() == VariableType::VT_INT) {
			if (isInteger(_bound)) {
				mLhs -= _bound;
			} else {
				mLhs = Pol(typename Pol::NumberType(0));
				mRelation = Relation::EQ;
			}
		} else {
			mLhs -= _bound;
		}
		break;
	}
}

template<typename Pol>
inline void RawConstraint<Pol>::set_constraint(const Pol& _lhs, Relation _rel) {
	if (_lhs.isConstant()) {
		CARL_LOG_DEBUG("carl.formula.constraint", _lhs << " is constant, we simply evaluate.");
		evaluate(_lhs.constantPart(), _rel) ? set_trivial(true) : set_trivial(false);
	} else if (_lhs.totalDegree() == 1 && (_rel != Relation::EQ && _rel != Relation::NEQ) && _lhs.isUnivariate()) {
		if (carl::isNegative(_lhs.lcoeff())) {
			CARL_LOG_DEBUG("carl.formula.constraint", "Normalizing leading coefficient of linear poly.");
			switch (_rel) {
			case Relation::LESS:
				_rel = Relation::GREATER;
				break;
			case Relation::GREATER:
				_rel = Relation::LESS;
				break;
			case Relation::LEQ:
				_rel = Relation::GEQ;
				break;
			case Relation::GEQ:
				_rel = Relation::LEQ;
				break;
			default:
				assert(false);
				break;
			}
		}
		CARL_LOG_DEBUG("carl.formula.constraint", "Rewriting to bound");
		set_bound(_lhs.getSingleVariable(), _rel, (-_lhs.constantPart()) / _lhs.lcoeff());
	} else {
		CARL_LOG_DEBUG("carl.formula.constraint", "Normalizing " << _lhs << " " << _rel << " 0");
		mLhs = isZero(_lhs) ? Pol(typename Pol::NumberType(0)) : _lhs.coprimeCoefficients();
		if (_rel == Relation::EQ || _rel == Relation::NEQ) {
			if (!isZero(_lhs) && mLhs.lterm().coeff() < typename Pol::NumberType(0)) mLhs = -mLhs;
			mRelation = _rel;
		} else if (_rel == Relation::LEQ || _rel == Relation::LESS) {
			if (!isZero(mLhs) && (_lhs.lterm().coeff() < 0) != (mLhs.lterm().coeff() < 0)) mLhs = -mLhs;
			mRelation = _rel;
		} else if (_rel == Relation::GREATER) {
			if (!isZero(mLhs) && (_lhs.lterm().coeff() < 0) == (mLhs.lterm().coeff() < 0)) mLhs = -mLhs;
			mRelation = Relation::LESS;
		} else if (_rel == Relation::GEQ) {
			if (!isZero(mLhs) && (_lhs.lterm().coeff() < 0) == (mLhs.lterm().coeff() < 0)) mLhs = -mLhs;
			mRelation = Relation::LEQ;
		}
		CARL_LOG_DEBUG("carl.formula.constraint", "-> " << mLhs << " " << mRelation << " 0");
	}
}

template<typename Pol>
inline void RawConstraint<Pol>::normalize_integer() {
	if (is_integer()) {
		if (mRelation == Relation::LESS) {
			mLhs += carl::constant_one<typename Pol::CoeffType>::get();
			mRelation = Relation::LEQ;
		} else if (mRelation == Relation::GREATER) {
			mLhs -= carl::constant_one<typename Pol::CoeffType>::get();
			mRelation = Relation::GEQ;
		}
	}
}

template<typename Pol>
inline void RawConstraint<Pol>::normalize_consistency() {
	unsigned consistency = is_consistent();
	if (consistency == 0) {
		set_trivial(false);
	} else if (consistency == 1) {
		set_trivial(true);
	}
}

template<typename Pol>
inline unsigned RawConstraint<Pol>::is_consistent() const {
	if (mLhs.isConstant()) {
		CARL_LOG_DEBUG("carl.formula.constraint", "Lhs " << mLhs << " is constant");
		return carl::evaluate(mLhs.constantPart(), mRelation) ? 1 : 0;
	} else {
		CARL_LOG_DEBUG("carl.formula.constraint", "Checking " << mRelation << " against " << mLhsDefiniteness);
		switch (mRelation) {
		case Relation::EQ: {
			if (mLhsDefiniteness == Definiteness::POSITIVE || mLhsDefiniteness == Definiteness::NEGATIVE) return 0;
			break;
		}
		case Relation::NEQ: {
			if (mLhsDefiniteness == Definiteness::POSITIVE || mLhsDefiniteness == Definiteness::NEGATIVE) return 1;
			break;
		}
		case Relation::LESS: {
			if (mLhsDefiniteness == Definiteness::NEGATIVE) return 1;
			if (mLhsDefiniteness >= Definiteness::POSITIVE_SEMI) return 0;
			break;
		}
		case Relation::GREATER: {
			if (mLhsDefiniteness == Definiteness::POSITIVE) return 1;
			if (mLhsDefiniteness <= Definiteness::NEGATIVE_SEMI) return 0;
			break;
		}
		case Relation::LEQ: {
			if (mLhsDefiniteness <= Definiteness::NEGATIVE_SEMI) return 1;
			if (mLhsDefiniteness == Definiteness::POSITIVE) return 0;
			break;
		}
		case Relation::GEQ: {
			if (mLhsDefiniteness >= Definiteness::POSITIVE_SEMI) return 1;
			if (mLhsDefiniteness == Definiteness::NEGATIVE) return 0;
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
inline bool RawConstraint<Pol>::simplify_nonlinear_univariate_monomial() {
	if (!(mVariables.size() == 1 && !mLhs.isLinear() && mLhs.nrTerms() == 1)) return false;

	// Left-hand side is a non-linear univariate monomial
	Relation rel = mRelation;
	if ((mLhsDefiniteness == Definiteness::POSITIVE_SEMI && rel == Relation::LEQ) || (mLhsDefiniteness == Definiteness::NEGATIVE_SEMI && rel == Relation::GEQ))
		rel = Relation::EQ;
	Variable var = mVariables.as_vector()[0];

	switch (rel) {
	case Relation::EQ:
		set_constraint(PolyT(var), rel);
		break;
	case Relation::NEQ:
		set_constraint(PolyT(var), rel);
		break;
	case Relation::LEQ:
		if (mLhsDefiniteness == Definiteness::NEGATIVE_SEMI) {
			set_constraint(PolyT(typename Pol::NumberType(-1)) * PolyT(var) * PolyT(var), rel);
		} else {
			set_constraint((mLhs.trailingTerm().coeff() > 0 ? PolyT(typename Pol::NumberType(1)) : PolyT(typename Pol::NumberType(-1))) * PolyT(var), rel);
		}
		break;
	case Relation::GEQ:
		if (mLhsDefiniteness == Definiteness::POSITIVE_SEMI) {
			set_constraint(PolyT(var) * PolyT(var), rel);
		} else {
			set_constraint((mLhs.trailingTerm().coeff() > 0 ? PolyT(typename Pol::NumberType(1)) : PolyT(typename Pol::NumberType(-1))) * PolyT(var), rel);
		}
		break;
	case Relation::LESS:
		if (mLhsDefiniteness == Definiteness::NEGATIVE_SEMI) {
			set_constraint(PolyT(var), Relation::NEQ);
		} else {
			if (mLhsDefiniteness == Definiteness::POSITIVE_SEMI) {
				set_constraint(PolyT(var) * PolyT(var), rel);
			} else {
				set_constraint((mLhs.trailingTerm().coeff() > 0 ? PolyT(typename Pol::NumberType(1)) : PolyT(typename Pol::NumberType(-1))) * PolyT(var), rel);
			}
		}
		break;
	case Relation::GREATER:
		if (mLhsDefiniteness == Definiteness::POSITIVE_SEMI) {
			set_constraint(PolyT(var), Relation::NEQ);
		} else {
			if (mLhsDefiniteness == Definiteness::NEGATIVE_SEMI) {
				set_constraint(PolyT(typename Pol::NumberType(-1)) * PolyT(var) * PolyT(var), rel);
			} else {
				set_constraint((mLhs.trailingTerm().coeff() > 0 ? PolyT(typename Pol::NumberType(1)) : PolyT(typename Pol::NumberType(-1))) * PolyT(var), rel);
			}
		}
		break;
	default:
		assert(false);
	}

	return true;
}

template<typename Pol>
inline bool RawConstraint<Pol>::simplify_integer() {
	if (!is_integer()) return false;

	typename Pol::NumberType constPart = mLhs.constantPart();
	if (constPart != typename Pol::NumberType(0)) {
		// Find the gcd of the coefficients of the non-constant terms.
		typename Pol::NumberType g = carl::abs(mLhs.coprimeFactorWithoutConstant());
		assert(g != typename Pol::NumberType(0));
		if (carl::mod(carl::getNum(constPart), carl::getDenom(g)) != 0) {
			switch (mRelation) {
			case Relation::EQ: {
				set_constraint(Pol(typename Pol::NumberType(0)), Relation::LESS);
				break;
			}
			case Relation::NEQ: {
				set_constraint(Pol(typename Pol::NumberType(0)), Relation::EQ);
				break;
			}
			case Relation::LEQ: {
				Pol newLhs = ((mLhs - constPart) * g);
				newLhs += carl::floor((constPart * g)) + typename Pol::NumberType(1);
				set_constraint(newLhs, Relation::LEQ);
				break;
			}
			case Relation::GEQ: {
				Pol newLhs = ((mLhs - constPart) * g);
				newLhs += carl::floor((constPart * g));
				set_constraint(newLhs, Relation::GEQ);
				break;
			}
			case Relation::LESS: {
				Pol newLhs = ((mLhs - constPart) * g);
				newLhs += carl::floor((constPart * g)) + typename Pol::NumberType(1);
				set_constraint(newLhs, Relation::LEQ);
				break;
			}
			case Relation::GREATER: {
				Pol newLhs = ((mLhs - constPart) * g);
				newLhs += carl::floor((constPart * g));
				set_constraint(newLhs, Relation::GEQ);
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

} // namespace carl