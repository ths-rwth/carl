
# Find Python and Virtualenv. We don't actually use the output of the
# find_package, but it'll give nicer errors.
find_package(PythonInterp 3 REQUIRED)
find_program(VIRTUALENV virtualenv)
if(NOT VIRTUALENV)
    message(FATAL_ERROR "Could not find `virtualenv` in PATH")
endif()

ExternalProject_Add(
    pycarl
    GIT_REPOSITORY https://github.com/moves-rwth/pycarl.git
    BUILD_IN_SOURCE YES
    CONFIGURE_COMMAND ${VIRTUALENV} -p python3 pycarl-venv
    BUILD_COMMAND ./pycarl-venv/bin/python setup.py build_ext --carl-dir ${CMAKE_BINARY_DIR} -j 1 develop
    INSTALL_COMMAND ""
    TEST_COMMAND ./pycarl-venv/bin/pip install pytest
)

ExternalProject_Get_Property(pycarl SOURCE_DIR)

add_dependencies(pycarl lib_carl carl-parser)
add_dependencies(addons pycarl)

add_test(NAME pycarl
    COMMAND ./pycarl-venv/bin/python -m pytest tests
    WORKING_DIRECTORY ${SOURCE_DIR}
)
