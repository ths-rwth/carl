/**
 * @file BVTerm.h
 * @author Andreas Krueger <andreas.krueger@rwth-aachen.de>
 */

#pragma once

#include <boost/variant.hpp>
#include "../../util/variant_util.h"

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
			default:
				CARL_LOG_ERROR("carl.formula.bv", "Tried to print an unknown value for BVTermType: " << unsigned(_type));
				return "unknown";
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
		friend BVTermPool;
	private:
		const BVTermContent * mpContent;

	public:

		BVTerm();

		BVTerm(BVTermType _type, BVValue _value);

		BVTerm(BVTermType _type, const BVVariable& _variable);

		BVTerm(BVTermType _type, const BVTerm& _operand, std::size_t _index = 0);

		BVTerm(BVTermType _type, const BVTerm& _first, const BVTerm& _second);

		BVTerm(BVTermType _type, const BVTerm& _operand, std::size_t _first, std::size_t _last);

		std::string toString(const std::string& _init = "", bool _oneline = true, bool _infix = false, bool _friendlyNames = true) const;

		friend std::ostream& operator<<(std::ostream& _out, const BVTerm& _term)
		{
			return _out << _term.toString();
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

		bool operator==(const BVTerm& rhs) const {
			return mpContent == rhs.mpContent;
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

	struct BVUnaryContent {
		BVTerm mOperand;
		std::size_t mIndex;

		explicit BVUnaryContent(BVTerm operand, std::size_t index = 0) :
			mOperand(operand), mIndex(index)
		{}

		bool operator==(const BVUnaryContent& rhs) const {
			return std::tie(mIndex,mOperand) == std::tie(rhs.mIndex,rhs.mOperand);
		}
		bool operator<(const BVUnaryContent& rhs) const {
			return std::tie(mOperand,mIndex) < std::tie(rhs.mOperand,rhs.mIndex);
		}
	};

	struct BVBinaryContent {
		BVTerm mFirst;
		BVTerm mSecond;

		BVBinaryContent(BVTerm first, BVTerm second):
			mFirst(first), mSecond(second)
		{}
		bool operator==(const BVBinaryContent& rhs) const {
			return std::tie(mFirst,mSecond) == std::tie(rhs.mFirst,rhs.mSecond);
		}
		bool operator<(const BVBinaryContent& rhs) const {
			return std::tie(mFirst,mSecond) < std::tie(rhs.mFirst,rhs.mSecond);
		}
	};

	struct BVExtractContent {
		BVTerm mOperand;
		std::size_t mHighest;
		std::size_t mLowest;

		BVExtractContent(BVTerm _operand, std::size_t _highest, std::size_t _lowest) :
		mOperand(_operand), mHighest(_highest), mLowest(_lowest)
		{}
		bool operator==(const BVExtractContent& rhs) const {
			return std::tie(mOperand,mHighest,mLowest) == std::tie(rhs.mOperand,rhs.mHighest,rhs.mLowest);
		}
		bool operator<(const BVExtractContent& rhs) const {
			return std::tie(mOperand,mHighest,mLowest) < std::tie(rhs.mOperand,rhs.mHighest,rhs.mLowest);
		}
	};

	class BVTermContent
	{
		friend class BVTermPool;
		friend class BVTerm;

	private:
		BVTermType mType = BVTermType::CONSTANT;
		
		boost::variant<BVVariable,BVValue,BVUnaryContent,BVBinaryContent,BVExtractContent> mContent = BVValue();
		
		template<typename T>
		T& getContent() {
			assert(carl::variant_is_type<T>(mContent));
			return boost::get<T>(mContent);
		}
		template<typename T>
		const T& getContent() const {
			assert(carl::variant_is_type<T>(mContent));
			return boost::get<T>(mContent);
		}
		
		BVVariable& getVariable() { return getContent<BVVariable>(); }
		const BVVariable& getVariable() const { return getContent<BVVariable>(); }
		BVValue& getValue() { return getContent<BVValue>(); }
		const BVValue& getValue() const { return getContent<BVValue>(); }
		BVUnaryContent& getUnary() { return getContent<BVUnaryContent>(); }
		const BVUnaryContent& getUnary() const { return getContent<BVUnaryContent>(); }
		BVBinaryContent& getBinary() { return getContent<BVBinaryContent>(); }
		const BVBinaryContent& getBinary() const { return getContent<BVBinaryContent>(); }
		BVExtractContent& getExtract() { return getContent<BVExtractContent>(); }
		const BVExtractContent& getExtract() const { return getContent<BVExtractContent>(); }

		std::size_t mWidth = 0;
		std::size_t mId = 0;
		std::size_t mHash;

	public:

		BVTermContent() :
		mHash(carl::variant_hash(mContent))
		{
		}

		BVTermContent(BVTermType _type, BVValue&& _value) :
			mType(_type), mContent(std::move(_value)), mWidth(getValue().width()),
			mHash(carl::variant_hash(mContent))
		{
			assert(_type == BVTermType::CONSTANT);
		}

		BVTermContent(BVTermType _type, const BVVariable& _variable) :
		mType(_type), mContent(_variable), mWidth(_variable.width()),
		mHash(carl::variant_hash(mContent))
		{
			assert(_type == BVTermType::VARIABLE);
		}

		BVTermContent(BVTermType _type, const BVTerm& _operand, const size_t _index = 0) :
			mType(_type), mContent(BVUnaryContent(_operand, _index)),
			mHash(carl::variant_hash(mContent))
		{
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
			mType(_type), mContent(BVBinaryContent(_first, _second)),
			mHash(carl::variant_hash(mContent))
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

		BVTermContent(BVTermType _type, const BVTerm& _operand, const size_t _highest, const size_t _lowest) :
			mType(_type), mContent(BVExtractContent(_operand, _highest, _lowest)), mWidth(_highest - _lowest + 1),
			mHash(carl::variant_hash(mContent))
		{
			assert(_type == BVTermType::EXTRACT);
			assert(_highest < _operand.width() && _highest >= _lowest);
		}

		std::size_t width() const {
			return mWidth;
		}

		BVTermType type() const {
			return mType;
		}

        bool isInvalid() const
        {
            return (mType == BVTermType::CONSTANT && mWidth == 0);
        }
		void collectVariables(std::set<BVVariable>& vars) const {
			if (mType == BVTermType::CONSTANT) {
			}
			else if (mType == BVTermType::VARIABLE) {
				vars.insert(getVariable());
			}
			else if (mType == BVTermType::EXTRACT) {
				getExtract().mOperand.collectVariables(vars);
			}
			else if (typeIsUnary(mType)) {
				getUnary().mOperand.collectVariables(vars);
			}
			else if (typeIsBinary(mType)) {
				getBinary().mFirst.collectVariables(vars);
				getBinary().mSecond.collectVariables(vars);
			}
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
		std::string toString(const std::string& _init = "", bool _oneline = true, bool _infix = false, bool _friendlyNames = true) const
		{
			if(isInvalid()) {
				return _init + "%invalid%";
			}
			else if (mType == BVTermType::CONSTANT) {
				return _init + getValue().toString();
			}
			else if (mType == BVTermType::VARIABLE) {
				return _init + getVariable().toString(_friendlyNames);
			}
			else {
				std::string operatorStr = carl::toString(mType);
				std::string operatorPrefix = operatorStr;
				std::string operatorInfix = operatorStr;

				std::string argFirst;
				std::string argSecond;
				std::string argThird;

				// Rewrite operator strings for indexed (parameterized) operators
				if(mType == BVTermType::EXTRACT) {
					operatorPrefix = "(_ " + operatorStr + " " + std::to_string(getExtract().mHighest) + " " + std::to_string(getExtract().mLowest) + ")";
					operatorInfix = operatorStr + "_{" + std::to_string(getExtract().mHighest) + "," + std::to_string(getExtract().mLowest) + "}";
				} else if(mType == BVTermType::LROTATE || mType == BVTermType::RROTATE
					|| mType == BVTermType::EXT_U || mType == BVTermType::EXT_S || mType == BVTermType::REPEAT) {
					operatorPrefix = "(_ " + operatorStr + " " + std::to_string(getUnary().mIndex) + ")";
					operatorInfix = operatorStr + "_" + std::to_string(getUnary().mIndex);
				}

				// Fill arg* variables
				if (mType == BVTermType::EXTRACT) {
					argFirst = getExtract().mOperand.toString((_oneline ? "" : _init + "   "), _oneline, _infix, _friendlyNames);
				}
				else if (typeIsUnary(mType)) {
					argFirst = getUnary().mOperand.toString((_oneline ? "" : _init + "   "), _oneline, _infix, _friendlyNames);
				}
				else if (typeIsBinary(mType)) {
					argFirst = getBinary().mFirst.toString((_oneline ? "" : _init + "   "), _oneline, _infix, _friendlyNames);
					argSecond = getBinary().mSecond.toString((_oneline ? "" : _init + "   "), _oneline, _infix, _friendlyNames);
				}
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
			return mHash;
		}

		bool operator==(const BVTermContent& rhs) const {
			if(mId != 0 && rhs.mId != 0) return mId == rhs.mId;
			return std::tie(mType, mHash, mWidth, mContent) == std::tie(rhs.mType, rhs.mHash, rhs.mWidth, rhs.mContent);
		}
		bool operator<(const BVTermContent& rhs) const {
			if(mId != 0 && rhs.mId != 0) return mId < rhs.mId;
			return std::tie(mType, mHash, mWidth, mContent) < std::tie(rhs.mType, rhs.mHash, rhs.mWidth, rhs.mContent);
		}

		/**
		 * The output operator of a term.
		 * @param _out The stream to print on.
		 * @param _term The term to be printed.
		 */
		friend std::ostream& operator<<(std::ostream& os, const BVTermContent& _term) {
			return os << _term.toString();
		}
	};
}

namespace std
{
	template<>
	struct hash<carl::BVUnaryContent>{
		std::size_t operator()(const carl::BVUnaryContent& uc) const {
			return carl::hash_all(uc.mOperand, uc.mIndex);
		}
	};
	template<>
	struct hash<carl::BVBinaryContent>{
		std::size_t operator()(const carl::BVBinaryContent& bc) const {
			return carl::hash_all(bc.mFirst, bc.mSecond);
		}
	};
	template<>
	struct hash<carl::BVExtractContent>{
		std::size_t operator()(const carl::BVExtractContent& ec) const {
			return carl::hash_all(ec.mOperand, ec.mHighest, ec.mLowest);
		}
	};
	
	/**
	 * Implements std::hash for bit vector term contents.
	 */
	template<>
	struct hash<carl::BVTermContent>{
		/**
		 * @param _termContent The bit vector term content to get the hash for.
		 * @return The hash of the given bit vector term content.
		 */
		std::size_t operator()(const carl::BVTermContent& tc) const {
			return tc.hash();
		}
	};

    /**
     * Implements std::hash for bit vector terms.
     */
    template<>
    struct hash<carl::BVTerm> {
        /**
         * @param _term The bit vector term to get the hash for.
         * @return The hash of the given bit vector term.
         */
        std::size_t operator()(const carl::BVTerm& t) const {
            return t.hash();
        }
    };
}    // namespace std
