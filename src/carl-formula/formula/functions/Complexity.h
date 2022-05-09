#pragma once

#include "Visit.h"

namespace carl {

template<typename Pol>
size_t complexity(const Formula<Pol>& f) {
    size_t result = 0;
    carl::visit(f,
        [&](const Formula<Pol>& _f)
        {
            switch( _f.getType() )
            {
                case FormulaType::TRUE:
                case FormulaType::FALSE:
                    break;
                case FormulaType::CONSTRAINT:
                    result += carl::complexity(_f.constraint().constr()); break;
                case FormulaType::BITVECTOR:
                    result += _f.bvConstraint().complexity(); break;
                case FormulaType::UEQ:
                    result += _f.uequality().complexity(); break;
                default:
                    ++result;
            }
        });
    return result;
}

}