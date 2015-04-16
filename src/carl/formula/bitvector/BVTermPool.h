/* 
 * File:   BVPools.h
 * Author: Andreas Krueger <andreas.krueger@rwth-aachen.de>
 */

#pragma once

#include "Pool.h"
#include "BVTerm.h"

namespace carl
{
	class BVTermPool : public Singleton<BVTermPool>, public Pool<BVTermContent>
	{
		friend Singleton<BVTermPool>;
	public:

		typedef BVTermContent Term;
		typedef Term* TermPtr;
		typedef const Term* ConstTermPtr;
	private:

		ConstTermPtr mpInvalid;

	public:

		BVTermPool() : Singleton<BVTermPool> (), Pool<BVTermContent> ()
		{
			this->mpInvalid = this->add(new Term);
		}

		ConstTermPtr create()
		{
			return this->mpInvalid;
		}

		ConstTermPtr create(BVTermType _type, const BVValue _value)
		{
			return this->add(new Term(_type, _value));
		}

		ConstTermPtr create(BVTermType _type, const BVVariable& _variable)
		{
			return this->add(new Term(_type, _variable));
		}

		ConstTermPtr create(BVTermType _type, const BVTerm& _operand, const size_t _index = 0)
		{
			return this->add(new Term(_type, _operand, _index));
		}

		ConstTermPtr create(BVTermType _type, const BVTerm& _first, const BVTerm& _second)
		{
			return this->add(new Term(_type, _first, _second));
		}

		ConstTermPtr create(BVTermType _type, const BVTerm& _operand, const size_t _first, const size_t _last)
		{
			return this->add(new Term(_type, _operand, _first, _last));
		}
	};
}

#define BV_TERM_POOL BVTermPool::getInstance()
