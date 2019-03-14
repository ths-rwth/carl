include(${CMAKE_SOURCE_DIR}/cmake/carlmacros.cmake)

add_definitions(-fPIC)

file(GLOB_RECURSE carl-cad_sources "*.cpp")

add_new_libraries(carl-cad "${PROJECT_VERSION_LIB}" "${carl-cad_sources}" "carl")
target_include_dirs_from(carl-cad-objects "" PUBLIC carl-shared)
target_link_libraries(carl-cad-shared carl-shared)
target_link_libraries(carl-cad-static carl-static)

install_libraries(carl carl-cad-shared carl-cad-static)
