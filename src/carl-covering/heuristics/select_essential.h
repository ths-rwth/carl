#pragma once

#include "../SetCover.h"

#include <carl/core/logging.h>

#include <algorithm>

namespace carl::covering::heuristic {

inline auto select_essential(SetCover& sc) {
	Bitset selected;
	for (std::size_t e = 0; e < sc.element_count(); ++e) {
		std::size_t num = 0;
		std::size_t set = 0;
		for (std::size_t s = 0; s < sc.set_count() && num <= 1; ++s) {
			if (sc.get_set(s).test(e)) {
				++num;
				set = s;
			}
		}
		if (num == 1) {
			sc.select_set(set);
			selected.set(set);
		}
	}
	return selected;
}

}