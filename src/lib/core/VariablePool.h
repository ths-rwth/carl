/**
 * @file VariablePool.h 
 * @author Sebastian Junges
 */

#pragma once

#include <string>
#include <memory>
#include <mutex>

#include "Variable.h"


namespace arithmetic 
{

/**
 * Class which saves names of variables and can generate fresh variables etc.
 * 
 * We decided it to be a singleton:
 * 1) We need global access to the variable pool. Everywhere where we want to 
 * interpret some polynomial, we should have access to the pool. 
 * 2) We only want !exactly! one instance of the pool, as otherwise we would get potential clashes
 *  if variables are not available in some method.
 *  Local variable pools make no sense, as variables cannot distinguish from which pool they stem.
 * 
 * 
 */
class VariablePool
{
private:
    
    /// Counter for the number of used variables.
    unsigned nrVariablesUsed;
    
    static std::shared_ptr< VariablePool >  instance;
    /**
     * Making sure that creating a new instance is called only once.
     * Note: According to the new C++11 standard this is not strictly necessary.
     * However, this is currently only supported in gcc 4.8.1 and newer. Therefore,
     * we decided to make explicit use of this flag, thereby supporting gcc 4.7 
     * and some other compilers.
     */
    static std::once_flag                   only_one;
    
    
    VariablePool();
    VariablePool(const VariablePool& rs) {
        instance  = rs.instance;
    }
    
    VariablePool& operator = (const VariablePool& rs) 
    {
        if (this != &rs) {
            instance  = rs.instance;
        }
 
        return *this;
    }   
    
 
public:
    static VariablePool& getInstance(  )
    {
        std::call_once( VariablePool::only_one, [] ()
            {
                VariablePool::instance.reset( new VariablePool() );
                LOGMSG_INFO("arithmetic","VariablePool created.");
            });
 
        return *VariablePool::instance;
    }
    
    /**
     * Get a variable which was not used before.
     * @param type
     * @return A variable of type type.
     */
    variable getFreshVariable(VariableType type = VT_REAL);
    /**
     * Get the name recorded for this variable. If no special name was set,
     * we use "x_id".
     * @param v
     * @return 
     */
    std::string getVariableName(variable v);
    /**
     * Add a name for a given variable.
     * @param v
     * @param name
     */
    void setVariableName(variable v, const std::string& name);
   
    /**
     * Remove all information.
     */
    void clear();
};

}