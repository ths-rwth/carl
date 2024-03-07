#pragma once

#include <carl-common/config.h>
#ifdef USE_LIBPOLY

#include <carl-common/memory/Singleton.h>
#include <map>
#include <optional>
#include "../../core/Variable.h"
#include <poly/poly.h>
#include <poly/polynomial_context.h>
#include <poly/polyxx.h>


namespace carl {

class LPVariables : public Singleton<LPVariables> {
    friend Singleton<LPVariables>;

    // mapping from carl variables to libpoly variables
    std::map<carl::Variable, lp_variable_t> vars_carl_libpoly;
    // mapping from libpoly variables to carl variables
    std::map<lp_variable_t, carl::Variable> vars_libpoly_carl;

public:
    lp_variable_db_t* lp_var_db;

    #define TEMP_VARIABLE_SIZE 10

    lp_variable_t lp_var_tmp[TEMP_VARIABLE_SIZE];

    LPVariables();
    ~LPVariables();
    std::optional<carl::Variable> carl_variable(lp_variable_t var) const;
    std::optional<lp_variable_t> lp_variable_opt(carl::Variable var) const;
    lp_variable_t lp_variable(carl::Variable var);
};
    
} // namespace carl

#endif