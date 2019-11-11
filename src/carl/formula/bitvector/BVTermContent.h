#pragma once

#include "BVTerm.h"
#include "BVTermType.h"
#include "BVValue.h"
#include "BVVariable.h"

#include <tuple>
#include <variant>

namespace carl {

struct BVUnaryContent {
	BVTerm mOperand;
	std::size_t mIndex;

	explicit BVUnaryContent(BVTerm operand, std::size_t index = 0)
		: mOperand(operand), mIndex(index) {}

	bool operator==(const BVUnaryContent& rhs) const {
		return std::tie(mIndex, mOperand) == std::tie(rhs.mIndex, rhs.mOperand);
	}
	bool operator<(const BVUnaryContent& rhs) const {
		return std::tie(mOperand, mIndex) < std::tie(rhs.mOperand, rhs.mIndex);
	}
};

struct BVBinaryContent {
	BVTerm mFirst;
	BVTerm mSecond;

	BVBinaryContent(BVTerm first, BVTerm second)
		: mFirst(first), mSecond(second) {}
	bool operator==(const BVBinaryContent& rhs) const {
		return std::tie(mFirst, mSecond) == std::tie(rhs.mFirst, rhs.mSecond);
	}
	bool operator<(const BVBinaryContent& rhs) const {
		return std::tie(mFirst, mSecond) < std::tie(rhs.mFirst, rhs.mSecond);
	}
};

struct BVExtractContent {
	BVTerm mOperand;
	std::size_t mHighest;
	std::size_t mLowest;

	BVExtractContent(BVTerm _operand, std::size_t _highest, std::size_t _lowest)
		: mOperand(_operand), mHighest(_highest), mLowest(_lowest) {}
	bool operator==(const BVExtractContent& rhs) const {
		return std::tie(mOperand, mHighest, mLowest) == std::tie(rhs.mOperand, rhs.mHighest, rhs.mLowest);
	}
	bool operator<(const BVExtractContent& rhs) const {
		return std::tie(mOperand, mHighest, mLowest) < std::tie(rhs.mOperand, rhs.mHighest, rhs.mLowest);
	}
};
} // namespace carl

namespace std {
template<>
struct hash<carl::BVUnaryContent> {
	std::size_t operator()(const carl::BVUnaryContent& uc) const {
		return carl::hash_all(uc.mOperand, uc.mIndex);
	}
};
template<>
struct hash<carl::BVBinaryContent> {
	std::size_t operator()(const carl::BVBinaryContent& bc) const {
		return carl::hash_all(bc.mFirst, bc.mSecond);
	}
};
template<>
struct hash<carl::BVExtractContent> {
	std::size_t operator()(const carl::BVExtractContent& ec) const {
		return carl::hash_all(ec.mOperand, ec.mHighest, ec.mLowest);
	}
};
} // namespace std

namespace carl {

struct BVTermContent {
	using ContentType = std::variant<BVVariable, BVValue, BVUnaryContent, BVBinaryContent, BVExtractContent>;

	BVTermType mType = BVTermType::CONSTANT;
	ContentType mContent = BVValue();
	std::size_t mWidth = 0;
	std::size_t mId = 0;
	std::size_t mHash = 0;

	std::size_t computeHash() const {
		return std::hash<ContentType>()(mContent);
	}

	template<typename T>
	const T& as() const {
		assert(std::holds_alternative<T>(mContent));
		return std::get<T>(mContent);
	}

	BVTermContent()
		: mHash(computeHash()) {}

	BVTermContent(BVTermType type, BVValue&& value)
		: mType(type), mContent(std::move(value)),
		  mWidth(as<BVValue>().width()), mHash(computeHash()) {
		assert(type == BVTermType::CONSTANT);
	}

	BVTermContent(BVTermType type, const BVVariable& variable)
		: mType(type), mContent(variable), mWidth(variable.width()),
		  mHash(computeHash()) {
		assert(type == BVTermType::VARIABLE);
	}

	BVTermContent(BVTermType type, const BVTerm& _operand, std::size_t _index = 0)
		: mType(type), mContent(BVUnaryContent(_operand, _index)),
		  mHash(computeHash()) {
		assert(typeIsUnary(type));
		if (type == BVTermType::NOT || type == BVTermType::NEG) {
			assert(_index == 0);
			mWidth = _operand.width();
		} else if (type == BVTermType::LROTATE || type == BVTermType::RROTATE) {
			mWidth = _operand.width();
		} else if (type == BVTermType::REPEAT) {
			assert(_index > 0);
			mWidth = _operand.width() * _index;
		} else if (type == BVTermType::EXT_U || type == BVTermType::EXT_S) {
			mWidth = _operand.width() + _index;
		} else {
			assert(false);
		}
	}

	BVTermContent(BVTermType type, const BVTerm& _first, const BVTerm& _second)
		: mType(type), mContent(BVBinaryContent(_first, _second)),
		  mHash(computeHash()) {
		assert(typeIsBinary(type));

		if (type == BVTermType::CONCAT) {
			mWidth = _first.width() + _second.width();
		} else if (type == BVTermType::EQ) {
			mWidth = 1;
		} else {
			mWidth = _first.width();
		}
	}

	BVTermContent(BVTermType type, const BVTerm& _operand, std::size_t _highest, std::size_t _lowest)
		: mType(type), mContent(BVExtractContent(_operand, _highest, _lowest)), mWidth(_highest - _lowest + 1),
		  mHash(computeHash()) {
		assert(type == BVTermType::EXTRACT);
		assert(_highest < _operand.width() && _highest >= _lowest);
	}

	std::size_t id() const {
		return mId;
	}

	std::size_t width() const {
		return mWidth;
	}

	BVTermType type() const {
		return mType;
	}

	const auto& content() const {
		return mContent;
	}

	bool isInvalid() const {
		return (mType == BVTermType::CONSTANT && mWidth == 0);
	}
	void collectVariables(std::set<BVVariable>& vars) const {
		if (mType == BVTermType::CONSTANT) {
		} else if (mType == BVTermType::VARIABLE) {
			vars.insert(as<BVVariable>());
		} else if (mType == BVTermType::EXTRACT) {
			as<BVExtractContent>().mOperand.collectVariables(vars);
		} else if (typeIsUnary(mType)) {
			as<BVUnaryContent>().mOperand.collectVariables(vars);
		} else if (typeIsBinary(mType)) {
			as<BVBinaryContent>().mFirst.collectVariables(vars);
			as<BVBinaryContent>().mSecond.collectVariables(vars);
		} else {
			std::cerr << "Type is " << mType << std::endl;
			assert(false);
		}
	}

	void gatherVariables(carlVariables& vars) const {
		std::visit(overloaded {
			[&vars](const BVVariable& v){ vars.add(v.variable()); },
			[&vars](const BVExtractContent& c){ c.mOperand.gatherVariables(vars); },
			[&vars](const BVUnaryContent& c){ c.mOperand.gatherVariables(vars); },
			[&vars](const BVBinaryContent& c){
				c.mFirst.gatherVariables(vars);
				c.mSecond.gatherVariables(vars);
			},
			[](const auto&){}
		}, mContent);
	}

	std::size_t complexity() const {
		if (type() == BVTermType::CONSTANT) {
			return 1;
		} else if (type() == BVTermType::VARIABLE) {
			return width();
		} else if (type() == BVTermType::EXTRACT) {
			return as<BVExtractContent>().mHighest - as<BVExtractContent>().mLowest + as<BVExtractContent>().mOperand.complexity();
		} else if (typeIsUnary(type())) {
			return width() + as<BVUnaryContent>().mOperand.complexity();
		} else if (typeIsBinary(type())) {
			return width() + as<BVBinaryContent>().mFirst.complexity() + as<BVBinaryContent>().mSecond.complexity();
		}
		assert(false);
		return 0;
	}

	std::size_t hash() const {
		return mHash;
	}
};

inline bool operator==(const BVTermContent& lhs, const BVTermContent& rhs) {
	if (lhs.id() != 0 && rhs.id() != 0) return lhs.id() == rhs.id();
	return std::forward_as_tuple(lhs.type(), lhs.hash(), lhs.width(), lhs.content()) == std::forward_as_tuple(rhs.type(), rhs.hash(), rhs.width(), rhs.content());
}
inline bool operator<(const BVTermContent& lhs, const BVTermContent& rhs) {
	if (lhs.id() != 0 && rhs.id() != 0) return lhs.id() < rhs.id();
	return std::forward_as_tuple(lhs.type(), lhs.hash(), lhs.width(), lhs.content()) < std::forward_as_tuple(rhs.type(), rhs.hash(), rhs.width(), rhs.content());
}

/**
 * The output operator of a term.
 * @param os Output stream.
 * @param term Content of a bitvector term.
 */
inline std::ostream& operator<<(std::ostream& os, const BVTermContent& term) {
	if (term.isInvalid()) {
		return os << "%invalid%";
	}
	switch (term.type()) {
	case BVTermType::CONSTANT:
		return os << term.as<BVValue>();
	case BVTermType::VARIABLE:
		return os << term.as<BVVariable>();
	case BVTermType::EXTRACT: {
		const auto& ex = term.as<BVExtractContent>();
		return os << term.type() << "_{" << ex.mHighest << "," << ex.mLowest << "}(" << ex.mOperand << ")";
	}
	case BVTermType::LROTATE:
	case BVTermType::RROTATE:
	case BVTermType::EXT_U:
	case BVTermType::EXT_S:
	case BVTermType::REPEAT: {
		const auto& un = term.as<BVUnaryContent>();
		return os << term.type() << "_" << un.mIndex << "(" << un.mOperand << ")";
	}
	default:
		if (typeIsUnary(term.type())) {
			return os << term.type() << "(" << term.as<BVUnaryContent>().mOperand << ")";
		} else {
			const auto& bi = term.as<BVBinaryContent>();
			return os << term.type() << "(" << bi.mFirst << ", " << bi.mSecond << ")";
		}
	}
}
} // namespace carl

namespace std {

/**
	 * Implements std::hash for bit vector term contents.
	 */
template<>
struct hash<carl::BVTermContent> {
	/**
		 * @param tc The bit vector term content to get the hash for.
		 * @return The hash of the given bit vector term content.
		 */
	std::size_t operator()(const carl::BVTermContent& tc) const {
		return tc.hash();
	}
};
} // namespace std
