/**
 * @file VariablePool.cpp
 * @author Sebastian Junges
 */

#include "VariablePool.h"

#include <mutex>

#include "logging.h"

namespace carl
{
std::shared_ptr< VariablePool > VariablePool::instance;
std::once_flag VariablePool::only_one;

VariablePool::VariablePool():
mNextVarId (1 << Variable::VARIABLE_BITS_RESERVED_FOR_TYPE)
{
    LOGMSG_INFO("carl.varpool", "Constructor called");
}
    
VariablePool& VariablePool::operator= (const VariablePool& rs) 
{
    if (this != &rs) {
        instance  = rs.instance;
    }
    return *this;
}   
VariablePool& VariablePool::getInstance(  )
{
    std::call_once( VariablePool::only_one, 
        [] ()
        {
            VariablePool::instance.reset( new VariablePool() );
            LOGMSG_INFO("carl","VariablePool created.");
        });
    return *VariablePool::instance;
}

Variable VariablePool::getFreshVariable(VariableType type)
{
    LOGMSG_TRACE("carl.varpool", "New variable of type" << type);
    return Variable(mNextVarId++, type);
}

}

