/**
 * @file BVConstraint.h
 * @author Andreas Krueger <andreas.krueger@rwth-aachen.de>
 */

#pragma once

#include "BVCompareRelation.h"
#include "BVTerm.h"

namespace carl {

// Forward declaration
class BVConstraintPool;

class BVConstraint {
	friend class BVConstraintPool;

private:
	/// The relation for comparing left- and right-hand side.
	BVCompareRelation mRelation;
	/// The left-hand side of the (in)equality.
	BVTerm mLhs;
	/// The right-hand size of the (in)equality.
	BVTerm mRhs;
	/// The hash value.
	std::size_t mHash = 0;
	/// The unique id.
	std::size_t mId = 0;

	explicit BVConstraint(bool _consistent = true)
		: mRelation(_consistent ? BVCompareRelation::EQ : BVCompareRelation::NEQ) {}

	/**
	 * Constructs the constraint: _lhs _relation _rhs
	 * @param _lhs The bit-vector term to be used as left-hand side of the constraint.
	 * @param _rhs The bit-vector term to be used as right-hand side of the constraint.
	 * @param _relation The relation symbol to be used for the constraint.
	 */
	BVConstraint(const BVCompareRelation& _relation, const BVTerm& _lhs, const BVTerm& _rhs)
		: mRelation(_relation), mLhs(_lhs), mRhs(_rhs),
		  mHash(carl::hash_all(mRelation, mLhs, mRhs)) {
		assert(mLhs.width() == mRhs.width());
	}

public:
	static BVConstraint create(bool _consistent = true);

	static BVConstraint create(const BVCompareRelation& _relation,
							   const BVTerm& _lhs, const BVTerm& _rhs);

	/**
	 * @return The bit-vector term being the left-hand side of this constraint.
	 */
	const BVTerm& lhs() const {
		return mLhs;
	}

	/**
	 * @return The bit-vector term being the right-hand side of this constraint.
	 */
	const BVTerm& rhs() const {
		return mRhs;
	}

	/**
	 * @return The relation symbol of this constraint.
	 */
	BVCompareRelation relation() const {
		return mRelation;
	}

	/**
	 * @return The unique id of this constraint.
	 */
	std::size_t id() const {
		return mId;
	}

	std::size_t hash() const {
		return mHash;
	}

	/**
	 * @return An approximation of the complexity of this bit vector constraint.
	 */
	std::size_t complexity() const {
		return 1 + mLhs.complexity() + mRhs.complexity();
	}

	void collectVariables(std::set<BVVariable>& vars) const {
		mLhs.collectVariables(vars);
		mRhs.collectVariables(vars);
	}

	/**
	 * @return A hash value for this constraint.
	 */
	std::size_t getHash() const {
		return mHash;
	}

	bool isConstant() const {
		return mLhs.isInvalid() && mRhs.isInvalid();
	}

	bool isAlwaysConsistent() const {
		return isConstant() && mRelation == BVCompareRelation::EQ;
	}

	bool isAlwaysInconsistent() const {
		return isConstant() && mRelation == BVCompareRelation::NEQ;
	}
};

bool operator==(const BVConstraint& lhs, const BVConstraint& rhs);
bool operator<(const BVConstraint& lhs, const BVConstraint& rhs);

std::ostream& operator<<(std::ostream& os, const BVConstraint& c);
} // namespace carl

namespace std {
/**
 * Implements std::hash for bit-vector constraints.
 */
template<>
struct hash<carl::BVConstraint> {
	/**
	 * @param _constraint The bit-vector constraint to get the hash for.
	 * @return The hash of the given constraint.
	 */
	std::size_t operator()(const carl::BVConstraint& c) const {
		return c.hash();
	}
};
} // namespace std
