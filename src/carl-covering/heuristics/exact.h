#pragma once

#include "../SetCover.h"
#include "../TypedSetCover.h"

namespace carl::covering::heuristic {

/**
 * Exact "heuristic":
 * Computes a minimum set cover.
 */
Bitset exact(SetCover& sc);

}