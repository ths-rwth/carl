/**
 * @file BVTerm.h
 * @author Andreas Krueger <andreas.krueger@rwth-aachen.de>
 */

#pragma once

#include <carl-arith/core/Variables.h>
#include <carl-common/util/variant_util.h>
#include <boost/variant.hpp>

#include "BVTermType.h"
#include "BVValue.h"
#include "BVVariable.h"

namespace carl {
// forward declaration
struct BVTermContent;

// Forward declaration
class BVTermPool;

class BVTerm {
	friend BVTermPool;
	friend std::ostream& operator<<(std::ostream& os, const BVTerm& term);
	friend bool operator==(const BVTerm& lhs, const BVTerm& rhs);
	friend bool operator<(const BVTerm& lhs, const BVTerm& rhs);

private:
	const BVTermContent* mpContent;

public:
	BVTerm();

	BVTerm(BVTermType _type, BVValue _value);

	BVTerm(BVTermType _type, const BVVariable& _variable);

	BVTerm(BVTermType _type, const BVTerm& _operand, std::size_t _index = 0);

	BVTerm(BVTermType _type, const BVTerm& _first, const BVTerm& _second);

	BVTerm(BVTermType _type, const BVTerm& _operand, std::size_t _first, std::size_t _last);

	std::size_t hash() const;

	std::size_t width() const;

	BVTermType type() const;

	bool is_constant() const {
		return type() == BVTermType::CONSTANT;
	}

	/**
	 * @return An approximation of the complexity of this bit vector term.
	 */
	size_t complexity() const;

	void gatherBVVariables(std::set<BVVariable>& vars) const;

	bool isInvalid() const;

	const BVTerm& operand() const;

	std::size_t index() const;

	const BVTerm& first() const;

	const BVTerm& second() const;

	std::size_t highest() const;

	std::size_t lowest() const;

	const BVVariable& variable() const;

	const BVValue& value() const;

	BVTerm substitute(const std::map<BVVariable, BVTerm>& /*unused*/) const;
};

bool operator==(const BVTerm& lhs, const BVTerm& rhs);
bool operator<(const BVTerm& lhs, const BVTerm& rhs);

std::ostream& operator<<(std::ostream& os, const BVTerm& term);

} // namespace carl

namespace std {
/**
 * Implements std::hash for bit vector terms.
 */
template<>
struct hash<carl::BVTerm> {
	/**
	 * @param t The bit vector term to get the hash for.
	 * @return The hash of the given bit vector term.
	 */
	std::size_t operator()(const carl::BVTerm& t) const {
		return t.hash();
	}
};
} // namespace std
