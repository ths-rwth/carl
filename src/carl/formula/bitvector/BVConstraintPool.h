/* 
 * File:   BVPools.h
 * Author: Andreas Krueger <andreas.krueger@rwth-aachen.de>
 */

#pragma once

#include "Pool.h"
#include "BVConstraint.h"

namespace carl
{
	template<typename Pol>
	class BVConstraintPool : public Singleton<BVConstraintPool<Pol>>, public Pool<BVConstraint<Pol>>
	{
		friend Singleton<BVConstraintPool>;

		typedef BVConstraint<Pol> Constraint;
		typedef Constraint* ConstraintPtr;
		typedef const Constraint* ConstConstraintPtr;

		public:

		ConstConstraintPtr create(const BVCompareRelation& _relation,
			const BVTerm<Pol>& _lhs, const BVTerm<Pol>& _rhs)
		{
			return this->add(new Constraint(_relation, _lhs, _rhs));
		}
	};
}

#define BV_CONSTRAINT_POOL BVConstraintPool<Pol>::getInstance()
