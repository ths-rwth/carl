
ExternalProject_Add(
    pycarl
    GIT_REPOSITORY https://github.com/moves-rwth/pycarl.git
    BUILD_IN_SOURCE YES
    CONFIGURE_COMMAND ""
    BUILD_COMMAND . pycarl-env/bin/activate && python setup.py build_ext -j 1 develop
    INSTALL_COMMAND ""
)

ExternalProject_Get_Property(pycarl SOURCE_DIR)

add_custom_command(
    OUTPUT ${SOURCE_DIR}/pycarl-env/bin/activate
    COMMAND virtualenv -p python3 pycarl-env
    WORKING_DIRECTORY ${SOURCE_DIR}
)

add_dependencies(pycarl carl-parser)

add_test(NAME pycarl
    COMMAND . pycarl-env/bin/activate && py.test tests/
    WORKING_DIRECTORY ${SOURCE_DIR}
)
