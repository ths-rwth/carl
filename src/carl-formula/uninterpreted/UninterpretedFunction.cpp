/**
 * @file UninterpretedFunction.cpp
 * @author Florian Corzilius <corzilius@cs.rwth-aachen.de>
 * @since 2014-10-30
 * @version 2014-10-30
 */

#include "UninterpretedFunction.h"

#include "../sort/Sort.h"
#include "UFManager.h"

#include <iostream>
#include <vector>

namespace carl
{
	const std::string& UninterpretedFunction::name() const {
		return UFManager::getInstance().get_name(*this);
	}

	const std::vector<Sort>& UninterpretedFunction::domain() const {
		return UFManager::getInstance().getDomain(*this);
	}

	Sort UninterpretedFunction::codomain() const {
		return UFManager::getInstance().getCodomain(*this);
	}
}
