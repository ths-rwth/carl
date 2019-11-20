/**
 * @file Sort.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 * @author Florian Corzilius <corzilius@cs.rwth-aachen.de>
 * @since 2014-10-30
 * @version 2014-10-30
 */

#pragma once

#include <ostream>
#include <type_traits>
#include <utility>

namespace carl {

/**
 * Implements a sort (for defining types of variables and functions).
 */
class Sort {
    public:
        friend class SortManager;

    private:
        // Members.

        /// A unique id to identify this sort in the sort manager.
        std::size_t mId = 0;

        /**
         * Constructs a sort.
         * @param _id The id of the sort to construct.
         */
        explicit Sort(std::size_t id): mId(id) {}

    public:
		
		Sort() noexcept = default;
		
        /**
         * @return The aritiy of this sort.
         */
        std::size_t arity() const;

        /**
         * @return The id of this sort.
         */
        std::size_t id() const
        {
            return mId;
        }

        /**
         * Prints the given sort on the given output stream.
         * @param _os The output stream to print on.
         * @param _sort The sort to print.
         * @return The output stream after printing the given sort on it.
         */
        friend std::ostream& operator<<(std::ostream& _os, const Sort& _sort);
};

static_assert(std::is_trivially_copyable<Sort>::value, "Sort should be trivially copyable.");
static_assert(std::is_literal_type<Sort>::value, "Sort should be a literal type.");
static_assert(sizeof(Sort) == sizeof(std::size_t), "Sort should be as large as its id type");

/**
* @param lhs The left sort.
* @param rhs The right sort.
* @return true, if the sorts are the same.
*/
inline bool operator==(Sort lhs, Sort rhs) {
	return lhs.id() == rhs.id();
}
/**
* @param lhs The left sort.
* @param rhs The right sort.
* @return true, if the sorts are different.
*/
inline bool operator!=(Sort lhs, Sort rhs) {
	return lhs.id() != rhs.id();
}

/**
* Checks whether one sort is smaller than another.
* @return true, if lhs is less than rhs.
*/
inline bool operator<(Sort lhs, Sort rhs) {
	return lhs.id() < rhs.id();
}

} // end namespace carl

namespace std {
    /**
     * Implements std::hash for sort.
     */
    template<>
    struct hash<carl::Sort> {
        /**
         * @param _sort The sort to get the hash for.
         * @return The hash of the given sort.
         */
        std::size_t operator()(const carl::Sort& _sort) const {
            return _sort.id();
        }
    };
} // end namespace std
