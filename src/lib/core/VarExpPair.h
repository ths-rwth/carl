/**
 * @file VarExpPair.h 
 * @author Sebastian Junges
 */

#pragma once

#include "Variable.h"
#include <iostream>

namespace arithmetic
{
    typedef unsigned exponent;
    struct VarExpPair
    {
        variable var;
        exponent exp;
        
        VarExpPair(variable v, exponent e) :
            var(v),
            exp(e)
        {}
        
        friend std::ostream& operator <<( std::ostream& os, const VarExpPair& rhs );
    };
}

