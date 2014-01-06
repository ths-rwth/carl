/**
 * @file VariablePool.cpp
 * @author Sebastian Junges
 */

#include "VariablePool.h"

#include <mutex>

#include "initialize.h"
#include "config.h"
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
//    std::call_once( VariablePool::only_one, 
//        [] ()
//        {
            VariablePool::instance.reset( new VariablePool() );
            LOGMSG_INFO("carl","VariablePool created.");
//        });
    return *VariablePool::instance;
}

Variable VariablePool::getFreshVariable(VariableType type)
{
    LOGMSG_DEBUG("carl.varpool", "New variable of type " << type << " with id " << mNextVarId + 1);
    return Variable(mNextVarId++, type);
}

const std::string VariablePool::getVariableName(Variable::Arg v, bool friendly) const
{
    if( v == Variable::NO_VARIABLE ) return "NO_VARIABLE";
    if( friendly )
    {
        std::map<Variable, std::string>::const_iterator it = mFriendlyNames.find(v);
        if(it == mFriendlyNames.end())
        {
            return "x_" + std::to_string(v.getId());
        }
        else
        {
            return it->second;
        }
    }
    else
    {
        return "x_" + std::to_string(v.getId());
    }
}

void VariablePool::setVariableName(Variable::Arg v, const std::string& name) 
{
    #ifdef CARL_USE_FRIENDLY_VARNAMES
    mFriendlyNames[v] = name;
    #endif
}





}

