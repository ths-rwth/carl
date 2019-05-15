/** 
 * @file BVCompareRelation.h
 * @author Andreas Krueger <andreas.krueger@rwth-aachen.de>
 */

#pragma once

#include <cassert>
#include <iostream>
#include <string>

namespace carl
{

	enum class BVCompareRelation : unsigned
	{
		EQ, // =
		NEQ, // <>
		ULT, // unsigned <
		ULE, // unsigned <=
		UGT, // unsigned >
		UGE, // unsigned >=
		SLT, // signed <
		SLE, // signed <=
		SGT, // signed >
		SGE // signed >=
	};

	inline std::string toString(BVCompareRelation _r)
	{
		switch(_r) {
		case BVCompareRelation::EQ: return "=";
		case BVCompareRelation::NEQ: return "<>";
		case BVCompareRelation::ULT: return "bvult";
		case BVCompareRelation::ULE: return "bvule";
		case BVCompareRelation::UGT: return "bvugt";
		case BVCompareRelation::UGE: return "bvuge";
		case BVCompareRelation::SLT: return "bvslt";
		case BVCompareRelation::SLE: return "bvsle";
		case BVCompareRelation::SGT: return "bvsgt";
		case BVCompareRelation::SGE: return "bvsge";
		}
		assert(false);
		return "";
	}

	inline std::ostream& operator<<(std::ostream& _os, const BVCompareRelation& _r)
	{
		return(_os << toString(_r));
	}

	inline std::size_t toId(const BVCompareRelation _relation)
	{
		return static_cast<std::size_t>(_relation);
	}

	inline BVCompareRelation inverse(BVCompareRelation _c)
	{
		switch(_c) {
		case BVCompareRelation::EQ: return BVCompareRelation::NEQ;
		case BVCompareRelation::NEQ: return BVCompareRelation::EQ;
		case BVCompareRelation::ULT: return BVCompareRelation::UGE;
		case BVCompareRelation::ULE: return BVCompareRelation::UGT;
		case BVCompareRelation::UGT: return BVCompareRelation::ULE;
		case BVCompareRelation::UGE: return BVCompareRelation::ULT;
		case BVCompareRelation::SLT: return BVCompareRelation::SGE;
		case BVCompareRelation::SLE: return BVCompareRelation::SGT;
		case BVCompareRelation::SGT: return BVCompareRelation::SLE;
		case BVCompareRelation::SGE: return BVCompareRelation::SLT;
		}
		assert(false);
		return BVCompareRelation::EQ;
	}

	inline bool relationIsStrict(BVCompareRelation _r)
	{
		return _r == BVCompareRelation::ULT ||
			_r == BVCompareRelation::UGT ||
			_r == BVCompareRelation::SLT ||
			_r == BVCompareRelation::SGT ||
			_r == BVCompareRelation::NEQ;
	}

	inline bool relationIsSigned(BVCompareRelation _r)
	{
		return _r == BVCompareRelation::SLT ||
			_r == BVCompareRelation::SLE ||
			_r == BVCompareRelation::SGT ||
			_r == BVCompareRelation::SGE;
	}

} // end namespace carl

namespace std
{

	template<>
	struct hash<carl::BVCompareRelation>
	{

		std::size_t operator()(const carl::BVCompareRelation& _rel) const
		{
			return static_cast<std::size_t>(_rel);
		}
	};

} // end namespace std
