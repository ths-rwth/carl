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
#include <numeric>
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

	std::size_t UFInstance::complexity() const {
		const auto& a = args();
		return std::accumulate(
			a.begin(), a.end(), static_cast<std::size_t>(1),
			[](std::size_t c, const auto& term){
				return c + term.complexity();
			}
		);
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
