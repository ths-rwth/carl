/*
 * GiNaCRA - GiNaC Real Algebra package
 * Copyright (C) 2010-2012  Ulrich Loup, Joachim Redies, Sebastian Junges
 *
 * This file is part of GiNaCRA.
 *
 * GiNaCRA is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GiNaCRA is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GiNaCRA.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


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
