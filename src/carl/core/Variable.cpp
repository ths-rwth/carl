#include "Variable.h"
#include "VariablePool.h"

namespace carl
{
	const Variable Variable::NO_VARIABLE = Variable();
	
	std::string Variable::getName() const {
		return carl::VariablePool::getInstance().getName(*this, true);
	}
}
