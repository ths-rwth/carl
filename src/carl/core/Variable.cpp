#include "Variable.h"
#include "VariablePool.h"

namespace carl
{
	const Variable Variable::NO_VARIABLE = Variable();
	
    void Variable::printFriendlyName(std::ostream& os) const {
        os << carl::VariablePool::getInstance().getName(*this, true);
    }
    
	std::ostream& operator<<(std::ostream& os, carl::Variable::Arg rhs)
	{
		#ifdef CARL_USE_FRIENDLY_VARNAMES
		rhs.printFriendlyName(os);
        return os;
		#else
		return os << "x_" << rhs.getId();
		#endif
	}
}
