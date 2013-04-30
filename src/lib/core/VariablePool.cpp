/**
 * @file VariablePool.h 
 * @author Sebastian Junges
 */

#include "VariablePool.h"

#include <mutex>

#include "logging.h"
#include <iostream>

namespace arithmetic
{

VariablePool::VariablePool()
{
    std::cout << "output" << std::endl;
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

std::shared_ptr< VariablePool > VariablePool::instance;
    /**
     * Making sure that creating a new instance is called only once.
     * Note: According to the new C++11 standard this is not strictly necessary.
     * However, this is currently only supported in gcc 4.8.1 and newer. Therefore,
     * we decided to make explicit use of this flag, thereby supporting gcc 4.7 
     * and some other compilers.
     */
std::once_flag VariablePool::only_one;
    

}

