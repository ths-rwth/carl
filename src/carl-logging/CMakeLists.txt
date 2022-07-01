include(${CMAKE_SOURCE_DIR}/cmake/carlmacros.cmake)

add_definitions(-fPIC)

# Configure all .in files
configure_everything()

file(GLOB_RECURSE carl-logging_sources "*.cpp")

add_new_libraries(carl-logging "${PROJECT_VERSION_LIB}" "${carl-logging_sources}" "")
target_include_dirs_from(carl-logging-objects "" PUBLIC carl-common-shared)
target_link_libraries(carl-logging-shared carl-common-shared)
target_link_libraries(carl-logging-static carl-common-static)

install_libraries(carl carl-logging-shared carl-logging-static)
