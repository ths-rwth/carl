# This is a CMake example for Python

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src ${include_dirs} ${PYTHON_INCLUDE_DIRS})

ADD_LIBRARY(pycarl SHARED pycarl.cpp)
#pycarl_parser.cpp)

SET_TARGET_PROPERTIES(pycarl PROPERTIES
        PREFIX ""
        SUFFIX ".so"
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}

)

if(BUILD_STATIC)
   TARGET_LINK_LIBRARIES(pycarl lib_carl_static ${BOOST_PYTHON_LIB} ${PYTHON_LIBRARIES} ${libs_static} )
else()
    TARGET_LINK_LIBRARIES(pycarl lib_carl ${BOOST_PYTHON_LIB} ${PYTHON_LIBRARIES} ${libs_dynamic})
endif()
