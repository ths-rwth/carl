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
 * REAL: the reals RR
 * RATIONAL: rational numbers QQ
 * INT: the integers ZZ
 * NATURAL: nonnegative integers NN
 */
enum class VariableType : unsigned { VT_REAL = 0, VT_RATIONAL = 1, VT_INT = 2, VT_NATURAL = 3, VT_BOOL = 4, MIN_TYPE = 0, MAX_TYPE = 4 };

/**
 * Streaming operator for VariableType.
 * @param os Output Stream.
 * @param t VariableType.
 * @return os.
 */
inline std::ostream& operator<<(std::ostream& os, const VariableType& t) {
	switch (t) {
		case VariableType::VT_BOOL: return os << "Bool";
		case VariableType::VT_INT: return os << "Int";
		case VariableType::VT_NATURAL: return os << "Natural";
		case VariableType::VT_RATIONAL: return os << "Rational";
		case VariableType::VT_REAL: return os << "Real";
	}
	return os << "Unknown";
}

/**
 * A Variable represents an algebraic variable that can be used throughout carl.
 *
 * Variables are basically bitvectors that contain `[rank | id | type]`, called *content*.
 * - The `id` is the actual identifier of this variable, unique throughout the program state.
 * - The `type` is the variable type. Although there can be multiple instances with the same id but different types, this shall be avoided.
 * - The `rank` is zero be default, but can be used to create a custom variable ordering, as the comparison operators compare the whole content.
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
	unsigned mContent;

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
	explicit Variable(unsigned id, VariableType type = VariableType::VT_REAL, unsigned rank = 0):
		mContent((rank << (AVAILABLE + RESERVED_FOR_TYPE)) | (id << RESERVED_FOR_TYPE) | (unsigned)type)
	{
		assert(rank < (1 << RESERVED_FOR_RANK));
		assert(0 < id && id < (1 << AVAILABLE));
		assert(VariableType::MIN_TYPE <= type && type <= VariableType::MAX_TYPE);
	}
	
	/**
	 * Retrieves the id of the variable.
	 * @return Variable id.
	 */
	unsigned getId() const {
		return (mContent >> RESERVED_FOR_TYPE) % (1 << AVAILABLE);
	}
	
	/**
	 * Retrieves the type of the variable.
	 * @return Variable type.
	 */
	VariableType getType() const {
		return (VariableType)(mContent % (1 << RESERVED_FOR_TYPE));
	}
	
	/**
	 * Retrieves the rank of the variable.
	 * @return Variable rank.
	 */
	unsigned getRank() const {
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
	 * Note that for performance reasons, we compare the not only the id but the whole content of the variable (including type and rank).
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
	friend bool operator>(Variable::Arg lhs, Variable::Arg rhs) {
		return lhs.mContent > rhs.mContent;
	}
	friend bool operator<(Variable::Arg lhs, Variable::Arg rhs) {
		return lhs.mContent < rhs.mContent;
	}
	friend bool operator<=(Variable::Arg lhs, Variable::Arg rhs) {
		return lhs.mContent <= rhs.mContent;
	}
	friend bool operator>=(Variable::Arg lhs, Variable::Arg rhs) {
		return lhs.mContent >= rhs.mContent;
	}
	/// @}

	/// Number of bits available for the content.
	static constexpr unsigned BITSIZE = CHAR_BIT * sizeof(mContent);
	/// Number of bits reserved for the type.
	static constexpr unsigned RESERVED_FOR_TYPE = 4;
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






