# This is a CMake example for Python

if(PYTHON3)
    set(PYTHON_VERSION_STRING 3)
    set(BOOST_PYTHON_COMPONENT python)
    set(BOOST_PYTHON_LIB boost_python3)
else()
    set(PYTHON_VERSION_STRING 2.7)
    set(BOOST_PYTHON_COMPONENT python)
    set(BOOST_PYTHON_LIB boost_python)
endif()

FIND_PACKAGE(PythonLibs ${PYTHON_VERSION_STRING})
FIND_PACKAGE(Boost COMPONENTS ${BOOST_PYTHON_COMPONENT})

ADD_LIBRARY(pycarl SHARED pycarl.cpp pycarl_parser.cpp)
include_directories(${PROJECT_SOURCE_DIR}/src ${include_dirs}  ${PYTHON_INCLUDE_DIR} )

set_target_properties( pycarl PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

if(BUILD_STATIC)
   TARGET_LINK_LIBRARIES(pycarl lib_carl_static ${BOOST_PYTHON_LIB} ${libs_static})
else()
    TARGET_LINK_LIBRARIES(pycarl lib_carl ${BOOST_PYTHON_LIB} ${libs_dynamic})
endif()
