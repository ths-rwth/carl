/**
 * @file UninterpretedFunction.cpp
 * @author Florian Corzilius <corzilius@cs.rwth-aachen.de>
 * @since 2014-10-30
 * @version 2014-10-30
 */

#include "UninterpretedFunction.h"

#include "../Sort.h"
#include "UFManager.h"

#include <iostream>
#include <vector>

namespace carl
{
    const std::string& UninterpretedFunction::name() const {
       return UFManager::getInstance().getName(*this);
    }

    const std::vector<Sort>& UninterpretedFunction::domain() const {
       return UFManager::getInstance().getDomain(*this);
    }

    const Sort& UninterpretedFunction::codomain() const {
       return UFManager::getInstance().getCodomain(*this);
    }

    std::ostream& operator<<(std::ostream& os, const UninterpretedFunction& ufun) {
        return UFManager::getInstance().print(os, ufun);
    }
}
