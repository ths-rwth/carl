/**
 * @file Variable.h 
 * @author Sebastian Junges
 */

#pragma once

#include <iostream>
#include <climits>
#include <cassert>

#include "config.h"


namespace carl
{

/// Type of an exponent.
typedef unsigned exponent;	
	
/**
 * Several types of variables are supported.
 * REAL: the reals RR
 * RATIONAL: rational numbers QQ
 * INT: the integers ZZ
 * NATURAL: nonnegative integers NN
 */
enum class VariableType : unsigned { VT_REAL = 0, VT_RATIONAL = 1, VT_INT = 2, VT_NATURAL = 3, VT_BOOL = 4 };

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
 * Variables are bitvectors with  [ ordering | id | type]
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
	typedef const Variable& ByRef;
	typedef Variable ByValue;
public:
#ifdef VARIABLE_PASS_BY_VALUE
	typedef VariableByValue Arg;
#else
	typedef ByRef Arg;
#endif
 
private:
	unsigned mVariable;

public:
	
	/**
	 * Default constructor, constructing a variable, which is considered as not an actual variable.
	 * The resulting variable is store in NO_VARIABLE, so use this if you need a default value for a variable.
	 */
	explicit Variable() : mVariable(0) {} 
	
	/**
	 * Although we recommend access through the VariablePool, we allow public construction of variables
	 * for local purposes. However, please be aware that clashes may occur, as all variables with the 
	 * same id are considered equal!
	 * @param id The identifier of the variable
	 * @param type The type (domain)
	 */
	explicit Variable(unsigned id, VariableType type = VariableType::VT_REAL) :
	mVariable((unsigned)type | id << VARIABLE_BITS_RESERVED_FOR_TYPE )
	{
		assert( id > 0 );
		assert(id <  (1 << (sizeof(mVariable) * CHAR_BIT - BITS_RESERVED)));
	}	
	
	/**
	 * Retrieves the id of the variable.
	 * @return Variable id.
	 */
	unsigned getId() const
	{
		return (mVariable >> VARIABLE_BITS_RESERVED_FOR_TYPE);
	}
	
	/**
	 * Retrieves the type of the variable.
	 * @return Variable type.
	 */
	VariableType getType() const
	{
		return (VariableType)(mVariable % (1 << VARIABLE_BITS_RESERVED_FOR_TYPE));
	}
	
	friend std::ostream& operator<<(std::ostream& os, Variable::Arg rhs);
	
	/**
	 * Notice that for performance reasons, we consider the whole variable and not just the id.
	 * This implies that equal variables with a different ordering-pattern are considered not equal.
	 */
	friend bool operator==(Variable::Arg lhs, Variable::Arg rhs)
	{
		return lhs.mVariable == rhs.mVariable;
	}
	/**
	 * Notice that for performance reasons, we consider the whole variable and not just the id.
	 * This implies that equal variables with a different ordering-pattern are considered not equal.
	 */
	friend bool operator!=(Variable::Arg lhs, Variable::Arg rhs)
	{
		return lhs.mVariable != rhs.mVariable;
	}
	friend bool operator>(Variable::Arg lhs, Variable::Arg rhs)
	{
		return lhs.mVariable > rhs.mVariable;
	}
	friend bool operator<(Variable::Arg lhs, Variable::Arg rhs)
	{
		return lhs.mVariable < rhs.mVariable;
	}
	friend bool operator<=(Variable::Arg lhs, Variable::Arg rhs)
	{
		return lhs.mVariable <= rhs.mVariable;
	}
	friend bool operator>=(Variable::Arg lhs, Variable::Arg rhs)
	{
		return lhs.mVariable >= rhs.mVariable;
	}
	/// Number of bytes reserved for type encoding.
	static const unsigned VARIABLE_BITS_RESERVED_FOR_TYPE = 4;
	static const unsigned VARIABLE_BITS_RESERVED_FOR_ORDERING = 8;
	static constexpr unsigned BITS_RESERVED = VARIABLE_BITS_RESERVED_FOR_ORDERING + VARIABLE_BITS_RESERVED_FOR_TYPE;
	static_assert(BITS_RESERVED < 
			CHAR_BIT * sizeof(mVariable), "Too many bits reserved for special use.");
	static const Variable NO_VARIABLE;
};

std::string varToString(carl::Variable::Arg rhs, bool friendlyVarName = true);

} // namespace carl

namespace std
{
	template<>
	struct hash<carl::Variable>
	{
		size_t operator()(const carl::Variable& variable) const 
		{
			return variable.getId();
		}
	};
} // namespace std






