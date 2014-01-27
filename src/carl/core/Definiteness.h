/** 
 * @file   Definiteness.h
 * @author Sebastian Junges
 * @author Florian Corzilius
 */

#pragma once

namespace carl
{
	/**
	 * Regarding a polynomial as a function, its definiteness gives information about the domain. 
	 */
    enum class Definiteness : unsigned { NEGATIVE = 0, NEGATIVE_SEMI = 1, NON = 2, POSITIVE_SEMI = 3, POSITIVE = 4 };	
}