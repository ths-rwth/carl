/**
 * @file Monomial.h 
 * @author Sebastian Junges
 */

#pragma once
#include "Variable.h"

namespace arithmetic
{   

    typedef unsigned exponent;
    
    struct VarExpPair
    {
        variable var;
        exponent exp;
    };
    /**
     *  The general-purpose multivariate polynomial class.
     */
    template<typename Coeff>
    class Monomial
    {
        protected:
            Coeff mCoefficient;
            std::vector<VarExpPair> mExponents;
            
    };
}
