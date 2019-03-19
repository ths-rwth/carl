#pragma once

#include "../SetCover.h"

#include <algorithm>

namespace carl::covering::heuristic {

inline auto remove_duplicates(SetCover& sc) {
	std::vector<Bitset> columns;
	for (std::size_t i = 0; i < sc.set_count(); ++i) {
		const auto& set = sc.get_set(i);
		while (columns.size() < set.size()) {
			columns.emplace_back();
		}
		for (std::size_t elem: set) {
			columns[elem].set(i);
		}
	}
	std::sort(columns.begin(), columns.end());
	columns.erase(std::unique(columns.begin(), columns.end()), columns.end());
	SetCover newsc;
	for (std::size_t i = 0; i < columns.size(); ++i) {
		for (std::size_t s: columns[i]) {
			newsc.set(s, i);
		}
	}
	sc = newsc;
	return Bitset();
}

}