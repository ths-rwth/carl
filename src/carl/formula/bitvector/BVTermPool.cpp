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
        return this->add(new Term(_type, _operand, _index));
    }

    BVTermPool::ConstTermPtr BVTermPool::create(BVTermType _type, const BVTerm& _first, const BVTerm& _second)
    {
        return this->add(new Term(_type, _first, _second));
    }

    BVTermPool::ConstTermPtr BVTermPool::create(BVTermType _type, const BVTerm& _operand, const size_t _first, const size_t _last)
    {
        return this->add(new Term(_type, _operand, _first, _last));
    }
}

#define BV_TERM_POOL BVTermPool::getInstance()
