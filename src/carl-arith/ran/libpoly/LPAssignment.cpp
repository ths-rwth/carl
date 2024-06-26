#include "LPAssignment.h"


#include <carl-common/config.h>
#ifdef USE_LIBPOLY

#include "../../poly/libpoly/LPVariables.h"


namespace carl {

LPAssignment::LPAssignment() {
    lp_assignment_construct(&lp_assignment, LPVariables::getInstance().lp_var_db);
}

LPAssignment::~LPAssignment() {
    lp_assignment_destruct(&lp_assignment);
}

lp_assignment_t& LPAssignment::get(const carl::Assignment<LPRealAlgebraicNumber>& ass) {
    if (last_assignment == ass) {
        return lp_assignment;
    } else {
        last_assignment = ass; 
        if (lp_assignment.values) {
            for (size_t i = 0; i < lp_assignment.size; ++ i) {
                lp_assignment_set_value(&lp_assignment, i, 0);
            }
        }
        for (const auto& entry : ass) {
            lp_assignment_set_value(&lp_assignment, LPVariables::getInstance().lp_variable(entry.first), entry.second.get_internal());
        }
        return lp_assignment;
    }
}
    
} // namespace carl

#endif