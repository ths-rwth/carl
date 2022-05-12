#pragma once

#include "Visit.h"
#include <carl/constraint/Complexity.h>

namespace carl {

template<typename Pol>
size_t complexity(const Formula<Pol>& f) {
    size_t result = 0;
    carl::visit(f,
        [&](const Formula<Pol>& _f)
        {
            switch( _f.type() )
            {
                case FormulaType::TRUE:
                case FormulaType::FALSE:
                    break;
                case FormulaType::CONSTRAINT:
                    result += carl::complexity(_f.constraint().constr()); break;
                case FormulaType::BITVECTOR:
                    result += _f.bv_constraint().complexity(); break;
                case FormulaType::UEQ:
                    result += _f.u_equality().complexity(); break;
                default:
                    ++result;
            }
        });
    return result;
}

}