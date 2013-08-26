#include "Variable.h"
#include "VariablePool.h"

namespace carl
{
std::ostream& operator<<(std::ostream& os, carl::Variable::Arg rhs)
{
    #ifdef CARL_USE_FRIENDLY_VARNAMES
    return os << carl::VariablePool::getFriendlyName(rhs);
    #else
    return os << "x_" << rhs.getId();
    #endif
}
}
