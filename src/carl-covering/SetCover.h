#pragma once

#include <carl/util/Bitset.h>

#include <vector>

namespace carl::covering {

/**
 * Represents a set cover problem.
 * Allows to state which sets cover which elements and offers some helper methods to work with this set cover for the heuristics.
 */
class SetCover {
public:
	friend std::ostream& operator<<(std::ostream& os, const SetCover& sc);
private:
	/// The actual sets.
	std::vector<carl::Bitset> mSets;
public:
	/// Returns the number of sets.
	std::size_t set_count() const {
		return mSets.size();
	}
	/// States that s covers the given element.
	void set(std::size_t set, std::size_t element);
	/// States that s covers the given elements.
	void set(std::size_t set, const Bitset& elements);
	/// Returns the given set.
	const auto& get_set(std::size_t set) const {
		return mSets[set];
	}
	/// Returns the number of elements.
	std::size_t element_count() const;
	/// Returns the id of the largest set.
	std::size_t largest_set() const;
	/// Returns the uncovered elements.
	Bitset get_uncovered() const;
	/// Selects the given set and purges the covered elements from all other sets.
	void select_set(std::size_t s);
};

/// Print the set cover to os.
std::ostream& operator<<(std::ostream& os, const SetCover& sc);

}