/**
 * @file VariablePool.h 
 * @author Sebastian Junges
 */

#include "VariablePool.h"

#include <mutex>

#include "logging.h"

namespace arithmetic
{

VariablePool::VariablePool()
{
    
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

}
