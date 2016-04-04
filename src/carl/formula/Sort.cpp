/**
 * @file Sort.cpp
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 * @author Florian Corzilius <corzilius@cs.rwth-aachen.de>
 * @since 2014-10-30
 * @version 2014-10-30
 */

#include "Sort.h"

#include "SortManager.h"

namespace carl
{
    size_t Sort::arity() const
    {
        return SortManager::getInstance().getArity( *this );
    }
    
    bool Sort::operator==( const Sort& _sort ) const
    {
        return mId == _sort.id();
    }

    bool Sort::operator<( const Sort& _sort ) const
    {
        return mId < _sort.id();
    }
    
    std::ostream& operator<<( std::ostream& _os, const Sort& _sort )
    {
        return SortManager::getInstance().print(_os, _sort);
    }
}
