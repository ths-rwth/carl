/* 
 * File:   BVTermPool.cpp
 * Author: Andreas Krueger <andreas.krueger@rwth-aachen.de>
 */

#include "BVTermPool.h"

namespace carl
{
    BVTermPool::BVTermPool() : Singleton<BVTermPool> (), Pool<BVTermContent> ()
    {
        this->mpInvalid = this->add(new Term);
    }

    BVTermPool::ConstTermPtr BVTermPool::create()
    {
        return this->mpInvalid;
    }

    BVTermPool::ConstTermPtr BVTermPool::create(BVTermType _type, const BVValue _value)
    {
        return this->add(new Term(_type, _value));
    }

    BVTermPool::ConstTermPtr BVTermPool::create(BVTermType _type, const BVVariable& _variable)
    {
        return this->add(new Term(_type, _variable));
    }

    BVTermPool::ConstTermPtr BVTermPool::create(BVTermType _type, const BVTerm& _operand, const size_t _index)
    {
        if (_operand.isConstant()) {
            switch (_type) {
                case BVTermType::NOT: {
                    return create(BVTermType::CONSTANT, BVValue(~_operand.value()()));
                }
                default:
                    CARL_LOG_WARN("carl.bitvector", "No simplification for " << _type << " BVTerm.");
            }
        }
        return this->add(new Term(_type, _operand, _index));
    }

    BVTermPool::ConstTermPtr BVTermPool::create(BVTermType _type, const BVTerm& _first, const BVTerm& _second)
    {
        if (_first.isConstant() && _second.isConstant()) {
            switch (_type) {
                case BVTermType::AND: {
                    assert(_first.width() == _second.width());
                    return create(BVTermType::CONSTANT, BVValue(_first.value()() & _second.value()()));
                }
                case BVTermType::OR: {
                    assert(_first.width() == _second.width());
                    return create(BVTermType::CONSTANT, BVValue(_first.value()() | _second.value()()));
                }
                case BVTermType::XOR: {
                    assert(_first.width() == _second.width());
                    return create(BVTermType::CONSTANT, BVValue(_first.value()() ^ _second.value()()));
                }
                case BVTermType::NAND: {
                    assert(_first.width() == _second.width());
                    return create(BVTermType::CONSTANT, BVValue(~(_first.value()() & _second.value()())));
                }
                case BVTermType::NOR: {
                    assert(_first.width() == _second.width());
                    return create(BVTermType::CONSTANT, BVValue(~(_first.value()() | _second.value()())));
                }
                case BVTermType::XNOR: {
                    assert(_first.width() == _second.width());
                    return create(BVTermType::CONSTANT, BVValue(~(_first.value()() ^ _second.value()())));
                }
                case BVTermType::EQ: {
                    assert(_first.width() == _second.width());
                    unsigned val = (_first.value()() == _second.value()()) ? 1 : 0;
                    return create(BVTermType::CONSTANT, BVValue(boost::dynamic_bitset<>(1, val)));
                }
                default:
                    CARL_LOG_WARN("carl.bitvector", "No simplification for " << _type << " BVTerm.");
            }
        }
        return this->add(new Term(_type, _first, _second));
    }

    BVTermPool::ConstTermPtr BVTermPool::create(BVTermType _type, const BVTerm& _operand, const size_t _first, const size_t _last)
    {
        return this->add(new Term(_type, _operand, _first, _last));
    }
}

#define BV_TERM_POOL BVTermPool::getInstance()
