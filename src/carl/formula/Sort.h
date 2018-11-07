/**
 * @file Sort.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 * @author Florian Corzilius <corzilius@cs.rwth-aachen.de>
 * @since 2014-10-30
 * @version 2014-10-30
 */

#pragma once

#include <iostream>
#include <type_traits>
#include <utility>

namespace carl {

/**
 * Implements a sort (for defining types of variables and functions).
 */
class Sort {
    public:
        /// The type if the unique id to identify a sort in the sort manager.
        using IDType = std::size_t;
    
        friend class SortManager;

    private:
        // Members.

        /// A unique id to identify this sort in the sort manager.
        IDType mId = 0;

        /**
         * Constructs a sort.
         * @param _id The id of the sort to construct.
         */
        explicit Sort(IDType id): mId(id) {}

    public:
		
		Sort() noexcept = default;
		
        /**
         * @return The aritiy of this sort.
         */
        std::size_t arity() const;

        /**
         * @return The id of this sort.
         */
        IDType id() const
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

/**
* @param lhs The left sort.
* @param rhs The right sort.
* @return true, if the sorts are the same.
*/
inline bool operator==(const Sort& lhs, const Sort& rhs) {
	return lhs.id() == rhs.id();
}
/**
* @param lhs The left sort.
* @param rhs The right sort.
* @return true, if the sorts are different.
*/
inline bool operator!=(const Sort& lhs, const Sort& rhs) {
	return lhs.id() != rhs.id();
}

/**
* @param _sort The sort to compare with.
* @return true, if this sort is less than the given one.
*/
inline bool operator<(const Sort& lhs, const Sort& rhs) {
	return lhs.id() < rhs.id();
}

static_assert(std::is_trivially_copyable<Sort>::value, "Sort should be trivially copyable.");

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
            return std::size_t(_sort.id());
        }
    };
} // end namespace std
