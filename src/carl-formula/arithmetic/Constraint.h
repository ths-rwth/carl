#pragma once

#include <carl-common/memory/Pool.h>
#include <carl-common/config.h>
#include <carl-arith/core/Variables.h>
#include <carl-arith/poly/umvpoly/functions/VarInfo.h>
#include <carl-arith/poly/umvpoly/functions/Factorization.h>
#include <carl-arith/core/Common.h>
#include <carl-arith/constraint/Simplification.h>
#include <carl-arith/constraint/Comparison.h>
#include <carl-arith/constraint/Evaluation.h>
#include <carl-arith/constraint/Substitution.h>
#include <carl-arith/constraint/Bound.h>

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

template<typename Pol>
struct CachedConstraintContent {
	/// Basic constraint.
	BasicConstraint<Pol> m_constraint;
	/// Cache for the factorization.
	mutable Factors<Pol> m_lhs_factorization;
	/// A container which includes all variables occurring in the polynomial considered by this constraint.
	mutable carlVariables m_variables;
	/// A map which stores information about properties of the variables in this constraint.
	mutable VarsInfo<Pol> m_var_info_map;
	#ifdef THREAD_SAFE
	/// Mutex for access to variable information map.
	std::mutex m_var_info_map_mutex;
	/// Mutex for access to the factorization.
	std::mutex m_lhs_factorization_mutex;
	/// Mutex for access to the variables.
	std::mutex m_variables_mutex;
	#endif

	CachedConstraintContent(BasicConstraint<Pol>&& c) : m_constraint(std::move(c)) {}
	const auto& key() const { return m_constraint; }
};

template<typename Pol>
using ConstraintPool = pool::Pool<CachedConstraintContent<Pol>>;


/**
 * Represent a polynomial (in)equality against zero. Such an (in)equality
 * can be seen as an atomic formula/atom for the theory of real arithmetic.
 */
template<typename Pol>
class Constraint {

private:
	pool::PoolElement<CachedConstraintContent<Pol>> m_element;

public:
	explicit Constraint(bool valid = true) : m_element(BasicConstraint<Pol>(valid)) {}

	explicit Constraint(carl::Variable::Arg var, Relation rel, const typename Pol::NumberType& bound = constant_zero<typename Pol::NumberType>::get())  : m_element(constraint::create_normalized_bound<Pol>(var,rel,bound)) {}

	explicit Constraint(const Pol& lhs, Relation rel) : m_element(constraint::create_normalized_constraint(lhs,rel)) {}

	explicit Constraint(const BasicConstraint<Pol>& constraint) : m_element(constraint::create_normalized_constraint(constraint.lhs(),constraint.relation())) {}

	Constraint(const Constraint& constraint) : m_element(constraint.m_element) {}

	Constraint(Constraint&& constraint) noexcept : m_element(std::move(constraint.m_element)) {}

	Constraint& operator=(const Constraint& constraint) {
		m_element = constraint.m_element;
		return *this;
	}

	Constraint& operator=(Constraint&& constraint) noexcept {
		m_element = std::move(constraint.m_element);
		return *this;
	}

	operator const BasicConstraint<Pol>& () const {
		return m_element->m_constraint;
	}

	operator BasicConstraint<Pol> () const {
		return m_element->m_constraint;
	}

	/**
	 * Returns the associated BasicConstraint.
	 */
	const BasicConstraint<Pol>& constr() const {
		return m_element->m_constraint;
	}

	/**
     * @return The considered polynomial being the left-hand side of this constraint.
     *          Hence, the right-hand side of any constraint is always 0.
     */
	const Pol& lhs() const {
		return m_element->m_constraint.lhs();
	}

	/**
     * @return The relation symbol of this constraint.
     */
	Relation relation() const {
		return m_element->m_constraint.relation();
	}

	/**
     * @return A hash value for this constraint.
     */
	size_t hash() const {
		return m_element->m_constraint.hash();
	}

	/**
     * @return A container containing all variables occurring in the polynomial of this constraint.
     */
	const auto& variables() const {
		#ifdef THREAD_SAFE
		m_element->m_variables_mutex.lock();
		#endif
		if (m_element->m_variables.empty()) {
			m_element->m_variables = carl::variables(lhs());
		}
		#ifdef THREAD_SAFE
		m_element->m_variables_mutex.unlock();
		#endif
		return m_element->m_variables;
	}

	const Factors<Pol>& lhs_factorization() const {
		#ifdef THREAD_SAFE
		m_element->m_lhs_factorization_mutex.lock();
		#endif
		if (m_element->m_lhs_factorization.empty()) {
			m_element->m_lhs_factorization = carl::factorization(lhs());
		}
		#ifdef THREAD_SAFE
		m_element->m_lhs_factorization_mutex.unlock();
		#endif
		return m_element->m_lhs_factorization;
	}

	/**
     * @param variable The variable to find variable information for.
     * @tparam gatherCoeff
     * @return The whole variable information object.
     * Note, that if the given variable is not in this constraints, this method fails.
     * Furthermore, the variable information returned do provide coefficients only, if
     * the given flag gatherCoeff is set to true.
     */
	template<bool gatherCoeff = false>
	const VarInfo<Pol>& var_info(const Variable variable) const {
		#ifdef THREAD_SAFE
		m_element->m_var_info_map_mutex.lock();
		#endif
		if (!m_element->m_var_info_map.occurs(variable) || (gatherCoeff && !m_element->m_var_info_map.var(variable).has_coeff())) {
			m_element->m_var_info_map.data()[variable] = carl::var_info(lhs(),variable,gatherCoeff);
			assert(m_element->m_var_info_map.occurs(variable));
		}
		#ifdef THREAD_SAFE
		m_element->m_var_info_map_mutex.unlock();
		#endif
		return m_element->m_var_info_map.var(variable);
	}

	template<bool gatherCoeff = false>
	const VarsInfo<Pol>& var_info() const {
		for (const auto& var : variables()) {
			var_info<gatherCoeff>(var);
		}
		return m_element->m_var_info_map;
	}

	/**
     * Checks, whether the constraint is consistent.
     * It differs between, containing variables, consistent, and inconsistent.
     * @return 0, if the constraint is not consistent.
     *          1, if the constraint is consistent.
     *          2, if the constraint still contains variables.
     */
	unsigned is_consistent() const {
		return m_element->m_constraint.is_consistent();
	}

	Constraint negation() const {
		return Constraint(constr().negation());
	}

	/* (legacy) convenience methods */

	/**
     * @param _variable The variable for which to determine the maximal degree.
     * @return The maximal degree of the given variable in this constraint. (Monomial-wise)
     */
	uint maxDegree(const Variable& _variable) const {
		if (!variables().has(_variable)) return 0;
		else return var_info(_variable).max_degree();
	}

	/**
     * @return The maximal degree of all variables in this constraint. (Monomial-wise)
     */
	uint max_degree() const {
		uint result = 0;
		for (const auto& var : variables()) {
			uint deg = maxDegree(var);
			if (deg > result) result = deg;
		}
		return result;
	}

	/**
     * Calculates the coefficient of the given variable with the given degree. Note, that it only
     * computes the coefficient once and stores the result.
     * @param _var The variable for which to calculate the coefficient.
     * @param _degree The according degree of the variable for which to calculate the coefficient.
     * @return The ith coefficient of the given variable, where i is the given degree.
     */
	Pol coefficient(const Variable& _var, uint _degree) const {
		auto& vi = var_info<true>(_var);
		auto d = vi.coeffs().find(_degree);
		return d != vi.coeffs().end() ? d->second : Pol(typename Pol::NumberType(0));
	}

	/**
     * @return true, if it contains only integer valued variables.
     */
	bool integer_valued() const {
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
     * Determines whether the constraint is pseudo-boolean.
     *
     * @return True if this constraint is pseudo-boolean. False otherwise.
     */
	bool isPseudoBoolean() const {
		return !variables().boolean().empty();
	}
	
	template<typename P>
	friend bool operator==(const Constraint<P>& lhs, const Constraint<P>& rhs);
	template<typename P>
	friend bool operator<(const Constraint<P>& lhs, const Constraint<P>& rhs);
	template<typename P>
	friend std::ostream& operator<<(std::ostream& os, const Constraint<P>& c);
};

template<typename P>
bool operator==(const Constraint<P>& lhs, const Constraint<P>& rhs) {
	return lhs.m_element.id() == rhs.m_element.id();
}

template<typename P>
bool operator!=(const Constraint<P>& lhs, const Constraint<P>& rhs) {
	return !(lhs == rhs);
}

template<typename P>
bool operator<(const Constraint<P>& lhs, const Constraint<P>& rhs) {
	return lhs.m_element.id() < rhs.m_element.id();
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
	return os << c.m_element->m_constraint;
}

template<typename Pol>
void variables(const Constraint<Pol>& c, carlVariables& vars) {
	vars.add(c.variables().begin(), c.variables().end());
}

template<typename Pol>
std::optional<std::pair<Variable, Pol>> get_substitution(const Constraint<Pol>& c, bool _negated = false, Variable _exclude = carl::Variable::NO_VARIABLE) {
	return get_substitution(c.constr(), _negated, _exclude, std::optional(c.template var_info<true>()));
}

// implicit conversions do not work for template argument deduction
template<typename Pol>
auto get_assignment(const Constraint<Pol>& c) { return get_assignment(c.constr()); }
template<typename Pol>
auto compare(const Constraint<Pol>& c1, const Constraint<Pol>& c2) { return compare(c1.constr(), c2.constr()); }
template<typename Pol>
auto satisfied_by(const Constraint<Pol>& c, const Assignment<typename Pol::NumberType>& a) { return satisfied_by(c.constr(), a); }
template<typename Pol>
bool is_bound(const Constraint<Pol>& constr, bool negated = false) {
	if (negated) {
		return is_bound(constr.constr().negation());
	} else {
		return is_bound(constr.constr());
	}
}
template<typename Pol>
bool is_lower_bound(const Constraint<Pol>& constr) { return is_lower_bound(constr.constr()); }
template<typename Pol>
bool is_upper_bound(const Constraint<Pol>& constr) { return is_upper_bound(constr.constr()); }

} // namespace carl

namespace std {

/**
 * Implements std::hash for constraints.
 */
template<typename Pol>
struct hash<carl::Constraint<Pol>> {
	/**
     * @param constraint The constraint to get the hash for.
     * @return The hash of the given constraint.
     */
	std::size_t operator()(const carl::Constraint<Pol>& constraint) const {
		return constraint.hash();
	}
};

/**
 * Implements std::hash for vectors of constraints.
 */
template<typename Pol>
struct hash<std::vector<carl::Constraint<Pol>>> {
	/**
     * @param arg The vector of constraints to get the hash for.
     * @return The hash of the given vector of constraints.
     */
	std::size_t operator()(const std::vector<carl::Constraint<Pol>>& arg) const {
		std::size_t seed = arg.size();
		for(auto& c : arg) {
			seed ^= std::hash<carl::Constraint<Pol>>()(c) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		}
		return seed;
	}
};
} // namespace std
