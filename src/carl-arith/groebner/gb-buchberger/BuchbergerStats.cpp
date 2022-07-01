


/*
 * @file   BuchbergerStats.cpp
 * @author Sebastian Junges
 *
 */

#include "BuchbergerStats.h"

namespace carl
{
BuchbergerStats* BuchbergerStats::instance = nullptr;

BuchbergerStats* BuchbergerStats::getInstance( )
{
    if( instance == nullptr )
        instance = new BuchbergerStats( );
    return instance;
}
}
