#include <carl-common/config.h>
#ifdef USE_LIBPOLY

#include "LPContext.h"

namespace carl {

LPContext::Data::Data(const std::vector<Variable>& v) : variable_order(v) {
    lp_var_order = lp_variable_order_new();
    // lp_context = lp_polynomial_context_new(0, LPVariables::getInstance().lp_var_db, lp_var_order);
    lp_context = (lp_polynomial_context_t*) malloc(sizeof(lp_polynomial_context_t));
    //lp_polynomial_context_construct(lp_context, 0, LPVariables::getInstance().lp_var_db, lp_var_order);
    lp_context->ref_count = 0;
    lp_context->var_db = LPVariables::getInstance().lp_var_db;
    lp_context->K = 0;
    lp_context->var_order = lp_var_order;
    #define TEMP_VARIABLE_SIZE 10
    lp_context->var_tmp = (lp_variable_t*)malloc(sizeof(lp_variable_t)*TEMP_VARIABLE_SIZE);
    for (size_t i = 0; i < TEMP_VARIABLE_SIZE; ++ i) {
        lp_context->var_tmp[i] = LPVariables::getInstance().lp_var_tmp[i];
    }
    lp_context->var_tmp_size = 0;
    lp_polynomial_context_attach(lp_context);
}

LPContext::Data::~Data() {
    lp_variable_order_detach(lp_var_order);
    lp_polynomial_context_detach(lp_context);
}

}

#endif