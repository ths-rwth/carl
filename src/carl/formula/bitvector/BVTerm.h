/**
 * @file BVTerm.h
 * @author Andreas Krueger <andreas.krueger@rwth-aachen.de>
 */

#pragma once

#include "BVVariable.h"
#include "../Formula.h"
#include <boost/dynamic_bitset.hpp>

namespace carl
{
	typedef boost::dynamic_bitset<> BVValue;

	enum class BVTermType
	{
		CONSTANT,
		VARIABLE,
		ITE,
		CONCAT, EXTRACT,
		NOT,
		NEG,
		AND, OR, XOR, NAND, NOR, XNOR,
		ADD, SUB, MUL, DIV_U, DIV_S, MOD_U, MOD_S1, MOD_S2,
		EQ,
		LSHIFT, RSHIFT_LOGIC, RSHIFT_ARITH,
		LROTATE, RROTATE,
		EXT_U, EXT_S,
		REPEAT
	};

	inline std::string toString(const BVTermType _type)
	{
		switch(_type) {
		case BVTermType::CONSTANT: return "CONSTANT";
		case BVTermType::VARIABLE: return "VARIABLE";
		case BVTermType::ITE: return "ite";
		case BVTermType::CONCAT: return "concat";
		case BVTermType::EXTRACT: return "extract";
		case BVTermType::NOT: return "bvnot";
		case BVTermType::NEG: return "bvneg";
		case BVTermType::AND: return "bvand";
		case BVTermType::OR: return "bvor";
		case BVTermType::XOR: return "bvxor";
		case BVTermType::NAND: return "bvnand";
		case BVTermType::NOR: return "bvnor";
		case BVTermType::XNOR: return "bvxnor";
		case BVTermType::ADD: return "bvplus";
		case BVTermType::SUB: return "bvsub";
		case BVTermType::MUL: return "bvmul";
		case BVTermType::DIV_U: return "bvudiv";
		case BVTermType::DIV_S: return "bvsdiv";
		case BVTermType::MOD_U: return "bvurem";
		case BVTermType::MOD_S1: return "bvsrem";
		case BVTermType::MOD_S2: return "bvsmod";
		case BVTermType::EQ: return "bvcomp";
		case BVTermType::LSHIFT: return "bvshl";
		case BVTermType::RSHIFT_LOGIC: return "bvlshr";
		case BVTermType::RSHIFT_ARITH: return "bvashr";
		case BVTermType::LROTATE: return "rotate_left";
		case BVTermType::RROTATE: return "rotate_right";
		case BVTermType::EXT_U: return "zero_extend";
		case BVTermType::EXT_S: return "sign_extend";
		case BVTermType::REPEAT: return "repeat";
		}
		assert(false);
		return "";
	}

	// forward declaration
	template<typename Pol>
	class BVTerm;

	template<typename Pol>
	struct BVITEContent
	{
		const Formula<Pol>& mCondition;
		const BVTerm<Pol>& mThen;
		const BVTerm<Pol>& mElse;

		BVITEContent(const Formula<Pol>& _condition, const BVTerm<Pol>& _then, const BVTerm<Pol>& _else) :
		mCondition(_condition), mThen(_then), mElse(_else)
		{
			assert(_then.width() == _else.width());
		}
	};

	template<typename Pol>
	struct BVUnaryContent
	{
		const BVTerm<Pol>& mOperand;
		size_t mIndex;

		BVUnaryContent(const BVTerm<Pol>& _operand, const size_t _index = 0) :
		mOperand(_operand), mIndex(_index)
		{
		}
	};

	template<typename Pol>
	struct BVBinaryContent
	{
		const BVTerm<Pol>& mFirst;
		const BVTerm<Pol>& mSecond;

		BVBinaryContent(const BVTerm<Pol>& _first, const BVTerm<Pol>& _second) :
		mFirst(_first), mSecond(_second)
		{
		}
	};

	template<typename Pol>
	struct BVExtractContent
	{
		const BVTerm<Pol>& mOperand;
		size_t mFirst;
		size_t mLast;

		BVExtractContent(const BVTerm<Pol>& _operand, const size_t _first, const size_t _last) :
		mOperand(_operand), mFirst(_first), mLast(_last)
		{
		}
	};

	// Forward declaration
	template<typename Element>
	class Pool;

	template<typename Pol>
	class BVTerm
	{
		friend class Pool<BVTerm<Pol>>;

	private:
		BVTermType mType;

		union
		{
			BVVariable mVariable;
			BVValue mValue;
			BVITEContent<Pol> mIte;
			BVUnaryContent<Pol> mUnary;
			BVBinaryContent<Pol> mBinary;
			BVExtractContent<Pol> mExtract;
		};
		size_t mWidth;
		size_t mId;

	public:

		BVTerm(BVTermType _type, BVValue _value) :
		mType(_type), mValue(_value), mWidth(_value.size())
		{
			assert(_type == BVTermType::CONSTANT);
		}

		BVTerm(BVTermType _type, Variable::Arg _variable, size_t _width = 1) :
		mType(_type), mVariable(_variable, _width), mWidth(_width)
		{
			assert(_type == BVTermType::VARIABLE);
			// assert( _variable.getType() == VariableType::VT_BITVECTOR );
		}

		BVTerm(BVTermType _type, const Formula<Pol>& _booleanFormula, const BVTerm<Pol>& _subtermA, const BVTerm<Pol>& _subtermB) :
		mType(_type), mIte(_booleanFormula, _subtermA, _subtermB), mWidth(_subtermA.width())
		{
			assert(_type == BVTermType::ITE);
			assert(_subtermA.width() == _subtermB.width());
		}

		BVTerm(BVTermType _type, const BVTerm<Pol>& _operand, const size_t _index = 0) :
		mType(_type), mUnary(_operand, _index)
		{
			if(_type == BVTermType::NOT || _type == BVTermType::NEG) {
				assert(_index == 0);
				mWidth = _operand.width();
			} else if(_type == BVTermType::LROTATE || _type == BVTermType::RROTATE) {
				assert(_index >= 0);
				mWidth = _operand.width();
			} else if(_type == BVTermType::REPEAT) {
				assert(_index > 0);
				mWidth = _operand.width() * _index;
			} else if(_type == BVTermType::EXT_U || _type == BVTermType::EXT_S) {
				assert(_index >= _operand.width());
				mWidth = _index;
			} else {
				assert(false);
			}
		}

		BVTerm(BVTermType _type, const BVTerm<Pol>& _first, const BVTerm<Pol>& _second) :
		mType(_type), mBinary(_first, _second)
		{
			assert(
				_type == BVTermType::CONCAT || _type == BVTermType::AND || _type == BVTermType::OR
				|| _type == BVTermType::XOR || _type == BVTermType::NAND || _type == BVTermType::NOR
				|| _type == BVTermType::XNOR || _type == BVTermType::ADD || _type == BVTermType::SUB
				|| _type == BVTermType::MUL || _type == BVTermType::DIV_U || _type == BVTermType::DIV_S
				|| _type == BVTermType::MOD_U || _type == BVTermType::MOD_S1 || _type == BVTermType::MOD_S2
				|| _type == BVTermType::EQ || _type == BVTermType::LSHIFT || _type == BVTermType::RSHIFT_LOGIC
				|| _type == BVTermType::RSHIFT_ARITH
				);

			if(_type == BVTermType::CONCAT) {
				mWidth = _first.width() + _second.width();
			} else if(_type == BVTermType::EQ) {
				mWidth = 1;
			} else {
				mWidth = _first.width();
			}
		}

		BVTerm(BVTermType _type, const BVTerm<Pol>& _operand, const size_t _first, const size_t _last) :
		mType(_type), mExtract(_operand, _first, _last)
		{
			assert(_type == BVTermType::EXTRACT);
			assert(_first >= 0 && _last >= _first && _last < _operand.width());
			mWidth = _last - _first + 1;
		}

		~BVTerm()
		{
			if(mType == BVTermType::VARIABLE) {
				mVariable.~BVVariable();
			} else if(mType == BVTermType::CONSTANT) {
				mValue.~BVValue();
			}
		}

		size_t width() const
		{
			return mWidth;
		}

		BVTermType type() const
		{
			return mType;
		}

		/**
		 * Gives the string representation of this bit vector term.
		 * @param _withActivity A flag which indicates whether to add the formulas' activity to the result.
		 *                      (not yet supported for bit vector objects, but is passed to subformulae)
		 * @param _resolveUnequal A switch which indicates how to represent the relation symbol for unequal.
		 *                        (Again, only applies to non-bitvector parts of the term.
		 *                        For further description see documentation of Formula::toString( .. ))
		 * @param _init The initial string of every row of the result.
		 * @param _oneline A flag indicating whether the term shall be printed on one line.
		 * @param _infix A flag indicating whether to print the term in infix or prefix notation.
		 * @param _friendlyNames A flag that indicates whether to print the variables with their internal representation (false)
		 *                        or with their dedicated names.
		 * @return The resulting string representation of this term.
		 */
		std::string toString(bool _withActivity = false, unsigned _resolveUnequal = 0, const std::string _init = "", bool _oneline = true, bool _infix = false, bool _friendlyNames = true) const
		{
			if(mType == BVTermType::CONSTANT) {
				string valueStr;
				boost::to_string(mValue, valueStr);
				return _init + "0b" + valueStr;
			} else if(mType == BVTermType::VARIABLE) {
				return _init + mVariable.toString(_friendlyNames);
			} else {
				std::string operatorStr = carl::toString(mType);
				std::string operatorPrefix = operatorStr;
				std::string operatorInfix = operatorStr;

				std::string argFirst;
				std::string argSecond;
				std::string argThird;

				// Rewrite operator strings for indexed (parameterized) operators
				if(mType == BVTermType::EXTRACT) {
					operatorPrefix = "(_ " + operatorStr + " " + std::to_string(mExtract.mFirst) + " " + std::to_string(mExtract.mLast) + ")";
					operatorInfix = operatorStr + "_{" + std::to_string(mExtract.mFirst) + "," + std::to_string(mExtract.mLast) + "}";
				} else if(mType == BVTermType::LROTATE || mType == BVTermType::RROTATE
					|| mType == BVTermType::EXT_U || mType == BVTermType::EXT_S || mType == BVTermType::REPEAT) {
					operatorPrefix = "(_ " + operatorStr + " " + std::to_string(mUnary.mIndex) + ")";
					operatorInfix = operatorStr + "_" + std::to_string(mUnary.mIndex);
				}

				// Fill arg* variables
				if(mType == BVTermType::ITE) {
					argFirst = mIte.mCondition.toString(_withActivity, _resolveUnequal, (_oneline ? "" : _init + "   "), _oneline, _infix, _friendlyNames);
					argSecond = mIte.mThen.toString(_withActivity, _resolveUnequal, (_oneline ? "" : _init + "   "), _oneline, _infix, _friendlyNames);
					argThird = mIte.mElse.toString(_withActivity, _resolveUnequal, (_oneline ? "" : _init + "   "), _oneline, _infix, _friendlyNames);
				} else if(mType == BVTermType::EXTRACT) {
					argFirst = mExtract.mOperand.toString(_withActivity, _resolveUnequal, (_oneline ? "" : _init + "   "), _oneline, _infix, _friendlyNames);
				} else if(mType == BVTermType::NOT || mType == BVTermType::NEG || mType == BVTermType::LROTATE
					|| mType == BVTermType::RROTATE || mType == BVTermType::REPEAT || mType == BVTermType::EXT_U
					|| mType == BVTermType::EXT_S) {
					argFirst = mUnary.mOperand.toString(_withActivity, _resolveUnequal, (_oneline ? "" : _init + "   "), _oneline, _infix, _friendlyNames);
				} else if(mType == BVTermType::CONCAT || mType == BVTermType::AND || mType == BVTermType::OR
					|| mType == BVTermType::XOR || mType == BVTermType::NAND || mType == BVTermType::NOR
					|| mType == BVTermType::XNOR || mType == BVTermType::ADD || mType == BVTermType::SUB
					|| mType == BVTermType::MUL || mType == BVTermType::DIV_U || mType == BVTermType::DIV_S
					|| mType == BVTermType::MOD_U || mType == BVTermType::MOD_S1 || mType == BVTermType::MOD_S2
					|| mType == BVTermType::EQ || mType == BVTermType::LSHIFT || mType == BVTermType::RSHIFT_LOGIC
					|| mType == BVTermType::RSHIFT_ARITH) {
					argFirst = mBinary.mFirst.toString(_withActivity, _resolveUnequal, (_oneline ? "" : _init + "   "), _oneline, _infix, _friendlyNames);
					argSecond = mBinary.mSecond.toString(_withActivity, _resolveUnequal, (_oneline ? "" : _init + "   "), _oneline, _infix, _friendlyNames);
				} else
					assert(false);

				if(_infix && !argFirst.empty() && !argSecond.empty() && argThird.empty()) {
					// Infix notation is only really infix if we have exactly two arguments
					return _init + "("
						+ (_oneline ? "" : "\n")
						+ argFirst
						+ (_oneline ? " " : ("\n" + _init))
						+ operatorInfix
						+ (_oneline ? " " : "\n")
						+ argSecond
						+ (_oneline ? "" : ("\n" + _init))
						+ ")";
				} else {
					return _init
						+ (_infix ? (operatorInfix + "(") : ("(" + operatorPrefix + (_oneline ? " " : "")))
						+ (_oneline ? "" : "\n") + argFirst
						+ (argSecond.empty() ? "" : ((_oneline ? " " : "\n") + argSecond))
						+ (argThird.empty() ? "" : ((_oneline ? " " : "\n") + argThird))
						+ ")";
				}
			}
		}

		bool operator==(const BVTerm<Pol>& _other) const
		{
			return mId == _other.mId; // TODO: Make sure this also works if any mId is not set
		}

		/**
		 * The output operator of a term.
		 * @param _out The stream to print on.
		 * @param _term The term to be printed.
		 */
		template<typename P>
		friend std::ostream& operator<<(std::ostream& _out, const BVTerm<P>& _term)
		{
			return(_out << _term.toString());
		}
	};
}

namespace std
{
	/**
	 * Implements std::hash for bit vector terms.
	 */
	template<typename Pol>
	struct hash<carl::BVTerm<Pol>>
	{
		public:

		/**
		 * @param _formula The bit vector term to get the hash for.
		 * @return The hash of the given bit vector term.
		 */
		size_t operator()(const carl::BVTerm<Pol>& _term) const
		{
			return 0; // TODO: Implement :)
		}
	};
}