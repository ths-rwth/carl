#pragma once

#include <carl-arith/core/Relation.h>
#include <carl-arith/core/Variables.h>

namespace carl {

#define CONSTRAINT_HASH(_lhs, _rel, _type) (size_t)((size_t)(std::hash<_type>()(_lhs) << 3) ^ (size_t)_rel)

/**
 * Represent a polynomial (in)equality against zero. Such an (in)equality
 * can be seen as an atomic formula/atom for the theory of real arithmetic.
 */
template<typename Pol>
class BasicConstraint {
	/// The polynomial which is compared by this constraint to zero.
	Pol m_lhs;
	/// The relation symbol comparing the polynomial considered by this constraint to zero.
	Relation m_relation;
	/// Cache for the hash.
	std::size_t m_hash;

public:
	BasicConstraint(bool is_true) : m_lhs(typename Pol::NumberType(!is_true)), m_relation(Relation::EQ), m_hash(CONSTRAINT_HASH(m_lhs, m_relation, Pol)) {}

	BasicConstraint(const Pol& lhs, const Relation rel) : m_lhs(lhs), m_relation(rel), m_hash(CONSTRAINT_HASH(m_lhs, m_relation, Pol)) {}

	BasicConstraint(Pol&& lhs, const Relation rel) : m_lhs(std::move(lhs)), m_relation(rel), m_hash(CONSTRAINT_HASH(m_lhs, m_relation, Pol)) {}

	/**
     * @return The considered polynomial being the left-hand side of this constraint. Hence, the right-hand side of any constraint is always 0.
     */
	const Pol& lhs() const {
		return m_lhs;
	}

	/**
     * @return The considered polynomial being the left-hand side of this constraint. Hence, the right-hand side of any constraint is always 0.
     */
	void set_lhs(Pol&& lhs) {
		m_lhs = std::move(lhs);
		m_hash = CONSTRAINT_HASH(m_lhs, m_relation, Pol);
	}

	/**
     * @return The relation symbol of this constraint.
     */
	Relation relation() const {
		return m_relation;
	}

	/**
     * @return The relation symbol of this constraint.
     */
	void set_relation(Relation rel) {
		m_relation = rel;
		m_hash = CONSTRAINT_HASH(m_lhs, m_relation, Pol);
	}

	/**
     * @return A hash value for this constraint.
     */
	size_t hash() const {
		return m_hash;
	}

	bool is_trivial_true() const  {
		if(is_constant(m_lhs)) {
			if(m_lhs.constant_part() == 0) {
				return is_weak(m_relation);
			} else if(m_lhs.constant_part() > 0) {
				return m_relation == Relation::GEQ || m_relation == Relation::GREATER || m_relation == Relation::NEQ;
			} else {
				assert(m_lhs.constant_part() < 0);
				return m_relation == Relation::LEQ || m_relation == Relation::LESS || m_relation == Relation::NEQ;
			}
		}
		return false;
	}
	
	bool is_trivial_false() const {
		if(is_constant(m_lhs)) {
			if(m_lhs.constant_part() == 0) {
				return is_strict(m_relation);
			} else if(m_lhs.constant_part() > 0) {
				return m_relation == Relation::LEQ || m_relation == Relation::LESS || m_relation == Relation::EQ;
			} else {
				assert(m_lhs.constant_part() < 0);
				return m_relation == Relation::GEQ || m_relation == Relation::GREATER || m_relation == Relation::EQ;
			}
		}
		return false;
	}

	unsigned is_consistent() const {
		if (is_trivial_false()) return 0;
		else if (is_trivial_true()) return 1;
		else return 2;
	}

	BasicConstraint<Pol> negation() const {
		return BasicConstraint<Pol>(lhs(), carl::inverse(relation()));
	}
};

template<typename P>
bool operator==(const BasicConstraint<P>& lhs, const BasicConstraint<P>& rhs) {
	return lhs.hash() == rhs.hash() && lhs.relation() == rhs.relation() && lhs.lhs() == rhs.lhs();
}

template<typename P>
bool operator!=(const BasicConstraint<P>& lhs, const BasicConstraint<P>& rhs) {
	return !(lhs == rhs);
}

template<typename P>
bool operator<(const BasicConstraint<P>& lhs, const BasicConstraint<P>& rhs) {
	return lhs.hash() < rhs.hash() || (lhs.hash() == rhs.hash() && (lhs.relation() < rhs.relation() || (lhs.relation() == rhs.relation() && lhs.lhs() < rhs.lhs()))); 
	// return lhs.lhs() < rhs.lhs() || (lhs.lhs() == rhs.lhs() && lhs.relation() < rhs.relation());
}

template<typename P>
bool operator<=(const BasicConstraint<P>& lhs, const BasicConstraint<P>& rhs) {
	return lhs == rhs || lhs < rhs;
}

template<typename P>
bool operator>(const BasicConstraint<P>& lhs, const BasicConstraint<P>& rhs) {
	return rhs < lhs;
}

template<typename P>
bool operator>=(const BasicConstraint<P>& lhs, const BasicConstraint<P>& rhs) {
	return rhs <= lhs;
}


template<typename Pol>
void variables(const BasicConstraint<Pol>& c, carlVariables& vars) {
	variables(c.lhs(), vars);
}

/**
 * Prints the given constraint on the given stream.
 * @param os The stream to print the given constraint on.
 * @param c The formula to print.
 * @return The stream after printing the given constraint on it.
 */
template<typename Poly>
std::ostream& operator<<(std::ostream& os, const BasicConstraint<Poly>& c) {
	return os << c.lhs() << " " << c.relation() << " 0";
}

} // namespace carl

namespace std {
/**
 * Implements std::hash for constraints.
 */
template<typename Pol>
struct hash<carl::BasicConstraint<Pol>> {
	/**
     * @param constraint The constraint to get the hash for.
     * @return The hash of the given constraint.
     */
	std::size_t operator()(const carl::BasicConstraint<Pol>& constraint) const {
		return constraint.hash();
	}
};

/**
 * Implements std::hash for vectors of constraints.
 */
template<typename Pol>
struct hash<std::vector<carl::BasicConstraint<Pol>>> {
	/**
     * @param _arg The vector of constraints to get the hash for.
     * @return The hash of the given vector of constraints.
     */
	std::size_t operator()(const std::vector<carl::BasicConstraint<Pol>>& arg) const {
		std::size_t seed = arg.size();
		for(auto& c : arg) {
			seed ^= std::hash<carl::BasicConstraint<Pol>>()(c) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		}
		return seed;
	}
};

} // namespace std