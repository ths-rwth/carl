/**
 * @file VariablePool.h 
 * @author Sebastian Junges
 */

#pragma once

#include <string>

#include "Variable.h"


namespace arithmetic 
{

/**
 * Class which saves names of variables and can generate fresh variables etc.
 */
class VariablePool
{
    /**
     * Get a variable which was not used before.
     * @param type
     * @return A variable of type type.
     */
    variable getFreshVariable(VariableType type = VT_REAL);
    /**
     * 
     * @param v
     * @return 
     */
    std::string getVariableName(variable v);
    /**
     * 
     * @param v
     * @param name
     */
    void setVariableName(variable v, const std::string& name);
};

}