#include <iostream>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "../lib/core/initialize.h"
#include "../lib/core/logging.h"

int main( int argc, char** argv )
{
    // Get the top level suite from the registry
    CppUnit::Test*              test_all = CppUnit::TestFactoryRegistry::getRegistry().makeTest();
    CppUnit::TextUi::TestRunner runner;
    runner.addTest( test_all );

    // Change the default outputter to a compiler error format outputter
    runner.setOutputter( new CppUnit::CompilerOutputter( &runner.result(), std::cerr ) );

    LOGMSG_DEBUG(getLog("arithmetic.tests"), "Logging up and running");
    
    // Run the tests. Return 1 if a test didnt pass.
    return !runner.run();

}
