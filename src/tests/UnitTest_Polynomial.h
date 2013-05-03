#pragma once
#include <cppunit/extensions/HelperMacros.h>
#include "../lib/core/Monomial.h"


class UnitTest_Monomial : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( UnitTest_Monomial );
    CPPUNIT_TEST( testMethod );
    CPPUNIT_TEST( testFailedMethod );
    CPPUNIT_TEST_SUITE_END( );

public:
    UnitTest_Monomial( );
    virtual ~UnitTest_Monomial( );
    void setUp( );
    void tearDown( );

private:
    void testMethod( );
    void testFailedMethod( );
};

