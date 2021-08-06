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
#include "../core/polynomialfunctions/Complexity.h"
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
	const auto varInfo(const Variable& _variable) const {
		return mLhs.template getVarInfo<gatherCoeff>(_variable);
	}

	bool relationIsStrict() const {
		return isStrict(mRelation);
	}
	bool relationIsWeak() const {
		return isWeak(mRelation);
	}

	/**
     * Checks if the given variable occurs in the constraint.
     * @param _var  The variable to check for.
     * @return true, if the given variable occurs in the constraint;
     *          false, otherwise.
     */
	bool hasVariable(const Variable& _var) const {
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
     * @return An approximation of the complexity of this constraint.
     */
	size_t complexity() const {
		return 1 + carl::complexity(mLhs);
	}

	/**
     * Checks whether the given assignment satisfies this constraint.
     * @param _assignment The assignment.
     * @return 1, if the given assignment satisfies this constraint.
     *          0, if the given assignment contradicts this constraint.
     *          2, otherwise (possibly not defined for all variables in the constraint,
     *                       even then it could be possible to obtain the first two results.)
     */
	unsigned satisfiedBy(const EvaluationMap<typename Pol::NumberType>& _assignment) const;

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
     * Checks whether the given interval assignment may fulfill the constraint.
     * Note that the assignment must be complete.
     * There are three possible outcomes:
     * - True (4), i.e. all actual assignments satisfy the constraint: $p ~_\alpha 0 \Leftrightarrow True$
     * - Maybe (3), i.e. some actual assignments satisfy the constraint: $p ~_\alpha 0 \Leftrightarrow ?$
     * - Not null (2), i.e. all assignments that make the constraint evaluate not to zero satisfy the constraint: $p ~_\alpha 0 \Leftrightarrow p \neq 0$
     * - Null (1), i.e. only assignments that make the constraint evaluate to zero satisfy the constraint: $p ~_\alpha 0 \Leftrightarrow p_\alpha = 0$
     * - False (0), i.e. no actual assignment satisfies the constraint: $p ~_\alpha 0 \Leftrightarrow False$
     * @param _assignment Variable assignment.
     * @return 0, 1 or 2.
     */
	unsigned evaluate(const EvaluationMap<Interval<typename carl::UnderlyingNumberType<Pol>::type>>& _assignment) const;

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
     * If this constraint represents a substitution (equation, where at least one variable occurs only linearly),
     * this method detects a (there could be various possibilities) corresponding substitution variable and term.
     * @param _substitutionVariable Is set to the substitution variable, if this constraint represents a substitution.
     * @param _substitutionTerm Is set to the substitution term, if this constraint represents a substitution.
     * @return true, if this constraints represents a substitution;
     *         false, otherwise.
     */
	bool getSubstitution(Variable& _substitutionVariable, Pol& _substitutionTerm, bool _negated = false, const Variable& _exclude = carl::Variable::NO_VARIABLE) const;

	bool getAssignment(Variable& _substitutionVariable, typename Pol::NumberType& _substitutionValue) const;

	/**
     * Determines whether the constraint is pseudo-boolean.
     *
     * @return True if this constraint is pseudo-boolean. False otherwise.
     */
	bool isPseudoBoolean() const;
};

template<typename P>
bool operator==(const Constraint<P>& lhs, const Constraint<P>& rhs) {
	return lhs.lhs() == rhs.lhs() && lhs.relation() == rhs.relation();
}

template<typename P>
bool operator!=(const Constraint<P>& lhs, const Constraint<P>& rhs) {
	return lhs.lhs() != rhs.lhs() || lhs.relation() != rhs.relation();
}

template<typename P>
bool operator<(const Constraint<P>& lhs, const Constraint<P>& rhs) {
	return lhs.lhs() < rhs.lhs() || (lhs.lhs() == rhs.lhs() && lhs.relation() < rhs.relation());
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
