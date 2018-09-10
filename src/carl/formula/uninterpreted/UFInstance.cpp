/**
 * @file UFInstance.cpp
 * @author Florian Corzilius <corzilius@cs.rwth-aachen.de>
 * @since 2014-10-30
 * @version 2014-10-30
 */


#include "UFInstance.h"

#include "UFInstanceManager.h"
#include "UVariable.h"

#include <iostream>
#include <sstream>
#include <vector>

namespace carl
{
    const UninterpretedFunction& UFInstance::uninterpretedFunction() const {
       return UFInstanceManager::getInstance().getUninterpretedFunction(*this);
    }

    const std::vector<UTerm>& UFInstance::args() const {
       return UFInstanceManager::getInstance().getArgs(*this);
    }

    std::string UFInstance::toString(bool infix, bool friendlyNames) const {
        std::stringstream ss;
        UFInstanceManager::getInstance().print(ss, *this, infix, friendlyNames);
        return ss.str();
    }

    std::ostream& operator<<(std::ostream& os, const UFInstance& ufun) {
        return UFInstanceManager::getInstance().print(os, ufun);
    }
}
