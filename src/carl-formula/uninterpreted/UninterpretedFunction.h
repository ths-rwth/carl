/**
 * @file UninterpretedFunction.h
 * @author Florian Corzilius<corzilius@cs.rwth-aachen.de>
 * @since 2014-10-30
 * @version 2014-10-30
 */

#pragma once

#include <carl-common/util/hash.h>
#include "../sort/Sort.h"
#include <carl-common/util/streamingOperators.h>

#include <iostream>
#include <utility>
#include <vector>

namespace carl
{
	/**
	 * Implements an uninterpreted function.
	 */
	class UninterpretedFunction {
		public:
			friend class UFManager;

		private:
			/// A unique id.
			std::size_t mId = 0;

			/**
			 * Constructs an uninterpreted function.
			 * @param id
			 */
			explicit UninterpretedFunction(std::size_t id):
				mId(id) {}

		public:
			/**
			 * Default constructor.
			 */
			UninterpretedFunction() noexcept = default;

			/**
			 * @return The unique id of this uninterpreted function instance.
			 */
			std::size_t id() const {
				return mId;
			}

			/**
			 * @return The name of this uninterpreted function.
			 */
			const std::string& name() const;

			/**
			 * @return The domain of this uninterpreted function.
			 */
			const std::vector<Sort>& domain() const;

			/**
			 * @return The codomain of this uninterpreted function.
			 */
			Sort codomain() const;
	};
	static_assert(std::is_trivially_copyable<UninterpretedFunction>::value, "UninterpretedFunction should be trivially copyable.");
	static_assert(std::is_literal_type<UninterpretedFunction>::value, "UninterpretedFunction should be a literal type.");
	static_assert(sizeof(UninterpretedFunction) == sizeof(std::size_t), "UninterpretedFunction should be as large as its id");

	/**
	 * Check whether two uninterpreted functions are equal.
	 * @return true, if the two given uninterpreted functions are equal.
	 */
	inline bool operator==(const UninterpretedFunction& lhs, const UninterpretedFunction& rhs) {
		return lhs.id() == rhs.id();
	}

	/**
	 * Check whether one uninterpreted function is smaller than another.
	 * @return true, if one uninterpreted function is less than the other one.
	 */
	inline bool operator<(const UninterpretedFunction& lhs, const UninterpretedFunction& rhs) {
		return lhs.id() < rhs.id();
	}

	/**
	 * Prints the given uninterpreted function on the given output stream.
	 * @param os The output stream to print on.
	 * @param ufun The uninterpreted function to print.
	 * @return The output stream after printing the given uninterpreted function on it.
	 */
	inline std::ostream& operator<<(std::ostream& os, const UninterpretedFunction& ufun) {
		os << ufun.name() << "(";
		os << carl::stream_joined(", ", ufun.domain());
		os << ") " << ufun.codomain();
		return os;
	}
} // end namespace carl


namespace std
{
	/**
	 * Implements std::hash for uninterpreted functions.
	 */
	template<>
	struct hash<carl::UninterpretedFunction> {
		/**
		 * @param uf The uninterpreted function to get the hash for.
		 * @return The hash of the given uninterpreted function.
		 */
		std::size_t operator()(const carl::UninterpretedFunction& uf) const {
			return carl::hash_all(uf.id());
		}
	};
}
