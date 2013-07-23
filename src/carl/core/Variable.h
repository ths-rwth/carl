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
/**
 * Several types of variables are supported.
 * TODO: consider making this strongly typedefed (new in C++11). 
 * REAL: the reals RR
 * RATIONAL: rational numbers QQ
 * INT: the integers ZZ
 * NATURAL: nonnegative integers NN
 */
enum VariableType { VT_REAL = 0, VT_RATIONAL = 1, VT_INT = 2, VT_NATURAL = 3 };

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
class Monomial;

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
     * Although we recommend access through the VariablePool, we allow public construction of variables
     * for local purposes. However, please be aware that clashes may occur, as all variables with the 
     * same id are considered equal!
     * @param id The identifier of the variable
     * @param type The type (domain)
     */
    explicit Variable(unsigned id, VariableType type = VT_REAL) :
    mVariable((unsigned)type | id << VARIABLE_BITS_RESERVED_FOR_TYPE )
    {
        assert(id < sizeof(mVariable) * CHAR_BIT - BITS_RESERVED);
    }    
    
    unsigned getId() const
    {
        return (mVariable >> VARIABLE_BITS_RESERVED_FOR_TYPE);
    }
    
    VariableType getType() const
    {
        return (VariableType)(mVariable % (1 << VARIABLE_BITS_RESERVED_FOR_TYPE));
    }
    
    
    friend const Monomial operator*(const Variable& lhs, const Variable& rhs);
    
    friend std::ostream& operator<<(std::ostream& os, Variable::Arg rhs)
    {
        return os << "x_" << rhs.getId();
    }
    
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
};
}






