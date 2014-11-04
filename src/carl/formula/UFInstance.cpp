/**
 * @file UFInstance.cpp
 * @author Florian Corzilius <corzilius@cs.rwth-aachen.de>
 * @since 2014-10-30
 * @version 2014-10-30
 */

#include "UFInstance.h"
#include "UFInstanceManager.h"

using namespace std;

namespace carl
{
    const UninterpretedFunction& UFInstance::uninterpretedFunction() const
    {
       return UFInstanceManager::getInstance().getUninterpretedFunction( *this );
    }

    const vector<UVariable>& UFInstance::args() const
    {
       return UFInstanceManager::getInstance().getArgs( *this );
    }
    
    ostream& operator<<( ostream& _out, const UFInstance& _ufic )
    {
        return UFInstanceManager::getInstance().print( _out, _ufic );
    }
}