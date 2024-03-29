#pragma once

#include <map>
#include <set>
#include <vector>


#include <carl-arith/core/Variable.h>
#include <carl-arith/numbers/numbers.h>

namespace carl {	
    template<typename T> 
    using Assignment = std::map<Variable, T>;

    template<typename T> 
    using OrderedAssignment = std::vector<std::pair<Variable, T>>;
        
    using Variables = std::set<Variable>;
    
    template<typename Pol>
    using Factors = std::map<Pol,uint>;	
}
