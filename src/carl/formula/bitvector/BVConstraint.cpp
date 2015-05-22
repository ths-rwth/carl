/**
 * @file BVConstraint.cpp
 * @author Andreas Krueger <andreas.krueger@rwth-aachen.de>
 */

#include "BVConstraintPool.h"
#include "BVConstraint.h"

namespace carl
{
    BVConstraint BVConstraint::create(bool _consistent)
    {
        return *(BVConstraintPool::getInstance().create(_consistent));
    }

    BVConstraint BVConstraint::create(const BVCompareRelation& _relation,
        const BVTerm& _lhs, const BVTerm& _rhs)
    {
        return *(BVConstraintPool::getInstance().create(_relation, _lhs, _rhs));
    }
} // namespace carl
