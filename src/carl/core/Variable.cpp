#include "Variable.h"
#include "VariablePool.h"

namespace carl
{
	const Variable Variable::NO_VARIABLE = Variable();
	
	std::string Variable::name() const {
		return carl::VariablePool::getInstance().getName(*this, true);
	}
	std::string Variable::safe_name() const {
		switch (type()) {
			case VariableType::VT_BOOL:
				return "b" + std::to_string(id());
			case VariableType::VT_REAL:
				return "r" + std::to_string(id());
			case VariableType::VT_INT:
				return "i" + std::to_string(id());
			case VariableType::VT_UNINTERPRETED:
				return "u" + std::to_string(id());
			case VariableType::VT_BITVECTOR:
				return "bv" + std::to_string(id());
			default:
				CARL_LOG_ERROR("carl", "Variable has invalid type: " << type());
				assert(false);
				return "";
		}
	}
}
