#pragma once

#include <carl/util/Bitset.h>

#include <numeric>
#include <vector>

namespace carl::covering {

/**
 * Represents a set cover problem.
 */
class SetCover {
public:
	friend std::ostream& operator<<(std::ostream& os, const SetCover& sc);
private:
	std::vector<carl::Bitset> mSets;
public:
	std::size_t set_count() const {
		return mSets.size();
	}
	void set(std::size_t set, std::size_t element) {
		while (set >= mSets.size()) {
			mSets.emplace_back();
		}
		mSets[set].set(element);
	}
	const auto& get_set(std::size_t set) const {
		return mSets[set];
	}
	std::size_t element_count() const;
	std::size_t largest_set() const;
	Bitset get_uncovered() const {
		return std::accumulate(mSets.begin(), mSets.end(), Bitset(),
			[](const auto& lhs, const auto& rhs){
				return lhs | rhs;
			}
		);
	}
	void select_set(std::size_t s);
};

std::ostream& operator<<(std::ostream& os, const SetCover& sc);

}