include(${CMAKE_SOURCE_DIR}/cmake/carlmacros.cmake)

add_definitions(-fPIC)

# Configure all .in files
configure_everything()

file(GLOB_RECURSE carl-statistics_sources "*.cpp")

add_new_libraries(carl-statistics "${PROJECT_VERSION_LIB}" "${carl-statistics_sources}" "")
target_include_dirs_from(carl-statistics-objects "" PUBLIC carl-common-shared)
target_link_libraries(carl-statistics-shared carl-common-shared)
target_link_libraries(carl-statistics-static carl-common-static)

install_libraries(carl carl-statistics-shared carl-statistics-static)
