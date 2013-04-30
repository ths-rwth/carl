/* 
 * File:   UnitTest_VariablePool.cpp
 * Author: Sebastian Junges
 * 
 * Created on April 23, 2013, 4:20 PM
 */

#include "UnitTest_VariablePool.h"

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

void UnitTest_VariablePool::testMethod( )
{
    arithmetic::VariablePool& vp(arithmetic::VariablePool::getInstance());
}

void UnitTest_VariablePool::testFailedMethod( )
{
    
}
