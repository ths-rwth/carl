/**
 * @file SortValue.cpp
 * @author Florian Corzilius <corzilius@cs.rwth-aachen.de>
 * @since 2014-10-24
 * @version 2014-10-24
 */

#include "SortValue.h"
#include "SortValueManager.h"

namespace carl
{   
    bool SortValue::operator==( const SortValue& _sortValue ) const noexcept
    {
        assert( mSort == _sortValue.sort() );
        return mId == _sortValue.id();
    }

    bool SortValue::operator<( const SortValue& _sortValue ) const noexcept
    {
        assert( mSort == _sortValue.sort() );
        return mId < _sortValue.id();
    }
    
    std::ostream& operator<<( std::ostream& _out, const SortValue& _sortValue )
    {
        return (_out << _sortValue.sort() << "!val!" << _sortValue.id());
    }
}
