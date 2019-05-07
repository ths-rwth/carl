#include "exact.h"

#include "remove_duplicates.h"
#include "select_essential.h"

#include <carl/core/logging.h>

#include <optional>

namespace carl::covering::heuristic {

std::optional<Bitset> exact_of_size(const SetCover& sc, const Bitset& uncovered, const std::vector<std::size_t>& id_map, std::size_t size) {
	std::vector<bool> selection(id_map.size() - size, false);
	selection.resize(id_map.size(), true);
	
	do {
		Bitset covered;
		for (std::size_t id = 0; id < selection.size(); ++id) {
			if (selection[id]) {
				covered |= sc.get_set(id_map[id]);
			}
		}
		if (uncovered.is_subset_of(covered)) {
			Bitset result;
			for (std::size_t id = 0; id < selection.size(); ++id) {
				result.set(id, id_map[selection[id]]);
			}
			return result;
		}
	} while (std::next_permutation(selection.begin(), selection.end()));
	return std::nullopt;
}

Bitset exact(SetCover& sc) {
	Bitset pre;
	pre |= carl::covering::heuristic::remove_duplicates(sc);
	pre |= carl::covering::heuristic::select_essential(sc);

	const auto uncovered = sc.get_uncovered();

	// Maps local ids to ids in sc. We only consider active sets for local ids.
	std::vector<std::size_t> id_map;
	for (std::size_t sid = 0; sid < sc.set_count(); ++sid) {
		if (sc.get_set(sid).any()) id_map.emplace_back(sid);
	}

	for (std::size_t size = 0; size < sc.active_set_count(); ++size) {
		auto res = exact_of_size(sc, uncovered, id_map, size);
		if (res) {
			return pre | *res;
		}
	}

	CARL_LOG_ERROR("smtrat.mis", "Did not find an exact set cover.")
	return Bitset();
}

}