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
	class BVTermPool : public Singleton<BVTermPool<Pol>>, public Pool<BVTerm<Pol>>
	{
		friend Singleton<BVTermPool>;

		typedef BVTerm<Pol> Term;
		typedef Term* TermPtr;
		typedef const Term* ConstTermPtr;

		public:

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
