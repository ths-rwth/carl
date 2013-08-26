/**
 * @file CompareResult.h 
 * @author Sebastian Junges
 */

#pragma once

namespace carl
{
enum class CompareResult : int { LESS = -1, EQUAL = 0, GREATER = 1 };
}