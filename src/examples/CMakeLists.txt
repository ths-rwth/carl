
configure_everything()

add_custom_target(examples)

file(GLOB_RECURSE example_sources LIST_DIRECTORIES false RELATIVE ${CMAKE_SOURCE_DIR}/src/examples/ "*.cpp")

foreach(src ${example_sources})
	string(REGEX REPLACE ".cpp$" "" name ${src})
	string(REGEX REPLACE "/" "-" name "example-${name}")
	add_executable(${name} ${src})
	target_link_libraries(${name} carl-shared)
	add_dependencies(examples ${name})
endforeach()
