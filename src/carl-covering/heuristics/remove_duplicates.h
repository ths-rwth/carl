#pragma once

#include "../SetCover.h"

namespace carl::covering::heuristic {

/**
 * Preprocessing heuristic:
 * Compresses the matrix by removing duplicate columns.
 * The order of the columns changes!
 */
Bitset remove_duplicates(SetCover& sc);

}