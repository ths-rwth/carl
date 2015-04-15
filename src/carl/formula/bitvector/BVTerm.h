/**
 * @file BVTerm.h
 * @author Andreas Krueger <andreas.krueger@rwth-aachen.de>
 */

#pragma once

#include "BVVariable.h"
#include "../Formula.h"
#include <boost/dynamic_bitset.hpp>

/**
 * Implementation of boost::hash_value for dynamic bitsets.
 * TODO: Make more efficient (currently uses convertion to string).
 * See also: https://stackoverflow.com/q/3896357
 */
namespace boost
{

	template <typename B, typename A>
	std::size_t hash_value(const boost::dynamic_bitset<B, A>& bs)
	{
		std::string stringRepresentation;
		boost::to_string(bs, stringRepresentation);
		return std::hash<std::string>()(stringRepresentation);
	}
}

namespace carl
{
	typedef boost::dynamic_bitset<> BVValue;

	enum class BVTermType : unsigned
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
		default: return "invalid";
		}
		assert(false);
		return "";
	}
	inline std::ostream& operator<<(std::ostream& os, BVTermType type) {
		return os << toString(type);
	}

	inline std::size_t typeId(const BVTermType _type)
	{
		return static_cast<std::size_t>(_type);
	}

	inline bool typeIsUnary(const BVTermType _type)
	{
		return(
			_type == BVTermType::NOT || _type == BVTermType::NEG || _type == BVTermType::LROTATE
			|| _type == BVTermType::RROTATE || _type == BVTermType::REPEAT || _type == BVTermType::EXT_U
			|| _type == BVTermType::EXT_S
			);
	}

	inline bool typeIsBinary(const BVTermType _type)
	{
		return(
			_type == BVTermType::CONCAT || _type == BVTermType::AND || _type == BVTermType::OR
			|| _type == BVTermType::XOR || _type == BVTermType::NAND || _type == BVTermType::NOR
			|| _type == BVTermType::XNOR || _type == BVTermType::ADD || _type == BVTermType::SUB
			|| _type == BVTermType::MUL || _type == BVTermType::DIV_U || _type == BVTermType::DIV_S
			|| _type == BVTermType::MOD_U || _type == BVTermType::MOD_S1 || _type == BVTermType::MOD_S2
			|| _type == BVTermType::EQ || _type == BVTermType::LSHIFT || _type == BVTermType::RSHIFT_LOGIC
			|| _type == BVTermType::RSHIFT_ARITH
			);
	}

	// forward declaration
	template<typename Pol>
	class BVTerm;

	template<typename Pol>
	struct BVITEContent
	{
		Formula<Pol> mCondition;
		BVTerm<Pol> mThen;
		BVTerm<Pol> mElse;

		BVITEContent(const Formula<Pol>& _condition, const BVTerm<Pol>& _then, const BVTerm<Pol>& _else) :
		mCondition(_condition), mThen(_then), mElse(_else)
		{
			assert(_then.width() == _else.width());
		}

		bool operator==(const BVITEContent& _other) const
		{
			return mCondition == _other.mCondition && mThen == _other.mThen && mElse == _other.mElse;
		}
		bool operator<(const BVITEContent& _other) const
		{
			if (mCondition != _other.mCondition) return mCondition < _other.mCondition;
			if (!(mThen == _other.mThen)) return mThen < _other.mThen;
			if (!(mElse == _other.mElse)) return mElse < _other.mElse;
			return false;
		}
	};

	template<typename Pol>
	struct BVUnaryContent
	{
		BVTerm<Pol> mOperand;
		size_t mIndex;

		BVUnaryContent(const BVTerm<Pol>& _operand, const size_t _index = 0) :
		mOperand(_operand), mIndex(_index)
		{
		}

		bool operator==(const BVUnaryContent& _other) const
		{
			return mOperand == _other.mOperand && mIndex == _other.mIndex;
		}
		bool operator<(const BVUnaryContent& _other) const
		{
			if (!(mOperand == _other.mOperand)) return mOperand < _other.mOperand;
			if (mIndex != _other.mIndex) return mIndex < _other.mIndex;
			return false;
		}
	};

	template<typename Pol>
	struct BVBinaryContent
	{
		BVTerm<Pol> mFirst;
		BVTerm<Pol> mSecond;

		BVBinaryContent(const BVTerm<Pol>& _first, const BVTerm<Pol>& _second) :
		mFirst(_first), mSecond(_second)
		{
		}

		bool operator==(const BVBinaryContent& _other) const
		{
			return mFirst == _other.mFirst && mSecond == _other.mSecond;
		}
		bool operator<(const BVBinaryContent& _other) const
		{
			if (!(mFirst == _other.mFirst)) return mFirst < _other.mFirst;
			if (!(mSecond == _other.mSecond)) return mSecond < _other.mSecond;
			return false;
		}
	};

	template<typename Pol>
	struct BVExtractContent
	{
		BVTerm<Pol> mOperand;
		size_t mFirst;
		size_t mLast;

		BVExtractContent(const BVTerm<Pol>& _operand, const size_t _first, const size_t _last) :
		mOperand(_operand), mFirst(_first), mLast(_last)
		{
		}

		bool operator==(const BVExtractContent& _other) const
		{
			return mOperand == _other.mOperand && mFirst == _other.mFirst && mLast == _other.mLast;
		}
		bool operator<(const BVExtractContent& _other) const
		{
			if (!(mOperand == _other.mOperand)) return mOperand < _other.mOperand;
			if (mFirst != _other.mFirst) return mFirst < _other.mFirst;
			if (mLast != _other.mLast) return mLast < _other.mLast;
			return false;
		}
	};

	// Forward declaration
	template<typename Element>
	class Pool;

	template<typename Pol>
	class BVTermContent
	{
		friend class Pool<BVTermContent<Pol>>;

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
		size_t mHash;

	public:

		BVTermContent() :
		mType(BVTermType::CONSTANT), mValue(), mWidth(0), mId(0), mHash(0)
		{
		}

		BVTermContent(BVTermType _type, BVValue _value) :
		mType(_type), mValue(_value), mWidth(_value.size()), mId(0),
		mHash((boost::hash_value(_value) << 5) ^ typeId(_type))
		{
			assert(_type == BVTermType::CONSTANT);
		}

		BVTermContent(BVTermType _type, const BVVariable& _variable) :
		mType(_type), mVariable(_variable), mWidth(_variable.width()), mId(0), mHash(0) //,
		// TODO: Hash - mHash(((size_t)_variable.getId() << 5) ^ typeId(_type))
		{
			assert(_type == BVTermType::VARIABLE);
		}

		BVTermContent(BVTermType _type, const Formula<Pol>& _booleanFormula, const BVTerm<Pol>& _subtermA, const BVTerm<Pol>& _subtermB) :
		mType(_type), mIte(_booleanFormula, _subtermA, _subtermB), mWidth(_subtermA.width()), mId(0),
		mHash((_booleanFormula.getHash() << 15) ^ (_subtermA.hash() << 10) ^ (_subtermB.hash() << 5) ^ typeId(_type))
		{
			assert(_type == BVTermType::ITE);
			assert(_subtermA.width() == _subtermB.width());
		}

		BVTermContent(BVTermType _type, const BVTerm<Pol>& _operand, const size_t _index = 0) :
		mType(_type), mUnary(_operand, _index), mWidth(0), mId(0),
		mHash((_index << 10) ^ (_operand.hash() << 5) ^ typeId(_type))
		{
			assert(typeIsUnary(_type));

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

		BVTermContent(BVTermType _type, const BVTerm<Pol>& _first, const BVTerm<Pol>& _second) :
		mType(_type), mBinary(_first, _second), mWidth(0), mId(0),
		mHash((_first.hash() << 10) ^ (_second.hash() << 5) ^ typeId(_type))
		{
			assert(typeIsBinary(_type));

			if(_type == BVTermType::CONCAT) {
				mWidth = _first.width() + _second.width();
			} else if(_type == BVTermType::EQ) {
				mWidth = 1;
			} else {
				mWidth = _first.width();
			}
		}

		BVTermContent(BVTermType _type, const BVTerm<Pol>& _operand, const size_t _first, const size_t _last) :
		mType(_type), mExtract(_operand, _first, _last), mWidth(_first - _last + 1), mId(0),
		mHash((_first << 15) ^ (_last << 10) ^ (_operand.hash() << 5) ^ typeId(_type))
		{
			assert(_type == BVTermType::EXTRACT);
			assert(_first < _operand.width() && _first >= _last && _last >= 0);
		}

		~BVTermContent()
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
				if(mWidth == 0) {
					return _init + "%invalid%";
				} else {
					string valueStr;
					boost::to_string(mValue, valueStr);
					return _init + "0b" + valueStr;
				}
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
				} else if(typeIsUnary(mType)) {
					argFirst = mUnary.mOperand.toString(_withActivity, _resolveUnequal, (_oneline ? "" : _init + "   "), _oneline, _infix, _friendlyNames);
				} else if(typeIsBinary(mType)) {
					argFirst = mBinary.mFirst.toString(_withActivity, _resolveUnequal, (_oneline ? "" : _init + "   "), _oneline, _infix, _friendlyNames);
					argSecond = mBinary.mSecond.toString(_withActivity, _resolveUnequal, (_oneline ? "" : _init + "   "), _oneline, _infix, _friendlyNames);
				} else {
					assert(false);
				}

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

		size_t hash() const
		{
			return this->mHash;
		}

		bool operator==(const BVTermContent<Pol>& _other) const
		{
			if(mId != 0 && _other.mId != 0) {
				return mId == _other.mId;
			}

			if(mType != _other.mType) {
				return false;
			}

			if(mType == BVTermType::CONSTANT) {
				return mValue == _other.mValue;
			} else if(mType == BVTermType::VARIABLE) {
				return mVariable == _other.mVariable;
			} else if(mType == BVTermType::ITE) {
				return mIte == _other.mIte;
			} else if(typeIsUnary(mType)) {
				return mUnary == _other.mUnary;
			} else if(typeIsBinary(mType)) {
				return mBinary == _other.mBinary;
			} else {
				assert(false);
				return false;
			}
		}
		bool operator<(const BVTermContent<Pol>& rhs) const {
			if(mId != 0 && rhs.mId != 0) return mId < rhs.mId;
			if(mType != rhs.mType) return mType < rhs.mType;

			if(mType == BVTermType::CONSTANT) {
				return mValue < rhs.mValue;
			} else if(mType == BVTermType::VARIABLE) {
				return mVariable < rhs.mVariable;
			} else if(mType == BVTermType::ITE) {
				return mIte < rhs.mIte;
			} else if(typeIsUnary(mType)) {
				return mUnary < rhs.mUnary;
			} else if(typeIsBinary(mType)) {
				return mBinary < rhs.mBinary;
			} else {
				assert(false);
				return false;
			}
		}

		/**
		 * The output operator of a term.
		 * @param _out The stream to print on.
		 * @param _term The term to be printed.
		 */
		template<typename P>
		friend std::ostream& operator<<(std::ostream& _out, const BVTermContent<P>& _term)
		{
			return(_out << _term.toString());
		}
	};

	// Forward declaration
	template<typename Pol>
	class BVTermPool;

	template<typename Pol>
	class BVTerm
	{
	private:
		const BVTermContent<Pol> * mpContent;

	public:

		BVTerm() :
		mpContent(BVTermPool<Pol>::getInstance().create())
		{
		}

		BVTerm(BVTermType _type, BVValue _value) :
		mpContent(BVTermPool<Pol>::getInstance().create(_type, _value))
		{
		}

		BVTerm(BVTermType _type, const BVVariable& _variable) :
		mpContent(BVTermPool<Pol>::getInstance().create(_type, _variable))
		{
		}

		BVTerm(BVTermType _type, const Formula<Pol>& _booleanFormula, const BVTerm<Pol>& _subtermA, const BVTerm<Pol>& _subtermB) :
		mpContent(BVTermPool<Pol>::getInstance().create(_type, _booleanFormula, _subtermA, _subtermB))
		{
		}

		BVTerm(BVTermType _type, const BVTerm<Pol>& _operand, const size_t _index = 0) :
		mpContent(BVTermPool<Pol>::getInstance().create(_type, _operand, _index))
		{
		}

		BVTerm(BVTermType _type, const BVTerm<Pol>& _first, const BVTerm<Pol>& _second) :
		mpContent(BVTermPool<Pol>::getInstance().create(_type, _first, _second))
		{
		}

		BVTerm(BVTermType _type, const BVTerm<Pol>& _operand, const size_t _first, const size_t _last) :
		mpContent(BVTermPool<Pol>::getInstance().create(_type, _operand, _first, _last))
		{
		}

		std::string toString(bool _withActivity = false, unsigned _resolveUnequal = 0, const std::string _init = "", bool _oneline = true, bool _infix = false, bool _friendlyNames = true) const
		{
			return mpContent->toString(_withActivity, _resolveUnequal, _init, _oneline, _infix, _friendlyNames);
		}

		template<typename P>
		friend std::ostream& operator<<(std::ostream& _out, const BVTerm<P>& _term)
		{
			return(_out << _term.toString());
		}

		size_t hash() const
		{
			return mpContent->hash();
		}

		size_t width() const
		{
			return mpContent->width();
		}

		BVTermType type() const
		{
			return mpContent->type();
		}

		bool operator==(const BVTerm<Pol>& _other) const
		{
			return mpContent == _other.mpContent;
		}
		bool operator<(const BVTerm<Pol>& rhs) const {
			return *(this->mpContent) < *(rhs.mpContent);
		}
	};
}

namespace std
{
	/**
	 * Implements std::hash for bit vector terms.
	 */
	template<typename Pol>
	struct hash<carl::BVTermContent<Pol>>
	{
		public:

		/**
		 * @param _formula The bit vector term to get the hash for.
		 * @return The hash of the given bit vector term.
		 */
		size_t operator()(const carl::BVTermContent<Pol>& _term) const
		{
			return _term.hash();
		}
	};
}
