/**
 * @file VarExpPair.h 
 * @author Sebastian Junges
 * A small internal piece of data, moved to own header for better overview, 
 * as well as reducing the lines of recompiled code.
 */

#pragma once

#include "Variable.h"
#include <iostream>

namespace arithmetic
{
    typedef unsigned exponent;
    /**
     * Internal struct used in monomials to represent a variable with an exponent.
     */
    struct VarExpPair
    {
        Variable var;
        exponent exp;
        
        VarExpPair(Variable v, exponent e) :
            var(v),
            exp(e)
        {}
        
        friend std::ostream& operator <<( std::ostream& os, const VarExpPair& rhs )
        {
            os << (rhs.var);
            if(rhs.exp > 1) os << "^" << rhs.exp;
            return os;
        }
    };
}

