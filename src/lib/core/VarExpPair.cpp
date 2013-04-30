/**
 * @file VarExpPair.cpp
 * @author Sebastian Junges
 */
#include "VarExpPair.h"
#include "Variable.h"

namespace arithmetic
{
    std::ostream& operator <<( std::ostream& os, const VarExpPair& rhs )
    {
        return os << getVarId(rhs.var) << "^" << rhs.exp;
    }
}