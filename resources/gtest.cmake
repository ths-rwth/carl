set(gtest_VERSION "1.7.0")

ExternalProject_Add(
    googletest
    URL "https://github.com/google/googletest/archive/release-${gtest_VERSION}.zip"
    INSTALL_COMMAND ""
    LOG_DOWNLOAD ON
    LOG_CONFIGURE ON
    LOG_BUILD ON
)

ExternalProject_Get_Property(googletest source_dir)
set(GTEST_INCLUDE_DIR "${source_dir}/include")

ExternalProject_Get_Property(googletest binary_dir)
set(GTEST_LIBRARIES "${binary_dir}/${CMAKE_FIND_LIBRARY_PREFIXES}gtest.a ${binary_dir}/${CMAKE_FIND_LIBRARY_PREFIXES}gtest_main.a")

add_dependencies(resources googletest)
