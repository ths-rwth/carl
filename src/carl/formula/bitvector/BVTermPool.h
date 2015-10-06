/* 
 * File:   BVTermPool.h
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

		BVTermPool();

		ConstTermPtr create();

		ConstTermPtr create(BVTermType _type, const BVValue _value);

		ConstTermPtr create(BVTermType _type, const BVVariable& _variable);

		ConstTermPtr create(BVTermType _type, const BVTerm& _operand, const size_t _index = 0);

		ConstTermPtr create(BVTermType _type, const BVTerm& _first, const BVTerm& _second);

		ConstTermPtr create(BVTermType _type, const BVTerm& _operand, const size_t _first, const size_t _last);

		void assignId(TermPtr _term, std::size_t _id);
	};
}

#define BV_TERM_POOL BVTermPool::getInstance()
