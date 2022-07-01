/**
 * @file SortValue.h
 * @author Florian Corzilius <corzilius@cs.rwth-aachen.de>
 * @since 2014-10-24
 * @version 2014-10-24
 */

#pragma once

#include <carl-common/util/hash.h>
#include <carl-formula/sort/Sort.h>

#include <cassert>
#include <iostream>
#include <utility>

namespace carl {

/**
 * Implements a sort value, being a value of the uninterpreted domain specified by this sort.
 */
class SortValue {
public:
	friend class SortValueManager;

private:
	// Members.

	/// The sort defining the domain in which this value is.
	carl::Sort mSort;
	/// A unique id to identify this sort in the sort value manager.
	std::size_t mId = 0;

	/**
	 * Constructs a sort value.
	 * @parem _sort The sort.
	 * @param _id The id of the sort value to construct.
	 */
	explicit SortValue(Sort _sort, std::size_t _id)
		: mSort(_sort),
		  mId(_id) {}

public:
	SortValue() noexcept = default;

	/**
	 * @return The sort of this value.
	 */
	const carl::Sort& sort() const noexcept {
		return mSort;
	}

	/**
	 * @return The id of this sort value.
	 */
	std::size_t id() const noexcept {
		return mId;
	}
};

/**
 * Prints the given sort value on the given output stream.
 * @param os The output stream to print on.
 * @param sv The sort value to print.
 * @return The output stream after printing the given sort value on it.
 */
inline std::ostream& operator<<(std::ostream& os, const SortValue& sv) {
	return os << sv.sort() << "!val!" << sv.id();
}

/**
 * Compares two sort values for equality.
 */
inline bool operator==(const SortValue& lhs, const SortValue& rhs) {
	assert(lhs.sort() == rhs.sort());
	return lhs.id() == rhs.id();
}

/**
 * Orders two sort values.
 */
inline bool operator<(const SortValue& lhs, const SortValue& rhs) {
	assert(lhs.sort() == rhs.sort());
	return lhs.id() < rhs.id();
}

} // namespace carl

namespace std {
/**
 * Implements std::hash for sort value.
 */
template<>
struct hash<carl::SortValue> {
	/**
	 * @param sv The sort value to get the hash for.
	 * @return The hash of the given sort value.
	 */
	std::size_t operator()(const carl::SortValue& sv) const {
		return carl::hash_all(sv.id(), sv.sort());
	}
};
} // namespace std
