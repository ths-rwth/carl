#include "VarExpPair.h"

bool operator==(const carl::VarExpPair& lhs, const carl::VarExpPair& rhs)
{
    return lhs.var == rhs.var && lhs.exp  == rhs.exp;
}
bool operator!=(const carl::VarExpPair& lhs, const carl::VarExpPair& rhs)
{
    return lhs.var != rhs.var || lhs.exp  != rhs.exp;
}
// Operators checking only the variable.
bool operator==(const carl::VarExpPair& lhs, carl::Variable::Arg rhs)
{
    return lhs.var == rhs;
}
bool operator!=(const carl::VarExpPair& lhs, carl::Variable::Arg rhs)
{
    return lhs.var != rhs;
}
bool operator<=(const carl::VarExpPair& lhs, carl::Variable::Arg rhs)
{
    return lhs.var <= rhs;
}
bool operator>=(const carl::VarExpPair& lsh, carl::Variable::Arg rhs)
{
    return lsh.var >= rhs;
}
bool operator<(const carl::VarExpPair& lhs, carl::Variable::Arg rhs)
{
    return lhs.var < rhs;
}
bool operator>(const carl::VarExpPair& lsh, carl::Variable::Arg rhs)
{
    return lsh.var > rhs;
}

std::ostream& operator <<( std::ostream& os, const carl::VarExpPair& rhs )
{
    os << rhs.var;
    if(rhs.exp > 1) os << "^" << rhs.exp;
    return os;
}


