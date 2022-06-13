/**
 * @file CompareResult.h 
 * @author Sebastian Junges
 */

#pragma once

#include <ostream>

namespace carl
{
	enum class CompareResult { LESS = -1, EQUAL = 0, GREATER = 1 };
	
	inline std::ostream& operator<<(std::ostream& os, CompareResult cr) {
		switch (cr) {
			case CompareResult::LESS: return os << "LESS";
			case CompareResult::EQUAL: return os << "EQUAL";
			case CompareResult::GREATER: return os << "GREATER";
		}
	}
}
