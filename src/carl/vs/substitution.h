#pragma once

#include "SqrtEx.h"

namespace carl::vs {

enum class substitution_type {
	NORMAL,
	PLUS_EPSILON,
	MINUS_INFINITY,
	PLUS_INFINITY
};

template<class Poly>
class substitution_term {
private:                    
	/// The substitution type.
	substitution_type m_type;
	/// A square root expression.
	std::optional<SqrtEx<Poly>> m_sqrt_ex;

public:
	substitution_term(substitution_type type, std::optional<SqrtEx<Poly>> sqrt_ex)
		: m_type(type), m_sqrt_ex(sqrt_ex) {}

	static substitution_term normal(const SqrtEx<Poly>& sqrt_ex) {
		return substitution_term(substitution_type::NORMAL, sqrt_ex);
	}

	static substitution_term plus_eps(const SqrtEx<Poly>& sqrt_ex) {
		return substitution_term(substitution_type::PLUS_EPSILON, sqrt_ex);
	}

	static substitution_term minus_infty() {
		return substitution_term(substitution_type::MINUS_INFINITY, std::nullopt);
	}

	static substitution_term plus_infty() {
		return substitution_term(substitution_type::PLUS_INFINITY, std::nullopt);
	}

	bool is_normal() const {
		return m_type == substitution_type::NORMAL;
	}

	bool is_plus_eps() const {
		return m_type == substitution_type::PLUS_EPSILON;
	}

	bool is_minus_infty() const {
		return m_type == substitution_type::MINUS_INFINITY;
	}

	bool is_plus_infty() const {
		return m_type == substitution_type::PLUS_INFINITY;
	}

	const SqrtEx<Poly> sqrt_ex() const {
		return *m_sqrt_ex;
	}

    substitution_type type() const {
        return m_type;
    } 

    bool operator==(const substitution_term&) const;
};

template<class Poly>
std::ostream& operator<<(std::ostream& os, const substitution_term<Poly>& s);

template<class Poly>
class substitution {
private:
	/// The variable to substitute.
	Variable m_variable;
	/// The substitution term
	substitution_term<Poly> m_term;
	/// The variables occurring in the term to substitute for.
	mutable std::optional<carlVariables> m_term_variables;
	/// The side conditions, which have to hold to make this substitution valid. (e.g. [x -> 1/a] has the side condition {a!=0})
	carl::Constraints<Poly> m_side_condition;

public:
	/**
     * Constructs a substitution with a square root term to map to.
     * @param _variable The variable to substitute of the substitution to construct.
     * @param _term The square root term to which the variable maps to.
     * @param _oConditions The original conditions of the substitution to construct.
     * @param _sideCondition The side conditions of the substitution to construct.
     */
	substitution(const carl::Variable& _variable, const substitution_term<Poly>& _term, Constraints<Poly>&& _sideCondition = Constraints<Poly>());

	/**
     * @return The variable this substitution substitutes.
     */
	const carl::Variable& variable() const {
		return m_variable;
	}

	/**
     * @return The term this substitution maps its variable to.
     */
	const substitution_term<Poly>& term() const {
		return m_term;
	}

	/**
     * @return The side condition of this substitution.
     */
	const Constraints<Poly>& side_condition() const {
		return m_side_condition;
	}

	/**
     * @return The variables occurring in the substitution term.
     */
	const carl::carlVariables& term_variables() const {
		if (!m_term_variables) {
			m_term_variables = carl::variables(m_term);
		}
		return *m_term_variables;
	}

	/**
     * @param The substitution to compare with.
     * @return true, if this substitution is equal to the given substitution;
     *          false, otherwise.
     */
	bool operator==(const substitution&) const;
};
/**
 * Prints the given substitution on the given output stream.
 * @param _out The output stream, on which to write.
 * @param _substitution  The substitution to print.
 * @return The output stream after printing the substitution on it.
 */
template<class Poly>
std::ostream& operator<<(std::ostream& os, const substitution<Poly>& s);

} // namespace carl::vs

namespace std {
template<class Poly>
struct hash<carl::vs::substitution<Poly>> {
public:
	size_t operator()(const carl::vs::substitution<Poly>& _substitution) const {
		return ((hash<carl::SqrtEx<Poly>>()(_substitution.term().sqrt_ex()) << 7) ^ hash<carl::Variable>()(_substitution.variable()) << 2) ^ _substitution.term().type();
	}
};
} // namespace std

#include "substitution.tpp"