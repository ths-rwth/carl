/**
 * @file BVConstraint.cpp
 * @author Andreas Krueger <andreas.krueger@rwth-aachen.de>
 */

#include "BVConstraint.h"
#include "BVConstraintPool.h"

namespace carl {
BVConstraint BVConstraint::create(bool _consistent) {
	return *(BVConstraintPool::getInstance().create(_consistent));
}

BVConstraint BVConstraint::create(const BVCompareRelation& _relation,
								  const BVTerm& _lhs, const BVTerm& _rhs) {
	return *(BVConstraintPool::getInstance().create(_relation, _lhs, _rhs));
}

bool operator==(const BVConstraint& lhs, const BVConstraint& rhs) {
	if (lhs.id() != 0 && rhs.id() != 0) {
		return lhs.id() == rhs.id();
	}
	return std::forward_as_tuple(lhs.relation(), lhs.lhs(), lhs.rhs()) == std::forward_as_tuple(rhs.relation(), rhs.lhs(), rhs.rhs());
}
bool operator<(const BVConstraint& lhs, const BVConstraint& rhs) {
	if (lhs.id() != 0 && rhs.id() != 0) {
		return lhs.id() < rhs.id();
	}
	return std::forward_as_tuple(lhs.relation(), lhs.lhs(), lhs.rhs()) < std::forward_as_tuple(rhs.relation(), rhs.lhs(), rhs.rhs());
}

std::ostream& operator<<(std::ostream& os, const BVConstraint& c) {
	if (c.isAlwaysConsistent()) return os << "TRUE";
	if (c.isAlwaysInconsistent()) return os << "FALSE";
	return os << c.lhs() << " " << c.relation() << " " << c.rhs();
}
} // namespace carl
