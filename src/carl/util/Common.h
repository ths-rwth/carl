#pragma once

#include "../core/Variable.h"
#include "../numbers/numbers.h"
#include <carl-common/util/container_types.h>


namespace carl
{	
	// Structures.
    
    // Further type definitions.
    template<typename T> 
    using EvaluationMap = std::map<Variable, T>;
    
//    template<typename Pol>
//    using VarInfo = VariableInformation<true, Pol>;
//    
//    template<typename Pol>
//    using VarInfoMap = std::map<Variable, VarInfo<Pol>>;
    
    using Variables = std::set<Variable>;

    using QuantifiedVariables = std::vector<Variables>;
    
    template<typename Pol>
    using Factors = std::map<Pol,uint>;
    
    // Constants.
    ///@todo move static variables to own cpp
    
    static constexpr uint MAX_DEGREE_FOR_FACTORIZATION = 6;
    
    static constexpr uint MIN_DEGREE_FOR_FACTORIZATION = 1;
    
    static constexpr uint MAX_DIMENSION_FOR_FACTORIZATION = 6;
    
    static constexpr uint MAX_NUMBER_OF_MONOMIALS_FOR_FACTORIZATION = 10;
    
    static constexpr bool FULL_EFFORT_FOR_DEFINITENESS_CHECK = false;
	
}	// namespace carl
