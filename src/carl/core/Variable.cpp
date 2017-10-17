#include "Variable.h"
#include "VariablePool.h"

#include <stdexcept>

namespace carl
{
	const Variable Variable::NO_VARIABLE = Variable();
	
	std::string Variable::getName() const {
		return carl::VariablePool::getInstance().getName(*this, true);
	}

	VariableType variableTypeFromString(const std::string& in) {
		if(in == "Real") {
			return VariableType::VT_REAL;
		} else if(in == "Int") {
			return VariableType::VT_INT;
		} else if(in == "Bool") {
			return VariableType::VT_BOOL;
		} else if(in == "Bitvector") {
			return VariableType ::VT_BITVECTOR;
		} else if(in == "Uninterpreted") {
			return VariableType::VT_UNINTERPRETED;
		} else {
			assert(false);
			CARL_LOG_ERROR("carl.variable", "Input " << in << " is not a variable type.");
			return VariableType::VT_REAL;
		}

	}
}
