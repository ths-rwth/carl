/**
 * @file Variable.cpp
 * @author Sebastian Junges
 */

#include <limits.h>

#include "Variable.h"


namespace arithmetic
{

VariableType getVarType(variable v)
{
    return (VariableType)(v >> ((sizeof(variable) * CHAR_BIT) - VARIABLE_BITS_RESERVED_FOR_TYPE));
}

unsigned getVarId(variable v)
{
    unsigned mask = -1;
    return mask >> VARIABLE_BITS_RESERVED_FOR_TYPE & v;
}

std::string variableToString(variable v)
{
    return "x_" + v;
}


}