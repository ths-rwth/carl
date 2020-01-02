#include "trivial.h"

#include <carl/util/Bitset.h>

namespace carl::covering::heuristic {

/// Trivial heuristic: select all sets.
Bitset trivial(SetCover& sc) {
	Bitset result;
	result.set_interval(0, sc.set_count() - 1);
	return result;
}

}