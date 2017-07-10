add_custom_target(addons)

add_dependencies(addons lib_carl)

include(resources/addons/carl-parser.cmake)
