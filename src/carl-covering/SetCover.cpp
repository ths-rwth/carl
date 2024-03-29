#include "SetCover.h"

#include <carl-common/datastructures/Bitset.h>

#include <cassert>
#include <numeric>

namespace carl::covering {

void SetCover::set(std::size_t set, std::size_t element) {
	while (set >= mSets.size()) {
		mSets.emplace_back();
	}
	mSets[set].set(element);
}

void SetCover::set(std::size_t set, const Bitset& elements) {
	while (set >= mSets.size()) {
		mSets.emplace_back();
	}
	mSets[set] |= elements;
}

std::size_t SetCover::element_count() const {
	std::size_t max = 0;
	for (const auto& d: mSets) {
		max = std::max(max, d.size());
	}
	return max;
}

void SetCover::prune_sets() {
	for (auto& d: mSets) {
		if (d.none()) {
			d = Bitset();
		}
	}
}

std::size_t SetCover::set_count() const {
	return mSets.size();
}

std::size_t SetCover::active_set_count() const {
	return static_cast<std::size_t>(std::count_if(mSets.begin(), mSets.end(),
		[](const auto& d){ return d.any(); }
	));
}

std::size_t SetCover::largest_set() const {
	assert(mSets.size() > 0);
	std::size_t largest_id = 0;
	std::size_t largest_size = mSets[0].count();
	for (std::size_t id = 1; id < mSets.size(); ++id) {
		std::size_t size = mSets[id].count();
		if (size > largest_size) {
			largest_id = id;
			largest_size = size;
		}
	}
	return largest_id;
}

std::size_t SetCover::largest_set(const std::vector<double>& weights) const {
	assert(mSets.size() > 0);
	std::size_t largest_id = 0;
	double largest_size = mSets[0].count() * weights[0];
	for (std::size_t id = 1; id < mSets.size(); ++id) {
		double size = mSets[id].count() * weights[id];
		if (size > largest_size) {
			largest_id = id;
			largest_size = size;
		}
	}
	return largest_id;
}

Bitset SetCover::get_uncovered() const {
	return std::accumulate(mSets.begin(), mSets.end(), Bitset(),
		[](const auto& lhs, const auto& rhs){
			return lhs | rhs;
		}
	);
}

void SetCover::select_set(std::size_t s) {
	assert(mSets.size() > s);
	auto selected = mSets[s];
	for (auto& d: mSets){
		d -= selected;
	}
}

std::ostream& operator<<(std::ostream& os, const SetCover& sc) {
	std::size_t elems = sc.element_count();
	os << "SetCover:" << std::endl;
	for (const auto& s: sc.mSets) {
		s.resize(elems);
		os << "\t" << s << std::endl;
	}
	return os;
}

}