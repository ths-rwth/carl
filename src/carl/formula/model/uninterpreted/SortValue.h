/**
 * @file SortValue.h
 * @author Florian Corzilius <corzilius@cs.rwth-aachen.de>
 * @since 2014-10-24
 * @version 2014-10-24
 */

#pragma once

#include "../../Sort.h"
#include "../../../util/hash.h"

#include <iostream>
#include <utility>

namespace carl
{

/**
 * Implements a sort value, being a value of the uninterpreted domain specified by this sort.
 */
class SortValue
{
    public:
        friend class SortValueManager;

    private:
        // Members.

        /// The sort defining the domain in which this value is.
        carl::Sort mSort;
        /// A unique id to identify this sort in the sort value manager.
        std::size_t mId;
        
        /**
         * Constructs a sort value.
         * @param _id The id of the sort value to construct.
         */
        explicit SortValue(Sort _sort, std::size_t _id):
            mSort(_sort),
            mId(_id)
        {}

    public:

        SortValue() noexcept:
            mSort(),
            mId(0)
        {}
        
        /**
         * Constructs a sort value by copying the given sort value.
         * @param _sortValue The sort value to copy.
         */
        SortValue(const SortValue& _sortValue) noexcept:
            mSort(_sortValue.sort()),
            mId(_sortValue.id())
        {}

        /**
         * @return The sort of this value.
         */
        const carl::Sort& sort() const noexcept
        {
            return mSort;
        }
            
        /**
         * @return The id of this sort value.
         */
        std::size_t id() const noexcept
        {
            return mId;
        }

        /**
         * Prints the given sort value on the given output stream.
         * @param _os The output stream to print on.
         * @param _sortValue The sort value to print.
         * @return The output stream after printing the given sort value on it.
         */
        friend std::ostream& operator<<(std::ostream& os, const SortValue& sv);

        /**
         * @param _sortValue The sort value to compare with.
         * @return true, if this sort value equals the given one.
         */
        bool operator==( const SortValue& _sortValue ) const noexcept;

        /**
         * @param _sortValue The sort value to compare with.
         * @return true, if this sort value is less than the given one.
         */
        bool operator<( const SortValue& _sortValue ) const noexcept;
};

}

namespace std
{
    /**
     * Implements std::hash for sort value.
     */
    template<>
    struct hash<carl::SortValue>
    {
    public:
        /**
         * @param _sortValue The sort value to get the hash for.
         * @return The hash of the given sort value.
         */
        std::size_t operator()(const carl::SortValue& sv) const {
            return carl::hash_all(sv.id(), sv.sort());
        }
    };
}
