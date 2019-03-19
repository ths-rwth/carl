include(${CMAKE_SOURCE_DIR}/cmake/carlmacros.cmake)

add_definitions(-fPIC)

file(GLOB_RECURSE carl-covering_sources "*.cpp")

add_new_libraries(carl-covering "${PROJECT_VERSION_LIB}" "${carl-covering_sources}" "carl-shared")
target_include_dirs_from(carl-covering-objects "" PUBLIC carl-shared)
target_link_libraries(carl-covering-shared carl-shared)
target_link_libraries(carl-covering-static carl-static)

install_libraries(carl carl-covering-shared carl-covering-static)
