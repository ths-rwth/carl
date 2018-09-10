/**
 * @file UTerm.cpp
 * @author Rebecca Haehn <haehn@cs.rwth-aachen.de>
 * @since 2018-08-28
 * @version 2018-08-28
 */


#include "UTerm.h"

#include <iostream>
#include <sstream>

namespace carl
{
    /**
     * @return The domain of this uninterpreted term.
     */
    const Sort& UTerm::domain() const {
      return std::visit(overloaded {
              [](const UVariable& var) -> auto& { return var.domain(); },
              [](const UFInstance& ufi) -> auto& { return ufi.uninterpretedFunction().codomain(); },
          }, mTerm);
    }

    /**
      * Prints the given uninterpreted term on the given output stream.
      * @param os The output stream to print on.
      * @param ut The uninterpreted term to print.
      * @return The output stream after printing the given uninterpreted term on it.
      */
    std::ostream& operator<<(std::ostream& os, const UTerm& ut) {
        return std::visit(overloaded {
                [&os](const UVariable& var) -> auto& { return os << var; },
                [&os](const UFInstance& ufi) -> auto& { return os << ufi; },
            }, ut.mTerm);
    }
}
