
ExternalProject_Add(
    pycarl
    GIT_REPOSITORY https://github.com/moves-rwth/pycarl.git
    BUILD_IN_SOURCE YES
    CONFIGURE_COMMAND virtualenv -p python3 pycarl-env
    BUILD_COMMAND . pycarl-env/bin/activate && python setup.py build_ext -j 1 develop
    INSTALL_COMMAND ""
)

add_dependencies(pycarl carl-parser)

ExternalProject_Get_Property(pycarl SOURCE_DIR)

add_test(NAME pycarl
    COMMAND py.test tests/
    WORKING_DIRECTORY ${SOURCE_DIR}
)