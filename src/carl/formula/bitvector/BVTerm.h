/**
 * @file BVTerm.h
 * @author Andreas Krueger <andreas.krueger@rwth-aachen.de>
 */

#pragma once

#include "BVVariable.h"
#include "BVValue.h"

namespace carl
{
	enum class BVTermType : unsigned
	{
		CONSTANT,
		VARIABLE,
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
			case BVTermType::ADD: return "bvadd";
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
	class BVTermContent;

	// Forward declaration
	class BVTermPool;

	class BVTerm
	{
	private:
		const BVTermContent * mpContent;

	public:

		BVTerm();

		BVTerm(BVTermType _type, BVValue _value);

		BVTerm(BVTermType _type, const BVVariable& _variable);

		BVTerm(BVTermType _type, const BVTerm& _operand, const size_t _index = 0);

		BVTerm(BVTermType _type, const BVTerm& _first, const BVTerm& _second);

		BVTerm(BVTermType _type, const BVTerm& _operand, const size_t _first, const size_t _last);

		std::string toString(const std::string _init = "", bool _oneline = true, bool _infix = false, bool _friendlyNames = true) const;

		friend std::ostream& operator<<(std::ostream& _out, const BVTerm& _term)
		{
			return(_out << _term.toString());
		}

		std::size_t hash() const;

		std::size_t width() const;

		BVTermType type() const;
		
		bool isConstant() const {
			return type() == BVTermType::CONSTANT;
		}
            
        /**
         * @return An approximation of the complexity of this bit vector term.
         */
        size_t complexity() const;
		
		void collectVariables(std::set<BVVariable>& vars) const;

		bool isInvalid() const;

		bool operator==(const BVTerm& _other) const
		{
			return mpContent == _other.mpContent;
		}
		bool operator<(const BVTerm& rhs) const;

		const BVTerm& operand() const;

		std::size_t index() const;

		const BVTerm& first() const;

		const BVTerm& second() const;

		std::size_t highest() const;

		std::size_t lowest() const;

		const BVVariable& variable() const;

		const BVValue& value() const;
		
		BVTerm substitute(const std::map<BVVariable,BVTerm>& /*unused*/) const;
	};

	struct BVUnaryContent
	{
		BVTerm mOperand;
		size_t mIndex;

		explicit BVUnaryContent(const BVTerm& _operand, const size_t _index = 0) :
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

	struct BVBinaryContent
	{
		BVTerm mFirst;
		BVTerm mSecond;

		BVBinaryContent(const BVTerm& _first, const BVTerm& _second) :
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

	struct BVExtractContent
	{
		BVTerm mOperand;
		std::size_t mHighest;
		std::size_t mLowest;

		BVExtractContent(const BVTerm& _operand, const size_t _highest, const size_t _lowest) :
		mOperand(_operand), mHighest(_highest), mLowest(_lowest)
		{
		}

		bool operator==(const BVExtractContent& _other) const
		{
			return mOperand == _other.mOperand && mHighest == _other.mHighest && mLowest == _other.mLowest;
		}
		bool operator<(const BVExtractContent& _other) const
		{
			if (!(mOperand == _other.mOperand)) return mOperand < _other.mOperand;
			if (mHighest != _other.mHighest) return mHighest < _other.mHighest;
			if (mLowest != _other.mLowest) return mLowest < _other.mLowest;
			return false;
		}
	};

	class BVTermContent
	{
		friend class BVTermPool;
		friend class BVTerm;

	private:
		BVTermType mType;

#ifdef __VS
		union
		{
			BVVariable* mpVariableVS;
			BVValue* mpValueVS;
			BVUnaryContent* mpUnaryVS;
			BVBinaryContent* mpBinaryVS;
			BVExtractContent* mpExtractVS;
		};
#else
		union
		{
			BVVariable mVariable;
			BVValue mValue;
			BVUnaryContent mUnary;
			BVBinaryContent mBinary;
			BVExtractContent mExtract;
		};
#endif
		std::size_t mWidth;
		std::size_t mId;
		std::size_t mHash;

	public:

		BVTermContent() :
#ifdef __VS
		mType(BVTermType::CONSTANT), mWidth(0), mId(0), mHash(0)
		{
			mpValueVS = new BVValue();
		}
#else
		mType(BVTermType::CONSTANT), mValue(), mWidth(0), mId(0), mHash(0)
		{
		}
#endif

		BVTermContent(BVTermType _type, BVValue _value) :
#ifdef __VS
		mType(_type), mWidth(_value.width()), mId(0),
		mHash((std::hash<BVValue>()(_value) << 5) ^ typeId(_type))
		{
			mpValueVS = new BVValue(_value);
			assert(_type == BVTermType::CONSTANT);
		}
#else
			mType(_type), mValue(_value), mWidth(_value.width()), mId(0),
			mHash((std::hash<BVValue>()(_value) << 5) ^ typeId(_type))
		{
			assert(_type == BVTermType::CONSTANT);
		}
#endif

		BVTermContent(BVTermType _type, const BVVariable& _variable) :
#ifdef __VS
		mType(_type), mWidth(_variable.width()), mId(0),
		mHash(((std::size_t)_variable().getId() << 5) ^ typeId(_type))
		{
			mpVariableVS = new BVVariable(_variable);
#else
		mType(_type), mVariable(_variable), mWidth(_variable.width()), mId(0),
		mHash((_variable().getId() << 5) ^ typeId(_type))
		{
#endif
			assert(_type == BVTermType::VARIABLE);
		}

		BVTermContent(BVTermType _type, const BVTerm& _operand, const size_t _index = 0) :
#ifdef __VS
			mType(_type), mWidth(0), mId(0),
		mHash((_index << 10) ^ (_operand.hash() << 5) ^ typeId(_type))
		{
			mpUnaryVS = new BVUnaryContent(_operand, _index);
#else
			mType(_type), mUnary(_operand, _index), mWidth(0), mId(0),
			mHash((_index << 10) ^ (_operand.hash() << 5) ^ typeId(_type))
		{
#endif
			assert(typeIsUnary(_type));
			if (_type == BVTermType::NOT || _type == BVTermType::NEG) {
				assert(_index == 0);
				mWidth = _operand.width();
			} else if(_type == BVTermType::LROTATE || _type == BVTermType::RROTATE) {
				mWidth = _operand.width();
			} else if(_type == BVTermType::REPEAT) {
				assert(_index > 0);
				mWidth = _operand.width() * _index;
			} else if(_type == BVTermType::EXT_U || _type == BVTermType::EXT_S) {
				mWidth = _operand.width() + _index;
			} else {
				assert(false);
			}
		}

		BVTermContent(BVTermType _type, const BVTerm& _first, const BVTerm& _second) :
#ifdef __VS
			mType(_type), mWidth(0), mId(0),
		mHash((_first.hash() << 10) ^ (_second.hash() << 5) ^ typeId(_type))
		{
			mpBinaryVS = new BVBinaryContent(_first, _second); 
#else
			mType(_type), mBinary(_first, _second), mWidth(0), mId(0),
			mHash((_first.hash() << 10) ^ (_second.hash() << 5) ^ typeId(_type))
		{
#endif
			assert(typeIsBinary(_type));

			if(_type == BVTermType::CONCAT) {
				mWidth = _first.width() + _second.width();
			} else if(_type == BVTermType::EQ) {
				mWidth = 1;
			} else {
				mWidth = _first.width();
			}
		}

		BVTermContent(BVTermType _type, const BVTerm& _operand, const size_t _highest, const size_t _lowest) :
#ifdef __VS
			mType(_type), mWidth(_highest - _lowest + 1), mId(0),
		mHash((_highest << 15) ^ (_lowest << 10) ^ (_operand.hash() << 5) ^ typeId(_type))
		{
			mpExtractVS = new BVExtractContent(_operand, _highest, _lowest);
#else
			mType(_type), mExtract(_operand, _highest, _lowest), mWidth(_highest - _lowest + 1), mId(0),
			mHash((_highest << 15) ^ (_lowest << 10) ^ (_operand.hash() << 5) ^ typeId(_type))
		{
#endif
			assert(_type == BVTermType::EXTRACT);
			assert(_highest < _operand.width() && _highest >= _lowest);
		}

		~BVTermContent()
		{
#ifdef __VS
			if(mType == BVTermType::VARIABLE) {
				mpVariableVS->~BVVariable();
			} else if(mType == BVTermType::CONSTANT) {
				mpValueVS->~BVValue();
			}
#else
			if (mType == BVTermType::VARIABLE) {
				mVariable.~BVVariable();
			}
			else if (mType == BVTermType::CONSTANT) {
				mValue.~BVValue();
			}
#endif
		}

		size_t width() const
		{
			return mWidth;
		}

		BVTermType type() const
		{
			return mType;
		}

        bool isInvalid() const
        {
            return (mType == BVTermType::CONSTANT && mWidth == 0);
        }
		void collectVariables(std::set<BVVariable>& vars) const {
#ifdef __VS
			if(mType == BVTermType::CONSTANT) {
			} else if(mType == BVTermType::VARIABLE) {
				vars.insert(*mpVariableVS);
			} else if(mType == BVTermType::EXTRACT) {
				mpExtractVS->mOperand.collectVariables(vars);
			} else if(typeIsUnary(mType)) {
				mpUnaryVS->mOperand.collectVariables(vars);
			}
			else if (typeIsBinary(mType)) {
				mpBinaryVS->mFirst.collectVariables(vars);
				mpBinaryVS->mSecond.collectVariables(vars);
			}
#else
			if (mType == BVTermType::CONSTANT) {
			}
			else if (mType == BVTermType::VARIABLE) {
				vars.insert(mVariable);
			}
			else if (mType == BVTermType::EXTRACT) {
				mExtract.mOperand.collectVariables(vars);
			}
			else if (typeIsUnary(mType)) {
				mUnary.mOperand.collectVariables(vars);
			}
			else if (typeIsBinary(mType)) {
				mBinary.mFirst.collectVariables(vars);
				mBinary.mSecond.collectVariables(vars);
			}
#endif
			else {
				std::cerr << "Type is " << mType << std::endl;
				assert(false);
			}
		}

		/**
		 * Gives the string representation of this bit vector term.
		 * @param _init The initial string of every row of the result.
		 * @param _oneline A flag indicating whether the term shall be printed on one line.
		 * @param _infix A flag indicating whether to print the term in infix or prefix notation.
		 * @param _friendlyNames A flag that indicates whether to print the variables with their internal representation (false)
		 *                        or with their dedicated names.
		 * @return The resulting string representation of this term.
		 */
		std::string toString(const std::string _init = "", bool _oneline = true, bool _infix = false, bool _friendlyNames = true) const
		{
            if(isInvalid()) {
                return _init + "%invalid%";
#ifdef __VS
            } else if(mType == BVTermType::CONSTANT) {
                return _init + mpValueVS->toString();
			}
			else if (mType == BVTermType::VARIABLE) {
				return _init + mpVariableVS->toString(_friendlyNames);
			}
#else
			}
			else if (mType == BVTermType::CONSTANT) {
				return _init + mValue.toString();
			}
			else if (mType == BVTermType::VARIABLE) {
				return _init + mVariable.toString(_friendlyNames);
			}
#endif
			else {
				std::string operatorStr = carl::toString(mType);
				std::string operatorPrefix = operatorStr;
				std::string operatorInfix = operatorStr;

				std::string argFirst;
				std::string argSecond;
				std::string argThird;

				// Rewrite operator strings for indexed (parameterized) operators
				if(mType == BVTermType::EXTRACT) {
#ifdef __VS
					operatorPrefix = "(_ " + operatorStr + " " + std::to_string(mpExtractVS->mHighest) + " " + std::to_string(mpExtractVS->mLowest) + ")";
					operatorInfix = operatorStr + "_{" + std::to_string(mpExtractVS->mHighest) + "," + std::to_string(mpExtractVS->mLowest) + "}";
#else
					operatorPrefix = "(_ " + operatorStr + " " + std::to_string(mExtract.mHighest) + " " + std::to_string(mExtract.mLowest) + ")";
					operatorInfix = operatorStr + "_{" + std::to_string(mExtract.mHighest) + "," + std::to_string(mExtract.mLowest) + "}";
#endif
				} else if(mType == BVTermType::LROTATE || mType == BVTermType::RROTATE
					|| mType == BVTermType::EXT_U || mType == BVTermType::EXT_S || mType == BVTermType::REPEAT) {
#ifdef __VS
					operatorPrefix = "(_ " + operatorStr + " " + std::to_string(mpUnaryVS->mIndex) + ")";
					operatorInfix = operatorStr + "_" + std::to_string(mpUnaryVS->mIndex);
#else
					operatorPrefix = "(_ " + operatorStr + " " + std::to_string(mUnary.mIndex) + ")";
					operatorInfix = operatorStr + "_" + std::to_string(mUnary.mIndex);
#endif
				}

				// Fill arg* variables
#ifdef __VS
				if(mType == BVTermType::EXTRACT) {
					argFirst = mpExtractVS->mOperand.toString((_oneline ? "" : _init + "   "), _oneline, _infix, _friendlyNames);
				} else if(typeIsUnary(mType)) {
					argFirst = mpUnaryVS->mOperand.toString((_oneline ? "" : _init + "   "), _oneline, _infix, _friendlyNames);
				} else if(typeIsBinary(mType)) {
					argFirst = mpBinaryVS->mFirst.toString((_oneline ? "" : _init + "   "), _oneline, _infix, _friendlyNames);
					argSecond = mpBinaryVS->mSecond.toString((_oneline ? "" : _init + "   "), _oneline, _infix, _friendlyNames);
				}
#else
				if (mType == BVTermType::EXTRACT) {
					argFirst = mExtract.mOperand.toString((_oneline ? "" : _init + "   "), _oneline, _infix, _friendlyNames);
				}
				else if (typeIsUnary(mType)) {
					argFirst = mUnary.mOperand.toString((_oneline ? "" : _init + "   "), _oneline, _infix, _friendlyNames);
				}
				else if (typeIsBinary(mType)) {
					argFirst = mBinary.mFirst.toString((_oneline ? "" : _init + "   "), _oneline, _infix, _friendlyNames);
					argSecond = mBinary.mSecond.toString((_oneline ? "" : _init + "   "), _oneline, _infix, _friendlyNames);
				}
#endif
				else {
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

		std::size_t hash() const
		{
			return this->mHash;
		}

		bool operator==(const BVTermContent& _other) const
		{
			if(mId != 0 && _other.mId != 0) {
				return mId == _other.mId;
			}

			if(mType != _other.mType) {
				return false;
			}

#ifdef __VS
			if(mType == BVTermType::CONSTANT) {
				return *mpValueVS == *_other.mpValueVS;
			} else if(mType == BVTermType::VARIABLE) {
				return *mpVariableVS == *_other.mpVariableVS;
			} else if(mType == BVTermType::EXTRACT) {
				return *mpExtractVS == *_other.mpExtractVS;
			} else if(typeIsUnary(mType)) {
				return *mpUnaryVS == *_other.mpUnaryVS;
			} else if(typeIsBinary(mType)) {
				return *mpBinaryVS == *_other.mpBinaryVS;
			}
#else
			if (mType == BVTermType::CONSTANT) {
				return mValue == _other.mValue;
			}
			else if (mType == BVTermType::VARIABLE) {
				return mVariable == _other.mVariable;
			}
			else if (mType == BVTermType::EXTRACT) {
				return mExtract == _other.mExtract;
			}
			else if (typeIsUnary(mType)) {
				return mUnary == _other.mUnary;
			}
			else if (typeIsBinary(mType)) {
				return mBinary == _other.mBinary;
			}
#endif
			else {
				std::cerr << "Type is " << mType << std::endl;
				assert(false);
				return false;
			}
		}
		bool operator<(const BVTermContent& rhs) const {
			if(mId != 0 && rhs.mId != 0) return mId < rhs.mId;
			if(mType != rhs.mType) return mType < rhs.mType;
#ifdef __VS
			if (mType == BVTermType::CONSTANT) {
				return *mpValueVS < *rhs.mpValueVS;
			}
			else if (mType == BVTermType::VARIABLE) {
				return *mpVariableVS < *rhs.mpVariableVS;
			}
			else if (mType == BVTermType::EXTRACT) {
				return *mpExtractVS < *rhs.mpExtractVS;
			}
			else if (typeIsUnary(mType)) {
				return *mpUnaryVS < *rhs.mpUnaryVS;
			}
			else if (typeIsBinary(mType)) {
				return *mpBinaryVS < *rhs.mpBinaryVS;
			}
#else
			if (mType == BVTermType::CONSTANT) {
				return mValue < rhs.mValue;
			}
			else if (mType == BVTermType::VARIABLE) {
				return mVariable < rhs.mVariable;
			}
			else if (mType == BVTermType::EXTRACT) {
				return mExtract < rhs.mExtract;
			}
			else if (typeIsUnary(mType)) {
				return mUnary < rhs.mUnary;
			}
			else if (typeIsBinary(mType)) {
				return mBinary < rhs.mBinary;
			}
#endif
			else {
				assert(false);
				return false;
			}
		}

		/**
		 * The output operator of a term.
		 * @param _out The stream to print on.
		 * @param _term The term to be printed.
		 */
		friend std::ostream& operator<<(std::ostream& _out, const BVTermContent& _term)
		{
			return(_out << _term.toString());
		}
	};
}

namespace std
{
	/**
	 * Implements std::hash for bit vector term contents.
	 */
	template <>
	struct hash<carl::BVTermContent>
	{
		public:

		/**
		 * @param _termContent The bit vector term content to get the hash for.
		 * @return The hash of the given bit vector term content.
		 */
		size_t operator()(const carl::BVTermContent& _termContent) const
		{
			return _termContent.hash();
		}
	};

    /**
     * Implements std::hash for bit vector terms.
     */
    template <>
    struct hash<carl::BVTerm>
    {
        public:

        /**
         * @param _term The bit vector term to get the hash for.
         * @return The hash of the given bit vector term.
         */
        size_t operator()(const carl::BVTerm& _term) const
        {
            return _term.hash();
        }
    };
}    // namespace std
