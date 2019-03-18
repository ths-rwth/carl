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
    UFInstance UFInstanceManager::newUFInstance(std::unique_ptr<UFInstanceContent>&& ufic) {
        auto iter = mUFInstanceIdMap.find(ufic.get());
        // Check if this uninterpreted function content has already been created
        if(iter != mUFInstanceIdMap.end()) {
            return UFInstance(iter->second);
        }
        // Create the uninterpreted function instance
        mUFInstanceIdMap.emplace(ufic.get(), mUFInstances.size());
        UFInstance ufi(mUFInstances.size());
        mUFInstances.emplace_back(std::move(ufic));
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
