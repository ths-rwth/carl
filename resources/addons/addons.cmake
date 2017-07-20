add_custom_target(addons)

add_dependencies(addons lib_carl)

set_directory_properties(PROPERTIES EP_PREFIX ${CMAKE_BINARY_DIR}/addons)

include(resources/addons/carl-parser.cmake)
include(resources/addons/pycarl.cmake)
