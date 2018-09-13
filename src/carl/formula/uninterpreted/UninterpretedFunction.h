/**
 * @file UninterpretedFunction.h
 * @author Florian Corzilius<corzilius@cs.rwth-aachen.de>
 * @since 2014-10-30
 * @version 2014-10-30
 */

#pragma once

#include "../../util/hash.h"
#include "../Sort.h"
#include "../../io/streamingOperators.h"

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
			/// A unique id to identify this uninterpreted function in it's manager.
			using IDType = std::size_t;

		private:
			/// A unique id.
			IDType mId = 0;

			/**
			 * Constructs an uninterpreted function.
			 * @param id
			 */
			explicit UninterpretedFunction(IDType id):
				mId(id) {}

		public:
			/**
			 * Default constructor.
			 */
			UninterpretedFunction() noexcept = default;

			/**
			 * @return The unique id of this uninterpreted function instance.
			 */
			IDType id() const {
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
			const Sort& codomain() const;
	};


	/**
	 * @param ufun The uninterpreted function to compare with.
	 * @return true, if this and the given uninterpreted function are equal.
	 */
	inline bool operator==(const UninterpretedFunction& lhs, const UninterpretedFunction& rhs) {
		return lhs.id() == rhs.id();
	}

	/**
	 * @param ufun The uninterpreted function to compare with.
	 * @return true, if this uninterpreted function is less than the given one.
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
