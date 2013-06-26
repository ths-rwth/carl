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
        
        explicit VarExpPair(Variable v, exponent e=1) :
            var(v),
            exp(e)
        {}
        
        friend bool operator==(const VarExpPair& lhs, const VarExpPair& rhs)
        {
            return lhs.var == rhs.var && lhs.exp  == rhs.exp;
        }
        friend bool operator!=(const VarExpPair& lhs, const VarExpPair& rhs)
        {
            return lhs.var != rhs.var || lhs.exp  != rhs.exp;
        }
        // Operators checking only the variable.
        friend bool operator==(const VarExpPair& lhs, Variable::Arg rhs)
        {
            return lhs.var == rhs;
        }
        friend bool operator!=(const VarExpPair& lhs, Variable::Arg rhs)
        {
            return lhs.var != rhs;
        }
        friend bool operator<=(const VarExpPair& lhs, Variable::Arg rhs)
        {
            return lhs.var <= rhs;
        }
        friend bool operator>=(const VarExpPair& lsh, Variable::Arg rhs)
        {
            return lsh.var >= rhs;
        }
        friend bool operator<(const VarExpPair& lhs, Variable::Arg rhs)
        {
            return lhs.var < rhs;
        }
        friend bool operator>(const VarExpPair& lsh, Variable::Arg rhs)
        {
            return lsh.var > rhs;
        }
        
        friend std::ostream& operator <<( std::ostream& os, const VarExpPair& rhs )
        {
            os << (rhs.var);
            if(rhs.exp > 1) os << "^" << rhs.exp;
            return os;
        }
        
        
    };
}

