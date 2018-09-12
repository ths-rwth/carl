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
    std::ostream& UFInstanceManager::print(std::ostream& out, const UFInstance& ufi, bool infix) const {
        assert(ufi.id() != 0);
        assert(ufi.id() < mUFInstances.size());
        const UFInstanceContent& ufic = *mUFInstances[ufi.id()];
        if(infix) {
            out << ufi.uninterpretedFunction().name() << "(";
        } else {
            out << "(" << ufic.uninterpretedFunction().name();
        }
        for(auto iter = ufi.args().begin(); iter != ufi.args().end(); ++iter) {
            if(infix) {
                if(iter != ufi.args().begin()) {
                    out << ", ";
                }
            } else {
                out << " ";
            }
            out << (&iter);//->toString( _friendlyNames );
        }
        out << ")";
        return out;
    }

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
