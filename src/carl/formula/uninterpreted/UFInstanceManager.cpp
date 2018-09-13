/**
 * @file UFInstanceManager.cpp
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 * @author Florian Corzilius <corzilius@cs.rwth-aachen.de>
 * @since 2014-10-30
 * @version 2014-10-30
 */

#include "UFInstanceManager.h"

namespace carl
{
    UFInstance UFInstanceManager::newUFInstance(const UFInstanceContent* ufic) {
        auto iter = mUFInstanceIdMap.find(ufic);
        // Check if this uninterpreted function content has already been created
        if(iter != mUFInstanceIdMap.end()) {
            delete ufic;
            return UFInstance(iter->second);
        }
        // Create the uninterpreted function instance
        mUFInstanceIdMap.emplace(ufic, mUFInstances.size());
        UFInstance ufi(mUFInstances.size());
        mUFInstances.push_back(ufic);
        return ufi;
    }

    bool UFInstanceManager::argsCorrect(const UFInstanceContent& ufic) {
        if(!(ufic.uninterpretedFunction().domain().size() == ufic.args().size())) {
            return false;
        }
        for(std::size_t i = 0; i < ufic.uninterpretedFunction().domain().size(); ++i) {
            if(!(ufic.uninterpretedFunction().domain().at(i) == ufic.args().at(i).domain())) {
                return false;
            }
        }
        return true;
    }
}
