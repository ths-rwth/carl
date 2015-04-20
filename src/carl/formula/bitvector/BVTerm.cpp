/**
 * @file BVTerm.cpp
 * @author Andreas Krueger <andreas.krueger@rwth-aachen.de>
 */

#include "BVTermPool.h"
#include "BVTerm.h"

namespace carl
{
    BVTerm::BVTerm() :
    mpContent(BVTermPool::getInstance().create())
    {
    }

    BVTerm::BVTerm(BVTermType _type, BVValue _value) :
    mpContent(BVTermPool::getInstance().create(_type, _value))
    {
    }

    BVTerm::BVTerm(BVTermType _type, const BVVariable& _variable) :
    mpContent(BVTermPool::getInstance().create(_type, _variable))
    {
    }

    BVTerm::BVTerm(BVTermType _type, const BVTerm& _operand, const size_t _index) :
    mpContent(BVTermPool::getInstance().create(_type, _operand, _index))
    {
    }

    BVTerm::BVTerm(BVTermType _type, const BVTerm& _first, const BVTerm& _second) :
    mpContent(BVTermPool::getInstance().create(_type, _first, _second))
    {
    }

    BVTerm::BVTerm(BVTermType _type, const BVTerm& _operand, const size_t _first, const size_t _last) :
    mpContent(BVTermPool::getInstance().create(_type, _operand, _first, _last))
    {
    }

    std::string BVTerm::toString(const std::string _init, bool _oneline, bool _infix, bool _friendlyNames) const
    {
        return mpContent->toString(_init, _oneline, _infix, _friendlyNames);
    }

    std::size_t BVTerm::hash() const
    {
        return mpContent->hash();
    }

    std::size_t BVTerm::width() const
    {
        return mpContent->width();
    }

    BVTermType BVTerm::type() const
    {
        return mpContent->type();
    }

    const BVTerm& BVTerm::operand() const
    {
        if(mpContent->type() == BVTermType::EXTRACT) {
            return mpContent->mExtract.mOperand;
        } else {
            return mpContent->mUnary.mOperand;
        }
    }

    std::size_t BVTerm::index() const
    {
        return mpContent->mUnary.mIndex;
    }

    const BVTerm& BVTerm::first() const
    {
        return mpContent->mBinary.mFirst;
    }

    const BVTerm& BVTerm::second() const
    {
        return mpContent->mBinary.mSecond;
    }

    std::size_t BVTerm::highest() const
    {
        return mpContent->mExtract.mHighest;
    }

    std::size_t BVTerm::lowest() const
    {
        return mpContent->mExtract.mLowest;
    }

    const BVVariable& BVTerm::variable() const
    {
        return mpContent->mVariable;
    }

    const BVValue& BVTerm::value() const
    {
        return mpContent->mValue;
    }

    bool BVTerm::operator<(const BVTerm& rhs) const {
        return *(this->mpContent) < *(rhs.mpContent);
    }

}
