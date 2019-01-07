/**
 * @file UEquality.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 * @author Florian Corzilius <corzilius@cs.rwth-aachen.de>
 * @since 2014-10-20
 * @version 2014-10-22
 */

#pragma once

#include "UVariable.h"
#include "UFInstance.h"
#include "UTerm.h"

namespace carl
{
    /**
     * Implements an uninterpreted equality, that is an equality of either
     *     two uninterpreted function instances,
     *     two uninterpreted variables,
     *     or an uninterpreted function instance and an uninterpreted variable.
     */
    class UEquality {
        private:
            /// A flag indicating whether this equality shall hold (if being false) or its negation (if being true), hence the inequality, shall hold.
            bool mNegated = false;
            /// The left-hand side of this uninterpreted equality.
            UTerm mLhs;
            /// The right-hand side of this uninterpreted equality.
            UTerm mRhs;

        public:
            UEquality() = default;

            UEquality(const UEquality&) = default;
            UEquality(UEquality&&) = default;

            UEquality& operator=(const UEquality&) = default;
            UEquality& operator=(UEquality&&) = default;
			/**
			 * Constructs an uninterpreted equality.
			 * @param negated true, if the negation of this equality shall hold, which means that it is actually an inequality.
			 * @param lhs An uninterpreted variable, which is going to be the left-hand side of this uninterpreted equality.
			 * @param rhs An uninterpreted variable, which is going to be the right-hand side of this uninterpreted equality.
			 */
			UEquality(const UTerm& lhs, const UTerm& rhs, bool negated):
				mNegated(negated), mLhs(lhs), mRhs(rhs) {
				assert(mLhs.domain() == mRhs.domain());
				if (mRhs < mLhs) std::swap(mLhs, mRhs);
			}

            /**
             * Copies the given uninterpreted equality.
             * @param ueq The uninterpreted equality to copy.
             * @param invert true, if the inverse of the given uninterpreted equality shall be constructed. (== -> != resp. != -> ==)
             */
            UEquality(const UEquality& ueq, bool invert):
                mNegated(invert ? !ueq.mNegated : ueq.mNegated),
                mLhs(ueq.mLhs),
                mRhs(ueq.mRhs) {}

            /**
             * @return true, if the negation of this equation shall hold, that is, it is actually an inequality.
             */
            bool negated() const {
                return mNegated;
            }

            /**
             * @return The left-hand side of this equality.
             */
            const UTerm& lhs() const {
                return mLhs;
            }

            /**
             * @return The right-hand side of this equality.
             */
            const UTerm& rhs() const {
                return mRhs;
            }

            /**
             * @return An approximation of the complexity of this uninterpreted equality.
             */
            std::size_t complexity() const {
				return 1 + mLhs.complexity() + mRhs.complexity();
			}

			/*
			 * @return The negation of the uninterpreted equality.
			 */
			UEquality negation() const {
				return UEquality(lhs(), rhs(), !negated());
			}

            void collectUVariables(std::set<UVariable>& uvars) const;
			void gatherVariables(carlVariables& vars) const {
				mLhs.gatherVariables(vars);
				mRhs.gatherVariables(vars);
			}

    };

	static_assert(std::is_literal_type<UEquality>::value, "UEquality should be a literal type.");

	/**
	 * @param lhs The left hand side.
	 * @param rhs The right hand side.
	 * @return true, if lhs and rhs are equal.
	 */
	inline bool operator==(const UEquality& lhs, const UEquality& rhs) {
		return std::forward_as_tuple(lhs.negated(), lhs.lhs(), lhs.rhs()) == std::forward_as_tuple(rhs.negated(), rhs.lhs(), rhs.rhs());
	}

	/**
	 * @param lhs The left hand side.
	 * @param rhs The right hand side.
	 * @return true, if lhs and rhs are not equal.
	 */
	inline bool operator!=(const UEquality& lhs, const UEquality& rhs) {
	    return !(lhs == rhs);
	}

	/**
	 * @param lhs The left hand side.
	 * @param rhs The right hand side.
	 * @return true, if the left equality is less than the right one.
	 */
	inline bool operator<(const UEquality& lhs, const UEquality& rhs) {
		return std::forward_as_tuple(lhs.negated(), lhs.lhs(), lhs.rhs()) < std::forward_as_tuple(rhs.negated(), rhs.lhs(), rhs.rhs());
	}

	/**
	 * Prints the given uninterpreted equality on the given output stream.
	 * @param os The output stream to print on.
	 * @param ueq The uninterpreted equality to print.
	 * @return The output stream after printing the given uninterpreted equality on it.
	 */
	inline std::ostream& operator<<(std::ostream& os, const UEquality& ueq) {
		return os << ueq.lhs() << (ueq.negated() ? " != " : " == ") << ueq.rhs();
	}
} // end namespace carl

namespace std
{
	/**
	 * Implements std::hash for uninterpreted equalities.
	 */
	template<>
	struct hash<carl::UEquality> {
	public:
		/**
		 * @param ueq The uninterpreted equality to get the hash for.
		 * @return The hash of the given uninterpreted equality.
		 */
		std::size_t operator()(const carl::UEquality& ueq) const {
			return carl::hash_all(ueq.negated(), ueq.lhs(), ueq.rhs());
		}
	};
}
