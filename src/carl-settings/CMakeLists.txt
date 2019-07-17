include(${CMAKE_SOURCE_DIR}/cmake/carlmacros.cmake)

add_definitions(-fPIC)

file(GLOB_RECURSE carl-settings_sources "*.cpp")

add_new_libraries(carl-settings "${PROJECT_VERSION_LIB}" "${carl-settings_sources}" "Boost::program_options::shared")
target_include_dirs_from(carl-settings-objects SYSTEM PUBLIC Boost::program_options::shared)
target_link_libraries(carl-settings-shared Boost::program_options::shared ${LIB_FILESYSTEM})
target_link_libraries(carl-settings-static Boost::program_options::static ${LIB_FILESYSTEM})

install_libraries(carl carl-settings-shared carl-settings-static)
