/* 
 * File:   UnitTest_VariablePool.cpp
 * Author: Sebastian Junges
 * 
 * Created on April 23, 2013, 4:20 PM
 */

#include "UnitTest_VariablePool.h"
#include "../lib/core/Variable.h"

CPPUNIT_TEST_SUITE_REGISTRATION( UnitTest_VariablePool );

UnitTest_VariablePool::UnitTest_VariablePool( )
{
}

UnitTest_VariablePool::~UnitTest_VariablePool( )
{
}

void UnitTest_VariablePool::setUp( )
{
    
}

void UnitTest_VariablePool::tearDown( )
{
   
}

void UnitTest_VariablePool::testVariablePoolCreation( )
{
    arithmetic::VariablePool& vp(arithmetic::VariablePool::getInstance());
}

void UnitTest_VariablePool::testFreshVariablesAndTypes( )
{
    using namespace arithmetic;
    VariablePool& vp(VariablePool::getInstance());
    std::cout << getVarId(vp.getFreshVariable()) << std::endl;
    std::cout << vp.getFreshVariable() << std::endl;
    std::cout << getVarId(vp.getFreshVariable(VT_INT)) << std::endl;;;
    std::cout << getVarType(vp.getFreshVariable(VT_INT)) << std::endl;;;
}
