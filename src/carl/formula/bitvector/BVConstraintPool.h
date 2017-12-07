/* 
 * File:   BVConstraintPool.h
 * Author: Andreas Krueger <andreas.krueger@rwth-aachen.de>
 */

#pragma once

#include "Pool.h"

namespace carl
{
	// Forward declaration
	class BVConstraint;
	enum class BVCompareRelation : unsigned;
	class BVTerm;

	class BVConstraintPool : public Singleton<BVConstraintPool>, public Pool<BVConstraint>
	{
		friend Singleton<BVConstraintPool>;

		using Constraint = BVConstraint;
		using ConstraintPtr = Constraint*;
		using ConstConstraintPtr = const Constraint*;

		public:

		ConstConstraintPtr create(bool _consistent = true);

		ConstConstraintPtr create(const BVCompareRelation& _relation,
			const BVTerm& _lhs, const BVTerm& _rhs);

		void assignId(ConstraintPtr _constraint, std::size_t _id) override;
	};
}

#define BV_CONSTRAINT_POOL BVConstraintPool::getInstance()
