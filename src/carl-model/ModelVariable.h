#pragma once

#include <carl/core/Variable.h>
#include <carl/formula/bitvector/BVVariable.h>
#include <carl/formula/uninterpreted/UVariable.h>
#include <carl/formula/uninterpreted/UninterpretedFunction.h>
#include <carl/util/variant_util.h>

#include <cassert>
#include <variant>

namespace carl {

/**
 * Represent a sum type/variant over the different kinds of variables that
 * exist in CARL to use them in a more uniform way,
 * e.g. an (algebraic) "carl::Variable", an (uninterpreted) "carl::UVariable",
 * an "carl::UninterpretedFunction" etc.
 */
class ModelVariable {
	/**
	 * Base type for the content.
	 */
	using Base = std::variant<Variable, BVVariable, UVariable, UninterpretedFunction>;

	Base mData;

public:
	friend bool operator==(const ModelVariable& lhs, const ModelVariable& rhs);
	friend bool operator<(const ModelVariable& lhs, const ModelVariable& rhs);
	friend std::ostream& operator<<(std::ostream& os, const ModelVariable& mv);
	friend std::hash<ModelVariable>;

	/**
	 * Initialize the ModelVariable from some valid type of the underlying variant.
	 */
	template<typename T, typename T2 = typename std::enable_if<convertible_to_variant<T, Base>::value, T>::type>
	ModelVariable(const T& _t)
		: mData(_t) {}

	/**
	 * @return true, if the stored value is a variable.
	 */
	bool isVariable() const {
		return std::holds_alternative<carl::Variable>(mData);
	}

	/**
	 * @return true, if the stored value is a bitvector variable.
	 */
	bool isBVVariable() const {
		return std::holds_alternative<carl::BVVariable>(mData);
	}

	/**
	 * @return true, if the stored value is an uninterpreted variable.
	 */
	bool isUVariable() const {
		return std::holds_alternative<carl::UVariable>(mData);
	}

	/**
	 * @return true, if the stored value is a function.
	 */
	bool isFunction() const {
		return std::holds_alternative<carl::UninterpretedFunction>(mData);
	}

	/**
	 * @return The stored value as a variable.
	 */
	carl::Variable asVariable() const {
		assert(isVariable());
		return std::get<carl::Variable>(mData);
	}

	/**
	 * @return The stored value as a bitvector variable.
	 */
	const carl::BVVariable& asBVVariable() const {
		assert(isBVVariable());
		return std::get<carl::BVVariable>(mData);
	}

	/**
	 * @return The stored value as an uninterpreted variable.
	 */
	const carl::UVariable& asUVariable() const {
		assert(isUVariable());
		return std::get<carl::UVariable>(mData);
	}

	/**
	 * @return The stored value as a function.
	 */
	const carl::UninterpretedFunction& asFunction() const {
		assert(isFunction());
		return std::get<carl::UninterpretedFunction>(mData);
	}
};

/**
 * Return true if lhs is equal to rhs.
 */
inline bool operator==(const ModelVariable& lhs, const ModelVariable& rhs) {
	return lhs.mData == rhs.mData;
}

/**
 * Return true if lhs is smaller than rhs.
 */
inline bool operator<(const ModelVariable& lhs, const ModelVariable& rhs) {
	return lhs.mData < rhs.mData;
}

inline std::ostream& operator<<(std::ostream& os, const ModelVariable& mv) {
	return os << mv.mData;
}
} // namespace carl

namespace std {
template<>
struct hash<carl::ModelVariable> {
	std::size_t operator()(const carl::ModelVariable& mv) const {
		return std::hash<carl::ModelVariable::Base>()(mv.mData);
	}
};
} // namespace std
