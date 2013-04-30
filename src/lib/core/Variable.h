/**
 * @file Variable.h 
 * @author Sebastian Junges
 */

#pragma once


namespace arithmetic
{
typedef unsigned variable;
static const unsigned VARIABLE_BITS_RESERVED_FOR_TYPE = 4;


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
 */
VariableType getVarType(variable v);
/**
 * Get the id of the variable, that is without the bits encoding extra information.
 * @param v
 * @return 
 */
unsigned getVarId(variable v);

}






