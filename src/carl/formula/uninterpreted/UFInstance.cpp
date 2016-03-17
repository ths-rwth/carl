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
    const UninterpretedFunction& UFInstance::uninterpretedFunction() const
    {
       return UFInstanceManager::getInstance().getUninterpretedFunction( *this );
    }

    const std::vector<UVariable>& UFInstance::args() const	
    {
       return UFInstanceManager::getInstance().getArgs( *this );
    }
    
    std::string UFInstance::toString( bool _infix, bool _friendlyNames ) const
    {
        std::stringstream ss;
        UFInstanceManager::getInstance().print( ss, *this, _infix, _friendlyNames );
        return ss.str();
    }
    
    std::ostream& operator<<( std::ostream& _os, const UFInstance& _ufun )
    {
        return UFInstanceManager::getInstance().print( _os, _ufun );
    }
}
