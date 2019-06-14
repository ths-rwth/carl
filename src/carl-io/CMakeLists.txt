include(${CMAKE_SOURCE_DIR}/cmake/carlmacros.cmake)

add_definitions(-fPIC)

file(GLOB_RECURSE carl-io_sources "*.cpp")

add_new_libraries(carl-io "${PROJECT_VERSION_LIB}" "${carl-io_sources}" "carl-shared;carl-model-shared")
target_include_dirs_from(carl-io-objects "" PUBLIC carl-shared carl-model-shared)
target_link_libraries(carl-io-shared carl-shared carl-model-shared)
target_link_libraries(carl-io-static carl-static carl-model-static)

install_libraries(carl carl-io-shared carl-io-static)
