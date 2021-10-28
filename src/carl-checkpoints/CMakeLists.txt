include(${CMAKE_SOURCE_DIR}/cmake/carlmacros.cmake)

add_definitions(-fPIC)

# Configure all .in files
configure_everything()

file(GLOB_RECURSE carl-checkpoints_sources "*.cpp")

add_new_libraries(carl-checkpoints "${PROJECT_VERSION_LIB}" "${carl-checkpoints_sources}" "")

install_libraries(carl carl-checkpoints-shared carl-checkpoints-static)
