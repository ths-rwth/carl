include(${CMAKE_SOURCE_DIR}/cmake/carlmacros.cmake)

add_definitions(-fPIC)

file(GLOB_RECURSE carl-extpolys_sources "*.cpp")

add_new_libraries(carl-extpolys "${PROJECT_VERSION_LIB}" "${carl-extpolys_sources}" "carl-shared")
target_include_dirs_from(carl-extpolys-objects "" PUBLIC carl-shared)
target_link_libraries(carl-extpolys-shared carl-shared)
target_link_libraries(carl-extpolys-static carl-static)

install_libraries(carl carl-extpolys-shared carl-extpolys-static)
