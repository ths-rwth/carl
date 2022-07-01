/**
 * @file UTerm.h
 * @author Rebecca Haehn <haehn@cs.rwth-aachen.de>
 * @since 2018-08-28
 * @version 2018-08-28
 */

#pragma once

#include <carl-arith/core/Variables.h>
#include "UFInstance.h"
#include "UVariable.h"

#include <iostream>
#include <variant>

namespace carl
{
	/**
	 * Implements an uninterpreted term, that is either an uninterpreted variable or an uninterpreted function instance.
	 */
	class UTerm {
		using Super = std::variant<UVariable, UFInstance>;
		Super mTerm;

	public:
		/**
		 * Default constructor.
		 */
		UTerm() = default;

		UTerm(UVariable v): mTerm(v) {}
		UTerm(UFInstance ufi): mTerm(ufi) {}

		/**
		 * Constructs an uninterpreted term.
		 * @param term
		 */
		explicit UTerm(const Super& term): mTerm(term) {}

		const auto& asVariant() const {
			return mTerm;
		}

		/**
		 * @return true, if the stored term is a UVariable.
		 */
		bool isUVariable() const {
			return std::holds_alternative<UVariable>(mTerm);
		}

		/**
		 * @return true, if the stored term is a UFInstance.
		 */
		bool isUFInstance() const {
			return std::holds_alternative<UFInstance>(mTerm);
		}

		/**
		 * @return The stored term as UVariable.
		 */
		UVariable asUVariable() const {
			assert(isUVariable());
			return std::get<UVariable>(mTerm);
		}
		/**
		 * @return The stored term as UFInstance.
		 */
		UFInstance asUFInstance() const {
			assert(isUFInstance());
			return std::get<UFInstance>(mTerm);
		}

		/**
		 * @return The domain of this uninterpreted term.
		 */
		Sort domain() const;

		std::size_t complexity() const;

		void gatherVariables(carlVariables& vars) const;
		void gatherUFs(std::set<UninterpretedFunction>& ufs) const;
	};
	static_assert(std::is_literal_type<UTerm>::value, "UTerm should be a literal type.");

	/**
	 * @param lhs The uninterpreted term to the left.
	 * @param rhs The uninterpreted term to the right.
	 * @return true, if the given uninterpreted terms are equal.
	 */
	bool operator==(const UTerm& lhs, const UTerm& rhs);

	bool operator!=(const UTerm& lhs, const UTerm& rhs);

	/**
	 * @param lhs The uninterpreted term to the left.
	 * @param rhs The uninterpreted term to the right.
	 * @return true, if lhs is smaller than rhs.
	 */
	bool operator<(const UTerm& lhs, const UTerm& rhs);

	/**
	 * Prints the given uninterpreted term on the given output stream.
	 * @param os The output stream to print on.
	 * @param ut The uninterpreted term to print.
	 * @return The output stream after printing the given uninterpreted term on it.
	 */
	std::ostream& operator<<(std::ostream& os, const UTerm& ut);
} // end namespace carl

namespace std
{
	/**
	 * Implements std::hash for uninterpreted terms.
	 */
	template<>
	struct hash<carl::UTerm> {
		/**
		 * @param ut The uninterpreted term to get the hash for.
		 * @return The hash of the given uninterpreted term.
		 */
		std::size_t operator()(const carl::UTerm& ut) const {
			return std::visit(carl::overloaded {
				[](carl::UVariable var) { return carl::hash_all(var); },
				[](carl::UFInstance ufi) { return carl::hash_all(ufi); },
			}, ut.asVariant());
		}
	};
}
