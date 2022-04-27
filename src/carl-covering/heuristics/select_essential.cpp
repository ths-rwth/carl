#include "select_essential.h"

#include <carl-common/datastructures/Bitset.h>

namespace carl::covering::heuristic {

Bitset select_essential(SetCover& sc) {
	Bitset selected;
	bool has_selected = true;
	while (has_selected) {
		has_selected = false;
		for (std::size_t e = 0; e < sc.element_count(); ++e) {
			std::size_t num = 0;
			std::size_t set = 0;
			for (std::size_t s = 0; (s < sc.set_count()) && (num <= 1); ++s) {
				if (sc.get_set(s).test(e)) {
					++num;
					set = s;
				}
			}
			if (num == 1) {
				sc.select_set(set);
				selected.set(set);
				has_selected = true;
			}
		}
	}
	return selected;
}

}