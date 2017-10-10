
ExternalProject_Add(
    pycarl
    GIT_REPOSITORY https://github.com/moves-rwth/pycarl.git
    BUILD_IN_SOURCE YES
    CONFIGURE_COMMAND ""
    BUILD_COMMAND . pycarl-venv/bin/activate && python setup.py build_ext -j 1 develop
    INSTALL_COMMAND ""
)

ExternalProject_Get_Property(pycarl SOURCE_DIR)

ExternalProject_Add_Step(pycarl create-venv
    COMMAND virtualenv -p python3 pycarl-venv
    WORKING_DIRECTORY ${SOURCE_DIR}
    DEPENDERS build
    DEPENDEES download
)

add_dependencies(pycarl lib_carl carl-parser)
add_dependencies(addons pycarl)

add_test(NAME pycarl
    COMMAND . pycarl-env/bin/activate && py.test tests/
    WORKING_DIRECTORY ${SOURCE_DIR}
)
