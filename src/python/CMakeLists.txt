# This is a CMake example for Python

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src ${pycarl_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${pybind11_incdir})

ADD_LIBRARY(pycarlcore SHARED pycarl.cpp)
ADD_LIBRARY(pycarlnumbers SHARED pycarl_numbers.cpp)
ADD_LIBRARY(pycarlparsing SHARED pycarl_parsing.cpp parser.cpp)


SET(PYCARL_SOURCE_DIR "${PROJECT_SOURCE_DIR}")
SET(PYCARL_OUTPUT_DIR "${PROJECT_BINARY_DIR}/pycarl")

SET_TARGET_PROPERTIES(pycarlcore PROPERTIES
        OUTPUT_NAME _core
        PREFIX ""
        SUFFIX ".so"
        LIBRARY_OUTPUT_DIRECTORY ${PYCARL_OUTPUT_DIR}/core
)

SET_TARGET_PROPERTIES(pycarlnumbers PROPERTIES
        OUTPUT_NAME _numbers
        PREFIX ""
        SUFFIX ".so"
        LIBRARY_OUTPUT_DIRECTORY ${PYCARL_OUTPUT_DIR}/numbers
)

SET_TARGET_PROPERTIES(pycarlparsing PROPERTIES
        OUTPUT_NAME _parsing
        PREFIX ""
        SUFFIX ".so"
        LIBRARY_OUTPUT_DIRECTORY ${PYCARL_OUTPUT_DIR}/parsing
)

if(BUILD_STATIC)
    TARGET_LINK_LIBRARIES(pycarlcore lib_carl_static  ${pycarl_LIBRARIES_STATIC} ${PYTHON_LIBRARIES} ${libs_static} )
    TARGET_LINK_LIBRARIES(pycarlnumbers lib_carl_static ${pycarl_LIBRARIES_STATIC} ${PYTHON_LIBRARIES} ${libs_static} )
    TARGET_LINK_LIBRARIES(pycarlparsing lib_carl_static ${pycarl_LIBRARIES_STATIC} ${PYTHON_LIBRARIES} ${libs_static} )
else()
    TARGET_LINK_LIBRARIES(pycarlcore lib_carl ${pycarl_LIBRARIES_DYNAMIC} ${PYTHON_LIBRARIES} ${libs_dynamic})
    TARGET_LINK_LIBRARIES(pycarlnumbers lib_carl ${pycarl_LIBRARIES_DYNAMIC} ${PYTHON_LIBRARIES} ${libs_dynamic})
    TARGET_LINK_LIBRARIES(pycarlparsing lib_carl ${pycarl_LIBRARIES_DYNAMIC} ${PYTHON_LIBRARIES} ${libs_dynamic})
endif()
#
add_custom_target(pycarl DEPENDS pycarlcore pycarlnumbers pycarlparsing)

