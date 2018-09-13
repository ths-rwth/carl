/**
 * @file UFInstance.h
 * @author Florian Corzilius <corzilius@cs.rwth-aachen.de>
 * @since 2014-10-30
 * @version 2014-10-30
 */

#pragma once

#include "UVariable.h"
#include "UninterpretedFunction.h"

#include <iostream>

#include <utility>
#include <vector>

namespace carl
{
    class UTerm;

    /**
     * Implements an uninterpreted function instance.
     */
    class UFInstance {
        public:
            friend class UFInstanceManager;

        private:
            /// A unique id.
            std::size_t mId = 0;

            /**
             * Constructs an uninterpreted function instance.
             * @param id
             */
            explicit UFInstance(std::size_t id) noexcept: mId(id) {}

        public:
            /**
             * @return The unique id of this uninterpreted function instance.
             */
            std::size_t id() const {
                return mId;
            }

            /**
             * @return The underlying uninterpreted function of this instance.
             */
            const UninterpretedFunction& uninterpretedFunction() const;

            /**
             * @return The arguments of this uninterpreted function instance.
             */
            const std::vector<UTerm>& args() const;

			std::size_t complexity() const;
    };

	/**
	 * @param lhs The left function instance.
	 * @param rhs The right function instance.
	 * @return true, if lhs == rhs.
	 */
	inline bool operator==(const UFInstance& lhs, const UFInstance& rhs) {
		return lhs.id() == rhs.id();
	}

	/**
	 * @param lhs The left function instance.
	 * @param rhs The right function instance.
	 * @return true, if lhs < rhs.
	 */
	inline bool operator<(const UFInstance& lhs, const UFInstance& rhs) {
		return lhs.id() < rhs.id();
	}

	/**
	 * Prints the given uninterpreted function instance on the given output stream.
	 * @param os The output stream to print on.
	 * @param ufun The uninterpreted function instance to print.
	 * @return The output stream after printing the given uninterpreted function instance on it.
	 */
	std::ostream& operator<<(std::ostream& os, const UFInstance& ufun);
} // end namespace carl

namespace std {
	/**
	 * Implements std::hash for uninterpreted function instances.
	 */
	template<>
	struct hash<carl::UFInstance> {
	public:
		/**
		 * @param ufi The uninterpreted function instance to get the hash for.
		 * @return The hash of the given uninterpreted function instance.
		 */
		std::size_t operator()(const carl::UFInstance& ufi) const {
			return carl::hash_all(ufi.id());
		}
	};
}
