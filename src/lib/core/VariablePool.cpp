/**
 * @file VariablePool.h 
 * @author Sebastian Junges
 */

#include "VariablePool.h"

#include <mutex>
#include <climits>

#include "logging.h"

namespace arithmetic
{

std::shared_ptr< VariablePool > VariablePool::instance;
std::once_flag VariablePool::only_one;


VariablePool::VariablePool()
{
    mNextVarId = 0;
    LOGMSG_INFO("arithmetic.varpool", "Constructor called");
}
VariablePool::VariablePool(const VariablePool& rs) {
    instance  = rs.instance;
}
    
VariablePool& VariablePool::operator = (const VariablePool& rs) 
{
    if (this != &rs) {
        instance  = rs.instance;
    }
    return *this;
}   


VariablePool& VariablePool::getInstance(  )
{
    std::call_once( VariablePool::only_one, [] ()
        {
            VariablePool::instance.reset( new VariablePool() );
            LOGMSG_INFO("arithmetic","VariablePool created.");
        });
    return *VariablePool::instance;
}

variable VariablePool::getFreshVariable(VariableType type)
{
    size_t t(type);
    return (t << ((sizeof(variable) * CHAR_BIT) - VARIABLE_BITS_RESERVED_FOR_TYPE) | mNextVarId++);
}

}

