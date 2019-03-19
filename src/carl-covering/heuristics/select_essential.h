#pragma once

#include "../SetCover.h"

namespace carl::covering::heuristic {

/**
 * Preprocessing heuristic:
 * Selects essential sets which are the only once covering some element.
 */
Bitset select_essential(SetCover& sc);

}