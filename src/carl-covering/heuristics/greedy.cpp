#include "greedy.h"

#include <carl/util/Bitset.h>

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

Bitset greedy_bounded(SetCover& sc, std::size_t bound) {
	Bitset result;
	Bitset uncovered = sc.get_uncovered();
	while (sc.active_set_count() > bound) {
		auto s = sc.largest_set();
		result.set(s);
		sc.select_set(s);
		sc.prune_sets();
	}
	return result;
}

Bitset greedy_weighted(SetCover& sc, const std::vector<double>& weights, std::size_t bound) {
	Bitset result;
	Bitset uncovered = sc.get_uncovered();
	while (sc.active_set_count() > bound) {
		auto s = sc.largest_set(weights);
		result.set(s);
		sc.select_set(s);
		sc.prune_sets();
	}
	return result;
}

}