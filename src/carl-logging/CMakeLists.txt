include(${CMAKE_SOURCE_DIR}/cmake/carlmacros.cmake)

add_definitions(-fPIC)

# Configure all .in files
configure_everything()

file(GLOB_RECURSE carl-logging_sources "*.cpp")

add_new_libraries(carl-logging "${PROJECT_VERSION_LIB}" "${carl-logging_sources}" "")

install_libraries(carl carl-logging-shared carl-logging-static)
