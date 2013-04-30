# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#                                                             #
# File: src/CMakeLists.txt                                    #
#                                                             #
# Project name: SMT-RAT                                       #
# Contact: Florian Corzilius <corzilius@cs.rwth-aachen.de>,   #
#          Ulrich Loup <loup@cs.rwth-aachen.de>               #
# Project sourceforge page: http://smtrat.sourceforge.net/    #
# Development status: beta                                    #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# Version: 2012-07-30                                         #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

include_directories( ${PROJECT_SOURCE_DIR}/src/lib )
link_directories( ${PROJECT_BINARY_DIR} )


add_executable( check_arithmetic test.cpp 
	UnitTest_Monomial.cpp	
	UnitTest_VariablePool.cpp
)

# executable target
add_custom_target( check
                   COMMAND check_arithmetic
                   DEPENDS check_arithmetic
                   COMMENT "${${PROJECT_NAME}_NAME} ${${PROJECT_NAME}_VERSION} CPPUnit Test Suite"
)

# link the libraries
target_link_libraries( check_arithmetic
                       ${CPPUNIT_LIBRARIES}
                       lib_arithmetic
                       ${libraries}
)

set_target_properties( check_arithmetic PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} )
