/**
 * @file VarExpPair.cpp
 * @author Sebastian Junges
 */
#include "VarExpPair.h"
#include "Variable.h"
#include "VariablePool.h"

namespace arithmetic
{
    std::ostream& operator <<( std::ostream& os, const VarExpPair& rhs )
    {
        os << variableToString(rhs.var);
        if(rhs.exp > 1) os << "^" << rhs.exp;
        return os;
    }
}