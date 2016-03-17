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
        if(_lhs.isConstant() && _rhs.isConstant()) {
            assert(_lhs.width() == _rhs.width());
            const BVValue& lval = _lhs.value();
            const BVValue& rval = _rhs.value();

            if(_relation == BVCompareRelation::EQ) {
                return create(lval == rval);
            } else if(_relation == BVCompareRelation::NEQ) {
                return create(!(lval == rval));
            }

            bool less = lval < rval;

            if(_relation == BVCompareRelation::ULT) {
                return create(less);
            } else if(_relation == BVCompareRelation::ULE) {
                return create(less || (lval == rval));
            } else if(_relation == BVCompareRelation::UGT) {
                return create(! less && !(lval == rval));
            } else if(_relation == BVCompareRelation::UGE) {
                return create(! less);
            }

            bool lhsNegative = lval[lval.width()-1];
            bool rhsNegative = rval[rval.width()-1];

            if(_relation == BVCompareRelation::SLT) {
                return create((lhsNegative && ! rhsNegative) || (lhsNegative == rhsNegative && less));
            } else if(_relation == BVCompareRelation::SLE) {
                return create((lhsNegative && ! rhsNegative) || (lhsNegative == rhsNegative && less) || lval == rval);
            } else if(_relation == BVCompareRelation::SGT) {
                return create((! lhsNegative && rhsNegative) || (lhsNegative == rhsNegative && ! less && !(lval == rval)));
            } else if(_relation == BVCompareRelation::SGE) {
                return create((! lhsNegative && rhsNegative) || (lhsNegative == rhsNegative && ! less));
            }

            CARL_LOG_WARN("carl.bitvector", "No simplification for " << _relation << " BVConstraint.");
        }

        return this->add(new Constraint(_relation, _lhs, _rhs));
    }

    void BVConstraintPool::assignId(ConstraintPtr _constraint, std::size_t _id)
    {
        _constraint->mId = _id;
    }

}
