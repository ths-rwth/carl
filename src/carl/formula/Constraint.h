/**
 * Constraint.h
 * @author Florian Corzilius<corzilius@cs.rwth-aachen.de>
 * @author Sebastian Junges
 * @author Ulrich Loup
 * @since 2010-04-26
 * @version 2014-10-30
 */

#pragma once

#include "../config.h"
#include "../core/Relation.h"
#include "../core/VariableInformation.h"
#include "../core/Variables.h"
#include "../core/VariablesInformation.h"
#include "../core/polynomialfunctions/Definiteness.h"
#include "../interval/Interval.h"
#include "../interval/IntervalEvaluation.h"
#include "../util/Common.h"
#include "config.h"
#include "ConstraintRaw.h"

#include <cassert>
#include <cstring>
#include <iostream>
#include <sstream>

namespace carl {
// Forward definition.
template<typename Pol>
class Constraint;

template<typename Poly>
using Constraints = std::set<Constraint<Poly>, carl::less<Constraint<Poly>, false>>;

#define CONSTRAINT_HASH(_lhs, _rel, _type) (size_t)((size_t)(std::hash<_type>()(_lhs) << 3) ^ (size_t)_rel)

/**
 * Represent a polynomial (in)equality against zero. Such an (in)equality
 * can be seen as an atomic formula/atom for the theory of real arithmetic.
 */
template<typename Pol>
class Constraint {

private:
	/// The polynomial which is compared by this constraint to zero.
	Pol mLhs;
	/// The relation symbol comparing the polynomial considered by this constraint to zero.
	Relation mRelation;

public:
	explicit Constraint(bool _valid = true);
	explicit Constraint(carl::Variable::Arg _var, Relation _rel, const typename Pol::NumberType& _bound = constant_zero<typename Pol::NumberType>::get());
	explicit Constraint(const Pol& _lhs, Relation _rel);
	Constraint(const Constraint& _constraint);
	Constraint(Constraint&& _constraint) noexcept;
	Constraint& operator=(const Constraint& _constraint);
	Constraint& operator=(Constraint&& _constraint) noexcept;

	/**
     * @return The considered polynomial being the left-hand side of this constraint.
     *          Hence, the right-hand side of any constraint is always 0.
     */
	const Pol& lhs() const {
		return mLhs;
	}

	/**
     * @return A container containing all variables occurring in the polynomial of this constraint.
     */
	const auto variables() const {
		return carl::variables(mLhs);
	}

	void gatherVariables(carlVariables& vars) const {
		carl::variables(mLhs, vars);
	}

	/**
     * @return The relation symbol of this constraint.
     */
	Relation relation() const {
		return mRelation;
	}

	/**
     * @return A hash value for this constraint.
     */
	size_t hash() const {
		return CONSTRAINT_HASH(mLhs, mRelation, Pol);
	}

	/**
     * @return The constant part of the polynomial compared by this constraint.
     */
	typename Pol::NumberType constantPart() const {
		return mLhs.constantPart();
	}

	/**
     * @param _variable The variable for which to determine the maximal degree.
     * @return The maximal degree of the given variable in this constraint. (Monomial-wise)
     */
	uint maxDegree(const Variable& _variable) const {
		if (!hasVariable(_variable)) return 0;
		else return varInfo(_variable).maxDegree();
	}

	/**
     * @return The maximal degree of all variables in this constraint. (Monomial-wise)
     */
	uint maxDegree() const {
		uint result = 0;
		for (const auto& var : variables()) {
			uint deg = maxDegree(var);
			if (deg > result) result = deg;
		}
		return result;
	}

	/**
     * @param _variable The variable to find variable information for.
     * @param gatherCoeff
     * @return The whole variable information object.
     * Note, that if the given variable is not in this constraints, this method fails.
     * Furthermore, the variable information returned do provide coefficients only, if
     * the given flag gatherCoeff is set to true.
     */
	template<bool gatherCoeff = false>
	const auto varInfo(const Variable _variable) const {
		return mLhs.template getVarInfo<gatherCoeff>(_variable);
	}

	/**
     * Checks if the given variable occurs in the constraint.
     * @param _var  The variable to check for.
     * @return true, if the given variable occurs in the constraint;
     *          false, otherwise.
     */
	bool hasVariable(const Variable _var) const {
		return mLhs.has(_var);
	}

	/**
     * @return true, if it contains only integer valued variables.
     */
	bool integerValued() const {
		return variables().filter(variable_type_filter::excluding({carl::VariableType::VT_INT})).size() == 0;
	}

	/**
     * @return true, if it contains only real valued variables.
     */
	bool realValued() const {
		return variables().filter(variable_type_filter::excluding({carl::VariableType::VT_REAL})).size() == 0;
	}

	/**
     * Checks if this constraints contains an integer valued variable.
     * @return true, if it does;
     *          false, otherwise.
     */
	bool hasIntegerValuedVariable() const {
		return !variables().integer().empty();
	}

	/**
     * Checks if this constraints contains an real valued variable.
     * @return true, if it does;
     *          false, otherwise.
     */
	bool hasRealValuedVariable() const {
		return !variables().real().empty();
	}

	/**
     * @return true, if this constraint is a bound.
     */
	bool isBound(bool negated = false) const {
		if (variables().size() != 1 || maxDegree(variables().as_vector()[0]) != 1) return false;
		if (negated) {
			return mRelation != Relation::EQ;
		} else {
			return mRelation != Relation::NEQ;
		}
	}

	/**
     * @return true, if this constraint is a lower bound.
     */
	bool isLowerBound() const {
		if (isBound()) {
			if (mRelation == Relation::EQ) return true;
			const typename Pol::NumberType& coeff = mLhs.lterm().coeff();
			if (coeff < 0)
				return (mRelation == Relation::LEQ || mRelation == Relation::LESS);
			else {
				assert(coeff > 0);
				return (mRelation == Relation::GEQ || mRelation == Relation::GREATER);
			}
		}
		return false;
	}

	/**
     * @return true, if this constraint is an upper bound.
     */
	bool isUpperBound() const {
		if (isBound()) {
			if (mRelation == Relation::EQ) return true;
			const typename Pol::NumberType& coeff = mLhs.lterm().coeff();
			if (coeff > 0)
				return (mRelation == Relation::LEQ || mRelation == Relation::LESS);
			else {
				assert(coeff < 0);
				return (mRelation == Relation::GEQ || mRelation == Relation::GREATER);
			}
		}
		return false;
	}

	/**
     * Checks, whether the constraint is consistent.
     * It differs between, containing variables, consistent, and inconsistent.
     * @return 0, if the constraint is not consistent.
     *          1, if the constraint is consistent.
     *          2, if the constraint still contains variables.
     */
	unsigned isConsistent() const {
		if (mLhs == RawConstraint<Pol>(false).mLhs && mRelation == RawConstraint<Pol>(false).mRelation) return 0;
		else if (mLhs == RawConstraint<Pol>(true).mLhs && mRelation == RawConstraint<Pol>(true).mRelation) return 1;
		else return 2;
	}

	/**
     * Checks whether this constraint is consistent with the given assignment from 
     * the its variables to interval domains.
     * @param _solutionInterval The interval domains of the variables.
     * @return 1, if this constraint is consistent with the given intervals;
     *          0, if this constraint is not consistent with the given intervals;
     *          2, if it cannot be decided whether this constraint is consistent with the given intervals.
     */
	unsigned consistentWith(const EvaluationMap<Interval<double>>& _solutionInterval) const;

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
	unsigned consistentWith(const EvaluationMap<Interval<double>>& _solutionInterval, Relation& _stricterRelation) const;

	/**
     * @param _var The variable to check the size of its solution set for.
     * @return true, if it is easy to decide whether this constraint has a finite solution set
     *                in the given variable;
     *          false, otherwise.
     */
	bool hasFinitelyManySolutionsIn(const Variable& _var) const;

	/**
     * Calculates the coefficient of the given variable with the given degree. Note, that it only
     * computes the coefficient once and stores the result.
     * @param _var The variable for which to calculate the coefficient.
     * @param _degree The according degree of the variable for which to calculate the coefficient.
     * @return The ith coefficient of the given variable, where i is the given degree.
     */
	Pol coefficient(const Variable& _var, uint _degree) const;

	Constraint negation() const {
		CARL_LOG_DEBUG("carl.formula.constraint", "negation of " << *this << " is " << Constraint(lhs(), carl::inverse(relation())));
		return Constraint(lhs(), carl::inverse(relation()));
	}

	/**
     * Determines whether the constraint is pseudo-boolean.
     *
     * @return True if this constraint is pseudo-boolean. False otherwise.
     */
	bool isPseudoBoolean() const;
};

template<typename P>
bool operator==(const Constraint<P>& lhs, const Constraint<P>& rhs) {
	return lhs.relation() == rhs.relation() && lhs.hash() == rhs.hash() && lhs.lhs() == rhs.lhs();
}

template<typename P>
bool operator!=(const Constraint<P>& lhs, const Constraint<P>& rhs) {
	return lhs.relation() != rhs.relation() || lhs.hash() != rhs.hash() || lhs.lhs() != rhs.lhs();
}

template<typename P>
bool operator<(const Constraint<P>& lhs, const Constraint<P>& rhs) {
	return lhs.relation() < rhs.relation() || (lhs.relation() == rhs.relation() && (lhs.hash() < rhs.hash() || (lhs.hash() == rhs.hash() && lhs.lhs() < rhs.lhs()))); 
	// return lhs.lhs() < rhs.lhs() || (lhs.lhs() == rhs.lhs() && lhs.relation() < rhs.relation());
}

template<typename P>
bool operator<=(const Constraint<P>& lhs, const Constraint<P>& rhs) {
	return lhs == rhs || lhs < rhs;
}

template<typename P>
bool operator>(const Constraint<P>& lhs, const Constraint<P>& rhs) {
	return rhs < lhs;
}

template<typename P>
bool operator>=(const Constraint<P>& lhs, const Constraint<P>& rhs) {
	return rhs <= lhs;
}


/**
 * Prints the given constraint on the given stream.
 * @param os The stream to print the given constraint on.
 * @param c The formula to print.
 * @return The stream after printing the given constraint on it.
 */
template<typename Poly>
std::ostream& operator<<(std::ostream& os, const Constraint<Poly>& c) {
	return os << c.lhs() << " " << c.relation() << " 0";
}

const signed A_IFF_B = 2;
const signed A_IMPLIES_B = 1;
const signed B_IMPLIES_A = -1;
const signed NOT__A_AND_B = -2;
const signed A_AND_B__IFF_C = -3;
const signed A_XOR_B = -4;

/**
 * Compares _constraintA with _constraintB.
 * @return  2, if it is easy to decide that _constraintA and _constraintB have the same solutions. _constraintA = _constraintB
 *           1, if it is easy to decide that _constraintB includes all solutions of _constraintA;   _constraintA -> _constraintB
 *          -1, if it is easy to decide that _constraintA includes all solutions of _constraintB;   _constraintB -> _constraintA
 *          -2, if it is easy to decide that _constraintA has no solution common with _constraintB; not(_constraintA and _constraintB)
 *          -3, if it is easy to decide that _constraintA and _constraintB can be intersected;      _constraintA and _constraintB = _constraintC
 *          -4, if it is easy to decide that _constraintA is the inverse of _constraintB;           _constraintA xor _constraintB
 *           0, otherwise.
 */
template<typename Pol>
signed compare(const Constraint<Pol>& _constraintA, const Constraint<Pol>& _constraintB);

/**
 * Checks whether the given assignment satisfies this constraint.
 * @param _assignment The assignment.
 * @return 1, if the given assignment satisfies this constraint.
 *          0, if the given assignment contradicts this constraint.
 *          2, otherwise (possibly not defined for all variables in the constraint,
 *                       even then it could be possible to obtain the first two results.)
 */
template<typename Pol>
unsigned satisfiedBy(const Constraint<Pol>&, const EvaluationMap<typename Pol::NumberType>& _assignment);

} // namespace carl

namespace std {

/**
 * Implements std::hash for constraints.
 */
template<typename Pol>
struct hash<carl::Constraint<Pol>> {
	/**
     * @param _constraint The constraint to get the hash for.
     * @return The hash of the given constraint.
     */
	std::size_t operator()(const carl::Constraint<Pol>& _constraint) const {
		return _constraint.hash();
	}
};

/**
 * Implements std::hash for vectors of constraints.
 */
template<typename Pol>
struct hash<std::vector<carl::Constraint<Pol>>> {
	/**
     * @param _arg The vector of constraints to get the hash for.
     * @return The hash of the given vector of constraints.
     */
	std::size_t operator()(const std::vector<carl::Constraint<Pol>>& _arg) const {
		std::size_t seed = _arg.size();
		for(auto& c : _arg) {
			seed ^= std::hash<carl::Constraint<Pol>>()(c) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		}
		return seed;
	}
};
} // namespace std

#include "Constraint.tpp"
