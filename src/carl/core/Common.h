#pragma once

#include <map>
#include <set>
#include <vector>


#include "../core/Variable.h"
#include "../numbers/numbers.h"

namespace carl {	
    template<typename T> 
    using Assignment = std::map<Variable, T>;

    template<typename T> 
    using EvaluationMap = Assignment<T>;

    template<typename T> 
    using OrderedAssignment = std::vector<Variable, T>;
        
    using Variables = std::set<Variable>;
    
    template<typename Pol>
    using Factors = std::map<Pol,uint>;	
}
