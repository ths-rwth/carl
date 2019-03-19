#pragma once

#include "../SetCover.h"

namespace carl::covering::heuristic {

inline auto trivial(SetCover& sc) {
	Bitset result;
	result.set_interval(0, sc.set_count());
	return result;
}

}