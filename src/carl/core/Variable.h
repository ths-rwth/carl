/**
 * @file Variable.h 
 * @author Sebastian Junges
 */

#pragma once

#include <iostream>
#include <climits>
#include <cassert>

#include "config.h"


namespace carl {

/// Type of an exponent.
typedef unsigned exponent;	
	
/**
 * Several types of variables are supported.
 * BOOL: the booleans
 * REAL: the reals
 * INT: the integers
 */
enum class VariableType : unsigned { VT_BOOL = 0, VT_REAL = 1, VT_INT = 2, VT_UNINTERPRETED = 3, VT_BITVECTOR = 4, MIN_TYPE = VT_BOOL, MAX_TYPE = VT_BITVECTOR, TYPE_SIZE = MAX_TYPE - MIN_TYPE + 1 };

/**
 * Streaming operator for VariableType.
 * @param os Output Stream.
 * @param t VariableType.
 * @return os.
 */
inline std::ostream& operator<<(std::ostream& os, const VariableType& t) {
	switch (t) {
		case VariableType::VT_BOOL: return os << "Bool";
		case VariableType::VT_REAL: return os << "Real";
		case VariableType::VT_INT: return os << "Int";
		case VariableType::VT_UNINTERPRETED: return os << "Uninterpreted";
		case VariableType::VT_BITVECTOR: return os << "Bitvector";
		default: return os << "Invalid " << unsigned(t);
	}
	return os << "Unknown";
}

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
private:
	/// Type if a variable is passed by reference.
	typedef const Variable& ByRef;
	/// Type if a variable is passed by value.
	typedef Variable ByValue;
public:
#ifdef VARIABLE_PASS_BY_VALUE
	/// Argument type for variables being function arguments.
	typedef VariableByValue Arg;
#else
	/// Argument type for variables being function arguments.
	typedef ByRef Arg;
#endif
 
private:
	/**
	 * The content of the variable.
	 * In order to keep a variable object small, this is the only data member.
	 * All other data (like names or alike) are stored in the VariablePool.
	 */
	std::size_t mContent;

public:
	
	/**
	 * Default constructor, constructing a variable, which is considered as not an actual variable.
	 * Such an invalid variable is stored in NO_VARIABLE, so use this if you need a default value for a variable.
	 */
	explicit Variable() : mContent(0) {} 
	
	/**
	 * Although we recommend access through the VariablePool, we allow public construction of variables for local purposes.
	 * However, please be aware that clashes may occur, as all variables with the same id are considered equal!
	 * @param id The identifier of the variable.
	 * @param type The type.
	 * @param rank The rank.
	 */
	explicit Variable(std::size_t id, VariableType type = VariableType::VT_REAL, std::size_t rank = 0):
		mContent((rank << (AVAILABLE + RESERVED_FOR_TYPE)) | (id << RESERVED_FOR_TYPE) | (unsigned)type)
	{
		assert(rank < (1 << RESERVED_FOR_RANK));
		assert(0 < id && id < ((std::size_t)1 << AVAILABLE));
		assert(VariableType::MIN_TYPE <= type && type <= VariableType::MAX_TYPE);
	}
	
	/**
	 * Retrieves the id of the variable.
	 * @return Variable id.
	 */
	std::size_t getId() const {
		return (mContent >> RESERVED_FOR_TYPE) % ((std::size_t)1 << AVAILABLE);
	}
	
	/**
	 * Retrieves the type of the variable.
	 * @return Variable type.
	 */
	VariableType getType() const {
		return (VariableType)(mContent % ((std::size_t)1 << RESERVED_FOR_TYPE));
	}
	
	/**
	 * Retrieves the rank of the variable.
	 * @return Variable rank.
	 */
	std::size_t getRank() const {
		return mContent >> (AVAILABLE + RESERVED_FOR_TYPE);
	}
	
	/**
	 * Streaming operator for Variable.
	 * @param os Output stream.
	 * @param rhs Variable.
	 * @return `os`
	 */
	friend std::ostream& operator<<(std::ostream& os, Variable::Arg rhs);
	
	/**
	 * Print the friendly name of the variable to the stream
     * @param os The stream where the variable name should be printed
     */
	void printFriendlyName(std::ostream& os = std::cout) const;
	
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
	friend bool operator==(Variable::Arg lhs, Variable::Arg rhs) {
		return lhs.mContent == rhs.mContent;
	}
	friend bool operator!=(Variable::Arg lhs, Variable::Arg rhs) {
		return lhs.mContent != rhs.mContent;
	}
	friend bool operator<(Variable::Arg lhs, Variable::Arg rhs) {
		return lhs.mContent > rhs.mContent;
	}
	friend bool operator<=(Variable::Arg lhs, Variable::Arg rhs) {
		return lhs.mContent >= rhs.mContent;
	}
	friend bool operator>(Variable::Arg lhs, Variable::Arg rhs) {
		return lhs.mContent < rhs.mContent;
	}
	friend bool operator>=(Variable::Arg lhs, Variable::Arg rhs) {
		return lhs.mContent <= rhs.mContent;
	}
	/// @}

	/// Number of bits available for the content.
	static constexpr unsigned BITSIZE = CHAR_BIT * sizeof(mContent);
	/// Number of bits reserved for the type.
	static constexpr unsigned RESERVED_FOR_TYPE = 3;
	/// Number of bits reserved for the rank.
	static constexpr unsigned RESERVED_FOR_RANK = 4;
	/// Overall number of bits reserved.
	static constexpr unsigned RESERVED = RESERVED_FOR_RANK + RESERVED_FOR_TYPE;
	static_assert(RESERVED < BITSIZE, "Too many bits reserved for special use.");
	/// Number of bits available for the id.
	static constexpr unsigned AVAILABLE = BITSIZE - RESERVED;

	/// Instance of an invalid variable.
	static const Variable NO_VARIABLE;
};

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
		size_t operator()(const carl::Variable& variable) const {
			return variable.getId();
		}
	};
} // namespace std






