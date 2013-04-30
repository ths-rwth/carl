#pragma once
#include <cppunit/extensions/HelperMacros.h>

#include "../lib/core/VariablePool.h"


class UnitTest_VariablePool : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( UnitTest_VariablePool );
    CPPUNIT_TEST( testMethod );
    CPPUNIT_TEST( testFailedMethod );
    CPPUNIT_TEST_SUITE_END( );

public:
    UnitTest_VariablePool( );
    virtual ~UnitTest_VariablePool( );
    void setUp( );
    void tearDown( );

private:
    
    void testMethod( );
    void testFailedMethod( );
};

