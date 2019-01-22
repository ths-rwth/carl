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

	void UFInstance::gatherVariables(carlVariables& vars) const {
		for (const auto& a: args()) {
			a.gatherVariables(vars);
		}
	}
	void UFInstance::gatherUFs(std::set<UninterpretedFunction>& ufs) const {
		ufs.insert(uninterpretedFunction());
		for (const auto& a: args()) {
			a.gatherUFs(ufs);
		}
	}

	std::ostream& operator<<(std::ostream& os, const UFInstance& ufun) {
		assert(ufun.id() != 0);
		os << ufun.uninterpretedFunction().name() << "(";
		os << carl::stream_joined(", ", ufun.args());
		os << ")";
		return os;
	}
}
