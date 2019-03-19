#pragma once

#include "../SetCover.h"

namespace carl::covering::heuristic {

/**
 * Simple greedy heuristic:
 * Selects the largest remaining set until all elements are covered.
 */
Bitset greedy(SetCover& sc);

}