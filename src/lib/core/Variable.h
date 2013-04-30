/**
 * @file Variable.h 
 * @author Sebastian Junges
 */

#pragma once


namespace arithmetic
{
typedef size_t variable;

/**
 * Several types of variables are supported. 
 * REAL: the reals RR
 * RATIONAL: rational numbers QQ
 * INT: the integers ZZ
 * NATURAL: nonnegative integers NN
 */
enum VariableType { VT_REAL, VT_RATIONAL, VT_INT, VT_NATURAL };

/**
 * For a variable, determine the type.
 */
VariableType getType(variable v);

}







