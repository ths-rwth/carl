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
    return (VariableType)(v % (1 << VARIABLE_BITS_RESERVED_FOR_TYPE));
}

unsigned getVarId(variable v)
{
    return (v >> VARIABLE_BITS_RESERVED_FOR_TYPE);
}

std::string variableToString(variable v)
{
    return "x_" + v;
}


}