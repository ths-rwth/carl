#include "LPVariables.h"


#include <carl-common/config.h>
#ifdef USE_LIBPOLY


namespace carl {

LPVariables::LPVariables() {
    lp_var_db = lp_variable_db_new();
    for (size_t i = 0; i < TEMP_VARIABLE_SIZE; ++ i) {
        char name[10];
        sprintf(name, "#%zu", i);
        lp_var_tmp[i] = lp_variable_db_new_variable(lp_var_db, name);
    }
}

LPVariables::~LPVariables() {
    lp_variable_db_detach(lp_var_db);
}


std::optional<carl::Variable> LPVariables::carl_variable(lp_variable_t var) const {
    auto it = vars_libpoly_carl.find(var);
    if(it == vars_libpoly_carl.end()) return std::nullopt;
    CARL_LOG_TRACE("carl.poly", "Mapping libpoly variable " << lp_variable_db_get_name(lp_var_db, var) << " (" << var << ") -> " << it->second << " (" << it->second.id() << ")");
    return it->second;
}

std::optional<lp_variable_t> LPVariables::lp_variable_opt(carl::Variable var) const {    
    auto it = vars_carl_libpoly.find(var);
    if(it == vars_carl_libpoly.end()) return std::nullopt;
    CARL_LOG_TRACE("carl.poly", "Mapping carl variable " << var << " (" << var.id() << ") -> " << lp_variable_db_get_name(lp_var_db, it->second) << " (" << it->second << ")");
    return it->second;
}

lp_variable_t LPVariables::lp_variable(carl::Variable var) {
    auto it = vars_carl_libpoly.find(var);
    if(it != vars_carl_libpoly.end()) {
        CARL_LOG_TRACE("carl.poly", "Mapping carl variable " << var << " (" << var.id() << ") -> " << lp_variable_db_get_name(lp_var_db, it->second) << " (" << it->second << ")");
        return it->second;
    } else {
        std::string var_name = var.name();
        lp_variable_t poly_var = lp_variable_db_new_variable(lp_var_db, &var_name[0]);
        vars_carl_libpoly.emplace(var, poly_var);
        vars_libpoly_carl.emplace(poly_var, var);
        CARL_LOG_TRACE("carl.poly", "Mapping carl variable " << var << " (" << var.id() << ") -> " << lp_variable_db_get_name(lp_var_db, poly_var) << " (" << poly_var << ")");
        return poly_var;
    }
}
    
} // namespace carl

#endif