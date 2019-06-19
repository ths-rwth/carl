include(${CMAKE_SOURCE_DIR}/cmake/carlmacros.cmake)

add_definitions(-fPIC)

file(GLOB_RECURSE carl-model_sources "*.cpp")

add_new_libraries(carl-model "${PROJECT_VERSION_LIB}" "${carl-model_sources}" "carl-shared")
target_include_dirs_from(carl-model-objects "" PUBLIC carl-shared)
target_link_libraries(carl-model-shared carl-shared)
target_link_libraries(carl-model-static carl-static)

install_libraries(carl carl-model-shared carl-model-static)
