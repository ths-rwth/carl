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
            UTerm mLhs = UTerm(UVariable(Variable::NO_VARIABLE, Sort()));
            /// The right-hand side of this uninterpreted equality.
            UTerm mRhs = UTerm(UVariable(Variable::NO_VARIABLE, Sort()));

        public:
            UEquality() = default;

            /**
             * Constructs an uninterpreted equality.
             * @param negated true, if the negation of this equality shall hold, which means that it is actually an inequality.
             * @param uvarA An uninterpreted variable, which is going to be the left-hand side of this uninterpreted equality.
             * @param uvarB An uninterpreted variable, which is going to be the right-hand side of this uninterpreted equality.
             */
            UEquality(UTerm uvarA, UTerm uvarB, bool negated):
                mNegated(negated),
	              mLhs(std::move(uvarA)),
		            mRhs(std::move(uvarB)) {
	              if(mLhs.isUVariable() && mRhs.isUVariable()) {
                    assert(mLhs.asUVariable().domain() == mRhs.asUVariable().domain());
                    if(mRhs.asUVariable() < mLhs.asUVariable()) {
                        std::swap(mLhs, mRhs);
                    }
                } else if(mLhs.isUVariable() && mRhs.isUFInstance()) {
                    assert(mLhs.asUVariable().domain() == mRhs.asUFInstance().uninterpretedFunction().codomain());
                } else if(mLhs.isUFInstance() && mRhs.isUVariable()) {
                    assert(mLhs.asUFInstance().uninterpretedFunction().codomain() == mRhs.asUVariable().domain());
                    std::swap(mLhs, mRhs);
                } else if(mLhs.isUFInstance() && mRhs.isUFInstance()) {
                    assert(mLhs.asUFInstance().uninterpretedFunction().codomain() == mRhs.asUFInstance().uninterpretedFunction().codomain());
                    if(mRhs.asUFInstance() < mLhs.asUFInstance()) {
                        std::swap(mLhs, mRhs);
                    }
                }
            }

            /**
             * Constructs an uninterpreted equality.
             * @param negated true, if the negation of this equality shall hold, which means that it is actually an inequality.
             * @param uvarA An uninterpreted variable, which is going to be the left-hand side of this uninterpreted equality.
             * @param uvarB An uninterpreted variable, which is going to be the right-hand side of this uninterpreted equality.
             */
            UEquality(const UVariable& uvarA, const UVariable& uvarB, bool negated, bool orderCorrect = false):
                mNegated(negated),
                mLhs(uvarA),
                mRhs(uvarB) {
                assert(uvarA.domain() == uvarB.domain());
                if(!orderCorrect && mRhs.asUVariable() < mLhs.asUVariable()) {
                    std::swap(mLhs, mRhs);
                }
            }

            /**
             * Constructs an uninterpreted equality.
             * @param negated true, if the negation of this equality shall hold, which means that it is actually an inequality.
             * @param uvar An uninterpreted variable, which is going to be the left-hand side of this uninterpreted equality.
             * @param ufun An uninterpreted function instance, which is going to be the right-hand side of this uninterpreted equality.
             */
            UEquality(const UVariable& uvar, const UFInstance& ufun, bool negated):
                mNegated(negated),
                mLhs(uvar),
                mRhs(ufun) {
                assert(uvar.domain() == ufun.uninterpretedFunction().codomain());
            }

            /**
             * Constructs an uninterpreted equality.
             * @param negated true, if the negation of this equality shall hold, which means that it is actually an inequality.
             * @param ufun An uninterpreted function instance, which is going to be the left-hand side of this uninterpreted equality.
             * @param uvar An uninterpreted variable, which is going to be the right-hand side of this uninterpreted equality.
             */
            UEquality(const UFInstance& ufun, const UVariable& uvar, bool negated):
                mNegated(negated),
                mLhs(uvar),
                mRhs(ufun) {
                assert(uvar.domain() == ufun.uninterpretedFunction().codomain());
            }

            /**
             * Constructs an uninterpreted equality.
             * @param negated true, if the negation of this equality shall hold, which means that it is actually an inequality.
             * @param ufunA An uninterpreted function instance, which is going to be the left-hand side of this uninterpreted equality.
             * @param ufunB An uninterpreted function instance, which is going to be the right-hand side of this uninterpreted equality.
             */
            UEquality(const UFInstance& ufunA, const UFInstance& ufunB, bool negated, bool orderCorrect = false):
                mNegated(negated),
                mLhs(ufunA),
                mRhs(ufunB) {
                assert(ufunA.uninterpretedFunction().codomain() == ufunB.uninterpretedFunction().codomain());
                if(!orderCorrect && mRhs.asUFInstance() < mLhs.asUFInstance()) {
                    std::swap(mLhs, mRhs);
                }
            }

            /**
             * Copies the given uninterpreted equality.
             * @param ueq The uninterpreted equality to copy.
             * @param invert true, if the inverse of the given uninterpreted equality shall be constructed. (== -> != resp. != -> ==)
             */
            UEquality(const UEquality& ueq, bool invert = false):
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
            std::size_t complexity() const;

            /*
             * @return The negation of the uninterpreted equality.
             */
      			UEquality negation() const {
      				return UEquality(lhs(), rhs(), !negated());
      			}

            /**
             * @param ueq The uninterpreted equality to compare with.
             * @return true, if this and the given equality instance are equal.
             */
            bool operator==(const UEquality& ueq) const;

            /**
             * @param ueq The uninterpreted equality to compare with.
             * @return true, if this uninterpreted equality is less than the given one.
             */
            bool operator<(const UEquality& ueq) const;

            std::string toString(unsigned unequalSwitch, bool infix, bool friendlyNames ) const;

            void collectUVariables(std::set<UVariable>& uvars) const;

    };
	/**
	 * Prints the given uninterpreted equality on the given output stream.
	 * @param os The output stream to print on.
	 * @param ueq The uninterpreted equality to print.
	 * @return The output stream after printing the given uninterpreted equality on it.
	 */
	std::ostream& operator<<(std::ostream& os, const UEquality& ueq);
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
        size_t operator()(const carl::UEquality& ueq) const {
            return carl::hash_all(ueq.lhs(), ueq.rhs());
        }
    };
}
