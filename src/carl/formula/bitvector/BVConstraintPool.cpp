/* 
 * File:   BVConstraintPool.cpp
 * Author: Andreas Krueger <andreas.krueger@rwth-aachen.de>
 */

#include "BVConstraintPool.h"
#include "BVConstraint.h"

namespace carl
{
    BVConstraintPool::ConstConstraintPtr BVConstraintPool::create(bool _consistent)
    {
        return this->add(new Constraint(_consistent));
    }

    BVConstraintPool::ConstConstraintPtr BVConstraintPool::create(const BVCompareRelation& _relation,
        const BVTerm& _lhs, const BVTerm& _rhs)
    {
        return this->add(new Constraint(_relation, _lhs, _rhs));
    }
}
