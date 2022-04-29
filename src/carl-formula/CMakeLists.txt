include(${CMAKE_SOURCE_DIR}/cmake/carlmacros.cmake)

add_definitions(-fPIC)

configure_everything()

file(GLOB_RECURSE carl-formula_sources "*.cpp")

add_new_libraries(carl-formula "${PROJECT_VERSION_LIB}" "${carl-formula_sources}" "carl-shared")
target_include_dirs_from(carl-formula-objects "" PUBLIC carl-shared)
target_link_libraries(carl-formula-shared carl-shared)
target_link_libraries(carl-formula-static carl-static)

install_libraries(carl carl-formula-shared carl-formula-static)
