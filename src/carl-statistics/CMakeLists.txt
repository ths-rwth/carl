include(${CMAKE_SOURCE_DIR}/cmake/carlmacros.cmake)

add_definitions(-fPIC)

# Configure all .in files
configure_everything()

file(GLOB_RECURSE carl-statistics_sources "*.cpp")

add_new_libraries(carl-statistics "${PROJECT_VERSION_LIB}" "${carl-statistics_sources}" "")

install_libraries(carl carl-statistics-shared carl-statistics-static)
