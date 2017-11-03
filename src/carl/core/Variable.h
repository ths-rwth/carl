/**
 * @file Variable.h 
 * @author Sebastian Junges
 */

#pragma once

#include <cassert>
#include <climits>
#include <iostream>
#include <string>
#include <type_traits>

#include "config.h"
#include "logging.h"

namespace carl {
	
/**
 * Several types of variables are supported.
 * BOOL: the Booleans
 * REAL: the reals
 * INT: the integers
 * UNINTERPRETED: all uninterpreted types
 * BITVECTOR: bitvectors of any length
 */
enum class VariableType { VT_BOOL = 0, VT_REAL = 1, VT_INT = 2, VT_UNINTERPRETED = 3, VT_BITVECTOR = 4, MIN_TYPE = VT_BOOL, MAX_TYPE = VT_BITVECTOR, TYPE_SIZE = MAX_TYPE - MIN_TYPE + 1 };


/**
 * To string conversion for variable type
 * @param t  VariableType
 * @return  String Representation
 */
inline std::string to_string(const VariableType& t) {
	switch (t) {
		case VariableType::VT_BOOL:
			return "Bool";
		case VariableType::VT_REAL:
			return "Real";
		case VariableType::VT_INT:
			return "Int";
		case VariableType::VT_UNINTERPRETED:
			return "Uninterpreted";
		case VariableType::VT_BITVECTOR:
			return "Bitvector";
		default:
			return "Invalid " + std::to_string(static_cast<std::underlying_type_t<VariableType>>(t));
	}
}

/**
 *
 * @param in A string describing the variable type (e.g. produced by to_string)
 * @return the corresponding variable type
 */
VariableType variableTypeFromString(const std::string& in);

/**
 * Streaming operator for VariableType.
 * @param os Output Stream.
 * @param t VariableType.
 * @return os.
 */
inline std::ostream& operator<<(std::ostream& os, const VariableType& t) {
	return os << to_string(t);
}

class VariablePool;

/**
 * A Variable represents an algebraic variable that can be used throughout carl.
 *
 * Variables are basically bitvectors that contain `[rank | id | type]`, called *content*.
 * - The `id` is the identifier of this variable.
 * - The `type` is the variable type.
 * - The `rank` is zero be default, but can be used to create a custom variable ordering, as the comparison operators compare the whole content.
 * The `id` and the `type` together form a unique identifier for a variable.
 * If the VariablePool is used to construct variables (and we advise to do so), the id's will be consecutive starting with one for each variable type.
 * The `rank` is meant to change the variable order when passing a set of variables to another context, for example a function.
 * A single variable (identified by `id` and `type`) should not occur with two different `rank` values in the same context and hence such a comparison should never take place.
 *
 * A variable with id zero is considered invalid. It can be used as a default argument and can be compared to Variable::NO_VARIABLE.
 * Such a variable can only be constructed using the default constructor and its content will always be zero.
 *
 * Although not templated, we keep the whole class inlined for efficiency purposes.
 * Note that this way, any decent compiler removes the overhead introduced, while
 * having gained strong type-definitions and thus the ability to provide operator overloading.
 *
 * Moreover, notice that for small classes like this, pass-by-value could be faster than pass-by-ref.
 * However, this depends much on the capabilities of the compiler. 
 */
class Variable
{
	friend VariablePool;
	/// Type if a variable is passed by reference.
	using ByRef = const Variable&;
	/// Type if a variable is passed by value.
	using ByValue = Variable;
public:
#ifdef VARIABLE_PASS_BY_VALUE
	/// Argument type for variables being function arguments.
	using Arg = VariableByValue;
#else
	/// Argument type for variables being function arguments.
	using Arg = ByRef;
#endif
 
private:
	/**
	 * The content of the variable.
	 * In order to keep a variable object small, this is the only data member.
	 * All other data (like names or alike) are stored in the VariablePool.
	 */
	std::size_t mContent = 0;
	
	/**
	* Private constructor to be used by the VariablePool.
	* @param id The identifier of the variable.
	* @param type The type.
	* @param rank The rank.
	*/
	explicit Variable(std::size_t id, VariableType type = VariableType::VT_REAL, std::size_t rank = 0) noexcept :
		mContent((rank << (AVAILABLE + RESERVED_FOR_TYPE)) | (id << RESERVED_FOR_TYPE) | static_cast<std::size_t>(type))
	{
		assert(rank < (1 << RESERVED_FOR_RANK));
		assert(0 < id && id < (std::size_t(1) << AVAILABLE));
		assert(VariableType::MIN_TYPE <= type && type <= VariableType::MAX_TYPE);
	}

public:
	/**
	 * Default constructor, constructing a variable, which is considered as not an actual variable.
	 * Such an invalid variable is stored in NO_VARIABLE, so use this if you need a default value for a variable.
	 */
	constexpr Variable() = default;

	/**
	 * Retrieves the id of the variable.
	 * @return Variable id.
	 */
	constexpr std::size_t id() const noexcept {
		return (mContent >> RESERVED_FOR_TYPE) % (static_cast<std::size_t>(1) << AVAILABLE);
	}
	[[deprecated("use id() instead.")]]
	constexpr std::size_t getId() const noexcept {
		return id();
	}
	
	/**
	 * Retrieves the type of the variable.
	 * @return Variable type.
	 */
	constexpr VariableType type() const noexcept {
		return static_cast<VariableType>(mContent % (std::size_t(1) << RESERVED_FOR_TYPE));
	}
	[[deprecated("use type() instead.")]]
	constexpr VariableType getType() const noexcept {
		return type();
	}
	
	/**
	 * Retrieves the name of the variable.
	 * @return Variable name.
	 */
	std::string name() const;
	[[deprecated("use name() instead.")]]
	std::string getName() const {
		return name();
	}
	
	/**
	 * Retrieves the rank of the variable.
	 * @return Variable rank.
	 */
	constexpr std::size_t rank() const noexcept {
		return mContent >> (AVAILABLE + RESERVED_FOR_TYPE);
	}
	[[deprecated("use rank() instead.")]]
	constexpr std::size_t getRank() const noexcept {
		return rank();
	}
	
	/**
	 * Streaming operator for Variable.
	 * @param os Output stream.
	 * @param rhs Variable.
	 * @return `os`
	 */
	friend std::ostream& operator<<(std::ostream& os, Variable rhs) {
		return os << rhs.name();
	}
	
	/// @name Comparison operators
	/// @{
	/**
	 * Compares two variables.
	 *
	 * Note that for performance reasons, we compare the whole content of the variable (including the rank).
	 *
	 * Note that the variable order is not the order of the variable id. We consider variables greater, if they are defined earlier, i.e. if they have a smaller id.
	 * Hence, the variables order and the order of the variable ids are reversed.
	 * @param lhs First variable.
	 * @param rhs Second variable.
	 * @return `lhs ~ rhs`, `~` being the relation that is checked.
	 */
	friend bool operator==(Variable lhs, Variable rhs) noexcept {
		return lhs.mContent == rhs.mContent;
	}
	friend bool operator!=(Variable lhs, Variable rhs) noexcept {
		return lhs.mContent != rhs.mContent;
	}
	friend bool operator<(Variable lhs, Variable rhs) noexcept {
		return lhs.mContent < rhs.mContent;
	}
	friend bool operator<=(Variable lhs, Variable rhs) noexcept {
		return lhs.mContent <= rhs.mContent;
	}
	friend bool operator>(Variable lhs, Variable rhs) noexcept {
		return lhs.mContent > rhs.mContent;
	}
	friend bool operator>=(Variable lhs, Variable rhs) noexcept {
		return lhs.mContent >= rhs.mContent;
	}
	/// @}

	/// Number of bits available for the content.
	static constexpr std::size_t BITSIZE = CHAR_BIT * sizeof(std::size_t); //mContent has type size_t
	/// Number of bits reserved for the type.
	static constexpr std::size_t RESERVED_FOR_TYPE = 3;
	/// Number of bits reserved for the rank.
	static constexpr std::size_t RESERVED_FOR_RANK = 4;
	/// Overall number of bits reserved.
	static constexpr std::size_t RESERVED = RESERVED_FOR_RANK + RESERVED_FOR_TYPE;
	static_assert(RESERVED < BITSIZE, "Too many bits reserved for special use.");
	/// Number of bits available for the id.
	static constexpr std::size_t AVAILABLE = BITSIZE - RESERVED;

	/// Instance of an invalid variable.
	static const Variable NO_VARIABLE;
};
//static_assert(std::is_trivially_copyable<Variable>::value, "Variable should be trivially copyable.");
static_assert(std::is_literal_type<Variable>::value, "Variable should be a literal type.");

} // namespace carl

namespace std {
	/**
	 * Specialization of `std::hash` for Variable.
	 */
	template<>
	struct hash<carl::Variable> {
		/**
		 * Calculates the hash of a Variable.
		 * @param variable Variable.
		 * @return Hash of variable
		 */
		std::size_t operator()(carl::Variable variable) const noexcept {
			return variable.id();
		}
	};
} // namespace std
