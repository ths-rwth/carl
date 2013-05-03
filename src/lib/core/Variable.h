/**
 * @file Variable.h 
 * @author Sebastian Junges
 */

#pragma once

#include <string>

namespace arithmetic
{
/// Variables are bitvectors with  [ ordering | id | type]
typedef unsigned variable;
/// Number of bytes reserved for type encoding.
static const unsigned VARIABLE_BITS_RESERVED_FOR_TYPE = 4;
static const unsigned VARIABLE_BITS_RESERVED_FOR_ORDERING = 8;


/**
 * Several types of variables are supported. 
 * REAL: the reals RR
 * RATIONAL: rational numbers QQ
 * INT: the integers ZZ
 * NATURAL: nonnegative integers NN
 */
enum VariableType { VT_REAL = 0, VT_RATIONAL = 1, VT_INT = 2, VT_NATURAL = 3 };

/**
 * For a variable, determine the type.
 * @param v The variable.
 * @return The variable type in its enum. 
 */
VariableType getVarType(variable v);
/**
 * Get the id of the variable, that is without the bits encoding extra information.
 * @param v The variable
 * @return its id.
 */
unsigned getVarId(variable v);

std::string variableToString(variable v);
}






