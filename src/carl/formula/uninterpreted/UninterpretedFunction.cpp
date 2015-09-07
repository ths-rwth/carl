/**
 * @file UninterpretedFunction.cpp
 * @author Florian Corzilius <corzilius@cs.rwth-aachen.de>
 * @since 2014-10-30
 * @version 2014-10-30
 */

#include "UninterpretedFunction.h"
#include "UFManager.h"

using namespace std;

namespace carl
{
    const string& UninterpretedFunction::name() const
    {
       return UFManager::getInstance().getName( *this );
    }

    const vector<Sort>& UninterpretedFunction::domain() const
    {
       return UFManager::getInstance().getDomain( *this );
    }

    const Sort& UninterpretedFunction::codomain() const
    {
       return UFManager::getInstance().getCodomain( *this );
    }
    
    ostream& operator<<( ostream& _os, const UninterpretedFunction& _ufun )
    {
        return UFManager::getInstance().print( _os, _ufun );
    }
}