include(${CMAKE_SOURCE_DIR}/cmake/carlmacros.cmake)

add_definitions(-fPIC)

#add_subdirectory(datastructures)
# Configure all .in files
configure_everything()

file(GLOB_RECURSE carl-common_sources "*.cpp")
add_new_libraries(carl-common "${PROJECT_VERSION_LIB}" "${carl-common_sources}" "resources")

set(BOOST_LIBS_SHARED "Boost_system_SHARED")
set(BOOST_LIBS_STATIC "Boost_system_STATIC")
target_include_dirs_from(carl-common-objects SYSTEM PUBLIC ${BOOST_LIBS_SHARED} ${CMAKE_DL_LIBS} pthread)
target_link_libraries(carl-common-shared PUBLIC ${BOOST_LIBS_SHARED} ${CMAKE_DL_LIBS} ${LIB_FILESYSTEM} pthread)
target_link_libraries(carl-common-static PUBLIC ${BOOST_LIBS_STATIC} ${CMAKE_DL_LIBS} ${LIB_FILESYSTEM} pthread)

install_libraries(carl carl-common-shared carl-common-static)