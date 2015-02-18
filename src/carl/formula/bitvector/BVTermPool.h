/* 
 * File:   BVPools.h
 * Author: Andreas Krueger <andreas.krueger@rwth-aachen.de>
 */

#pragma once

#include "Pool.h"
#include "BVTerm.h"

namespace carl
{
	template<typename Pol>
	class BVTermPool : public Singleton<BVTermPool<Pol>>, public Pool<BVTermContent<Pol>>
	{
		friend Singleton<BVTermPool>;

		typedef BVTermContent<Pol> Term;
		typedef Term* TermPtr;
		typedef const Term* ConstTermPtr;

		ConstTermPtr mpInvalid;

		public:

		BVTermPool() : Singleton<BVTermPool<Pol>> (), Pool<BVTermContent<Pol>> ()
		{
			this->mpInvalid = this->add(new Term);
		}

		ConstTermPtr create()
		{
			return this->mpInvalid;
		}

		ConstTermPtr create(BVTermType _type, BVValue _value)
		{
			return this->add(new Term(_type, _value));
		}

		ConstTermPtr create(BVTermType _type, Variable::Arg _variable, size_t _width = 1)
		{
			return this->add(new Term(_type, _variable, _width));
		}

		ConstTermPtr create(BVTermType _type, const Formula<Pol>& _booleanFormula, const BVTerm<Pol>& _subtermA, const BVTerm<Pol>& _subtermB)
		{
			return this->add(new Term(_type, _booleanFormula, _subtermA, _subtermB));
		}

		ConstTermPtr create(BVTermType _type, const BVTerm<Pol>& _operand, const size_t _index = 0)
		{
			return this->add(new Term(_type, _operand, _index));
		}

		ConstTermPtr create(BVTermType _type, const BVTerm<Pol>& _first, const BVTerm<Pol>& _second)
		{
			return this->add(new Term(_type, _first, _second));
		}

		ConstTermPtr create(BVTermType _type, const BVTerm<Pol>& _operand, const size_t _first, const size_t _last)
		{
			return this->add(new Term(_type, _operand, _first, _last));
		}
	};
}

#define BV_TERM_POOL BVTermPool<Pol>::getInstance()
