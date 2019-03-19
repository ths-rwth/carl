#pragma once

#include "SetCover.h"

namespace carl::covering {

/**
 * Represents a set cover problem.
 */
template<typename Set>
class TypedSetCover {
public:
	template<typename T>
	friend std::ostream& operator<<(std::ostream& os, const TypedSetCover<T>& tsc);
private:
	SetCover mSetCover;
	std::vector<Set> mSets;
	std::map<Set, std::size_t> mSetMap;

	std::size_t get_set_id(const Set& s) {
		auto it = mSetMap.try_emplace(s, mSetMap.size());
		if (it.second) {
			assert(it.first->second == mSets.size());
			mSets.emplace_back(s);
		}
		return it.first->second;
	}
public:
	void set(const Set& s, std::size_t element) {
		mSetCover.set(get_set_id(s), element);
	}
	void set(const Set& s, const Bitset& elements) {
		mSetCover.set(get_set_id(s), elements);
	}

	operator const SetCover&() const {
		return mSetCover;
	}

	template<typename F>
	std::vector<Set> get_cover(F&& heuristic) {
		std::vector<Set> res;
		for (std::size_t id: heuristic(mSetCover)) {
			res.emplace_back(mSets[id]);
		}
		return res;
	}
};

template<typename T>
std::ostream& operator<<(std::ostream& os, const TypedSetCover<T>& tsc) {
	return os << static_cast<const SetCover&>(tsc);
}

}