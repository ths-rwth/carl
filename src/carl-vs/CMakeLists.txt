include(${CMAKE_SOURCE_DIR}/cmake/carlmacros.cmake)

add_definitions(-fPIC)

file(GLOB_RECURSE carl-vs_sources "*.cpp")

add_new_libraries(carl-vs "${PROJECT_VERSION_LIB}" "${carl-vs_sources}" "carl-arith-shared")
target_include_dirs_from(carl-vs-objects "" PUBLIC carl-arith-shared)
target_link_libraries(carl-vs-shared carl-arith-shared)
target_link_libraries(carl-vs-static carl-arith-static)

install_libraries(carl carl-vs-shared carl-vs-static)
