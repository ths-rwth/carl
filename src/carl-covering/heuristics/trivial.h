#pragma once

#include "../SetCover.h"

namespace carl::covering::heuristic {

/// Trivial heuristic: select all sets.
Bitset trivial(SetCover& sc);

}