/**
 * @file UTerm.h
 * @author Rebecca Haehn <haehn@cs.rwth-aachen.de>
 * @since 2018-08-28
 * @version 2018-08-28
 */

#pragma once

#include "UVariable.h"
#include "UFInstance.h"
#include <variant>

#include <iostream>

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

            /**
             * Constructs an uninterpreted term.
             * @param term
             */
            UTerm(Super term): mTerm(term) {}

            /**
              * @return true, if the stored term is a UVariable.
              */
            bool isUVariable() const {
                return std::visit(overloaded {
                        [](const UVariable& var) { return true; },
                        [](const UFInstance& ufi) { return false; },
                    }, mTerm);
            }

            /**
              * @return true, if the stored term is a UFInstance.
              */
            bool isUFInstance() const {
              return std::visit(overloaded {
                      [](const UVariable& var) { return false; },
                      [](const UFInstance& ufi) { return true; },
                  }, mTerm);
            }

            /**
              * @return The stored term as UVariable.
              */
            const UVariable asUVariable() const {
              assert(isUVariable());
              return std::get<UVariable>(mTerm);
            }
            /**
              * @return The stored term as UFInstance.
              */
            const UFInstance asUFInstance() const {
              assert(isUFInstance());
              return std::get<UFInstance>(mTerm);
            }

            /**
             * @return The domain of this uninterpreted term.
             */
             const Sort& domain() const;

            /**
             * @param ut The uninterpreted term to compare with.
             * @return true, if this and the given uninterpreted term are equal.
             */
            bool operator==(const UTerm& ut) const
            {
                return std::visit(overloaded {
                        [](const UVariable& varL, const UVariable& varR) { return varL.variable().id() == varR.variable().id(); },
                        [](const UFInstance& ufiL, const UFInstance& ufiR) { return ufiL.id() == ufiR.id(); },
                        [](const auto& termL, const auto& termR) { return false; },
                    }, (*this).mTerm, ut.mTerm);
            }

            /**
             * @param ut The uninterpreted term to compare with.
             * @return true, if this uninterpreted term is less than the given one.
             */
            bool operator<(const UTerm& ut) const
            {
              return std::visit(overloaded {
                      [](const UVariable& varL, const UVariable& varR) { return varL.variable().id() < varR.variable().id(); },
                      [](const UFInstance& ufiL, const UFInstance& ufiR) { return ufiL.id() < ufiR.id(); },
                      [](const UVariable& varL, const UFInstance& ufiR) { return true; },
                      [](const auto& termL, const auto& termR) { return false; },
                  }, (*this).mTerm, ut.mTerm);
            }

            /**
             * Prints the given uninterpreted term on the given output stream.
             * @param os The output stream to print on.
             * @param ut The uninterpreted term to print.
             * @return The output stream after printing the given uninterpreted term on it.
             */
            friend std::ostream& operator<<( std::ostream& os, const UTerm& ut);

            friend struct std::hash<carl::UTerm>;
    };
} // end namespace carl

namespace std
{
    /**
     * Implements std::hash for uninterpreted terms.
     */
    template<>
    struct hash<carl::UTerm> {
      public:
          /**
           * @param ut The uninterpreted term to get the hash for.
           * @return The hash of the given uninterpreted term.
           */
          std::size_t operator()(const carl::UTerm& ut) const {
              return std::visit(carl::overloaded {
                [](const carl::UVariable& var) { return carl::hash_all(var); },
                [](const carl::UFInstance& ufi) { return carl::hash_all(ufi); },
              }, ut.mTerm);
          }
    };
}
