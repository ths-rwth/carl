#include "greedy.h"

namespace carl::covering::heuristic {

Bitset greedy(SetCover& sc) {
	Bitset result;
	Bitset uncovered = sc.get_uncovered();
	while (uncovered.any()) {
		auto s = sc.largest_set();
		result.set(s);
		uncovered -= sc.get_set(s);
		sc.select_set(s);
	}
	return result;
}

}