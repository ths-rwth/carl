#pragma once

#include "../SetCover.h"
#include "../TypedSetCover.h"

namespace carl::covering::heuristic {

/**
 * Simple greedy heuristic:
 * Selects the largest remaining set until all elements are covered.
 */
Bitset greedy(SetCover& sc);

/**
 * Bounded greedy heuristic:
 * Selects the largest remaining set until at most bound constraints remain.
 */
Bitset greedy_bounded(SetCover& sc, std::size_t bound = 12);

/**
 * Weighted greedy heuristic:
 * Selects the largest remaining set according to the given weight function until at most bound constraints remain.
 */
Bitset greedy_weighted(SetCover& sc, const std::vector<double>& weights, std::size_t bound = 12);

/**
 * Weighted greedy heuristic:
 * Selects the largest remaining set according to the given weight function until at most bound constraints remain.
 */
template<typename T, typename F>
Bitset greedy_weighted(TypedSetCover<T>& tsc, F&& weight, std::size_t bound = 12) {
	std::vector<double> weights;
	for (std::size_t sid = 0; sid < tsc.set_cover().set_count(); ++sid) {
		weights.emplace_back(weight(tsc.get_set(sid)));
	}
	return greedy_weighted(tsc.set_cover(), weights, bound);
}

}