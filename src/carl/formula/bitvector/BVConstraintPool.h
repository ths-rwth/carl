/* 
 * File:   BVConstraintPool.h
 * Author: Andreas Krueger <andreas.krueger@rwth-aachen.de>
 */

#pragma once

#include "Pool.h"
#include "BVConstraint.h"

namespace carl
{
	class BVConstraintPool : public Singleton<BVConstraintPool>, public Pool<BVConstraint>
	{
		friend Singleton<BVConstraintPool>;

		typedef BVConstraint Constraint;
		typedef Constraint* ConstraintPtr;
		typedef const Constraint* ConstConstraintPtr;

		public:

		ConstConstraintPtr create(bool _consistent = true)
		{
			return this->add(new Constraint(_consistent));
		}

		ConstConstraintPtr create(const BVCompareRelation& _relation,
			const BVTerm& _lhs, const BVTerm& _rhs)
		{
			return this->add(new Constraint(_relation, _lhs, _rhs));
		}
	};
}

#define BV_CONSTRAINT_POOL BVConstraintPool::getInstance()
